#ifndef __CNETWORK_H__
#define __CNETWORK_H__

#include <mutex>
#include <ostream>
#include <cstdint>
#include <vector>
#include <list>
#include <deque>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <charconv>
#include <array>

#if !defined(_WIN32)
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#undef min
#undef max
#endif


//o------------------------------------------------------------------------------------------------o
// ByteBufferBounds_st
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
struct ByteBufferBounds_st : public std::out_of_range
{
	int offset;
	int amount;
	int buffersize;
	std::string _msg;
	explicit ByteBufferBounds_st( int offset, int amount, int size );
	auto what() const noexcept -> const char* override;
};

//o------------------------------------------------------------------------------------------------o
// Radix that are supported
// Normally part of strutil
enum class radix_t { dec = 10, oct = 8, hex = 16, bin = 2 };

//o------------------------------------------------------------------------------------------------o
// ByteBuffer_t
//o------------------------------------------------------------------------------------------------o
//o------------------------------------------------------------------------------------------------o
class ByteBuffer_t
{
	// These are compatability to "look" like a the original CPacketStream
	//=============================================================================================
public:
	auto ReserveSize( size_t len ) -> void
	{
		this->size( static_cast<int>( len ), 0 );
	}
	auto WriteByte( size_t pos, std::uint8_t toWrite ) -> void
	{
		this->write( static_cast<int>( pos ), toWrite );
	}
	// Why isn't thist std::int16_t?
	auto WriteShort( size_t pos, std::int32_t toWrite ) -> void
	{
		this->write( static_cast<int>( pos ), static_cast<int16_t>( toWrite ));
	}
	auto WriteLong( size_t pos, std::uint32_t toWrite ) -> void
	{
		this->write( static_cast<int>( pos ), toWrite );
	}
	auto WriteString( size_t pos, const std::string& toWrite, size_t len ) -> void
	{
		this->write( static_cast<int>( pos ), toWrite, static_cast<int>( len ), false );
	}
	auto WriteArray( size_t pos, const std::uint8_t *toWrite, size_t len ) -> void
	{
		this->write( static_cast<int>( pos ), toWrite, static_cast<int>( len ), false );
	}
	auto GetByte( size_t pos) const -> std::uint8_t
	{
		try
		{
			return this->read<std::uint8_t>( static_cast<int>( pos ));
		}
		catch(...)
		{
			return 0;
		}
	}
	auto GetShort( size_t pos ) const -> std::int16_t
	{
		try
		{
			return this->read<std::int16_t>( static_cast<int>( pos ));
		}
		catch(...)
		{
			return 0;
		}
	}
	auto GetUShort( size_t pos ) const -> std::uint16_t
	{
		try
		{
			return this->read<std::uint16_t>( static_cast<int>( pos ));
		}
		catch(...)
		{
			return 0;
		}
	}
	auto GetLong( size_t pos ) const -> std::int32_t
	{
		try
		{
			return this->read<std::int32_t>( static_cast<int>( pos ));
		}
		catch(...)
		{
			return 0;
		}
	}
	auto GetULong( size_t pos ) const -> std::uint32_t
	{
		try
		{
			return this->read<std::uint32_t>( static_cast<int>( pos ));
		}
		catch(...)
		{
			return 0;
		}
	}
	
	auto GetBuffer() const -> const std::uint8_t*
	{
		return this->raw();
	}

	auto GetSize() const -> size_t
	{
		return this->size();
	}

	// End compatability of original CPacketStream
	//o------------------------------------------------------------------------------------------------o

	//o------------------------------------------------------------------------------------------------o
	// Normally part of strutil, but included here for stand alone
public:

	// The maximum characters in a string number for conversion sake
	static constexpr auto max_characters_in_number = 12;

	// Dumps a byte buffer, formatted to a provided stream.
	// The entries_line indicate how many bytes to display per line.
	static auto DumpByteBuffer( std::ostream &output, const std::uint8_t *buffer, std::size_t length, radix_t radix = radix_t::hex, std::size_t entries_line = 8 ) -> void;

private:

	// Convert a bool to a string
	// the true_value/false_value are returned based on the bool
	static auto ntos( bool value, const std::string &true_value = "true", const std::string &false_value = "false" ) -> std::string;


