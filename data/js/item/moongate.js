function onUse( pUser, iUsed )
{
	var socket = pUser.socket;
	if( socket && iUsed && iUsed.isItem )
	{
		if( pUser.inRange( iUsed, 3 ) )
		{
			var otherGate = CalcItemFromSer( iUsed.morex );
			pUser.Teleport( otherGate );
		}
	}
	return false;
}
