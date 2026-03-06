/*
 * Copyright (c) 2026 Piotr Miszta <miszta.piotr.pm@gmail.com>
 * This software is released under the MIT License.
 */

#ifndef IOT_LOGGER_H
#define IOT_LOGGER_H
#include <stdio.h>

#define _log(type, ...)                                                        \
    do                                                                         \
    {                                                                          \
        fprintf(stdout, "%s %s:%d  --  ", type, __FILE__, __LINE__);           \
        fprintf(stdout, __VA_ARGS__);                                          \
        fflush(stdout);                                                        \
    } while (0)

#define LOG_DEBUG(...) _log("\x1b[32mDEBUG\x1b[0m", __VA_ARGS__)
#define LOG_ERROR(...) _log("\x1b[31mERROR\x1b[0m", __VA_ARGS__)
#define LOG_WARNING(...) _log("\x1b[34mWARNING\x1b[0m", __VA_ARGS__)
#define LOG_FATAL(...) _log("\x1b[31mFATAL\x1b[0m", __VA_ARGS__)
#define LOG_INFO(...) _log("\x1b[35mINFO\x1b[0m", __VA_ARGS__)

#define LOG_TRACE(...) _log("\x1b[33mTRACE\x1b[0m", __VA_ARGS__)
#endif // IOT_LOGGER_H