	// Convert a number to a string, with options on radix, prefix, size, pad
	// Radix indicates the radix the string will represent
	// prefix indicates if the "radix" indicator will be present at the front of the string
	// size is the minimum size the string must be (if the number is not large enough
	// it will use the "pad" character prepended to the number
	template <typename T>
	static auto ntos( T value, radix_t radix = radix_t::dec, bool prefix = false, int size = 0, char pad = '0' ) -> std::string
	{
		if constexpr( std::is_integral_v<T> && !std::is_same_v<T, bool> )
		{
			// first, thing we need to convert the value to a string
			std::array<char, max_characters_in_number> str;
			
			if( auto [pc, ec] = std::to_chars( str.data(), str.data() + str.size(), value, static_cast<int>( radix )); ec == std::errc() )
			{
				// How many characters did this number take
				auto numchars = static_cast<int>( std::distance( str.data(), pc ));
				// what is larger, that is the size of our string
				auto sizeofstring = std::max( numchars, size );
				// where do we start adding the number into our string ?
				auto index = sizeofstring - numchars;
				if( prefix )
				{
					// We have a prefix, so we add two characters to the beginning
					sizeofstring += 2;
					index += 2;
				}
				auto rValue = std::string( sizeofstring, pad );
				// copy the value into the string
				std::copy( str.data(), pc, rValue.begin() + index );
				// do we need our prefix?
				if( prefix )
				{
					switch( static_cast<int>( radix ))
					{
						case static_cast<int>( radix_t::dec ):
							// We dont add anything for decimal!
							break;
						case static_cast<int>( radix_t::hex ):
							rValue[0] = '0';
							rValue[1] = 'x';
							break;
						case static_cast<int>( radix_t::oct ):
							rValue[0] = '0';
							rValue[1] = 'o';
							break;
						case static_cast<int>( radix_t::bin ):
							rValue[0] = '0';
							rValue[1] = 'b';
							break;
							
						default:
							break;
					}
				}
				return rValue;
			}
			else
			{
				// The conversion was not successful, so we return an empty string
				return std::string();
			}
		}
	}
	// End of strutil inclusion
	//=============================================================================================

protected:
	mutable int _index;
	std::vector<std::uint8_t> _bytedata;
	auto Exceeds( int offset, int bytelength ) const -> bool;
	auto Exceeds( int offset, int bytelength, bool expand ) -> bool;
public:
	ByteBuffer_t( int size = 0, int reserve = 0 );

	auto size() const -> size_t;
	auto size( int value, std::uint8_t fill = 0 ) -> void;

	auto index() const -> int;
	auto index( int value ) -> void;

	auto raw() const -> const std::uint8_t*;
	auto raw() -> std::uint8_t*;

	auto operator[]( int index ) const -> const std::uint8_t &;
	auto operator[]( int index ) -> std::uint8_t &;

	auto Fill( std::uint8_t value, int offset, int length ) -> void;
	auto LogByteBuffer( std::ostream &output, radix_t radix = radix_t::hex, int entries_line = 8 ) const -> void;

	// we need to read :integral/floating, vectors/list/strings
	template <typename T>
	auto read( int offset =- 1, [[maybe_unused]] int amount =- 1, bool reverse = true ) const -> T
	{
		if( offset < 0 )
		{
			offset = _index;
		}
		if constexpr( std::is_integral_v<T> || std::is_floating_point_v<T> )
		{
			// we ignore amount for integrals and floating point
			auto size = static_cast<int>( sizeof( T ));
			if constexpr( std::is_same_v<T, bool> )
			{
				size = 1;
			}
			if( Exceeds( offset, size ))
			{
				throw ByteBufferBounds_st( offset, size, static_cast<int>( _bytedata.size() ));
			}
			T value( 0 );
			std::copy( _bytedata.data() + offset, _bytedata.data() + offset + size, reinterpret_cast<std::uint8_t*>( &value ));
			if( reverse && ( size > 1 ))
			{
				std::reverse( reinterpret_cast<std::uint8_t*>( &value ), reinterpret_cast<std::uint8_t*>( &value ) + size );
			}
			_index = offset + size;
			return value;
		}
		else if constexpr( std::is_class_v<T> )
		{
			if constexpr( std::is_integral_v<typename T::value_type> )
			{
				// It is a supported container (hopefully) they want back!
				// what is the size of the entry in the container
				auto entry_size = static_cast<int>( sizeof( typename T::value_type ));
				if( amount < 0 )
				{
					// what we do is calcualate how many entries are left in the buffer
					amount = ( static_cast<int>( _bytedata.size() ) - offset ) / entry_size;
				}
				auto requested_size = entry_size * amount;
				// are we exceeding that?
				if( Exceeds( offset, requested_size ))
				{
					throw ByteBufferBounds_st( offset, requested_size, static_cast<int>( _bytedata.size() ));
				}
				// we need to loop through and read a "character" at a time
				typename T::value_type character = 0;
				T rValue;
				
				for( auto i = 0; i < amount; ++i )
				{
					std::copy( _bytedata.data() + offset + i * entry_size, _bytedata.data() + offset + ( i + 1 ) * entry_size, reinterpret_cast<std::uint8_t*>( &character ));
					// should we "reverse it" ?
					if(( entry_size > 1 ) && reverse )
					{
						std::reverse( reinterpret_cast<std::uint8_t*>( &character ), reinterpret_cast<std::uint8_t*>( &character ) + entry_size );
					}
					// If this is a string, we stop at a null terminator.  Do we really want to do this?
					if constexpr( std::is_same_v<std::string, T> || std::is_same_v<std::wstring, T> || std::is_same_v<std::u16string, T> || std::is_same_v<std::u32string, T> )
					{
						if( character == 0 )
						{
							break;
						}
					}
					rValue.push_back( character );
				}
				_index = offset + requested_size;
				return rValue;
			}
		}
	}

