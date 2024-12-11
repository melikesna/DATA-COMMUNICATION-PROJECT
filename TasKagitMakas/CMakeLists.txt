cmake_minimum_required(VERSION 3.16)
project(TasKagitMakas C)

set(CMAKE_C_STANDARD 11)

add_executable(server server.c)
add_executable(client client.c
)

target_link_libraries(client ws2_32)
target_link_libraries(server ws2_32)
