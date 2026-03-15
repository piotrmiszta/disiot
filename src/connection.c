/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#include "connection.h"
#include "client.h"
#include "err_codes.h"
#include "logger.h"
#include "message.h"
#include "server.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static pthread_t server_thread;
static pthread_t client_thread;

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

    while (1)
    {
        if (connect(conn->fd, (struct sockaddr*)&conn->addr,
                    sizeof(conn->addr)) < 0)
        {
            LOG_ERROR("Cannot connect to server! %s\n", strerror(errno));
            sleep(1);
        }
        else
        {
            LOG_TRACE("Connected to server: %s:%d\n",
                      inet_ntoa(conn->addr.sin_addr),
                      ntohs(conn->addr.sin_port));
            break;
        }
    }
    pthread_create(&client_thread, NULL, client_routine, conn);
    return IOT_SUCCESS;
}
