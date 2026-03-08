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
#include <sys/time.h>
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

        memset(buff, 0, 1024);

        char buffer[MESSAGE_HEADER_SIZE + MESSAGE_PING_PAYLOAD_SIZE +
                    TRACEROUTE_ENTRY_SIZE] = {0};

        assign_ping_payload(
            (message_t*)buffer,
            (message_ping_payload_t*)(buffer + MESSAGE_HEADER_SIZE), conn,
            conn->addr);

        if (write(fd, buffer, sizeof(buffer)) < 0)
        {
            LOG_ERROR("Cannot send message to client %s:%d: %s",
                      inet_ntoa(conn->addr.sin_addr),
                      ntohs(conn->addr.sin_port), strerror(errno));
            conn->err_code = IOT_ECONN;
            return &conn->err_code;
        }
        LOG_TRACE("Sent pong message to client %s:%d\n",
                  inet_ntoa(conn->addr.sin_addr), ntohs(conn->addr.sin_port));
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
        LOG_FATAL("Cannot bind to port %d: %s\n", params->port,
                  strerror(errno));
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
    conn->addr.sin_addr.s_addr = inet_addr(params->host);
    conn->addr.sin_port = htons((uint16_t)params->port);

    if (connect(conn->fd, (struct sockaddr*)&conn->addr, sizeof(conn->addr)) <
        0)
    {
        LOG_ERROR("Cannot connect to server! %s\n", strerror(errno));
        return IOT_ECONN;
    }
    LOG_TRACE("Connected to server: %s:%d\n", inet_ntoa(conn->addr.sin_addr),
              ntohs(conn->addr.sin_port));

    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(conn->fd, (struct sockaddr*)&local_addr, &addr_len) < 0)
    {
        LOG_ERROR("getsockname failed: %s\n", strerror(errno));
        return IOT_ESYS;
    }

    while (1)
    {
        /* Currently send only PING message */
        char buffer[MESSAGE_HEADER_SIZE + MESSAGE_PING_PAYLOAD_SIZE +
                    TRACEROUTE_ENTRY_SIZE] = {0};

        uint64_t first_time_stamp = assign_ping_payload(
            (message_t*)buffer,
            (message_ping_payload_t*)(buffer + MESSAGE_HEADER_SIZE), conn,
            local_addr);
        if (write(conn->fd, buffer, sizeof(buffer)) < 0)
        {
            LOG_ERROR("Cannot send message to server! %s\n", strerror(errno));
            return IOT_ECONN;
        }
        LOG_TRACE("Sent PING message to server\n");

        char recv_buffer[1024] = {0};
        ssize_t bytes_read = read(conn->fd, recv_buffer, sizeof(recv_buffer));
        if (bytes_read < 0)
        {
            LOG_ERROR("Error reading from server: %s\n", strerror(errno));
            return IOT_ECONN;
        }
        else if (bytes_read == 0)
        {
            LOG_INFO("Server disconnected\n");
            return IOT_ECONN;
        }

        LOG_TRACE("Received pong message elapsed time: %lu microseconds\n",
                  ((message_ping_payload_t*)(recv_buffer + MESSAGE_HEADER_SIZE))
                          ->entries[0]
                          .time_stamp -
                      first_time_stamp);
        sleep(1);
    }
}
