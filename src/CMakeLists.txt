cmake_minimum_required(VERSION 3.10.2)
project(TP1 C)

set(CMAKE_C_STANDARD 99)
find_package(Threads REQUIRED)

add_executable(TP1 main.c)
target_link_libraries(${PROJECT_NAME} Threads::Threads)