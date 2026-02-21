#ifndef CONNECTION_H_
#define CONNECTION_H_
#include <netinet/in.h>

/* TODO: this will be platform specific, now interesting is only linux tcp
 * connection */

typedef struct
{
    int fd;
    struct sockaddr_in addr;
    int addr_len;
} conn_t;

typedef struct
{
    int port;
    const char* host;
} conn_param_t;

int conn_start_server(conn_t* conn, conn_param_t params);

int conn_connect(conn_t* conn, conn_param_t params);

#endif
