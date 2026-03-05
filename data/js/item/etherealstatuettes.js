const maxControlSlots = GetServerSetting( "MaxControlSlots" );
// maxFollowers only comes into play if maxControlSlots is set to 0 in UOX.INI
const maxFollowers = GetServerSetting( "MaxFollowers" );
const coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ) );

/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var iTime = GetCurrentClock();
	var NextUse = iUsed.GetTempTag( "castDelayed" );
	var delayed = pUser.GetTempTag( "statueDelayed" );
	var Delay = 3000;

	// Find owner of root container iUsed is contained in, if any
	var packOwner = GetPackOwner( iUsed, 0 );
	var pSocket = pUser.socket;
	if( pSocket == null || !ValidateObject( pUser ))
		return false;

	var etherealMount = pUser.FindItemLayer( 0x19 );

	if( packOwner == null && packOwner.serial != pUser.serial )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6581, pSocket.language ));// This must be in the top layer of your pack to use it.
		return false;
	}

	if( delayed )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6582, pSocket.language ));// You must finish casting before using another one.
		return false;
	}

	NextUse = NextUse != null ? parseInt( NextUse ) : 0;

	if(( iTime - NextUse ) < Delay )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6582, pSocket.language ));// You must finish casting before using another one.
		return false;
	}

	if( pUser.race == 2 )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6583, pSocket.language ));// Gargoyles are unable to ride animals.
		return false;
	}

	if( etherealMount != null )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6584, pSocket.language ));// Please dismount first.
		return false;
	}

	if( pUser.isPolymorphed )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6585, pSocket.language ));// You can't do that while polymorphed.
		return false;
	}

	if( pUser.frozen || pUser.dead )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6586, pSocket.language ));// You cannot summon a mount right now.
		return false;
	}

	if( maxControlSlots > 0 )
	{
		if( pUser.controlSlotsUsed + 1 > maxControlSlots )
		{
			pSocket.SysMessage( GetDictionaryEntry( 2390, pSocket.language )); // That would exceed your maximum pet control slots.
			return false;
		}
	}
	else if( maxFollowers > 0 && ( pUser.followerCount + 1 > maxFollowers ))
	{
		pSocket.SysMessage( GetDictionaryEntry( 2400, pSocket.language )); // You have too many followers already!
		return false;
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
	pUser.SetTag( "EtherealMountStatueSerial", iUsed.serial.toString() );
	pUser.SetTag( "EtherealMountSectionID", iUsed.sectionID.toString().toLowerCase() );
	iUsed.SetTempTag( "castDelayed", iTime.toString() );
	pUser.SetTempTag( "statueDelayed", true );
	pUser.StartTimer( 1300, 1, 5300 );
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( pUser, timerID )
{

	var pSocket = pUser.socket;
	if( pSocket == null )
		return;

	if( pUser.atWar || pUser.attacker != null)
	{
		pSocket.SysMessage( GetDictionaryEntry( 6587, pSocket.language )); // You have been disrupted while attempting to summon your ethereal mount!
		pUser.SetTag( "EtherealMountStatueSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		return;
	}

	if( pUser.dead )
	{
		pSocket.SysMessage( GetDictionaryEntry( 6587, pSocket.language )); // You have been disrupted while attempting to summon your ethereal mount!
		pUser.SetTag( "EtherealMountStatueSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		return;
	}

	var timeId = 2;
	if( coreShardEra >= EraStringToNum( "aos" ))
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
		var etherealSerial = parseInt( pUser.GetTag( "EtherealMountStatueSerial" ));
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
			pUser.SetTag( "EtherealMountStatueSerial", null );
			pUser.SetTag( "EtherealMountSectionID", null );
			pUser.SetTempTag( "statueDelayed", null );
			pUser.frozen = false;
			return;
		}

		// Check retouching style: transparent or normal
		var retouchingStyle = etherealStatuette.GetTag( "retouching" );
		var savedHue = parseInt( etherealStatuette.GetTag( "saveColor" ));
		var customHue = etherealStatuette.color;
		var itemMade = CreateDFNItem( pSocket, pUser, statueData.section, 1, "ITEM", true );
		if( itemMade )
		{
			itemMade.container = pUser;
			itemMade.layer = 0x19;

			if( retouchingStyle == "transparent" )
			{
				itemMade.SetTag( "saveColor", customHue );
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

		pUser.SetTempTag( "statueDelayed", false );
		pUser.SetTag( "EtherealMountStatueSerial", null );
		pUser.SetTag( "EtherealMountSectionID", null );
		pUser.frozen = false;
		pUser.AddScriptTrigger( 5301 );
		pSocket.SysMessage( GetDictionaryEntry( 6588, pSocket.language )); // You summon your ethereal steed.
		etherealStatuette.Delete();
	}
}
