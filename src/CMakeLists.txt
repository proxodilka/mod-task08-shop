
set(LIB_TARGET_NAME ${PROJECT_NAME})

set(LIB_SRC task.cpp statistics.cpp)
file(GLOB_RECURSE LIB_HEADERS ${TASK_LIB_INCLUDE}*.h)

add_library(${LIB_TARGET_NAME} STATIC ${LIB_SRC} ${LIB_HEADERS})
target_link_libraries(${LIB_TARGET_NAME} ThreadUtils pthread)
target_include_directories(
  ${LIB_TARGET_NAME} PUBLIC
  ${TASK_LIB_INCLUDE} ${THREAD_UTILS_INCLUDE})

set(TARGET_NAME ${PROJECT_NAME}.info)
file(GLOB_RECURSE SOURCE main.cpp)

add_executable(${TARGET_NAME} ${SOURCE})
target_link_libraries(${TARGET_NAME} ${LIB_TARGET_NAME})
target_include_directories(${TARGET_NAME} PUBLIC ${TASK_LIB_INCLUDE})

install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION "${tool_dest}"
        COMPONENT library)

install(TARGETS ${LIB_TARGET_NAME}
        LIBRARY DESTINATION "${lib_dest}"
        ARCHIVE DESTINATION "${lib_dest}"
        COMPONENT library)
install(FILES ${LIB_HEADERS} DESTINATION "${include_dest}")
