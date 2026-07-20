#ifndef js_cpucfg___
#define js_cpucfg___

/* AUTOMATICALLY GENERATED - DO NOT EDIT */

#define IS_LITTLE_ENDIAN 1
#undef  IS_BIG_ENDIAN

#ifdef __hppa
# define JS_STACK_GROWTH_DIRECTION (1)
#else
# define JS_STACK_GROWTH_DIRECTION (-1)
#endif
#endif /* js_cpucfg___ */
