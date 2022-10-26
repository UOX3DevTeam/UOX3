// This command is used to enable/disable spawn regions in specific facets
const facetList = [ "felucca", "trammel", "ilshenar", "malas", "tokuno", "termur" ];

function CommandRegistration()
{
	RegisterCommand( "enablespawns", 3, true );
	RegisterCommand( "disablespawns", 3, true );
}

function command_ENABLESPAWNS( socket, cmdString )
{
	var pUser = socket.currentChar;
	var facetNum = parseInt( cmdString );

	if( !isNaN( facetNum ) && facetNum >= 0 )
	{
		SetSpawnRegionFacetStatus( facetNum, true );
		socket.SysMessage( "Spawn Regions have been enabled for facet " + facetNum + " (" + facetList[facetNum] + ")" );
	}
	else
	{
		var cmdStringTemp = ( cmdString.replace( / /g, "" ));
		var facetListIndex = facetList.indexOf( cmdStringTemp.toLowerCase() );
		if( facetListIndex != -1 )
		{
			SetSpawnRegionFacetStatus( facetListIndex, true );
			socket.SysMessage( "Spawn Regions have been enabled for facet " + facetListIndex + " (" + cmdString + ")" );
		}
	}
}

function command_DISABLESPAWNS( socket, cmdString )
{
	var pUser = socket.currentChar;
	var facetNum = parseInt( cmdString );

	if( !isNaN( facetNum ) && facetNum >= 0 )
	{
		SetSpawnRegionFacetStatus( facetNum, false );
		socket.SysMessage( "Spawn Regions have been disabled for facet " + facetNum + " (" + facetList[facetNum] + ")" );
	}
	else
	{
		var cmdStringTemp = ( cmdString.replace( / /g, "" ));
		var facetListIndex = facetList.indexOf( cmdStringTemp.toLowerCase() );
		if( facetListIndex != -1 )
		{
			SetSpawnRegionFacetStatus( facetListIndex, true );
			socket.SysMessage( "Spawn Regions have been disabled for facet " + facetListIndex + " (" + cmdString + ")" );
		}
	}
}
