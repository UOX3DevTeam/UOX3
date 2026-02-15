/// <reference path="../../definitions.d.ts" />
// @ts-check
// Description: Gumps for virtue system
// virtue0 = Humility
// virtue1 = Sacrifice
// virtue2 = Compassion
// virtue3 = Spirituality
// virtue4 = Valor
// virtue5 = Honor
// virtue6 = Justice
// virtue7 = Honesty
//
// Virtue levels by value:
// 0 - 3999      -> None
// 4000 - 9999   -> Seeker
// 10000 - 19999 -> Follower
// Max value     -> Knight
//
// Max values per virtue:
// Humility, Compassion, Spirituality, Valor, Justice, Honesty -> 21000
// Sacrifice -> 22000
// Honor     -> 20000

const compassionVirtueEnabled = GetServerSetting( "CompassionVirtueEnabled" );
const honestyVirtueEnabled = GetServerSetting( "HonestyVirtueEnabled" );
const humilityVirtueEnabled = GetServerSetting( "HumilityVirtueEnabled" );
const sacrificeVirtueEnabled = GetServerSetting( "SacrificeVirtueEnabled" );

/** @type { ( pUser: Character, tChar: Character ) => void } */
function VirtueGump( pUser, tChar )
{
	if( !ValidateObject( pUser ) || !ValidateObject( tChar ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	var isOwner = ( pUser.serial === tChar.serial );

	var virtueGump = new Gump();

	virtueGump.AddPage( 0 );
	virtueGump.AddGump( 30, 40, 104 );
	virtueGump.AddPage( 1 );

	// Humility ( index 0 )
	var humHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 0 );
	var humTipID = Virtue_GetTooltipID( tChar, 0 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 61, 71, 108, 108, 1, 0, 11, 0x1018, humHue, 61, 71 );
	}
	virtueGump.AddGumpColor( 61, 71, 108, humHue );
	virtueGump.AddToolTip( humTipID );

	// Sacrifice ( index 1 )
	var sacHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 1 );
	var sacTipID = Virtue_GetTooltipID( tChar, 1 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 35, 135, 110, 110, 1, 0, 12, 0x1018, sacHue, 35, 135 );
	}
	virtueGump.AddGumpColor( 35, 135, 110, sacHue );
	virtueGump.AddToolTip( sacTipID );

	// Compassion ( index 2 )
	var compHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 2 );
	var compTipID = Virtue_GetTooltipID( tChar, 2 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 211, 133, 105, 105, 1, 0, 13, 0x1018, compHue, 211, 133 );
	}
	virtueGump.AddGumpColor( 211, 133, 105, compHue );
	virtueGump.AddToolTip( compTipID );

	// Spirituality ( index 3 )
	var spirHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 3 );
	var spirTipID = Virtue_GetTooltipID( tChar, 3 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 61, 195, 111, 111, 1, 0, 14, 0x1018, spirHue, 61, 195 );
	}
	virtueGump.AddGumpColor( 61, 195, 111, spirHue );
	virtueGump.AddToolTip( spirTipID );

	// Valor ( index 4 )
	var valHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 4 );
	var valTipID = Virtue_GetTooltipID( tChar, 4 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 123, 46, 112, 112, 1, 0, 15, 0x1018, valHue, 123, 46 );
	}
	virtueGump.AddGumpColor( 123, 46, 112, valHue );
	virtueGump.AddToolTip( valTipID );

	// Honor ( index 5 )
	var honHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 5 );
	var honTipID = Virtue_GetTooltipID( tChar, 5 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 187, 70, 107, 107, 1, 0, 16, 0x1018, honHue, 187, 70 );
	}
	virtueGump.AddGumpColor( 187, 70, 107, honHue );
	virtueGump.AddToolTip( honTipID );

	// Justice ( index 6 )
	var jusHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 6 );
	var jusTipID = Virtue_GetTooltipID( tChar, 6 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 186, 195, 109, 109, 1, 0, 17, 0x1018, jusHue, 186, 195 );
	}
	virtueGump.AddGumpColor( 186, 195, 109, jusHue );
	virtueGump.AddToolTip( jusTipID );

	// Honesty ( index 7 )
	var honstHue   = TriggerEvent( 8003, "Virtue_GetHue", tChar, 7 );
	var honstTipID = Virtue_GetTooltipID( tChar, 7 );
	if( isOwner )
	{
		virtueGump.AddButtonTileArt( 121, 221, 106, 106, 1, 0, 18, 0x1018, honstHue, 121, 221 );
	}
	virtueGump.AddGumpColor( 121, 221, 106, honstHue );
	virtueGump.AddToolTip( honstTipID );

	// Only show status / close buttons when viewing self
	if( isOwner )
	{
		virtueGump.AddButton( 57, 269, 2027, 1, 0, 100 ); // status
		virtueGump.AddButton( 186, 269, 2071, 1, 0, 104 ); // close
	}

	virtueGump.Send( pSocket );
	virtueGump.Free();
}

