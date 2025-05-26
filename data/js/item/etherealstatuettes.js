const maxControlSlots = GetServerSetting( "MaxControlSlots" );
// maxFollowers only comes into play if maxControlSlots is set to 0 in UOX.INI
const maxFollowers = GetServerSetting( "MaxFollowers" );
const coreShardEra = EraStringToNum(GetServerSetting("CoreShardEra"));

function onUseChecked( pUser, iUsed )
{
	var iTime = GetCurrentClock();
	var NextUse = iUsed.GetTempTag( "castDelayed" );
	var delayed = pUser.GetTempTag( "statueDelayed" );
	var Delay = 3000;

	// Find owner of root container iUsed is contained in, if any
	var packOwner = GetPackOwner( iUsed, 0 );
	var socket = pUser.socket;
	var etherealMount = pUser.FindItemLayer( 0x19 );

	if( packOwner == null && packOwner.serial != pUser.serial )
	{
		socket.SysMessage( "This must be in the top layer of your pack to use it." );// This must be in the top layer of your pack to use it.
		return;
	}

	if( delayed )
	{
		socket.SysMessage( "You must finish casting before using another one." );// You must finish casting before using another one.
		return;
	}

	NextUse = NextUse != null ? parseInt( NextUse ) : 0;

	if(( iTime - NextUse ) < Delay )
	{
		pUser.SysMessage( "You must finish casting before using another one." );// You must finish casting before using another one.
		return;
	}

	if( pUser.race == 2 )
	{
		socket.SysMessage( "Gargoyles are unable to ride animals." );// Gargoyles are unable to ride animals.
		return;
	}

	if( etherealMount != null )
	{
		socket.SysMessage( "Please dismount first." );// Please dismount first.
		return;
	}

	if( pUser.isPolymorphed )
	{
		socket.SysMessage( "You can't do that while polymorphed." );// You can't do that while polymorphed.
		return;
	}

	if( pUser.frozen || pUser.dead )
	{
		socket.SysMessage( "You cannot summon a mount right now." );// You cannot summon a mount right now.
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
	pUser.SetTag( "EtherealMountStatueSerial", iUsed.serial.toString() );
	pUser.SetTag( "EtherealMountSectionID", iUsed.sectionID );
	iUsed.SetTempTag( "castDelayed", iTime.toString() );
	pUser.SetTempTag( "statueDelayed", true );
	pUser.StartTimer( 1300, 1, 5300 );
}

function onTimer( pUser, timerID )
{
	var socket = pUser.socket;
	if( pUser.atWar || pUser.attacker != null)
	{
		socket.SysMessage( "You have been disrupted while attempting to summon your ethereal mount!" ); // You have been disrupted while attempting to summon your ethereal mount!
		pUser.SetTag( "EtherealMountStatueSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		return;
	}

	if( pUser.dead )
	{
		socket.SysMessage( "You have been disrupted while attempting to summon your ethereal mount!" ); // You have been disrupted while attempting to summon your ethereal mount!
		pUser.SetTag( "EtherealMountStatueSerial", null );
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
		var etherealSerial = parseInt(pUser.GetTag( "EtherealMountStatueSerial" ));
		var etherealSectionID = pUser.GetTag( "EtherealMountSectionID" );
		var etherealStatuette = CalcItemFromSer( etherealSerial );

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
			"etherealcusidhestatuette": { section: "cusidhe", color: 0x4001  },
			"etherealreptalonstatuette": { section: "hiryu", color: 0x4001  },
			"etherealhiryustatuette": { section: "reptalon", color: 0x4001  },
			"etherealancienthellhoundstatuette": { section: "ancienthellhound", color: 0x4001   },
			"chargerofthefallenstatuette": { section: "chargerofthefallen" },
			"rideablebourastatuette": { section: "boura" },
			"lasherstatuette": { section: "lasher" },
			"etherealdragonstatuette": { section: "serpentinedragin", color: 0x4001  },
			"etherealwarboarstatuette": { section: "warboar", color: 0x4001 },
			"tarantulastatuette": { section: "tarantula" },
			"etherealtigerstatuette": { section: "etherealtiger", color: 0x4001 },
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

		if( !statueData )
		{
			pUser.SysMessage( "Unknown etherealSectionID: " + etherealSectionID );
			pUser.SetTag( "EtherealMountStatueSerial", null );
			pUser.SetTag( "EtherealMountSectionID", null );
			pUser.frozen = false;
			return;
		}

		// Check retouching style: transparent or normal
		var retouchingStyle = etherealStatuette.GetTag( "retouching" );
		var savedHue = parseInt( etherealStatuette.GetTag( "saveColor" ));
		var customHue = etherealStatuette.color;
		var itemMade = CreateDFNItem( socket, pUser, statueData.section, 1, "ITEM", true );
		if( itemMade )
		{
			itemMade.container = pUser;
			itemMade.layer = 0x19;

			if( retouchingStyle == "transparent" )
			{
				itemMade.color = 0x4001;
				pUser.SetTag( "retouching", "transparent" );
			}
			else
			{
				// Use custom dyed hue if present, else fallback to default
				if( customHue != null && customHue != 0 || savedHue != 0)
				{
					itemMade.color = customHue;
					pUser.SetTag( "customhue", customHue );
					itemMade.SetTag( "saveColor", customHue );
				}
				else if( statueData.color != undefined )
				{
					itemMade.color = statueData.color;
				}
				pUser.SetTag( "retouching", "normal" );
			}
		}

		// Increase pet control slots in use for owner, if feature is enabled
		if( maxControlSlots > 0 )
		{
			pUser.controlSlotsUsed = pUser.controlSlotsUsed + 1;
		}

		etherealStatuette.Delete();
		pUser.SetTempTag( "statueDelayed", false );
		pUser.SetTag( "EtherealMountStatueSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		pUser.AddScriptTrigger( 5301 );
		pUser.SoundEffect( 0x5B5, true );
		socket.SysMessage( "You summon your ethereal steed." );
	}
}

function onTooltip( etherealStatuette )
{
	var tooltipText = "";
	var vetReward = etherealStatuette.GetTag( "vetRewardYear" );
	var retouchColor = etherealStatuette.GetTag( "retouching" );

	// Handle vetRewardYear
	if( vetReward )
	{
		var indicator = "th";
		switch( vetReward )
		{
			case 1: indicator = "st"; break;
			case 2: indicator = "nd"; break;
			case 3: indicator = "rd"; break;
		}
		tooltipText = vetReward + indicator + " Year Veteran Reward";
	}

	// Handle retouching color
	if( retouchColor != null )
	{
		if( tooltipText != "" )
			tooltipText += "\n";

		tooltipText += "Mount Hue: " + retouchColor;
	}

	if( tooltipText != "" )
		etherealStatuette.SetTempTag( "tooltipSortOrder", 21 );

	return tooltipText;
}

function _restorecontext_() {}