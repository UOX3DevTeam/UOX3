// baking script

function onUseChecked ( pUser, iUsed )
{
	// get users socket
	var srcSock = pUser.socket;

	// is it in users pack?
	if( iUsed.container != null )
	{
		var iPackOwner = GetPackOwner( iUsed, 0 );
		if( iPackOwner.serial != pUser.serial )
		{
			srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
		}
		else
		{
			// let the user target the heat source
			srcSock.tempObj = iUsed;
			srcSock.CustomTarget( 0, GetDictionaryEntry( 6097, srcSock.language )); // What do you want to cook this on?
		}
	}
	else
	{
		srcSock.SysMessage( GetDictionaryEntry( 6022, srcSock.language )); // This has to be in your backpack before you can use it.
	}
	return false;
}

function onCallback0( tSock, myTarget )
{
	Baking( tSock, myTarget );
}

function Baking( tSock, myTarget )
{
	var pUser = tSock.currentChar;
	var iUsed = tSock.tempObj;
	var strangeByte = tSock.GetWord( 1 );
	var tileID = tSock.GetWord( 17 );
	var foodColor = iUsed.colour;
	var cookingMsg = "";

	if( tileID == 0 || ( strangeByte == 0 && ValidateObject( myTarget ) && myTarget.isChar ))
	{
		cookingMsg = GetDictionaryEntry( 6095, tSock.language ); // You cannot cook your %s on that.
		tSock.SysMessage( cookingMsg.replace( /%s/gi, iUsed.name ));
		return;
	}

	if( TriggerEvent( 106, "Ovens", tileID ))
	{
		if( TriggerEvent( 103, "RangeCheck", tSock, pUser ))
		{
			tSock.SysMessage( GetDictionaryEntry( 393, tSock.language )); // That is too far away.
			return;
		}

		if( !pUser.CheckSkill( 13, 0, 140 ))
		{
			cookingMsg = GetDictionaryEntry( 6093, tSock.language ); // You burn the %s to a crisp! It's ruined.
			tSock.SysMessage( cookingMsg.replace( /%s/gi, iUsed.name ));
			iUsed.id = 0x09F2; // burn food id
			return;
		}

		if( pUser.GetTempTag( "CookingDelay" ) == 1 )
		{
			tSock.SysMessage( GetDictionaryEntry( 6094, tSock.language )); // You are already cooking something.
			return;
		}

		switch( iUsed.name )
		{
			case "unbaked apple pie":
				pUser.StartTimer( 5000, 1, true );
				break;
			case "unbaked pumpkin pie":
				pUser.StartTimer( 5000, 2, true );
				break;
			case "unbaked meat pie":
				pUser.StartTimer( 5000, 3, true );
				break;
			case "unbaked fruit pie":
				pUser.StartTimer( 5000, 4, true );
				break;
			case "unbaked peach cobbler":
				pUser.StartTimer( 5000, 5, true );
				break;
			case "unbaked quiche":
				pUser.StartTimer( 5000, 6, true );
				break;
			case "cake mix":
				pUser.StartTimer( 5000, 7, true );
				break;
			case "cookie mix":
				pUser.StartTimer( 5000, 8, true );
				break;
		}
		pUser.SetTempTag( "CookingDelay", 1 )
		pUser.UseResource( 1, iUsed.id, foodColor );
		pUser.SoundEffect( 0x0057, true );
		return;
	}
}

function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ) || pUser.socket == null )
		return;

	var socket = pUser.socket;
	switch( timerID )
	{
		case 1:
			CreateDFNItem( socket, pUser, "baked_apple_pie", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6099, socket.language )); // You bake a pie.
			break;
		case 2:
			CreateDFNItem( socket, pUser, "baked_pumpkin_pie", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6099, socket.language )); // You bake a pie.
			break;
		case 3:
			CreateDFNItem( socket, pUser, "baked_meat_pie", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6099, socket.language )); // You bake a pie.
			break;
		case 4:
			CreateDFNItem( socket, pUser, "baked_fruit_pie", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6099, socket.language )); // You bake a pie.
			break;
		case 5:
			CreateDFNItem( socket, pUser, "baked_peach_cobbler", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6099, socket.language )); // You bake a pie.
			break;
		case 6:
			CreateDFNItem( socket, pUser, "baked_quiche", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6099, socket.language )); // You bake a pie.
			break;
		case 7:
			CreateDFNItem( socket, pUser, "0x09E9", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6100, socket.language )); // You bake a cake.
			break;
		case 8:
			CreateDFNItem( socket, pUser, "0x160B", 1, "ITEM", true );
			socket.SysMessage( GetDictionaryEntry( 6101, socket.language )); // You bake a cookie
			break;
		default:
			return;
	}
	pUser.SetTempTag( "CookingDelay", null );
}
