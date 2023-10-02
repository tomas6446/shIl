cmake_minimum_required(VERSION 3.26)
project(2_shell)

set(CMAKE_CXX_STANDARD 17)

add_executable(2_shell main.cpp main.h
)

target_link_libraries(2_shell readline)
