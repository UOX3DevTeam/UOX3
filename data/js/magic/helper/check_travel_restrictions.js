function CheckTravelRestrictions( targPlayer, spellNum, targX, targY, targWorld, targInstanceID )
{
    const targRegion = GetTownRegionFromXY( targX, targY, targWorld, targInstanceID );
	var pSocket = targPlayer.socket;

    const travelRules = {
        32: { type: 'canRecall', message: 15990 },    // You are not allowed to travel there. // Recall spell
        52: { type: 'canGate', message: 15990 },    // You are not allowed to travel there.  // Gate spell
        22: { type: 'canTeleport', message: 15991 }, // You cannot teleport from here to the destination.   // Teleport spell
        45: { type: 'canMark', message: 15992 }            // You can not mark a rune here.  // Mark spell
    };

    const travelType = travelRules[spellNum];

    if( travelType)
    {
        if( spellNum !== 45 && !targRegion[travelType.type] )
        {
			pSocket.SysMessage( GetDictionaryEntry( travelType.message, pSocket.language ));
            return true;
        }
        else if( !targPlayer.region[travelType.type] )
        {
			pSocket.SysMessage( GetDictionaryEntry( travelType.message, pSocket.language ));
            return true;
        }
    }
    return false;
}