function onUseChecked( pUser, iUsed )
{
	const socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		const typeID = iUsed.GetTag( "typeID" );
		const elixirFunctions = {
			1: elixirOfMetal,
			2: elixirOfAgapite,
			3: elixirOfGold,
			4: elixirOfValorite,
			5: elixirOfVerite
		};

		if( elixirFunctions[typeID] )
		{
			elixirFunctions[typeID]( pUser, iUsed );
		}
	}
	return false;
}

function elixirConversion( pUser, iUsed, ingotName, ingotColor, requiredResourceType ) 
{
	const socket = pUser.socket;
	const ingotsInPack = pUser.ResourceCount( 0x1bef, requiredResourceType );
	const ingotsInPack2 = pUser.ResourceCount( 0x1bf2, requiredResourceType );

	if( ingotsInPack || ingotsInPack2 )
	{
		if( ingotsInPack == 500 || ingotsInPack2 == 500 )
		{
			socket.SysMessage( GetDictionaryEntry( 19100, socket.language ));// You've successfully converted the metal.
			var ingot = CreateDFNItem( pUser.socket, pUser, "0x1bf2", 500, "ITEM", true, ingotColor );
			ingot.name = ingotName;
			pUser.UseResource( 500, 0x1bf2, requiredResourceType );
			iUsed.Delete();
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 19101, socket.language ));// You can only convert five hundred ingots at a time.
		}
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 19102, socket.language ));// The item must be in your backpack to be exchanged.
	}
}

function elixirOfMetal( pUser, iUsed )
{
	const ingotColorMap = {
		"Dull Copper Ingot": 0x973,
		"Shadow Iron Ingot": 0x966,
		"Copper Ingot": 0x96D,
		"Bronze Ingot": 0x972
	};

	let ingotNames = [];
	for( let ingotName in ingotColorMap )
	{
		if( ingotColorMap.hasOwnProperty( ingotName ))
		{
			ingotNames.push( ingotName );
		}
	}

	const randomIndex = RandomNumber( 0, ingotNames.length - 1 );
	const ingotName = ingotNames[randomIndex];
	const ingotColor = ingotColorMap[ingotName];

	elixirConversion( pUser, iUsed, ingotName, ingotColor, 0 );
}

function elixirOfAgapite( pUser, iUsed )
{
	elixirConversion( pUser, iUsed, "Agapite Ingot", 0x979, 0x966 );
}

function elixirOfGold( pUser, iUsed )
{
	elixirConversion( pUser, iUsed, "Gold Ingot", 0x8A5, 0x973 );
}

function elixirOfValorite( pUser, iUsed )
{
	elixirConversion( pUser, iUsed, "Valorite Ingot", 0x8AB, 0x972 );
}

function elixirOfVerite( pUser, iUsed )
{
	elixirConversion( pUser, iUsed, "Verite Ingot", 0x89F, 0x96D );
}