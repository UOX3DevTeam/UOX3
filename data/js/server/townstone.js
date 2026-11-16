/// <reference path="../definitions.d.ts" />
// @ts-check

// =============================================================================
// townstone.js
// UOX3 legacy civic townstone gump. This system is independent of Factions.
// =============================================================================

const townstoneContextTag = "civic_townstone_serial";
const townstoneViewTag = "civic_townstone_view";

function TownstoneIsFactionStone( iUsed )
{
	return iUsed.GetTag( "faction_townstone" ) == 1;
}

function TownstoneMemberSerials( townRegion )
{
	const members = String( townRegion.members || "" );
	return members === "" ? [] : members.split( "," );
}

function TownstoneIsMember( townRegion, pUser )
{
	return TownstoneMemberSerials( townRegion ).indexOf( String( pUser.serial ) ) !== -1;
}

function TownstonePlayerTownId( pUser )
{
	return pUser.town == null ? 255 : Number( pUser.town.id );
}

function TownstoneInRegion( townRegion, pUser )
{
	return pUser.region != null && Number( pUser.region.id ) === Number( townRegion.id );
}

function TownstoneFormattedEntry( entryId, pSock, value )
{
	let text = GetDictionaryEntry( entryId, pSock.language );
	if( typeof value !== "undefined" )
		text = text.replace( /%[disu]/, String( value ) );
	return text;
}

function TownstoneHeader( myGump, title, townRegion )
{
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 430, 430, 9200 );
	myGump.AddHTMLGump( 20, 15, 390, 25, 0, 0, "<CENTER><BIG><B>" + title + "</B></BIG></CENTER>" );
	myGump.AddHTMLGump( 25, 52, 380, 22, 0, 0, "<CENTER>" + townRegion.name + " - " + townRegion.owner + "</CENTER>" );
	myGump.AddHTMLGump( 25, 78, 380, 20, 0, 0, "<CENTER>Population: " + townRegion.population + "</CENTER>" );
}

function TownstoneAddAction( myGump, y, buttonId, label )
{
	myGump.AddButton( 30, y, 0xFA5, 1, 0, buttonId );
	myGump.AddHTMLGump( 70, y, 325, 22, 0, 0, label );
}

function TownstoneRegionIds( value )
{
	const regionIds = String( value || "" );
	return regionIds === "" ? [] : regionIds.split( "," );
}

function TownstoneAddRegionList( myGump, townIds, emptyText )
{
	if( townIds.length === 0 )
	{
		myGump.AddHTMLGump( 35, 118, 360, 230, 0, 1, emptyText );
		return;
	}

	let townList = "";
	for( let townIndex = 0; townIndex < townIds.length; townIndex++ )
	{
		const listedTown = GetTownRegion( Number( townIds[townIndex] ) );
		if( listedTown != null )
			townList += listedTown.name + " - " + listedTown.owner + "<BR>";
	}
	myGump.AddHTMLGump( 35, 118, 360, 230, 0, 1, townList );
}

