/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#ifndef IOT_ERR_CODES_H
#define IOT_ERR_CODES_H

typedef enum
{
    IOT_SUCCESS = 0,
    IOT_ERROR = -1,
    IOT_EALLOC = -2,
    IOT_ECONN = -3,
    IOT_ESOCKET = -4,
    IOT_EBIND = -5,
    IOT_ELISTEN = -6,
    IOT_ESYS = -7,

    IOT_MESSAGE_TOO_SHORT = -100,
    IOT_MESSAGE_INVALID_MAGIC = -101,
    IOT_MESSAGE_INVALID_VERSION = -102,
    IOT_MESSAGE_INVALID_TYPE = -103,
    IOT_MESSAGE_INVALID_SEQUENCE = -104,
    IOT_MESSAGE_INVALID_PAYLOAD_SIZE = -105,
    IOT_MESSAGE_INVALID_FLAGS = -106,
    IOT_MESSAGE_INVALID_CRC = -107,
} iot_err_code_t;

#endif // IOT_ERR_CODES_H
