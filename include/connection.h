#ifndef CONNECTION_H_
#define CONNECTION_H_
#include <netinet/in.h>
#include <stdint.h>

#include "err_codes.h"
/* TODO: this will be platform specific, now interesting is only linux tcp
 * connection */

typedef struct
{
    int fd;
    int addr_len;
    struct sockaddr_in addr;
    iot_err_code_t err_code;
} conn_t;

typedef struct
{
    const char* host;
    int port;
    int _padded;
} conn_param_t;

iot_err_code_t conn_start_server(conn_t conn[static 1],
                                 const conn_param_t params[static 1]);

iot_err_code_t conn_connect(conn_t conn[static 1],
                            const conn_param_t params[static 1]);

#endif
