set(TARGET_NAME ${PROJECT_NAME}.test)
file(GLOB_RECURSE SOURCE *.cpp)

add_executable(${TARGET_NAME} ${SOURCE})
target_link_libraries(${TARGET_NAME} gtest ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}.test 
  PRIVATE TEST_DIR="${CMAKE_CURRENT_LIST_DIR}/test")

install(TARGETS ${TARGET_NAME}
        DESTINATION "${tool_dest}")
