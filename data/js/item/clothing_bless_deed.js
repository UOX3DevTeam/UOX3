function onUseChecked( pUser, iUsed )
{
    const pSocket = pUser.socket;
    if( pUser.visible == 1 || pUser.visible == 2 )
    {
        pUser.visible = 0;
    }

    const itemOwner = GetPackOwner( iUsed, 0 );
    if ( itemOwner == null || itemOwner.serial != pUser.serial )
    {
        pUser.SysMessage( GetDictionaryEntry( 1763, pSocket.language )); // That item must be in your backpack before it can be used.
        return false;
    }

    pSocket.SysMessage( GetDictionaryEntry( 19001, pSocket.language) ); // Select the item of clothing that you want to bless.

    pUser.tempClothingBlessDeed = iUsed;
    pUser.CustomTarget( 0 );
}

function onCallback0( socket, targetObj )
{
    // Abort if player cancels target cursor or clicks empty space.
    const cancelCheck = parseInt( socket.GetByte( 11 ) );
    if( cancelCheck == 255 || !targetObj )
        return;

    // The following check assumes every does their diligence in adding a `good` property to clothing items.
    // `good` values of 18, 19, and 20 indicate clothes, footwear, and headwear respectively.
    if ( !targetObj.isItem || !targetObj.good || targetObj.good < 18 || targetObj.good > 20 )
    {
        socket.SysMessage( GetDictionaryEntry( 19003, socket.language) ); // You can only bless clothing with this deed!
    }
    else
    {
        const pUser = socket.currentChar;
        const targetObjOwner = GetPackOwner( targetObj, 0 );
        if ( targetObjOwner == null || targetObjOwner.serial != pUser.serial )
        {
            socket.SysMessage( GetDictionaryEntry( 19004, socket.language ) ); // Items must be equipped or in your backpack to be blessed.
            return;
        }
        targetObj.isNewbie = true;
        targetObj.Refresh();
        pUser.tempClothingBlessDeed.Delete();
        delete pUser.tempClothingBlessDeed;
        socket.SysMessage( GetDictionaryEntry( 19002, socket.language ) ); // The item has been blessed.
    }
}