function TownstoneSend( pSock, pUser, iUsed, viewName )
{
	if( !ValidateObject( iUsed ) || iUsed.region == null )
	{
		pSock.SysMessage( "This townstone is not linked to a valid town region." );
		return;
	}

	const townRegion = iUsed.region;
	const myGump = new Gump();
	let y = 112;

	TownstoneHeader( myGump, viewName === "mayor" ? "Mayor Controls" : "Civic Townstone", townRegion );

	if( viewName === "taxes" )
	{
		myGump.AddHTMLGump( 35, y, 360, 25, 0, 0, "Town Tax" );
		y += 34;
		myGump.AddPicture( 35, y, townRegion.taxResource );
		myGump.AddHTMLGump( 80, y, 300, 44, 0, 0, townRegion.tax + " units of resource 0x" + Number( townRegion.taxResource ).toString( 16 ) + " are collected from each citizen per tax cycle." );
		y += 65;
		TownstoneAddAction( myGump, y, 40, "Return to Main Menu" );
	}
	else if( viewName === "budget" )
	{
		myGump.AddHTMLGump( 35, y, 360, 24, 0, 0, TownstoneFormattedEntry( 1162, pSock ) + ": " + townRegion.guardsPurchased );
		y += 30;
		myGump.AddHTMLGump( 35, y, 360, 24, 0, 0, TownstoneFormattedEntry( 1163, pSock ) + ": " + townRegion.numGuards );
		y += 30;
		myGump.AddHTMLGump( 35, y, 360, 24, 0, 0, TownstoneFormattedEntry( 1164, pSock ) + ": " + ( townRegion.numGuards * 20 ) );
		y += 30;
		myGump.AddHTMLGump( 35, y, 360, 24, 0, 0, "Treasury Reserves: " + townRegion.reserves );
		y += 42;
		TownstoneAddAction( myGump, y, 40, "Return to Main Menu" );
	}
	else if( viewName === "members" )
	{
		const memberSerials = TownstoneMemberSerials( townRegion );
		let memberList = "";
		for( let memberIndex = 0; memberIndex < memberSerials.length; memberIndex++ )
		{
			const member = CalcCharFromSer( Number( memberSerials[memberIndex] ) );
			if( ValidateObject( member ) )
				memberList += member.name + ( pUser.isGM ? " (" + member.serial + ")" : "" ) + "<BR>";
		}
		myGump.AddHTMLGump( 35, y, 360, 230, 0, 1, memberList );
		TownstoneAddAction( myGump, 360, 40, "Return to Mayor Controls" );
	}
	else if( viewName === "allies" )
	{
		TownstoneAddRegionList( myGump, TownstoneRegionIds( townRegion.alliedTowns ), "This town has no allied towns." );
		TownstoneAddAction( myGump, 360, 40, "Return to Main Menu" );
	}
	else if( viewName === "enemies" )
	{
		TownstoneAddRegionList( myGump, TownstoneRegionIds( townRegion.enemyTowns ), "This town has no enemy towns." );
		TownstoneAddAction( myGump, 360, 40, "Return to Main Menu" );
	}
	else if( viewName === "enemy" )
	{
		TownstoneAddAction( myGump, y, 61, "Seize Townstone" );
		y += 28;
		TownstoneAddAction( myGump, y, 62, "Attack Townstone" );
	}
	else if( viewName === "outsider" )
	{
		TownstoneAddAction( myGump, y, 41, TownstoneFormattedEntry( 1128, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 3, TownstoneFormattedEntry( 1129, pSock ) );
	}
	else if( viewName === "visitor" )
	{
		myGump.AddHTMLGump( 35, y, 360, 80, 0, 0, "You are already a citizen of another town. You may inspect this town, but you cannot join it." );
		y += 85;
		TownstoneAddAction( myGump, y, 3, TownstoneFormattedEntry( 1129, pSock ) );
	}
	else if( viewName === "mayor" )
	{
		TownstoneAddAction( myGump, y, 21, TownstoneFormattedEntry( 1131, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 22, TownstoneFormattedEntry( 1132, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 23, TownstoneFormattedEntry( 1133, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 24, TownstoneFormattedEntry( 1134, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 25, TownstoneFormattedEntry( 1135, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 26, TownstoneFormattedEntry( 1138, pSock ) );
		y += 34;
		myGump.AddHTMLGump( 30, y, 365, 22, 0, 0, TownstoneFormattedEntry( 1137, pSock, townRegion.reserves ) );
		y += 30;
		TownstoneAddAction( myGump, y, 40, "Return to Main Menu" );
	}
	else
	{
		TownstoneAddAction( myGump, y, 2, TownstoneFormattedEntry( 1140, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 3, TownstoneFormattedEntry( 1141, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 4, "Toggle Town Title" );
		y += 28;
		TownstoneAddAction( myGump, y, 5, TownstoneFormattedEntry( 1143, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 6, TownstoneFormattedEntry( 1144, pSock ) );
		myGump.AddPicture( 350, y, townRegion.taxResource );
		y += 28;
		TownstoneAddAction( myGump, y, 7, TownstoneFormattedEntry( 1145, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 8, TownstoneFormattedEntry( 1146, pSock ) );
		y += 28;
		TownstoneAddAction( myGump, y, 9, TownstoneFormattedEntry( 1147, pSock ) );

		if( pUser.townPriv == 2 || pUser.isGM )
		{
			y += 34;
			TownstoneAddAction( myGump, y, 20, TownstoneFormattedEntry( 1148, pSock ) );
		}
	}

	myGump.AddButton( 30, 395, 0xFA5, 1, 0, 0 );
	myGump.AddHTMLGump( 70, 395, 100, 22, 0, 0, "Close" );
	pUser.SetTempTag( townstoneContextTag, iUsed.serial );
	pUser.SetTempTag( townstoneViewTag, viewName );
	myGump.Send( pSock );
	myGump.Free();
}

function TownstoneOpen( pSock, pUser, iUsed )
{
	const townRegion = iUsed.region;
	if( townRegion == null )
	{
		pSock.SysMessage( "This townstone is not linked to a valid town region." );
		return;
	}

	if( TownstoneIsMember( townRegion, pUser ) )
	{
		TownstoneSend( pSock, pUser, iUsed, "member" );
		return;
	}

	const playerTownId = TownstonePlayerTownId( pUser );
	const playerRaceId = pUser.race == null ? 0 : pUser.race.id;
	if( RaceCompareByRace( playerRaceId, townRegion.race ) <= -1 )
	{
		if( playerTownId === 255 )
			pSock.SysMessage( 1125 );
		else
			TownstoneSend( pSock, pUser, iUsed, "enemy" );
		return;
	}

	if( playerTownId !== 255 )
	{
		if( playerTownId === Number( townRegion.id ) || playerTownId === 0 )
		{
			townRegion.TownstoneAction( pSock, 41, iUsed.serial );
			TownstoneSend( pSock, pUser, iUsed, "member" );
		}
		else
		{
			TownstoneSend( pSock, pUser, iUsed, "visitor" );
		}
		return;
	}

	TownstoneSend( pSock, pUser, iUsed, "outsider" );
}

/** @type { ( user: Character, used: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	if( !ValidateObject( pUser ) || !ValidateObject( iUsed ) )
		return false;
	if( TownstoneIsFactionStone( iUsed ) )
		return true;
	if( !pUser.InRange( iUsed, 3 ))
	{
		pUser.SysMessage( GetDictionaryEntry( 393, pUser.socket.language ) );
		return false;
	}

	TownstoneOpen( pUser.socket, pUser, iUsed );
	return false;
}

function onGumpPress( pSock, pButton, gumpData )
{
	const pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) || pButton === 0 )
		return;

	const iUsed = CalcItemFromSer( Number( pUser.GetTempTag( townstoneContextTag ) ) );
	if( !ValidateObject( iUsed ) || TownstoneIsFactionStone( iUsed ) || iUsed.region == null )
		return;
	if( !pUser.InRange( iUsed, 3 ) || !TownstoneInRegion( iUsed.region, pUser ) )
	{
		pSock.SysMessage( "You must remain near this townstone to use it." );
		return;
	}

	const townRegion = iUsed.region;
	const isMember = TownstoneIsMember( townRegion, pUser );
	const viewName = String( pUser.GetTempTag( townstoneViewTag ) );

	if( pButton === 20 )
	{
		if( isMember && ( pUser.townPriv == 2 || pUser.isGM ))
			TownstoneSend( pSock, pUser, iUsed, "mayor" );
		return;
	}
	if( pButton === 40 )
	{
		if( viewName === "members" && isMember && ( pUser.townPriv == 2 || pUser.isGM ))
			TownstoneSend( pSock, pUser, iUsed, "mayor" );
		else
			TownstoneOpen( pSock, pUser, iUsed );
		return;
	}

	const memberAction = pButton >= 2 && pButton <= 9;
	const mayorAction = pButton >= 21 && pButton <= 26;
	const enemyAction = pButton === 61 || pButton === 62;
	if( memberAction && !isMember )
		return;
	if( mayorAction && !( isMember && ( pUser.townPriv == 2 || pUser.isGM )))
		return;
	if( pButton === 41 && viewName !== "outsider" )
		return;
	if( pButton === 3 && !isMember && viewName !== "outsider" && viewName !== "visitor" )
		return;
	if( enemyAction && viewName !== "enemy" )
		return;
	if( pButton === 3 )
	{
		TownstoneSend( pSock, pUser, iUsed, "taxes" );
		return;
	}
	if( pButton === 7 )
	{
		TownstoneSend( pSock, pUser, iUsed, "budget" );
		return;
	}
	if( pButton === 8 )
	{
		TownstoneSend( pSock, pUser, iUsed, "allies" );
		return;
	}
	if( pButton === 9 )
	{
		TownstoneSend( pSock, pUser, iUsed, "enemies" );
		return;
	}
	if( pButton === 22 )
	{
		TownstoneSend( pSock, pUser, iUsed, "members" );
		return;
	}

	townRegion.TownstoneAction( pSock, pButton, iUsed.serial );
	if( pButton === 4 || pButton === 41 )
		TownstoneOpen( pSock, pUser, iUsed );
}
