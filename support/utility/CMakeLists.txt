
# *************************************************************************
# how to use
# *************************************************************************
# mkdir build
# cd build
# cmake .. -DCMAKE_BUILD_TYPE=Release  (Windows/Linux/FreeBSD)
# cmake .. -DCMAKE_BUILD_TYPE=Release -G"Unix Makefiles"  (macOS)
# cmake --build . --config Release
# *************************************************************************

cmake_minimum_required(VERSION 3.24)
project(utility VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" )
endif()

# *************************************************************************
# The basic library we are making
# *************************************************************************

add_library(utility STATIC
    buffer.cpp
    filemap.cpp
    timer.cpp
    console.cpp
    sock.cpp
    netutil.cpp
    color.cpp
    image.cpp
    
    buffer.hpp
    filemap.hpp
    timer.hpp
    console.hpp
    sock.hpp
    netutil.hpp
    color.hpp
    image.hpp
    
    strutil.hpp
    numinc.hpp
    random.hpp
)

if (WIN32)

	target_compile_definitions( utility PRIVATE
		_LIB
		$<$<CONFIG:Release>:NDEBUG>
		$<$<CONFIG:Debug>:_DEBUG>
	)
	
	target_compile_options( utility PRIVATE
		$<$<CONFIG:Release>:/O2>
	)
	
else()

	if (APPLE)
		target_compile_options( utility PRIVATE
			$<$<CONFIG:Release>:-Os>
		)
	else()
		target_compile_options(utility PRIVATE
			$<$<CONFIG:Release>:-O2>
		)
	endif()
endif(WIN32) 


