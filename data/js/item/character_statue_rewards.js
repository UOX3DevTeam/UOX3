/// <reference path="../../../definitions.d.ts" />
// @ts-check

// Character statue maker for veteran rewards.
// Uses existing vet reward tooltip script 5303 on maker items from DFN.
var statueBaseNpcSection = "vet_character_statue_base";
var statuePlinthSection = "vet_character_statue_plinth";
var statuePlacementRange = 10;
var statueRequireHousePlacement = true;

var statueTypeNames = [ "Marble", "Jade", "Bronze" ];
var statueMaterialNames = [ "Antique", "Dark", "Medium", "Light" ];
var statuePoseNames = [ "Ready", "Casting", "Salute", "All Praise Me", "Fighting", "Hands On Hips" ];
var statuePoseActions = [ 4, 16, 33, 17, 31, 6 ];
var statuePoseFrames = [ 0, 2, 1, 4, 5, 1 ];
var statueMakerSections = [ "vet_marble_statue_maker", "vet_jade_statue_maker", "vet_bronze_statue_maker" ];

/** @type { ( pUser: Character, iUsed: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
	{
		return false;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	if( iUsed.GetTag( "statuePlinth" ))
	{
		ShowCharacterStatueGump( pUser, iUsed );
		return false;
	}

	var packOwner = GetPackOwner( iUsed, 0 );
	if( !ValidateObject( packOwner ) || packOwner.serial != pUser.serial )
	{
		socket.SysMessage( "That must be in your backpack to use it." );
		return false;
	}

	var statueType = GetStatueTypeFromMaker( iUsed );
	if( statueType < 0 )
	{
		socket.SysMessage( "This statue maker is not configured correctly." );
		return false;
	}

	pUser.SetTempTag( "CharacterStatueMakerSerial", iUsed.serial );
	socket.CustomTarget( 0, "Select a place where you would like to put your statue." );
	return false;
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, target )
{
	if( socket == null )
	{
		return;
	}

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
	{
		return;
	}

	var makerSerial = parseInt( pUser.GetTempTag( "CharacterStatueMakerSerial" ), 10 );
	pUser.SetTempTag( "CharacterStatueMakerSerial", null );

	if( isNaN( makerSerial ) || makerSerial <= 0 )
	{
		socket.SysMessage( "Unable to find the statue maker." );
		return;
	}

	var makerItem = CalcItemFromSer( makerSerial );
	if( !ValidateObject( makerItem ))
	{
		socket.SysMessage( "Unable to find the statue maker." );
		return;
	}

	var packOwner = GetPackOwner( makerItem, 0 );
	if( !ValidateObject( packOwner ) || packOwner.serial != pUser.serial )
	{
		socket.SysMessage( "That must be in your backpack to use it." );
		return;
	}

	var statueType = GetStatueTypeFromMaker( makerItem );
	if( statueType < 0 )
	{
		socket.SysMessage( "This statue maker is not configured correctly." );
		return;
	}

	var targetX = socket.GetWord( 11 );
	var targetY = socket.GetWord( 13 );
	var targetZ = socket.GetSByte( 16 );

	if( ValidateObject( target ) && target.isItem )
	{
		targetX = target.x;
		targetY = target.y;
		targetZ = target.z + GetTileHeight( target.id );
	}

	if( targetX <= 0 || targetY <= 0 )
	{
		socket.SysMessage( "That is not a valid location." );
		return;
	}

	if( DistanceBetween( pUser.x, pUser.y, targetX, targetY ) > statuePlacementRange )
	{
		socket.SysMessage( "That location is too far away." );
		return;
	}

	if( statueRequireHousePlacement )
	{
		var targetMulti = FindMulti( targetX, targetY, targetZ, pUser.worldnumber, pUser.instanceID );
		if( !ValidateObject( targetMulti ))
		{
			socket.SysMessage( "Statues can only be placed in houses." );
			return;
		}

		if( !targetMulti.IsOwner( pUser ) && !targetMulti.IsOnOwnerList( pUser ) && !targetMulti.IsOnFriendList( pUser ))
		{
			socket.SysMessage( "You must own or be friended to the house to place this statue." );
			return;
		}
	}

	CreateCharacterStatue( pUser, makerItem, statueType, targetX, targetY, targetZ );
}

/** @type { ( pUser: Character, makerItem: Item, statueType: number, targetX: number, targetY: number, targetZ: number ) => boolean } */
function CreateCharacterStatue( pUser, makerItem, statueType, targetX, targetY, targetZ )
{
	var socket = pUser.socket;
	if( socket == null )
	{
		return false;
	}

	var statueHue = GetCharacterStatueHue( statueType, 0 );
	var statue = SpawnNPC( statueBaseNpcSection, targetX, targetY, targetZ + 5, pUser.worldnumber, pUser.instanceID, false );
	if( !ValidateObject( statue ))
	{
		socket.SysMessage( "Unable to create the character statue." );
		return false;
	}

	statue.name = pUser.name;
	statue.id = pUser.id;
	statue.orgID = pUser.orgID;
	statue.color = statueHue;
	statue.dir = pUser.dir;
	statue.frozen = true;
	statue.invulnerable = true;
	statue.SetTag( "characterStatue", 1 );
	statue.SetTag( "statueType", statueType );
	statue.SetTag( "statueMaterial", 0 );
	statue.SetTag( "statuePose", 0 );
	statue.SetTag( "statueOwnerSerial", pUser.serial );
	statue.SetTag( "sculptedBy", pUser.name );
	statue.SetTag( "sculptedOn", GetCurrentClock() );

	CopyCharacterVisualsToStatue( pUser, statue, statueHue );

	var plinth = CreateDFNItem( socket, pUser, statuePlinthSection, 1, "ITEM", false );
	if( !ValidateObject( plinth ))
	{
		statue.Delete();
		socket.SysMessage( "Unable to create the statue plinth." );
		return false;
	}

	plinth.SetLocation( targetX, targetY, targetZ, pUser.worldnumber, pUser.instanceID );
	plinth.color = statueHue;
	plinth.SetTag( "statuePlinth", 1 );
	plinth.SetTag( "statueSerial", statue.serial );
	plinth.SetTag( "statueType", statueType );
	plinth.SetTag( "statueMaterial", 0 );
	plinth.SetTag( "statuePose", 0 );
	plinth.SetTag( "statueOwnerSerial", pUser.serial );
	plinth.SetTag( "makerSection", statueMakerSections[statueType] );

	statue.SetTag( "plinthSerial", plinth.serial );

	makerItem.Delete();
	statue.DoAction( statuePoseActions[0] );
	ShowCharacterStatueGump( pUser, plinth );
	socket.SysMessage( "You create a character statue." );
	return true;
}

