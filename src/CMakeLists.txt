configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.c.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/version.c" @ONLY)

add_library(util
            ${CMAKE_CURRENT_SOURCE_DIR}/log.c     
            ${CMAKE_CURRENT_SOURCE_DIR}/syslogd.c
            ${CMAKE_CURRENT_SOURCE_DIR}/version.c
)

target_link_libraries(util pthread)   
