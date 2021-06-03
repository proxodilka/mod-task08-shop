set(TARGET_NAME ${PROJECT_NAME}.test)
file(GLOB_RECURSE SOURCE *.cpp)

add_executable(${TARGET_NAME} ${SOURCE})
target_link_libraries(${TARGET_NAME} gtest pthread ${PROJECT_NAME})
target_include_directories(${TARGET_NAME} PUBLIC ${TASK_LIB_INCLUDE})

target_compile_definitions(${PROJECT_NAME}.test 
  PRIVATE TEST_DIR="${CMAKE_CURRENT_LIST_DIR}/test")

install(TARGETS ${TARGET_NAME}
        DESTINATION "${tool_dest}")
