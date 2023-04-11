add_library(fdlibm
	STATIC
		e_atan2.c
		e_pow.c
		e_sqrt.c
		k_standard.c
		s_atan.c
		s_copysign.c
		s_fabs.c
		s_finite.c
		s_isnan.c
		s_matherr.c
		s_rint.c
		s_scalbn.c
		w_atan2.c
		w_pow.c
		w_sqrt.c
		s_lib_version.c
)

target_compile_definitions(fdlibm
	PRIVATE
		_X86_
		$<$<PLATFORM_ID:Windows>:_IEEE_LIBM>
		$<$<PLATFORM_ID:Windows>:XP_WIN>
)

#target_compile_options(fdlibm
#	PRIVATE
#		$<$<C_COMPILER_ID:MSVC>:/W3>
#		$<$<C_COMPILER_ID:MSVC>:/EHsc>
#)