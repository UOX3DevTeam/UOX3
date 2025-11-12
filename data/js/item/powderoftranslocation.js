/// <reference path="../definitions.d.ts" />
// @ts-check
/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iPowder )
{
	var pSocket = pUser.socket;
	if( pSocket == null )
	{
		return false;
	}

	pSocket.tempObj = iPowder;
	pSocket.CustomTarget( 0 );
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, myTarget )
{
	var iPowder = socket.tempObj;
	if( !socket.GetWord( 1 ) && myTarget.isItem && ( myTarget.sectionID == "bagofsending" || myTarget.sectionID == "crystalballofpetsummoning" ))
	{
		var chargeTag = myTarget.GetTag("charges");
		if( !chargeTag )
			return;

		var myCharges = chargeTag.split("|");
		if( myCharges.length != 3 )
			return;

		var charges = parseInt( myCharges[0] );
		var reCharges = parseInt( myCharges[1] );
		var maxCharges = parseInt( myCharges[2] );

		if( charges >= maxCharges )
		{
			socket.SysMessage( GetDictionaryEntry( 19081, socket.language )); //  This item cannot absorb any more powder of translocation.
			return;
		}

		var maxReCharges = maxCharges - charges;
		if( maxReCharges > maxCharges - reCharges ) 
		{
			maxReCharges = maxCharges - reCharges;
			if( maxReCharges == 0 )
			{
				socket.SysMessage( GetDictionaryEntry( 19082, socket.language )); //  This item has been oversaturated with powder of translocation and can no longer be recharged.
				return;
			}
		}

		if( iPowder.amount > maxReCharges )
		{
			// We've got more powder than we can use
			charges += maxReCharges;
			iPowder.amount -= maxReCharges;
			reCharges += maxReCharges;
		}
		else
		{
			// We've got less or exactly the amount of powder we can use
			charges += iPowder.amount;
			reCharges += iPowder.amount;
			iPowder.Delete()
		}

		myTarget.SetTag( "charges", charges + "|" + reCharges + "|" + maxCharges );
		socket.SysMessage( "The " + myTarget.name + GetDictionaryEntry( 19083, socket.language )); //  The ~1_translocationItem~ glows with green energy and absorbs magical power from the powder.
		myTarget.Refresh();
	}
	else
	{
		socket.SysMessage( GetDictionaryEntry( 19084, socket.language )); //  Powder of translocation has no effect on this item.
		return;
	}
}
