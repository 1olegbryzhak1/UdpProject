qt_add_executable(server
    main.cpp
    server.cpp
    server.h
)

target_link_libraries(server
    PRIVATE
    Qt6::Core
    Qt6::Network
    common_headers
)
