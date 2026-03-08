/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#include "connection.h"
#include "err_codes.h"
#include "logger.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void* client_routine(void* arg)
{
    conn_t client = {0};
    conn_param_t* cparam = (conn_param_t*)arg;
    while (1)
    {
        if (conn_connect(&client, cparam) != IOT_SUCCESS)
        {
            LOG_ERROR("Cannot connect to server!\n");
            // return -1; // TODO: return correct error code
        }
        sleep(1); // TODO: get rid of sleep
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    printf("%lu %lu\n", sizeof(time_t), sizeof(int));
    fflush(stdout);
    if (argc >= 3)
    {
        int port = atoi(*(argv + 1));
        int cport = atoi(*(argv + 2));
        LOG_DEBUG("Server port: %d, client_port = %d\n", port, cport);

        conn_t conn;
        conn_param_t param = {.port = port, .host = "127.0.0.1"};
        conn_param_t cparam = {.port = cport, .host = "127.0.0.1"};
        if (conn_start_server(&conn, &param) != IOT_SUCCESS)
        {
            LOG_ERROR("Cannot start server!\n");
            return -1; // TODO: return correct error code
        }
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, client_routine, &cparam);

        pthread_join(client_thread, NULL);
        close(conn.fd);
    }
    else
    {
        LOG_ERROR("Wrong input parameters!\n");
    }
    return 0;
}
