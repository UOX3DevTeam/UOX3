// @ts-check
let AQUA_EVAL_MS = 86400000; // daily tick

function initAquarium( aquarium )
{
	// Skip if already initialized
	if(( aquarium.GetTag( "aqua_inited" ) | 0 ) === 1 )
	{
		aquarium.StartTimer( AQUA_EVAL_MS, 1, true );
		return;
	}

	// Baseline counts
	aquarium.SetTag( "aqua_live", 0 );
	aquarium.SetTag( "aqua_dead", 0 );
	aquarium.SetTag( "aqua_decor", 0 );
	aquarium.SetTag( "aqua_reward", 0 );
	aquarium.SetTag( "aqua_vacDays", 0 );
	aquarium.SetTag( "aqua_evalDay", 1 ); // start on state day

	// Food defaults ( Full + thresholds )
	aquarium.SetTag( "aqua_food_state", 3 );
	var fishMaint = RandomNumber( 1, 2 ); // 1..2
	aquarium.SetTag( "aqua_food_maint", fishMaint );
	aquarium.SetTag( "aqua_food_impr", fishMaint + 2 );
	aquarium.SetTag( "aqua_food_added", 0 );

	// Water defaults ( Strong + thresholds )
	aquarium.SetTag( "aqua_water_state", 4 );
	var waterMaint = RandomNumber( 1, 3 ); // 1..3
	aquarium.SetTag( "aqua_water_maint", waterMaint );
	aquarium.SetTag( "aqua_water_impr", waterMaint + 2 );
	aquarium.SetTag( "aqua_water_added", 0 );

	// Ordered list tag ( CSV; keep empty string )
	aquarium.SetTag( "aqua_list", "" );

	// Soft capacity ( fallback if unset/zero )
	var cap = aquarium.GetTag( "aqua_maxItems" );
	if( cap <= 0 )
		aquarium.SetTag( "aqua_maxItems", 30 );

	// Mark initialized and start timer
	aquarium.SetTag( "aqua_inited", 1 );
	aquarium.SetTag( "isAquariumTank", 1 );
	aquarium.StartTimer( AQUA_EVAL_MS, 1, true );
}

function totalItems( aquarium )
{
	var live = aquarium.GetTag( "aqua_live" );
	var dead = aquarium.GetTag( "aqua_dead" );
	var deco = aquarium.GetTag( "aqua_decor" );
	return ( live + dead + deco );
}

function maxLiveCreatures( aquarium )
{
	var fish = aquarium.GetTag( "aqua_food_state" );
	var water = aquarium.GetTag( "aqua_water_state" );

	var state = (( fish === 4 ) ? 1 : ( 3 - fish )) + ( 4 - water );
	if( state < 0 )
		state = 0;

	var penalty = Math.floor( Math.pow( state, 1.75 ));
	var baseCap = aquarium.GetTag( "aqua_maxItems" );
	if( baseCap <= 0 )
		baseCap = 30;

	var cap = baseCap - penalty;
	if( cap < 0 )
		cap = 0;
	return cap | 0;
}

function updateFood( aquarium )
{
	var foodState = aquarium.GetTag( "aqua_food_state" );
	var foodMaint = aquarium.GetTag( "aqua_food_maint" );
	var foodImpr = aquarium.GetTag( "aqua_food_impr" );
	var foodAdd = aquarium.GetTag( "aqua_food_added" );

	if( foodImpr <= 0 && foodState !== 4 && foodState !== 0 )
		foodImpr = foodMaint + 2;

	if( foodAdd < foodMaint )
		foodState = ( foodState > 0 ) ? ( foodState - 1 ) : 0;
	else if( foodAdd >= foodImpr )
		foodState = ( foodState < 4 ) ? ( foodState + 1 ) : 4;

	foodMaint = RandomNumber( 1, Math.max( 1, ( 4 - foodState )));
	foodImpr = ( foodState === 4 ) ? 0 : ( foodMaint + 2 );
	foodAdd = 0;

	aquarium.SetTag( "aqua_food_state", foodState );
	aquarium.SetTag( "aqua_food_maint", foodMaint );
	aquarium.SetTag( "aqua_food_impr", foodImpr );
	aquarium.SetTag( "aqua_food_added", foodAdd );
}

