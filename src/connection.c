/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#include "connection.h"

#include "err_codes.h"
#include "logger.h"
#include "message.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static pthread_t server_thread;

static void* server_routine(void* arg);

static void* server_routine(void* arg)
{
    conn_t* conn = arg;
    socklen_t addrlen = sizeof(conn->addr);
    int fd = accept(conn->fd, (struct sockaddr*)&conn->addr, &addrlen);
    if (fd < 0)
    {
        LOG_ERROR("Cannot accept client: %s\n", strerror(errno));
        conn->err_code = IOT_ECONN;
        return &conn->err_code;
    }

    LOG_TRACE("Accepted client: %s:%d\n", inet_ntoa(conn->addr.sin_addr),
              ntohs(conn->addr.sin_port));

    char buff[1024] = {0};

    while (1)
    {
        ssize_t bytes_read = read(fd, buff, sizeof(buff));
        if (bytes_read < 0)
        {
            LOG_ERROR("Error reading from client %s:%d: %s",
                      inet_ntoa(conn->addr.sin_addr),
                      ntohs(conn->addr.sin_port), strerror(errno));

            conn->err_code = IOT_ECONN;
            return &conn->err_code;
        }
        else if (bytes_read == 0)
        {
            LOG_INFO("Client %s:%d disconnected\n",
                     inet_ntoa(conn->addr.sin_addr),
                     ntohs(conn->addr.sin_port));
            conn->err_code = IOT_ECONN;
            return &conn->err_code;
        }

        LOG_INFO("Received message from %s:%d: %s",
                 inet_ntoa(conn->addr.sin_addr), ntohs(conn->addr.sin_port),
                 buff);
        message_t* message = (message_t*)buff;
        message_ping_payload_t* payload =
            (message_ping_payload_t*)message->payload;
        LOG_DEBUG("Message details: magic=0x%X, version=%d, type=%d, seq=%d, "
                  "dest=%s:%d, src=%s:%d, payload_size=%d, flags=0x%X, "
                  "crc=0x%X PAYLOAD: entries=%d, time_stamp=%lu\n",
                  message->magic_number, message->version,
                  message->message_type, message->sequence_number,
                  inet_ntoa(*(struct in_addr*)&message->dest),
                  ntohs(*(uint16_t*)&message->dest + 4),
                  inet_ntoa(*(struct in_addr*)&message->src),
                  ntohs(*(uint16_t*)&message->src + 4), message->payload_size,
                  message->flags, message->crc, payload->number_of_entries,
                  payload->entries[0].time_stamp);
        fflush(stdout);
        memset(buff, 0, 1024);
    }
    return NULL;
}

iot_err_code_t conn_start_server(conn_t conn[static 1],
                                 const conn_param_t params[static 1])
{
    conn->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->fd <= 0)
    {
        LOG_FATAL("Cannot create socket! Aborted ()\n"); // TODO: add more info
                                                         // about error
        return IOT_ESOCKET;
    }
    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port = htons((uint16_t)params->port);
    conn->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    if (setsockopt(conn->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        LOG_FATAL("Cannot set socket options: %s", strerror(errno));
        return IOT_ESOCKET;
    }

    if (bind(conn->fd, (struct sockaddr*)&conn->addr, sizeof(conn->addr)) < 0)
    {
        LOG_FATAL("Cannot bind to port %d: %s", params->port, strerror(errno));
        return IOT_EBIND;
    }

    listen(conn->fd, 1);

    if (pthread_create(&server_thread, NULL, server_routine, conn) != 0)
    {
        LOG_FATAL("Cannot create server thread: %s", strerror(errno));
        return IOT_ESYS;
    }

    return IOT_SUCCESS;
}

iot_err_code_t conn_connect(conn_t conn[static 1],
                            const conn_param_t params[static 1])
{
    conn->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->fd <= 0)
    {
        LOG_FATAL("Cannot create socket for connection!\n"); // TODO: add more
                                                             // info about error
        return IOT_ESOCKET;
    }

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    conn->addr.sin_port = htons((uint16_t)params->port);

    if (connect(conn->fd, (struct sockaddr*)&conn->addr, sizeof(conn->addr)) <
        0)
    {
        LOG_ERROR("Cannot connect to server! %s\n", strerror(errno));
        return IOT_ECONN;
    }
    LOG_TRACE("Connected to server: %s:%d\n", inet_ntoa(conn->addr.sin_addr),
              ntohs(conn->addr.sin_port));
    while (1)
    {
        char buffer[sizeof(message_t) + sizeof(message_ping_payload_t) +
                    sizeof(traceroute_entry_t)] = {0};

        message_ping_payload_t* payload =
            (message_ping_payload_t*)(buffer + sizeof(message_t));
        traceroute_entry_t* entry = payload->entries;
        payload->number_of_entries = 1;
        entry->ip_addr = inet_addr("127.0.0.1");
        entry->hop_number = 1;
        entry->status = 0;
        entry->time_stamp = time(NULL);

        message_t* message = (message_t*)buffer;
        message->magic_number = 0xAB;
        message->version = 1;
        message->message_type = IOT_MESSAGE_PING;
        message->sequence_number = 0;
        message->dest = inet_addr("127.0.0.1");
        message->src = inet_addr("127.0.0.1");
        message->payload_size =
            sizeof(message_ping_payload_t) + sizeof(traceroute_entry_t);
        message->flags = 0;
        message->crc = 0; // TODO: calculate crc

        if (write(conn->fd, buffer, sizeof(buffer)) < 0)
        {
            LOG_ERROR("Cannot send message to server! %s\n", strerror(errno));
            return IOT_ECONN;
        }
        sleep(1);
    }
}
