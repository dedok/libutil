configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.c.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/version.c" @ONLY)

add_library(dev-util
            ${CMAKE_CURRENT_SOURCE_DIR}/log.c
            ${CMAKE_CURRENT_SOURCE_DIR}/syslogd.c
            ${CMAKE_CURRENT_SOURCE_DIR}/version.c
            ${CMAKE_CURRENT_SOURCE_DIR}/config.c
)

target_link_libraries(dev-util ${LUA_LIBRARY} pthread)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libutil
        DESTINATION ${PREFIX}/include
        PATTERN ".*" EXCLUDE
        PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ)

install(TARGETS dev-util
        RUNTIME DESTINATION bin/
        LIBRARY DESTINATION lib/
        ARCHIVE DESTINATION lib/)