/** @type { ( pUser: Character, tChar: Character ) => void } */
function VirtueGumpStatus( pUser, tChar )
{
	if( !ValidateObject( pUser ) || !ValidateObject( tChar ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	var virtueGumpStatus = new Gump();
	virtueGumpStatus.AddPage( 0 );

	virtueGumpStatus.AddGump(30, 40, 2080);
	virtueGumpStatus.AddGump(47, 77, 2081);
	virtueGumpStatus.AddGump(47, 147, 2081);
	virtueGumpStatus.AddGump(47, 217, 2081);
	virtueGumpStatus.AddGump(47, 267, 2083);
	virtueGumpStatus.AddGump(70, 213, 2091);

	virtueGumpStatus.AddPage(1);

	virtueGumpStatus.AddXMFHTMLGump(140, 73, 200, 20, 1077972, false, false); // The Virtues

	virtueGumpStatus.AddXMFHTMLGump(80, 100, 100, 40, 1051000, false, false); // Humility
	virtueGumpStatus.AddXMFHTMLGump(80, 129, 100, 40, 1051001, false, false); // Sacrifice
	virtueGumpStatus.AddXMFHTMLGump(80, 159, 100, 40, 1051002, false, false); // Compassion
	virtueGumpStatus.AddXMFHTMLGump(80, 189, 100, 40, 1051003, false, false); // Spirituality
	virtueGumpStatus.AddXMFHTMLGump(200, 100, 200, 40, 1051004, false, false); // Valor
	virtueGumpStatus.AddXMFHTMLGump(200, 129, 200, 40, 1051005, false, false); // Honor
	virtueGumpStatus.AddXMFHTMLGump(200, 159, 200, 40, 1051006, false, false); // Justice
	virtueGumpStatus.AddXMFHTMLGump(200, 189, 200, 40, 1051007, false, false); // Honesty
	virtueGumpStatus.AddXMFHTMLGump(75, 224, 220, 60, 1052062, false, false); // Click on a blue gem to view your status in that virtue.

	virtueGumpStatus.AddButton(60, 100, 1210, 1210, 1, 0, 1);
	virtueGumpStatus.AddButton(60, 129, 1210, 1210, 1, 0, 2);
	virtueGumpStatus.AddButton(60, 159, 1210, 1210, 1, 0, 3);
	virtueGumpStatus.AddButton(60, 189, 1210, 1210, 1, 0, 4);
	virtueGumpStatus.AddButton(180, 100, 1210, 1210, 1, 0, 5);
	virtueGumpStatus.AddButton(180, 129, 1210, 1210, 1, 0, 6);
	virtueGumpStatus.AddButton(180, 159, 1210, 1210, 1, 0, 7);
	virtueGumpStatus.AddButton(180, 189, 1210, 1210, 1, 0, 8);
	virtueGumpStatus.AddButton(280, 43, 4014, 4014, 1, 0, 101);

	virtueGumpStatus.Send( pSocket );
	virtueGumpStatus.Free();
}

/** @type { ( pUser: Character, virtueIndex: number, descriptionID: number, webPage: string ) => void } */
function ShowVirtueInfoGump( pUser, virtueIndex, descriptionID, webPage )
{
	if( !ValidateObject( pUser ))
		return;

	var pSocket = pUser.socket;
	if( pSocket == null)
		return;

	if( virtueIndex < 0 )
	{
		virtueIndex = 0;
	}

	if( virtueIndex > 7 )
	{
		virtueIndex = 7;
	}

	var value = TriggerEvent( 8003, "Virtue_GetValue", pUser, virtueIndex );
	var maxValue = TriggerEvent( 8003, "Virtue_GetMaxAmount", virtueIndex );

	// Compute number of filled dots (0..10), matching RunUO logic
	var dots = 0;

	if( value < 4000 )
	{
		dots = Math.floor( value / 400 );
	}
	else if( value < 10000 )
	{
		dots = Math.floor(( value - 4000 ) / 600 );
	}
	else if( value < maxValue )
	{
		var denom = ( maxValue - 10000 ) / 10;
		if( denom <= 0 )
			dots = 10;
		else
			dots = Math.floor(( value - 10000 ) / denom );
	}
	else
	{
		dots = 10;
	}

	if( dots < 0 )
		dots = 0;

	if( dots > 10 )
		dots = 10;

	// Value description ID (cliloc), same logic as RunUO
	var valueDescID;

	if( value < 1 )
		valueDescID = 1052044; // You have not started on the path of this Virtue.
	else if( value < 400 )
		valueDescID = 1052045; // You have barely begun your journey...
	else if( value < 2000 )
		valueDescID = 1052046; // You have progressed in this Virtue...
	else if( value < 3600 )
		valueDescID = 1052047; // Your journey through the path is going well.
	else if( value < 4000 )
		valueDescID = 1052048; // You feel very close to achieving your next path...
	else if( dots < 1 )
		valueDescID = 1052049; // You have achieved a path in this Virtue.
	else if( dots < 9 )
		valueDescID = 1052047; // Your journey through the path is going well.
	else if( dots < 10 )
		valueDescID = 1052048; // You feel very close to achieving your next path...
	else
		valueDescID = 1052050; // You have achieved the highest path in this Virtue.

	pUser.SetTempTag( "VirtueInfo_VirtueIndex", virtueIndex );
	pUser.SetTempTag( "VirtueInfo_DescID", descriptionID );
	pUser.SetTempTag( "VirtueInfo_WebPage", webPage ? webPage : "" );

	var showVirtueInfoGump = new Gump();

	showVirtueInfoGump.AddPage( 0 );

	// Frame art (same coords and gumps as RunUO)
	showVirtueInfoGump.AddGump( 30, 40, 2080 );
	showVirtueInfoGump.AddGump( 47, 77, 2081 );
	showVirtueInfoGump.AddGump( 47, 147, 2081 );
	showVirtueInfoGump.AddGump( 47, 217, 2081 );
	showVirtueInfoGump.AddGump( 47, 267, 2083 );
	showVirtueInfoGump.AddGump( 70, 213, 2091 );

	showVirtueInfoGump.AddPage( 1 );

	// Progress dots at top
	for( var i = 0; i < 10; i++ )
	{
		var x = 95 + ( i * 17 );
		var gumpID = ( i < dots ) ? 2362 : 2360; // Filled vs empty dot art
		showVirtueInfoGump.AddGump( x, 50, gumpID );
	}

	// Virtue name line: 1051000 + virtueIndex (same as RunUO)
	// AddXMFHTMLGump( x, y, width, height, clilocID, background, scrollbar )
	showVirtueInfoGump.AddXMFHTMLGump( 157, 73, 200, 40, 1051000 + virtueIndex, false, false );

	// Main description text (passed in as descriptionID)
	showVirtueInfoGump.AddXMFHTMLGump( 75, 95, 220, 140, descriptionID, false, false );

	// Value description ("You have not started", "Your journey...", etc.)
	showVirtueInfoGump.AddXMFHTMLGump( 70, 224, 229, 60, valueDescID, false, false );

	// Button 2: "learn more" / web link (top right)
	showVirtueInfoGump.AddButton( 65, 277, 1209, 1, 0, 102 );

	// Button 1: back (bottom left)
	showVirtueInfoGump.AddButton( 280, 43, 4014, 1, 0, 103 );

	// Bottom line: if no webPage, "This virtue is not yet defined."
	// else "-click to learn more (opens webpage)"
	var hasWeb = webPage && webPage.length > 0;
	var bottomID = hasWeb ? 1052052 : 1052055;

	showVirtueInfoGump.AddXMFHTMLGump( 83, 275, 400, 40, bottomID, false, false );

	showVirtueInfoGump.Send( pSocket );
	showVirtueInfoGump.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( socket, buttonID, gumpID )
{
	if( socket == null )
		return;

	var pUser = socket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var tChar = pUser;

	switch( buttonID )
	{
		case 1: ShowVirtueInfoGump( pUser, 0, 1052051, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#humility" );break;
		case 2: ShowVirtueInfoGump( pUser, 1, 1052053, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#sacrifice"); break;
		case 3: ShowVirtueInfoGump( pUser, 2, 1052054, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#compassion"); break;
		case 4: ShowVirtueInfoGump( pUser, 3, 1052056, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#spirituality"); break;
		case 5: ShowVirtueInfoGump( pUser, 4, 1052057, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#valor"); break;
		case 6: ShowVirtueInfoGump( pUser, 5, 1052058, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#honor"); break;
		case 7: ShowVirtueInfoGump( pUser, 6, 1052059, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#justice"); break;
		case 8: ShowVirtueInfoGump( pUser, 7, 1052060, "http://uo.com/wiki/ultima-online-wiki/gameplay/npc-commercial-transactions/the-virtues/#honesty"); break;
		case 11:
			if( humilityVirtueEnabled )
			{
				TriggerEvent( 8005, "Humility_StartPetTarget", pUser );
				VirtueGump( pUser, tChar );
			}
			break;
		case 12:
			if( sacrificeVirtueEnabled )
			{
				TriggerEvent( 8008, "Sacrifice_OnVirtueUsed", pUser );
				VirtueGump( pUser, tChar );
			}
			break;
		case 13: pUser.SysMessage("This virtue is not activated through the virtue menu."); VirtueGump( pUser, tChar ); break;
		case 14: pUser.SysMessage("Not In"); VirtueGump( pUser, tChar ); break;
		case 15: pUser.SysMessage("Not In"); VirtueGump( pUser, tChar ); break;
		case 16: pUser.SysMessage("Not In"); VirtueGump( pUser, tChar ); break;
		case 17: pUser.SysMessage("Not In"); VirtueGump( pUser, tChar ); break;
		case 18: pUser.SysMessage("This virtue is not activated through the virtue menu."); VirtueGump( pUser, tChar ); break;
	}

	// Main Virtue gump buttons
	if( buttonID === 100 )
	{
		if( pUser.serial === tChar.serial )
		{
			VirtueGumpStatus(pUser, tChar)
		}

		return;
	}

	if( buttonID === 101 )
	{
		VirtueGump( pUser, tChar );
		return;
	}

	if( buttonID === 102 )
	{
		var virtueIndex = pUser.GetTempTag( "VirtueInfo_VirtueIndex" );
		var virtueDescID = pUser.GetTempTag( "VirtueInfo_DescID" );
		var webURL = pUser.GetTempTag( "VirtueInfo_WebPage" )
		socket.OpenURL( webURL.toString() );
		ShowVirtueInfoGump( pUser, virtueIndex, virtueDescID, webURL.toString() );
	}

	if( buttonID === 103 )
	{
		if( pUser.serial === tChar.serial )
		{
			VirtueGumpStatus( pUser, tChar )
		}
		return;
	}

	// Status gump: any button closes it, nothing else
	if( buttonID === 104 )
	{
		// Status gump: any button closes it, nothing else
		return;
	}

	// Not our gump
	return;
}

/** @type { ( tChar: Character, virtueIndex: number ) => number } */
function Virtue_GetTooltipID( tChar, virtueIndex )
{
	// level 0..3 from helper script
	var level = TriggerEvent( 8003, "Virtue_GetLevel", tChar, virtueIndex );
	if( level < 0 )
		level = 0;
	if( level > 3 )
		level = 3;

	var baseID = 1051000 + virtueIndex; // 0=Humility, 1=Sacrifice, etc
	return baseID + ( level * 8 );
}