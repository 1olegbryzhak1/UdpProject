cmake_minimum_required(VERSION 3.16)

project(UdpProject LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/oleh/dev/Qt/6.8.0/gcc_64/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
