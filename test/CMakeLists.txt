
### Test cases
add_executable(link2c ${CMAKE_CURRENT_SOURCE_DIR}/link.c)
target_link_libraries(link2c util)

add_executable(link2cpp ${CMAKE_CURRENT_SOURCE_DIR}/link.cpp)
target_link_libraries(link2cpp util)

### Auto tests 
add_custom_target(compiling ALL 
                  DEPENDS link2c link2cpp 
                  VERBATIM)

add_test(compile-all 
         ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compiling)
set_tests_properties(compile-all PROPERTIES DEPENDS compiling)

add_test(linking-to-c-code 
        ${CMAKE_SOURCE_DIR}/tmp-build/test/link2c)
add_test(linking-to-cpp-code 
        ${CMAKE_SOURCE_DIR}/tmp-build/test/link2cpp)