function updateWater( aquarium )
{
	var WaterState = aquarium.GetTag( "aqua_water_state" );
	var WaterMaint = aquarium.GetTag( "aqua_water_maint" );
	var WaterImpr = aquarium.GetTag( "aqua_water_impr" );
	var WaterAdd = aquarium.GetTag( "aqua_water_added" );

	if( WaterImpr <= 0 && WaterState !== 4 && WaterState !== 0 )
		WaterImpr = WaterMaint + 2;

	if( WaterAdd < WaterMaint )
		WaterState = ( WaterState > 0 ) ? ( WaterState - 1 ) : 0;
	else if( WaterAdd >= WaterImpr )
		WaterState = ( WaterState < 4 ) ? ( WaterState + 1 ) : 4;

	WaterMaint = RandomNumber( 1, Math.max( 1, ( 4 - WaterState )) );
	WaterImpr = ( WaterState === 4 ) ? 0 : ( WaterMaint + 2 );
	WaterAdd = 0;

	aquarium.SetTag( "aqua_water_state", WaterState );
	aquarium.SetTag( "aqua_water_maint", WaterMaint );
	aquarium.SetTag( "aqua_water_impr", WaterImpr );
	aquarium.SetTag( "aqua_water_added", WaterAdd );
}

function hatchOrCull( aquarium )
{
	var live = aquarium.GetTag( "aqua_live" );
	var cap  = maxLiveCreatures( aquarium );

	var fish = aquarium.GetTag( "aqua_food_state" );
	var water = aquarium.GetTag( "aqua_water_state" );
	var optimal = ( fish === 3 && water === 4 );

	if( optimal && live < cap )
	{
		var chance = 0.005 * live;
		if( Math.random() < chance )
		{
			live += 1;
			aquarium.SetTag( "aqua_live", live );
		}
	}

	cap = maxLiveCreatures( aquarium );

	if( live > cap )
	{
		var kill = live - cap;

		// try to mark 'kill' number of live fish items dead
		var actuallyKilled = 0;
		for( var k = 0; k < kill; k++ )
		{
			if( markRandomFishDead( aquarium ))
				actuallyKilled++;
			else
				break;
		}

		if( actuallyKilled > 0 )
		{
			live -= actuallyKilled;
			var dead = aquarium.GetTag( "aqua_dead" );
			dead += actuallyKilled;
			aquarium.SetTag( "aqua_live", live );
			aquarium.SetTag( "aqua_dead", dead );
		}
	}
	else
	{
		if( Math.random() < 0.01 && live > 0 )
		{
			// convert one live fish item to dead art
			var changed = markRandomFishDead( aquarium );
			// only decrement counters if we actually flipped an item
			if( changed )
			{
				live -= 1;
				var dead2 = aquarium.GetTag( "aqua_dead" );
				dead2 += 1;
				aquarium.SetTag( "aqua_live", live );
				aquarium.SetTag( "aqua_dead", dead2 );
			}
		}
	}
}

// Put near top with other helpers
function stopFishAirTimer( fish )
{
	fish.KillJSTimer( 1, 7532 );
	fish.SetTag( "fishAirActive", 0 );
}

function doEvaluate( aquarium )
{
	var vac = aquarium.GetTag( "aqua_vacDays" );
	if( vac > 0 )
	{
		aquarium.SetTag( "aqua_vacDays", vac - 1 );
		return;
	}

	var evalDay = aquarium.GetTag( "aqua_evalDay" );
	if( evalDay === 1 )
	{
		updateFood( aquarium );
		updateWater( aquarium );

		var live = aquarium.GetTag( "aqua_live" );
		if( live > 0 )
			aquarium.SetTag( "aqua_reward", 1 );
	}
	else
	{
		hatchOrCull( aquarium );
	}

	aquarium.SetTag( "aqua_evalDay", ( evalDay === 1 ) ? 0 : 1 );
	aquarium.Refresh();
}

