#ifndef __FUNCDECLS_H__
#define __FUNCDECLS_H__

// Pack functions
CItem *	getRootPack( CItem *p );
CChar *	getPackOwner( CItem *p );
CItem *	getPack( CChar *p );
void	openPack( cSocket  *s, CItem *i );

// Distance functions
UI16	getDist( cBaseObject *a, cBaseObject *b );

// Range check functions
bool	checkItemRange( CChar *mChar, CItem *i, UI16 distance );
bool	objInRange( cSocket *mSock, cBaseObject *obj, UI16 distance );
bool	objInRange( cBaseObject *a, cBaseObject *b, UI16 distance );
bool	itemInRange( CChar *mChar, CItem *i );
bool	charInRange( CChar *a, CChar *b );
bool	inBankRange( CChar *i );

// Multi functions
CMultiObj *	findMulti( SI16 x, SI16 y, SI08 z, UI08 worldNumber );
CMultiObj *	findMulti( cBaseObject *i );
bool		inMulti( SI16 x, SI16 y, SI08 z, CItem *m, UI08 worldNumber );

// Speech functions
bool	response( cSocket *mSock );
void	responsevendor( cSocket *mSock );
void	sysmessage( cSocket *s, const char *txt, ... );
void	sysmessage( cSocket *s, SI32 dictEntry, ... );
void	objMessage( cSocket *s, const char *txt, cBaseObject *getObj, R32 secsFromNow = 0.0f, UI16 Color = 0x03B2 );
void	objMessage( cSocket *s, SI32 dictEntry, cBaseObject *getObj, R32 secsFromNow = 0.0f, UI16 Color = 0x03B2, ... );
void	itemTalk( cSocket *s, CItem *item, SI32 dictEntry, R32 secsFromNow = 0.0f, UI16 Colour = 0 );
void	npcTalkAll( CChar *npc, const char *txt, bool antispam );
void	npcTalkAll( CChar *npc, SI32 dictEntry, bool antispam, ... );
void	npcTalk( cSocket *s, CChar *npc, const char *txt, bool antispam ); // NPC speech
void	npcTalk( cSocket *s, CChar *npc, SI32 dictEntry, bool antispam, ... ); // NPC speech
void	npcEmote( cSocket *s, CChar *npc, const char *txt, bool antispam );
void	npcEmote( cSocket *s, CChar *npc, SI32 dictEntry, bool antispam, ... );
void	npcEmoteAll( CChar *npc, const char *txt, bool antispam );
void	npcEmoteAll( CChar *npc, SI32 dictEntry, bool antispam, ... );
void	unicodetalking( cSocket *mSock ); // PC speech
void	talking( cSocket *mSock ); // PC speech
void	textflags( cSocket *s, CChar *i, const char *name );
void	sysbroadcast( const char *txt );
void	house_speech( cSocket *mSock, const char *talk );
void	showcname( cSocket *s, CChar *i, char b );
void	tellmessage( cSocket *i, cSocket *s, const char *txt );
UI16	GetFlagColour( CChar *src, CChar *trg );

// Click functions
void	doubleClick( cSocket *mSock );
void	singleClick( cSocket *mSock );

// Calculation functions (socket, char, item and so forth)
UOXSOCKET	calcSocketFromChar( CHARACTER i );
UOXSOCKET	calcSocketFromChar( CChar *i );
UOXSOCKET	calcSocketFromSockObj( cSocket *s );
cSocket		*calcSocketObjFromChar( CHARACTER i );
cSocket		*calcSocketObjFromChar( CChar *i );
cSocket		*calcSocketObjFromSock( UOXSOCKET s );
ITEM		calcItemFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 );
ITEM		calcItemFromSer( SERIAL ser );
CItem *		calcItemObjFromSer( SERIAL targSerial );
CHARACTER	calcCharFromSer( UI08 ser1, UI08 ser2, UI08 ser3, UI08 ser4 );
CHARACTER	calcCharFromSer( SERIAL ser );
CChar *		calcCharObjFromSer( SERIAL targSerial );
UI32		calcGold( CChar *p );
inline UI32 calcserial( UI08 a1, UI08 a2, UI08 a3, UI08 a4 ) { return ( (a1<<24) + (a2<<16) + (a3<<8) + a4 ); }	

// Doors
void	useDoor( cSocket *s, CItem *item );
bool	isDoorBlocked( CItem *door );
void	DoorMacro( cSocket *s );
bool	keyInPack( cSocket *mSock, CChar *mChar, CItem *pack, CItem *x );

