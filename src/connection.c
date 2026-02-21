//
// Created by piotrmiszta on 2/20/26.
//

#include "connection.h"

#include "err_codes.h"
#include "logger.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
pthread_t server_thread;
pthread_t client_thread;

void* server_routine(void* arg)
{
    conn_t* conn = arg;
    socklen_t addrlen = sizeof(conn->addr);
    int fd = accept(conn->fd, (struct sockaddr*)&conn->addr, &addrlen);
    LOG_TRACE("Accepted client!\n");

    char buff[1024] = {};
    while (1)
    {
        read(fd, buff, sizeof(buff));
        printf("%s", buff);
        memset(buff, 0, 1024);
    }
    return NULL;
}

int conn_start_server(conn_t* conn, conn_param_t params)
{
    conn->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->fd <= 0)
    {
        LOG_FATAL("Cannot create socket! Aborted ()\n");
        abort();
    }
    conn->addr.sin_family = AF_INET;
    conn->addr.sin_port = htons(params.port);
    conn->addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(conn->fd, (struct sockaddr*)&conn->addr, sizeof(conn->addr)) < 0)
    {
        LOG_FATAL("Cannot bind to port %d", params.port);
        abort();
    }

    listen(conn->fd, 1);

    pthread_create(&server_thread, NULL, server_routine, conn);
    return IOT_SUCCESS;
}

int conn_connect(conn_t* conn, conn_param_t params)
{
    conn->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->fd <= 0)
    {
        LOG_FATAL("Cannot create socket for connection!\n");
        abort();
    }

    conn->addr.sin_family = AF_INET;
    conn->addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    conn->addr.sin_port = htons(params.port);

    if (connect(conn->fd, (struct sockaddr*)&conn->addr, sizeof(conn->addr)) <
        0)
    {
        LOG_ERROR("Cannot connect to server! %s\n", strerror(errno));
        return -1;
    }
    LOG_TRACE("Connected to server!\n");
    while (1)
    {
        write(conn->fd, "TEST\n", 6);
        sleep(1);
    }
}
