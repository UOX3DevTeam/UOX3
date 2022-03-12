cmake_minimum_required(VERSION 3.12.3)


project(uox VERSION 0.01 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_subdirectory(uoxserver)
add_subdirectory(support/jsengine)
add_subdirectory(support/utility)

add_executable(uox 
	uoxserver/uox3.cpp
	$<$<PLATFORM_ID:Windows>:uoxserver/uox3.rc>
)

# We need to link to zlib(z) for linux, but we dont build it, that is why it is here
# and not for windows, which adds it to be build with add_subdirectory below
if (WIN32)
add_subdirectory(support/zlib)

target_link_libraries(uox PUBLIC
	uoxserver 
	utility
	jsengine
	zlib
	-lWs2_32
	-lwinmm
)
target_link_options(uox 
	PUBLIC
		 /OPT:NOREF
)
else()
target_link_libraries(uox 
	uoxserver 
	jsengine
	utility
	-lz
	-lpthread
)
endif(WIN32)

target_include_directories(uox
	PRIVATE
		${PROJECT_SOURCE_DIR}/support/jsengine
		${PROJECT_SOURCE_DIR}/support/utility
		${PROJECT_SOURCE_DIR}/uoxserver
		$<$<PLATFORM_ID:Windows>:${PROJECT_SOURCE_DIR}/support/zlib>
)

target_compile_options(uox 
	PRIVATE
		$<$<PLATFORM_ID:Windows>:/J>
		$<$<PLATFORM_ID:Windows>:/W3>
		$<$<PLATFORM_ID:Windows>:/fp:precise>
		$<$<PLATFORM_ID:Windows>:/GL>
		$<$<PLATFORM_ID:Windows>:/Gy>
		$<$<PLATFORM_ID:Windows>:/GF>
		$<$<PLATFORM_ID:Windows>:/Zc:inline>

		$<$<PLATFORM_ID:Linux>:-pthread>
		
)
target_compile_definitions(uox
	PRIVATE
		$<$<PLATFORM_ID:Windows>:_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING>
		$<$<PLATFORM_ID:Windows>:_CRT_NO_VA_START_VALIDATION>
		$<$<PLATFORM_ID:Windows>:EXPORT_JS_API>
		$<$<PLATFORM_ID:Windows>:_MBCS>
		NDEBUG
)



