add_library(js32
	STATIC
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
		jsiter.c
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



target_compile_definitions(js32
	PRIVATE
		_X86_
		$<$<PLATFORM_ID:Windows>:XP_WIN>
		$<$<PLATFORM_ID:Windows>:JSFILE>
		$<$<PLATFORM_ID:Windows>:EXPORT_JS_API>
)



set_target_properties(js32
	PROPERTIES
         RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)