function giveReward( pUser, aquarium )
{
	var rwd = aquarium.GetTag( "aqua_reward" );
	if( rwd !== 1 )
	{
		pUser.SysMessage( "No reward available." );
		return;
	}

	var live   = aquarium.GetTag( "aqua_live" );
	var maxTot = aquarium.GetTag( "aqua_maxItems" );
	if( maxTot <= 0 )
		maxTot = 30;

	var bucket = Math.floor(( live / Math.max( 1, maxTot )) * 5 );
	if( bucket < 0 )
		bucket = 0;
	if( bucket > 4 )
		bucket = 4;

	var table = [ "Shell",  "Shell1", "AquariumMessage", "CaptainBlackheartsFishingPole", "CraftysFishingHat", "FishBones", "IslandStatue", "ToyBoat", "WaterloggedBoots", "Coral", "Coral1", "Coral2", "brineshrimp", "fullmoonfish", "seahorsefish", "strippedflakefish", "StrippedSosarianSwill"];
	var sid = table[bucket];

	if( sid )
	{
		var r = CreateDFNItem( pUser.socket, pUser, sid, 1, "ITEM", true );
		if( ValidateObject( r ))
		{
			pUser.SoundEffect( 0x05A3, true);
			pUser.SysMessage( "You receive a reward." );
		}
	}
	aquarium.SetTag( "aqua_reward", 0 );
}

function markRandomFishDead( aquarium )
{
    var items = readAquaList( aquarium );
    if( !items.length ) 
		return false;

    // filter to live fish (isAquariumFish=1 and not dead)
    var candidates = [];
    for( var i = 0; i < items.length; i++ )
    {
        var aquariumItem = items[i];
        if( !ValidateObject( aquariumItem ))
			continue;

        if( !ValidateObject( aquariumItem.container) || aquariumItem.container.serial !== aquarium.serial )
			continue;

        var isFish = aquariumItem.GetTag( "isAquariumFish" );
        var isDead = aquariumItem.GetTag( "dead" );
        if( isFish === 1 && isDead === 0 )
			candidates.push( aquariumItem );
    }
    if( !candidates.length )
		return false;

    var pick = candidates[( Math.random() * candidates.length ) | 0];

    // flip to bones/dead art + tag
    pick.SetTag( "dead", 1 );
    pick.color = 0;               // optional: clear hue
    pick.id = 0x3B0C;       // dead art
    pick.Refresh();

    return true;
}

function BID_REMOVE( number )
{ 
	return 1000 + ( number|0 );
}

function readAquaList( aquarium )
{
	var raw = aquarium.GetTag( "aqua_list" );
	if( raw === null || raw === 0 ) 
		return [];

	if( raw === "" )
		return [];
	var parts = raw.split( "," );
	var out = [];
	for(  var i = 0; i < parts.length; i++  )
	{
		var s = parts[i];
		if( s === 0 )
			continue;

		var ser = parseInt( s, 10 );
		if( isNaN( ser ))
			continue;

		var aquariumItem = CalcItemFromSer( ser );
		if( ValidateObject( aquariumItem ))
			out.push( aquariumItem );
	}
	return out;
}

function writeAquaList( aquarium, items )
{
	var ss = [];
	for( var i = 0; i < items.length; i++ )
	{
		if( ValidateObject( items[i] ))
			ss.push( String( items[i].serial ));
	}
	aquarium.SetTag( "aqua_list", ss.join( "," ));
}

function sanitizeAquaList( aquarium )
{
	var items = readAquaList( aquarium );
	writeAquaList( aquarium, items );
	return items;
}