	// This reads into the buffer supplied, only for integral types;
	template<typename T>
	auto read( int offset, T *value, int amount = -1, bool reverse = true ) const -> void
	{
		if( offset < 0 )
		{
			offset = _index;
		}
		if constexpr( std::is_integral_v<T> || std::is_floating_point_v<T> )
		{
			auto entry_size = static_cast<int>( sizeof( T ));
			if constexpr( std::is_same_v<T, bool> )
			{
				entry_size = 1;
			}
			auto size = amount * entry_size;
			if( amount < 0 )
			{
				size = entry_size;
				amount = 1;
			}
			if( amount > 0 )
			{
				if( Exceeds( offset, size ))
				{
					throw ByteBufferBounds_st( offset, size, static_cast<int>( _bytedata.size() ));
				}
				// We now get to read
				T input;
				for( auto i = 0; i < amount; ++i )
				{
					std::copy( _bytedata.begin() + offset + ( i * entry_size ), _bytedata.begin() + offset + (( i + 1 ) * entry_size ), reinterpret_cast<std::uint8_t*>( &input ));
					if( reverse )
					{
						std::reverse( reinterpret_cast<std::uint8_t*>( &input ), reinterpret_cast<std::uint8_t*>( &input ) + entry_size );
					}
					// Now put it into the data stream
					std::copy( reinterpret_cast<std::uint8_t*>( &input ), reinterpret_cast<std::uint8_t*>( &input ) + entry_size, value + ( i * entry_size ));
				}
				_index = offset + amount * entry_size;
			}
		}
	}

	template<typename T>
	auto write( int offset, const T *value, int amount =- 1, bool reverse = true, bool expand = true ) -> ByteBuffer_t&
	{
		if( offset < 0 )
		{
			offset = _index;
		}
		if constexpr( std::is_integral_v<T> || std::is_floating_point_v<T> )
		{
			auto entry_size = static_cast<int>( sizeof( T ));
			if constexpr( std::is_same_v<T, bool> )
			{
				entry_size = 1;
			}
			auto size = amount * entry_size;
			if( amount < 0 )
			{
				size = entry_size;
				amount = 1;
			}
			if( amount > 0 )
			{
				if( Exceeds( offset, size, expand ))
				{
					throw ByteBufferBounds_st( offset, size, static_cast<int>( _bytedata.size() ));
				}
				// we need to write it
				for( auto i = 0; i < amount; ++i )
				{
					auto input = value[i];
					if( reverse && ( entry_size > 1 ))
					{
						std::reverse( reinterpret_cast<std::uint8_t*>( &input ), reinterpret_cast<std::uint8_t*>( &input ) + entry_size );
					}
					std::copy( reinterpret_cast<std::uint8_t*>( &input ), reinterpret_cast<std::uint8_t*>( &input ) + entry_size, _bytedata.begin() + offset + ( i * entry_size ));
				}
				_index = offset + ( entry_size * amount );
			}
			return *this;  // we put this here, versue at the end, for we want a compile error if a type not caught with if constexpr. So a return in each if constexpr at the top level
		}
	}

