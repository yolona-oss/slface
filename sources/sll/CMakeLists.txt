cmake_minimum_required(VERSION 3.21)
project(sll)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif(CCACHE_FOUND)

# include(FetchContent)
# FetchContent_Declare(cpr
# 	GIT_REPOSITORY https://github.com/libcpr/cpr.git
# 	GIT_TAG        f4622efcb59d84071ae11404ae61bd821c1c344b # the commit hash for 1.6.2
# 	GIT_SHALLOW    TRUE)
# FetchContent_MakeAvailable(cpr)
# if(NOT cpr_POPULATED)
# 	FetchContent_Populate(cpr)
# 	set(USE_SYSTEM_CURL ON CACHE BOOL "" FORCE)
# 	add_subdirectory(${cpr_SOURCE_DIR} ${cpr_BINARY_DIR})
# endif()

# set(USE_SYSTEM_CURL ON)

include(ExternalProject)
include("${CMAKE_SOURCE_DIR}/vendor/rapidjson.cmake")
include_directories(${RAPIDJSON_INCLUDE_DIR})

find_package(QT NAMES Qt6 Qt5 COMPONENTS QtCore REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS QtSql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS QtNetwork REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

set(SRC_LIB_LIST base.cpp
				 api_iterator.cpp
				 itemupdater.cpp
				 storage.cpp
				 database.cpp
				 input.cpp
				 ../../headers/sll/base.h
				 ../../headers/sll/input.h
				 ../../headers/sll/database.h
	)

# set(CMAKE_AUTOMOC ON)
set_property(SOURCE ${SRC_LIB_LIST} PROPERTY CMAKE_AUTOMOC ON)

add_library(${PROJECT_NAME} SHARED ${SRC_LIB_LIST})

target_link_libraries(${PROJECT_NAME}
					  Qt${QT_VERSION_MAJOR}::Core
					  Qt${QT_VERSION_MAJOR}::Sql
					  Qt${QT_VERSION_MAJOR}::Network
					  # cpr::cpr
					 )
