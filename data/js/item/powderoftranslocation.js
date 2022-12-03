function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	socket.CustomTarget( 0 );
}

function onCallback0( socket, myTarget )
{
	var pUser = socket.currentChar;
	var iUsed = socket.tempObj;
	var Charges = parseInt( myTarget.GetTag( "Charges" ));
	var maxCharges = parseInt( myTarget.GetTag( "maxCharges" ));

	if( !socket.GetWord(1) && myTarget.isItem && myTarget.sectionID == "bagofsending" )
	{
		if( Charges >= maxCharges )
		{
			pUser.SysMessage( "This item cannot absorb any more powder of translocation." ); //  This item cannot absorb any more powder of translocation.
			return false;
		}
		else
		{
			myTarget.SetTag( "Charges", Charges + 1 );
			if( iUsed.amount > 1 )
			{
				iUsed.amount = ( iUsed.amount - 1 );
			}
			else
			{
				iUsed.Delete();
			}
			myTarget.Refresh();
		}
	}
	else
	{
		pUser.SysMessage( "Powder of translocation has no effect on this item." ); //  Powder of translocation has no effect on this item.
		return false;
	}
}