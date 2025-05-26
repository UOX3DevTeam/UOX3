function onCharDoubleClick( pUser, targChar )
{
	var pPack = pUser.pack;
	if( pPack.totalItemCount >= pPack.maxItems || pPack.weight >= pPack.weightMax )
	{
		pSocket.SysMessage( "You cannot dismount your ethereal because your pack cannot hold it" ); // You cannot dismount your ethereal because your pack cannot hold it
	}
	else
	{
		DismountEtherealMount( pUser );
	}
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
		var mountIDtoSection = {
			0x3EAA: "etherealhorsestatuette",
			0x3EAB: "etherealllamastatuette",
			0x3EAC: "etherealostardstatuette",
			0x3E9A: "etherealridgebackstatuette",
			0x3E98: "etherealswampdragonstatuette",
			0x3E97: "etherealbeetlestatuette",
			0x3EAD: "etherealkirinstatuette",
			0x3EB4: "etherealunicornstatuette",
			0x3E91: "etherealcusidhestatuette",
			0x3E90: "etherealreptalonstatuette",
			0x3E94: "etherealhiryustatuette",
			0x3EC9: "etherealancienthellhoundstatuette",
			0x3E92: "chargerofthefallenstatuette",
			0x3EC6: "rideablebourastatuette",
			0x3ECB: "lasherstatuette",
			0x3ECE: "etherealdragonstatuette",
			0x3ED2: "etherealwarboarstatuette",
			0x3ECA: "tarantulastatuette",
			0x3EC8: "etherealtigerstatuette",
			0x3EC5: "rideablepolarbearstatuette",
			0x3ECC: "windrunnerstatuette",
			0x3ECF: "eowmustatuette",
			0x3ED1: "coconutcrabstatuette",
			0x3ED3: "capybarastatuette",
			0x3ED0: "skeletalcatstatuette",
			0x3EDB: "manticorestatuette",
			0x3EDC: "molderingursinestatuette"
		};

		var sectionID = mountIDtoSection[etherealMount.id];
		if( sectionID )
		{
			var iMountStatue = CreateDFNItem( pUser.socket, pUser, sectionID, 1, "ITEM", true );
			if( ValidateObject( iMountStatue ))
			{
				pUser.RemoveScriptTrigger(5301);
				pUser.controlSlotsUsed = Math.max( 0, pUser.controlSlotsUsed - 1 );
				etherealMount.Delete();
			}
		}
	}
}