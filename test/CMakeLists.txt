set(Target HttpTest)

message(STATUS "Start building : " ${Target})

include_directories(${CMAKE_SOURCE_DIR}/src)

# 设置依赖动态库、静态库路径
link_directories(
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
        )

aux_source_directory(. SRCS)

add_executable(${Target} ${SRCS})

target_link_libraries(${Target} small_http)
IF(UNIX)
target_link_libraries(${Target} pthread)
ENDIF()

SET_TARGET_PROPERTIES(${Target} PROPERTIES FOLDER "test")
set_target_properties(${Target} PROPERTIES DEBUG_POSTFIX "_d")

