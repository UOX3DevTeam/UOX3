cmake_minimum_required(VERSION 3.18)
project(fdlibm VERSION 1.7 LANGUAGES C)

add_library(fdlibm STATIC
    ../../../fdlibm/e_atan2.c
    ../../../fdlibm/e_pow.c
    ../../../fdlibm/e_sqrt.c
    ../../../fdlibm/k_standard.c
    ../../../fdlibm/s_atan.c
    ../../../fdlibm/s_copysign.c
    ../../../fdlibm/s_fabs.c
    ../../../fdlibm/s_finite.c
    ../../../fdlibm/s_isnan.c
    ../../../fdlibm/s_matherr.c
    ../../../fdlibm/s_rint.c
    ../../../fdlibm/s_scalbn.c
    ../../../fdlibm/w_atan2.c
    ../../../fdlibm/w_pow.c
    ../../../fdlibm/w_sqrt.c
    ../../../fdlibm/fdlibm.h
)

target_compile_definitions( fdlibm PRIVATE
		XP_WIN
		JSFILE
		EXPORT_JS_API
		_CRT_SECURE_NO_WARNINGS
		_CRT_SECURE_NO_DEPRECATE
		_CRT_SECURE_NO_DEPRECATE
		_IEEE_LIBM
		_LIB
		_WINDOWS
		_X86_=1
)
	
target_compile_options( fdlibm PRIVATE
		/W3
		/sdl-
		/WX-
		$<$<CONFIG:Release>:/O2>
)
	