function openAquaGump( pUser, aquarium )
{
	var items = sanitizeAquaList( aquarium );
	var count = items.length;

	var cur = pUser.GetTempTag( "aqua_page" ) | 0;
	if( cur <= 0 )
		cur = 1;

	if( count === 0 )
		cur = 1;

	if( cur > count )
		cur = count;

	var aquariumGump = new Gump;
	aquariumGump.AddBackground( 0, 0, 350, 323, 0x0E10 );
	aquariumGump.AddGump( 0, 0, 0x2C96 );

	// remember binding + current page
	pUser.SetTag( "aqua_serial", aquarium.serial );
	pUser.SetTempTag( "aqua_page", cur );

	if( count === 0 )
	{
		aquariumGump.AddText( 20, 195, 0x480, "No items in this aquarium." );
		aquariumGump.Send( pUser.socket );
		aquariumGump.Free();
		return;
	}

	var aquariumItem = items[cur - 1];
	aquariumGump.AddPage( 1 );

	var itemName = ( aquariumItem.name && aquariumItem.name !== "" ) ? aquariumItem.name : ( "Item 0x" + aquariumItem.id.toString( 16 ));
	aquariumGump.AddText( 20, 217, 0x480, itemName );

	var isFish = ( aquariumItem.GetTag( "isAquariumFish" ));
	aquariumGump.AddText( 20, 239, 0x480, ( isFish === 1 ) ? "Aquarium creature" : "An aquarium decoration" );

	if( typeof aquariumGump.AddPictureColor === "function" )
		aquariumGump.AddPictureColor( 150, 80, aquariumItem.id, aquariumItem.color | 0 );
	else
		aquariumGump.AddPicture( 150, 80, aquariumItem.id );

	aquariumGump.AddText( 20, 195, 0x480, ( cur + "/" + count ));

	var edit = ( pUser.isGM === true );
	if( edit )
	{
		aquariumGump.AddBackground( 230, 195, 100, 26, 0x13BE );
		// ACTION button ( close=1 ), uniqueID = BID_REMOVE( cur ), page=0
		aquariumGump.AddButton( 235, 200, 0x0845, 1, 0, BID_REMOVE( cur ));
		aquariumGump.AddXMFHTMLTok(  260, 198, 60, 26, false, false, 0, 1073838, "", "", ""  );// Remove
	}

	// Prev ( ACTION: close=1, go handle in onGumpPress )
	if( cur > 1 )
	{
		aquariumGump.AddButton( 45, 280, 0x0FAE, 1, 0, 90002 );
		aquariumGump.AddXMFHTMLTok(  80, 283, 100, 18, false, false, 0xFFFFFF, 1044044, "", "", ""  );// PREV PAGE
	}

	// Next ( ACTION: close=1 )
	if( cur < count )
	{
		aquariumGump.AddButton( 195, 280, 0x0FA5, 1, 0, 90001 );
		aquariumGump.AddXMFHTMLTok(  230, 283, 100, 18, false, false, 0xFFFFFF, 1044045, "", "", ""  );// NEXT PAGE
	}

	aquariumGump.Send( pUser.socket );
	aquariumGump.Free();
}

function showEvents( pUser, aquarium )
{
	var aquariumGump = new Gump;
	aquariumGump.AddBackground( 0, 0, 220, 350, 2600 );

	var y = 30;
	aquariumGump.AddText( 55, y, 0x480, "Aquarium Events" );
	y += 20;

	var raw = aquarium.GetTag( "aqua_events" );
	if( !( raw === null || raw === 0 ))
	{
		if( raw !== 0 )
		{
			var parts = raw.split( "," );
			for( var i = 0; i < parts.length; i++ )
			{
				if( parts[i] === 0 )
					continue;

				aquariumGump.AddText( 18, y, 0x480, "- " + parts[i] ); y += 16;
			}
		}
	}
	else
	{
		var live = aquarium.GetTag( "aqua_live" );
		var dead = aquarium.GetTag( "aqua_dead" );
		var deco = aquarium.GetTag( "aqua_decor" );
		var fSt  = aquarium.GetTag( "aqua_food_state" );
		var wSt  = aquarium.GetTag( "aqua_water_state" );

		aquariumGump.AddText( 18, y, 0x480, "Live: " + live + "  Dead: " + dead + "  Decor: " + deco ); y += 18;
		aquariumGump.AddText( 18, y, 0x480, "Food State: " + fSt + "  Water State: " + wSt ); y += 18;
	}

	aquariumGump.Send( pUser.socket );
	aquariumGump.Free();
}


