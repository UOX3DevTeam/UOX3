#include "uox3.h"
#include "power.h"

#include "stream.h"

// New class written based upon old UOX char_st.  Number of old members removed
// and a number of members types modified as well

// Version History
// 1.0		Abaddon		29th March, 2000
//			Initial implementation, all get/set mechanisms written up to
//			the end of SetSayColour().  Bools reduced down to a single char
//			with get/set mechanisms for setting/clearings bits
//			No documentation currently done, all functions obvious so far
//			Note:  individual byte setting within SI32s/SI08s need speeding up
//			consider memcpy SI32o correct word, note that this will be endian specific!
// 1.1		Abaddon		2nd April, 2000
//			Finished up the initial implementation on all classes. Fixed a minor bit shift error
//			on some of the char variables, all working fine now
// 1.2		Abaddon		25 July, 2000
//			All Accessor funcs plus a few others are now flagged as const functions, meaning that
//			those functions GUARENTEE never to alter data, at compile time
//			Thoughts about cBaseObject and prelim plans made
// 1.3		Abaddon		28 July, 2000
//			Initial cBaseObject implementation put in.  CChar reworked to deal with only things it has to
//			Proper constructor written
//			Plans for CChar derived objects thought upon (notably CPC and CNPC)

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetNPCAiType( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the ai type of the NPC
//o---------------------------------------------------------------------------o
SI16 CChar::GetNPCAiType( void ) const
{
	return npcaitype;
}

SERIAL CChar::GetGuarding( void ) const
{
	return petguarding;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetWeight( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the weight of the CHARACTER
//o---------------------------------------------------------------------------o
SI16 CChar::GetWeight( void ) const
{
	return weight;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SERIAL GetMaking( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the making attribute.  Used for making items
//o---------------------------------------------------------------------------o
SERIAL CChar::GetMaking( void ) const
{
	return making;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetTaming( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the minimum skill required to tame it
//o---------------------------------------------------------------------------o
SI16 CChar::GetTaming( void ) const
{
	return taming;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetHidden( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the visibility flag for the CHARACTER
//|						0 - Visible
//|						1 - Hidden
//|						2 - Magically Invisible
//o---------------------------------------------------------------------------o
SI08 CChar::GetHidden( void ) const
{
	return cBaseObject::GetVisible();
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetHunger( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the hunger level of the CHARACTER
//o---------------------------------------------------------------------------o
SI08 CChar::GetHunger( void ) const
{
	return hunger;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 IsBlocked( void ) const
//|   Date        -  Unknown 
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the blocked status of the CHARACTER
//o---------------------------------------------------------------------------o
SI08 CChar::IsBlocked( void ) const
{
	return blocked;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 GetFixedLight( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the fixed light level of the CHARACTER
//|                  255 is not fixed
//o---------------------------------------------------------------------------o
UI08 CChar::GetFixedLight( void ) const
{
	return fixedlight;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 GetTown( void ) const 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the town the CHARACTER beSI32s to
//o---------------------------------------------------------------------------o
UI08 CChar::GetTown( void ) const
{
	 return town;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 GetMed( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the state of meditation of the CHARACTER
//o---------------------------------------------------------------------------o
UI08 CChar::GetMed( void ) const
{
	return med;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 GetTrainingPlayerIn( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the skill the player is being trained in. 255 is no training
//o---------------------------------------------------------------------------o
UI08 CChar::GetTrainingPlayerIn( void ) const
{
	return trainingplayerin;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI32 GetAccount( void )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the account associated with the CHARACTER. -1 is for NPCs
//o---------------------------------------------------------------------------o
SI32 CChar::GetAccount( void ) const
{
	return account;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI32 GetCarve( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  
//|                  
//o---------------------------------------------------------------------------o
SI32 CChar::GetCarve( void ) const
{
	return carve;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI32 GetHoldG( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the amount of gold being held by a Player Vendor
//o---------------------------------------------------------------------------o
UI32 CChar::GetHoldG( void ) const
{
	return holdg;
}

//o--------------------------------------------------------------------------o
//|	Function			-	void CChar::SetAccount( SI32 newVal )
//|	Date					-	1/14/2003 6:17:45 AM
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Set the account object for this character to the specified
//|									account block. This functuo uses the accounts ID to link
//|									the character to the account.
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
void CChar::SetAccount(SI16 wAccountID)
{
	if(!Accounts->GetAccountByID(wAccountID,(ACCOUNTSBLOCK&)ourAccount))
	{
		// Ok there was an error setting an account to this character.
		account=0xffff;
		ourAccount.wAccountIndex=0xffff;
		return;
	}
	account = ourAccount.wAccountIndex;
}
//
void CChar::SetAccount(std::string sUsername)
{
	if(!Accounts->GetAccountByName(sUsername,(ACCOUNTSBLOCK&)ourAccount))
	{
		// Ok there was an error setting an account to this character.
		account=0xffff;
		ourAccount.wAccountIndex=0xffff;
		return;
	}
	account = ourAccount.wAccountIndex;
}
//
void CChar::SetAccount(ACCOUNTSBLOCK &actbAccount)
{
	if(actbAccount.wAccountIndex==0xffff)
	{
		account=0xffff;
		return;
	}
	ourAccount = actbAccount;
	account=ourAccount.wAccountIndex;
}
//

//o---------------------------------------------------------------------------o
//|   Function    -  void SetWeight( SI08 newVal )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the weight of the CHARACTER
//o---------------------------------------------------------------------------o
void CChar::SetWeight( SI16 newVal )
{
	weight = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFixedLight( UI08 newLight )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the fixed light level of the CHARACTER.  255 turns it off
//o---------------------------------------------------------------------------o
void CChar::SetFixedLight( UI08 newVal )
{
	fixedlight = newVal;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetHidden( SI08 newVisibility )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the visibility of the CHARACTER
//|						0 - visible
//|						1 - hidden/stealth
//|						2 - magically invisible
//o---------------------------------------------------------------------------o
void CChar::SetHidden( SI08 newValue )
{
	SetVisible( newValue );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetHunger( SI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's hunger value
//o---------------------------------------------------------------------------o
void CChar::SetHunger( SI08 newValue )
{
	hunger = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetNPCAiType( SI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's AI type to newValue
//o---------------------------------------------------------------------------o
void CChar::SetNPCAiType( SI16 newValue )
{
	npcaitype = newValue;
}

void CChar::SetGuarding( SERIAL newValue )
{
	petguarding = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetMaking( SERIAL newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's making value
//o---------------------------------------------------------------------------o
void CChar::SetMaking( SERIAL newValue )
{
	making = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetBlocked( SI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's blocked value
//o---------------------------------------------------------------------------o
void CChar::SetBlocked( SI08 newValue )
{
	blocked = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetTaming( SI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the minimum skill required to tame it
//o---------------------------------------------------------------------------o
void CChar::SetTaming( SI16 newValue )
{
	taming = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetTown( UI08 newTown )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the new town value of the CHARACTER
//o---------------------------------------------------------------------------o
void CChar::SetTown( UI08 newValue )
{
	town = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetMed( UI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's meditation value
//o---------------------------------------------------------------------------o
void CChar::SetMed( UI08 newValue )
{
	med = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetHoldG( UI32 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's held gold value
//o---------------------------------------------------------------------------o
void CChar::SetHoldG( UI32 newValue )
{
	holdg = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetCarve( SI32 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's carve value
//o---------------------------------------------------------------------------o
void CChar::SetCarve( SI32 newValue )
{
	carve = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetTrainingPlayerIn( UI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the skill that the player is being trained in
//o---------------------------------------------------------------------------o
void CChar::SetTrainingPlayerIn( UI08 newValue )
{
	trainingplayerin = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 GetSplit( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the split level of the CHARACTER
//o---------------------------------------------------------------------------o
UI08 CChar::GetSplit( void ) const
{
	return split;
}

//o---------------------------------------------------------------------------o
//|   Function    -  UI08 GetSplitChance( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the chance of an NPC splitting
//o---------------------------------------------------------------------------o
UI08 CChar::GetSplitChance( void ) const
{
	return splitchnc;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetSplit( UI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the split value of the CHARACTER
//o---------------------------------------------------------------------------o
void CChar::SetSplit( UI08 newValue )
{
	split = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetSplitChance( UI08 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the chance for an npc to split
//o---------------------------------------------------------------------------o
void CChar::SetSplitChance( UI08 newValue )
{
	splitchnc = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool isFree( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is free
//o---------------------------------------------------------------------------o
bool CChar::isFree( void ) const
{
	return ( (bools&0x01) == 0x01 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool isUnicode( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is speaking in unicode
//o---------------------------------------------------------------------------o
bool CChar::isUnicode( void ) const
{
	return ( (bools&0x02) == 0x02 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool isNpc( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is an npc
//o---------------------------------------------------------------------------o
bool CChar::IsNpc( void ) const
{
	return ( (bools&0x04) == 0x04 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsShop( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is a shopkeeper
//o---------------------------------------------------------------------------o
bool CChar::IsShop( void ) const
{
	return ( (bools&0x08) == 0x08 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsDead( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is dead
//o---------------------------------------------------------------------------o
bool CChar::IsDead( void ) const
{
	return ( (bools&0x10) == 0x10 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsAtWar( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is at war
//o---------------------------------------------------------------------------o
bool CChar::IsAtWar( void ) const
{
	return ( (bools&0x20) == 0x20 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool DidAttackFirst( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER attacked first
//o---------------------------------------------------------------------------o
bool CChar::DidAttackFirst( void ) const
{
	return ( (bools&0x40) == 0x40 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsOnHorse( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is on a horse
//o---------------------------------------------------------------------------o
bool CChar::IsOnHorse( void ) const
{
	return ( (bools&0x80) == 0x80 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetTownTitle( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER's town info is displayed
//o---------------------------------------------------------------------------o
bool CChar::GetTownTitle( void ) const
{
	return ( (bools&0x100) == 0x100 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetReactiveArmour( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER has reactive armour
//o---------------------------------------------------------------------------o
bool CChar::GetReactiveArmour( void ) const
{
	return ( (bools&0x200) == 0x200 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool CanTrain( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is can train
//o---------------------------------------------------------------------------o
bool CChar::CanTrain( void ) const
{
	return ( (bools&0x400) == 0x400 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool GetGuildToggle( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER displays guild information
//o---------------------------------------------------------------------------o
bool CChar::GetGuildToggle( void ) const
{
	return ( (bools&0x800) == 0x800 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsTamed( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is tamed
//o---------------------------------------------------------------------------o
bool CChar::IsTamed( void ) const
{
	return ( (bools&0x1000) == 0x1000 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsGuarded( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is guarded
//o---------------------------------------------------------------------------o
bool CChar::IsGuarded( void ) const
{
	return ( (bools&0x2000) == 0x2000 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool CanRun( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER can run
//o---------------------------------------------------------------------------o
bool CChar::CanRun( void ) const
{
	return ( (bools&0x4000) == 0x4000 );
}

bool CChar::IsPolymorphed( void ) const
{
	return ( (bools&0x10000) == 0x10000 );
}
bool CChar::IsIncognito( void ) const
{
	return ( (bools&0x20000) == 0x20000 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void setFree( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's free status
//o---------------------------------------------------------------------------o
void CChar::setFree( bool newVal )
{
	if( newVal )
		bools |= 0x0001;
	else
		bools &= ~0x0001;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void setUnicode( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's unicode speech status
//o---------------------------------------------------------------------------o
void CChar::setUnicode( bool newVal )
{
	if( newVal )
		bools |= 0x0002;
	else
		bools &= ~0x0002;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetNpc( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's npc status
//o---------------------------------------------------------------------------o
void CChar::SetNpc( bool newVal )
{
	if( newVal )
		bools |= 0x0004;
	else
		bools &= ~0x0004;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetShop( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's shopkeeper status
//o---------------------------------------------------------------------------o
void CChar::SetShop( bool newVal )
{
	if( newVal )
		bools |= 0x0008;
	else
		bools &= ~0x0008;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetDead( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's dead status
//o---------------------------------------------------------------------------o
void CChar::SetDead( bool newValue )
{
	if( newValue )
		bools |= 0x0010;
	else
		bools &= ~0x0010;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetWar( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's at war status
//o---------------------------------------------------------------------------o
void CChar::SetWar( bool newValue )
{
	if( newValue )
		bools |= 0x0020;
	else
		bools &= ~0x0020;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetAttackFirst( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the CHARACTER attacked first
//o---------------------------------------------------------------------------o
void CChar::SetAttackFirst( bool newValue )
{
	if( newValue )
		bools |= 0x0040;
	else
		bools &= ~0x0040;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetOnHorse( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the CHARACTER is on a horse or not
//o---------------------------------------------------------------------------o
void CChar::SetOnHorse( bool newValue )
{
	if( newValue )
		bools |= 0x0080;
	else
		bools &= ~0x0080;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetTownTitle( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's town title display status
//o---------------------------------------------------------------------------o
void CChar::SetTownTitle( bool newValue )
{
	if( newValue )
		bools |= 0x0100;
	else
		bools &= ~0x0100;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetReactiveArmour( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the CHARACTER has reactive armour
//o---------------------------------------------------------------------------o
void CChar::SetReactiveArmour( bool newValue )
{
	if( newValue )
		bools |= 0x0200;
	else
		bools &= ~0x0200;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetCanTrain( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's trainer status
//o---------------------------------------------------------------------------o
void CChar::SetCanTrain( bool newValue )
{
	if( newValue )
		bools |= 0x0400;
	else
		bools &= ~0x0400;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetGuildToggle( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's guild display status
//o---------------------------------------------------------------------------o
void CChar::SetGuildToggle( bool newValue )
{
	if( newValue )
		bools |= 0x0800;
	else
		bools &= ~0x0800;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetTamed( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's tamed status
//o---------------------------------------------------------------------------o
void CChar::SetTamed( bool newValue )
{
	if( newValue )
		bools |= 0x1000;
	else
		bools &= ~0x1000;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetGuarded( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's guarded status
//o---------------------------------------------------------------------------o
void CChar::SetGuarded( bool newValue )
{
	if( newValue )
		bools |= 0x2000;
	else
		bools &= ~0x2000;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetRun( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the CHARACTER can run
//o---------------------------------------------------------------------------o
void CChar::SetRun( bool newValue )
{
	if( newValue )
		bools |= 0x4000;
	else
		bools &= ~0x4000;
}

void CChar::IsPolymorphed( bool newValue )
{
	if( newValue )
		bools |= 0x10000;
	else
		bools &= ~0x10000;
}

void CChar::IsIncognito( bool newValue )
{
	if( newValue )
		bools |= 0x20000;
	else
		bools &= ~0x20000;
}

void CChar::SetSerial( SERIAL newSerial, CHARACTER c )
{
	ncharsp.Remove( newSerial, c );
	cBaseObject::SetSerial( newSerial );
	if( c != INVALIDSERIAL && newSerial != INVALIDSERIAL )
		ncharsp.AddSerial( newSerial, c );
}

void CChar::SetRobe( UI32 newValue )
{
	robe = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RemoveSelfFromOwner( void ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Removes CHARACTER from it's owner's pet list
//o---------------------------------------------------------------------------o
void CChar::RemoveSelfFromOwner( void )
{
	cBaseObject *oldOwner = GetOwnerObj();
	if( oldOwner != NULL )
	{
		if( oldOwner->GetObjType() == OT_CHAR )
			((CChar *)oldOwner)->RemovePet( this );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddSelfToOwner( void ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds CHARACTER to it's new owner's pet list
//o---------------------------------------------------------------------------o
void CChar::AddSelfToOwner( void )
{
	cBaseObject *newOwner = GetOwnerObj();
	if( newOwner == NULL || newOwner->GetObjType() != OT_CHAR )
		SetTamed( false );
	else
	{
		((CChar *)newOwner)->AddPet( this );
		SetTamed( true );
	}
}

void CChar::SetOwner( UI32 newValue )
{
	RemoveSelfFromOwner();
	cBaseObject::SetOwner( newValue );
	AddSelfToOwner();
}

void CChar::SetSpawn( UI08 newVal, UI08 part, CHARACTER c )
{
	if( GetSpawn() != 0 )
		ncspawnsp.Remove( GetSpawn(), c );
	cBaseObject::SetSpawn( newVal, part );
	if( GetSpawn() != 0 ) 
		ncspawnsp.AddSerial( GetSpawn(), c );
}

void CChar::SetSpawn( UI32 newValue, CHARACTER c )
{
	if( GetSpawn() != 0 )
		ncspawnsp.Remove( GetSpawn(), c );
	cBaseObject::SetSpawn( newValue );
	if( GetSpawn() != 0 ) 
		ncspawnsp.AddSerial( GetSpawn(), c );
}
void CChar::SetTownVote( UI08 newValue, UI08 part )
{
	UI08 part1 = (UI08)(townvote>>24);
	UI08 part2 = (UI08)(townvote>>16);
	UI08 part3 = (UI08)(townvote>>8);
	UI08 part4 = (UI08)(townvote%256);
	switch( part )
	{
	case 1:		part1 = newValue;		break;
	case 2:		part2 = newValue;		break;
	case 3:		part3 = newValue;		break;
	case 4:		part4 = newValue;		break;
	}
	townvote = (part1<<24) + (part2<<16) + (part3<<8) + part4;
}

void CChar::SetTownVote( UI32 newValue )
{
	townvote = newValue;
}

void CChar::SetTrainer( UI32 newValue )
{
	trainer = newValue;
}
void CChar::SetGuildFealty( UI32 newValue )
{
	guildfealty = newValue;
}
void CChar::SetPoisonSerial( UI32 newValue )
{
	poisonserial = newValue;
}

UI32 CChar::GetRobe( void ) const
{
	return robe;
}
UI32 CChar::GetTownVote( void ) const
{
	return townvote;
}
UI32 CChar::GetTrainer( void ) const
{
	return trainer;
}
UI32 CChar::GetGuildFealty( void ) const
{
	return guildfealty;
}
UI32 CChar::GetPoisonSerial( void ) const
{
	return poisonserial;
}
UI08 CChar::GetRobe( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(robe>>24);
	case 2:		return (UI08)(robe>>16);
	case 3:		return (UI08)(robe>>8);
	case 4:
	default:	return (UI08)(robe%256);
	}
	return (UI08)(robe%256);
}
UI08 CChar::GetTownVote( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(townvote>>24);
	case 2:		return (UI08)(townvote>>16);
	case 3:		return (UI08)(townvote>>8);
	case 4:
	default:	return (UI08)(townvote%256);
	}
	return (UI08)(townvote%256);
}
const char *CChar::GetOrgName( void ) const
{
	return orgname;
}
const char *CChar::GetLastOn( void ) const
{
	return laston;
}
const char *CChar::GetGuildTitle( void ) const
{
	return guildtitle;
}

void CChar::SetOrgName( const char *newName )
{
	strncpy( orgname, newName, MAX_NAME );
}
void CChar::SetLastOn( const char *newValue )
{
	strncpy( laston, newValue, MAX_LASTON );
}
void CChar::SetGuildTitle( const char *newValue )
{
	strncpy( guildtitle, newValue, MAX_GUILDTITLE );
}

SI32 CChar::GetTimeout( void ) const
{
	return timeout;
}
SI32 CChar::GetLogout( void ) const
{
	return logout;
}
UI32 CChar::GetRegen( UI08 part ) const
{
	return regen[part];
}
UI32 CChar::GetNpcMoveTime( void ) const
{
	return npcmovetime;
}
UI32 CChar::GetInvisTimeout( void ) const
{
	return invistimeout;
}
UI32 CChar::GetHungerTime( void ) const
{
	return hungertime;
}
UI32 CChar::GetSpiritSpeakTimer( void ) const
{
	return spiritspeaktimer;
}
UI32 CChar::GetSpaTimer( void ) const
{
	return spatimer;
}
UI32 CChar::GetSummonTimer( void ) const
{
	return summontimer;
}
UI32 CChar::GetTrackingTimer( void ) const
{
	return trackingtimer;
}
UI32 CChar::GetFishingTimer( void ) const
{
	return fishingtimer;
}
UI32 CChar::GetPoisonTime( void ) const
{
	return poisontime;
}
UI32 CChar::GetPoisonTextTime( void ) const
{
	return poisontxt;
}
UI32 CChar::GetPoisonWearOffTime( void ) const
{
	return poisonwearofftime;
}
SI32 CChar::GetSpellTime( void ) const
{
	return spelltime;
}
UI32 CChar::GetNextAct( void ) const
{
	return nextact;
}
UI32 CChar::GetSmokeTimer( void ) const
{
	return smoketimer;
}
UI32 CChar::GetSmokeDisplayTimer( void ) const
{
	return smokedisplaytimer;
}
UI32 CChar::GetAntiSpamTimer( void ) const
{
	return antispamtimer;
}
UI32 CChar::GetSkillDelay( void ) const
{
	return skilldelay;
}
SI32 CChar::GetCrimFlag( void ) const
{
	return crimflag;
}
SI32 CChar::GetMuteTime( void ) const
{
	return mutetime;
}
SI32 CChar::GetObjectDelay( void ) const
{
	return objectdelay;
}
TIMERVAL CChar::GetWeathDamage( UI08 part ) const
{
	return weathDamage[part];
}
SI32 CChar::GetMurderRate( void ) const
{
	return murderrate;
}
UI32 CChar::GetTrackingDisplayTimer( void ) const
{
	return trackingdisplaytimer;
}

void CChar::SetTimeout( SI32 newValue )
{
	timeout = newValue;
}
void CChar::SetRegen( UI32 newValue, UI08 part )
{
	regen[part] = newValue;
}
void CChar::SetNpcMoveTime( UI32 newValue )
{
	npcmovetime = newValue;
}
void CChar::SetInvisTimeout( SI32 newValue )
{
	invistimeout = newValue;
}
void CChar::SetHungerTime( SI32 newValue )
{
	hungertime = newValue;
}
void CChar::SetSpiritSpeakTimer( UI32 newValue )
{
	spiritspeaktimer = newValue;
}
void CChar::SetSpaTimer( UI32 newValue )
{
	spatimer = newValue;
}
void CChar::SetSummonTimer( UI32 newValue )
{
	summontimer = newValue;
}
void CChar::SetTrackingTimer( UI32 newValue )
{
	trackingtimer = newValue;
}
void CChar::SetFishingTimer( UI32 newValue )
{
	fishingtimer = newValue;
}
void CChar::SetPoisonTime( UI32 newValue )
{
	poisontime = newValue;
}
void CChar::SetPoisonTextTime( UI32 newValue )
{
	poisontxt = newValue;
}
void CChar::SetPoisonWearOffTime( UI32 newValue )
{
	poisonwearofftime = newValue;
}
void CChar::SetCrimFlag( SI32 newValue )
{
	crimflag = newValue;
}
void CChar::SetSpellTime( UI32 newValue )
{
	spelltime = newValue;
}
void CChar::SetNextAct( UI32 newValue )
{
	nextact = newValue;
}
void CChar::SetMuteTime( SI32 newValue )
{
	mutetime = newValue;
}
void CChar::SetLogout( SI32 newValue )
{
	logout = newValue;
}
void CChar::SetWeathDamage( TIMERVAL newValue, UI08 part )
{
	weathDamage[part] = newValue;
}
void CChar::SetSmokeTimer( UI32 newValue )
{
	smoketimer = newValue;
}
void CChar::SetSmokeDisplayTimer( UI32 newValue )
{
	smokedisplaytimer = newValue;
}
void CChar::SetAntiSpamTimer( UI32 newValue )
{
	antispamtimer = newValue;
}
void CChar::SetSkillDelay( UI32 newValue )
{
	skilldelay = newValue;
}
void CChar::SetObjectDelay( SI32 newValue )
{
	objectdelay = newValue;
}
void CChar::SetMurderRate( SI32 newValue )
{
	murderrate = newValue;
}
void CChar::SetTrackingDisplayTimer( UI32 newValue )
{
	trackingdisplaytimer = newValue;
}

UI08 CChar::GetID( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(id>>8);
	case 2:
	default:	return (UI08)(id%256);
	}
	return (UI08)(id%256);
}
UI08 CChar::GetxID( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(xid>>8);
	case 2:
	default:	return (UI08)(xid%256);
	}
	return (UI08)(xid%256);
}
UI08 CChar::GetOrgID( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(orgid>>8);
	case 2:
	default:	return (UI08)(orgid%256);
	}
	return (UI08)(orgid%256);
}
UI08 CChar::GetHairStyle( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(hairstyle>>8);
	case 2:
	default:	return (UI08)(hairstyle%256);
	}
	return (UI08)(hairstyle%256);
}
UI08 CChar::GetBeardStyle( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(beardstyle>>8);
	case 2:
	default:	return (UI08)(beardstyle%256);
	}
	return (UI08)(beardstyle%256);
}
UI08 CChar::GetEnvokeID( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(envokeid>>8);
	case 2:
	default:	return (UI08)(envokeid%256);
	}
	return (UI08)(envokeid%256);
}

UI16 CChar::GetEnvokeID( void ) const
{
	return envokeid;
}
UI16 CChar::GetID( void ) const
{
	return id;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	UI16 CChar::GetID2( void ) const
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Get the value contained in the member variable
//o--------------------------------------------------------------------------o
//|	Returns				-	[UI16] containing the stored member value
//o--------------------------------------------------------------------------o
UI16 CChar::GetID2( void ) const
{
	return ID2;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
UI16 CChar::GetSkin2( void ) const
{
	return Skin2;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
UI16 CChar::GetStamina( void ) const
{
	return Stamina;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
//UI16 CChar::GetMana( void ) const
//{
//	return Mana;
//}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
UI16 CChar::GetNoMove( void ) const
{
	return Nomove;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
UI16 CChar::GetPoisonChance( void ) const
{
	return PoisonChance;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
UI16 CChar::GetPoisonStrength( void ) const
{
	return PoisonStrength;
}

UI16 CChar::GetxID( void ) const
{
	return xid;
}
UI16 CChar::GetOrgID( void ) const
{
	return orgid;
}
UI16 CChar::GetHairStyle( void ) const
{
	return hairstyle;
}
UI16 CChar::GetBeardStyle( void ) const
{
	return beardstyle;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	void CChar::GetID2( UI16 value ) 
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	Set the value contained in the member variable
//o--------------------------------------------------------------------------o
void CChar::SetID2( UI16 value )
{
	ID2=value;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
void CChar::SetSkin2( UI16 value )
{
	Skin2=value;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
void CChar::SetStamina( UI16 value )
{
	Stamina=value;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//void CChar::SetMana( UI16 value) const
//{
//	Mana=value;
//}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
void CChar::SetNoMove( UI16 value )
{
	Nomove=value;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
void CChar::SetPoisonChance( UI16 value ) 
{
	PoisonChance=value;
}

//o--------------------------------------------------------------------------o
//|	Function/Class-	
//|	Date					-	09/23/2002
//|	Developer(s)	-	EviLDeD
//|	Company/Team	-	UOX3 DevTeam
//|	Status				-	
//o--------------------------------------------------------------------------o
//|	Description		-	
//o--------------------------------------------------------------------------o
//|	Returns				-
//o--------------------------------------------------------------------------o
//|	Notes					-	
//o--------------------------------------------------------------------------o	
void CChar::SetPoisonStrength( UI16 value ) 
{
	PoisonStrength=value;
}

void CChar::SetxID( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(xid>>8);
	UI08 part2 = (UI08)(xid%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	xid = (UI16)((part1<<8) + part2);
}
void CChar::SetxID( UI16 value )
{
	xid = value;
}
void CChar::SetOrgID( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(orgid>>8);
	UI08 part2 = (UI08)(orgid%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	orgid = (UI16)((part1<<8) + part2);
}
void CChar::SetOrgID( UI16 value )
{
	orgid = value;
}
void CChar::SetHairStyle( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(hairstyle>>8);
	UI08 part2 = (UI08)(hairstyle%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	hairstyle = (UI16)((part1<<8) + part2);
}
void CChar::SetHairStyle( UI16 value )
{
	hairstyle = value;
}
void CChar::SetBeardStyle( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(beardstyle>>8);
	UI08 part2 = (UI08)(beardstyle%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	beardstyle = (UI16)((part1<<8) + part2);
}
void CChar::SetBeardStyle( UI16 value )
{
	beardstyle = value;
}
void CChar::SetEnvokeID( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(envokeid>>8);
	UI08 part2 = (UI08)(envokeid%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	envokeid = (UI16)((part1<<8) + part2);
}
void CChar::SetEnvokeID( UI16 newValue )
{
	envokeid = newValue;
}

UI08 CChar::GetSkin( UI08 part ) const
{
	return GetColour( part );
}
UI08 CChar::GetOrgSkin( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(orgSkin>>8);
	case 2:
	default:	return (UI08)(orgSkin%256);
	}
	return (UI08)(orgSkin%256);
}
UI08 CChar::GetxSkin( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(xskin>>8);
	case 2:
	default:	return (UI08)(xskin%256);
	}
	return (UI08)(xskin%256);
}
UI08 CChar::GetHairColour( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(haircolor>>8);
	case 2:
	default:	return (UI08)(haircolor%256);
	}
	return (UI08)(haircolor%256);
}
UI08 CChar::GetBeardColour( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(beardcolour>>8);
	case 2:
	default:	return (UI08)(beardcolour%256);
	}
	return (UI08)(beardcolour%256);
}
UI08 CChar::GetSayColour( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(saycolor>>8);
	case 2:
	default:	return (UI08)(saycolor%256);
	}
	return (UI08)(saycolor%256);
}
UI08 CChar::GetEmoteColour( UI08 part ) const
{
	switch( part )
	{
	case 1:		return (UI08)(emotecolor>>8);
	case 2:
	default:	return (UI08)(emotecolor%256);
	}
	return (UI08)(emotecolor%256);
}

UI16 CChar::GetEmoteColour( void ) const
{
	return emotecolor;
}
UI16 CChar::GetSayColour( void ) const
{
	return saycolor;
}
UI16 CChar::GetHairColour( void ) const
{
	return haircolor;
}
UI16 CChar::GetBeardColour( void ) const
{
	return beardcolour;
}
UI16 CChar::GetSkin( void ) const
{
	return GetColour();
}
UI16 CChar::GetOrgSkin( void ) const
{
	return orgSkin;
}
UI16 CChar::GetxSkin( void ) const
{
	return xskin;
}

void CChar::SetSkin( UI08 value, UI08 part )
{
	SetColour( value, part );
}
void CChar::SetSkin( UI16 value )
{
	SetColour( value );
}
void CChar::SetOrgSkin( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(orgSkin>>8);
	UI08 part2 = (UI08)(orgSkin%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	orgSkin = (UI16)((part1<<8) + part2);
}
void CChar::SetOrgSkin( UI16 value )
{
	orgSkin = value;
}
void CChar::SetxSkin( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(xskin>>8);
	UI08 part2 = (UI08)(xskin%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	xskin = (UI16)((part1<<8) + part2);
}
void CChar::SetxSkin( UI16 value )
{
	xskin = value;
}
void CChar::SetHairColour( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(haircolor>>8);
	UI08 part2 = (UI08)(haircolor%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	haircolor = (UI16)((part1<<8) + part2);
}
void CChar::SetHairColour( UI16 value )
{
	haircolor = value;
}
void CChar::SetBeardColour( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(beardcolour>>8);
	UI08 part2 = (UI08)(beardcolour%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	beardcolour = (UI16)((part1<<8) + part2);
}
void CChar::SetBeardColour( UI16 value )
{
	beardcolour = value;
}
void CChar::SetEmoteColour( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(emotecolor>>8);
	UI08 part2 = (UI08)(emotecolor%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	emotecolor = (UI16)((part1<<8) + part2);
}
void CChar::SetEmoteColour( UI16 newValue )
{
	emotecolor = newValue;
}
void CChar::SetSayColour( UI08 value, UI08 part )
{
	UI08 part1 = (UI08)(saycolor>>8);
	UI08 part2 = (UI08)(saycolor%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	}
	saycolor = (UI16)((part1<<8) + part2);
}
void CChar::SetSayColour( UI16 newValue )
{
	saycolor = newValue;
}

SI16 CChar::GetFx( UI08 part ) const
{
	switch( part )
	{
	case 1:		return fx1;
	case 2:		return fx2;
	default:	return fx1;
	}
	return fx1;
}

SI16 CChar::GetFy( UI08 part ) const
{
	switch( part )
	{
	case 1:		return fy1;
	case 2:		return fy2;
	default:	return fy1;
	}
	return fy1;
}
SI08 CChar::GetDispZ( void ) const
{
	return dispz;
}
SI08 CChar::GetFz( void ) const
{
	return fz1;
}
SI08 CChar::GetStealth( void ) const
{
	return stealth;
}
SI08 CChar::GetDir2( void ) const
{
	return dir2;
}
SI08 CChar::GetCell( void ) const
{
	return cell;
}
SI08 CChar::GetNpcWander( void ) const
{
	return npcWander;
}
SI08 CChar::GetOldNpcWander( void ) const
{
	return oldnpcWander;
}
SI08 CChar::GetFlySteps( void ) const
{
	return fly_steps;
}
UI08 CChar::GetRunning( void ) const
{
	return running;
}
UI08 CChar::GetStep( void ) const
{
	return step;
}
UI08 CChar::GetRegion( void ) const
{
	return region;
}
void CChar::SetDispZ( SI08 newZ )
{
	dispz = newZ;
}

void CChar::SetLocation( const cBaseObject *toSet )
{
	if( toSet == NULL )
		return;
	MapRegion->RemoveChar( this );
	x = toSet->GetX();
	y = toSet->GetY();
	z = toSet->GetZ();
	if( toSet->GetObjType() == OT_CHAR )
		dispz = ( (CChar *)toSet )->GetDispZ();
	else
		dispz = toSet->GetZ();
	WorldNumber( toSet->WorldNumber() );
	MapRegion->AddChar( this );
}
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ, UI08 world )
{
	MapRegion->RemoveChar( this );
	x = newX;
	y = newY;
	z = newZ;
	dispz = newZ;
	WorldNumber( world );
	MapRegion->AddChar( this );
}
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ )
{
	MapRegion->RemoveChar( this );
	x = newX;
	y = newY;
	z = newZ;
	dispz = newZ;
	MapRegion->AddChar( this );
}

void CChar::SetLocation( struct point3 &targ )
{
	SetLocation( (SI16)targ.x, (SI16)targ.y, (SI08)targ.z );
}
void CChar::SetFx( SI16 newVal, UI08 part )
{
	switch( part )
	{
	case 1:		fx1 = newVal;		break;
	case 2:		fx2 = newVal;		break;
	}
}
void CChar::SetFy( SI16 newVal, UI08 part )
{
	switch( part )
	{
	case 1:		fy1 = newVal;		break;
	case 2:		fy2 = newVal;		break;
	}
}
	
void CChar::SetFz( SI08 newVal )
{
	fz1 = newVal;
}

void CChar::SetDir2( SI08 newValue )
{
	dir2 = newValue;
}
void CChar::SetCell( SI08 newVal )
{
	cell = newVal;
}
void CChar::SetStealth( SI08 newValue )
{
	stealth = newValue;
}
void CChar::SetRunning( UI08 newValue )
{
	running = newValue;
}
void CChar::SetNpcWander( SI08 newValue )
{
	npcWander = newValue;
}
void CChar::SetOldNpcWander( SI08 newValue )
{
	oldnpcWander = newValue;
}
void CChar::SetFlySteps( SI08 newValue )
{
	fly_steps = newValue;
}
void CChar::SetStep( UI08 newValue )
{
	step = newValue;
}
void CChar::SetRegion( UI08 newValue )
{
	region = newValue;
}
CItem * CChar::GetPackItem( void ) const
{
	return packitem;
}
CHARACTER CChar::GetTarg( void ) const
{
	return targ;
}
CHARACTER CChar::GetAttacker( void ) const
{
	return attacker;
}
CHARACTER CChar::GetFTarg( void ) const
{
	return ftarg;
}
ITEM CChar::GetSmeltItem( void ) const
{
	return smeltitem;
}
ITEM CChar::GetTailItem( void ) const
{
	return tailitem;
}
ITEM CChar::GetAdvObj( void ) const
{
	return advobj;
}
ITEM CChar::GetEnvokeItem( void ) const
{
	return envokeitem;
}
CHARACTER CChar::GetSwingTarg( void ) const
{
	return swingtarg;
}
CHARACTER CChar::GetTrackingTarget( void ) const
{
	return trackingtarget;
}
CHARACTER CChar::GetTrackingTargets( UI08 targetNum ) const
{
	return trackingtargets[targetNum];
}
RACEID CChar::GetRaceGate( void ) const
{
	return raceGate;
}
void CChar::SetPackItem( CItem *newVal )
{
	packitem = newVal;
}
void CChar::SetTarg( CHARACTER newTarg )
{
	targ = newTarg;
}
void CChar::SetAttacker( CHARACTER newValue )
{
	attacker = newValue;
}
void CChar::SetFTarg( CHARACTER newTarg )
{
	ftarg = newTarg;
}
void CChar::SetTailItem( ITEM newValue )
{
	tailitem = newValue;
}
void CChar::SetSmeltItem( ITEM newValue )
{
	smeltitem = newValue;
}
void CChar::SetAdvObj( ITEM newValue )
{
	advobj = newValue;
}
void CChar::SetEnvokeItem( ITEM newValue )
{
	envokeitem = newValue;
}
void CChar::SetSwingTarg( CHARACTER newValue )
{
	swingtarg = newValue;
}
void CChar::SetTrackingTarget( CHARACTER newValue )
{
	trackingtarget = newValue;
}
void CChar::SetRaceGate( RACEID newValue )
{
	raceGate = newValue;
}
void CChar::SetTrackingTargets( CHARACTER newValue, UI08 targetNum )
{
	trackingtargets[targetNum] = newValue;
}

SI08 CChar::GetCasting( void ) const
{
	return casting;
}
SI08 CChar::GetSpellCast( void ) const
{
	return spellCast;
}
SI16 CChar::GetSpellAction( void ) const
{
	return spellaction;
}
SI16 CChar::GetSpAttack( void ) const
{
	return spattack;
}
SI08 CChar::GetSpDelay( void ) const
{
	return spadelay;
}

void CChar::SetSpellCast( SI08 newValue )
{
	spellCast = newValue;
}
void CChar::SetCasting( SI08 newValue )
{
	casting = newValue;
}
void CChar::SetSpellAction( SI16 newValue )
{
	spellaction = newValue;
}
void CChar::SetSpAttack( SI16 newValue )
{
	spattack = newValue;
}
void CChar::SetSpDelay( SI08 newValue )
{
	spadelay = newValue;
}

UI08 CChar::GetQuestType( void ) const
{
	return questType;
}
UI08 CChar::GetQuestOrigRegion( void ) const
{
	return questDestRegion;
}
UI08 CChar::GetQuestDestRegion( void ) const
{
	return questOrigRegion;
}

void CChar::SetQuestDestRegion( UI08 newValue )
{
	questDestRegion = newValue;
}
void CChar::SetQuestType( UI08 newValue )
{
	questType = newValue;
}
void CChar::SetQuestOrigRegion( UI08 newValue )
{
	questOrigRegion = newValue;
}

SI16 CChar::GetFleeAt( void ) const
{
	return fleeat;
}
SI16 CChar::GetReattackAt( void ) const
{
	return reattackat;
}

void CChar::SetFleeAt( SI16 newValue )
{
	fleeat = newValue;
}
void CChar::SetReattackAt( SI16 newValue )
{
	reattackat = newValue;
}

UI08 CChar::GetCommandLevel( void ) const
{
	return commandLevel;
}
UI08 CChar::GetPostType( void ) const
{
	return postType;
}
UI08 CChar::GetPriv( void ) const
{
	return (UI08)(priv%256);
}
UI08 CChar::GetPriv2( void ) const
{
	return (UI08)(priv>>8);
}
SI08 CChar::GetTownPriv( void ) const
{
	return townpriv;
}
void CChar::SetPriv( UI08 newValue )
{
	UI08 part1 = (UI08)(priv>>8);
	priv = (UI16)((part1<<8) + newValue);
}
void CChar::SetPriv2( UI08 newValue )
{
	UI08 part2 = (UI08)(priv%256);
	priv = (UI16)((newValue<<8) + part2);
}
void CChar::SetPostType( UI08 newValue )
{
	postType = newValue;
}
void CChar::SetCommandLevel( UI08 newValue )
{
	commandLevel = newValue;
}
void CChar::SetTownpriv( SI08 newValue )
{
	townpriv = newValue;
}

SI16 CChar::GetMana2( void ) const
{
	return mn2;
}
SI16 CChar::GetStamina2( void ) const
{
	return stm2;
}

void CChar::SetStamina2( SI16 newVal )
{
	stm2 = newVal;
}
void CChar::SetMana( SI16 mn )
{
	mana = min( max( 0, mn ), GetMaxMana() );
}
void CChar::SetMana2( SI16 newVal )
{
	mn2 = newVal;
}

UI16 CChar::GetBaseSkill( UI08 skillToGet ) const
{
	return baseskill[skillToGet];
}
UI16 CChar::GetAtrophy( UI08 skillToGet ) const
{
	return atrophy[skillToGet];
}
UI08 CChar::GetSkillLock( UI08 skillToGet ) const
{
	return lockState[skillToGet];
}
UI16 CChar::GetSkill( UI08 skillToGet ) const
{
	SI32 modifier = Races->DamageFromSkill( skillToGet, race );
	if( modifier == 0 )
		return skill[skillToGet];
	else
	{
		UI16 toAdd = (UI16)( (R32)skill[skillToGet] * ( (R32)modifier / 1000 ) );		// percentage to add
		return (UI16)( skill[skillToGet] + toAdd );	// return the bonus
	}
}

void CChar::SetBaseSkill( UI16 newSkillValue, UI08 skillToSet )
{
	baseskill[skillToSet] = newSkillValue;
}
void CChar::SetSkill( UI16 newSkillValue, UI08 skillToSet )
{
	skill[skillToSet] = newSkillValue;
}
void CChar::SetAtrophy( UI16 newValue, UI08 skillToSet )
{
	atrophy[skillToSet] = newValue;
}
void CChar::SetSkillLock( UI08 newSkillValue, UI08 skillToSet )
{
	lockState[skillToSet] = newSkillValue;
}

SI16 CChar::GetFame2( void ) const
{
	return fame2;
}
UI16 CChar::GetDeaths( void ) const
{
	return deaths;
}
SI16 CChar::GetGuildNumber( void ) const
{
	return guildnumber;
}
SI08 CChar::GetFlag( void ) const
{
	return flag;
}

void CChar::SetFame2( SI16 newVal )
{
	fame2 = newVal;
}
void CChar::SetKarma2( SI16 newVal )
{
	karma2 = newVal;
}
void CChar::SetDeaths( UI16 newVal )
{
	deaths = newVal;
}
void CChar::SetFlag( SI08 newValue )
{
	flag = newValue;
}
void CChar::SetGuildNumber( SI16 newValue )
{
	guildnumber = newValue;
}

SI16 CChar::GetCallNum( void ) const
{
	return callnum;
}
SI16 CChar::GetPageGM( void ) const
{
	return pagegm;
}
SI16 CChar::GetPlayerCallNum( void ) const
{
	return playercallnum;
}

void CChar::SetPlayerCallNum( SI16 newValue )
{
	playercallnum = newValue;
}
void CChar::SetCallNum( SI16 newValue )
{
	callnum = newValue;
}
void CChar::SetPageGM( SI16 newValue )
{
	pagegm = newValue;
}

SI08 CChar::GetFontType( void ) const
{
	return fonttype;
}
UI08 CChar::GetSquelched( void ) const
{
	return squelched;
}

void CChar::SetFontType( SI08 newType )
{
	fonttype = newType;
}
void CChar::SetSquelched( UI08 newValue )
{
	squelched = newValue;
}

SI08 CChar::GetPoison( void ) const
{
	return poison;
}
SI08 CChar::GetPoisoned( void ) const
{
	return poisoned;
}

void CChar::SetPoison( SI08 newValue )
{
	poison = newValue;
}
void CChar::SetPoisoned( SI08 newValue )
{
	poisoned = newValue;
}

CChar::CChar( CHARACTER c, bool zeroSer ) : robe( INVALIDSERIAL ), townvote( INVALIDSERIAL ), bools( 0 ), antispamtimer( 0 ), 
account( -1 ), dispz( 0 ), xskin( colour ), fonttype( 3 ), maxHP( 0 ), maxHP_oldstr( 0 ), maxHP_oldrace( 0 ), maxMana( 0 ), maxMana_oldint( 0 ), maxMana_oldrace( 0 ),
maxStam( 0 ), maxStam_olddex( 0 ), maxStam_oldrace( 0 ),
saycolor( 0x0058 ), emotecolor( 0x0023 ), mn2( 0 ), cell( -1 ), deaths( 0 ), packitem( NULL ), fixedlight( 255 ), weight( 0 ), 
targ( INVALIDSERIAL ), timeout( 0 ), attacker( INVALIDSERIAL ), npcmovetime( 0 ), npcWander( 0 ), oldnpcWander( 0 ), ftarg( INVALIDSERIAL ), fx1( -1 ),
fx2( -1 ), fy1( -1 ), fy2( -1 ), fz1( -1 ), invistimeout( 0 ), hunger( 6 ), hungertime( 0 ), smeltitem( INVALIDSERIAL ), tailitem( INVALIDSERIAL ),
npcaitype( 0 ), callnum( -1 ), playercallnum( -1 ), pagegm( 0 ), region( 255 ), skilldelay( 0 ), objectdelay( 0 ), making( INVALIDSERIAL ),
blocked( 0 ), dir2( 0 ), spiritspeaktimer( 0 ), spattack( 0 ), spadelay( 0 ), spatimer( 0 ), taming( 0 ), summontimer( 0 ),
trackingtimer( 0 ), trackingtarget( 0 ), fishingtimer( 0 ), town( 255 ), townpriv( 0 ), advobj( 0 ), poison( 0 ), poisoned( 0 ),
poisontime( 0 ), poisontxt( 0 ), poisonwearofftime( 0 ), fleeat( 0 ), reattackat( 0 ), envokeid( 0 ), envokeitem( INVALIDSERIAL ), split( 0 ),
splitchnc( 0 ), trainer( 0 ), trainingplayerin( 0 ), guildfealty( INVALIDSERIAL ), guildnumber( -1 ), flag( 0x04 ), murderrate( 0 ),
crimflag( -1 ), casting( 0 ), spelltime( 0 ), spellCast( -1 ), spellaction( 0 ), nextact( 0 ), poisonserial( INVALIDSERIAL ),
squelched( 0 ), mutetime( 0 ), med( 0 ), stealth( -1 ), running( 0 ), logout( 0 ), swingtarg( INVALIDSERIAL ), holdg( 0 ), raceGate( 65535 ),
fly_steps( 0 ), smoketimer( 0 ), smokedisplaytimer( 0 ), carve( -1 ), commandLevel( 0 ), postType( LOCALPOST ),
questType( 0 ), questDestRegion( 0 ), questOrigRegion( 0 ), ourAccount(), fame2( 0 ), karma2( 0 ), step( 0 ),
stm2( 0 ), hairstyle( INVALIDID ), beardstyle( INVALIDID ), haircolor( INVALIDCOLOUR ), beardcolour( INVALIDCOLOUR ), orgSkin( colour ), petguarding( INVALIDSERIAL ),
trackingdisplaytimer( 0 ), may_levitate( false ), oldx( 0 ), oldy( 0 ), oldz( 0 ), speechItem( INVALIDSERIAL ), SavedAt( 0 ), addMenuLoc( INVALIDSERIAL ), remove( 0 )
{
	id = 0x0190;
	xid = id;
	orgid = id;
	objType = OT_CHAR;
	if( zeroSer )
		SetSerial( INVALIDSERIAL, c );
	else
	{
		SetSerial( charcount2, c );
		charcount2++;
	}
	memset( itemLayers, 0, sizeof( itemLayers[0] ) * MAXLAYERS );
	petsControlled.resize( 0 );
	petFriends.resize( 0 );
	strcpy( name, "Mr. noname" );
	title[0] = 0x00;
	orgname[0] = 0x00;
	SetPriv( 0 );	
	SetPriv2( 0 );	
	for( SI32 k = 0; k <= ALLSKILLS; k++ )
		baseskill[k] = 10;
	memset( skill, 0, sizeof( skill[0] ) * (ALLSKILLS+1) );
	memset( lockState, 0, sizeof( lockState[0] ) * (ALLSKILLS+1) );
	for( UI16 j = 0; j <= ALLSKILLS; j++ )
		atrophy[j] = j;
	memset( trackingtargets, 0, sizeof( trackingtargets[0] ) * MAXTRACKINGTARGETS );
	SetCanTrain( true );
	laston[0] = 0;
	guildtitle[0] = 0;

	memset( weathDamage, 0, sizeof( weathDamage[0] ) * WEATHNUM );
	skillUsed[0] = skillUsed[1] = 0;
	memset( regen, 0, sizeof( UI32 ) * 3 );
}

CChar::~CChar()
{
	// Destructor to clean things up when deleted
	if( !isFree() )	// We're not the default item in the handler
	{
		// For EVERY item that we have, let's move them either a) into our parent container if it's an item
		// or b) On to the ground where we were
//		for( CItem *tItem = FirstItemObj(); !FinishedItems(); tItem = NextItemObj() )
//		{
//			if( tItem != NULL )
//			{
//				tItem->HardContSet( INVALIDSERIAL );
//				Items->DeleItem( tItem );
//			}
//		}
		CHARLIST tPets;
		for( UI32 i = 0; i < petsControlled.size(); i++ )
			tPets.push_back( petsControlled[i] );
		for( UI32 j = 0; j < tPets.size(); j++ )
		{
			if( tPets[j] != NULL )
				tPets[j]->SetOwner( INVALIDSERIAL );
		}
		MapRegion->RemoveChar( this );	// Let's remove it from a mapregion (if any)
		SetOwner( INVALIDSERIAL );	// Let's remove it from our owner (if any)
	}
}

bool CChar::IsGM( void ) const
{
	return ( (priv&0x01) == 0x01 );
}
bool CChar::CanBroadcast( void ) const
{
	return ( (priv&0x02) == 0x02 );
}
bool CChar::IsInvulnerable( void ) const
{
	return ( (priv&0x04) == 0x04 );
}
bool CChar::GetSingClickSer( void ) const
{
	return ( (priv&0x08) == 0x08 );
}
bool CChar::NoSkillTitles( void ) const
{
	return ( (priv&0x10) == 0x10 );
}
bool CChar::IsGMPageable( void ) const
{
	return ( (priv&0x20) == 0x20 );
}
bool CChar::CanSnoop( void ) const
{
	return ( (priv&0x40) == 0x40 );
}
bool CChar::IsCounselor( void ) const
{
	return ( (priv&0x80) == 0x80 );
}

void CChar::SetGM( bool newValue )
{
	if( newValue )
		priv |= 0x0001;
	else
		priv &= ~0x0001;
}
void CChar::SetBroadcast( bool newValue )
{
	if( newValue )
		priv |= 0x0002;
	else
		priv &= ~0x0002;
}
void CChar::SetInvulnerable( bool newValue )
{
	if( newValue )
		priv |= 0x0004;
	else
		priv &= ~0x0004;
}
void CChar::SetSingClickSer( bool newValue )
{
	if( newValue )
		priv |= 0x0008;
	else
		priv &= ~0x0008;
}
void CChar::SetSkillTitles( bool newValue )
{
	if( newValue )
		priv |= 0x0010;
	else
		priv &= ~0x0010;
}
void CChar::SetGMPageable( bool newValue )
{
	if( newValue )
		priv |= 0x0020;
	else
		priv &= ~0x0020;
}
void CChar::SetSnoop( bool newValue )
{
	if( newValue )
		priv |= 0x0040;
	else
		priv &= ~0x0040;
}
void CChar::SetCounselor( bool newValue )
{
	if( newValue )
		priv |= 0x0080;
	else
		priv &= ~0x0080;
}

bool CChar::AllMove( void ) const
{
	return ( (priv&0x0100) == 0x0100 );
}
bool CChar::IsFrozen( void ) const
{
	return ( (priv&0x0200) == 0x0200 );
}
bool CChar::ViewHouseAsIcon( void ) const
{
	return ( (priv&0x0400) == 0x0400 );
}
bool CChar::IsPermHidden( void ) const
{
	return ( (priv&0x0800) == 0x0800 );
}
bool CChar::NoNeedMana( void ) const
{
	return ( (priv&0x1000) == 0x1000 );
}
bool CChar::IsDispellable( void ) const
{
	return ( (priv&0x2000) == 0x2000 );
}
bool CChar::IsPermReflected( void ) const
{
	return ( (priv&0x4000) == 0x4000 );
}
bool CChar::NoNeedReags( void ) const
{
	return ( (priv&0x8000) == 0x8000 );
}

void CChar::SetAllMove( bool newValue )
{
	if( newValue )
		priv |= 0x0100;
	else
		priv &= ~0x0100;
}
void CChar::SetFrozen( bool newValue )
{
	if( newValue )
		priv |= 0x0200;
	else
		priv &= ~0x0200;
}
void CChar::SetViewHouseAsIcon( bool newValue )
{
	if( newValue )
		priv |= 0x0400;
	else
		priv &= ~0x0400;
}
void CChar::SetPermHidden( bool newValue )
{
	if( newValue )
		priv |= 0x0800;
	else
		priv &= ~0x0800;
}
void CChar::SetNoNeedMana( bool newValue )
{
	if( newValue )
		priv |= 0x1000;
	else
		priv &= ~0x1000;
}
void CChar::SetDispellable( bool newValue )
{
	if( newValue )
		priv |= 0x2000;
	else
		priv &= ~0x2000;
}
void CChar::SetPermReflected( bool newValue )
{
	if( newValue )
		priv |= 0x4000;
	else
		priv &= ~0x4000;
}
void CChar::SetNoNeedReags( bool newValue )
{
	if( newValue )
		priv |= 0x8000;
	else
		priv &= ~0x8000;
}

CChar *CChar::Dupe( void )
{
	CHARACTER targetOff;
	CChar *target = Npcs->MemCharFree( targetOff );
	if( target == NULL )
		return NULL;

	target->SetMulti( GetMultiObj() );
	target->SetOwner( GetOwner() );
	target->SetRobe( robe );
	target->SetSpawn( GetSpawn(), targetOff );
	target->SetTownVote( townvote );
	target->SetPriv( GetPriv() );
	target->SetPriv2( GetPriv2() );
	target->SetName( name );
	target->SetTitle( title );
	target->SetOrgName( orgname );
	target->SetAntiSpamTimer( antispamtimer );
	target->SetAccount( account );
	target->SetLocation( this );
	target->SetDispZ( dispz );
	target->SetDir( dir );
	target->SetID( id );
	target->SetxID( xid );
	target->SetOrgID( orgid );
	target->SetSkin( GetColour() );
	target->SetxSkin( xskin );
	target->SetFontType( fonttype );
	target->SetSayColour( saycolor );
	target->SetEmoteColour( emotecolor );
	target->SetStrength( strength );
	target->SetDexterity( dexterity );
	target->SetIntelligence( intelligence );
	target->SetHP(  hitpoints );
	target->SetStamina( stm2 );
	target->SetMana( mn2 );

	target->Strength2( st2 );
	target->Dexterity2( dx2 );
	target->Intelligence2( in2 );
	target->SetMana2( mn2 );
	target->SetStamina2( stm2 );

	target->SetHiDamage( hidamage );
	target->SetLoDamage( lodamage );

	for( UI08 i = 0; i <= ALLSKILLS; i++ )
	{
		target->SetBaseSkill( baseskill[i], i );
		target->SetSkill( skill[i], i );
		target->SetAtrophy( atrophy[i], i );
		target->SetSkillLock( lockState[i], i );
	}

	target->SetCell( cell );
	target->SetKarma( karma );
	target->SetFame( fame );
	target->SetKills( kills );
	target->SetDeaths( deaths );
	target->SetPackItem( packitem );
	target->SetFixedLight( fixedlight );
	target->SetWeight( weight );
	target->SetDef( def );
	target->SetTarg( targ );
	target->SetTimeout( timeout );
	target->SetRegen( regen[0], 0 );
	target->SetRegen( regen[1], 1 );
	target->SetRegen( regen[2], 2 );
	target->SetAttacker( attacker );
	target->SetNpcMoveTime( npcmovetime );
	target->SetNpcWander( npcWander );
	target->SetOldNpcWander( oldnpcWander );
	target->SetFTarg( ftarg );
	target->SetFx( fx1, 1 );
	target->SetFx( fx2, 2 );
	target->SetFy( fy1, 1 );
	target->SetFy( fy2, 2 );
	target->SetFz( fz1 );
	target->SetVisible( GetVisible() );
	target->SetInvisTimeout( invistimeout );
	target->SetHunger( hunger );
	target->SetHungerTime( hungertime );
	target->SetSmeltItem( smeltitem );
	target->SetTailItem( tailitem );
	target->SetNPCAiType( npcaitype );
	target->SetGuarding( petguarding );
	target->SetCallNum( callnum );
	target->SetPlayerCallNum( playercallnum );
	target->SetPageGM( pagegm );
	target->SetRegion( region );
	target->SetSkillDelay( skilldelay );
	target->SetObjectDelay( objectdelay );
	target->SetMaking( making );
	target->SetBlocked( blocked );
	target->SetDir2( dir2 );
	target->SetSpiritSpeakTimer( spiritspeaktimer );
	target->SetSpAttack( spattack );
	target->SetSpDelay( spadelay );
	target->SetTaming( taming );
	target->SetSummonTimer( summontimer );
	target->SetTrackingTimer( trackingtimer );
	target->SetTrackingTarget( trackingtarget );
	for( UI08 counter = 0; counter < MAXTRACKINGTARGETS; counter++ )
	{
		target->SetTrackingTargets( trackingtargets[counter], counter );
	}
	target->SetFishingTimer( fishingtimer );
	target->SetTown( town );
	target->SetTownpriv( townpriv );
	target->SetAdvObj( advobj );
	target->SetPoison( poison );
	target->SetPoisoned( poisoned );
	target->SetPoisonTime( poisontime );
	target->SetPoisonTextTime( poisontxt );
	target->SetPoisonWearOffTime( poisonwearofftime );

	target->SetFleeAt( fleeat );
	target->SetReattackAt( reattackat );
	target->SetDisabled( disabled );
	target->SetEnvokeID( envokeid );
	target->SetEnvokeItem( envokeitem );
	target->SetSplit( split );
	target->SetSplitChance( splitchnc );
	target->SetTrainer( trainer );
	target->SetTrainingPlayerIn( trainingplayerin );
	target->SetCanTrain( CanTrain() );
	target->SetLastOn( laston );
	target->SetGuildTitle( guildtitle );
	target->SetGuildFealty( guildfealty );
	target->SetGuildNumber( guildnumber );
	target->SetFlag( flag );
	target->SetMurderRate( murderrate );
	target->SetCrimFlag( crimflag );
	target->SetCasting( casting );
	target->SetSpellTime( spelltime );
	target->SetSpellCast( spellCast );
	target->SetSpellAction( spellaction );
	target->SetNextAct( nextact );
	target->SetPoisonSerial( poisonserial );
	target->SetSquelched( GetSquelched() );
	target->SetMuteTime( mutetime );
	target->SetMed( med );
	target->SetStealth( stealth );
	target->SetRunning( running );
	target->SetLogout( logout );
	target->SetSwingTarg( swingtarg );
	target->SetHoldG( holdg );
	target->SetRace( GetRace() );
	target->SetRaceGate( raceGate );
	target->SetFlySteps( fly_steps );
	target->SetSmokeTimer( smoketimer );
	target->SetSmokeDisplayTimer( smokedisplaytimer );
	target->SetCarve( carve );
	target->SetCommandLevel( commandLevel );
	target->SetPostType( postType );
	target->SetQuestType( questType );
	target->SetQuestDestRegion( questDestRegion );
	target->SetQuestOrigRegion( questOrigRegion );
	for( UI08 counter2 = 0; counter2 < WEATHNUM; counter2++ )
	{
		target->SetWeathDamage( weathDamage[counter2], counter2 );
	}
	return target;

	target->SetMaxHP( maxHP, maxHP_oldstr, maxHP_oldrace );
	target->SetMaxMana( maxMana, maxMana_oldint, maxMana_oldrace );
}

void CChar::SetRemove( bool newValue )
{
	remove = newValue;
}

bool CChar::GetRemove( void ) const
{
	return remove;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::RemoveFromSight
//|   Date        -  April 7th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Loops through all online SI08s and removes the CHARACTER
//|                  from their sight
//o---------------------------------------------------------------------------o
void CChar::RemoveFromSight( void )
{
	CPRemoveItem toSend = (*this);
	Network->PushConn();
	for( cSocket *mSock = Network->FirstSocket(); !Network->FinishedSockets(); mSock = Network->NextSocket() )
	{
		if( mSock != NULL )
			mSock->Send( &toSend );
	}
	Network->PopConn();
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SendInSight
//|   Date        -  April 7th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sends the information about this person to socket S
//|                  IF in range.  Essentially a replacement for impowncreate
//o---------------------------------------------------------------------------o

void CChar::SendToSocket( cSocket *s, bool sendDispZ, CChar *c )
{
	SI32 k;
	UI08 oc[1024];
	
	if( s == NULL || c == NULL ) 
		return;

	CChar *mCharObj = s->CurrcharObj();
	if( GetCommandLevel() > mCharObj->GetCommandLevel() && GetHidden() )
		return;
	if( !IsNpc() && !isOnline( this ) && !( mCharObj->IsGM() || mCharObj->IsCounselor() ) )
		return;

	oc[0] = 0x78; // Message type 78
	oc[3] = GetSerial( 1 ); // CHARACTER serial number
	oc[4] = GetSerial( 2 ); // CHARACTER serial number
	oc[5] = GetSerial( 3 ); // CHARACTER serial number
	oc[6] = GetSerial( 4 ); // CHARACTER serial number
	oc[7] = GetID( 1 ); // CHARACTER art id
	oc[8] = GetID( 2 ); // CHARACTER art id
	oc[9] = (UI08)(GetX()>>8);  // CHARACTER x position
	oc[10] = (UI08)(GetX()%256); // CHARACTER x position
	oc[11] = (UI08)(GetY()>>8); // CHARACTER y position
	oc[12] = (UI08)(GetY()%256); // CHARACTER y position
	if( sendDispZ ) 
		oc[13] = GetDispZ(); // CHARACTER z position
	else 
		oc[13] = GetZ();
	oc[14] = GetDir(); // CHARACTER direction
	oc[15] = GetSkin( 1 ); // CHARACTER skin color
	oc[16] = GetSkin( 2 ); // CHARACTER skin color
	oc[17] = 0; // CHARACTER flags
	if( GetHidden() || !( isOnline( c ) || IsNpc() ) ) 
		oc[17] |= 0x80; // Show hidden state correctly
	if( GetPoisoned() )
		oc[17] |= 0x04; //AntiChrist -- thnx to SpaceDog 
	k = 19;
	GuildRelation guild = GuildSys->Compare( mCharObj, c );
	SI32 raceCmp = Races->Compare( mCharObj, c );
	if( GetKills() > cwmWorldState->ServerData()->GetRepMaxKills() )
		oc[18] = 6;
	else if( guild == GR_ALLY || guild == GR_SAME || raceCmp > 0 ) // Same guild (Green), racial ally, allied guild
		oc[18] = 2;
	else if( guild == GR_WAR || raceCmp < 0 ) // Enemy guild.. set to orange
		oc[18] = 5;
	else
	{
		if( IsMurderer() )		// Murderer
			oc[18] = 6;
		else if( IsCriminal() )	// Criminal
			oc[18] = 3;
		else					// Other
			oc[18] = 1;
	}
	
	for( SI32 counter = 0; counter < MAXLAYERS; counter++ )
	{
		if( itemLayers[counter] != NULL )
		{
			oc[k+0] = itemLayers[counter]->GetSerial( 1 );
			oc[k+1] = itemLayers[counter]->GetSerial( 2 );
			oc[k+2] = itemLayers[counter]->GetSerial( 3 );
			oc[k+3] = itemLayers[counter]->GetSerial( 4 );
			oc[k+4] = itemLayers[counter]->GetID( 1 );
			oc[k+5] = itemLayers[counter]->GetID( 2 );
			oc[k+6] = itemLayers[counter]->GetLayer();
			k += 7;
			if( itemLayers[counter]->GetColour() != 0 )
			{
				oc[k-3] |= 0x80;
				oc[k+0] = itemLayers[counter]->GetColour( 1 );
				oc[k+1] = itemLayers[counter]->GetColour( 2 );
				k += 2;
			}
		}
	}
	
	oc[k+0] = 0;// Not well understood.  It's a serial number.  I set this to my serial number,
	oc[k+1] = 0;// and all of my messages went to my paperdoll gump instead of my CHARACTER's
	oc[k+2] = 0;// head, when I was a CHARACTER with serial number 0 0 0 1.
	oc[k+3] = 0;
	k += 4;
	
	oc[1] = (UI08)(k>>8);
	oc[2] = (UI08)(k%256);
	s->Send( oc, k );
}

void CChar::Teleport( void )
{
	SI16 visrange = MAXVISRANGE + Races->VisRange( GetRace() );
	cSocket *mSock = calcSocketObjFromChar( this ), *tSock = NULL;

	RemoveFromSight();
	if( mSock != NULL )
	{
		visrange = mSock->Range() + Races->VisRange( GetRace() );
		CPDrawGamePlayer gpToSend = (*this);
		mSock->Send( &gpToSend );

		statwindow( mSock, this );
		mSock->WalkSequence( -1 );
	}
	
	Network->PushConn();
	for( tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( tSock == NULL )
			continue;
		if( charInRange( this, tSock->CurrcharObj() ) )
			SendToSocket( tSock, true, this );
	}
	Network->PopConn();
	if( mSock != NULL )
	{
		SI32 xOffset = MapRegion->GetGridX( GetX() );
		SI32 yOffset = MapRegion->GetGridY( GetY() );
		for( SI08 counter1 = -1; counter1 <= 1; counter1++ )
		{
			for( SI08 counter2 = -1; counter2 <= 1; counter2++ )
			{
				SubRegion *MapArea = MapRegion->GetGrid( xOffset + counter1, yOffset + counter2, worldNumber );
				if( MapArea == NULL )	// no valid region
					continue;
				MapArea->PushChar();
				MapArea->PushItem();
				for( CChar *tempChar = MapArea->FirstChar(); !MapArea->FinishedChars(); tempChar = MapArea->GetNextChar() )
				{
					if( tempChar == NULL )
						continue;
					if( this != tempChar && charInRange( this, tempChar ) && ( isOnline( tempChar ) || tempChar->IsNpc() || IsGM() ) )
						tempChar->SendToSocket( mSock, true, tempChar );
				}
				for( CItem *tempItem = MapArea->FirstItem(); !MapArea->FinishedItems(); tempItem = MapArea->GetNextItem() )
				{
					if( itemInRange( this, tempItem, visrange ) )
					{
						if( !tempItem->GetVisible() || ( tempItem->GetVisible() && IsGM() ) )
							sendItem( mSock, tempItem );
					}
				}
				MapArea->PopChar();
				MapArea->PopItem();
			}
		}
	}
	
	checkRegion( this );
}

void CChar::ExposeToView( void )
{
	SetVisible( 0 );
	SetStealth( -1 );

	// hide it from ourselves, we want to show ourselves to everyone in range
	// now we tell everyone we exist
	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
		SendToSocket( tSock, true, this );
	Network->PopConn();

}
void CChar::HideFromView( void )
{
	SetHidden( 0 );
	RemoveFromSight();
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechItem
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item related to the speech we're working on
//|                  IE the item for name deed if we're renaming ourselves
//o---------------------------------------------------------------------------o
SERIAL CChar::GetSpeechItem( void ) const
{
	return speechItem;
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechMode
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns speech mode information, as to what mode of speech
//|                  we are in.  Valid values are found just below
//|                  0 normal speech
//|                  1 GM page
//|                  2 Counselor page
//|                  3 Player Vendor item pricing
//|                  4 Player Vendor item describing
//|                  5 Key renaming
//|                  6 Name deed
//|                  7 Rune renaming
//|                  8 Sign renaming
//|                  9 JS Speech
//o---------------------------------------------------------------------------o
UI08 CChar::GetSpeechMode( void ) const
{
	return speechMode;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetSpeechItem
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Stores the index SI32o items[] about the item that is
//|                  likely to be deleted on switching out of our current mode
//o---------------------------------------------------------------------------o
void CChar::SetSpeechItem( SERIAL newValue )
{
	speechItem = newValue;
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetSpeechMode
//|   Date        -  April 7th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the mode of speech that will be used next time
//o---------------------------------------------------------------------------o
void CChar::SetSpeechMode( UI08 newValue )
{
	speechMode = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetAddPart
//|   Date        -  April 9th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  So we can track which part of the add menu we are in
//|				     Until I understand gumps better, it's in here
//o---------------------------------------------------------------------------o
SI32	CChar::GetAddPart( void ) const
{
	return addMenuLoc;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetAddPart
//|   Date        -  April 9th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Stores value of the add menu that we are currently at
//|				     Until I understand gumps better, it's in here
//o---------------------------------------------------------------------------o
void CChar::SetAddPart( SI32 newValue )
{
	addMenuLoc = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void Update( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sends update to all those in range
//o---------------------------------------------------------------------------o
void CChar::Update( void )
{
	RemoveFromSight();

	Network->PushConn();
	for( cSocket *tSock = Network->FirstSocket(); !Network->FinishedSockets(); tSock = Network->NextSocket() )
	{
		if( charInRange( this, tSock->CurrcharObj() ) )
			SendToSocket( tSock, true, this );
	}
	Network->PopConn();
}

bool CChar::IsInUse( void ) const
{
	return InUse;
}
bool CChar::IsSaved( void ) const
{
	return Saved;
}
SI32 CChar::GetSavedAt( void ) const
{
	return SavedAt;
}
void CChar::SetInUse( bool newValue )
{
	InUse = newValue;
}
void CChar::SetSaved( bool newValue )
{
	Saved = newValue;
}
void CChar::SetSavedAt( SI32 newValue )
{
	SavedAt = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem *GetItemAtLayer( UI08 Layer ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item at layer Layer on paperdoll
//o---------------------------------------------------------------------------o
CItem *CChar::GetItemAtLayer( UI08 Layer ) const
{
	if( Layer >= MAXLAYERS )
		return NULL;
	return itemLayers[Layer];
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool WearItem( CItem *toWear ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Wears the item toWear and adjusts the stats if any are
//|					 required to change.  Returns true if successfully equipped
//o---------------------------------------------------------------------------o
bool CChar::WearItem( CItem *toWear )
{
	UI08 tLayer = toWear->GetLayer();
	if( tLayer >= MAXLAYERS || toWear->GetLayer() == 0 )
		return false;
	if( itemLayers[tLayer] != NULL )
	{
		// Console << "Doubled up item (" << toWear->GetSerial() << ") at layer " << toWear->GetLayer() << " on SI08 " << GetName() << "(" << GetSerial() << ")" << endl;
		erroredLayers[tLayer]++;
		return false;
	}
	itemLayers[tLayer] = toWear;

	IncStrength2( itemLayers[tLayer]->Strength2() );
	IncDexterity2( itemLayers[tLayer]->Dexterity2() );
	IncIntelligence2( itemLayers[tLayer]->Intelligence2() );
	if( itemLayers[tLayer]->GetPoisoned() )
	{
		SetPoison( GetPoison() - itemLayers[tLayer]->GetPoisoned() );
		if( GetPoison() < 0 )
			SetPoison( 0 );
	}

	cScript *tScript = NULL;
	UI16 scpNum = toWear->GetScriptTrigger();
	tScript = Trigger->GetScript( scpNum );
	if( tScript != NULL )
		tScript->OnEquip( this, toWear );
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool TakeOffItem( UI08 Layer )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Takes the item off the CHARACTER's paperdoll,
//|					 ensuring that any stat adjustments are made
//|					 Returns true if successfully unequipped
//o---------------------------------------------------------------------------o
bool CChar::TakeOffItem( UI08 Layer )
{
	if( Layer >= MAXLAYERS || itemLayers[Layer] == NULL )
		return false;
	
	if( Layer == 0x15 )	// It's our pack!
	{
		SetPackItem( NULL );
	}
	IncStrength2( -itemLayers[Layer]->Strength2() );
	IncDexterity2( -itemLayers[Layer]->Dexterity2() );
	IncIntelligence2( -itemLayers[Layer]->Intelligence2() );
	if( itemLayers[Layer]->GetPoisoned() )
	{
		SetPoison( GetPoison() - itemLayers[Layer]->GetPoisoned() );
		if( GetPoison() < 0 )
			SetPoison( 0 );
	}
	
	cScript *tScript = NULL;
	UI16 scpNum = itemLayers[Layer]->GetScriptTrigger();
	tScript = Trigger->GetScript( scpNum );
	if( tScript != NULL )
		tScript->OnUnequip( this, itemLayers[Layer] );

	itemLayers[Layer] = NULL;
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem *FirstItem( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item reference for the first item on paperdoll
//o---------------------------------------------------------------------------o
CItem *CChar::FirstItem( void ) const
{
	iCounter = 0;
	return itemLayers[iCounter];
}

//o---------------------------------------------------------------------------o
//|   Function    -  CItem *NextItem( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item reference for the next item on paperdoll
//o---------------------------------------------------------------------------o
CItem *CChar::NextItem( void ) const
{
	iCounter++;
	if( iCounter >= MAXLAYERS )
		return NULL;
	return itemLayers[iCounter];
}
//o---------------------------------------------------------------------------o
//|   Function    -  bool FinishedItems( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if there are no more items on the paperdoll
//o---------------------------------------------------------------------------o
bool CChar::FinishedItems( void ) const
{
	return( iCounter >= MAXLAYERS );
}

//o---------------------------------------------------------------------------o
//|   Function    - UI32 NumItems( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the number of items on the paperdoll
//o---------------------------------------------------------------------------o
UI32 CChar::NumItems( void ) const
{
	return MAXLAYERS;
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsValidMount( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is a valid mount
//o---------------------------------------------------------------------------o
bool CChar::IsValidMount( void ) const
{
	switch( id )
	{
	//case 0xC8:    // horse 
	//case 0xE2:    // horse 
	//case 0xE4:    // horse 
	//case 0xCC:    // horse 
	//case 0xD2:    // Desert Ostard 
	//case 0xDA:    // Frenzied Ostard 
	//case 0xDB:    // Forest Ostard 
	//case 0xDC:    // llama 
	//case 0x72: // Dark Steed 
	//case 0x73: // Etheral Horse 
	//case 0x74: // Nightmare 
	//case 0x75: // Silver Steed 
	//case 0x76: // Faction Horse 
	//case 0x77: // Faction Horse 
	//case 0x78: // Faction Horse 
	//case 0x79: // Faction Horse 
	//case 0x84: // Dragon Horse 
	//case 0x90: // Sea Horse 
	//case 0x7A: // Unicournaybe 
	//case 0xAA: // Etheral Llama 
	//case 0xAB: // Etheral Ostard 
	//case 0xBB: // Ridgeback 
    
    
                   // New LBR ridables added 
                   case 0x347: //ridgeback ridgeback 
                   case 0xBB: //ridgeback ridgeback 
                   case 0xC8: //equines horse dappled brown 
                   case 0x334: //equines horse dappled brown 
                   //case 0x123: //equines horse dappled brown pack 
                   case 0xE2: //equines horse dappled grey 
                   case 0x34: //eequines horse dappled grey 
                   case 0xCC: //equines_horse dark brown 
                   case 0x338: //equines horse dark brown 
                   case 0xE4: //equines horse tan 
                   case 0x350: //equines horse_tan 
                   case 0x72: //equines horse dark steed 
                   case 0x73: //equines horse ethereal 
                   case 0x74: //equines horse nightmare 
                   case 0xB1: //equines horse nightmare 
                   case 0xB2: //equines horse nightmare 
                   case 0xB3: //equines horse nightmare 
                   case 0x75: //equines horse silver steed 
                   case 0x76: //equines horse war brittanian 
                   case 0x77: //equines horse war mage_council 
                   case 0x78: //equines horse war minax 
                   case 0x79: //equines horse war shadowlord 
                   case 0x7A: //equines unicorn 
                   case 0x84: //kirin kirin 
                   case 0xE6: //kirin kirin 
                   case 0xDC: //llamas llama 
                   case 0x3C: //llamas llama 
                   case 0xAA: //llamas llama ethereal 
                   case 0xD2: //ostards ostard desert 
                   case 0x39: //ostards ostard desert 
                   case 0xDA: //ostards ostard forest 
                   case 0x3A: //ostards ostard forest 
                   case 0xDB: //ostards ostard frenzied 
                   case 0x3B: //ostards ostard frenzied 
                   case 0xAB: //ostards ostard ethereal 
                   case 0x90: //sea horse sea horse 
                   case 0x319: //skeletal mount 
                   case 0x31A: //swamp dragon 
                   case 0x31F: //swamp dragon_armor 
                   case 0x317: //giant beetle 

			return true;
		default:
			return false;
	}		

	return false;
}

bool CChar::DumpHeader( std::ofstream &outStream, SI32 mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << "[CHARACTER]" << std::endl;
		break;
	}
	return true;
}
bool CChar::DumpBody( std::ofstream &outStream, SI32 mode ) const
{
	std::string destination; 
	std::ostringstream dumping( destination ); 
	BinBuffer buff;
	CItem *packItem = NULL;
	int tempCounter, bsc;

	switch( mode )
	{
	case 1:	//binary
		if( !cBaseObject::DumpBody( buff ) )	// Make the default save of BaseObject members now
		{
			return false;
		}
		buff.SetType( 1 );

		if( DefChar->GetAccount().wAccountIndex != GetAccount() )
		{
			buff.PutByte( CHARTAG_ACCOUNT );
			buff.PutLong( GetAccount() );
		}

		if( DefChar->GetSayColour() != GetSayColour() || DefChar->GetEmoteColour() != GetEmoteColour() )
		{
			buff.PutByte( CHARTAG_TXTCOLOR );
			buff.PutShort( GetSayColour() );
			buff.PutShort( GetEmoteColour() );
		}

		if( !IsNpc() )
		{
			buff.PutByte( CHARTAG_LASTON );
			buff.PutStr( GetLastOn() );
		}

		if( strcmp( DefChar->GetOrgName(), GetOrgName() ) )
		{
			buff.PutByte( CHARTAG_ORGNAME );
			buff.PutStr( GetOrgName() );
		}

		if( strcmp( DefChar->GetGuildTitle(), GetGuildTitle() ) )
		{
			buff.PutByte( CHARTAG_GUILDTITLE );
			buff.PutStr( GetGuildTitle() );
		}
	
		if( DefChar->GetNPCAiType() != GetNPCAiType() && IsNpc() )
		{
			buff.PutByte( CHARTAG_NPCAI );
			buff.PutShort( GetNPCAiType() );
		}

		if( DefChar->GetMaking() != GetMaking() )
		{
			buff.PutByte( CHARTAG_MAKING );
			buff.PutLong( GetMaking() );
		}

		if( DefChar->GetTaming() != GetTaming() )
		{
			buff.PutByte( CHARTAG_TAMING );
			buff.PutShort( GetTaming() );
		}

		if( DefChar->GetWeight() != GetWeight() )
		{
			buff.PutByte( CHARTAG_WEIGHT );
			buff.PutShort( GetWeight() );
		}

		if( DefChar->GetHunger() != GetHunger() )
		{
			buff.PutByte( CHARTAG_HUNGER );
			buff.PutByte( GetHunger() );
		}

		if( DefChar->GetFixedLight() != GetFixedLight() )
		{
			buff.PutByte( CHARTAG_FIXEDLIGHT );
			buff.PutByte( GetFixedLight() );
		}

		if( DefChar->GetTown() != GetTown() || DefChar->GetTownVote() != GetTownVote() ||
			DefChar->GetTownTitle() != GetTownTitle() || DefChar->GetPostType() != GetPostType() ||
			DefChar->GetTownPriv() != GetTownPriv() )
		{
			buff.PutByte( CHARTAG_TOWN );
			buff.PutByte( GetTown() );
			buff.PutLong( GetTownVote() );
			buff.PutByte( GetTownTitle() );
			buff.PutByte( GetTownPriv() );
			buff.PutByte( GetPostType() );
		}

		if( DefChar->GetCarve() != GetCarve() )
		{
			buff.PutByte( CHARTAG_CARVE );
			buff.PutLong( GetCarve() );
		}

		if( DefChar->GetHoldG() != GetHoldG() )
		{
			buff.PutByte( CHARTAG_HOLD_G );
			buff.PutLong( GetHoldG() );
		}

		if( DefChar->GetSplit() != GetSplit() || DefChar->GetSplitChance() != GetSplitChance() )
		{
			buff.PutByte( CHARTAG_SPLIT );
			buff.PutByte( GetSplit() );
			buff.PutByte( GetSplitChance() );
		}

		if( DefChar->GetRobe() != GetRobe() )
		{
			buff.PutByte( CHARTAG_ROBE );
			buff.PutLong( GetRobe() );
		}

		if( DefChar->GetGuildFealty() != GetGuildFealty() || DefChar->GetGuildNumber() != GetGuildNumber() ||
			DefChar->GetGuildToggle() != GetGuildToggle() )// || DefChar->IsAtWar() != IsAtWar() )
		{
			buff.PutByte( CHARTAG_GUILD );
			buff.PutLong( GetGuildFealty() );
			buff.PutShort( GetGuildNumber() );
			buff.PutByte( GetGuildToggle() );
//			buff.PutByte( IsAtWar() );
		}

		if( DefChar->GetPoisonSerial() != GetPoisonSerial() )
		{
			buff.PutByte( CHARTAG_POISONSER );
			buff.PutLong( GetPoisonSerial() );
		}

		if( DefChar->GetSummonTimer() != GetSummonTimer() && IsNpc() )
		{
			buff.PutByte( CHARTAG_SUMMONTIMER );
			buff.PutLong( GetSummonTimer() );
		}

		if( DefChar->GetxID() != GetxID() || DefChar->GetOrgID() != DefChar->GetOrgID() )
		{
			buff.PutByte( CHARTAG_ID2 );
			buff.PutShort( GetxID() );
			buff.PutShort( GetOrgID() );
		}

		if( DefChar->GetHairStyle() != GetHairStyle() || DefChar->GetBeardStyle() != GetBeardStyle() ||
			DefChar->GetHairColour() != GetHairColour() || DefChar->GetBeardColour() != GetBeardColour() )
		{
			buff.PutByte( CHARTAG_HAIR );
			buff.PutShort( GetHairStyle() );
			buff.PutShort( GetBeardStyle() );
			buff.PutShort( GetHairColour() );
			buff.PutShort( GetBeardColour() );
		}

		if( DefChar->GetxSkin() != GetxSkin() || DefChar->GetOrgSkin() != GetOrgSkin() )
		{
			buff.PutByte( CHARTAG_SKIN );
			buff.PutShort( GetxSkin() );
			buff.PutShort( GetOrgSkin() );
		}

		if( DefChar->MayLevitate() != MayLevitate() )
		{
			buff.PutByte( CHARTAG_LEV );
			buff.PutByte( MayLevitate() );
		}

		if( DefChar->GetFx( 1 ) != GetFx( 1 ) || DefChar->GetFx( 2 ) != GetFx( 2 ) ||
			DefChar->GetFy( 1 ) != GetFy( 1 ) || DefChar->GetFy( 2 ) != GetFy( 2 ) ||
			DefChar->GetFz() != GetFz() )
		{
			buff.PutByte( CHARTAG_F_X );
			buff.PutShort( GetFx( 1 ) );
			buff.PutShort( GetFx( 2 ) );
			buff.PutShort( GetFy( 1 ) );
			buff.PutShort( GetFy( 2 ) );
			buff.PutByte( GetFz() );
		}

		if( DefChar->GetDispZ() != GetDispZ() )
		{
			buff.PutByte( CHARTAG_DISPZ );
			buff.PutByte( GetDispZ() );
		}

		if( DefChar->GetStealth() != GetStealth() )
		{
			buff.PutByte( CHARTAG_STEALTH );
			buff.PutByte( GetStealth() );
		}

		if( DefChar->GetDir2() != GetDir2() )
		{
			buff.PutByte( CHARTAG_DIR2 );
			buff.PutByte( GetDir2() );
		}

		if( DefChar->GetCell() != GetCell() )
		{
			buff.PutByte( CHARTAG_CELL );
			buff.PutByte( GetCell() );
		}
		
		if( DefChar->GetNpcWander() != GetNpcWander() || DefChar->GetOldNpcWander() != GetOldNpcWander() ||
			DefChar->GetFlySteps() != GetFlySteps() || DefChar->GetRunning() != GetRunning() ||
			DefChar->GetStep() != GetStep() )
		{
			buff.PutByte( CHARTAG_NPCOPTS );
			buff.PutByte( GetNpcWander() );
			buff.PutByte( GetOldNpcWander() );
			buff.PutByte( GetFlySteps() );
			buff.PutByte( GetRunning() );
			buff.PutByte( GetStep() );
		}

		if( DefChar->GetRegion() != GetRegion() )
		{
			buff.PutByte( CHARTAG_REGION );
			buff.PutByte( GetRegion() );
		}

		packItem = GetPackItem();
		if( packItem )
		{
			buff.PutByte( CHARTAG_PACK );
			buff.PutLong( packItem->GetSerial() );
		}
		
		if( DefChar->GetAdvObj() != GetAdvObj() || DefChar->GetRaceGate() != GetRaceGate() )
		{
			buff.PutByte( CHARTAG_GATE );
			buff.PutLong( GetAdvObj() );
			buff.PutShort( GetRaceGate() );
		}

		if( DefChar->GetSpAttack() != GetSpAttack() || DefChar->GetSpDelay() != GetSpDelay() )
		{
			buff.PutByte( CHARTAG_SPA );
			buff.PutShort( GetSpAttack() );
			buff.PutByte( GetSpDelay() );
		}

		if( DefChar->GetQuestType() != GetQuestType() || DefChar->GetQuestDestRegion() != GetQuestDestRegion() ||
			DefChar->GetQuestOrigRegion() != GetQuestOrigRegion() )
		{
			buff.PutByte( CHARTAG_QUEST );
			buff.PutByte( GetQuestType() );
			buff.PutByte( GetQuestDestRegion() );
			buff.PutByte( GetQuestOrigRegion() );
		}

		if( DefChar->GetFleeAt() != GetFleeAt() || DefChar->GetReattackAt() != GetReattackAt() )
		{
			buff.PutByte( CHARTAG_FLEE );
			buff.PutShort( GetFleeAt() );
			buff.PutShort( GetReattackAt() );
		}

		if( DefChar->GetPriv() != GetPriv() || DefChar->GetCommandLevel() != GetCommandLevel() )
		{
			buff.PutByte( CHARTAG_PRIVS );
			buff.PutByte( GetPriv() );
			buff.PutByte( GetCommandLevel() );
		}

		buff.PutByte( CHARTAG_BOOLS );
		buff.PutLong( bools );

		if( DefChar->GetStamina2() != GetStamina2() || DefChar->GetMana2() != GetMana2() )
		{
			buff.PutByte( CHARTAG_STATS2 );
			buff.PutShort( GetMana2() );
			buff.PutShort( GetStamina2() );
		}

		buff.PutByte( CHARTAG_SKILLS );
		buff.PutByte( TRUESKILLS );
		for( bsc = 0; bsc < TRUESKILLS; bsc++ )
			buff.PutShort( GetBaseSkill( bsc ) );
		
		
		if( !IsNpc() )
		{
			buff.PutByte( CHARTAG_ATROPH );
			buff.PutByte( ALLSKILLS );
			for( UI08 atc = 0; atc < ALLSKILLS; atc++ )
				buff.PutShort( GetAtrophy( atc ) );

			buff.PutByte( CHARTAG_SKLOCKS );
			buff.PutByte( TRUESKILLS );
			for( UI08 slc = 0; slc < TRUESKILLS; slc++ )
			{
				if( GetSkillLock( slc ) <= 2 )
					buff.PutByte( GetSkillLock( slc ) );
				else
					buff.PutByte( 0 );
			}
		}

		if( DefChar->GetKarma2() != GetKarma2() || DefChar->GetFame2() != GetFame2() )
		{
			buff.PutByte( CHARTAG_NOTOR2 );
			buff.PutShort( GetKarma2() );
			buff.PutShort( GetFame2() );
		}

		if( DefChar->GetDeaths() != GetDeaths() )
		{
			buff.PutByte( CHARTAG_DEATHS );
			buff.PutShort( GetDeaths() );
		}

		if( DefChar->GetFontType() != GetFontType() || DefChar->GetSquelched() != GetSquelched() )
		{
			buff.PutByte( CHARTAG_FONTSQ );
			buff.PutByte( GetFontType() );
			buff.PutByte( GetSquelched() );
		}

		if( DefChar->GetPoison() != GetPoison() || DefChar->GetPoisoned() != GetPoisoned() )
		{
			buff.PutByte( CHARTAG_POISON );
			buff.PutByte( GetPoison() );
			buff.PutByte( GetPoisoned() );
		}

		FilePosition = outStream.tellp();
		buff.Write( outStream );
		DumpFooter( outStream, mode );

		for( tempCounter = 0; tempCounter < MAXLAYERS; tempCounter++ )
		{
			if( itemLayers[tempCounter] != NULL && itemLayers[tempCounter]->ShouldSave() )
				itemLayers[tempCounter]->Save( outStream, mode );
		}
		break;

	case 0:
	default:
		cBaseObject::DumpBody( outStream, mode );	// Make the default save of BaseObject members now
		dumping << "Account=" << GetAccount() << std::endl;
		dumping << "LastOn=" << GetLastOn() << std::endl;
		dumping << "OrgName=" << GetOrgName() << std::endl;
		dumping << "GuildTitle=" << GetGuildTitle() << std::endl;  
		dumping << "NpcAIType=" << GetNPCAiType() << std::endl;
		dumping << "Making=" << GetMaking() << std::endl;
		dumping << "Taming=" << GetTaming() << std::endl;
		dumping << "Weight=" << GetWeight() << std::endl;
		dumping << "Hunger=" << (SI16)GetHunger() << std::endl;
		dumping << "FixedLight=" << (SI16)GetFixedLight() << std::endl;
		dumping << "Town=" << (SI16)GetTown() << std::endl;
		dumping << "Carve=" << GetCarve() << std::endl;
		dumping << "HoldG=" << GetHoldG() << std::endl;
		dumping << "Split=" << (SI16)GetSplit() << std::endl;
		dumping << "SplitChance=" << (SI16)GetSplitChance() << std::endl;
		dumping << "RobeSerial=" << GetRobe() << std::endl;
		dumping << "TownVote=" << GetTownVote() << std::endl;
		dumping << "GuildFealty=" << GetGuildFealty() << std::endl;  
		dumping << "PoisonSerial=" << GetPoisonSerial() << std::endl;
		dumping << "SummonTimer=" << GetSummonTimer() << std::endl;
		dumping << "XBodyID=" << GetxID() << std::endl;
		dumping << "OriginalBodyID=" << GetOrgID() << std::endl;
		dumping << "HairStyle=" << GetHairStyle() << std::endl;
		dumping << "BeardStyle=" << GetBeardStyle() << std::endl;
		dumping << "XSkinID=" << GetxSkin() << std::endl;
		dumping << "OriginalSkinID=" << GetOrgSkin() << std::endl;
		dumping << "HairColour=" << GetHairColour() << std::endl;
		dumping << "BeardColour=" << GetBeardColour() << std::endl;
		dumping << "Say=" << GetSayColour() << std::endl;
		dumping << "Emotion=" << GetEmoteColour() << std::endl;
		dumping << "MayLevitate=" << (MayLevitate()?"1":"0") << std::endl;
		dumping << "FX1=" << GetFx( 1 ) << std::endl;
		dumping << "FX2=" << GetFx( 2 ) << std::endl;
		dumping << "FY1=" << GetFy( 1 ) << std::endl;
		dumping << "FY2=" << GetFy( 2 ) << std::endl;
		dumping << "FZ1=" << (SI16)GetFz() << std::endl;
		dumping << "DisplayZ=" << (SI16)GetDispZ() << std::endl;
		dumping << "Stealth=" << (SI16)GetStealth() << std::endl;
		dumping << "Dir2=" << (SI16)GetDir2() << std::endl;
		dumping << "Reserved=" << (SI16)GetCell() << std::endl;
		dumping << "NpcWander=" << (SI16)GetNpcWander() << std::endl;
		dumping << "XNpcWander=" << (SI16)GetOldNpcWander() << std::endl;
		dumping << "FlySteps=" << (SI16)GetFlySteps() << std::endl;
		dumping << "Running=" << (SI16)GetRunning() << std::endl;
		dumping << "Step=" << (SI16)GetStep() << std::endl;
		dumping << "Region=" << (SI16)GetRegion() << std::endl;
		CItem *packItem = GetPackItem();
		if( packItem != NULL )
			dumping << "PackItem=" << packItem->GetSerial() << std::endl;	// store something meaningful
		else
			dumping << "PackItem=" << INVALIDSERIAL << std::endl;
		dumping << "AdvanceObject=" << GetAdvObj() << std::endl;
		dumping << "AdvRaceObject=" << GetRaceGate() << std::endl;
		dumping << "SPAttack=" << GetSpAttack() << std::endl;
		dumping << "SpecialAttackDelay=" << (SI16)GetSpDelay() << std::endl;
		dumping << "QuestType=" << (SI16)GetQuestType() << std::endl;	
		dumping << "QuestDestinationRegion=" << (SI16)GetQuestDestRegion() << std::endl;	
		dumping << "QuestOriginalRegion=" << (SI16)GetQuestOrigRegion() << std::endl;	
		dumping << "FleeAt=" << GetFleeAt() << std::endl;
		dumping << "ReAttackAt=" << GetReattackAt() << std::endl;
		dumping << "Privileges=" << (SI16)GetPriv() << std::endl;
		dumping << "CommandLevel=" << (SI16)GetCommandLevel() << std::endl;	// command level
		dumping << "PostType=" << (SI16)GetPostType() << std::endl;
		dumping << "TownPrivileges=" << (SI16)GetTownPriv() << std::endl;
		dumping << "XMana=" << GetMana2() << std::endl;
		dumping << "XStamina=" << GetStamina2() << std::endl;

		// Write out the BaseSkills and the SkillLocks here
		// Format: BaseSkills=[0,34]-[1,255]-[END]
		dumping << "BaseSkills=";
		for( bsc = 0; bsc < TRUESKILLS; bsc++ )
			dumping << "[" << (SI32)bsc << "," << GetBaseSkill( bsc ) << "]-";
		dumping << "[END]" << std::endl;

		if( !IsNpc() )
		{
			dumping << "Atrophy=";
			if( GetAtrophy( 0 ) >= 10 )
				dumping << GetAtrophy( 0 );
			else
				dumping << "0" << GetAtrophy( 0 );

			for( UI08 atc = 1; atc < ALLSKILLS; atc++ )
			{
				if( GetAtrophy( atc ) >= 10 )
					dumping << "," << GetAtrophy( atc );
				else 
					dumping << ",0" << GetAtrophy( atc );
			}
			dumping << "[END]" << std::endl;

			// Format: SkillLocks=[0,34]-[1,255]-[END]
			dumping << "SkillLocks=";
			for( UI08 slc = 0; slc < TRUESKILLS; slc++ )
			{
				if( GetSkillLock( slc ) <= 2 )
					dumping << "[" << (SI16)slc << "," << (SI16)GetSkillLock( slc ) << "]-";
				else
					dumping << "[" << (SI16)slc << ",0]-";
			}
			dumping << "[END]" << std::endl;
		}
		dumping << "XKarma=" << GetKarma2() << std::endl;
		dumping << "XFame=" << GetFame2() << std::endl;
		dumping << "GuildNumber=" << GetGuildNumber() << std::endl;  
		dumping << "Deaths=" << GetDeaths() << std::endl;
		dumping << "FontType=" << (SI16)GetFontType() << std::endl;
		dumping << "Squelched=" << (SI16)GetSquelched() << std::endl;
		dumping << "Poison=" << (SI16)GetPoison() << std::endl;
		dumping << "Poisoned=" << (SI16)GetPoisoned() << std::endl;
		dumping << "TownTitle=" << (GetTownTitle()?1:0) << std::endl;
		//-------------------------------------------------------------------------------------------
		dumping << "CanRun=" << (SI32)((CanRun() && IsNpc())?1:0) << std::endl;
		dumping << "AllMove=" << (SI16)GetPriv2() << std::endl;
		dumping << "IsNpc=" << (SI32)(IsNpc()?1:0) << std::endl;
		dumping << "IsShop=" << (SI32)(IsShop()?1:0) << std::endl;
		dumping << "Dead=" << (SI32)(IsDead()?1:0) << std::endl;
		dumping << "CanTrain=" << (SI32)(CanTrain()?1:0) << std::endl;
		dumping << "IsWarring=" << (SI32)(IsAtWar()?1:0) << std::endl;
		dumping << "GuildToggle=" << (SI32)(GetGuildToggle()?1:0) << std::endl;  

		outStream << dumping.str();

		DumpFooter( outStream, mode );

		for( SI32 tempCounter = 0; tempCounter < MAXLAYERS; tempCounter++ )
		{
			if( itemLayers[tempCounter] != NULL && itemLayers[tempCounter]->ShouldSave() )
				itemLayers[tempCounter]->Save( outStream, mode );
		}
		break;
	}
	return true;
}

//o-----------------------------------------------------------------------o
//|	Function		-	Save(ofstream &outstream, SI32 mode)
//|	Date				-	July 21, 2000
//|	Programmer	-	EviLDeD
//o-----------------------------------------------------------------------o
//|	Returns			-	true/false indicating the success of the write operation
//o-----------------------------------------------------------------------o
bool CChar::Save( std::ofstream &outStream, SI32 mode ) const
{
	if( isFree() )
		return false;
	SI16 mX = GetX();
	SI16 mY = GetY();
	if( mX < 0 || ( mX >= 6144 && mX < 7000 ) )
		return false;
	if( mY < 0 || ( mY >= 4096 && mY < 7000 ) )
		return false;

	DumpHeader( outStream, mode );
	DumpBody( outStream, mode );
	return true;
}

//o--------------------------------------------------------------------------o
//|	Function			-	ACCOUNTSBLOCK &CChar::GetAccount( void )
//|	Date					-	3/13/2003
//|	Developers		-	Abaddon / EviLDeD
//|	Organization	-	UOX3 DevTeam
//|	Status				-	Currently under development
//o--------------------------------------------------------------------------o
//|	Description		-	Returns the account, if any, associated with the CHARACTER
//o--------------------------------------------------------------------------o
//| Modifications	-	
//o--------------------------------------------------------------------------o
ACCOUNTSBLOCK &CChar::GetAccount(void) 
{
	if(IsNpc()||ourAccount.wAccountIndex==AB_INVALID_ID)
	{
		// Set this just in case that this is an NPC.
		ourAccount.wAccountIndex=AB_INVALID_ID;
	}
	//
	return ourAccount;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void BreakConcentration( cSocket *sock = NULL )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Breaks the concentration of the CHARACTER
//|					 sending a message is a socket exists
//o---------------------------------------------------------------------------o
void CChar::BreakConcentration( cSocket *sock )
{
	if( med )
	{
		med = 0;
		if( sock != NULL )
			sysmessage( sock, 100 );
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsUsingPotion( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is using a potion
//o---------------------------------------------------------------------------o
bool CChar::IsUsingPotion( void ) const
{
	return ( (bools&0x10000) == 0x10000 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetUsingPotion( bool newVal )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the CHARACTER is using a potion or not
//o---------------------------------------------------------------------------o
void CChar::SetUsingPotion( bool newVal )
{
	if( newVal )
		bools |= 0x10000;
	else
		bools &= ~0x10000;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CHARLIST *GetPetList( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the list of pets the character owns
//o---------------------------------------------------------------------------o
CHARLIST *CChar::GetPetList( void )
{
	return &petsControlled;
}

//o---------------------------------------------------------------------------o
//|   Function    -  CHARLIST *GetFriendList( void )
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the characters list of friends
//o---------------------------------------------------------------------------o
CHARLIST *CChar::GetFriendList( void )
{
	return &petFriends;
}

//o---------------------------------------------------------------------------o
//|   Function    -  ITEMLIST *GetOwnedItems( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the list of items the char owns
//o---------------------------------------------------------------------------o
ITEMLIST *CChar::GetOwnedItems( void )
{
	return &ownedItems;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddPet( CChar *toAdd ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds the pet toAdd to the player's pet list
//|					 ensuring it is already not on it
//o---------------------------------------------------------------------------o
void CChar::AddPet( CChar *toAdd )
{
	for( UI32 iCounter = 0; iCounter < petsControlled.size(); iCounter++ )
	{
		if( petsControlled[iCounter] == toAdd )
			return;
	}
	petsControlled.push_back( toAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RemovePet( CChar *toRemove )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Removes the pet toRemove from the player's pet list
//o---------------------------------------------------------------------------o
void CChar::RemovePet( CChar *toRemove )
{
	for( UI32 iCounter = 0; iCounter < petsControlled.size(); iCounter++ )
	{
		if( petsControlled[iCounter] == toRemove )
		{
			for( UI32 rCounter = iCounter; rCounter < (petsControlled.size() - 1); rCounter++ )
			{
				petsControlled[rCounter] = petsControlled[rCounter+1];
			}
			petsControlled.resize( petsControlled.size() - 1 );
			return;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddPet( CChar *toAdd ) const
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds the friend toAdd to the player's friends list
//|					 ensuring it is already not on it
//o---------------------------------------------------------------------------o
void CChar::AddFriend( CChar *toAdd )
{
	for( UI32 iCounter = 0; iCounter < petFriends.size(); iCounter++ )
	{
		if( petFriends[iCounter] == toAdd )
			return;
	}
	petFriends.push_back( toAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RemoveFriemd( CChar *toRemove )
//|   Date        -  20 July 2001
//|   Programmer  -  Zane
//o---------------------------------------------------------------------------o
//|   Purpose     -  Removes the friend toRemove from the pets friends list
//o---------------------------------------------------------------------------o
void CChar::RemoveFriend( CChar *toRemove )
{
	for( UI32 iCounter = 0; iCounter < petFriends.size(); iCounter++ )
	{
		if( petFriends[iCounter] == toRemove )
		{
			for( UI32 rCounter = iCounter; rCounter < (petFriends.size() - 1); rCounter++ )
			{
				petFriends[rCounter] = petFriends[rCounter+1];
			}
			petFriends.resize( petFriends.size() - 1 );
			return;
		}
	}
}

//o---------------------------------------------------------------------------o
//|   Function    -  void AddOwnedItem( CItem *toAdd ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Adds the item toAdd to the player's owned list
//|					 ensuring that it is not ALREADY in the list
//o---------------------------------------------------------------------------o
void CChar::AddOwnedItem( CItem *toAdd )
{
	for( UI32 iCounter = 0; iCounter < ownedItems.size(); iCounter++ )
	{
		if( ownedItems[iCounter] == toAdd )		// We already have it
			return;
	}
	ownedItems.push_back( toAdd );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void RemoveOwnedItem( CItem *toRemove )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Removes the item toRemove from the player's owned list
//o---------------------------------------------------------------------------o
void CChar::RemoveOwnedItem( CItem *toRemove )
{
	for( UI32 iCounter = 0; iCounter < ownedItems.size(); iCounter++ )
	{
		if( ownedItems[iCounter] == toRemove )
		{
			for( UI32 rCounter = iCounter; rCounter < (ownedItems.size() - 1); rCounter++ )
			{
				ownedItems[rCounter] = ownedItems[rCounter+1];
			}
			ownedItems.resize( ownedItems.size() - 1 );
			return;
		}
	}
}

bool CChar::MayLevitate( void ) const
{
	return may_levitate;
}

void CChar::MayLevitate( bool newValue )
{
	may_levitate = newValue;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMaxHP( UI16 newmaxhp, SI16 newoldstr, RACEID newoldrace )
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets maximum hitpoints of the object
//o--------------------------------------------------------------------------
void CChar::SetMaxHP( UI16 newmaxhp, SI16 newoldstr, RACEID newoldrace )
{
	maxHP = newmaxhp;
	maxHP_oldstr = newoldstr;
	maxHP_oldrace = newoldrace;
	return;
}

//o--------------------------------------------------------------------------
//|	Function		-	UI16 GetMaxHP()
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns maximum hitpoints of the object
//o--------------------------------------------------------------------------
UI16 CChar::GetMaxHP( void )
{
    if( maxHP_oldstr != GetStrength() || maxHP_oldrace != GetRace() )
	//if str/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );
	
		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );
   
		maxHP = (UI16)(GetStrength() 
			+ (UI16)( ((float)GetStrength()) * ((float)(pRace->HPModifier())) / 100 ));
		// set max. hitpoints to strength + hpmodifier% of strength
		
		maxHP_oldstr = GetStrength();
		maxHP_oldrace = GetRace();

	}
	return maxHP;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMaxMana( UI16 newmaxmana, SI16 newoldint, RACEID newoldrace )
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets maximum mana of the object
//o--------------------------------------------------------------------------
void CChar::SetMaxMana( SI16 newmaxmana, SI16 newoldint, RACEID newoldrace )
{
	maxMana = newmaxmana;
	maxMana_oldint = newoldint;
	maxMana_oldrace = newoldrace;
	return;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetMaxMana()
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns maximum mana of the object
//o--------------------------------------------------------------------------
SI16 CChar::GetMaxMana( void )
{
    if( maxMana_oldint != GetIntelligence() || maxMana_oldrace != GetRace() )
	//if int/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );
	
		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );
   
		maxMana = (SI16)(GetIntelligence() 
			+ (SI16)( ((float)GetIntelligence()) * ((float)(pRace->ManaModifier())) / 100 ));
		// set max. mana to int + manamodifier% of int
		
		maxMana_oldint = GetIntelligence();
		maxMana_oldrace = GetRace();

	}
	return maxMana;
}

//o--------------------------------------------------------------------------
//|	Function		-	void SetMaxStam( UI16 newmaxstam, SI16 newolddex, RACEID newoldrace )
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets maximum stamina of the object
//o--------------------------------------------------------------------------
void CChar::SetMaxStam( SI16 newmaxstam, SI16 newolddex, RACEID newoldrace )
{
	maxStam = newmaxstam;
	maxStam_olddex = newolddex;
	maxStam_oldrace = newoldrace;
	return;
}

//o--------------------------------------------------------------------------
//|	Function		-	SI16 GetMaxStam()
//|	Date			-	15 February, 2002
//|	Programmer		-	sereg
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns maximum stamina of the object
//o--------------------------------------------------------------------------
SI16 CChar::GetMaxStam( void )
{
    if( maxStam_olddex != GetDexterity() || maxStam_oldrace != GetRace() )
	//if dex/race changed since last calculation, recalculate maxhp
	{
		CRace *pRace = Races->Race( GetRace() );
	
		// if race is invalid just use default race
		if( pRace == NULL )
			pRace = Races->Race( 0 );
   
		maxStam = (SI16)(GetDexterity() 
			+ (SI16)( ((float)GetDexterity()) * ((float)(pRace->StamModifier())) / 100 ));
		// set max. stamina to dex + stammodifier% of dex
		
		maxStam_olddex = GetDexterity();
		maxStam_oldrace = GetRace();

	}
	return maxStam;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ActualStrength( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the actual strength (minus mods) of the CHARACTER
//o---------------------------------------------------------------------------o
SI16 CChar::ActualStrength( void ) const
{
	return cBaseObject::GetStrength();
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetStrength( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the strength (incl mods) of the CHARACTER
//o---------------------------------------------------------------------------o
SI16 CChar::GetStrength( void ) const
{
	return (SI16)(cBaseObject::GetStrength() + Strength2());
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ActualIntelligence( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the actual intelligence (minus mods) of the char
//o---------------------------------------------------------------------------o
SI16 CChar::ActualIntelligence( void ) const
{
	return cBaseObject::GetIntelligence();
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetIntelligence( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the intelligence (incl mods) of the CHARACTER
//o---------------------------------------------------------------------------o
SI16 CChar::GetIntelligence( void ) const
{
	return (SI16)(cBaseObject::GetIntelligence() + Intelligence2());
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 ActualDextierty( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the actual (minus mods) dexterity of the CHARACTER
//o---------------------------------------------------------------------------o
SI16 CChar::ActualDexterity( void ) const
{
	return cBaseObject::GetDexterity();
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08 GetDexterity( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the effective dexterity (including modifications)
//|					 of the player
//o---------------------------------------------------------------------------o
SI16 CChar::GetDexterity( void ) const
{
	return (SI16)(cBaseObject::GetDexterity() + Dexterity2());
}

//o---------------------------------------------------------------------------o
//|   Function    -  void IncStrength2( SI08 toAdd = 1 )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Increments strength2 (modifications) by toAdd
//o---------------------------------------------------------------------------o
void CChar::IncStrength2( SI16 toAdd )
{
	Strength2( (SI16)(Strength2() + toAdd) );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void IncDexterity2( SI08 toAdd = 1 )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Increments dexterity2 (modifications) by toAdd
//o---------------------------------------------------------------------------o
void CChar::IncDexterity2( SI16 toAdd )
{
	Dexterity2( (SI16)(Dexterity2() + toAdd) );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void IncSI32elligence2( SI08 toAdd = 1 ) 
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Increments intelligence2 (modifications) by toAdd
//o---------------------------------------------------------------------------o
void CChar::IncIntelligence2( SI16 toAdd )
{
	Intelligence2( (SI16)(Intelligence2() + toAdd) );
}


//o---------------------------------------------------------------------------o
//|   Function    -  bool IsMurderer( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is a murderer
//o---------------------------------------------------------------------------o
bool CChar::IsMurderer( void ) const
{
	return ( (GetFlag()&0x01) == 0x01 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsCriminal( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is a criminal
//o---------------------------------------------------------------------------o
bool CChar::IsCriminal( void ) const
{
	return ( (GetFlag()&0x02) == 0x02 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsInnocent( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER is innocent
//o---------------------------------------------------------------------------o
bool CChar::IsInnocent( void ) const
{
	return ( (GetFlag()&0x04) == 0x04 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void DecHunger( const SI08 amt = 1 )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Decrements the CHARACTER's hunger
//o---------------------------------------------------------------------------o
void CChar::DecHunger( const SI08 amt )
{
	SetHunger( (SI08)(GetHunger() - 1) );
}

void CChar::StopSpell( void )
{
	SetSpellTime( 0 );
	SetCasting( 0 );
	SetSpellCast( -1 );
}

SI16 CChar::GetKarma2( void ) const
{
	return karma2;
}

bool CChar::DumpFooter( std::ofstream &outStream, SI32 mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << std::endl << "o---o" << std::endl << std::endl;
	}
	return true;
}

bool CChar::HandleLine( char *tag, char *data )
{
	if( cBaseObject::HandleLine( tag, data ) )
		return true;
	switch( tag[0] )
	{
	case 'A':
		if( !strcmp( tag, "Account" ) )
		{
			SetAccount( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Atrophy" ) )
		{
			UI16 atrophyNum = 0;
			std::string s(data);
			std::istringstream ss(s);
			char comma;
			for( UI08 aCtr = 0; aCtr < TRUESKILLS; aCtr++ )
			{
				ss >> atrophyNum >> comma;
				SetAtrophy( atrophyNum, aCtr );
			}
			return true;
		}
		else if( !strcmp( tag, "AdvanceObject" ) )
		{
			SetAdvObj( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "AdvRaceObject" ) )
		{
			SetRaceGate( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "AllMove" ) )
		{
			SetPriv2( (UI08)makeNum( data ) );
			return true;
		}
		break;

	case 'B':
		if( !strcmp( tag, "BeardStyle" ) )
		{
			SetBeardStyle( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "BeardColour" ) )
		{
			SetBeardColour( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "BaseSkills" ) )
		{
			UI08 skillNum = 0;
			UI16 skillVal = 0;
			UI08 skillCtr = 0;
			// Format: BaseSkills=[0,34]-[1,255]-[END]

			std::string s( data );
			std::istringstream ss( s );
			char rbracket, lbracket, dash, comma;
			int num, val;
			for( skillCtr = 0; skillCtr < TRUESKILLS; skillCtr++ )
			{
				ss >> lbracket >> num >> comma >> val >> rbracket >> dash;
				skillVal = val; skillNum = num;
				SetBaseSkill( skillVal, skillNum );
			}
			return true;
		}
		break;
	case 'C':
		if( !strcmp( tag, "Carve" ) )
		{
			SetCarve( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "CommandLevel" ) )
		{
			SetCommandLevel( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "CanRun" ) )
		{
			SetRun( makeNum( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "CanTrain" ) )
		{
			SetCanTrain( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'D':
		if( !strcmp( tag, "DisplayZ" ) )
		{
			SetDispZ( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Dir2" ) )
		{
			SetDir2( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Deaths" ) )
		{
			SetDeaths( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Dead" ) )
		{
			SetDead( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'E':
		if( !strcmp( tag, "Emotion" ) )
		{
			SetEmoteColour( (UI16)makeNum( data ) );
			return true;
		}
		break;
	case 'F':
		if( !strcmp( tag, "FixedLight" ) )
		{
			SetFixedLight( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "FX1" ) )
		{
			SetFx( (SI16)makeNum( data ), 1 );
			return true;
		}
		else if( !strcmp( tag, "FX2" ) )
		{
			SetFx( (SI16)makeNum( data ), 2 );
			return true;
		}
		else if( !strcmp( tag, "FY1" ) )
		{
			SetFy( (SI16)makeNum( data ), 1 );
			return true;
		}
		else if( !strcmp( tag, "FY2" ) )
		{
			SetFy( (SI16)makeNum( data ), 2 );
			return true;
		}
		else if( !strcmp( tag, "FZ1" ) )
		{
			SetFz( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "FlySteps" ) )
		{
			SetFlySteps( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "FleeAt" ) )
		{
			SetFleeAt( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "FontType" ) )
		{
			SetFontType( (SI08)makeNum( data ) );
			return true;
		}
		break;
	case 'G':
		if( !strcmp( tag, "GuildFealty" ) )
		{
			SetGuildFealty( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "GuildTitle" ) )
		{
			SetGuildTitle( data );
			return true;
		}
		else if( !strcmp( tag, "GuildNumber" ) )
		{
			SetGuildNumber( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "GuildToggle" ) )
		{
			SetGuildToggle( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'H':
		if( !strcmp( tag, "Hunger" ) )
		{
			SetHunger( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "HoldG" ) )
		{
			SetHoldG( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "HairStyle" ) )
		{
			SetHairStyle( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "HairColour" ) )
		{
			SetHairColour( (UI16)makeNum( data ) );
			return true;
		}
		break;
	case 'I':
		if( !strcmp( tag, "IsNpc" ) )
		{
			SetNpc( makeNum( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "IsShop" ) )
		{
			SetShop( makeNum( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "IsWarring" ) )
		{
			SetWar( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'L':
		if( !strcmp( tag, "LastOn" ) )
		{
			SetLastOn( data );
			return true;
		}
		break;
	case 'M':
		if( !strcmp( tag, "Making" ) )
		{
			SetMaking( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "MayLevitate" ) )
		{
			MayLevitate( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'N':
		if( !strcmp( tag, "NpcAIType" ) )
		{
			SetNPCAiType( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "NpcWander" ) )
		{
			SetNpcWander( (SI08)makeNum( data ) );
			return true;
		}
		break;
	case 'O':
		if( !strcmp( tag, "OrgName" ) )
		{
			SetOrgName( data );
			return true;
		}
		else if( !strcmp( tag, "OriginalBodyID" ) )
		{
			SetOrgID( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "OriginalSkinID" ) )
		{
			SetOrgSkin( (UI16)makeNum( data ) );
			return true;
		}
		break;
	case 'P':
		if( !strcmp( tag, "PoisonSerial" ) )
		{
			SetPoisonSerial( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Privileges" ) )
		{
			SetPriv( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "PostType" ) )
		{
			SetPostType( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Poison" ) )
		{
			SetPoison( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Poisoned" ) )
		{
			SetPoisoned( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "PackItem" ) )
		{
			SERIAL packSer = makeNum( data );
			if( packSer != INVALIDSERIAL )
				SetPackItem( calcItemObjFromSer( packSer ) );
			else
				SetPackItem( NULL );
			return true;
		}
		break;
	case 'Q':
		if( !strcmp( tag, "QuestType" ) )
		{
			SetQuestType( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "QuestDestinationRegion" ) )
		{
			SetQuestDestRegion( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "QuestOriginalRegion" ) )
		{
			SetQuestOrigRegion( (UI08)makeNum( data ) );
			return true;
		}
		break;
	case 'R':
		if( !strcmp( tag, "RobeSerial" ) )
		{
			SetRobe( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Reserved" ) )
		{
			SetCell( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Running" ) )
		{
			SetRunning( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Region" ) )
		{
			SetRegion( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "ReAttackAt" ) )
		{
			SetReattackAt( (SI16)makeNum( data ) );
			return true;
		}
		break;
	case 'S':
		if( !strcmp( tag, "Split" ) )
		{
			SetSplit( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "SplitChance" ) )
		{
			SetSplitChance( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "SummonTimer" ) )
		{
			SetSummonTimer( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Say" ) )
		{
			SetSayColour( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Stealth" ) )
		{
			SetStealth( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Step" ) )
		{
			SetStep( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "SPAttack" ) )
		{
			SetSpAttack( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "SpecialAttackDelay" ) )
		{
			SetSpDelay( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Squelched" ) )
		{
			SetSquelched( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "SkillLocks" ) )
		{
			UI08 lockNum = 0;
			UI08 lockVal = 0;
			UI08 lockCtr = 0;
			// Format: Baselocks=[0,34]-[1,255]-[END]
			std::string s( data );
			std::istringstream ss( s );
			char rbracket, lbracket, dash, comma;
			int num, val;
			for( lockCtr = 0; lockCtr < TRUESKILLS; lockCtr++ )
			{
				ss >> lbracket >> num >> comma >> val >> rbracket >> dash;
				lockNum = num; lockVal = val;
				SetSkillLock( lockVal, lockNum );
			}
			return true;
		}
		break;
	case 'T':
		if( !strcmp( tag, "Taming" ) )
		{
			SetTaming( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "Town" ) )
		{
			SetTown( (UI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "TownVote" ) )
		{
			SetTownVote( makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "TownPrivileges" ) )
		{
			SetTownpriv( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "TownTitle" ) )
		{
			SetTownTitle( makeNum( data ) == 1 );
			return true;
		}
		break;
	case 'W':
		if( !strcmp( tag, "Weight" ) )
		{
			SetWeight( (SI16)makeNum( data ) );
			return true;
		}
		break;
	case 'X':
		if( !strcmp( tag, "XBodyID" ) )
		{
			SetxID( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "XSkinID" ) )
		{
			SetxSkin( (UI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "XNpcWander" ) )
		{
			SetOldNpcWander( (SI08)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "XMana" ) )
		{
			SetMana2( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "XStamina" ) )
		{
			SetStamina2( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "XKarma" ) )
		{
			SetKarma2( (SI16)makeNum( data ) );
			return true;
		}
		else if( !strcmp( tag, "XFame" ) )
		{
			SetFame2( (SI16)makeNum( data ) );
			return true;
		}
		break;
	}
	return false;
}

bool CChar::HandleBinTag( UI08 tag, BinBuffer &buff )
{
	int i;
	SERIAL Ser;
	UI08 temp;

	if( cBaseObject::HandleBinTag( tag, buff ) )
		return true;

	switch ( tag )
	{
	case CHARTAG_ACCOUNT:
		SetAccount( buff.GetLong() );
		break;

	case CHARTAG_TXTCOLOR:
		SetSayColour( buff.GetShort() );
		SetEmoteColour( buff.GetShort() );
		break;

	case CHARTAG_LASTON:
		buff.GetStr( laston, MAX_LASTON );
		break;

	case CHARTAG_ORGNAME:
		buff.GetStr( orgname, MAX_NAME );
		break;

	case CHARTAG_GUILDTITLE:
		buff.GetStr( guildtitle, MAX_GUILDTITLE );
		break;

	case CHARTAG_NPCAI:
		SetNPCAiType( buff.GetShort() );
		break;

	case CHARTAG_MAKING:
		SetMaking( buff.GetLong() );
		break;
	
	case CHARTAG_TAMING:
		SetTaming( buff.GetShort() );
		break;

	case CHARTAG_WEIGHT:
		SetWeight( buff.GetShort() );
		break;

	case CHARTAG_HUNGER:
		SetHunger( buff.GetByte() );
		break;

	case CHARTAG_FIXEDLIGHT:
		SetFixedLight( buff.GetByte() );
		break;

	case CHARTAG_TOWN:
		SetTown( buff.GetByte() );
		SetTownVote( buff.GetLong() );
		SetTownTitle( buff.GetByte() != 0 );
		SetTownpriv( buff.GetByte() );
		SetPostType( buff.GetByte() );
		break;

	case CHARTAG_CARVE:
		SetCarve( buff.GetLong() );
		break;

	case CHARTAG_HOLD_G:
		SetHoldG( buff.GetLong() );
		break;

	case CHARTAG_SPLIT:
		SetSplit( buff.GetByte() );
		SetSplitChance( buff.GetByte() );
		break;

	case CHARTAG_ROBE:
		SetRobe( buff.GetLong() );
		break;

	case CHARTAG_GUILD:
		SetGuildFealty( buff.GetLong() );
		SetGuildNumber( buff.GetShort() );
		SetGuildToggle( buff.GetByte() != 0 );
		break;

	case CHARTAG_POISONSER:
		SetPoisonSerial( buff.GetLong() );
		break;

	case CHARTAG_SUMMONTIMER:
		SetSummonTimer( buff.GetLong() );
		break;

	case CHARTAG_ID2:
		SetxID( buff.GetShort() );
		SetOrgID( buff.GetShort() );
		break;

	case CHARTAG_HAIR:
		SetHairStyle( buff.GetShort() );
		SetBeardStyle( buff.GetShort() );
		SetHairColour( buff.GetShort() );
		SetBeardColour( buff.GetShort() );
		break;

	case CHARTAG_SKIN:
		SetxSkin( buff.GetShort() );
		SetOrgSkin( buff.GetShort() );
		break;

	case CHARTAG_LEV:
		MayLevitate( buff.GetByte() == 0 ? false : true );
		break;

	case CHARTAG_F_X:
		SetFx( buff.GetShort(), 1 );
		SetFx( buff.GetShort(), 2 );
		SetFy( buff.GetShort(), 1 );
		SetFy( buff.GetShort(), 2 );
		SetFz( buff.GetByte() );
		break;

	case CHARTAG_DISPZ:
		SetDispZ( buff.GetByte() );
		break;

	case CHARTAG_STEALTH:
		SetStealth( buff.GetByte() );
		break;

	case CHARTAG_DIR2:
		SetDir2( buff.GetByte() );
		break;

	case CHARTAG_CELL:
		SetCell( buff.GetByte() );
		break;

	case CHARTAG_NPCOPTS:
		SetNpcWander( buff.GetByte() );
		SetOldNpcWander( buff.GetByte() );
		SetFlySteps( buff.GetByte() );
		SetRunning( buff.GetByte() );
		SetStep( buff.GetByte() );
		break;

	case CHARTAG_REGION:
		SetRegion( buff.GetByte() );
		break;

	case CHARTAG_PACK:
		Ser = buff.GetLong();
		if( Ser != INVALIDSERIAL )
			SetPackItem( calcItemObjFromSer( Ser ) );
		else
			SetPackItem( NULL );
		break;

	case CHARTAG_GATE:
		SetAdvObj( buff.GetLong() );
		SetRaceGate( buff.GetShort() );
		break;

	case CHARTAG_SPA:
		SetSpAttack( buff.GetShort() );
		SetSpDelay( buff.GetByte() );
		break;

	case CHARTAG_QUEST:
		SetQuestType( buff.GetByte() );
		SetQuestDestRegion( buff.GetByte() );
		SetQuestOrigRegion( buff.GetByte() );
		break;

	case CHARTAG_FLEE:
		SetFleeAt( buff.GetShort() );
		SetReattackAt( buff.GetShort() );
		break;

	case CHARTAG_PRIVS:
		SetPriv( buff.GetByte() );
		SetCommandLevel( buff.GetByte() );
		break;

	case CHARTAG_BOOLS:
		bools = buff.GetLong();
		break;

	case CHARTAG_STATS2:
		SetMana2( buff.GetShort() );
		SetStamina2( buff.GetShort() );
		break;

	case CHARTAG_SKILLS:
		temp = buff.GetByte();
		for( i = 0; i < TRUESKILLS; i++ )
			SetBaseSkill( buff.GetShort(), i );
		break;

	case CHARTAG_ATROPH:
		temp = buff.GetByte();
		for( i = 0; i < ALLSKILLS; i++ )
			SetAtrophy( buff.GetShort(), i );
		break;

	case CHARTAG_SKLOCKS:
		temp = buff.GetByte();
		for( i = 0; i < TRUESKILLS; i++ )
			SetSkillLock( buff.GetByte(), i );
		break;

	case CHARTAG_NOTOR2:
		SetKarma2( buff.GetShort() );
		SetFame2( buff.GetShort() );
		break;

	case CHARTAG_DEATHS:
		SetDeaths( buff.GetShort() );
		break;

	case CHARTAG_FONTSQ:
		SetFontType( buff.GetByte() );
		SetSquelched( buff.GetByte() );
		break;

	case CHARTAG_POISON:
		SetPoison( buff.GetByte() );
		SetPoisoned( buff.GetByte() );
		break;

	default: 
		return false;
	}
	return true;
}

bool CChar::Load( BinBuffer &buff, CHARACTER arrayOffset )
{
	UI08 tag = 0;
	UI08 ltag;

	isDirty = buff.GetByte();
	// Remove this when dirty flagging works right and activate commented code at the end")
	isDirty = 1;
	serial = (UI32)buff.GetLong();

	x = buff.GetShort();
	y = buff.GetShort();
	z = (SI08)buff.GetByte();
	worldNumber = buff.GetByte();
	dir = buff.GetByte();

	id = (UI16)buff.GetShort();

	buff.GetStr( name, MAX_NAME );

	while( !buff.End() )
	{
		ltag = tag;
		tag = buff.GetByte();
		if( !buff.End() )
		{
			if( !HandleBinTag( tag, buff ) )
				Console.Warning( 2, "Unknown CChar world file tag %i length of %i. (Last Tag %i)", tag, CharSaveTable[tag], ltag );
		}
	}
	// We have to do this here, otherwise we return prematurely, WITHOUT having read the entire record!
//	if( isDirty )
//		return false;
	return LoadRemnants( arrayOffset );
}

bool CChar::Load( std::ifstream &inStream, CHARACTER arrayOffset )
{
	char tag[128], data[512];
	bool bFinished;

	bFinished = false;
	while( !bFinished)
	{
		ReadWorldTagData( inStream, tag, data );
		bFinished = ( strcmp( tag, "o---o" ) == 0 );
		if( !bFinished )
		{
			if( !HandleLine( tag, data ) )
			{
				Console.Warning( 2, "Unknown world file tag %s with contents of %s", tag, data );
			}
		}
	}
	return LoadRemnants( arrayOffset );
}

//o--------------------------------------------------------------------------
//|	Function		-	bool LoadRemnants( int arrayOffset )
//|	Date			-	21st January, 2002
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	After handling data specific load, other parts go here
//o--------------------------------------------------------------------------
bool CChar::LoadRemnants( int arrayOffset )
{
	if( charcount2 <= serial ) 
		charcount2 = serial + 1;
	SetSerial( serial, arrayOffset );

	if( IsNpc() && IsAtWar() ) 
		SetWar( false );

	if( IsCounselor() && GetCommandLevel() < CNSCMDLEVEL && !IsGM() ) // SI32erim line to retain compatibility, MUST BE TAKEN out in the SI32 term!
		SetCommandLevel( CNSCMDLEVEL );
	if( IsGM() && GetCommandLevel() < GMCMDLEVEL )	// SI32erim line to retain compatibility, MUST BE TAKEN out in the SI32 term!
		SetCommandLevel( GMCMDLEVEL );
	//if(ourAccount->lpaarHolding->->bFlags|=0x8000)
	//	SetCommandLevel( GMCMDLEVEL );
	////////////////////////////////////////////////////////////////////

	SetRegion( calcRegionFromXY( GetX(), GetY(), worldNumber ) );
	SetAntiSpamTimer( 0 );
	if( GetID() != GetOrgID() )
		SetID( GetOrgID() );

	UI16 k = GetID();
	if( k > 0x3E1 )
	{ 
		if(GetAccount().wAccountIndex==0xffff) 
		{ 
			Console << "npc: " << GetSerial() << "[" << GetName() << "] with bugged body value detected, deleted for stability reasons" << myendl;
			return false;
		} 
		else 
			SetID( 0x0190 );
	}

	MapRegion->AddChar( this );

	SI16 mx = GetX();
	SI16 my = GetY();
	SI32 acct = GetAccount().wAccountIndex;

	bool overRight = ( mx > MapWidths[worldNumber] );
	bool overBottom = ( my > MapHeights[worldNumber] );

	if( acct == -1 && ( ( overRight && mx < 7000 ) || ( overBottom && my < 7000 ) || mx < 0 || my < 0 ) )
	{
		if( IsNpc() )
			return false;
		else
			SetLocation( 1000, 1000, 0 );
	}

	if( !IsNpc() )
		Accounts->AddCharacter(GetAccount().wAccountIndex, this );	// account ID, and account object.
 	return true;
}

//o--------------------------------------------------------------------------
//|	Function		-	bool SkillUsed( UI08 skillNum )
//|	Date			-	May 2, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Returns true if the skill is being used
//o--------------------------------------------------------------------------
bool CChar::SkillUsed( UI08 skillNum ) const
{
	if( skillNum >= TRUESKILLS )
		return false;
	UI08 part = (UI08)(skillNum / 32);
	UI08 offset = (UI08)(skillNum % 32);
	UI32 compMask = power( 2, offset );
	return ( ( skillUsed[part] & compMask ) == compMask );
	
}

//o--------------------------------------------------------------------------
//|	Function		-	void SkillUsed( bool value, UI08 skillNum )
//|	Date			-	May 2, 2001
//|	Programmer		-	Abaddon
//|	Modified		-
//o--------------------------------------------------------------------------
//|	Purpose			-	Sets whether the skill skillNum is being used or not
//o--------------------------------------------------------------------------
void CChar::SkillUsed( bool value, UI08 skillNum )
{
	if( skillNum >= TRUESKILLS )
		return;
	UI08 part = (UI08)(skillNum / 32);
	UI08 offset = (UI08)(skillNum % 32);
	UI32 compMask = power( 2, offset );
	if( value )
		skillUsed[part] |= compMask;
	else
	{
		UI32 flagMask = 0xFFFFFFFF;
		flagMask ^= compMask;
		skillUsed[part] &= flagMask;
	}
}

UI08 CChar::PopDirection( void )
{
	if( pathToFollow.empty() )
		return 0;

	UI08 toReturn = pathToFollow.front();	
	pathToFollow.pop();	
	return toReturn;
}
void CChar::PushDirection( UI08 newDir )
{
	pathToFollow.push( newDir );
}

bool CChar::StillGotDirs( void ) const
{
	return !pathToFollow.empty();
}

void CChar::FlushPath( void )
{
	while( StillGotDirs() )
		PopDirection();
}

void CChar::PostLoadProcessing( UI32 index )
{
	cBaseObject::PostLoadProcessing( index );
	if( packitem != NULL )
		SetPackItem( packitem );		// we stored the serial in packitem
// Thyme - no longer required
// This is done within the cSpawnRegion class now.
//	
//	if( GetSpawn( 1 ) <= 0x40 && GetSpawn() != 0  )		// Not an item (region spawn)
//	{
//		cSpawnRegion *spawnReg = spawnregion[GetSpawn( 3 )];
//		if( spawnReg != NULL )
//			spawnReg->SetCurrent( spawnReg->GetCurrent() + 1 );
//	}
//	
	if( GetWeight() <= 0 )
		Weight->calcWeight( this );
	for( UI08 i = 0; i < TRUESKILLS; i++ )
		Skills->updateSkillLevel( this, i );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsJailed( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is in a jail cell
//o---------------------------------------------------------------------------o
bool CChar::IsJailed( void ) const
{
	return( GetCell() != -1 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  bool IsMounted( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Unknown
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the character is mounted
//o---------------------------------------------------------------------------o
bool CChar::IsMounted( void ) const
{
	return( GetX() >= 7000 || GetY() >= 7000 );
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFlagRed( void )
//|   Date        -  2nd October, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the character's flag to reflect murderer status
//o---------------------------------------------------------------------------o
void CChar::SetFlagRed( void )
{
	flag |= 0x01;
	flag &= 0x1B;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFlagBlue( void )
//|   Date        -  2nd October, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the character's flag to reflect innocence
//o---------------------------------------------------------------------------o
void CChar::SetFlagBlue( void )
{
	flag |= 0x04;
	flag &= 0x1C;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetFlagGray( void )
//|   Date        -  2nd October, 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Updates the character's flag to reflect criminality
//o---------------------------------------------------------------------------o
void CChar::SetFlagGray( void )
{
	flag |= 0x02;
	flag &= 0x1B;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetSpeechID
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the ID for the JS Speech Input
//o---------------------------------------------------------------------------o
void CChar::SetSpeechID( UI08 newValue )
{
	speechID = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SetSpeechCallback
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the Callback for the onSpeechInput function
//o---------------------------------------------------------------------------o
void CChar::SetSpeechCallback( UI16 newValue )
{
	speechCallback = newValue;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechID
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Gets the ID for the JS Speech Input
//o---------------------------------------------------------------------------o
UI08 CChar::GetSpeechID( void ) const
{
	return speechID;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechCallback
//|   Date        -  January 20th, 2002
//|   Programmer  -  Dark-Storm
//o---------------------------------------------------------------------------o
//|   Purpose     -  Gets the Callback for the onSpeechInput function
//o---------------------------------------------------------------------------o
UI16 CChar::GetSpeechCallback( void ) const
{
	return speechCallback;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::IsInBank( CItem* i )
//|   Date        -  May 3rd, 2002
//|   Programmer  -  sereg
//o---------------------------------------------------------------------------o
//|   Purpose     -  Determines if the item is in the player's bank box (recursively)
//o---------------------------------------------------------------------------o
bool CChar::IsInBank( CItem* i )
{
	if( i->GetMoreX() == 1 )
		return true;
	else if( i->GetCont() != INVALIDSERIAL && i->GetCont() >= 0x40000000 )
		return this->IsInBank( calcItemObjFromSer( i->GetCont() ) );
	else
		return false;
}

