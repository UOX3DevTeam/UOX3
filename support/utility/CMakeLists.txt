cmake_minimum_required(VERSION 3.12.3)

project(utility VERSION 0.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "/O2")
elseif (APPLE)
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
else ()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()

add_library(utility 
	STATIC
		bytebuffer.cpp
		EventTimer.cpp
		header.cpp
		ip4addr.cpp
		ip4list.cpp
		keyvalue.cpp
		mapip.cpp
		netexcept.cpp
		netsupport.cpp
		secgroup.cpp
		section.cpp
		socket.cpp
		strutil.cpp
)


		
target_compile_options(utility 
	PRIVATE
		$<$<PLATFORM_ID:Windows>:/W3>

		$<$<PLATFORM_ID:Linux>:-Wno-unused-result>
)

target_compile_definitions(utility
	PRIVATE
		$<$<PLATFORM_ID:Windows>:_LIB>
		NDEBUG
)



