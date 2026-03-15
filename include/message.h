/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#ifndef IOT_MESSAGE_H_
#define IOT_MESSAGE_H_
#include "err_codes.h"
#include <stdint.h>

typedef enum MessageTypeE
{
    IOT_MESSAGE_PING = 0,
    IOT_MESSAGE_PONG,
    IOT_MESSAGE_GET,
    IOT_MESSAGE_POST,
    IOT_MESSAGE_INFO,
    IOT_MESSAGE_ACK,
} MessageTypeE;

typedef enum MessageFlagsE
{
    LAST_MESSAGE_IN_SEQUENCE_FLAG = 1U << 0,
    ACK_FLAG = 1U << 1,
    ERROR_FLAG = 1U << 2,
} MessageFlagsE;

typedef struct message_t
{
    /* HEADER START */
    uint8_t magic_number; // 0xAB
    uint8_t version;      // 0x01
    uint8_t message_type; // MessageTypeE
    /*  Number of which message in the sequence is this one.
        If flag LAST_MESSAGE_IN_SEQUENCE is set next message should have
       sequence_number = 0 */
    uint8_t sequence_number;

    uint32_t src_addr;
    uint32_t dest_addr;
    uint16_t src_port;  /* TODO: Think about that if this is needed or this
                           communication can be in one port */
    uint16_t dest_port; /* TODO: Think about that if this is needed or this
                           communication can be in one port */

    uint32_t payload_size; /* Size of the payload in bytes */
    uint16_t flags;        /* Message flags:
                               - 0bit: LAST_MESSAGE_IN_SEQUENCE: If set, this is the
                              last message in a sequence
                               - 1bit: ACK: If set, this message is an acknowledgment
                               - 2bit: ERROR: If set, this message contains an error */
    uint16_t crc; /* CRC should be calculated with whole message with payload
                     bytes */

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

iot_err_code_t parse_message(const char* buffer, uint64_t buffer_size,
                             message_t* message);

#endif
