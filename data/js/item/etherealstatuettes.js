const maxControlSlots = GetServerSetting( "MaxControlSlots" );
// maxFollowers only comes into play if maxControlSlots is set to 0 in UOX.INI
const maxFollowers = GetServerSetting( "MaxFollowers" );
const coreShardEra = EraStringToNum(GetServerSetting("CoreShardEra"));

function onUseChecked( pUser, iUsed )
{
	var socket = pUser.socket;
	var etherealMount = pUser.FindItemLayer( 0x19 );
	if( etherealMount != null )
	{
		socket.SysMessage( "You are already mounted." );
		return;
	}

	if( pUser.frozen || pUser.dead )
	{
		socket.SysMessage( "You cannot summon a mount right now." );
		return;
	}

	if( maxControlSlots > 0 )
	{
		if( pUser.controlSlotsUsed + 1 > maxControlSlots )
		{
			socket.SysMessage( GetDictionaryEntry( 2390, socket.language )); // That would exceed your maximum pet control slots.
			return;
		}
	}
	else if( maxFollowers > 0 && ( pUser.followerCount + 1 > maxFollowers ))
	{
		socket.SysMessage( GetDictionaryEntry( 2400, socket.language )); // You have too many followers already!
		return;
	}

	// Apply spell delay
	if( pUser.commandlevel < 2 )
	{
		pUser.SetTimer( 6, 3000 );
		pUser.frozen = true;
	}
	else
	{
		pUser.SetTimer( 6, 0 );
	}

	pUser.DoAction( 230 );
	pUser.SoundEffect( 0x5B4, true );
	pUser.SetTag( "EtherealMountSerial", iUsed.serial.toString() );
	pUser.SetTag( "EtherealMountSectionID", iUsed.sectionID );
	pUser.StartTimer( 1300, 1, 5300 );
}

function onTimer(pUser, timerID)
{
	var socket = pUser.socket;
	if( pUser.atWar || pUser.attacker != null)
	{
		socket.SysMessage( "You have been disrupted while attempting to summon your ethereal mount!" ); // You have been disrupted while attempting to summon your ethereal mount!
		pUser.SetTag( "EtherealMountSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		return;
	}

	if( pUser.dead )
	{
		socket.SysMessage( "You have been disrupted while attempting to summon your ethereal mount!" ); // You have been disrupted while attempting to summon your ethereal mount!
		pUser.SetTag( "EtherealMountSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		return;
	}

	var timeId = 2;
	if (coreShardEra >= EraStringToNum( "aos" ))
	{
		timeId = 3;
	}

	if( timerID < timeId )
	{
		pUser.DoAction( 230 );
		pUser.StartTimer( 1300, timerID + 1, 5300 );
	}

	if( timerID == timeId )
	{
		var etherealSerial = parseInt(pUser.GetTag( "EtherealMountSerial" ));
		var etherealSectionID = pUser.GetTag( "EtherealMountSectionID" );
		var etherealStatus = CalcItemFromSer( etherealSerial );

		if( !ValidateObject( pUser ) || pUser.mounted || pUser.dead || pUser.npc )
			return;

		var statueMap = {
			"etherealhorsestatuette": { section: "etherealhorse", color: 0x4001 },
			"etherealllamastatuette": { section: "ethereal_llama", color: 0x4001 },
			"etherealostardstatuette": { section: "etherealostard", color: 0x4001 },
			"etherealridgebackstatuette": { section: "etherealridgeback", color: 0x4001 },
			"etherealswampdragonstatuette": { section: "etherealswampdragon", color: 0x4001 },
			"etherealbeetlestatuette": { section: "etherealbeetle", color: 0x4001 },
			"etherealkirinstatuette": { section: "etherealkirin", color: 0x4001 },
			"etherealunicornstatuette": { section: "etherealunicorn", color: 0x4001 },
			"etherealcusidhestatuette": { section: "cusidhe" },
			"etherealreptalonstatuette": { section: "reptalon" },
			"etherealancienthellhoundstatuette": { section: "ancienthellhound" },
			"chargerofthefallenstatuette": { section: "chargerofthefallen" },
			"rideablebourastatuette": { section: "boura" },
			"lasherstatuette": { section: "lasher" },
			"etherealdragonstatuette": { section: "serpentinedragin" },
			"etherealwarboarstatuette": { section: "warboar" },
			"tarantulastatuette": { section: "tarantula" },
			"etherealtigerstatuette": { section: "tiger" },
			"rideablepolarbearstatuette": { section: "polarbear" },
			"windrunnerstatuette": { section: "windrunner" },
			"eowmustatuette": { section: "eowmu" },
			"coconutcrabstatuette": { section: "coconutcrab" },
			"capybarastatuette": { section: "capybara" },
			"skeletalcatstatuette": { section: "skeletalcat" },
			"manticorestatuette": { section: "manticore" },
			"molderingursinestatuette": { section: "molderingursine" }
		};

		var statueData = statueMap[etherealSectionID];

		if (!statueData)
		{
			pUser.SysMessage("Unknown etherealSectionID: " + etherealSectionID);
			pUser.SetTag("EtherealMountSerial", null);
			pUser.SetTag("EtherealMountSectionID", null);
			pUser.frozen = false;
			return;
		}

		var itemMade = CreateDFNItem(socket, pUser, statueData.section, 1, "ITEM", true);
		if (itemMade)
		{
			itemMade.container = pUser;
			itemMade.layer = 0x19;
			// Assign color only if explicitly defined
			if (statueData.color != undefined)
			{
				itemMade.color = statueData.color;
			}
		}

		// Increase pet control slots in use for owner, if feature is enabled
		if( maxControlSlots > 0 )
		{
			pUser.controlSlotsUsed = pUser.controlSlotsUsed + 1;
		}

		etherealStatus.Delete();
		pUser.Mounted  = true;
		pUser.SetTag( "EtherealMountSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		pUser.AddScriptTrigger( 5301 );
		pUser.SoundEffect( 0x5B5, true );
		socket.SysMessage( "You summon your ethereal steed." );
	}
}