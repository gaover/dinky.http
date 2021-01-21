cmake_minimum_required (VERSION 3.8)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
project(small_http C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

SET(DEBUG_SUFFIX "_d")

IF (MSVC)
  SET(PLATFORM "win64")
  # Build with multiple processes
  add_definitions(/MP)
  add_definitions(-DWIN32)
  add_definitions(-D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)
  # add_definitions(-DUNICODE)
  ADD_DEFINITIONS(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
  # MSVC warning suppressions
  add_definitions(
    /wd4018 # 'expression' : signed/unsigned mismatch
    /wd4065 # switch statement contains 'default' but no 'case' labels
    /wd4146 # unary minus operator applied to unsigned type, result still unsigned
    /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
    /wd4251 # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
    /wd4305 # 'identifier' : truncation from 'type1' to 'type2'
    /wd4307 # 'operator' : integral constant overflow
    /wd4309 # 'conversion' : truncation of constant value
    /wd4334 # 'operator' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
    /wd4355 # 'this' : used in base member initializer list
    /wd4506 # no definition for inline function 'function'
    /wd4800 # 'type' : forcing value to bool 'true' or 'false' (performance warning)
    /wd4996 # The compiler encountered a deprecated declaration.
    /wd4221 # The compiler encountered a deprecated declaration.
    /wd4100 # The compiler encountered a deprecated declaration.
    /wd4201 # The compiler encountered a deprecated declaration.
    /wd4353 # The compiler encountered a deprecated declaration.
  )
  
  # for zookeeper


  SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj /diagnostics:classic")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI -D_DEBUG /MDd /Od /W3 ")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi -DNDEBUG /MD /W4  ")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj /diagnostics:classic -std=c++11  ")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI -D_WIN64 -D_DEBUG /MDd /Od /W3 ")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi -D_WIN64 -DNDEBUG /MD /W3 ")

  # Suppress linker warnings about files with no symbols defined.
  
  SET(PLATFORM_EXEC_SUFFIX ".exe")


ELSE()
    SET(PLATFORM "linux")
	ADD_DEFINITIONS(-D__LINUX__)
	SET(CMAKE_CXX_FLAGS "-Wall -Werror -fPIC -fno-strict-aliasing -Wno-multichar -Wno-sign-compare -std=c++11")
	SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,-as-needed")
	#SET(CMAKE_CXX_FLAGS_RELEASE "-g -O2 -Wall -Werror -fPIC -fno-strict-aliasing -Wno-multichar")
	SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Werror -fPIC -fno-strict-aliasing -Wno-multichar")
	SET(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -g -Wall -Werror -fPIC -fno-strict-aliasing -Wno-multichar -DDEBUG -D_DEBUG")
	# SET(CMAKE_INSTALL_RPATH lib)
	SET(SYSTEM_INCLUDE_DIR /usr/local/include /home/mysql/devdb/include /usr/include/zookeeper)
	SET(SYSTEM_LIB_DIR /usr/local/lib /home/mysql/devdb/lib)
	
	INCLUDE_DIRECTORIES(${SYSTEM_INCLUDE_DIR})
	LINK_DIRECTORIES(${SYSTEM_LIB_DIR})
	IF(USE_EPOLL_ET)
		ADD_DEFINITIONS(-DUSE_EPOLL_ET_MODE)
	ENDIF(USE_EPOLL_ET)
	
ENDIF(MSVC)

set(CMAKE_DEBUG_POSTFIX "_d")

IF(IS_DEBUG)
  SET(CMAKE_EXECUTABLE_SUFFIX ${DEBUG_SUFFIX}${PLATFORM_EXEC_SUFFIX})
ENDIF()

# Meta information about the project
set(META_PROJECT_NAME        dinky)
set(META_PROJECT_DESCRIPTION "CMake project of my http")
set(META_AUTHOR_ORGANIZATION "gaover")
set(META_AUTHOR_MAINTAINER   "gaover@yahoo.com")
set(META_VERSION_MAJOR       "1")
set(META_VERSION_MINOR       "0")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_NAME_VERSION        "${META_PROJECT_NAME} v${META_VERSION}")

# Create version file
file(WRITE "${PROJECT_BINARY_DIR}/VERSION" "${META_NAME_VERSION}")

# 分别设置Debug和Release输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)




add_subdirectory(src)
add_subdirectory(test)