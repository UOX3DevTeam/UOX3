cmake_minimum_required(VERSION 3.15.3)

project(UOX3 CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSION OFF)

add_executable(uox3 source/uox3.cpp)

target_link_libraries(uox3
	PRIVATE
		uox
)

add_subdirectory(spidermonkey)
add_subdirectory(source)