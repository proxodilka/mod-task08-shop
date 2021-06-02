set(TARGET_NAME ThreadUtils)
set(SOURCE_PATH, "src/")
set(HEADER_PATH, "include/")

file(GLOB_RECURSE SOURCE ${SOURCE_PATH}*.cpp)
file(GLOB_RECURSE HEADERS ${HEADER_PATH}*.h)

add_library(${TARGET_NAME} STATIC ${SOURCE} ${HEADERS})
target_include_directories(${TARGET_NAME} PUBLIC "include/")

install(TARGETS ${TARGET_NAME}
		LIBRARY DESTINATION "${lib_dest}"
        ARCHIVE DESTINATION "${lib_dest}"
        COMPONENT library)
install(FILES ${HEADERS} DESTINATION "${include_dest}")
