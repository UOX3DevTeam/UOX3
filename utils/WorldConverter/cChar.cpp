#include "uox3.h"
#include "power.h"

#include <sstream>

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
//|   Function    -  SI32 GetMaking( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the making attribute.  Used for making items
//o---------------------------------------------------------------------------o
SI32 CChar::GetMaking( void ) const
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

//o---------------------------------------------------------------------------o
//|   Function    -  void SetAccount( SI32 newAccount )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the account associated with the CHARACTER
//o---------------------------------------------------------------------------o
void CChar::SetAccount( SI32 newVal )
{
	// note: Need to update so that it copes with the OBJ change
	account = newVal;
}

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

//o---------------------------------------------------------------------------o
//|   Function    -  void SetMaking( SI32 newValue )
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets the CHARACTER's making value
//o---------------------------------------------------------------------------o
void CChar::SetMaking( SI32 newValue )
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

//o---------------------------------------------------------------------------o
//|   Function    -  bool BankBoxOpen( void ) const
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns true if the CHARACTER's bank box is open
//o---------------------------------------------------------------------------o
bool CChar::BankBoxOpen( void ) const
{
	return ( (bools&0x8000) == 0x8000 );
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
		bools = bools|0x01;
	else
		bools = bools&0xFFFE;
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
		bools = bools|0x02;
	else
		bools = bools&0xFFFD;
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
		bools = bools|0x04;
	else
		bools = bools&0xFFFB;
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
		bools = bools|0x08;
	else
		bools = bools&0xFFF7;
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
		bools = bools|0x10;
	else
		bools = bools&0xFFEF;
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
		bools = bools|0x20;
	else
		bools = bools&0xFFDF;
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
		bools = bools|0x40;
	else
		bools = bools&0xFFBF;
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
		bools = bools|0x80;
	else
		bools = bools&0xFF7F;
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
		bools = bools|0x100;
	else
		bools = bools&0xFEFF;
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
		bools = bools|0x200;
	else
		bools = bools&0xFDFF;
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
		bools = bools|0x400;
	else
		bools = bools&0xFBFF;
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
		bools = bools|0x800;
	else
		bools = bools&0xF7FF;
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
		bools = bools|0x1000;
	else
		bools = bools&0xEFFF;
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
		bools = bools|0x2000;
	else
		bools = bools&0xDFFF;
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
		bools = bools|0x4000;
	else
		bools = bools&0xBFFF;
}

//o---------------------------------------------------------------------------o
//|   Function    -  void SetBankBoxOpen( bool newVal ) 
//|   Date        -  Unknown
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sets whether the SI08's bank box is open
//o---------------------------------------------------------------------------o
void CChar::SetBankBoxOpen( bool newValue )
{
	if( newValue )
		bools = bools|0x8000;
	else
		bools = bools&0x7FFF;
}

void CChar::SetSerial( UI32 newSerial, CHARACTER c )
{
	// remove from hash table
	cBaseObject::SetSerial( newSerial );
	if( c != -1 )
		ncharsp.AddSerial( newSerial, c );
	// add to hash table
}
void CChar::SetSerial( UI08 newSerial, UI08 newPart, CHARACTER c )
{
	cBaseObject::SetSerial( newSerial, newPart );
	if( c != -1 )
		ncharsp.AddSerial( serial, c );
}

