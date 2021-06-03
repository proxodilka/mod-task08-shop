cmake_minimum_required (VERSION 3.10.2)
project (mod-task08-shop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g")

set(tool_dest "bin")
set(lib_dest "lib")
set(include_dest "include")
set(THREAD_UTILS_INCLUDE ${CMAKE_SOURCE_DIR}/thread_utils/include)
set(TASK_LIB_INCLUDE ${CMAKE_SOURCE_DIR}/include/)

add_subdirectory(thread_utils)
add_subdirectory(src)
add_subdirectory(test)