// Socket stuff
void	SendVecsAsGump( cSocket *sock, STRINGLIST& one, STRINGLIST& two, long type, SERIAL serial );
void	SendWorldChange( WorldType season, cSocket *sock );
void	SendMapChange( UI08 worldNumber, cSocket *sock, bool initialLogin = false );
void	SocketMapChange( cSocket *sock, CChar *charMoving, CItem *gate );
void	updates( cSocket *s );
void	startChar( cSocket *mSock, bool onCreate = false );
void	sendItem( cSocket *s, CItem *i );
void	sendPackItem( cSocket *s, CItem *i );
void	wornItems( cSocket *s, CChar *j );
void	RefreshItem( CItem *i );
void	updateStats( CChar *c, char x );
bool	isOnline( CChar *c );
void	updateskill( cSocket *mSock, UI08 skillnum );
void	playChar( cSocket *mSock ); // After hitting "Play Character" button //Instalog

// Target functions
void	vialtarget( cSocket *nSocket );
void	target( cSocket *s, UI08 targType, UI08 targID, const char *txt );
void	target( cSocket *s, UI08 targType, UI08 targID,  SI32 dictEntry );
void	target( cSocket *s, SERIAL ser, const char *txt );

// String related functions
int		makenumber( int countx );
void	safeCopy( char *dest, const char *src, UI32 maxLen );
void	scriptcommand( cSocket *s, const char *cmd, const char *data );
void	endmessage( int x );
void	consolebroadcast( const char *txt );
void	gettokennum( const char * s, int num, char *gettokenstr );
void	numtostr( int i, char *string );
SI32	makeNum( const char *s );
SI32	makeNum( const std::string *s );
char *	RealTime( char *time_str );
int		getStringValue( const char *string );

// Light related functions
void	doLight( cSocket *s, char level );

// Amount related
UI32	GetAmount( CChar *s, UI16 realID );
UI32	GetSubAmount( CItem *p, UI16 realID );
UI32	DeleteQuantity( CChar *s, UI16 realID, UI32 amount );
UI32	DeleteSubQuantity( CItem *p, UI16 realID, UI32 amount );
UI32	GetItemAmount( CChar *s, UI16 realID, UI16 realColour );
UI32	GetSubItemAmount( CItem *p, UI16 realID, UI16 realColour );
UI32	DeleteItemAmount( CChar *s, UI16 realID, UI16 realColour, UI32 amount );
UI32	DeleteSubItemAmount( CItem *p, UI16 realID, UI16 realColour, UI32 amount );
SI16	DeleteItemsFromChar( CChar *toFind, UI16 itemID );
SI16	DeleteItemsFromPack( CItem *item, UI16 itemID );
UI32	GetBankCount( CChar *p, UI16 itemID, UI16 colour = 0x0000 );
UI32	DeleteBankItem( CChar *p, UI16 itemID, UI16 colour, UI32 amt );
CItem *	DecreaseItemAmount( CItem *toDelete, UI16 amt = 1 );

// Trade related
CItem *	startTrade( cSocket *mSock, CChar *i );
void	clearTrades( void );
void	doTradeMsg( cSocket *s );
void	endTrade( SERIAL targSer );
void	sendTradeStatus( CItem *cont1, CItem *cont2 );
void	doTrade( CItem *cont1, CItem *cont2 );
void	killTrades( CChar *i );

// Vendor Stuff
void	restock( bool stockAll );
void	restockNPC( CChar *i, bool stockAll );
bool	sendSellStuff( cSocket *s, CChar *i );
void	sendSellSubItem( CChar *npc, CItem *p, CItem *q, UI08 *m1, int &m1t);
void	buyItem( cSocket *mSock );
void	sellItem( cSocket *mSock );
UI32	calcValue( CItem *i, UI32 value );
UI32	calcGoodValue( CChar *npcnum2, CItem *i, UI32 value, bool isSelling );
void	PlVGetgold( cSocket *mSock, CChar *v );

// Region related
void	checkRegion( CChar *i );
UI08	calcRegionFromXY( SI16 x, SI16 y, UI08 worldNumber );

// House related
void	buildHouse( cSocket *s, UI32 i );
void	deedHouse( cSocket *s, CItem *i);
void	killKeys( SERIAL targSerial );
UI08	AddToHouse( CMultiObj *house, CChar *toAdd, UI08 mode = 0 );
bool	OnHouseList( CMultiObj *house, CChar *toCheck );
bool	DeleteFromHouseList( CMultiObj *house, CChar *toDelete, UI08 mode = 0 );

// Find functions
CItem *	FindItemOfType( CChar *toFind, UI08 type );
CItem *	SearchSubPackForItemOfType( CItem *toSearch, UI08 type );
CItem *	FindItem( CChar *toFind, UI16 itemID );
CItem *	SearchSubPackForItem( CItem *toSearch, UI16 itemID );

