#if !defined( __CDATALIST_H__ )
#define __CDATALIST_H__
//o--------------------------------------------------------------------------o
//|	File			-	CDataList.h
//|	Date			-	8/1/2004
//|	Developers		-	giwo
//|	Organization	-	UOX3 DevTeam
//|	Status			-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	This is a wrapper for std::deque. Its purpose is to allow us
//|						to safely create, iterate (allowing nested iterations), and
//|						maintain a randomly-accessible list containing any type of object.
//|
//|					-	Version History
//|									
//|							1.0		giwo		1st Aug, 2004
//|							Initial implementation. Basic routines, First(), Next(), Finished(),
//|							along with safe Add() and Remove() functions. Also can Push() or Pop()
//|							our iterator backup list.
//|
//|							1.1		giwo		12th Aug, 2004
//|							Added an accessor to return the size of the container, also can now
//|							grab a specific object from the array (or the one our iterator is currently
//|							at). Added a FindEntry() function to simplify some internal code.							
//|									
//o--------------------------------------------------------------------------o

namespace UOX
{

	template < typename T > 
	class CDataList
	{
	protected:
		typedef std::deque< T >						DATALIST;
		typedef typename std::deque< T >::iterator	DATALIST_ITERATOR;

		DATALIST					objData;
		DATALIST_ITERATOR			objIterator;
		std::vector< size_t >		objIteratorBackup;

		DATALIST_ITERATOR FindEntry( T toFind )
		{
			DATALIST_ITERATOR fIter = objData.end();
			for( fIter = objData.begin(); fIter != objData.end(); ++fIter )
			{
				if( (*fIter) == toFind )
					break;
			}
			return fIter;
		}
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

		T GetCurrent( size_t index = 0 ) const
		{
			if( index > 0 && index < objData.size() )
				return objData[index];
			return (*objIterator);
		}

		T First( void )
		{
			T rvalue	= NULL;
			objIterator	= objData.begin();
			if( !Finished() )
				rvalue = (*objIterator);
			return rvalue;
		}
		T Next( void )
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
		bool Finished( void )
		{
			return ( objIterator == objData.end() );
		}

		size_t Num( void ) const
		{
			return objData.size();
		}

		bool Add( T toAdd )
		{
			if( FindEntry( toAdd ) != objData.end() )
				return false;

			const bool updateCounter = (objIterator == objData.end());
			objData.push_back( toAdd );
			if( updateCounter )
				objIterator = objData.end();
			return true;
		}
		bool Remove( T toRemove )
		{
			DATALIST_ITERATOR rIter = FindEntry( toRemove );
			if( rIter != objData.end() )
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
			return false;
		}

		void Pop( void )
		{
			objIterator = (objData.begin() + objIteratorBackup.back());
			objIteratorBackup.pop_back();
		}
		void Push( void )
		{
			objIteratorBackup.push_back( (objIterator - objData.begin()) );
		}
	};

}

#endif // __CDATALIST_H__

