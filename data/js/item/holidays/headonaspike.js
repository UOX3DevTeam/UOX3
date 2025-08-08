function onUseChecked( pUser, iUsed )
{
	var pSock = pUser.socket;
	var headspike = new Gump;
	var head = 0;

	switch( iUsed.sectionID )
	{
		case "MrsTroubleMakersHeadOnASpike": head = 30522; break;
		case "BrutrinsHeadOnASpike": head = 30522; break;
		case "StethunsHeadOnASpike": head = 30522; break;
		case "RakbansHeadOnASpike": head = 30522; break;
		case "ThatbloksHeadOnASpike": head = 30522; break;
		case "GryphonsHeadOnASpike": head = 30522; break;
		case "KyronixsHeadOnASpike": head = 30522; break;
		case "MisksHeadOnASpike": head = 30522; break;
		case "BleaksHeadOnASpike": head = 30522; break;
		case "OnifrksHeadOnASpike": head = 30522; break;
		case "MesannasHeadOnASpike": head = 30522; break;
		default: head = 30522;
	}

	headspike.AddGump( 100, 100, head );
	headspike.Send( pUser );
	headspike.Free();
}