	template<typename T>
	auto write( int offset, const T &value, int amount = -1, bool reverse = true, bool expand = true ) -> ByteBuffer_t&
	{
    (void)amount; // unused variable
		if( offset < 0 )
		{
			offset = _index;
		}

		if constexpr( std::is_integral_v<T> || std::is_floating_point_v<T> )
		{
			// we ignore amount for integrals and floating point
			auto size = static_cast<int>( sizeof( T ));
			if constexpr( std::is_same_v<T, bool> )
			{
				size = 1;
			}
			if( Exceeds( offset, size, expand ))
			{

				throw ByteBufferBounds_st( offset, size, static_cast<int>( _bytedata.size() ));
			}
			// we need to write it
			T temp = value;
			if( reverse )
			{
				std::reverse( reinterpret_cast<std::uint8_t*>( &temp ), reinterpret_cast<std::uint8_t*>( &temp ) + size );
			}
			std::copy( reinterpret_cast<std::uint8_t*>( &temp ), reinterpret_cast<std::uint8_t*>( &temp ) + size, _bytedata.begin() + offset );
			
			_index = offset + size;
			return *this;  // we put this here, versue at the end, for we want a compile error if a type not caught with if constexpr. So a return in each if constexpr at the top level
		}
		else if constexpr( std::is_class_v< T> )
		{
			if constexpr( std::is_integral_v<typename T::value_type> )
			{
				// It is a supported container (hopefully) they want back!
				// what is the size of the entry in the container
				auto entry_size = static_cast<int>( sizeof( typename T::value_type ));
				auto container_size = static_cast<int>( value.size() );
				auto fill_size = 0;
				auto write_size = container_size;
				if( amount < 0 )
				{
					amount = container_size;
				}
				else
				{
					write_size = amount;
					if( amount > container_size )
					{
						fill_size = amount - container_size;
						write_size = container_size;
					}
				}
				auto requested_size = ( write_size + fill_size ) * entry_size;
				// are we exceeding that?
				if( Exceeds( offset, requested_size, expand ))
				{
					throw ByteBufferBounds_st( offset, requested_size, static_cast<int>( _bytedata.size( )));
				}
				// Ok, so now we get to go and do our thing
				for( auto i = 0; i < write_size; ++i )
				{
					auto entry = value[i];
					if( reverse && ( entry_size > 1 ))
					{
						std::reverse( reinterpret_cast<std::uint8_t*>( &entry ), reinterpret_cast<std::uint8_t*>( &entry ) + entry_size );
					}
					std::copy( reinterpret_cast<std::uint8_t*>( &entry ), reinterpret_cast<std::uint8_t*>( &entry ) + entry_size, _bytedata.data() + offset + i * entry_size );
					
				}
				_index = offset + ( write_size *entry_size );
				
				// Now we need to do the fill if anyway
				if( fill_size > 0 )
				{
					std::fill( _bytedata.data() +_index, _bytedata.data() + _index + ( fill_size * entry_size ), 0 );
				}
				_index += fill_size * entry_size;
			}
			return *this;
		}
		else if constexpr( std::is_array_v<T> )
		{
			if constexpr( std::is_integral_v<typename std::remove_extent<T>::type> || std::is_floating_point_v<typename std::remove_extent<T>::type> )
			{
				// It is an array!
				auto entry_size = static_cast<int>( sizeof( typename std::remove_extent<T>::type ));
				if( amount < 0 )
				{
					amount = 1;
				}
				auto requested_size = amount * entry_size;
				
				if( Exceeds( offset, requested_size, expand ))
				{
					throw ByteBufferBounds_st( offset, requested_size, static_cast<int>( _bytedata.size() ));
				}
				// We need to check and loop through if we are reversing;
				
				// we need to write it
				for( auto i = 0; i < amount; ++i )
				{
					auto input = value[i];
					if( reverse && ( entry_size > 1 ))
					{
						std::reverse( reinterpret_cast<std::uint8_t*>( &input ), reinterpret_cast<std::uint8_t*>( &input ) + entry_size );
					}
					std::copy( reinterpret_cast<std::uint8_t*>( &input ), reinterpret_cast<std::uint8_t*>( &input ) + entry_size, _bytedata.begin() + offset + ( i * entry_size ));
				}
				_index = offset + ( entry_size * amount );
				return *this;
			}
		}
	}
};