void CChar::SetRobe( UI08 value, UI08 part )
{
	UI08 part1, part2, part3, part4;
	part1 = (UI08)(robe>>24);
	part2 = (UI08)(robe>>16);
	part3 = (UI08)(robe>>8);
	part4 = (UI08)(robe%256);
	switch( part )
	{
	case 1:		part1 = value;		break;
	case 2:		part2 = value;		break;
	case 3:		part3 = value;		break;
	case 4:		part4 = value;		break;
	}
	robe = (part1<<24) + (part2<<16) + (part3<<8) + part4;
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
	if( GetOwner() != 0xFFFFFFFF )
	{
		CChar *oldOwner = calcCharObjFromSer( GetOwner() );
		if( oldOwner != NULL )
		{
			oldOwner->RemovePet( this );
		}
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
	if( GetOwner() != 0xFFFFFFFF )
	{
		CChar *newOwner = calcCharObjFromSer( GetOwner() );
		if( newOwner != NULL )
		{
			newOwner->AddPet( this );
			SetTamed( true );
		}
	}
	else
		SetTamed( false );
}

void CChar::SetOwner( UI08 value, UI08 part )
{
	RemoveSelfFromOwner();
	cBaseObject::SetOwner( value, part );
	AddSelfToOwner();
}

void CChar::SetOwner( UI32 newValue )
{
	RemoveSelfFromOwner();
	cBaseObject::SetOwner( newValue );
	AddSelfToOwner();
}

void CChar::SetSpawn( UI08 newVal, UI08 part, CHARACTER c )
{
	cBaseObject::SetSpawn( newVal, part );
}

void CChar::SetSpawn( UI32 newValue, CHARACTER c )
{
	cBaseObject::SetSpawn( newValue );
}
void CChar::SetTownVote( UI08 newValue, UI08 part )
{
	UI08 part1, part2, part3, part4;
	part1 = (UI08)(townvote>>24);
	part2 = (UI08)(townvote>>16);
	part3 = (UI08)(townvote>>8);
	part4 = (UI08)(townvote%256);
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

void CChar::SetxID( UI08 value, UI08 part )
{
	UI08 part1, part2;
	part1 = (UI08)(xid>>8);
	part2 = (UI08)(xid%256);
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
	UI08 part1, part2;
	part1 = (UI08)(orgid>>8);
	part2 = (UI08)(orgid%256);
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
	UI08 part1, part2;
	part1 = (UI08)(hairstyle>>8);
	part2 = (UI08)(hairstyle%256);
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
	UI08 part1, part2;
	part1 = (UI08)(beardstyle>>8);
	part2 = (UI08)(beardstyle%256);
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
	UI08 part1, part2;
	part1 = (UI08)(envokeid>>8);
	part2 = (UI08)(envokeid%256);
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
	UI08 part1, part2;
	part1 = (UI08)(orgSkin>>8);
	part2 = (UI08)(orgSkin%256);
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
	UI08 part1, part2;
	part1 = (UI08)(xskin>>8);
	part2 = (UI08)(xskin%256);
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
	UI08 part1, part2;
	part1 = (UI08)(haircolor>>8);
	part2 = (UI08)(haircolor%256);
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
	UI08 part1, part2;
	part1 = (UI08)(beardcolour>>8);
	part2 = (UI08)(beardcolour%256);
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
	UI08 part1, part2;
	part1 = (UI08)(emotecolor>>8);
	part2 = (UI08)(emotecolor%256);
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
	UI08 part1, part2;
	part1 = (UI08)(saycolor>>8);
	part2 = (UI08)(saycolor%256);
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

SI16 CChar::GetOldX( void ) const
{
	return oldx;
}
SI16 CChar::GetOldY( void ) const
{
	return oldy;
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
SI08 CChar::GetOldZ( void ) const
{
	return oldz;
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
void CChar::SetLocation( SI16 newX, SI16 newY, SI08 newZ, SI08 newDispZ )
{
	MapRegion->RemoveChar( this );
	x = newX;
	y = newY;
	z = newZ;
	dispz = newDispZ;
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
	SetLocation( (SI16)targ.x, (SI16)targ.y, (SI08)targ.z, (SI08)targ.z );
}
void CChar::SetOldX( SI16 oldXToSet )
{
	oldx = oldXToSet;
}
void CChar::SetOldY( SI16 oldYToSet )
{
	oldy = oldYToSet;
}
void CChar::SetOldZ( SI08 oldZToSet )
{
	oldz = oldZToSet;
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

SI32 CChar::GetPackItem( void ) const
{
	return packitem;
}
CItem * CChar::GetPackItemObj( void ) const
{
	if( packitem == -1 || packitem > imem )
		return NULL;
	else
		return &items[packitem];
}
SI32 CChar::GetTarg( void ) const
{
	return targ;
}
SI32 CChar::GetAttacker( void ) const
{
	return attacker;
}
SI32 CChar::GetFTarg( void ) const
{
	return ftarg;
}
SI32 CChar::GetSmeltItem( void ) const
{
	return smeltitem;
}
SI32 CChar::GetTailItem( void ) const
{
	return tailitem;
}
SI32 CChar::GetAdvObj( void ) const
{
	return advobj;
}
SI32 CChar::GetEnvokeItem( void ) const
{
	return envokeitem;
}
SI32 CChar::GetSwingTarg( void ) const
{
	return swingtarg;
}
UI32 CChar::GetTrackingTarget( void ) const
{
	return trackingtarget;
}
UI32 CChar::GetTrackingTargets( UI08 targetNum ) const
{
	return trackingtargets[targetNum];
}
RACEID CChar::GetRaceGate( void ) const
{
	return raceGate;
}

void CChar::SetPackItem( SI32 newVal )
{
	packitem = newVal;
}
void CChar::SetTarg( SI32 newTarg )
{
	targ = newTarg;
}
void CChar::SetAttacker( SI32 newValue )
{
	attacker = newValue;
}
void CChar::SetFTarg( SI32 newTarg )
{
	ftarg = newTarg;
}
void CChar::SetTailItem( SI32 newValue )
{
	tailitem = newValue;
}
void CChar::SetSmeltItem( SI32 newValue )
{
	smeltitem = newValue;
}
void CChar::SetAdvObj( SI32 newValue )
{
	advobj = newValue;
}
void CChar::SetEnvokeItem( SI32 newValue )
{
	envokeitem = newValue;
}
void CChar::SetSwingTarg( SI32 newValue )
{
	swingtarg = newValue;
}
void CChar::SetTrackingTarget( UI32 newValue )
{
	trackingtarget = newValue;
}
void CChar::SetRaceGate( RACEID newValue )
{
	raceGate = newValue;
}
void CChar::SetTrackingTargets( UI32 newValue, UI08 targetNum )
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
	return skill[skillToGet];
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

SI32 CChar::GetPoison( void ) const
{
	return poison;
}
SI32 CChar::GetPoisoned( void ) const
{
	return poisoned;
}

void CChar::SetPoison( SI32 newValue )
{
	poison = newValue;
}
void CChar::SetPoisoned( SI32 newValue )
{
	poisoned = newValue;
}

CChar::CChar( CHARACTER c, bool zeroSer ) : robe( INVALIDSERIAL ), townvote( INVALIDSERIAL ), bools( 0 ), antispamtimer( 0 ), 
account( -1 ), dispz( 0 ), oldx( 0 ), oldy( 0 ), oldz( 0 ), xskin( colour ), fonttype( 3 ),
saycolor( 0x1700 ), emotecolor( 0x0023 ), mn2( 0 ), cell( 0 ), deaths( 0 ), packitem( -1 ), fixedlight( 255 ), weight( 0 ), 
targ( -1 ), timeout( 0 ), attacker( -1 ), npcmovetime( 0 ), npcWander( 0 ), oldnpcWander( 0 ), ftarg( -1 ), fx1( -1 ),
fx2( -1 ), fy1( -1 ), fy2( -1 ), fz1( -1 ), invistimeout( 0 ), hunger( 6 ), hungertime( 0 ), smeltitem( -1 ), tailitem( -1 ),
npcaitype( 0 ), callnum( -1 ), playercallnum( -1 ), pagegm( 0 ), region( 255 ), skilldelay( 0 ), objectdelay( 0 ), making( -1 ),
blocked( 0 ), dir2( 0 ), spiritspeaktimer( 0 ), spattack( 0 ), spadelay( 0 ), spatimer( 0 ), taming( 0 ), summontimer( 0 ),
trackingtimer( 0 ), trackingtarget( 0 ), fishingtimer( 0 ), town( 255 ), townpriv( 0 ), advobj( 0 ), poison( 0 ), poisoned( 0 ),
poisontime( 0 ), poisontxt( 0 ), poisonwearofftime( 0 ), fleeat( 0 ), reattackat( 0 ), envokeid( 0 ), envokeitem( -1 ), split( 0 ),
splitchnc( 0 ), trainer( 0 ), trainingplayerin( 0 ), guildfealty( INVALIDSERIAL ), guildnumber( -1 ), flag( 0x04 ), murderrate( 0 ),
crimflag( -1 ), casting( 0 ), spelltime( 0 ), spellCast( -1 ), spellaction( 0 ), nextact( 0 ), poisonserial( INVALIDSERIAL ),
squelched( 0 ), mutetime( 0 ), med( 0 ), stealth( -1 ), running( 0 ), logout( 0 ), swingtarg( -1 ), holdg( 0 ), raceGate( 65535 ),
fly_steps( 0 ), smoketimer( 0 ), smokedisplaytimer( 0 ), carve( -1 ), commandLevel( 0 ), postType( 0 ),
questType( 0 ), questDestRegion( 0 ), questOrigRegion( 0 ), fame2( 0 ), karma2( 0 ),
stm2( 0 ), hairstyle( 0xFFFF ), beardstyle( 0xFFFF ), haircolor( 0xFFFF ), beardcolour( 0xFFFF ), orgSkin( colour )
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
	for( SI32 i = 0; i < MAXLAYERS; i++ )
		itemLayers[i].resize( 0 );
	petsControlled.resize( 0 );
	strcpy( name, "Mr. noname" );
	title[0] = 0x00;
	orgname[0] = 0x00;
	SetPriv( 0 );	
	SetPriv2( 0 );	
	for( SI32 k = 0; k <= ALLSKILLS; k++ )
		baseskill[k] = 10;
	memset( skill, 0, sizeof( skill[0] ) * (ALLSKILLS+1) );
	memset( lockState, 0, sizeof( lockState[0] ) * ALLSKILLS );
	for( UI16 j = 0; j <= ALLSKILLS; j++ )
		atrophy[j] = j;
	memset( trackingtargets, 0, sizeof( trackingtargets[0] ) * MAXTRACKINGTARGETS );
	SetCanTrain( true );
	laston[0] = '\x00'; //Last time a CHARACTER was on
	// Begin of Guild Related CHARACTER information (DasRaetsel)
	guildtitle[0]='\x00';	// Title Guildmaster granted player						(DasRaetsel)
    //initialize weatherdamage

	memset( weathDamage, 0, sizeof( weathDamage[0] ) * WEATHNUM );
}

CChar::~CChar()
{
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
		priv &= 0xFFFE;
}
void CChar::SetBroadcast( bool newValue )
{
	if( newValue )
		priv |= 0x0002;
	else
		priv &= 0xFFFD;
}
void CChar::SetInvulnerable( bool newValue )
{
	if( newValue )
		priv |= 0x0004;
	else
		priv &= 0xFFFB;
}
void CChar::SetSingClickSer( bool newValue )
{
	if( newValue )
		priv |= 0x0008;
	else
		priv &= 0xFFF7;
}
void CChar::SetSkillTitles( bool newValue )
{
	if( newValue )
		priv |= 0x0010;
	else
		priv &= 0xFFEF;
}
void CChar::SetGMPageable( bool newValue )
{
	if( newValue )
		priv |= 0x0020;
	else
		priv &= 0xFFDF;
}
void CChar::SetSnoop( bool newValue )
{
	if( newValue )
		priv |= 0x0040;
	else
		priv &= 0xFFBF;
}
void CChar::SetCounselor( bool newValue )
{
	if( newValue )
		priv |= 0x0080;
	else
		priv &= 0xFF7F;
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
		priv &= 0xFEFF;
}
void CChar::SetFrozen( bool newValue )
{
	if( newValue )
		priv |= 0x0200;
	else
		priv &= 0xFDFF;
}
void CChar::SetViewHouseAsIcon( bool newValue )
{
	if( newValue )
		priv |= 0x0400;
	else
		priv &= 0xFBFF;
}
void CChar::SetPermHidden( bool newValue )
{
	if( newValue )
		priv |= 0x0800;
	else
		priv &= 0xF7FF;
}
void CChar::SetNoNeedMana( bool newValue )
{
	if( newValue )
		priv |= 0x1000;
	else
		priv &= 0xEFFF;
}
void CChar::SetDispellable( bool newValue )
{
	if( newValue )
		priv |= 0x2000;
	else
		priv &= 0xDFFF;
}
void CChar::SetPermReflected( bool newValue )
{
	if( newValue )
		priv |= 0x4000;
	else
		priv &= 0xBFFF;
}
void CChar::SetNoNeedReags( bool newValue )
{
	if( newValue )
		priv |= 0x8000;
	else
		priv &= 0x7FFF;
}

void CChar::Dupe( CChar& target, CHARACTER c )
{

	target.SetMulti( GetMultiObj() );
	target.SetOwner( GetOwner() );
	target.SetRobe( robe );
	target.SetSpawn( GetSpawn(), c );
	target.SetTownVote( townvote );
	target.SetPriv( GetPriv() );
	target.SetPriv2( GetPriv2() );
	target.SetName( name );
	target.SetTitle( title );
	target.SetOrgName( orgname );
	target.SetAntiSpamTimer( antispamtimer );
	target.SetAccount( account );
	target.SetX( x );
	target.SetY( y );
	target.SetZ( z );
	target.SetDispZ( dispz );
	target.SetOldX( oldx );
	target.SetOldY( oldy );
	target.SetOldZ( oldz );
	target.SetDir( dir );
	target.SetID( id );
	target.SetxID( xid );
	target.SetOrgID( orgid );
	target.SetSkin( GetColour() );
	target.SetxSkin( xskin );
	target.SetFontType( fonttype );
	target.SetSayColour( saycolor );
	target.SetEmoteColour( emotecolor );
	target.SetStrength( strength );
	target.SetDexterity( dexterity );
	target.SetIntelligence( intelligence );
	target.SetHP(  hitpoints );
	target.SetStamina( stm2 );
	target.SetMana( mn2 );

	target.Strength2( st2 );
	target.Dexterity2( dx2 );
	target.Intelligence2( in2 );
	target.SetMana2( mn2 );
	target.SetStamina2( stm2 );

	target.SetHiDamage( hidamage );
	target.SetLoDamage( lodamage );

	for( UI08 i = 0; i <= ALLSKILLS; i++ )
	{
		target.SetBaseSkill( baseskill[i], i );
		target.SetSkill( skill[i], i );
		target.SetAtrophy( atrophy[i], i );
		target.SetSkillLock( lockState[i], i );
	}

	target.SetCell( cell );
	target.SetKarma( karma );
	target.SetFame( fame );
	target.SetKills( kills );
	target.SetDeaths( deaths );
	target.SetPackItem( packitem );
	target.SetFixedLight( fixedlight );
	target.SetWeight( weight );
	target.SetDef( def );
	target.SetTarg( targ );
	target.SetTimeout( timeout );
	target.SetRegen( regen[0], 0 );
	target.SetRegen( regen[1], 1 );
	target.SetRegen( regen[2], 2 );
	target.SetAttacker( attacker );
	target.SetNpcMoveTime( npcmovetime );
	target.SetNpcWander( npcWander );
	target.SetOldNpcWander( oldnpcWander );
	target.SetFTarg( ftarg );
	target.SetFx( fx1, 1 );
	target.SetFx( fx2, 2 );
	target.SetFy( fy1, 1 );
	target.SetFy( fy2, 2 );
	target.SetFz( fz1 );
	target.SetVisible( GetVisible() );
	target.SetInvisTimeout( invistimeout );
	target.SetHunger( hunger );
	target.SetHungerTime( hungertime );
	target.SetSmeltItem( smeltitem );
	target.SetTailItem( tailitem );
	target.SetNPCAiType( npcaitype );
	target.SetCallNum( callnum );
	target.SetPlayerCallNum( playercallnum );
	target.SetPageGM( pagegm );
	target.SetRegion( region );
	target.SetSkillDelay( skilldelay );
	target.SetObjectDelay( objectdelay );
	target.SetMaking( making );
	target.SetBlocked( blocked );
	target.SetDir2( dir2 );
	target.SetSpiritSpeakTimer( spiritspeaktimer );
	target.SetSpAttack( spattack );
	target.SetSpDelay( spadelay );
	target.SetTaming( taming );
	target.SetSummonTimer( summontimer );
	target.SetTrackingTimer( trackingtimer );
	target.SetTrackingTarget( trackingtarget );
	for( UI08 counter = 0; counter < MAXTRACKINGTARGETS; counter++ )
	{
		target.SetTrackingTargets( trackingtargets[counter], counter );
	}
	target.SetFishingTimer( fishingtimer );
	target.SetTown( town );
	target.SetTownpriv( townpriv );
	target.SetAdvObj( advobj );
	target.SetPoison( poison );
	target.SetPoisoned( poisoned );
	target.SetPoisonTime( poisontime );
	target.SetPoisonTextTime( poisontxt );
	target.SetPoisonWearOffTime( poisonwearofftime );

	target.SetFleeAt( fleeat );
	target.SetReattackAt( reattackat );
	target.SetDisabled( disabled );
	target.SetEnvokeID( envokeid );
	target.SetEnvokeItem( envokeitem );
	target.SetSplit( split );
	target.SetSplitChance( splitchnc );
	target.SetTrainer( trainer );
	target.SetTrainingPlayerIn( trainingplayerin );
	target.SetCanTrain( CanTrain() );
	target.SetLastOn( laston );
	target.SetGuildTitle( guildtitle );
	target.SetGuildFealty( guildfealty );
	target.SetGuildNumber( guildnumber );
	target.SetFlag( flag );
	target.SetMurderRate( murderrate );
	target.SetCrimFlag( crimflag );
	target.SetCasting( casting );
	target.SetSpellTime( spelltime );
	target.SetSpellCast( spellCast );
	target.SetSpellAction( spellaction );
	target.SetNextAct( nextact );
	target.SetPoisonSerial( poisonserial );
	target.SetSquelched( GetSquelched() );
	target.SetMuteTime( mutetime );
	target.SetMed( med );
	target.SetStealth( stealth );
	target.SetRunning( running );
	target.SetLogout( logout );
	target.SetSwingTarg( swingtarg );
	target.SetHoldG( holdg );
	target.SetRace( GetRace() );
	target.SetRaceGate( raceGate );
	target.SetFlySteps( fly_steps );
	target.SetSmokeTimer( smoketimer );
	target.SetSmokeDisplayTimer( smokedisplaytimer );
	target.SetCarve( carve );
	target.SetCommandLevel( commandLevel );
	target.SetPostType( postType );
	target.SetQuestType( questType );
	target.SetQuestDestRegion( questDestRegion );
	target.SetQuestOrigRegion( questOrigRegion );
	for( UI08 counter2 = 0; counter2 < WEATHNUM; counter2++ )
	{
		target.SetWeathDamage( weathDamage[counter2], counter2 );
	}

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
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SendInSight
//|   Date        -  April 7th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Sends the information about this person to socket S
//|                  IF in range.  Essentially a replacement for impowncreate
//o---------------------------------------------------------------------------o

//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::GetSpeechItem
//|   Date        -  April 8th, 2000
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item related to the speech we're working on
//|                  IE the item for name deed if we're renaming ourselves
//o---------------------------------------------------------------------------o
SI32 CChar::GetSpeechItem( void ) const
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
void CChar::SetSpeechItem( SI32 newValue )
{
	speechItem = newValue;
}
//o---------------------------------------------------------------------------o
//|   Function    -  void CChar::SendInSight
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
	if( itemLayers[Layer].size() == 0 )
		return NULL;
	return (itemLayers[Layer])[0];
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
	if( tLayer >= MAXLAYERS )
		return false;
	if( itemLayers[tLayer].size() > 0 )
	{
		fprintf( errorLog, "Doubled up item (%i) at layer (%i) on char %s (%i)\n", toWear->GetSerial(), toWear->GetLayer(), GetName(), GetSerial() );
		// Console << "Doubled up item (" << toWear->GetSerial() << ") at layer " << toWear->GetLayer() << " on SI08 " << GetName() << "(" << GetSerial() << ")" << endl;
		erroredLayers[tLayer]++;
//		return false;
	}
	itemLayers[tLayer].push_back( toWear );

	IncStrength2( toWear->Strength2() );
	IncDexterity2( toWear->Dexterity2() );
	IncIntelligence2( toWear->Intelligence2() );
	if( toWear->GetPoisoned() )
	{
		SetPoison( GetPoison() - toWear->GetPoisoned() );
		if( GetPoison() < 0 )
			SetPoison( 0 );
	}

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
bool CChar::TakeOffItem( UI08 Layer, CItem *toTakeOff )
{
	if( Layer >= MAXLAYERS || itemLayers[Layer].size() == 0 || toTakeOff == NULL )
		return false;
	bool bFound = false;
	for( int i = 0; i < itemLayers[Layer].size(); i++ )
	{
		if( (itemLayers[Layer])[i] == toTakeOff )
		{
			bFound = true;
			for( int j = i; j < itemLayers[Layer].size() - 1; j++ )
			{
				(itemLayers[Layer])[j] = (itemLayers[Layer])[j+1];
			}
			itemLayers[Layer].resize( itemLayers[Layer].size() - 1 );
		}
	}
	if( !bFound )
		fprintf( errorLog, "ERROR: Unable to find item (%i) on character (%i)", toTakeOff->GetSerial(), GetSerial() );
	IncStrength2( -toTakeOff->Strength2() );
	IncDexterity2( -toTakeOff->Dexterity2() );
	IncIntelligence2( -toTakeOff->Intelligence2() );
	if( toTakeOff->GetPoisoned() )
	{
		SetPoison( GetPoison() - toTakeOff->GetPoisoned() );
		if( GetPoison() < 0 )
			SetPoison( 0 );
	}
	
	return true;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI32 FirstItem( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item reference for the first item on paperdoll
//o---------------------------------------------------------------------------o
SI32 CChar::FirstItem( void ) const
{
	iCounter = 0;
	if( itemLayers[iCounter].size() == 0 )
		return -1;
	return calcItemFromSer( (itemLayers[iCounter])[0]->GetSerial() );
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI32 NextItem( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the item reference for the next item on paperdoll
//o---------------------------------------------------------------------------o
SI32 CChar::NextItem( void ) const
{
	iCounter++;
	if( iCounter >= MAXLAYERS || itemLayers[iCounter].size() == 0 )
		return -1;
	return calcItemFromSer( (itemLayers[iCounter])[0]->GetSerial() );
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
//|   Function    -  SI32 NumItems( void ) const
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the number of items on the paperdoll
//o---------------------------------------------------------------------------o
SI32	CChar::NumItems( void ) const
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
		case 0xC8:	// horse
		case 0xE2:	// horse
		case 0xE4:	// horse
		case 0xCC:	// horse
		case 0xD2:	// Desert Ostard
		case 0xDA:	// Frenzied Ostard
		case 0xDB:	// Forest Ostard
		case 0xDC:	// llama
			return true;
		default:
			return false;
	}		

	return false;
}

bool CChar::DumpHeader( ofstream &outStream, SI32 mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
//		SI32 zFilePosition = outStream.tellp();
//		outStream << "[" << GetSerial() << "]" << GetObjType() << "[" << zFilePosition << "] - CHARACTER Dump" << endl;
		outStream << "[CHARACTER]" << endl;
		break;
	}
	return true;
}
bool CChar::DumpBody( ofstream &outStream, SI32 mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		cBaseObject::DumpBody( outStream, mode );	// Make the default save of BaseObject members now
		outStream << "Account=" << GetAccount() << endl;
		outStream << "NpcAIType=" << GetNPCAiType() << endl;
		outStream << "Making=" << GetMaking() << endl;
		outStream << "Taming=" << GetTaming() << endl;
		outStream << "Weight=" << GetWeight() << endl;
		outStream << "Hunger=" << (short)GetHunger() << endl;
		outStream << "FixedLight=" << (short)GetFixedLight() << endl;
		outStream << "Town=" << (short)GetTown() << endl;
		outStream << "Carve=" << GetCarve() << endl;
		outStream << "HoldG=" << GetHoldG() << endl;
		outStream << "Split=" << (short)GetSplit() << endl;
		outStream << "SplitChance=" << (short)GetSplitChance() << endl;
		outStream << "RobeSerial=" << GetRobe() << endl;
		outStream << "TownVote=" << GetTownVote() << endl;
		outStream << "GuildFealty=" << GetGuildFealty() << endl;  
		outStream << "PoisonSerial=" << GetPoisonSerial() << endl;
		outStream << "OrgName=" << GetOrgName() << endl;
		outStream << "LastOn=" << GetLastOn() << endl;
		outStream << "GuildTitle=" << GetGuildTitle() << endl;  
		outStream << "SummonTimer=" << GetSummonTimer() << endl;
		outStream << "XBodyID=" << GetxID() << endl;
		outStream << "OriginalBodyID=" << GetOrgID() << endl;
		outStream << "HairStyle=" << GetHairStyle() << endl;
		outStream << "BeardStyle=" << GetBeardStyle() << endl;
		outStream << "XSkinID=" << GetxSkin() << endl;
		outStream << "OriginalSkinID=" << GetOrgSkin() << endl;
		outStream << "HairColour=" << GetHairColour() << endl;
		outStream << "BeardColour=" << GetBeardColour() << endl;
		outStream << "Say=" << GetSayColour() << endl;
		outStream << "Emotion=" << GetEmoteColour() << endl;
		outStream << "MayLevitate=" << (MayLevitate()?"1":"0") << endl;
		outStream << "FX1=" << GetFx( 1 ) << endl;
		outStream << "FX2=" << GetFx( 2 ) << endl;
		outStream << "FY1=" << GetFy( 1 ) << endl;
		outStream << "FY2=" << GetFy( 2 ) << endl;
		outStream << "FZ1=" << (short)GetFz() << endl;
		outStream << "DisplayZ=" << (short)GetDispZ() << endl;
		outStream << "Stealth=" << (short)GetStealth() << endl;
		outStream << "Dir2=" << (short)GetDir2() << endl;
		outStream << "Reserved=" << (short)GetCell() << endl;
		outStream << "NpcWander=" << (short)GetNpcWander() << endl;
		outStream << "XNpcWander=" << (short)GetOldNpcWander() << endl;
		outStream << "FlySteps=" << (short)GetFlySteps() << endl;
		outStream << "Running=" << (short)GetRunning() << endl;
		outStream << "Step=" << (short)GetStep() << endl;
		outStream << "Region=" << (short)GetRegion() << endl;
		if( GetPackItem() != -1 )
			outStream << "PackItem=" << items[GetPackItem()].GetSerial() << endl;	// store something meaningful
		outStream << "AdvanceObject=" << GetAdvObj() << endl;
		outStream << "AdvRaceObject=" << GetRaceGate() << endl;
		outStream << "SPAttack=" << GetSpAttack() << endl;
		outStream << "SpecialAttackDelay=" << (short)GetSpDelay() << endl;
		outStream << "QuestType=" << (short)GetQuestType() << endl;	
		outStream << "QuestDestinationRegion=" << (short)GetQuestDestRegion() << endl;	
		outStream << "QuestOriginalRegion=" << (short)GetQuestOrigRegion() << endl;	
		outStream << "FleeAt=" << GetFleeAt() << endl;
		outStream << "ReAttackAt=" << GetReattackAt() << endl;
		outStream << "Privileges=" << (short)GetPriv() << endl;
		outStream << "CommandLevel=" << (short)GetCommandLevel() << endl;	// command level
		outStream << "PostType=" << (short)GetPostType() << endl;
		outStream << "TownPrivileges=" << (short)GetTownPriv() << endl;
		outStream << "XMana=" << GetMana2() << endl;
		outStream << "XStamina=" << GetStamina2() << endl;
		// Write out the BaseSkills and the SkillLocks here
		// Format: BaseSkills=[0,34]-[1,255]-[END]
		outStream << "BaseSkills=";
		for( UI08 bsc = 0; bsc < TRUESKILLS; bsc++ )
			outStream << "[" << (SI32)bsc << "," << GetBaseSkill( bsc ) << "]-";
		outStream << "[END]" << endl;
		outStream << "Atrophy=";
		if( GetAtrophy( 0 ) >= 10 )
			outStream << GetAtrophy( 0 );
		else
			outStream << "0" << GetAtrophy( 0 );
		for( UI08 atc = 1; atc < ALLSKILLS; atc++ )
		{
			if( GetAtrophy( atc ) >= 10 )
				outStream << "," << GetAtrophy( atc );
			else 
				outStream << ",0" << GetAtrophy( atc );
		}
		outStream << "[END]" << endl;
		// Format: SkillLocks=[0,34]-[1,255]-[END]
		outStream << "SkillLocks=";
		for( UI08 slc = 0; slc < TRUESKILLS; slc++ )
		{
			if( GetSkillLock( slc ) >= 0 && GetSkillLock( slc ) <= 2 )
				outStream << "[" << (SI16)slc << "," << (short)GetSkillLock( slc ) << "]-";
			else
				outStream << "[" << (SI16)slc << ",0]-";
		}
		outStream << "[END]" << endl;

		outStream << "XKarma=" << GetKarma2() << endl;
		outStream << "XFame=" << GetFame2() << endl;
		outStream << "GuildNumber=" << GetGuildNumber() << endl;  
		outStream << "Deaths=" << GetDeaths() << endl;
		outStream << "FontType=" << (short)GetFontType() << endl;
		outStream << "Squelched=" << (short)GetSquelched() << endl;
		outStream << "Poison=" << GetPoison() << endl;
		outStream << "Poisoned=" << GetPoisoned() << endl;
		outStream << "TownTitle=" << (GetTownTitle()?1:0) << endl;
		//-------------------------------------------------------------------------------------------
		bool crCheck = ( CanRun() && IsNpc() );
		outStream << "CanRun=" << (SI32)(crCheck?1:0) << endl;
		outStream << "AllMove=" << (short)GetPriv2() << endl;
		outStream << "IsNpc=" << (SI32)(IsNpc()?1:0) << endl;
		outStream << "IsShop=" << (SI32)(IsShop()?1:0) << endl;
		outStream << "Dead=" << (SI32)(IsDead()?1:0) << endl;

		outStream << "CanTrain=" << (SI32)(CanTrain()?1:0) << endl;
		outStream << "IsWarring=" << (SI32)(IsAtWar()?1:0) << endl;
		outStream << "GuildToggle=" << (SI32)(GetGuildToggle()?1:0) << endl;  

		DumpFooter( outStream, mode );
		for( SI32 tempCounter = 0; tempCounter < MAXLAYERS; tempCounter++ )
		{
			if( itemLayers[tempCounter].size() > 0 )
			{
				for( int iLCnt = 0; iLCnt < itemLayers[tempCounter].size(); iLCnt++ )
					(itemLayers[tempCounter])[iLCnt]->Save( outStream, mode );
			}
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
bool CChar::Save( ofstream &outStream, SI32 mode ) const
{
	charsWritten++;
	DumpHeader( outStream, mode );
	DumpBody( outStream, mode );
	return true;
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
		bools &= 0xEFFFF;
}

//o---------------------------------------------------------------------------o
//|   Function    -  SI08LIST *GetPetList( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the list of pets the SI08 owns
//o---------------------------------------------------------------------------o
CHARLIST *CChar::GetPetList( void )
{
	return &petsControlled;
}

//o---------------------------------------------------------------------------o
//|   Function    -  ITEMLIST *GetOwnedItems( void )
//|   Date        -  13 March 2001
//|   Programmer  -  Abaddon
//o---------------------------------------------------------------------------o
//|   Purpose     -  Returns the list of items the SI08 owns
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

bool CChar::DumpFooter( ofstream &outStream, SI32 mode ) const
{
	switch( mode )
	{
	case 1:	break;
	case 0:
	default:
		outStream << endl << endl << "o---o" << endl;
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
			SetAccount( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Atrophy" ) )
		{
			UI16 atrophyNum = 0;
			char *nextComma;
			nextComma = strtok( data, "," );
			atrophyNum = (UI16)(atoi( nextComma ));
			SetAtrophy( atrophyNum, 0 );
			for( UI08 aCtr = 1; aCtr < TRUESKILLS; aCtr++ )
			{
				nextComma = strtok( NULL, "," );
				atrophyNum = (UI16)(atoi( nextComma ));
				SetAtrophy( atrophyNum, aCtr );
			}
			return true;
		}
		else if( !strcmp( tag, "AdvanceObject" ) )
		{
			SetAdvObj( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "AdvRaceObject" ) )
		{
			SetRaceGate( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "AllMove" ) )
		{
			SetPriv2( (UI08)atoi( data ) );
			return true;
		}
		break;
	case 'B':
		if( !strcmp( tag, "BeardStyle" ) )
		{
			SetBeardStyle( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "BeardColour" ) )
		{
			SetBeardColour( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "BaseSkills" ) )
		{
			UI08 skillNum = 0;
			UI16 skillVal = 0;
			UI08 skillCtr = 0;
			// Format: BaseSkills=[0,34]-[1,255]-[END]

			string s( data );
			istringstream ss( s );
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
			SetCarve( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "CommandLevel" ) )
		{
			SetCommandLevel( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "CanRun" ) )
		{
			SetRun( atoi( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "CanTrain" ) )
		{
			SetCanTrain( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'D':
		if( !strcmp( tag, "DisplayZ" ) )
		{
			SetDispZ( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Dir2" ) )
		{
			SetDir2( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Deaths" ) )
		{
			SetDeaths( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Dead" ) )
		{
			SetDead( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'E':
		if( !strcmp( tag, "Emotion" ) )
		{
			SetEmoteColour( (UI16)atoi( data ) );
			return true;
		}
		break;
	case 'F':
		if( !strcmp( tag, "FixedLight" ) )
		{
			SetFixedLight( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "FX1" ) )
		{
			SetFx( (SI16)atoi( data ), 1 );
			return true;
		}
		else if( !strcmp( tag, "FX2" ) )
		{
			SetFx( (SI16)atoi( data ), 2 );
			return true;
		}
		else if( !strcmp( tag, "FY1" ) )
		{
			SetFy( (SI16)atoi( data ), 1 );
			return true;
		}
		else if( !strcmp( tag, "FY2" ) )
		{
			SetFy( (SI16)atoi( data ), 2 );
			return true;
		}
		else if( !strcmp( tag, "FZ1" ) )
		{
			SetFz( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "FlySteps" ) )
		{
			SetFlySteps( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "FleeAt" ) )
		{
			SetFleeAt( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "FontType" ) )
		{
			SetFontType( (SI08)atoi( data ) );
			return true;
		}
		break;
	case 'G':
		if( !strcmp( tag, "GuildFealty" ) )
		{
			SetGuildFealty( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "GuildTitle" ) )
		{
			SetGuildTitle( data );
			return true;
		}
		else if( !strcmp( tag, "GuildNumber" ) )
		{
			SetGuildNumber( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "GuildToggle" ) )
		{
			SetGuildToggle( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'H':
		if( !strcmp( tag, "Hunger" ) )
		{
			SetHunger( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "HoldG" ) )
		{
			SetHoldG( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "HairStyle" ) )
		{
			SetHairStyle( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "HairColour" ) )
		{
			SetHairColour( (UI16)atoi( data ) );
			return true;
		}
		break;
	case 'I':
		if( !strcmp( tag, "IsNpc" ) )
		{
			SetNpc( atoi( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "IsShop" ) )
		{
			SetShop( atoi( data ) == 1 );
			return true;
		}
		else if( !strcmp( tag, "IsWarring" ) )
		{
			SetWar( atoi( data ) == 1 );
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
			SetMaking( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "MayLevitate" ) )
		{
			MayLevitate( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'N':
		if( !strcmp( tag, "NpcAIType" ) )
		{
			SetNPCAiType( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "NpcWander" ) )
		{
			SetNpcWander( (SI08)atoi( data ) );
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
			SetOrgID( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "OriginalSkinID" ) )
		{
			SetOrgSkin( (UI16)atoi( data ) );
			return true;
		}
		break;
	case 'P':
		if( !strcmp( tag, "PoisonSerial" ) )
		{
			SetPoisonSerial( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Privileges" ) )
		{
			SetPriv( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "PostType" ) )
		{
			SetPostType( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Poison" ) )
		{
			SetPoison( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Poisoned" ) )
		{
			SetPoisoned( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "PackItem" ) )
		{
			packitem = atoi( data );
			return true;
		}
		break;
	case 'Q':
		if( !strcmp( tag, "QuestType" ) )
		{
			SetQuestType( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "QuestDestinationRegion" ) )
		{
			SetQuestDestRegion( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "QuestOriginalRegion" ) )
		{
			SetQuestOrigRegion( (UI08)atoi( data ) );
			return true;
		}
		break;
	case 'R':
		if( !strcmp( tag, "RobeSerial" ) )
		{
			SetRobe( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Reserved" ) )
		{
			SetCell( (SI08)atoi( data ) );
			if( GetCell() == -1 )	// convert from new -> old
				SetCell( 0 );
			return true;
		}
		else if( !strcmp( tag, "Running" ) )
		{
			SetRunning( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Region" ) )
		{
			SetRegion( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "ReAttackAt" ) )
		{
			SetReattackAt( (SI16)atoi( data ) );
			return true;
		}
		break;
	case 'S':
		if( !strcmp( tag, "Split" ) )
		{
			SetSplit( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "SplitChance" ) )
		{
			SetSplitChance( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "SummonTimer" ) )
		{
			SetSummonTimer( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Say" ) )
		{
			SetSayColour( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Stealth" ) )
		{
			SetStealth( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Step" ) )
		{
			SetStep( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "SPAttack" ) )
		{
			SetSpAttack( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "SpecialAttackDelay" ) )
		{
			SetSpDelay( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Squelched" ) )
		{
			SetSquelched( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "SkillLocks" ) )
		{
			UI08 lockNum = 0;
			UI08 lockVal = 0;
			UI08 lockCtr = 0;
			// Format: Baselocks=[0,34]-[1,255]-[END]
			string s( data );
			istringstream ss( s );
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
			SetTaming( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "Town" ) )
		{
			SetTown( (UI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "TownVote" ) )
		{
			SetTownVote( atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "TownPrivileges" ) )
		{
			SetTownpriv( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "TownTitle" ) )
		{
			SetTownTitle( atoi( data ) == 1 );
			return true;
		}
		break;
	case 'W':
		if( !strcmp( tag, "Weight" ) )
		{
			SetWeight( (SI16)atoi( data ) );
			return true;
		}
		break;
	case 'X':
		if( !strcmp( tag, "XBodyID" ) )
		{
			SetxID( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "XSkinID" ) )
		{
			SetxSkin( (UI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "XNpcWander" ) )
		{
			SetOldNpcWander( (SI08)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "XMana" ) )
		{
			SetMana2( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "XStamina" ) )
		{
			SetStamina2( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "XKarma" ) )
		{
			SetKarma2( (SI16)atoi( data ) );
			return true;
		}
		else if( !strcmp( tag, "XFame" ) )
		{
			SetFame2( (SI16)atoi( data ) );
			return true;
		}
		break;
	}
	return false;
}
bool CChar::Load( ifstream &inStream, int mode, int arrayOffset )
{
	switch( mode )
	{
	case 1:	break;	// binary
	case 0:
	default:
		char tag[128], data[512];
		bool bFinished;
		bFinished = false;
		while( !bFinished )
		{
			ReadWorldTagData( inStream, tag, data );
			bFinished = ( strcmp( tag, "o---o" ) == 0 );
			if( !bFinished )
			{
				if( !HandleLine( tag, data ) )
					printf( "ERROR: Unknown world file tag %s with contents of %s\n", tag, data );
			}
		}
		break;
	}

	if( charcount2 <= serial ) 
		charcount2 = serial + 1;
	SetSerial( serial, arrayOffset );

	if( IsNpc() && IsAtWar() ) 
		SetWar( false );

	if( IsCounselor() && GetCommandLevel() < 1 && !IsGM() ) // SI32erim line to retain compatibility, MUST BE TAKEN out in the SI32 term!
		SetCommandLevel( 1 );
	if( IsGM() && GetCommandLevel() < 2 )	// SI32erim line to retain compatibility, MUST BE TAKEN out in the SI32 term!
		SetCommandLevel( 2 );

	////////////////////////////////////////////////////////////////////

	UI16 k = GetID();
	if( k > 0x3E1 )
	{ 
		if( GetAccount() == -1 ) 
		{ 
			return false;
		} 
		else 
			SetID( 0x0190 );
	}

	MapRegion->AddChar( this );

	SI16	mx = GetX();
	SI16	my = GetY();
	SI32	acct = GetAccount();

	bool	overRight = ( mx > MapWidth );
	bool	overBottom = ( my > MapHeight );

	if( acct == -1 && ( ( overRight && mx < 7000 ) || ( overBottom && my < 7000 ) || mx < 0 || my < 0 ) )
	{
		if( IsNpc() )
			return false;
		else
			SetLocation( 1000, 1000, 0 );
	}

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

void CChar::PostLoadProcessing( SI32 index )
{
	cBaseObject::PostLoadProcessing( index );
	if( packitem != -1 )
		SetPackItem( calcItemFromSer( packitem ) );		// we stored the serial in packitem
}

bool CChar::IsPolymorphed( void ) const
{
	return ( (bools&0x10000) == 0x10000 );
}
bool CChar::IsIncognito( void ) const
{
	return ( (bools&0x20000) == 0x20000 );
}
void CChar::IsPolymorphed( bool newValue )
{
	if( newValue )
		bools |= 0x10000;
	else
		bools &= 0xEFFFF;
}

void CChar::IsIncognito( bool newValue )
{
	if( newValue )
		bools |= 0x20000;
	else
		bools &= 0xDFFFF;
}