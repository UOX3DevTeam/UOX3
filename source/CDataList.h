#if !defined( __CDATALIST_H__ )
#define __CDATALIST_H__

namespace UOX
{

	template < typename T > class CDataList
	{
	protected:
		typedef std::deque< T >				DATALIST;
		typedef std::deque< T >::iterator	DATALIST_ITERATOR;

		DATALIST				objData;
		std::vector< size_t >	objIteratorBackup;
		DATALIST_ITERATOR		objIterator;
	public:
		CDataList()
		{
			objData.resize( 0 );
			objIteratorBackup.resize( 0 );
			objIterator = objData.end();
		}
		~CDataList()
		{
			objData.clear();
			objIteratorBackup.clear();
		}
		inline T GetCurrent( size_t index = 0 ) const
		{
			if( index > 0 && index < objData.size() )
				return objData[index];
			return (*objIterator);
		}
		inline T First( void )
		{
			T rvalue	= NULL;
			objIterator	= objData.begin();
			if( !Finished() )
				rvalue = (*objIterator);
			return rvalue;
		}
		inline T Next( void )
		{
			T rvalue = NULL;
			if( !Finished() )
			{
				++objIterator;
				if( !Finished() )
					rvalue = (*objIterator);
			}
			return rvalue;
		}
		inline bool Finished( void )
		{
			return ( objIterator == objData.end() );
		}
		inline size_t Num( void ) const
		{
			return objData.size();
		}

		inline bool Add( T toAdd )
		{
			DATALIST_ITERATOR iIter;
			for( iIter = objData.begin(); iIter != objData.end(); ++iIter )
			{
				if( (*iIter) == toAdd )
					return false;
			}
			const bool updateCounter = (objIterator == objData.end());
			objData.push_back( toAdd );
			if( updateCounter )
				objIterator = objData.end();
			return true;
		}
		inline bool Remove( T toRemove )
		{
			DATALIST_ITERATOR rIter;
			for( rIter = objData.begin(); rIter != objData.end(); ++rIter )
			{
				T blah = (*rIter);
				if( (*rIter) == toRemove )
				{
					const bool updateCounter = (objIterator != objData.end());
					if( rIter != objData.begin() && rIter <= objIterator )
						--objIterator;

					const size_t iterPos		= (rIter - objData.begin());
					const size_t iCounterPos	= (objIterator - objData.begin());
						
					for( size_t q = 0; q < objIteratorBackup.size(); ++q )
					{
						if( objIteratorBackup[q] > 0 && iterPos <= objIteratorBackup[q] )
							--objIteratorBackup[q];
					}
					objData.erase( rIter );

					if( updateCounter )
						objIterator = (objData.begin() + iCounterPos );
					else
						objIterator = objData.end();
					return true;
				}
			}
			return false;
		}

		inline void Pop( void )
		{
			objIterator = (objData.begin() + objIteratorBackup.back());
			objIteratorBackup.pop_back();
		}
		inline void Push( void )
		{
			objIteratorBackup.push_back( (objIterator - objData.begin()) );
		}
	};

}

#endif // __CDATALIST_H__

