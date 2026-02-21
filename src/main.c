//
// Created by piotrmiszta on 2/20/26.
//

#include "connection.h"
#include "logger.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    assert(argc == 3);
    int port = atoi(argv[1]);
    int cport = atoi(argv[2]);
    LOG_DEBUG("Server port: %d, client_port = %d\n", port, cport);

    conn_t conn;
    conn_param_t param = {.port = port};
    conn_t client = {};
    conn_param_t cparam = {.port = cport};
    conn_start_server(&conn, param);

    while (1)
    {
        conn_connect(&client, cparam);
        sleep(1);
    }
}
