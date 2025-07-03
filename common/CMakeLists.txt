add_library(common_headers STATIC
        protocol.h
        logger.h
)

target_include_directories(common_headers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})