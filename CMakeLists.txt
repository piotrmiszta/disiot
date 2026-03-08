
cmake_minimum_required(VERSION 3.30)
project(IOT)

set(CMAKE_C_FLAGS "-g -Werror -Wall -Wextra -pedantic -std=c2x -Wpadded -O3")


add_executable(${PROJECT_NAME} src/main.c
        src/connection.c
        src/message.c
        
        include/connection.h
        include/err_codes.h
        include/logger.h
	include/message.h
        )

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_link_libraries(${PROJECT_NAME} pthread)
