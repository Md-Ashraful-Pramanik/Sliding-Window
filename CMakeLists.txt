cmake_minimum_required(VERSION 3.17)
project(RTP)

set(CMAKE_CXX_STANDARD 14)

add_executable(RTP dll.c)
add_executable(crc  crc_test.c)