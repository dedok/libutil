cmake_minimum_required(VERSION 2.8)

project(libutil)

### setup envt

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include(GetGitRevisionDescription)

set(CFLAGS_COMMON   "-Wall -Wextra -Werror")
set(CFLAGS_REL      "-O2 ${CFLAGS_COMMON} -DNDEBUG")
set(CFLAGS_DBG      "-O0 -ggdb3 ${CFLAGS_COMMON} -DDEBUG")
set(CFLAGS_DBGREL   "-O2 -g ${CFLAGS_COMMON} -DNDEBUG")

set(CMAKE_C_FLAGS                "${CMAKE_C_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS              "${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG}      ${CFLAGS_DBG}")
set(CMAKE_CXX_FLAGS_DEBUG        "${CMAKE_CXX_FLAGS_DEBUG}    ${CFLAGS_DBG}")
set(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE}    ${CFLAGS_REL}")
set(CMAKE_CXX_FLAGS_RELEASE      "${CMAKE_CXX_FLAGS_RELEASE}  ${CFLAGS_REL}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELEASE}   ${CFLAGS_DBGREL}")
set(CMAKE_CXX_FLAGS_RELWITHDEBUGINFO
                        "${CMAKE_CXX_FLAGS_RELWITHDEBUGINFO} ${CFLAGS_DBGREL}")

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, using Release as default")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                      --force-new-ctest-process --output-on-failure
                      --build-config "$<CONFIGURATION>")
else()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                      --force-new-ctest-process --output-on-failure)
endif()

include(CTest)
enable_testing()

### entry
git_describe(VERSION)

find_package(Lua51 REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LUA_INCLUDE_DIR} $ENV{INCLUDE__})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

