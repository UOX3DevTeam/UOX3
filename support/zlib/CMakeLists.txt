cmake_minimum_required(VERSION 3.12.3)

# This code was from zlib
# Only the files used have been kept, so it not a complete
# set of the released version
# This is setup for windows. macOS comes with zlib, and 
# linux is usually easy enough to install system wide

project(zlib VERSION 1.2 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (WIN32)
set(CMAKE_C_FLAGS_RELEASE "/O2")
elseif (APPLE)
set(CMAKE_C_FLAGS_RELEASE "-Os")
else ()
set(CMAKE_C_FLAGS_RELEASE "-O2")
endif()

add_library(zlib
	STATIC
    	adler32.c
   		compress.c
   	 	crc32.c
   		deflate.c
   		gzclose.c
   		gzlib.c
  		gzread.c
  		gzwrite.c
  		infback.c
  		inffast.c
    	inflate.c
   		inftrees.c
    	trees.c
    	uncompr.c
  		zutil.c
)


		
target_compile_options(zlib 
	PRIVATE
		$<$<PLATFORM_ID:Windows>:/W3>
		$<$<PLATFORM_ID:Windows>:/wd4267>
)

target_compile_definitions(zlib
	PRIVATE
		$<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_DEPRECATE>
		$<$<PLATFORM_ID:Windows>:_CRT_NONSTDC_NO_DEPRECATE>
		$<$<PLATFORM_ID:Windows>:WIN32>
		$<$<PLATFORM_ID:Windows>:_LIB>
		$<$<PLATFORM_ID:Windows>:NDEBUG>
)



