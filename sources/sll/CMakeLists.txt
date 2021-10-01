cmake_minimum_required(VERSION 3.21)
project(sll)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif(CCACHE_FOUND)

find_package(QT NAMES Qt6 Qt5 COMPONENTS QtCore REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS QtSql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Sql REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS QtNetwork REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)
# find_package(QT NAMES Qt6 Qt5 COMPONENTS QtJson REQUIRED)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Json REQUIRED)

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
					 )