/** @type { ( pUser: Character, statue: Character, statueHue: number ) => void } */
function CopyCharacterVisualsToStatue( pUser, statue, statueHue )
{
	var currentItem;
	for( currentItem = pUser.FirstItem(); !pUser.FinishedItems(); currentItem = pUser.NextItem() )
	{
		if( !ValidateObject( currentItem ))
		{
			continue;
		}

		if( ShouldSkipStatueLayer( currentItem.layer ))
		{
			continue;
		}

		CopyVisualItemToStatue( pUser, statue, currentItem, statueHue );
	}
}

/** @type { ( layer: number ) => boolean } */
function ShouldSkipStatueLayer( layer )
{
	return ( layer == 0x15 || layer == 0x19 || layer == 0x1D || layer == 0x1A || layer == 0x1B || layer == 0x1C );
}

/** @type { ( pUser: Character, statue: Character, sourceItem: Item, statueHue: number ) => void } */
function CopyVisualItemToStatue( pUser, statue, sourceItem, statueHue )
{
	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	var itemIdText = "0x" + sourceItem.id.toString( 16 );
	var copiedItem = CreateDFNItem( socket, pUser, itemIdText, 1, "ITEM", false );
	if( !ValidateObject( copiedItem ))
	{
		return;
	}

	copiedItem.name = sourceItem.name;
	copiedItem.layer = sourceItem.layer;
	copiedItem.color = statueHue;
	copiedItem.movable = 2;
	copiedItem.container = statue;
}

