//
//	Module :	necro.cpp
//	Purpose:	store all necromancy related functions
//	Created:	Genesis 11-12-1998
//	History:	None

#include "uox3.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "necro.cpp"

void VialTargetChar( cSocket *nSocket, CChar *trgChar )
{
	CChar *mChar = nSocket->CurrcharObj();
	if( mChar == NULL )
		return;
	CItem *nVialID = nSocket->AddMItem();
	if( nVialID == NULL )
		return;
	CChar *nCharID = trgChar;
	CItem *nDagger = Combat->getWeapon( mChar );
	if( nDagger == NULL ) 
	{
		sysmessage( nSocket, 742 );
		return;
	}
	if( nDagger->GetID() != 0x0F51 && nDagger->GetID() != 0x0F52 )
	{
		sysmessage( nSocket, 743 );
		return;
	}

	nVialID->SetMore( 0, 1 );

	if( nCharID == mChar )
	{
		if( nCharID->GetHP() <= 10 )
		{
			sysmessage( nSocket, 744 );
			return;
		}
		else
		{
			sysmessage( nSocket, 745 );
			nCharID->SetHP( nCharID->GetHP() - ( RandomNum( 0, 5 ) + 2 ) );
			MakeNecroReg( nSocket, nVialID, 0x0E24 );
			return;
		}
	}
	else
	{
		if( objInRange( mChar, nCharID, 2 ) )
		{
			if( nCharID->IsNpc() )
			{
				Karma( mChar, nCharID, -nCharID->GetKarma() );
				if( nCharID->GetID( 1 ) == 0x00 && ( nCharID->GetID( 2 ) == 0x0C ||
					( nCharID->GetID( 2 ) >= 0x3B && nCharID->GetID( 2 ) <= 0x3D ) ) )
					nVialID->SetMore( 1, 1 );
				nCharID->SetHP( nCharID->GetHP() - ( RandomNum( 0, 5 ) + 2 ) );
				MakeNecroReg( nSocket, nVialID, 0x0E24 );
				// Guard be summuned if in town and good npc
				// if good flag criminal
				// if evil npc attack necromancer but don't flag criminal
			}
			else
			{
				Karma( mChar, nCharID, -nCharID->GetKarma() );
				nCharID->SetHP( nCharID->GetHP() - ( rand()%6 + 2 ) );
				sysmessage( calcSocketObjFromChar( nCharID ), 746, mChar->GetName() );
				MakeNecroReg( nSocket, nVialID, 0x0E24 );
				// flag criminal						
			}
		}
		else 
		{
			sysmessage( nSocket, 747 );
			return;
		}
	}
}
void VialTargetItem( cSocket *nSocket, CItem *nItemID )
{
	CChar *mChar = nSocket->CurrcharObj();
	if( mChar == NULL || nItemID == NULL )
		return;
	CItem *nVialID = nSocket->AddMItem();
	if( nVialID == NULL )
		return;

	CItem *nDagger = Combat->getWeapon( mChar );
	if( nDagger == NULL )
	{
		sysmessage( nSocket, 748 );
		return;
	}
	if( !( nDagger->GetID( 1 ) == 0x0F && ( nDagger->GetID( 2 ) == 0x51 || nDagger->GetID( 2 ) == 0x52) ) )
	{
		sysmessage( nSocket, 743 );
		return;
	}

	nVialID->SetMore( 0, 1 );
	if( !nItemID->isCorpse() )
		sysmessage( nSocket, 749 );
	else
	{
		nVialID->SetMore( nItemID->GetMore( 1 ), 1 );
		Karma( mChar, NULL, -1000 );
		if( nItemID->GetMore( 2 ) < 4 )
		{
			sysmessage( nSocket, 750 );
			MakeNecroReg( nSocket, nVialID, 0x0E24 );
			nItemID->SetMore( nItemID->GetMore( 2 ) + 1, 2 );
		}
		else 
			sysmessage( nSocket, 751 );
	}
}

