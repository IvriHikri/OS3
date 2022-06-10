cmake_minimum_required(VERSION 3.0.0)
project(OS_Wet3 VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(OS_Wet3 server.c queue.c segel.c request.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set (CMAKE_EXE_LINKER_FLAGS -pthread )
include(CPack)