class socket_error : public std::runtime_error
{
private:
	UI32		errorNum;
public:
	socket_error( const std::string& what_arg );
	socket_error( const UI32 errorNumber );
	socket_error( void );
	UI32		ErrorNumber( void ) const;
	const char *what( void ) const throw();
};

class CPUOXBuffer
{
private:
	std::vector<UI08>		packedBuffer;
	bool					isPacked;
	UI32					packedLength;

protected:
	ByteBuffer_t			pStream;

	virtual void			InternalReset( void );

public:
	CPUOXBuffer();
	virtual					~CPUOXBuffer();
	CPUOXBuffer( CPUOXBuffer *initBuffer );
	CPUOXBuffer &operator=( CPUOXBuffer &copyFrom );

	UI32					Pack( void );
	virtual bool			ClientCanReceive( CSocket *mSock );
	ByteBuffer_t&			GetPacketStream( void );

	UI32					PackedLength( void ) const;
	const UI08 *			PackedPointer( void ) const;

	virtual void			Log( std::ofstream &outStream, bool fullHeader = true );
};

class CPInputBuffer
{
protected:
	CSocket *				tSock;

public:
	CPInputBuffer();
	CPInputBuffer( CSocket *input );
	virtual					~CPInputBuffer()
	{
	}

	virtual void			Receive( void ) = 0;
	virtual void			Log( std::ofstream &outStream, bool fullHeader = true );
	virtual bool			Handle( void );
	void					SetSocket( CSocket *toSet );
	CSocket *				GetSocket( void ) const;
};

class CNetworkStuff
{

public:
	CNetworkStuff();
	~CNetworkStuff();
	auto 		Startup() -> void;
	void		Disconnect( UOXSOCKET s );
	void		Disconnect( CSocket *s );
	void		ClearBuffers( void );
	void		CheckLoginMessage( void );
	void		CheckMessage( void );
	void		SockClose( void );
	void		SetLastOn( CSocket *s );
	CSocket *	GetSockPtr( UOXSOCKET s );
	UOXSOCKET	FindNetworkPtr( CSocket *toFind );

	CSocket *	FirstSocket( void );
	CSocket *	NextSocket( void );
	CSocket *	PrevSocket( void );
	CSocket *	LastSocket( void );
	bool		FinishedSockets( void );

	void		CheckConnections( void );
	void		CheckMessages( void );

	void		Transfer( CSocket *s );

	size_t		PeakConnectionCount( void ) const;

	void		PushConn( void );
	void		PopConn( void );
	void 		pushConn( void );
	void 		popConn( void );

	void		pushLogg();
	void 		popLogg();

	void		PushLogg( void );
	void		PopLogg( void );

	// Login Specific
	void		LoginDisconnect( UOXSOCKET s );
	void		LoginDisconnect( CSocket *s );

	void		RegisterPacket( UI08 packet, UI08 subCmd, UI16 scriptId );

	// We don't want to do this, but given we have outside classes
	// we can either friend a lot of things, or just put it out here
	std::mutex				internallock;
	std::vector<CSocket *>	connClients, loggedInClients;

private:
	struct FirewallEntry_st
	{
		SI16 b[4];
		FirewallEntry_st( SI16 p1, SI16 p2, SI16 p3, SI16 p4 )
		{
			b[0] = p1; b[1] = p2; b[2] = p3; b[3] = p4;
		}
	};

	std::map<UI16, UI16>		packetOverloads;
	std::vector<FirewallEntry_st>	slEntries;
	SI32					a_socket;

	struct sockaddr_in		client_addr;

	size_t					peakConnectionCount;

	std::vector<std::vector<CSocket *>::iterator>	connIteratorBackup, loggIteratorBackup;
	std::vector<CSocket *>::iterator					currConnIter, currLoggIter;

	void		LoadFirewallEntries( void );
	void		GetMsg( UOXSOCKET s );
	void		sockInit( void );
	void		GetLoginMsg( UOXSOCKET s );
	UOXSOCKET	FindLoginPtr( CSocket *s );

	void		CheckConn( void );
	void		LogOut( CSocket *s );

	bool		IsFirewallBlocked( UI08 part[4] );

};

extern CNetworkStuff *Network;

#endif

