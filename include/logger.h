//
// Created by piotrmiszta on 2/20/26.
//

#ifndef IOT_LOGGER_H
#define IOT_LOGGER_H
#include <stdio.h>

#define _log(type, ...)                                                        \
    do                                                                         \
    {                                                                          \
        fprintf(stdout, "%s %s:%d  --  ", type, __FILE__, __LINE__);            \
        fprintf(stdout, __VA_ARGS__);                                          \
                                                                               \
    } while (0)

#define LOG_DEBUG(...) _log("DEBUG", __VA_ARGS__);
#define LOG_ERROR(...) _log("ERROR", __VA_ARGS__);
#define LOG_WARNING(...) _log("WARNING", __VA_ARGS__);
#define LOG_FATAL(...) _log("FATAL", __VA_ARGS__);
#define LOG_INFO(...) _log("INFO", __VA_ARGS__);

#define LOG_TRACE(...) _log("TRACE", __VA_ARGS__);
#endif // IOT_LOGGER_H
