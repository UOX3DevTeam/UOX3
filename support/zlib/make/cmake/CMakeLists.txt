cmake_minimum_required(VERSION 3.18)
project(z VERSION 1 LANGUAGES C)

add_library(z STATIC
	../../adler32.c
	../../compress.c
	../../crc32.c
	../../deflate.c
	../../gzclose.c
	../../gzlib.c
	../../gzread.c
	../../gzwrite.c
	../../infback.c
	../../inffast.c
	../../inflate.c
	../../inftrees.c
	../../trees.c
	../../uncompr.c
	../../zutil.c
	../../crc32.h
	../../deflate.h
	../../gzguts.h
	../../inffast.h
	../../inffixed.h
	../../inflate.h
	../../inftrees.h
	../../trees.h
	../../zconf.h
	../../zlib.h
	../../zutil.h
)

if (WIN32)

	target_compile_definitions(z PRIVATE
		_LIB
		_WINDOWS
		_CRT_SECURE_NO_DEPRECATE
		_CRT_NONSTDC_NO_DEPRECATE
	)
	
	target_compile_options( z PRIVATE
		/W3
		/sdl-
		/wd4244 
		/wd4267 
		/wd4047
		/wd4146
		/wd4334
		/wd4311
		/WX-
		$<$<CONFIG:Release>:/O2>
	)
	
else()

	target_compile_definitions( z PRIVATE
	)
	
	target_compile_options( z PRIVATE
		-Wno-implicit-function-declaration
	)
	if (APPLE)
		target_compile_options( z PRIVATE
			$<$<CONFIG:Release>:-Os>
		)
	else()
		target_compile_options(z PRIVATE
			$<$<CONFIG:Release>:-O2>
		)
	endif(APPLE)
endif(WIN32)


