
#ifndef ostype_h
#define ostype_h

// Finds the OS
#if defined(_WIN32)

#define XP_WIN // JS API Requires we define OS we compile with
#define XP_PC
#ifdef _WIN64
#define OS_STR "Win64"
#else
#define OS_STR "Win32"
#endif //_WIN64

#else // A unix type system

#define XP_UNIX // JS API Requires we define OS we compile with
#if defined(__linux__)

#if INTPTR_MAX == INT64_MAX
#define OS_STR "Linux64"
#elif INTPTR_MAX == INT32_MAX
#define OS_STR "Linux32"
#endif // INTPTR_MAX

#elif defined(__APPLE__)

#if INTPTR_MAX == INT64_MAX
#define OS_STR "MacOS64"
#elif INTPTR_MAX == INT32_MAX
#define OS_STR "MacOS32"
#endif // INPTR_MAX

#else

#define OS_STR "Unknown"
#endif //__linux__
#endif //_WIN32

#endif /* ostype_h */
