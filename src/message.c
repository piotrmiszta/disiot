/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#include "message.h"
#include "connection.h"
#include "err_codes.h"
#include "logger.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#define MESSAGE_MAX_LENGTH 1024

iot_err_code_t parse_message_header(const char* buffer, uint64_t buffer_size,
                                    message_t* message)
{
    if (buffer_size < sizeof(message_t))
    {
        // Handle error: buffer too small
        LOG_ERROR("Buffer size (%lu) is too small to contain a valid message "
                  "header\n",
                  buffer_size);
        return IOT_MESSAGE_TOO_SHORT;
    }

    memcpy(message, buffer, sizeof(message_t));
    return IOT_SUCCESS;
}

void message_dump(const message_t* message)
{
    switch ((MessageTypeE)message->message_type)
    {
    case IOT_MESSAGE_PING:
        message_ping_payload_t* payload =
            (message_ping_payload_t*)message->payload;
        LOG_TRACE(
            "PING MESSAGE: HEADER: magic=0x%X, version=%d, type=%d, seq=%d, "
            "dest=%s:%d, src=%s:%d, payload_size=%d, flags=0x%X, crc=0x%X, "
            "PAYLOAD: entries=%d, time_stamp=%lu\n",
            message->magic_number, message->version, message->message_type,
            message->sequence_number,
            inet_ntoa(*(struct in_addr*)&message->dest_addr),
            ntohs(message->dest_port),
            inet_ntoa(*(struct in_addr*)&message->src_addr),
            ntohs(message->src_port), message->payload_size, message->flags,
            message->crc, payload->number_of_entries,
            payload->entries[0].time_stamp);
        break;
    case IOT_MESSAGE_PONG:
    case IOT_MESSAGE_GET:
    case IOT_MESSAGE_POST:
    case IOT_MESSAGE_INFO:
    case IOT_MESSAGE_ACK:
        assert(0 && "Not implemented message dump for this type");
        break;
    default:
        assert(0 && "Not implemented message dump for this type");
        break;
    }
}

uint64_t assign_ping_payload(message_t* message,
                             message_ping_payload_t* payload, conn_t* conn,
                             struct sockaddr_in local_addr)
{
    struct timeval time;
    gettimeofday(&time, NULL);
    traceroute_entry_t* entry = payload->entries;
    payload->number_of_entries = 1;
    entry->ip_addr = inet_addr("127.0.0.1");
    entry->hop_number = 1;
    entry->status = 0;
    entry->time_stamp = time.tv_sec * 1000000 + time.tv_usec;

    message->magic_number = 0xAB;
    message->version = 1;
    message->message_type = IOT_MESSAGE_PING;
    message->sequence_number = 0;
    message->dest_addr = conn->addr.sin_addr.s_addr;
    message->dest_port = conn->addr.sin_port;
    message->src_addr = local_addr.sin_addr.s_addr;
    message->src_port = local_addr.sin_port;
    message->payload_size =
        sizeof(message_ping_payload_t) + sizeof(traceroute_entry_t);
    message->flags = 0;
    message->crc = 0; // TODO: calculate crc
    return entry->time_stamp;
}

iot_err_code_t parse_message(const char* buffer, uint64_t buffer_size,
                             message_t* message)
{
    /* calculate message size */
    if (sizeof(message_t) > buffer_size)
    {
        LOG_ERROR("Buffer size (%lu) is too small to contain a valid message "
                  "header\n",
                  buffer_size);
        return IOT_MESSAGE_TOO_SHORT;
    }
    memcpy(message, buffer, sizeof(message_t));
    uint64_t message_size = sizeof(message_t) + message->payload_size;
    if (message_size > buffer_size)
    {
        LOG_ERROR("Buffer size (%lu) is too small to contain the complete "
                  "message\n",
                  buffer_size);
        return IOT_MESSAGE_TOO_SHORT;
    }

    if (message->magic_number != 0xAB)
    {
        LOG_ERROR("Invalid magic number: expected 0xAB, got 0x%X\n",
                  message->magic_number);
        return IOT_MESSAGE_INVALID_MAGIC;
    }

    if (message->version != 1)
    {
        LOG_ERROR("Unsupported message version: expected 1, got %d\n",
                  message->version);
        return IOT_MESSAGE_INVALID_VERSION;
    }

    if (message->payload_size == 0 ||
        message->payload_size > MESSAGE_MAX_LENGTH)
    {
        LOG_ERROR("Invalid payload size: %d\n", message->payload_size);
        return IOT_MESSAGE_INVALID_PAYLOAD_SIZE;
    }

    if (message->flags & 0xFFFE)
    {
        LOG_ERROR("Invalid flags: only bit 0 (LAST_MESSAGE_IN_SEQUENCE) and "
                  "bit 1 (ACK) can be set, got 0x%X\n",
                  message->flags);
        return IOT_MESSAGE_INVALID_FLAGS;
    }

    /* check CRC */
    int16_t crc16_calculated = 0;
    if (message->crc != 0)
    {
        LOG_ERROR("Invalid crc! expected: %d, received: %d\n", crc16_calculated,
                  message->crc);
        return IOT_MESSAGE_INVALID_CRC;
    }

    return IOT_SUCCESS;
}
