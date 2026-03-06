/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#include "message.h"
#include "logger.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

message_t parse_message_header(const char* buffer, uint64_t buffer_size)
{
    if (buffer_size < sizeof(message_t))
    {
        // Handle error: buffer too small
    }
    message_t message;
    memcpy(&message, buffer, sizeof(message_t));
    return message;
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
            ntohs(message->src_port), message->payload_size,
            message->flags, message->crc, payload->number_of_entries,
            payload->entries[0].time_stamp);
        break;
    case IOT_MESSAGE_PONG:
    case IOT_MESSAGE_GET:
    case IOT_MESSAGE_POST:
    case IOT_MESSAGE_INFO:
    case IOT_MESSAGE_REPLY:
    case IOT_MESSAGE_ACK:
        assert(0 && "Not implemented message dump for this type");
        break;
    default:
        assert(0 && "Not implemented message dump for this type");
        break;
    }
}