function CommandRegistration()
{
	RegisterCommand( "wipe", 3, true );
	RegisterCommand( "iwipe", 3, true );
}

function command_WIPE( socket, cmdString )
{
	iWipe = false;
	CallWipe( socket, cmdString );
}

function command_IWIPE( socket, cmdString )
{
	iWipe = true;
	CallWipe( socket, cmdString );
}

function CallWipe( socket, cmdString )
{
	all	= false;
	if( cmdString )
	{
		var splitString = cmdString.split( " " );
		if( splitString[3] )
		{
			socket.clickX = parseInt( splitString[0] );
			socket.clickY = parseInt( splitString[1] );
			socket.SetWord( 11, parseInt( splitString[2] ) );
			socket.SetWord( 13, parseInt( splitString[3] ) );
			DoWipe( socket, null );
		}
		else if( splitString[0] )
		{
			var mChar	= socket.currentChar;
			all		= true;
			var uKey 	= splitString[0].toUpperCase();
			var saidAll 	= ( uKey == "ALL" );
			var counter	= 0;
			var counterStr	= "";
			socket.SysMessage( "Wiping " + uKey );
			if( saidAll || uKey == "ITEMS" )
			{
				Console.Print( mChar.name + " has initiated an item wipe.\n" );
				isItem 		= true;
				counter 	= IterateOver( "ITEM" );
				counterStr 	= counter.toString();
				Console.Print( "Item wipe deleted " + counterStr + " items.\n" );
				socket.SysMessage( "Wiped " + counterStr + " items" );
			}
			if( saidAll || uKey == "NPCS" )
			{
				Console.Print( mChar.name + " has initiated a npc wipe.\n" );
				isItem 		= false;
				counter 	= IterateOver( "CHARACTER" );
				counterStr 	= counter.toString();
				Console.Print( "NPC wipe deleted " + counterStr + " npcs.\n" );
				socket.SysMessage( "Wiped " + counterStr + " npcs" );
			}
		}
	}
	else
	{
		socket.clickX = -1;
		socket.clickY = -1;
		socket.CustomTarget( 0, "Choose top corner to wipe" );
	}
}

function onCallback0( socket, ourObj )
{
	socket.clickX 	= socket.GetWord( 11 );
	socket.clickY 	= socket.GetWord( 13 );
	socket.CustomTarget( 1, "Choose bottom corner to wipe" );
}

function onCallback1( socket, ourObj )
{
	DoWipe( socket, ourObj );
}

function DoWipe( socket, ourObj )
{
	var mChar = socket.currentChar;
	x1 = socket.clickX;
	y1 = socket.clickY;
	x2 = socket.GetWord( 11 );
	y2 = socket.GetWord( 13 );
	var tmpLoc;
	if( x1 > x2 )
	{
		tmpLoc 	= x1;
		x1 	= x2;
		x2	= tmpLoc;
	}
	if( y1 > y2 )
	{
		tmpLoc 	= y1;
		y1 	= y2;
		y2 	= tmpLoc;
	}

	Console.Print( mChar.name + " has initiated a wipe.\n" );
	socket.SysMessage( "Wiping.." );
	var counter 	= IterateOver( "ITEM" );
	var counterStr	= counter.toString();
	socket.SysMessage( "Wiped " + counterStr + " items" );
	Console.Print( "Wipe deleted " + counterStr + " items.\n" );

	socket.clickX = -1;
	socket.clickY = -1;
	x1 	= 0;
	y1 	= 0;
	x2 	= 0;
	y2 	= 0;
	iWipe 	= false;
	all	= false;
	isItem	= false;
}

function onIterate( toCheck )
{
	if( toCheck )
	{
		if( all )
		{
			if( isItem && toCheck.isItem )
			{
				if( toCheck.wipable && toCheck.container == null && toCheck.type != 202 )
				{
					toCheck.Delete();
					return true;
				}
			}
			else if( !isItem && toCheck.isChar )
			{
				if( toCheck.npc && toCheck.aitype != 17 && !toCheck.tamed )
				{
					toCheck.Delete();
					return true;
				}
			}
		}
		else
		{
			if( toCheck.isItem && toCheck.container == null )
			{
				var shouldWipe 	= iWipe;
				var tX 		= toCheck.x;
				var tY 		= toCheck.y;
				if( tX >= x1 && tX <= x2 && tY >= y1 && tY <= y2 )
					shouldWipe = !iWipe;
				if( shouldWipe )
				{
					toCheck.Delete();
					return true;
				}
			}
		}
	}
	return false;
}