/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSocket, buttonID )
{
	var pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var serial = Number( pUser.GetTag( "aqua_serial" ));
	var aquarium = CalcItemFromSer( serial );
	if( !ValidateObject( aquarium ))
		return;

	var items = sanitizeAquaList( aquarium );
	var count = items.length;
	var cur = Number( pUser.GetTempTag( "aqua_page" ));
	if( cur <= 0 ) 
		cur = 1;
	if( cur > count ) 
		cur = count;

	// Remove current pages item
	if( buttonID >= ( 1000 +1 ) && buttonID <= ( 1000 + count ))
	{
		var idx = ( buttonID - 1000 ) - 1;
		var aquariumItem = items[idx];
		if( ValidateObject( aquariumItem ))
		{
			var isFish = aquariumItem.GetTag( "isAquariumFish" );
			var isDead = aquariumItem.GetTag( "dead" );
			if( isFish === 1 )
			{
				if( isDead === 1 )
				{
					aquarium.SetTag( "aqua_dead", Math.max( 0, ( Number( aquarium.GetTag( "aqua_dead" ))) - 1 ));
				}
				else     
					aquarium.SetTag( "aqua_live", Math.max( 0, ( Number( aquarium.GetTag( "aqua_live" ))) - 1 ));
			}
			else
			{
				aquarium.SetTag( "aqua_decor", Math.max( 0, ( Number( aquarium.GetTag( "aqua_decor" ))) - 1 ));
			}
			aquariumItem.Delete();
		}
		items = sanitizeAquaList( aquarium );
		var newCount = items.length;
		var newPage = Math.min( Math.max( 1, idx + 1 ), Math.max( 1, newCount ));
		pUser.SetTempTag( "aqua_page", newPage );
		openAquaGump( pUser, aquarium );
		return;
	}

	// Navigation ( action buttons )
	if( buttonID === 90001 )
	{
		if( cur < count )
			pUser.SetTempTag( "aqua_page", cur + 1 );
		openAquaGump( pUser, aquarium );
		return;
	}

	if( buttonID === 90002 )
	{
		if( cur > 1 )
			pUser.SetTempTag( "aqua_page", cur - 1 );
		openAquaGump( pUser, aquarium );
		return;
	}

	// Claim, etc., if you use them:
	if( buttonID === 8001 )
	{
		giveReward( pUser, aquarium );
		openAquaGump( pUser, aquarium );
		return;
	}
}

/** @type { ( tSock: Socket, baseObj: BaseObject ) => boolean } */
function onContextMenuRequest( socket, targObj )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return false;

	var list = [];
	list.push( { id: 0x1001, text: 6235, flags: 0x0020, hue: 0x03E0 } ); // Examine Aquarium
	list.push( { id: 0x1002, text: 6239, flags: 0x0020, hue: 0x03E0 } ); // View Events
	list.push( { id: 0x1003, text: 6237, flags: 0x0020, hue: 0x03E0 } ); // Collect Reward

	if( pUser.isGM === true  )
	{
		list.push( { id: 0x2001, text: 6234, flags: 0x0020, hue: 0x03E0 } ); // GM Open
		list.push( { id: 0x2002, text: 6232, flags: 0x0020, hue: 0x03E0 } ); // GM Add Water
		list.push( { id: 0x2003, text: 6236, flags: 0x0020, hue: 0x03E0 } ); // GM Fill Food+Water
		list.push( { id: 0x2004, text: 6233, flags: 0x0020, hue: 0x03E0 } ); // GM Force Evaluate
	}

	var useKR = false; // 2D so hue works
	TriggerEvent( 18001, "modifyContextMenu", socket, targObj, list, useKR );
	return false;
}

