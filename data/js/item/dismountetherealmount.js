function onCharDoubleClick( pUser, targChar )
{
	DismountEtherealMount( pUser );
	return true;
}

function onDeathBlow( pDead, pKiller )
{
	DismountEtherealMount( pDead );
	return true;
}

function DismountEtherealMount( pUser )
{
	var etherealMount = pUser.FindItemLayer( 0x19 );
    if( etherealMount != null )
    {
        let iMountStatue = null;
        if( etherealMount.id == 0x3EAA )
        {
            iMountStatue = CreateDFNItem( pUser.socket, pUser, "etherealhorsestatuette", 1, "ITEM", true );
        }
        else if( etherealMount.id == 0x3EAB )
        {
            iMountStatue = CreateDFNItem( pUser.socket, pUser, "etherealllamastatuette", 1, "ITEM", true );
        }
        else if( etherealMount.id == 0x3EAC )
        {
            iMountStatue = CreateDFNItem( pUser.socket, pUser, "etherealostardstatuette", 1, "ITEM", true );
        }
		else if( etherealMount.id == 0x3E9A )
        {
            iMountStatue = CreateDFNItem( pUser.socket, pUser, "etherealridgebackstatuette", 1, "ITEM", true );
        }
		else if( etherealMount.id == 0x3E98 )
        {
            iMountStatue = CreateDFNItem( pUser.socket, pUser, "etherealswampdragonstatuette", 1, "ITEM", true );
        }

        if( ValidateObject( iMountStatue ))
        {
            pUser.RemoveScriptTrigger( 5301 );
            // Reduce control slots in use for player by amount occupied by pet
            pUser.controlSlotsUsed = Math.max( 0, pUser.controlSlotsUsed - 1 );
			pUser.Mounted  = false;
			etherealMount.Delete();
        }
    }
}