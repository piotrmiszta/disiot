/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */
#include "client.h"
#include "connection.h"
#include "err_codes.h"
#include "logger.h"
#include "message.h"
#include "unistd.h"
#include <errno.h>
#include <string.h>

void* client_routine(void* arg)
{
    conn_t* conn = arg;
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);
    if (getsockname(conn->fd, (struct sockaddr*)&local_addr, &addr_len) < 0)
    {
        LOG_ERROR("getsockname failed: %s\n", strerror(errno));
        return NULL;
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
            return NULL;
        }
        LOG_TRACE("Sent PING message to server\n");

        char recv_buffer[1024] = {0};
        ssize_t bytes_read = read(conn->fd, recv_buffer, sizeof(recv_buffer));
        if (bytes_read < 0)
        {
            LOG_ERROR("Error reading from server: %s\n", strerror(errno));
            return NULL;
        }
        else if (bytes_read == 0)
        {
            LOG_INFO("Server disconnected\n");
            return NULL;
        }

        LOG_TRACE("Received pong message elapsed time: %lu microseconds\n",
                  ((message_ping_payload_t*)(recv_buffer + MESSAGE_HEADER_SIZE))
                          ->entries[0]
                          .time_stamp -
                      first_time_stamp);
        sleep(1);
    }
}