/** @type { ( tSock: Socket, baseObj: BaseObject, popupEntry: number ) => boolean } */
function onContextMenuSelect( socket, targObj, popupEntry )
{
	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return false;

	pUser.SetTag( "aqua_serial", targObj.serial );

	switch(  popupEntry  )
	{
		case 0x1001:
			openAquaGump( pUser, targObj );
			break;

		case 0x1002:
			showEvents( pUser, targObj );
			break;

		case 0x1003:
			giveReward( pUser, targObj );
			openAquaGump( pUser, targObj );
			break;

		case 0x2001:
			openAquaGump( pUser, targObj );
			break;

		case 0x2002:
		{
			var waterAdd = Number( targObj.GetTag( "aqua_water_added" ));
			targObj.SetTag( "aqua_water_added", waterAdd + 1 );
			socket.SysMessage( "Added 1 water unit." );
			break;
		}

		case 0x2003:
		{
			var fishMaint = targObj.GetTag( "aqua_food_maint" );
			var waterMaint = targObj.GetTag( "aqua_water_maint" );
			targObj.SetTag( "aqua_food_added", fishMaint );
			targObj.SetTag( "aqua_water_added", waterMaint );
			socket.SysMessage( "Filled food and water to maintenance." );
			break;
		}

		case 0x2004:
			doEvaluate( targObj );
			openAquaGump( pUser, targObj );
			break;
	}
	return false;
}

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ))
		return false;

	pUser.SetTag( "aqua_serial", iUsed.serial );
	openAquaGump( pUser, iUsed );
	return false;
}

