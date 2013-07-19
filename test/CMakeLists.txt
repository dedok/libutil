
### Test cases
add_executable(link2c ${CMAKE_CURRENT_SOURCE_DIR}/link.c)
target_link_libraries(link2c dev-util)

add_executable(link2cpp ${CMAKE_CURRENT_SOURCE_DIR}/link.cpp)
target_link_libraries(link2cpp dev-util)

add_executable(config ${CMAKE_CURRENT_SOURCE_DIR}/config.c)
target_link_libraries(config dev-util)

### Auto tests 
add_custom_target(compiling ALL 
                  DEPENDS link2c link2cpp config 
                  VERBATIM)

add_test(compile-all 
         ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compiling)
set_tests_properties(compile-all PROPERTIES DEPENDS compiling)

add_test(linking-to-c-code 
        ${CMAKE_SOURCE_DIR}/tmp-build/test/link2c)

add_test(linking-to-cpp-code 
        ${CMAKE_SOURCE_DIR}/tmp-build/test/link2cpp)

add_test(NAME config-test 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_SOURCE_DIR}/test/config.sh)