/** @type { ( pUser: Character, plinth: Item ) => void } */
function ShowCharacterStatueGump( pUser, plinth )
{
	if( !ValidateObject( pUser ) || !ValidateObject( plinth ))
	{
		return;
	}

	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	var statue = GetStatueFromPlinth( plinth );
	if( !ValidateObject( statue ))
	{
		socket.SysMessage( "This statue is missing." );
		return;
	}

	var statueType = GetClampedStatueValue( plinth.GetTag( "statueType" ), 0, 2 );
	var statueMaterial = GetClampedStatueValue( plinth.GetTag( "statueMaterial" ), 0, 3 );
	var statuePose = GetClampedStatueValue( plinth.GetTag( "statuePose" ), 0, 5 );

	pUser.SetTempTag( "CharacterStatuePlinthSerial", plinth.serial );

	var statueGump = new Gump();
	statueGump.AddPage( 0 );
	statueGump.AddBackground( 60, 36, 327, 324, 0x13BE );
	statueGump.AddTiledGump( 70, 46, 307, 20, 0x0A40 );
	statueGump.AddTiledGump( 70, 76, 307, 244, 0x0A40 );
	statueGump.AddTiledGump( 70, 330, 307, 20, 0x0A40 );
	statueGump.AddCheckerTrans( 70, 46, 307, 304 );
	statueGump.AddText( 82, 48, 1152, "Character Statue Maker" );

	statueGump.AddText( 193, 77, 1152, "Choose Pose" );
	statueGump.AddText( 193, 97, 1918, statuePoseNames[statuePose] );
	statueGump.AddButton( 193, 117, 0x0FAE, 0x0FB0, 1, 0, 1 );
	statueGump.AddButton( 223, 117, 0x0FA5, 0x0FA7, 1, 0, 2 );

	statueGump.AddText( 193, 162, 1152, "Choose Direction" );
	statueGump.AddText( 193, 182, 1918, GetDirectionName( statue.direction ) );
	statueGump.AddButton( 193, 203, 0x0FAE, 0x0FB0, 1, 0, 3 );
	statueGump.AddButton( 223, 203, 0x0FA5, 0x0FA7, 1, 0, 4 );

	statueGump.AddText( 193, 247, 1152, "Choose Material" );
	statueGump.AddText( 193, 267, 1918, statueMaterialNames[statueMaterial] + " " + statueTypeNames[statueType] );
	statueGump.AddButton( 193, 289, 0x0FAE, 0x0FB0, 1, 0, 5 );
	statueGump.AddButton( 223, 289, 0x0FA5, 0x0FA7, 1, 0, 6 );

	statueGump.AddButton( 70, 330, 0x0FB1, 0x0FB2, 1, 0, 0 );
	statueGump.AddText( 105, 330, 1152, "Close" );
	statueGump.AddButton( 165, 330, 0x0FAB, 0x0FAD, 1, 0, 8 );
	statueGump.AddText( 200, 330, 1152, "Redeed" );
	statueGump.AddButton( 294, 330, 0x0FB7, 0x0FB9, 1, 0, 7 );
	statueGump.AddText( 329, 330, 1152, "Apply" );

	statueGump.Send( socket );
	statueGump.Free();
}

/** @type { ( socket: Socket, button: number, gumpData: GumpData ) => void } */
function onGumpPress( socket, button, gumpData )
{
	if( socket == null )
	{
		return;
	}

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
	{
		return;
	}

	var plinthSerial = parseInt( pUser.GetTempTag( "CharacterStatuePlinthSerial" ), 10 );
	if( isNaN( plinthSerial ) || plinthSerial <= 0 )
	{
		return;
	}

	var plinth = CalcItemFromSer( plinthSerial );
	if( !ValidateObject( plinth ))
	{
		return;
	}

	var statue = GetStatueFromPlinth( plinth );
	if( !ValidateObject( statue ))
	{
		socket.SysMessage( "This statue is missing." );
		return;
	}

	if( !CanModifyCharacterStatue( pUser, plinth ))
	{
		socket.SysMessage( "You cannot modify this statue." );
		return;
	}

	var shouldShowGump = true;
	switch( button )
	{
		case 0:
			shouldShowGump = false;
			break;
		case 1:
			ChangeStatuePose( statue, plinth, -1 );
			break;
		case 2:
			ChangeStatuePose( statue, plinth, 1 );
			break;
		case 3:
			ChangeStatueDirection( statue, -1 );
			break;
		case 4:
			ChangeStatueDirection( statue, 1 );
			break;
		case 5:
			ChangeStatueMaterial( statue, plinth, -1 );
			break;
		case 6:
			ChangeStatueMaterial( statue, plinth, 1 );
			break;
		case 7:
			ApplyStatueAppearance( statue, plinth );
			break;
		case 8:
			RedeedCharacterStatue( pUser, statue, plinth );
			shouldShowGump = false;
			break;
		default:
			shouldShowGump = false;
			break;
	}

	if( shouldShowGump && ValidateObject( plinth ))
	{
		ShowCharacterStatueGump( pUser, plinth );
	}
}

