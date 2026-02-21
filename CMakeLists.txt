
cmake_minimum_required(VERSION 3.30)
project(IOT)

set(CMAKE_C_FLAGS "-g -Werror -Wall -Wextra -pedantic")

add_executable(${PROJECT_NAME} src/main.c
        src/connection.c
        include/connection.h
        include/err_codes.h
        include/logger.h)

target_include_directories(${PROJECT_NAME} PUBLIC include/)
