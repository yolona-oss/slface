cmake_minimum_required(VERSION 3.21)
project(sqlite3)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_LIB_LIST 
		sqlite3.h
		sqlite3.c
	)

add_library(${PROJECT_NAME} SHARED ${SRC_LIB_LIST})