/** @type { ( item: Item, dropper: Character, dest: Item ) => number } */
function onDropItemOnItem( iDropped, cDropper, iDroppedOn )
{
	if( !ValidateObject( iDroppedOn ))
		return 0;

	// --- Vacation wafer / sphere (adds 7 days per item) ---
    var isVac = Number( iDropped.GetTag( "isAquariumVac" ));
    if( isVac === 1 )
	{
        var addDays = Math.max( 1, ( iDropped.amount ) || 1 ) * 7; // support stacks
        var curVac  = Number( iDroppedOn.GetTag( "aqua_vacDays" ));

        iDroppedOn.SetTag( "aqua_vacDays", curVac + addDays );
        cDropper.SysMessage( "You add vacation days to the aquarium (" + addDays + " days).");
        cDropper.SoundEffect(37, true); 
        iDroppedOn.Refresh();
        iDropped.Delete();
        return 2;
    }

	// Food
	var isFood = iDropped.GetTag( "isAquariumFood" );
	if( isFood === 1 )
	{
		var setFood = Number( iDroppedOn.GetTag( "aqua_food_added" ));
		iDroppedOn.SetTag( "aqua_food_added", setFood + 1 );
		cDropper.SysMessage( "You add food to the aquarium." );
		iDroppedOn.Refresh();

		// append to paged list
		var li = sanitizeAquaList( iDroppedOn ); // food is not displayed; do not add
		return 2;
	}

	// Water
	var isLiquidContainer = iDropped.GetTag("ContentsType"); // 2==water
	var usesLeft = iDropped.usesLeft;
	if( isLiquidContainer === "water" )
	{
		// This is a water container; treat the drop as a pour action, not a container move
		var waterAdd = Number( iDroppedOn.GetTag( "aqua_water_added" ));
		iDroppedOn.SetTag( "aqua_water_added", waterAdd + usesLeft );
		// consume one use
		iDropped.usesLeft = Math.max( 0, usesLeft - 5 );

		// flip art when empty if your helper exists (safe no-op if not)
		if( iDropped.usesLeft === 0 )
		{
			iDropped.SetTag( "ContentsType", 1 );
			iDropped.SetTag( "ContentsName", "nothing" );

			// caller context isnt guaranteed to have a socket; just pass null/undefined
			TriggerEvent( 2100, "switchPitcherID", null, iDropped );
		}
		cDropper.SoundEffect( 37, true );
		cDropper.SysMessage( "You pour water into the aquarium." );
		iDroppedOn.Refresh();

		return 0; // handled; do NOT move the pitcher into the tank
	}

	var total = totalItems( iDroppedOn );
	var maxTotal = Number( iDroppedOn.GetTag( "aqua_maxItems" ));
	if( maxTotal <= 0 )
		maxTotal = 30;

	// Fish
	var isFish = Number( iDropped.GetTag( "isAquariumFish" ));
	var isDead = Number( iDropped.GetTag( "dead" ));
	if(  isFish === 1 && isDead === 0  )
	{
		var live = Number( iDroppedOn.GetTag( "aqua_live" ));
		var liveCap = maxLiveCreatures( iDroppedOn );

		if(  total >= maxTotal || live >= liveCap  )
		{
			cDropper.SysMessage( "The aquarium can not hold the creature." );
			return 0;
		}

		stopFishAirTimer( iDropped );

		iDropped.container = iDroppedOn;
		iDroppedOn.SetTag( "aqua_live", live + 1 );

		// add to paged list
		var items = sanitizeAquaList( iDroppedOn );
		items.push( iDropped );
		iDroppedOn.Refresh();
		writeAquaList( iDroppedOn, items );

		cDropper.SysMessage( "You add the creature to your aquarium." );
		return 2;
	}

	// Decoration
	var isDecor = Number( iDropped.GetTag( "isAquariumDecor" ));
	if( isDecor === 1 )
	{
		if( total >= maxTotal )
		{
			cDropper.SysMessage( "The aquarium is full." );
			return 0;
		}

		iDropped.container = iDroppedOn;
		var dec = Number( iDroppedOn.GetTag( "aqua_decor" ));
		iDroppedOn.SetTag( "aqua_decor", dec + 1 );

		// add to paged list
		var items2 = sanitizeAquaList( iDroppedOn );
		items2.push( iDropped );
		iDroppedOn.Refresh();
		writeAquaList( iDroppedOn, items2 );

		cDropper.SysMessage( "You add the decoration to your aquarium." );
		return 2;
	}

	return 1;
}

