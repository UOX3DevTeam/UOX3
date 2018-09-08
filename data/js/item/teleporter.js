function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.InRange( iUsed, 3 ) )
		{
			var otherGate = CalcItemFromSer( iUsed.morex );
			if( ValidateObject( otherGate ) )
				pUser.Teleport( otherGate );
		}
	}
	return false;
}
