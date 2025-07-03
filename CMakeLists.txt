cmake_minimum_required(VERSION 3.16)

project(UdpProject LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/oleh/dev/Qt/6.8.0/gcc_64/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Core Network)

# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

# add_executable(UdpProject

# )
# target_link_libraries(UdpProject Qt${QT_VERSION_MAJOR}::Core)

# include(GNUInstallDirs)
# install(TARGETS UdpProject
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
