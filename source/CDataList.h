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
//|							1.2		giwo		21st Nov, 2004
//|							Found a pretty substantial flaw causing the first object in a list to be
//|							overlooked when the entire list was being emptied (IE at house deletion).
//|							Overhauled First/Next/Finished and Added Begin to allow iteration to work
//|							backwards thus resolving the issue.
//o--------------------------------------------------------------------------o

namespace UOX
{

	template < typename T > 
	class CDataList
	{
	private:
		typedef std::deque< T >								DATALIST;
		typedef typename std::deque< T >::iterator			DATALIST_ITERATOR;
		typedef typename std::deque< T >::const_iterator	DATALIST_CITERATOR;

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

		bool Begin( void )
		{
			return ( objIterator == objData.begin() );
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

		T GetCurrent( void )
		{
			T rvalue = NULL;
			if( objIterator != objData.end() )
				rvalue = (*objIterator);
			return rvalue;
		}

		T First( void )
		{
			objIterator	= objData.end();
			return Next();
		}
		T Next( void )
		{
			T rvalue = NULL;
			if( !Begin() )
			{
				--objIterator;
				rvalue = (*objIterator);
			}
			else
				objIterator = objData.end();
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
		bool Remove( T toRemove, bool handleAlloc = false )
		{
			DATALIST_ITERATOR rIter = FindEntry( toRemove );
			if( rIter != objData.end() )
			{
				const bool updateCounter = (objIterator != objData.end());
				if( rIter != objData.begin() && rIter < objIterator )
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

				if( handleAlloc )
					delete toRemove;
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
		void Sort( void )
		{
			std::sort( objData.begin(), objData.end() );
		}
		void Sort( bool Comparer( T one, T two ) )
		{
			std::sort( objData.begin(), objData.end(), Comparer );
		}
	};

}

#endif // __CDATALIST_H__

