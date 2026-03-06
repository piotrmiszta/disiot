/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#include "message.h"
#include <stdint.h>

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
