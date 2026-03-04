//
// Created by piotrmiszta on 2/20/26.
//

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
} iot_err_code_t;

#endif // IOT_ERR_CODES_H