void	wearItem( cSocket *s ); // Item is dropped on paperdoll
void	grabItem( cSocket *s ); // Client grabs an item
void	packItem( cSocket *s ); // Item is put into container
void	dropItem( cSocket *s ); // Item is dropped on ground

// Character management
void	deleteChar( cSocket *s ); // Deletion of character
void	createChar( cSocket *mSock ); // All the character creation stuff
UI16	addRandomColor( const char *colorlist );
void	setRandomName( CChar *s, const char *namelist );

// Reputation Stuff
void	Karma( CChar *nCharID, CChar *nKilledID, SI16 nKarma );
void	Fame( CChar *nCharID, SI16 nFame );

// Combat Stuff
void	PlayerAttack( cSocket *s );
void	npcAttackTarget( CChar *target2, CChar *target );
void	petGuardAttack( CChar *mChar, CChar *owner, SERIAL guarded );
void	criminal( CChar *c );
void	callGuards( CChar *mChar );
void	callGuards( CChar *mChar, CChar *targChar );

// Death Handling
void	doDeathStuff( CChar *i );
CItem *	GenerateCorpse( CChar *i, UI08 nType, CChar *murderer );

// NPC Actions
void	npcAction( CChar *npc, SI16 x );
void	npcAct( cSocket *s );
void	npcToggleCombat( CChar *s );

// Gump Stuff
void	entrygump( cSocket *s, SERIAL ser, char type, char index, SI16 maxlength, const char *text1 );
void	entrygump( cSocket *s, SERIAL ser, char type, char index, SI16 maxlength, SI32 dictEntry );
void	NewAddMenu( cSocket *s, int m ); // Menus for item creation
void	itemmenu( cSocket *s, int m );
void	gmmenu( cSocket *s, int m );
void	tweakItemMenu( cSocket *s, CItem *j );
void	tweakCharMenu( cSocket *s, CChar *c );
void	choice( cSocket *s );

void	ReadWorldTagData( std::ifstream &inStream, char *tag, char *data );

// Weather Stuff
bool	doHeatEffect( CChar *i );
bool	doLightEffect( CChar *i );
bool	doColdEffect( CChar *i );
bool	doRainEffect( CChar *i );
bool	doSnowEffect( CChar *i );

// Misc Functions
void	explodeItem( cSocket *mSock, CItem *nItem );
void	MonsterGate( CChar *s, SI32 x );
void	MakeNecroReg( cSocket *nSocket, CItem *nItem, UI16 itemID );
void	objTeleporters( CChar *s );
bool	checkBoundingBox( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, SI16 fx2, SI16 fy2, UI08 worldNumber );
bool	checkBoundingCircle( SI16 xPos, SI16 yPos, SI16 fx1, SI16 fy1, SI08 fz1, int radius, UI08 worldNumber );
void	MountCreature( CChar *s, CChar *x );
void	DismountCreature( CChar *s );
void	telltime( cSocket *s );
UI08	getCharDir( CChar *a, SI16 x, SI16 y );
UI08	getFieldDir( CChar *s, SI16 x, SI16 y );
SI08	validTelePos( cSocket *s );
void	doGCollect( void );
void	checkkey( void );
void	openBank( cSocket *s, CChar *i );
void	openSpecialBank( cSocket *s, CChar *i );
int		getTileName( CItem *i, char* itemname);
void	usePotion( CChar *p, CItem *i );
void	advanceObj( CChar *s, UI16 x, bool multiUse );
void	statwindow( cSocket *s, CChar *i );
void	fileArchive( void );
void	enlist( cSocket *mSock, SI32 listnum ); // For enlisting in army
bool	LineOfSight( cSocket *s, CChar *mChar, SI16 x2, SI16 y2, SI08 z2, int checkfor );
void	Shutdown( SI32 retCode );
void	batchcheck( cSocket *s );
void	weblaunch( cSocket *s, const char *txt );
void	setcharflag( CChar *c );
void	UseHairDye( cSocket *s, UI16 colour, CItem *x );	// s for socket, colour to dye, x is the item
void	SpawnGate( cSocket *sock, CChar *caster, SI16 srcX, SI16 srcY, SI08 srcZ, SI16 trgX, SI16 trgY, SI08 trgZ );
bool	FileExists( const char *filepath );

inline int RandomNum( int nLowNum, int nHighNum )
{
	if( nHighNum - nLowNum + 1 )
		return ((rand() % ( nHighNum - nLowNum + 1 )) + nLowNum );
	else
		return nLowNum;
}

#if defined(__unix__)
void	*CheckConsoleKeyThread( void *params );
#else
void	CheckConsoleKeyThread( void *params );
#endif

#endif

