add_library(js32
	SHARED
		jsapi.c
		jsarena.c
		jsarray.c
		jsatom.c
		jsbool.c
		jscntxt.c
		jsdate.c
		jsdbgapi.c
		jsdhash.c
		jsdtoa.c
		jsemit.c
		jsexn.c
		jsfun.c
		jsgc.c
		jshash.c
		jsinterp.c
		jslock.c
		jslog2.c
		jslong.c
		jsmath.c
		jsnum.c
		jsobj.c
		jsopcode.c
		jsparse.c
		jsprf.c
		jsregexp.c
		jsscan.c
		jsscope.c
		jsscript.c
		jsstr.c
		jsutil.c
		jsxdrapi.c
		jsxml.c
		prmjtime.c
)

target_link_libraries(js32
	PRIVATE
		fdlibm
)

#target_link_options(js32
#	PRIVATE
#		$<$<PLATFORM_ID:Windows>:/dll>
#		$<$<PLATFORM_ID:Windows>:/machine:I386>
#		$<$<PLATFORM_ID:Windows>:/opt:ref,noicf>
#		$<$<PLATFORM_ID:Windows>:/subsystem:windows>
#)

target_compile_definitions(js32
	PRIVATE
		_X86_
		$<$<PLATFORM_ID:Windows>:XP_WIN>
		$<$<PLATFORM_ID:Windows>:JSFILE>
		$<$<PLATFORM_ID:Windows>:EXPORT_JS_API>
)

#target_compile_options(js32
#	PRIVATE
#		$<$<C_COMPILER_ID:MSVC>:/W3>
#		$<$<C_COMPILER_ID:MSVC>:/EHsc>
#)

set_target_properties(js32
	PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)

add_subdirectory(fdlibm)