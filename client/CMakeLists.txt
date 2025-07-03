add_executable(client
    main.cpp
    client.cpp
    client.h
)

target_link_libraries(client
    PRIVATE
    Qt6::Core
    Qt6::Network
    common_headers
)