/** @type { ( statue: Character, plinth: Item, direction: number ) => void } */
function ChangeStatuePose( statue, plinth, direction )
{
	var statuePose = GetClampedStatueValue( plinth.GetTag( "statuePose" ), 0, 5 );
	statuePose += direction;

	if( statuePose < 0 )
	{
		statuePose = 5;
	}
	else if( statuePose > 5 )
	{
		statuePose = 0;
	}

	plinth.SetTag( "statuePose", statuePose );
	statue.SetTag( "statuePose", statuePose );

	InvalidateCharacterStatuePose( statue, plinth );
}

/** @type { ( statue: Character, direction: number ) => void } */
function ChangeStatueDirection( statue, direction )
{
	var statueDirection = parseInt( statue.direction, 10 );
	if( isNaN( statueDirection ))
	{
		statueDirection = 4;
	}

	statueDirection += direction;
	if( statueDirection < 0 )
	{
		statueDirection = 7;
	}
	else if( statueDirection > 7 )
	{
		statueDirection = 0;
	}

	statue.direction = statueDirection;
	statue.Refresh();
}

/** @type { ( statue: Character, plinth: Item, direction: number ) => void } */
function ChangeStatueMaterial( statue, plinth, direction )
{
	var statueType = GetClampedStatueValue( plinth.GetTag( "statueType" ), 0, 2 );
	var statueMaterial = GetClampedStatueValue( plinth.GetTag( "statueMaterial" ), 0, 3 );
	statueMaterial += direction;
	if( statueMaterial < 0 )
	{
		statueMaterial = 3;
	}
	else if( statueMaterial > 3 )
	{
		statueMaterial = 0;
	}

	plinth.SetTag( "statueMaterial", statueMaterial );
	statue.SetTag( "statueMaterial", statueMaterial );
	SetStatueHue( statue, plinth, GetCharacterStatueHue( statueType, statueMaterial ) );
}

/** @type { ( statue: Character, plinth: Item ) => void } */
function ApplyStatueAppearance( statue, plinth )
{
	InvalidateCharacterStatuePose( statue, plinth );
}

/** @type { ( statue: Character, plinth: Item, statueHue: number ) => void } */
function SetStatueHue( statue, plinth, statueHue )
{
	statue.color = statueHue;
	plinth.color = statueHue;

	var currentItem;
	for( currentItem = statue.FirstItem(); !statue.FinishedItems(); currentItem = statue.NextItem() )
	{
		if( ValidateObject( currentItem ))
		{
			currentItem.color = statueHue;
		}
	}
}

/** @type { ( pUser: Character, statue: Character, plinth: Item ) => void } */
function RedeedCharacterStatue( pUser, statue, plinth )
{
	var socket = pUser.socket;
	if( socket == null )
	{
		return;
	}

	var makerSection = plinth.GetTag( "makerSection" );
	if( !makerSection )
	{
		var statueType = GetClampedStatueValue( plinth.GetTag( "statueType" ), 0, 2 );
		makerSection = statueMakerSections[statueType];
	}

	var deedItem = CreateDFNItem( socket, pUser, String( makerSection ), 1, "ITEM", true );
	if( !ValidateObject( deedItem ))
	{
		socket.SysMessage( "You do not have enough room in your backpack." );
		return;
	}

	statue.Delete();
	plinth.Delete();
	socket.SysMessage( "The statue has been returned to deed form." );
}

/** @type { ( pUser: Character, plinth: Item ) => boolean } */
function CanModifyCharacterStatue( pUser, plinth )
{
	if( !ValidateObject( pUser ) || !ValidateObject( plinth ))
	{
		return false;
	}

	var ownerSerial = parseInt( plinth.GetTag( "statueOwnerSerial" ), 10 );
	if( !isNaN( ownerSerial ) && ownerSerial == pUser.serial )
	{
		return true;
	}

	var statueMulti = FindMulti( plinth.x, plinth.y, plinth.z, plinth.worldnumber, plinth.instanceID );
	if( ValidateObject( statueMulti ))
	{
		return ( statueMulti.IsOwner( pUser ) || statueMulti.IsOnOwnerList( pUser ) || statueMulti.IsOnFriendList( pUser ) );
	}

	return false;
}

