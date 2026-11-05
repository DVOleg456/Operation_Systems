cmake_minimum_required(VERSION 3.10)
project(OS_Labs C)

set(CMAKE_C_STANDARD 11)

add_executable(lab1 lab1_main.c)

add_library(os_process_lib STATIC process_runner.c)
add_executable(lab2 lab2_main.c)
target_link_libraries(lab2 os_process_lib)