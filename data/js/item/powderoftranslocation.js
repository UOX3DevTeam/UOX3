function onUseChecked( pUser, iPowder )
{
	var socket = pUser.socket;
	socket.tempObj = iPowder;
	socket.CustomTarget( 0 );
}

function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	var iPowder = socket.tempObj;

	if( !socket.GetWord( 1 ) && myTarget.isItem && ( myTarget.sectionID == "bagofsending" || myTarget.sectionID == "crystalballofpetsummoning" ))
	{
		var chargeTag = myTarget.GetTag("charges");
		if( !chargeTag )
			return false;

		var myCharges = chargeTag.split("|");
		if( myCharges.length != 3 )
			return false;

		var charges = parseInt( myCharges[0] );
		var reCharges = parseInt( myCharges[1] );
		var maxCharges = parseInt( myCharges[2] );

		if( charges >= maxCharges )
		{
			socket.SysMessage( GetDictionaryEntry( 19081, socket.language )); //  This item cannot absorb any more powder of translocation.
			return false;
		}

		var maxReCharges = maxCharges - charges;
		if( maxReCharges > maxCharges - reCharges ) 
		{
			maxReCharges = maxCharges - reCharges;
			if( maxReCharges == 0 )
			{
				socket.SysMessage( GetDictionaryEntry( 19082, socket.language )); //  This item has been oversaturated with powder of translocation and can no longer be recharged.
				return false;
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
		return false;
	}
}

function _restorecontext_() {}