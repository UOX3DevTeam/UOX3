function inRange( bakeKitsune, objInRange ) 
{
	if( !ValidateObject( objInRange ) || objInRange.npc || !objInRange.online || objInRange.dead || objInRange.isGM || objInRange.isCounselor || myNPC.tamed )
	{
		return;
	}

	var disguiseOnOff = bakeKitsune.GetTempTag("Disguise");

	if (bakeKitsune.atWar == false && !bakeKitsune.isDisguised && disguiseOnOff == 0 && RandomNumber( 0, 1 ) == 1)
	{
		var delay = RandomNumber( 15000, 30000 );

		bakeKitsune.StartTimer( delay, 0, true );
		bakeKitsune.SetTempTag( "Disguise", 1 );
	}

	var iTime = GetCurrentClock();
	var initSearchTime = parseInt( bakeKitsune.GetTempTag("initSearchTime" ) || 0 );

	if( initSearchTime !== 0 && (((( iTime - initSearchTime ) / 1000 ) < 15000 ) && !( initSearchTime > iTime )))
	{
		return;
	}

	if(((( iTime - initSearchTime ) / 1000 ) > 15000 ) || initSearchTime > iTime )
	{
		bakeKitsune.SetTempTag( "initSearchTime", iTime.toString() );
		bakeKitsune.StartTimer( 5000, 1, true );
	}
}

function searchForPlayers( bakeKitsune, trgChar, pSock )
{
	if( bakeKitsune == trgChar )
		return false;

	if( !ValidateObject( trgChar ) || trgChar.npc || !trgChar.online || trgChar.dead || trgChar.isGM || trgChar.isCounselor )
		return false;

	var disguiseOnOff = bakeKitsune.GetTempTag( "Disguise" );

	if( bakeKitsune.atWar == false && !bakeKitsune.isDisguised && disguiseOnOff == 0 && RandomNumber( 0, 1 ) == 1 ) 
	{
		var delay = RandomNumber( 15000, 30000 );

		bakeKitsune.StartTimer( delay, 0, true );
		bakeKitsune.SetTempTag( "Disguise", 1 );
	}

	return true;
}

function CreateDisguise( bakeKitsune )
{
	bakeKitsune.isDisguised = true;
	bakeKitsune.StaticEffect( 0x376A, 8, 32 );
	bakeKitsune.SoundEffect( 0x1FE, true );

	// Randomize gender and apply a random name from namelists
	var randomGender = RandomNumber( 0, 1 );
	bakeKitsune.gender = randomGender ? 0 : 1;

	if( bakeKitsune.gender == 1 )
	{
		bakeKitsune.id = 0x191;
		bakeKitsune.SetRandomName( "2" ); // Random female name from namelist 2
	}
	else
	{
		bakeKitsune.id = 0x190;
		bakeKitsune.SetRandomName( "1" ); // Random male name from namelist 1
	}

	bakeKitsune.title = "the mystic llama herder";
	bakeKitsune.SetRandomColor( "16" );//skin colours without dyed underwear
	bakeKitsune.race = 0;

	var hairItem = CreateBlankItem( null, bakeKitsune, 1, "hair", RandomHair(), 0, "ITEM", true );
	if( hairItem )
	{
		hairItem.container = bakeKitsune;
		hairItem.layer = 0x0B;
		hairItem.SetRandomColor( "15" );//Random hair color
	}

	var facialHairItem = CreateBlankItem( null, bakeKitsune, 1, "facial", RandomFacialHair(), 0, "ITEM", true );
	if( facialHairItem ) 
	{
		facialHairItem.container = bakeKitsune;
		facialHairItem.layer = 0x10;
		facialHairItem.SetRandomColor( "15" );//Random hair color
	}

	// Equip random shoes
	switch( RandomNumber( 0, 3 ))
	{
		case 0:
			var randomshoes = CreateDFNItem( null, bakeKitsune, "Shoes", 1, "ITEM", false, 0 );
			break;
		case 1:
			var randomshoes = CreateDFNItem( null, bakeKitsune, "Boots", 1, "ITEM", false, 0 );
			break;
		case 2:
			var randomshoes = CreateDFNItem( null, bakeKitsune, "Sandals", 1, "ITEM", false, 0 );
			break;
		case 3:
			var randomshoes = CreateDFNItem( null, bakeKitsune, "ThighBoots", 1, "ITEM", false, 0 );
			break;
	}

	if (randomshoes)
	{
		randomshoes.container = bakeKitsune;
		randomshoes.layer = 0x3;
		randomshoes.SetRandomColor( "9" );//neutral colors
	}

	// Equip robe
	var robe = CreateDFNItem( null, bakeKitsune, "Robe", 1, "ITEM", false, RandomNondyedHue() );
	if( robe )
	{
		robe.container = bakeKitsune;
		robe.layer = 0x16;
		switch( RandomNumber( 0, 5 )) 
		{
			case 0:
				robe.SetRandomColor( "8" );//Random Pink
				break;
			case 1:
				robe.SetRandomColor( "2" );//Random Blue
				break;
			case 2:
				robe.SetRandomColor( "4" );//Random Green
				break;
			case 3:
				robe.SetRandomColor( "5" );//Random Orange
				break;
			case 4:
				robe.SetRandomColor( "1" );//Random Red
				break;
			case 5:
				robe.SetRandomColor( "3" );//Random Yellow
				break;
		}
	}

	bakeKitsune.SetTempTag( "Disguise", 0 );
	bakeKitsune.StartTimer( 75000, 0, true );
}

function onTimer( bakeKitsune, timerID ) 
{
	var disguiseOnOff = bakeKitsune.GetTempTag( "Disguise" );
	if( timerID == 0 )
	{
		if( disguiseOnOff == 1 ) 
		{
			CreateDisguise( bakeKitsune );
		}
		else if(disguiseOnOff == 0 )
		{
			RemoveDisguise( bakeKitsune );
		}
	}
	else if( timerID == 1 )
	{
		AreaCharacterFunction( "searchForPlayers", bakeKitsune, 24 );
		bakeKitsune.StartTimer( 5000, 1, true );
	}
}


function RemoveDisguise( bakeKitsune )
{
	if( !bakeKitsune.isDisguised )
		return;

	bakeKitsune.name = "a bake kitsune";
	bakeKitsune.title = null;
	bakeKitsune.id = 0xf6;
	bakeKitsune.colour = 0;
	bakeKitsune.race = 9;
	var hair = bakeKitsune.FindItemLayer( 0x0B );
	var facial = bakeKitsune.FindItemLayer( 0x10 );
	var shoes = bakeKitsune.FindItemLayer( 0x03 );
	var robe = bakeKitsune.FindItemLayer( 0x16 );
	if( robe != null || shoes != null || hair != null || facial != null )
	{
		shoes.Delete();
		robe.Delete();
		hair.Delete();
		facial.Delete();
	}

	bakeKitsune.isDisguised = false;
	disguiseTimer = null;
}

function onDeathBlow( bakeKitsune, pKiller )
{
	RemoveDisguiseCallback( bakeKitsune );
	return true;
}

function RandomHair()
{
	var hairOptions = [ 0x203B, 0x203C, 0x203D, 0x2044, 0x2045, 0x2047, 0x2049, 0x204A ]; // human hair
	return hairOptions[ RandomNumber( 0, hairOptions.length - 1 )];
}

function RandomFacialHair()
{
	var facialHairOptions = [ 0x203E, 0x203F, 0x2040, 0x2041, 0x204B, 0x204C, 0x204D ]; // human facial hair
	return facialHairOptions[ RandomNumber( 0, facialHairOptions.length - 1 )];
}