#include "uox3.h"

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *FindItem( CChar *toFind, UI08 type )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain type in character's pack
//o---------------------------------------------------------------------------o
CItem *FindItemOfType( CChar *toFind, UI08 type )
{
	for( CItem *toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( toCheck != NULL )
		{
			if( toCheck->GetType() == type )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetLayer() == 0x15 )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				CItem *packSearchResult = SearchSubPackForItemOfType( toCheck, type );
				if( packSearchResult != NULL )
					return packSearchResult;
			}
		}
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *SearchSubPackForItem( CItem *toSearch, UI08 type )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific type
//o---------------------------------------------------------------------------o
CItem *SearchSubPackForItemOfType( CItem *toSearch, UI08 type )
{
	for( CItem *toCheck = toSearch->FirstItemObj(); !toSearch->FinishedItems(); toCheck = toSearch->NextItemObj() )
	{
		if( toCheck != NULL )
		{
			if( toCheck->GetType() == type )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetType() == 1 || toCheck->GetType() == 8 )	// search any subpacks, specifically pack and locked containers
			{ 
				CItem *packSearchResult = SearchSubPackForItem( toCheck, type );
				if( packSearchResult != NULL )
					return packSearchResult;
			}
		}
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	CItem *FindItem( CChar *toFind, UI16 itemID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Look for items of a certain ID in character's pack
//o---------------------------------------------------------------------------o
CItem *FindItem( CChar *toFind, UI16 itemID )
{
	for( CItem *toCheck = toFind->FirstItem(); !toFind->FinishedItems(); toCheck = toFind->NextItem() )
	{
		if( toCheck != NULL )
		{
			if( toCheck->GetID() == itemID )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetLayer() == 0x15 )	// could use packitem, but we're already in the same type of loop, so we'll check it ourselves
			{
				CItem *packSearchResult = SearchSubPackForItem( toCheck, itemID );
				if( packSearchResult != NULL )
					return packSearchResult;
			}
		}
	}
	return NULL;
}

//o---------------------------------------------------------------------------o
//|	Function	-	ITEM SearchSubPackForItem( CItem *toSearch, UI16 itemID )
//|	Programmer	-	Unknown
//o---------------------------------------------------------------------------o
//|	Purpose		-	Search character's subpacks for items of specific ID
//o---------------------------------------------------------------------------o
CItem *SearchSubPackForItem( CItem *toSearch, UI16 itemID )
{
	for( CItem *toCheck = toSearch->FirstItemObj(); !toSearch->FinishedItems(); toCheck = toSearch->NextItemObj() )
	{
		if( toCheck != NULL )
		{
			if( toCheck->GetID() == itemID )	// it's in our hand
				return toCheck;					// we've found the first occurance on the person!
			else if( toCheck->GetType() == 1 || toCheck->GetType() == 8 )	// search any subpacks, specifically pack and locked containers
			{ 
				CItem *packSearchResult = SearchSubPackForItem( toCheck, itemID );
				if( packSearchResult != NULL )
					return packSearchResult;
			}
		}
	}
	return NULL;
}
