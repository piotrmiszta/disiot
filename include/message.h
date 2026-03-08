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
    uint32_t src_addr;
    uint32_t dest_addr;
    uint16_t src_port;
    uint16_t dest_port;
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

/* Think about this payload*/
typedef message_ping_payload_t message_pong_payload_t;

#define MESSAGE_HEADER_SIZE sizeof(message_t)
#define MESSAGE_PING_PAYLOAD_SIZE sizeof(message_ping_payload_t)
#define MESSAGE_PONG_PAYLOAD_SIZE sizeof(message_pong_payload_t)
#define TRACEROUTE_ENTRY_SIZE sizeof(traceroute_entry_t)

typedef struct conn_t conn_t;
struct sockaddr_in;

void message_dump(const message_t* message);
uint64_t assign_ping_payload(message_t* message,
                             message_ping_payload_t* payload, conn_t* conn,
                             struct sockaddr_in local_addr);

#endif
