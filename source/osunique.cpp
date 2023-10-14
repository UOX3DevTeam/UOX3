#include "osunique.hpp"

#ifdef _WIN32

auto asciitime( char* buffer, size_t count, const struct tm& tmsource ) -> char*
{
	asctime_s( buffer, count, &tmsource );
	return buffer;
}
// Beware, MSVC localtime_s is not the same as the standard localtime_s
auto lcltime( const time_t& source, struct tm& dest ) -> struct tm*
{
	localtime_s( &dest, &source );
	return &dest;
}

auto strcopy( char* dest, rsize_t size, const char* src ) -> void
{
	strcpy_s( dest, size, src );

}
auto strncopy( char* dest, size_t size, const char* src, size_t count ) -> void
{
	strncpy( dest, src, count );
}

auto mstrcat( char* dest, size_t size, const char* src ) -> char*
{
	strcat_s( dest, size, src );
	return dest;
}

auto mgmtime( struct tm* dest, const time_t* timer ) -> struct tm*
{
	gmtime_s( dest, timer );
	return dest;
}

auto mctime( char* buffer, size_t size, const time_t* timer ) -> char*
{
	ctime_s( buffer, size, timer );
	return buffer;
}
auto mstrerror( char* buffer, size_t size, int errornum ) -> char*
{
	strerror_s( buffer, size, errornum );
	return buffer;
}
auto mfopen( FILE** stream, const char* filename, const char* mode ) -> FILE*
{
	fopen_s( stream, filename, mode );
	return *stream;
}
#else
auto asciitime( char* buffer, [[maybe_unused]] size_t count, const struct tm& tmsource ) -> char*
{
	return asctime_r( &tmsource, buffer );

}
auto lcltime( const time_t& source, struct tm& dest ) -> struct tm*
{
	localtime_r( &source, &dest );
	return &dest;
}
auto strcopy( char* dest, [[maybe_unused]] size_t size, const char* src ) -> void
{
	strcpy( dest, src );
}
auto strncopy( char* dest, [[maybe_unused]] size_t size, const char* src, size_t count ) -> void
{
	strncpy( dest, src, count );
}
auto mstrcat( char* dest, [[maybe_unused]] size_t size, const char* src ) -> char*
{
	return strcat( dest, src );
}
auto mgmtime( struct tm* dest, const time_t* timer ) -> struct tm*
{
	return gmtime_r( timer, dest );
}
auto mctime( char* buffer, [[maybe_unused]] size_t size, const time_t* timer ) -> char*
{
	return ctime_r( timer, buffer );
}
auto mstrerror( [[maybe_unused]] char *buffer, [[maybe_unused]] size_t size, int errornum ) -> char*
{
	return strerror( errornum );
}
auto mfopen( FILE** stream, const char* filename, const char* mode ) -> FILE*
{
	*stream = fopen( filename, mode );
	return *stream;
}
#endif
