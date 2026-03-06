/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#ifndef IOT_MESSAGE_H_
#define IOT_MESSAGE_H_
#include <stdint.h>

typedef enum MessageTypeE
{
    IOT_MESSAGE_PING = 0,
    IOT_MESSAGE_PONG,
    IOT_MESSAGE_GET,
    IOT_MESSAGE_POST,
    IOT_MESSAGE_INFO,
    IOT_MESSAGE_REPLY,
    IOT_MESSAGE_ACK,
} MessageTypeE;

typedef struct message_t
{
    /* HEADER START */
    uint8_t magic_number;
    uint8_t version;
    uint8_t message_type;
    uint8_t sequence_number;
    uint32_t dest;
    uint32_t src;
    uint32_t payload_size;
    uint16_t flags;
    uint16_t crc;

    /* PAYLOAD */
    uint8_t payload[];
} message_t;

typedef struct message_ping_payload_t
{
    uint32_t seq_number;
    uint32_t number_of_entries;
    struct
    {
        uint32_t src;
        uint32_t dest;

        uint64_t time_stamp;
    } traceroute;
} message_ping_payload_t;

#endif
