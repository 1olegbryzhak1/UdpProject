cmake_minimum_required(VERSION 3.16)

project(UdpProject LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
