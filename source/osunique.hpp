#ifndef __OSUNIQUE_HPP
#define __OSUNIQUE_HPP
#include <time.h>
#include <string.h>
#include <stdio.h>

#if !defined(_WIN32)
using rsize_t = size_t;
#endif

auto asciitime( char* buffer, size_t count, const struct tm& tmsource ) -> char*;
// Beware, MSVC localtime_s is not the same as the standard localtime_s
auto lcltime( const time_t& source, struct tm& dest ) -> struct tm*;

auto strcopy( char* dest, rsize_t size, const char* src ) -> void;

auto strncopy( char* dest, size_t size, const char* src, size_t count ) -> void;

auto mstrcat( char* dest, size_t size, const char* src ) -> char*;
auto mgmtime( struct tm* dest, const time_t* timer ) -> struct tm*;
auto mctime( char* buffer, size_t size, const time_t* timer ) -> char*;
auto mstrerror( char* buffer, size_t size, int errornum ) -> char*;
auto mfopen( FILE** stream, const char* filename, const char* mode ) -> FILE*;
#endif
