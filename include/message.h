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

/*
    |  0x00   |   0x01   |   0x02   |   0x03   |   0x04   |   0x05   |   0x06 |
   0x07   |
    ---------------------------------------------------------------------------------------
    | magic   | version  |  type    |  seq     |
    | number  |
*/

typedef struct message_t
{
    /* HEADER START */
    uint8_t magic_number;
    uint8_t version;
    uint8_t message_type;
    uint8_t sequence_number; /* Sequence number specify which exactly  */
    uint32_t dest;
    uint32_t src;
    uint32_t payload_size;
    uint16_t flags;
    uint16_t crc;

    /* PAYLOAD */
    uint8_t payload[];
} message_t;

typedef struct traceroute_entry_t
{
    uint32_t ip_addr;
    uint8_t hop_number;
    uint8_t status;
    uint16_t reserved;
    uint64_t time_stamp;
} traceroute_entry_t;

typedef struct message_ping_payload_t
{
    uint32_t number_of_entries;
    uint32_t reserved;
    traceroute_entry_t entries[];
} message_ping_payload_t;

#endif
