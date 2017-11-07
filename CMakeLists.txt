cmake_minimum_required(VERSION 3.8)
project(SPITESTC)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES SPI.c)
add_executable(SPITESTC ${SOURCE_FILES})