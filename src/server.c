#include "server.h"
#include "connection.h"
#include "err_codes.h"
#include "logger.h"
#include "message.h"
#include "unistd.h"
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

void* server_routine(void* arg)
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