/** @type { ( plinth: Item ) => Character | null } */
function GetStatueFromPlinth( plinth )
{
	if( !ValidateObject( plinth ))
	{
		return null;
	}

	var statueSerial = parseInt( plinth.GetTag( "statueSerial" ), 10 );
	if( isNaN( statueSerial ) || statueSerial <= 0 )
	{
		return null;
	}

	var statue = CalcCharFromSer( statueSerial );
	if( !ValidateObject( statue ))
	{
		return null;
	}

	return statue;
}

/** @type { ( makerItem: Item ) => number } */
function GetStatueTypeFromMaker( makerItem )
{
	if( !ValidateObject( makerItem ))
	{
		return -1;
	}

	var statueTypeText = String( makerItem.GetTag( "statueType" ) || "" ).toLowerCase();
	if( statueTypeText == "marble" )
	{
		return 0;
	}
	if( statueTypeText == "jade" )
	{
		return 1;
	}
	if( statueTypeText == "bronze" )
	{
		return 2;
	}

	return -1;
}

/** @type { ( statueType: number, statueMaterial: number ) => number } */
function GetCharacterStatueHue( statueType, statueMaterial )
{
	statueType = GetClampedStatueValue( statueType, 0, 2 );
	statueMaterial = GetClampedStatueValue( statueMaterial, 0, 3 );
	return 0x0B8F + ( statueType * 4 ) + statueMaterial;
}

/** @type { ( value: any, minValue: number, maxValue: number ) => number } */
function GetClampedStatueValue( value, minValue, maxValue )
{
	var parsedValue = parseInt( value, 10 );
	if( isNaN( parsedValue ))
	{
		parsedValue = minValue;
	}

	if( parsedValue < minValue )
	{
		parsedValue = minValue;
	}
	else if( parsedValue > maxValue )
	{
		parsedValue = maxValue;
	}

	return parsedValue;
}

/** @type { ( direction: number ) => string } */
function GetDirectionName( direction )
{
	direction = GetClampedStatueValue( direction, 0, 7 );
	var directionNames = [ "North", "Right", "East", "Down", "South", "Left", "West", "Up" ];
	return directionNames[direction];
}

var statuePosePacketStatue = null;
var statuePosePacketAnimation = 0;
var statuePosePacketFrame = 0;

/** @type { ( statue: Character, pose: number ) => void } */
function SendCharacterStatuePosePacket( statue, pose )
{
	if( !ValidateObject( statue ))
	{
		return;
	}

	pose = GetClampedStatueValue( pose, 0, 5 );

	statuePosePacketStatue = statue;
	statuePosePacketAnimation = statuePoseActions[pose];
	statuePosePacketFrame = statuePoseFrames[pose];

	AreaCharacterFunction( "SendCharacterStatuePosePacketToChar", statue, 18, null );

	statuePosePacketStatue = null;
	statuePosePacketAnimation = 0;
	statuePosePacketFrame = 0;
}

/** @type { ( srcChar: Character, trgChar: Character, socket: Socket ) => boolean } */
function SendCharacterStatuePosePacketToChar( srcChar, trgChar, socket )
{
	if( !ValidateObject( statuePosePacketStatue ) || !ValidateObject( trgChar ))
	{
		return false;
	}

	var targetSocket = trgChar.socket;
	if( targetSocket == null )
	{
		return false;
	}

	var toSend = new Packet();
	toSend.ReserveSize( 17 );

	toSend.WriteByte( 0, 0xBF ); // Packet
	toSend.WriteShort( 1, 0x0011 ); // Length
	toSend.WriteShort( 3, 0x0019 ); // Statue animation subcommand
	toSend.WriteByte( 5, 0x05 );
	toSend.WriteLong( 6, statuePosePacketStatue.serial );
	toSend.WriteByte( 10, 0x00 );
	toSend.WriteByte( 11, 0xFF );
	toSend.WriteByte( 12, 0x01 ); // Status
	toSend.WriteByte( 13, 0x00 );
	toSend.WriteByte( 14, statuePosePacketAnimation );
	toSend.WriteByte( 15, 0x00 );
	toSend.WriteByte( 16, statuePosePacketFrame );

	targetSocket.Send( toSend );
	toSend.Free();

	return true;
}

/** @type { ( statue: Character, plinth: Item ) => void } */
function InvalidateCharacterStatuePose( statue, plinth )
{
	if( !ValidateObject( statue ) || !ValidateObject( plinth ))
	{
		return;
	}

	var statuePose = GetClampedStatueValue( plinth.GetTag( "statuePose" ), 0, 5 );

	statue.Refresh();
	SendCharacterStatuePosePacket( statue, statuePose );
}
