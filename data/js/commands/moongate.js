// This command is used to enable/disable spawn regions in specific facets
const facetList = [ "felucca", "trammel", "ilshenar", "malas", "tokuno", "termur" ];

function CommandRegistration()
{
	RegisterCommand( "enablemoongates", 3, true );
	RegisterCommand( "disablemoongates", 3, true );
}

function command_ENABLEMOONGATES( socket, cmdString )
{
	var pUser = socket.currentChar;
	var facetNum = parseInt( cmdString );

	if( !isNaN( facetNum ) && facetNum >= 0 )
	{
		SetMoongateFacetStatus( facetNum, true );
		socket.SysMessage( "Moongates have been enabled for facet " + facetNum + " (" + facetList[facetNum] + ")" );
	}
	else
	{
		var cmdStringTemp = ( cmdString.replace( / /g, "" ));
		var facetListIndex = facetList.indexOf( cmdStringTemp.toLowerCase() );
		if( facetListIndex != -1 )
		{
			SetMoongateFacetStatus( facetListIndex, true );
			socket.SysMessage( "Moongates have been enabled for facet " + facetListIndex + " (" + cmdString + ")" );
		}
	}
	ReloadJSFile( 5011 );// reloads items/moongate.js
}

function command_DISABLEMOONGATES( socket, cmdString )
{
	var pUser = socket.currentChar;
	var facetNum = parseInt( cmdString );

	if( !isNaN( facetNum ) && facetNum >= 0 )
	{
		SetMoongateFacetStatus( facetNum, false );
		socket.SysMessage( "Moongates have been disabled for facet " + facetNum + " (" + facetList[facetNum] + ")" );
	}
	else
	{
		var cmdStringTemp = ( cmdString.replace( / /g, "" ));
		var facetListIndex = facetList.indexOf( cmdStringTemp.toLowerCase() );
		if( facetListIndex != -1 )
		{
			SetMoongateFacetStatus( facetListIndex, true );
			socket.SysMessage( "Moongates have been disabled for facet " + facetListIndex + " (" + cmdString + ")" );
		}
	}
	ReloadJSFile( 5011 );// reloads items/moongate.js
}
