configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.c.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/version.c" @ONLY)

add_library(dev-util
            ${CMAKE_CURRENT_SOURCE_DIR}/log.c     
            ${CMAKE_CURRENT_SOURCE_DIR}/syslogd.c
            ${CMAKE_CURRENT_SOURCE_DIR}/version.c
            ${CMAKE_CURRENT_SOURCE_DIR}/config.c
)

target_link_libraries(dev-util lua pthread)

