#if !defined( __GENERICLIST_H__ )
#define __GENERICLIST_H__
//o-----------------------------------------------------------------------------------------------o
//|	File		-	GenericList.h
//|	Date		-	2021-05-04
//o-----------------------------------------------------------------------------------------------o
//|	Purpose		-	Created as a counterpart of CDataList, based on std::list this class will behave
//|						as expected when removing records in the middle of the array.
//|
//|				-	Version Historyz
//|
//|						1.0		 		2021-05-04
//|						Initial implementation.
//o-----------------------------------------------------------------------------------------------o
#include <iostream>
#include <cstdint>
template < typename T >
class GenericList
{
private:
	typedef std::list< T >								GENERICLIST;
	typedef typename std::list< T >::iterator			GENERICLIST_ITERATOR;
	typedef typename std::list< T >::const_iterator		GENERICLIST_CITERATOR;

	GENERICLIST					objData;
	GENERICLIST_ITERATOR		objIterator;
	std::vector< GENERICLIST_ITERATOR >		objIteratorBackup;

	GENERICLIST_ITERATOR FindEntry(T toFind)
	{
		return std::find(objData.begin(), objData.end(), toFind);
	}

	bool Begin(void)
	{
		return (objIterator == objData.begin());
	}
public:
	GenericList()
	{
		objData.resize(0);
		objIteratorBackup.resize(0);
		objIterator = objData.end();
	}
	~GenericList()
	{
		objData.clear();
		objIteratorBackup.clear();
	}

	T GetCurrent(void)
	{
		T rvalue = NULL;
		if (objIterator != objData.end())
			rvalue = (*objIterator);
		return rvalue;
	}

	T First(void)
	{
		objIterator = objData.end();
		return Next();
	}
	T Next(void)
	{
		T rvalue = NULL;
		if (!Begin())
		{
			--objIterator;
			rvalue = (*objIterator);
		}
		else
			objIterator = objData.end();
		return rvalue;
	}
	bool Finished(void)
	{
		return (objIterator == objData.end());
	}

	size_t Num(void) const
	{
		return objData.size();
	}
	void Clear()
	{
		objData.clear();
	}
	bool Add(T toAdd)
	{
		if (FindEntry(toAdd) != objData.end())
			return false;

		const bool updateCounter = (objIterator == objData.end());
		objData.push_back(toAdd);
		if (updateCounter)
			objIterator = objData.end();

		return true;
	}
	bool Remove(T toRemove, bool handleAlloc = false)
	{
		GENERICLIST_ITERATOR rIter = FindEntry(toRemove);
		if (rIter != objData.end())
		{
			if (objIterator != objData.end() && rIter == objIterator)
			{
				++objIterator;
			}

			for (size_t q = 0; q < objIteratorBackup.size(); ++q)
			{
				if (objIteratorBackup[q] != objData.end() && rIter == objIteratorBackup[q]) {
					++objIteratorBackup[q];
				}
			}
			objData.erase(rIter);

			if (handleAlloc) {
				delete toRemove;
			}
			return true;
		}
		return false;
	}

	void Pop(void)
	{
		if (!objIteratorBackup.empty()) {
			objIterator = objIteratorBackup.back();

			objIteratorBackup.pop_back();
		}
		else {
			objIterator = objData.end();
		}
	}
	void Push(void)
	{
		objIteratorBackup.push_back(objIterator);

	}
	void Sort(void)
	{
		std::sort(objData.begin(), objData.end());

	}
	void Sort(bool Comparer(T one, T two))
	{
		std::sort(objData.begin(), objData.end(), Comparer);
	}
};



#endif // __CDATALIST_H__