//o--------------------------------------------------------------------------
//|	Function		-	CChar *SpawnRandomMonster( cSocket *nCharID, bool useNecro, char *cList, char *cNpcID )
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-	Changed to FileLookup based and return CChar * 
//|						instead of creature ID (Abaddon)
//o--------------------------------------------------------------------------
//|	Purpose			-	Gets the random monster number from the script and list specified
//|						Creates the creature and returns it
//o--------------------------------------------------------------------------
CChar *SpawnRandomMonster( cSocket *nCharID, bool useNecro, char* cList, char* cNpcID )
{
	char sect[512];
 	sprintf( sect, "%s %s", cList, cNpcID );
	ScriptSection *targData = NULL;
	if( useNecro )
	{
		targData = FileLookup->FindEntry( sect, necro_def );
		if( targData == NULL )
			return NULL;
	}
	else
	{
		targData = FileLookup->FindEntry( sect, npc_def );
		if( targData == NULL )
			return NULL;
	}
	SI32 i = targData->NumEntries();
	CChar *tChar = nCharID->CurrcharObj();
	if( i > 0 )
	{
		i = rand()%(i);
		const char *targID = targData->MoveTo( i );
		if( targID != NULL )
			return Npcs->AddNPC( nCharID, NULL, targID, tChar->WorldNumber() );
	}
	return NULL;
}

//o--------------------------------------------------------------------------
//|	Function		-	CItem *SpawnRandomItem( cSocket *nCharID, bool useNecro, char *cList, char *cItemID )
//|	Date			-	Unknown
//|	Programmer		-	Unknown
//|	Modified		-	Changed to FileLookup based and return CItem * 
//|						instead of creature ID (Abaddon)
//o--------------------------------------------------------------------------
//|	Purpose			-	Gets the random item number from the script and list specified
//|						Creates the item and returns it
//o--------------------------------------------------------------------------
CItem *SpawnRandomItem( cSocket *nCharID, bool useNecro, char* cList, char* cItemID )
{
	char sect[512];
	sprintf( sect, "%s %s", cList, cItemID );
	ScriptSection *randData = NULL;
	if( useNecro )
	{
		randData = FileLookup->FindEntry( sect, necro_def );
		if( randData == NULL )
			return NULL;
	}
	else
	{
		randData = FileLookup->FindEntry( sect, items_def );
		if( randData == NULL )
			return NULL;
	}
	SI32 numEntries = randData->NumEntries();
	if( numEntries > 0 )
	{
		SI32 i = rand()%(numEntries);
		const char *targID = randData->MoveTo( i );
		if( targID != NULL )
			return Items->SpawnItemToPack( nCharID, nCharID->CurrcharObj(), targID, true );
	}
	return NULL;
}

void MakeNecroReg( cSocket *nSocket, CItem *nItem, UI16 itemID )
{
	CItem *iItem = NULL;
	CChar *iCharID = nSocket->CurrcharObj();
	if( itemID >= 0x1B11 && itemID <= 0x1B1C ) // Make bone powder.
	{
		npcEmoteAll( iCharID, 741, true, iCharID->GetName() );
		tempeffect( iCharID, iCharID, 9, 0, 0, 0 );
		tempeffect( iCharID, iCharID, 9, 0, 3, 0 );
		tempeffect( iCharID, iCharID, 9, 0, 6, 0 );
		tempeffect( iCharID, iCharID, 9, 0, 9, 0 );
		iItem = Items->SpawnItem( nSocket, iCharID, 1, "bone powder", true, 0x0F8F, 0, true, true );
		if( iItem == NULL ) 
			return;
		iItem->SetMoreX( 666 );
		iItem->SetMore( 1, 1 ); // this will fill more with info to tell difference between ash and bone
		Items->DeleItem( nItem );
		
	}
	if( itemID == 0x0E24 ) // Make vial of blood.
	{
		if( nItem->GetMore( 1 ) == 1 )
		{
			iItem = Items->SpawnItem( nSocket, iCharID, 1, "#", true, 0x0F82, 0, true, true );
			if( iItem == NULL ) 
				return;
			iItem->SetValue( 15 );
			iItem->SetMoreX( 666 );
		}
		else
		{
			iItem = Items->SpawnItem( nSocket, iCharID, 1, "#", true, 0x0F7D, 0, true, true );
			if( iItem == NULL ) 
				return;
			iItem->SetValue( 10 );
			iItem->SetMoreX( 666 );
		}
		DecreaseItemAmount( nItem );
	}
}

void vialtarget( cSocket *nSocket )
{
	if( nSocket->GetDWord( 7 ) == INVALIDSERIAL )
		return;
	if( nSocket->GetByte( 7 ) >= 0x40 )
	{	// it's an item
		CItem *myItem = calcItemObjFromSer( nSocket->GetDWord( 7 ) );
		VialTargetItem( nSocket, myItem );
	}
	else
	{	// it's a char
		CChar *myChar = calcCharObjFromSer( nSocket->GetDWord( 7 ) );
		VialTargetChar( nSocket, myChar );
	}
}

