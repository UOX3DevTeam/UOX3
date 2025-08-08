const dragonArmors = [ "0x2645", "0x2647", "0x2643", "0x2641", "0x2657" ];

// Choose and apply scale material to chaos dragoon armor
function onCreateDFN( chaosDragoonNPC, objType )
{
	if( objType != 1 || !chaosDragoonNPC.npc )
		return;

	// Select a random scale color to use
	var scaleColor = GetRandomScaleColor();

	// Apply scale color to all equipped armor pieces
	ApplyArmorScaleColor( chaosDragoonNPC, scaleColor );

	// Add scale(s) of selected color as loot for NPC
	AddScaleLoot( chaosDragoonNPC, scaleColor );
}

function GetRandomScaleColor()
{
	switch( RandomNumber( 1, 6 ))
	{
		default:
		case 1: // Red Scale
			return 0x066d;
		case 2: // Yellow Scale
			return 0x08a8;
		case 3: // Black Scale
			return 0x0455;
		case 4: // Green Scale
			return 0x0851;
		case 5: // White Scale
			return 0x08fd;
		case 6: // Blue Scale
			return 0x08b0;
	}
}

function ApplyArmorScaleColor( chaosDragoonNPC, scaleColor )
{
	// Iterate through all equipped items and apply scale color to all dragon armor pieces
	var mItem;
	for( mItem = chaosDragoonNPC.FirstItem(); !chaosDragoonNPC.FinishedItems(); mItem = chaosDragoonNPC.NextItem() )
	{
		if( !ValidateObject( mItem ) || dragonArmors.indexOf( mItem.sectionID ) == -1 )
			continue;

		mItem.color = scaleColor;

		// Also make them newbiefied so they don't drop on death
		mItem.isNewbie = true;
	}

	// Also apply same color to armored swamp dragon for chaos dragoon elites
	var mountItem = chaosDragoonNPC.FindItemLayer( 0x19 );
	if( ValidateObject( mountItem ) && mountItem.sectionID.toLowerCase() == "armorswampdragon" )
	{
		mountItem.color = scaleColor;
	}
}

function AddScaleLoot( chaosDragoonNPC, scaleColor )
{
	var amount = RandomNumber( 1, 3 );

	var scaleItem;
	switch( scaleColor )
	{
		default:
		case 0x066d: // Red Scale
			scaleItem = "0xc006";
			break;
		case 0x08a8: // Yellow Scale
			scaleItem = "0xc008";
			break;
		case 0x0455: // Black Scale
			scaleItem = "0xc003";
			break;
		case 0x0851: // Green Scale
			scaleItem = "0xc005";
			break;
		case 0x08fd: // White Scale
			scaleItem = "0xc007";
			break;
		case 0x08b0: // Blue Scale
			scaleItem = "0xc004";
			break;
	}

	// Add scales of specified color and amount as loot to NPC's backpack
	var scaleLoot = CreateDFNItem( null, chaosDragoonNPC, scaleItem, amount, "ITEM", true );
}

function onDeath( chaosDragoonNPC, iCorpse )
{
	// Spawn a (non-armored) swamp dragon after chaos dragoon NPC dies
	var nSpawned = SpawnNPC( "swampdragon", chaosDragoonNPC.x, chaosDragoonNPC.y, chaosDragoonNPC.z, chaosDragoonNPC.worldnumber, chaosDragoonNPC.instanceID );
	return true;
}