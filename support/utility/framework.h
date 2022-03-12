#pragma once
#ifndef framework_h
#define framework_h
#if defined(_WIN32)
// We dont want winsock1, and some of the other items we we dont use
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// we dont want MS min/max macros, we are using <algorithm<
#ifndef NOMINMAX
#define NOMINMAX
#endif
// We dont want MBCS, we want UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
// We dont want MBCS, we want UNICODE (and yes, they have two symbos, one without the leading _
#ifndef UNICODE
#define UNICODE
#endif
// we dont want MBCS
#ifdef _MBCS
#undef _MBCS
#endif

#endif

#endif