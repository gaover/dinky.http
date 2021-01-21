set(Target small_http)

message(STATUS "Start building : " ${Target})

include_directories(${CMAKE_SOURCE_DIR}/src)

FILE(GLOB HTTP_SRCS ./*.cpp ./*.h ./*.hpp)

SOURCE_GROUP("src" FILES ${HTTP_SRCS})

add_library(${Target} STATIC ${HTTP_SRCS})

SET_TARGET_PROPERTIES(${Target} PROPERTIES FOLDER "libs")
set_target_properties(${Target} PROPERTIES DEBUG_POSTFIX "_d")