/** @type { ( thingCreated: BaseObject, thingType: 0 | 1 ) => void } */
function onCreateDFN( objMade, objType )
{
	if( objType == 0 )
		initAquarium( objMade );
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( aquarium, timerID )
{
	if( timerID !== 1 )
		return;

	doEvaluate( aquarium );
	aquarium.StartTimer( AQUA_EVAL_MS, 1, true );
}

function Aqua_GetData( tank )
{
	var data = {
		vacDays:   ( tank.GetTag("aqua_vacDays")),
		events:     tank.GetTag("aqua_events") || "",
		reward:    ( tank.GetTag("aqua_reward")),
		live:      ( tank.GetTag("aqua_live")),
		dead:      ( tank.GetTag("aqua_dead")),
		decorTag:  ( tank.GetTag("aqua_decor")),

		fishState: ( tank.GetTag("aqua_food_state")),
		fishAdded: ( tank.GetTag("aqua_food_added")),
		fishMaint: ( tank.GetTag("aqua_food_maint")),
		fishImprv: ( tank.GetTag("aqua_food_impr")),

		waterState:( tank.GetTag("aqua_water_state")),
		waterAdded:( tank.GetTag("aqua_water_added")),
		waterMaint:( tank.GetTag("aqua_water_maint")),
		waterImprv:( tank.GetTag("aqua_water_impr"))
	};

	// Defensive defaults
	if( data.fishImprv <= 0 && data.fishState !== 4 && data.fishState !== 0 )
		data.fishImprv = data.fishMaint + 2;
	if( data.waterImprv <= 0 && data.waterState !== 4 && data.waterState !== 0 )
		data.waterImprv = data.waterMaint + 2;

	return data;
}

function foodStateName( state )
{
	switch( state )
	{
		case 0: return "Dead";
		case 1: return "Starving";
		case 2: return "Hungry";
		case 3: return "Full";
		case 4: return "Overfed";
		default: return "Unknown";
	}
}

function waterStateName( state )
{
	switch( state )
	{
		case 0: return "Dead";
		case 1: return "Dying";
		case 2: return "Unhealthy";
		case 3: return "Healthy";
		case 4: return "Strong";
		default: return "Unknown";
	}
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( myObj, pSocket )
{
		var data = Aqua_GetData( myObj );

		var maxLive = maxLiveCreatures( myObj );
		var totalKnown = totalItems( myObj );
		var computedDec = totalKnown - ( data.live | 0 ) - ( data.dead | 0 );
		if( computedDec < 0 )
			computedDec = 0;

		// Count events safely
		var eventsCount = 0;
		if( data.events && data.events !== 0 )
		{
			var parts = ( 0 + data.events ).split( "," );
			for( var i = 0; i < parts.length; i++ )
			{
				if(( parts[i] | 0 ) !== 0 )
					eventsCount++;
			}
		}

		// Build lines
		var lines = [];

		if( data.vacDays > 0 )
			lines.push( "Vacation days left: " + ( data.vacDays | 0 ));

		if( eventsCount > 0 )
			lines.push( eventsCount + " event( s ) to view!" );

		if( ( data.reward | 0 ) === 1 )
			lines.push( "A reward is available!" );

		lines.push( "Live Creatures: " + ( data.live | 0 ) + " / " + maxLive );
		if( ( data.dead | 0 ) > 0 )
			lines.push( "Dead Creatures: " + ( data.dead | 0 ));

		var showDecor = ( computedDec > 0 ) ? computedDec : ( data.decorTag | 0 );
		if( showDecor > 0 )
			lines.push( "Decorations: " + showDecor );

		lines.push( "Food state: " + foodStateName( data.fishState ));
		lines.push( "Water state: " + waterStateName( data.waterState ));

		if( data.fishState === 0 )
			lines.push( "Food Added: " + ( data.fishAdded | 0 ) + "   Needed: " + ( data.fishImprv | 0 ));
		else if( data.fishState === 4 )
			lines.push( "Food Added: " + ( data.fishAdded | 0 ) + "   Needed: " + ( data.fishMaint | 0 ));
		else
			lines.push( "Food Added: " + ( data.fishAdded | 0 ) + "   Feed: " + ( data.fishMaint | 0 ) + "   Improve: " + ( data.fishImprv | 0 ));

		if( data.waterState === 0 )
			lines.push( "Water Added: " + ( data.waterAdded | 0 ) + "   Needed: " + ( data.waterImprv | 0 ));
		else if( data.waterState === 4 )
			lines.push( "Water Added: " + ( data.waterAdded | 0 ) + "   Needed: " + ( data.waterMaint | 0 ));
		else
			lines.push( "Water Added: " + ( data.waterAdded | 0 ) + "   Maintain: " + ( data.waterMaint | 0 ) + "   Improve: " + ( data.waterImprv | 0 ));

		// Optional: put tooltip after name but before maker mark ( see docs update )
		myObj.SetTempTag( "tooltipSortOrder", 1 );

		// Use a cliloc container if you like
		myObj.SetTempTag( "clilocTooltip", 1042971 );
		var tooltipText = lines.join( "\n" );
		return tooltipText;
}
