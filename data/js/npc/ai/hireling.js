// Hirelings, by Xuri (xuri@uox3.org)
// Version: 1.1
// Last Updated: September 10th 2022
//
// For context menu support, hireling needs to be setup with a HIRELING tag in the DFNs
//
// Supported commands:
/* 	"(Name) come"		Summons the hireling to your location.
	"(Name) drop"		Drops everything they're carrying to the ground
	"(Name) follow"		Follows targeted being.
	"(Name) follow me"	Follows you.
	"(Name) friend"		Treats targeted player as another owner.
	"(Name) guard"		Will guard you.
	"(Name) hire"		Responds with how much it will cost to hire them.
	"(Name) kill",
	"(Name) attack"		Attacks targeted being.
	"(Name) move"		Makes the hireling move away so you can pass.
	"(Name) report"		Responds with how they feel about their job.
	"(Name) stay"		Stops and stays in current spot.
	"(Name) stop"		Cancels any current orders to guard or follow.
	"(Name) transfer"	Transfers complete ownership to targeted player.
	"all come" 			All followers come to owner
	"all follow me"		All followers follows to owner
	"all follow" 		All followers follow a specific target
	"all attack" 		All followers attack a specified target
	"all guard (me)"	All followers guard the owner
	"all stop"			All followers stop patrolling, fighting or guarding
	"all stay"			All followers stay where they are*/

var order_messages = new Array();
var accepted_messages = new Array();
var occupied_messages = new Array();
var grace_messages = new Array();
var finished_messages = new Array();
var payday_messages = new Array();

// Dictionary messages sent when accepting order
accepted_messages[0] = 2306; // As thou please.
accepted_messages[1] = 2307; // I'll do my best!
accepted_messages[2] = 2308; // At thy behest.

// Dictionary messages sent when already hired by someone else
occupied_messages[0] = 2309; // I have already been hired.
occupied_messages[1] = 2310; // Sorry, I am already hired by someone else.
occupied_messages[2] = 2311; // Can't you see I'm busy?

// Dictionary messages sent when reporting on current status
grace_messages[0] = 2312; // I am considering quitting.
grace_messages[1] = 2313; // This job doth not pay enough.
grace_messages[2] = 2314; // 'Tis time to be thinking about a new master.
grace_messages[3] = 2315; // Should I not be paid soon?
grace_messages[4] = 2316; // I think more gold is required to keep me around much longer.
grace_messages[5] = 2317; // If my master wishest me near, payment would be needed!
grace_messages[6] = 2318; // My loyalty hath eroded, for lack of pay.
grace_messages[7] = 2319; // My term of service is ending, unless I be paid more.
grace_messages[8] = 2320; // 'Tis crass of me, but I want gold.
grace_messages[9] = 2321; // Methinks I shall quit my job soon.

// Dictionary messages sent when dismissed
finished_messages[0] = 2322; // Glad to have been of service.
finished_messages[1] = 2323; // Let me know if thee needeth aught else
finished_messages[2] = 2324; // Well, that's that, then. Farewell!

// Dictionary messages sent when hireling consumes enough gold in their pack to work for another day
payday_messages[0] = 2325; // Mmm. Payday! 'Tis my favourite day of the week.
payday_messages[1] = 2326; // Oh, happy day - 'tis payday!
payday_messages[2] = 2327; // Well, looks like I'll be sticking around for a while longer!

// Settings
const payPerDay = 90;
const payPerHour = 3;
const secPerUOMin = GetServerSetting( "SecondsPerUOMinute" );
const secPerUODay = ( secPerUOMin * 60 * 24 ); // amount of real life seconds that pass per in-game day
const gracePeriod = 60000; // Grace period in seconds after contract expires before hireling quits
const maxHireUODays = 3; // Maximum amount of in-game days that a hireling can be hired for

const maxFollowers = GetServerSetting( "MaxFollowers" ); // The maximum amount of followers/pets a character can have at a given time
const maxControlSlots = GetServerSetting( "MaxControlSlots" ); // The max amount of pet control slots a player can have active at a given time
const maxPetOwners = GetServerSetting( "MaxPetOwners" ); // The max amount of owners a pet/hireling is willing to accept orders from in its lifetime

function onSpeech( pSpeech, pChar, hireling )
{
	var pSock = pChar.socket;
	if( pSock == null || !ValidateObject( hireling ) || !pChar.InRange( hireling, 12 ))
		return false;

	var cliLang = pSock.language;
	var hirelingOwner = hireling.owner;

	// Ignore command if hireling's name is not included
	var allCmdFound = false;
	if( ValidateObject( hirelingOwner ) && pSpeech.toUpperCase().indexOf( hireling.name.toUpperCase() ) == -1 )
	{
		for( var trigWord = pSock.FirstTriggerWord(); !pSock.FinishedTriggerWords(); trigWord = pSock.NextTriggerWord() )
		{
			if(( trigWord >= 356 && trigWord <= 359 ) || trigWord == 363 || trigWord == 364 || trigWord == 368 || trigWord == 360 || trigWord == 361 )
			{
				allCmdFound = true;
			}
		}

		if( !allCmdFound )
			return false;
	}

	// If someone talks to the hireling and their previous master no longer exists, reset the hireling
	if( !ValidateObject( hirelingOwner ) && hireling.GetTag( "hired" ))
	{
		if( hireling.isHuman )
		{
			hireling.TextMessage( GetDictionaryEntry( 2328, cliLang )); // Hmmm. I seem to have lost my master.
		}
		ResetHireling( hireling );
	}

	// If the hire time has expired, reset hireling
	var hireExpire = hireling.GetTag( "hireExpire" );
	if( hireExpire < GetCurrentClock() && hireling.GetTag( "hired" ) && !hireling.GetTag( "gracePeriodActive" ))
	{
		if( ValidateObject( hirelingOwner ) && hirelingOwner.online )
		{
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2329, cliLang )); // Hmmm. Looks like our time together has come to an end!
			}

			var tempMsg = GetDictionaryEntry( 2330, cliLang ); // Your hireling - %s - has ceased working for you due to lack of pay!
			hirelingOwner.SysMessage( tempMsg.replace( /%s/gi, hireling.name ));
		}
		ResetHireling( hireling );
	}

	// Let's see if any commands were triggered - either through speech or context menus
	for( var trigWord = pSock.FirstTriggerWord(); !pSock.FinishedTriggerWords(); trigWord = pSock.NextTriggerWord() )
	{
		switch( trigWord )
		{
			case 357: // all follow
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// All hirelings (and pets) follows specified target
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 8;
					pChar.CustomTarget( 0, GetDictionaryEntry( 228, cliLang )); // Select player for the NPC to follow.
					return 1;
				}
				break;
			case 341: // <Name> come
			case 355: // <Name> follow me
				if( !allCmdFound && ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling follows owner
					hireling.Follow( pChar );
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetRandomString( pSock, accepted_messages ));
					}
					return 1;
				}
				break;
			case 356: // all come
			case 364: // all follow me
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// All pets come to owner
					var petList = hireling.owner.GetPetList();
					for( var i = 0; i < petList.length; i++ )
					{
						var tempPet = petList[i];
						if( ValidateObject( tempPet ) && tempPet.InRange( pChar, 12 ))
						{
							if( tempPet.isHuman )
							{
								tempPet.TextMessage( GetRandomString( pSock, accepted_messages ));
							}
							tempPet.Follow( pChar );
						}
						/*else if( ValidateObject( tempPet ))
						{
							// Commented out, since it would allow players to teleport their hirelings across the entire world!
							// Leave them somewhere nice and cozy.... then teleport them to you to do unexpected ambushes of other
							// players!
							tempPet.Teleport( pChar.x, pChar.y, pChar.z, pChar.worldnumber, pChar.instanceID );
						}*/
					}
					return 1;
				}
				break;
			case 346: // <Name> follow
				if( !allCmdFound && ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling follows specified target
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2331, cliLang )); // Who shall I follow?
					}
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 1;
					pChar.CustomTarget( 0, GetDictionaryEntry( 228, cliLang )); // Select player for the NPC to follow.
					return 1;
				}
				break;
			case 343: // <Name> fetch
			case 344: // <Name> get
			case 345: // <Name> bring
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling tries to fetch an object
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2332, cliLang )); // What shall I get for you?
					}
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 2;
					pChar.CustomTarget( 0, GetDictionaryEntry( 1316, cliLang )); // Click on the object to fetch.
					return 1;
				}
				break;
			case 342: // <Name> drop
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling drops all loot they've gathered to the ground
					DropLootOnGround( hireling, pSock );
					return 1;
				}
				break;
			case 347: // <Name> friend
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling treats targeted player as another owner.
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2333, cliLang )); // Who do you want to add as a friend?
					}
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 3;
					pChar.CustomTarget( 0, GetDictionaryEntry( 1620, cliLang )); // Who do you wish to befriend this creature to?
					return 1;
				}
				break;
			case 7:
			case 348: // <Name> guard
				if( !allCmdFound && ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling guards specified object
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2334, cliLang )); // Tell me what to guard.
					}
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 4;
					pChar.CustomTarget( 0, GetDictionaryEntry( 2302, cliLang ), 2 ); // Select target for your follower to guard:
					return 1;
				}
				break;
			case 409: // <Name> guard me
				if( !allCmdFound && ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling guards owner
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetRandomString( pSock, accepted_messages ));
					}
					hireling.Follow( hirelingOwner );
					hireling.guarding = hirelingOwner;
					hirelingOwner.isGuarded = true;
					var tempMsg = GetDictionaryEntry( 2374, cliLang ); // %s is now guarding you.
					pSock.SysMessage( tempMsg.replace( /%s/gi, hireling.name ));
					return 1;
				}
				break;
			case 358: // all guard
			case 363: // all guard me
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// All pets guard owner
					var petList = hireling.owner.GetPetList();
					for( var i = 0; i < petList.length; i++ )
					{
						var tempPet = petList[i];
						if( ValidateObject( tempPet ) && tempPet.InRange( pChar, 12 ))
						{
							if( tempPet.isHuman )
							{
								tempPet.TextMessage( GetRandomString( pSock, accepted_messages ));
							}

							tempPet.Follow( hirelingOwner );
							tempPet.guarding = hirelingOwner;
							hirelingOwner.isGuarded = true;
							var tempMsg = GetDictionaryEntry( 2374, cliLang ); // %s is now guarding you.
							pSock.SysMessage( tempMsg.replace( /%s/gi, tempPet.name ));
						}
					}
					return 1;
				}
				break;
			case 349: // <Name> kill
			case 350: // <Name> attack
				if( !allCmdFound && ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling attacks specific target
					var pRegion = pChar.region;
					if( pRegion && pRegion.isSafeZone )
					{
						pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
						return 0;
					}

					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2335, cliLang )); // Who should I attack?
					}
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 5;
					pChar.CustomTarget( 0, GetDictionaryEntry( 1313, cliLang ), 1 ); // Select the target to attack.
					return ( pSpeech != "" ? 1 : 2 );
				}
				break;
			case 360: // all kill
			case 361: // all attack
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling attacks specific target
					var pRegion = pChar.region;
					if( pRegion && pRegion.isSafeZone )
					{
						pSock.SysMessage( GetDictionaryEntry( 1799, pSock.language )); // Hostile actions are not permitted in this safe area.
						return 0;
					}

					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 9;
					pChar.CustomTarget( 0, GetDictionaryEntry( 1313, cliLang ), 1 ); // Select the target to attack.
					return ( pSpeech != "" ? 1 : 2 );
				}
				break;
			case 351: // <Name> patrol
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling patrols specified area
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2336, cliLang )); // Where should I patrol?
					}
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 6;
					pChar.CustomTarget( 0, GetDictionaryEntry( 2303, cliLang )); // Select location for your follower to patrol:
					return 1;
				}
				break;
			case 352: // <Name> report
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling reports their state of well-being
					// TODO - this should be based on loyalty? Or status of payment?
					if( hireExpire )
					{
						if( hireExpire > GetCurrentClock() )
						{
							if( hireling.isHuman )
							{
								var tempMsg = GetDictionaryEntry( 2337, cliLang ); // I will continue working for thee for an additional %i hours.
								hireling.TextMessage( tempMsg.replace( /%i/gi, Math.round(( hireExpire - GetCurrentClock() ) / 60 / 1000 )));
								pSock.SysMessage( GetDictionaryEntry( 2338, cliLang )); // Extra gold in a hireling's pack will be used to extend the contract once it expires.
							}
						}
						else
						{
							if( hireling.isHuman )
							{
								hireling.TextMessage( GetRandomString( pSock, grace_messages ));
							}
						}
					}
				}
				break;
			case 353: // <Name> stop
				if( !allCmdFound && ValidateObject( hireling.owner ) && hireling.owner == pChar )
				{
					// Stop patrolling
					if( hireling.GetTag( "pathMode" ) == "patrolling" ) // pathfind mode
					{
						hireling.SetTag( "patrolStop", true );
						pChar.SysMessage( GetDictionaryEntry( 2305, cliLang )); // Your pet stops patrolling.
					}

					// Stop fighting
					if( hireling.atWar )
					{
						hireling.SetTimer( Timer.PEACETIMER, 10000 ); // Prevent hireling from engaging in combat for 10 seconds
						hireling.target = null;
						hireling.atWar = false;
						hireling.attacker = null;
					}

					// Stop guarding
					var guardedByHireling = hireling.guarding;
					pChar.socket.SysMessage( guardedByHireling );
					if( ValidateObject( guardedByHireling ))
					{
						guardedByHireling.isGuarded = false;
						hireling.guarding = null;
						guardedByHireling.Refresh();
					}
					hireling.Follow( pChar );
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetRandomString( pSock, accepted_messages ));
					}
					return 1;
				}
				break;
			case 359: // all stop
				if( ValidateObject( hireling.owner ) && hireling.owner == pChar )
				{
					var petList = hireling.owner.GetPetList();
					for( var i = 0; i < petList.length; i++ )
					{
						var tempPet = petList[i];
						if( ValidateObject( tempPet ) && tempPet.InRange( pChar, 12 ))
						{
							// Stop patrolling
							if( tempPet.GetTag( "pathMode" ) == "patrolling" ) // pathfind mode
							{
								tempPet.SetTag( "patrolStop", true );
								pChar.SysMessage( GetDictionaryEntry( 2305, cliLang )); // Your pet stops patrolling.
							}

							// Stop fighting
							if( tempPet.atWar )
							{
								tempPet.SetTimer( Timer.PEACETIMER, 10000 ); // Prevent tempPet from engaging in combat for 10 seconds
								tempPet.target = null;
								tempPet.atWar = false;
								tempPet.attacker = null;
							}

							// Stop guarding
							var guardedBytempPet = tempPet.guarding;
							if( ValidateObject( guardedBytempPet ))
							{
								guardedBytempPet.isGuarded = false;
								tempPet.guarding = null;
								guardedBytempPet.Refresh();
							}
							tempPet.Follow( pChar );
							if( tempPet.isHuman )
							{
								tempPet.TextMessage( GetRandomString( pSock, accepted_messages ));
							}
						}
					}
					return 1;
				}
				break;
			case 12: // servant, serve, slave
			case 59: // greetings, hello, yo, hey, hail
			case 354: // hire
				if( !pChar.InRange( hireling, 3 ))
					break;

				if( !ValidateObject( hirelingOwner ))
				{
					// Hireling informs about the cost of hiring them
					var payRequired = CalculateHirelingPay( hireling );

					if( hireling.isHuman )
					{
						var tempMsg = GetDictionaryEntry( 2339, cliLang ); // I am available for hire for %i gold coins a day. If thou dost give me gold, I will work for thee.
						hireling.TextMessage( tempMsg.replace( /%i/gi, payRequired ));
					}

					// Turn the hireling towards the one talking
					hireling.TurnToward( pChar );

					// Let's pause the NPC in his tracks for X seconds so he doesn't wander off
					hireling.SetTimer( Timer.MOVETIME, 15000 );
				}
				else
				{
					// Hireling informs player they've already been hired
					if( hireling.owner = pChar )
					{
						if( hireling.isHuman )
						{
							hireling.TextMessage( GetDictionaryEntry( 2340, cliLang )); // Thou'rt already paying my wages, boss!
						}
					}
					else
					{
						if( hireling.isHuman )
						{
							hireling.TextMessage( GetRandomString( pSock, occupied_messages ));
						}
					}
				}
				break;
			case 373: // <Name> dismiss
				if( ValidateObject( hireling.owner ) && hireling.owner == pChar )
				{
					// Hireling is dismissed from service
					var objSerialFromCode = pChar.GetTag( "hirelingObj" );
					var objFromCode = CalcCharFromSer( objSerialFromCode );

					if( ValidateObject( objFromCode ) && objFromCode != hireling )
						return false; // Wrong hireling!

					if( hireling.isHuman )
					{
						hireling.TextMessage( GetRandomString( pSock, finished_messages ));
					}
					ResetHireling( hireling );
					return 1;
				}
				break;
			case 366: // <Name> transfer
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Ownership of hireling is transferred to another player
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2341, cliLang )); // Whom do you wish me to work for?
					}
					pSock.tempObj2 = hireling;
					pSock.tempInt2 = 7;
					pChar.CustomTarget( 0, GetDictionaryEntry( 2304, cliLang )); // Click on the person to transfer ownership to.
					return 1;
				}
				break;
			case 367: // <Name> stay
				if( !allCmdFound && ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					// Hireling stays put
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetRandomString( pSock, accepted_messages ));
					}
					hireling.oldwandertype = 0;
					hireling.wandertype = 0;
					return 1;
				}
				break;
			case 368: // all stay
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					var petList = hireling.owner.GetPetList();
					for( var i = 0; i < petList.length; i++ )
					{
						var tempPet = petList[i];
						if( ValidateObject( tempPet ) && tempPet.InRange( pChar, 12 ))
						{
							tempPet.oldwandertype = 0;
							tempPet.wandertype = 0;
						}
					}
					return 1;
				}
				break;
			case 157: // <Name> move
				if( ValidateObject( hirelingOwner ) && hirelingOwner == pChar )
				{
					if( hireling.atWar )
					{
						if( hireling.isHuman )
						{
							hireling.TextMessage( GetDictionaryEntry( 2373, cliLang )); // I am too busy fighting to deal with thee!
						}
						return 1;
					}

					// Hireling shuffles around a couple of steps to get out of the way
					if( hireling.InRange( pChar, 3 ))
					{
						var rnd = RandomNumber( -1, 0 );
						var rndXOffset = RandomNumber( 1, 2 ) * rnd;
						rnd = RandomNumber( -1, 0 );
						var rndYOffset = RandomNumber( 1, 2 ) * rnd;
						hireling.WalkTo( hireling.x + rndXOffset, hireling.y + rndYOffset, 5 );
					}
				}
				break;
			default:
				break;
		}
	}
	return false;
}

function ResetHireling( hireling )
{
	// Null out any tags that might have been used
	hireling.SetTag( "hired", null );
	hireling.SetTag( "pathMode", null );
	hireling.SetTag( "patrolling", null );
	hireling.SetTag( "gracePeriodActive", null );

	var hirelingOwner = hireling.owner;
	if( ValidateObject( hirelingOwner ))
	{
		// If owner still exists, reduce their controlSlotsUsed value by amount of slots hireling takes up
		hirelingOwner.controlSlotsUsed = Math.max( 0, hirelingOwner.controlSlotsUsed - hireling.controlSlots );
	}

	// Remove owner
	hireling.owner = null;

	// Stop guarding any objects
	var guardedObj = hireling.guarding;
	if( ValidateObject( guardedObj ))
	{
		guardedObj.isGuarded = false;
		guardedObj.Refresh();
		hireling.guarding = null;
	}

	// Clear friend list
	var friendList = hireling.GetFriendList();
	for( var i = 0; i < friendList.length; i++ )
	{
		hireling.RemoveFriend( friendList[i] );
	}
}

// Handle dropping of gold on NPC to hire them
function onDropItemOnNpc( pChar, hireling, iDropped )
{
	var hPack = hireling.pack;
	if( !ValidateObject( hPack )) // hireling has no backpack!
	{
		if( pChar.socket != null )
		{
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2342, pChar.socket.language) ); // I have no space to carry any items!
			}
		}
		return false;
	}

	var payRequired = CalculateHirelingPay( hireling );
	if( !ValidateObject( hireling.owner ))
	{
		// Hireling has no owner, so check if someone attempted to hire them
		if( iDropped.id != 0x0eed ) // gold
		{
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2343, pChar.socket.language )); // Not interested, sorry.
			}
			return false;
		}

		if( maxControlSlots > 0 && ( pChar.controlSlotsUsed + hireling.controlSlots > maxControlSlots ))
		{
			pChar.socket.SysMessage( GetDictionaryEntry( 2390, pChar.socket.language )); // That would exceed your maximum pet control slots.
			return false;
		}

		// Check to make sure the gold amount covers the hireling's wages for at least a day (of ingame time)
		if( iDropped.amount < payRequired )
		{
			if( hireling.isHuman )
			{
				var tempMsg = GetDictionaryEntry( 2339, pChar.socket.language ); // I am available for hire for %i gold coins a day. If thou dost give me gold, I will work for thee.
				hireling.TextMessage( tempMsg.replace( /%i/gi, payRequired ));
			}
			return false;
		}

		// Check to make sure player can still take on more followers
		if( pChar.petCount >= maxFollowers )
		{
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2345, pChar.socket.language )); // No thanks, you seem to already have enough followers!
			}

			var tempMsg = GetDictionaryEntry( 2346, pChar.socket.language ); // You can maximum have %i pets/followers active at the same time.
			pChar.socket.SysMessage( tempMsg.replace( /%i/gi, maxFollowers ));
			return false;
		}

		// Check to make sure hireling has space for more weight in backpack
		if(( hPack.weight + iDropped.weight ) > hPack.maxWeight )
		{
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2347, pChar.socket.language )); // I cannot carry any more!
			}
			return false;
		}

		// Deposit the remaining amount of money into the hireling's backpack
		iDropped.container = hireling.pack;

		// Calculate how many days to hire the hireling for
		var daysToHire = Math.floor( iDropped.amount / payRequired );
		if( daysToHire > maxHireUODays )
		{
			daysToHire = maxHireUODays;
		}

		var totalTimeHired = Math.round( secPerUODay * daysToHire * 1000 );
		var hireExpire = GetCurrentClock() + totalTimeHired;
		if( hireling.isHuman )
		{
			var tempMsg = GetDictionaryEntry( 2348, pChar.socket.language ); // I thank thee for paying me. I will work for thee for %i days.
			hireling.TextMessage( tempMsg.replace( /%i/gi, daysToHire ));

			pChar.socket.SysMessage( GetDictionaryEntry( 2338, pChar.socket.language )); // Extra gold in a hireling's pack will be used to extend the contract once it expires.
		}
		else
		{
			pChar.socket.SysMessage( 1318 ); // Your pet begins following you.
		}

		// Start a character timer (can we use custom timers?) for when next pay will be docked
		hireling.StartTimer( secPerUODay * 1000, 1, true );

		// Finalize hiring of the hireling, and consume the amount of gold matching the time they're hired for
		iDropped.amount -= ( payRequired * daysToHire );
		hireling.owner = pChar;
		hireling.SetTag( "hired", true );
		hireling.SetTag( "hireExpire", hireExpire );

		// Update control slots tracking
		pChar.controlSlotsUsed = pChar.controlSlotsUsed + hireling.controlSlots;

		if( hireling.isHuman )
		{
			var tempMsg = GetDictionaryEntry( 2349, pChar.socket.language ); // I am following %s.
			hireling.TextMessage( tempMsg.replace( /%s/gi, pChar.name ));
		}

		hireling.Follow( pChar );
		hireling.SetTimer( Timer.MOVETIME, 0 );
		return 2;
	}
	else
	{
		// Hireling already has an owner. Is this the owner trying to feed the hireling, or
		// to top up their pay?
		if( iDropped.id == 0x0eed ) // gold
		{
			// Validate that hireling has a backpack
			var hPack = hireling.pack;
			if( !ValidateObject( hPack ))
				return false;

			// Check that hireling's pack can hold the weight of the gold
			if(( hPack.weight + iDropped.weight ) > hPack.maxWeight )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 2347, pChar.socket.language )); // I cannot carry any more!
				}
				return false;
			}

			iDropped.container = hireling.pack;
			var totalGold = hireling.ResourceCount( 0x0eed, 0 );
			var daysToHire = Math.floor( totalGold / payRequired );
			if( daysToHire > maxHireUODays )
			{
				daysToHire = maxHireUODays;
			}

			if( hireling.isHuman )
			{
				var tempMsg = GetDictionaryEntry( 2344, pChar.socket.language ); // 2344=That's enough total gold to secure my services for another %i days!
				hireling.TextMessage( tempMsg.replace( /%i/gi, daysToHire ));
				pChar.socket.SysMessage( GetDictionaryEntry( 2338, pChar.socket.language )); // Extra gold in a hireling's pack will be used to extend the contract once it expires.
			}
		}
		else
		{
			var foodAccepted = iDropped.IsOnFoodList( hireling.foodList );
			if( hireling.hunger < 6 && foodAccepted )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 2350, pChar.socket.language )); // I thank thee! Much appreciated.
				}
				hireling.EmoteMessage( GetDictionaryEntry( 2351, pChar.socket.language )); // *eats food*
				hireling.hunger++;
				iDropped.Delete();
				return true;
			}
			else
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 2352, pChar.socket.language )); // I have no need for that.
				}
				return false;
			}
		}
	}
	return true;
}

function onTimer( hireling, timerID )
{
	if( !ValidateObject( hireling ))
		return;

	// Validate that owner still exists
	var hirelingOwner = hireling.owner;
	if( !ValidateObject( hirelingOwner ))
	{
		if( hireling.isHuman )
		{
			hireling.TextMessage( GetDictionaryEntry( 2328, 0 )); // Hmmm. I seem to have lost my master.
		}
		ResetHireling( hireling );
		return;
	}

	var payRequired = CalculateHirelingPay( hireling );

	if( timerID == 1 )
	{
		// Hire time has passed, and hireling will "consume" more gold from backpack to extend hire, if possible
		var totalGold = hireling.ResourceCount( 0x0eed, 0 );
		if( totalGold >= payRequired )
		{
			// Calculate new duration of the hire
			var daysToHire = Math.floor( totalGold / payRequired );
			if( daysToHire > maxHireUODays )
			{
				daysToHire = maxHireUODays;
			}

			var totalTimeHired = Math.round( secPerUODay * daysToHire * 1000 );
			var hireExpire = GetCurrentClock() + totalTimeHired;
			hireling.SetTag( "hireExpire", hireExpire );
			hireling.StartTimer( secPerUODay, 1, true );

			// Consume more gold!
			hireling.UseResource(( daysToHire * payRequired ), 0x0eed, 0 );
			if( hirelingOwner.socket != null )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetRandomString( hirelingOwner.socket, payday_messages ));
					var tempMsg = GetDictionaryEntry( 2348, hirelingOwner.socket.language ); // I thank thee for paying me. I will work for thee for %i days.
					hireling.TextMessage( tempMsg.replace( /%i/gi, daysToHire ));
				}
			}
		}
		else
		{
			// Not enough money in hireling's backpack! Start timer for grace period
			if( gracePeriod > 0 )
			{
				if( hirelingOwner.socket != null )
				{
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetRandomString( hirelingOwner.socket, grace_messages ));
					}
				}
				hireling.SetTag( "gracePeriodActive", true );
				hireling.StartTimer( 60000, 2, true ); // 60 second grace period
			}
		}
	}
	else if( timerID == 2 )
	{
		// Grace period is over, if there's still not enough money, hireling will quit
		var totalGold = hireling.ResourceCount( 0x0eed, 0 );
		if( totalGold < payRequired )
		{
			if( hirelingOwner.socket != null )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetRandomString( hirelingOwner.socket, finished_messages ));
				}
			}
			ResetHireling( hireling );
		}
	}
	else if( timerID == 3 )
	{
		// Check if hireling is too far away from owner, and return to owner if that's the case!
		if( !hireling.InRange( hirelingOwner, 24 ))
		{
			if( DistanceBetween( hireling, hirelingOwner ) < 48 )
			{
				hireling.SetTag( "pathMode", "returnToOwner" );
				hireling.RunTo( hirelingOwner, 75 );
			}
		}
	}
}

function CalculateHirelingPay( hireling )
{
	var hSkills = hireling.skills;
	var payPerDay = hSkills.anatomy + hSkills.tactics;
	payPerDay += hSkills.macefighting + hSkills.swordsmanship;
	payPerDay += hSkills.fencing + hSkills.archery;
	payPerDay += hSkills.magicresistance + hSkills.healing;
	payPerDay += hSkills.magery + hSkills.parry;
	payPerDay += ( hireling.strength * 10 );
	payPerDay += ( hireling.dexterity * 10 );
	payPerDay += ( hireling.intelligence * 10 );
	payPerDay /= 35;
	payPerDay += 1;
	return Math.round( payPerDay );
}

function onCallback0( socket, myTarget )
{
	if( socket == null )
		return;

	var cancelCheck = parseInt( socket.GetByte( 11 ));
	if( cancelCheck == 255 )
		return;

	var pChar = socket.currentChar;
	if( !ValidateObject( pChar ))
		return;

	var actionType = socket.tempInt2;
	socket.tempInt2 = 0;
	if( actionType != 6 && !ValidateObject( myTarget ))
	{
		socket.SysMessage( GetDictionaryEntry( 2353, socket.language )); // Invalid target.
	}

	switch( actionType )
	{
		case 1: // follow
		case 8: // all follow
			var allFollow = ( actionType == 8 );
			FollowTarget( socket, pChar, myTarget, allFollow );
			break;
		case 2: // fetch
			FetchItem( socket, pChar, myTarget );
			break;
		case 3: // friend
			AddFriend( socket, pChar, myTarget );
			break;
		case 4: // guard
			GuardObject( socket, pChar, myTarget );
			break;
		case 5: // attack
		case 9: // all attack
			var allAttack = ( actionType == 9 );
			AttackTarget( socket, pChar, myTarget, allAttack );
			break;
		case 6: // patrol
			PatrolArea( socket, pChar, myTarget );
			break;
		case 7: // transfer
			TransferOwnership( socket, pChar, myTarget );
			break;
		default:
			break;
	}
}

function FollowTarget( socket, pChar, myTarget, allFollow )
{
	if( !socket.GetWord( 1 ) && myTarget.isChar )
	{
		var hireling = socket.tempObj2;
		if( ValidateObject( hireling ) && hireling.owner == pChar )
		{
			if( allFollow )
			{
				var petList = pChar.GetPetList();
				for( var i = 0; i < petList.length; i++ )
				{
					var tempPet = petList[i];
					if( ValidateObject( tempPet ) && tempPet.InRange( pChar, 12 ))
					{
						if( !pChar.InRange( myTarget, 12 ))
						{
							socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
							return;
						}

						tempPet.Follow( myTarget );
						if( pChar.socket != null )
						{
							if( tempPet.isHuman )
							{
								tempPet.TextMessage( GetRandomString( pChar.socket, accepted_messages ));
							}
						}

						// Start a timer to check if tempPet is too far away from owner
						tempPet.StartTimer( 10000, 3, true );
					}
				}
				return;
			}

			if( !pChar.InRange( myTarget, 12 ))
			{
				socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
				return;
			}

			// Follow the specified target
			hireling.Follow( myTarget );
			if( pChar.socket != null )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetRandomString( pChar.socket, accepted_messages ));
				}
			}

			// Start a timer to check if hireling is too far away from owner
			hireling.StartTimer( 10000, 3, true );
		}
	}
}

function FetchItem( socket, pChar, myTarget )
{
	if( !socket.GetWord( 1 ) && myTarget.isItem )
	{
		var hireling = socket.tempObj2;
		if( ValidateObject( hireling ) && hireling.owner == pChar )
		{
			if( myTarget.worldnumber != hireling.worldnumber || myTarget.instanceID != hireling.instanceID )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 2354, socket.language )); // Impossible. That is out of this world!
				}
			}
			else if( myTarget.container != null )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 2355, socket.language )); // I cannot fetch items from inside a container.
				}
			}
			else if( myTarget.visible > 1 )
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 2356, socket.language )); // 2356=I cannot see that item!
				}
			}
			else if( !hireling.InRange( myTarget, 12 ))
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away!
				}
			}
			else
			{
				hireling.SetTag( "pathMode", "fetchItem" );
				hireling.SetTag( "itemToFetch", ( myTarget.serial ).toString() );
				hireling.RunTo( myTarget, 30 );
			}
		}
	}
}

function DropLootOnGround( hireling, pSock )
{
	var pack = hireling.pack;
	if( pack.itemsinside > 0 )
	{
		if( hireling.isHuman )
		{
			hireling.TextMessage( GetDictionaryEntry( 2357, pSock.language )); // Alright, this is all the loot I collected so far!
		}
		var lootItem;
		for( lootItem = pack.FirstItem(); !pack.FinishedItems(); lootItem = pack.NextItem() )
		{
			if( ValidateObject( lootItem ))
			{
				lootItem.container = null;
				lootItem.Teleport( hireling.x, hireling.y, hireling.z, hireling.worldnumber, hireling.instanceID );
				lootItem.decaytime = 0;
			}
		}
	}
	else
	{
		if( hireling.isHuman )
		{
			hireling.TextMessage( GetDictionaryEntry( 2358, pSock.language )); // I have not collected any loot!
		}
	}
}

function AddFriend( socket, pChar, myTarget )
{
	if( !socket.GetWord( 1 ) && myTarget.isChar )
	{
		var hireling = socket.tempObj2;
		if( ValidateObject( hireling ) && hireling.owner == pChar )
		{
			if( myTarget == pChar )
			{
				socket.SysMessage( GetDictionaryEntry( 2409, socket.language )); // That would be pointless - you are already their master!
				return;
			}

			if( myTarget.npc )
			{
				socket.SysMessage( GetDictionaryEntry( 2359, socket.language )); // Only players can be added to a follower's friend-list.
				return;
			}

			if( hireling.isDispellable )
			{
				socket.SysMessage( GetDictionaryEntry( 2385, socket.language )); // Summoned creatures are loyal only to their summoners.
				return;
			}

			if( hireling.AddFriend( myTarget ))
			{
				if( hireling.isHuman )
				{
					var tempMsg = GetDictionaryEntry( 2360, socket.language ); // I shall obey the orders given to me by %s and treat them as a friend.
					hireling.TextMessage( tempMsg.replace( /%s/gi, myTarget.name ));
				}

				tempMsg = GetDictionaryEntry( 2361, socket.language ); // %s will now accept commands from %t and will allow them to approach guarded locations.
				tempMsg = tempMsg.replace( /%s/gi, hireling.name );
				socket.SysMessage( tempMsg.replace( /%t/gi, myTarget.name ));

				tempMsg = GetDictionaryEntry( 2362, socket.language ); // %s has granted you the ability to give orders to their follower %t. They will now consider you as a friend.
				tempMsg = tempMsg.replace( /%s/gi, pChar.name );
				myTarget.socket.SysMessage( tempMsg.replace( /%t/gi, hireling.name ));
			}
			else
			{
				var friendList = hireling.GetFriendList();
				if( friendList.length >= 10 )
				{
					var tempMsg = GetDictionaryEntry( 2363, socket.language ); // Unable to add targeted player as friend of %s. Too many players on friend list already!
					socket.SysMessage( tempMsg.replace( /%s/gi, hireling.name ));
				}
			}
		}
	}
}

function GuardObject( socket, pChar, myTarget )
{
	if( !socket.GetWord( 1 ) && ( myTarget.isChar || myTarget.isItem ))
	{
		var hireling = socket.tempObj2;
		if( ValidateObject( hireling ) && hireling.owner == pChar )
		{
			if( !hireling.InRange( myTarget, 12 ))
			{
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
				}
				return;
			}

			var targRegion = myTarget.region;
			if( targRegion.isGuarded || targRegion.isSafeZone )
			{
				socket.SysMessage( GetDictionaryEntry( 2407, socket.language )); // Objects in this region cannot be guarded
			}
			else if( myTarget.npc && ( !myTarget.tamed || ( myTarget.owner && myTarget.owner != pChar )))
			{
				socket.SysMessage( GetDictionaryEntry( 2403, socket.language )); // Only your pets may be guarded.
				hireling.TextMessage( GetDictionaryEntry( 2405, socket.language )); // I cannot guard that, sorry!
			}
			else if( myTarget.container != null )
			{
				socket.SysMessage( GetDictionaryEntry( 2404, socket.language )); // Items in containers cannot be guarded.
				hireling.TextMessage( GetDictionaryEntry( 2405, socket.language )); // I cannot guard that, sorry!
			}
			else if( !hireling.CanSee( myTarget ))
			{
				hireling.TextMessage( GetDictionaryEntry( 2365, socket.language )); // I'm unable to reach that, sorry!
			}
			else if( myTarget.multi && myTarget.multi.owner && myTarget.multi.owner != hireling.owner )
			{
				socket.SysMessage( GetDictionaryEntry( 2406, socket.language )); // Items in other people's houses or ships cannot be guarded.
				hireling.TextMessage( GetDictionaryEntry( 2405, socket.language )); // I cannot guard that, sorry!
			}
			else
			{
				// Clear any existing guarded object
				var guardedObj = hireling.guarding;
				if( ValidateObject( guardedObj ))
				{
					guardedObj.isGuarded = false;
					hireling.guarding = null;
				}

				// Set new guarded object
				myTarget.isGuarded = true;
				hireling.guarding = myTarget;
				if( hireling.isHuman )
				{
					hireling.TextMessage( GetDictionaryEntry( 2364, socket.language )); // I will guard that with my life!
				}

				// Refresh if item
				myTarget.Refresh();

				hireling.SetTag( "pathMode", "guarding" );
				if( myTarget.isChar )
				{
					hireling.Follow( myTarget );
				}
				else
				{
					hireling.WalkTo( myTarget, 30 );
				}
			}
		}
	}
}

function AttackTarget( socket, pChar, myTarget, allAttack )
{
	if( !socket.GetWord( 1 ) && myTarget.isChar )
	{
		var hireling = socket.tempObj2;
		if( ValidateObject( hireling ) && hireling.owner == pChar )
		{
			if( hireling == myTarget )
			{
				socket.SysMessage( GetDictionaryEntry( 2387, socket.language )); // Your hireling cannot attack themselves!
				return;
			}

			var hRegion = hireling.region;
			var tRegion = myTarget.region;
			if(( hRegion && hRegion.isSafeZone ) || ( tRegion && tRegion.isSafeZone ))
			{
				// Hireling and/or target is in a safe zone where no aggressive actions can be taken, disallow
				socket.SysMessage( GetDictionaryEntry( 1799, socket.language )); // Hostile actions are not permitted in this safe area.
				return;
			}

			if( allAttack )
			{
				var petList = pChar.GetPetList();
				for( var i = 0; i < petList.length; i++ )
				{
					var tempPet = petList[i];
					if( ValidateObject( tempPet ) && tempPet.InRange( pChar, 12 ))
					{
						if( !pChar.InRange( myTarget, 12 ))
						{
							socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
							continue;
						}

						if( !pChar.criminal && myTarget != pChar && myTarget.innocent )
						{
							if( WillResultInCriminal( pChar, myTarget ))
							{
								pChar.criminal = true;
							}
						}

						if( tempPet.InitiateCombat( myTarget ))
						{
							if( WillResultInCriminal( pChar, myTarget ))
							{
								tempPet.criminal = true;
							}
						}
					}
				}
				return;
			}

			if( !pChar.InRange( myTarget, 12 ))
			{
				socket.SysMessage( GetDictionaryEntry( 393, socket.language )); // That is too far away.
				return;
			}

			if( hireling.InitiateCombat( myTarget ))
			{
				if( WillResultInCriminal( pChar, myTarget ))
				{
					hireling.criminal = true;
				}
			}
		}
	}
}

function PatrolArea( socket, pChar, myTarget )
{
	var hireling = socket.tempObj2;
	if( ValidateObject( hireling ) && hireling.owner == pChar )
	{
		hireling.SetTag( "pathMode", "patrolling" );
		var targX = socket.GetWord( 11 );
		var targY = socket.GetWord( 13 );
		var targZ = socket.GetSByte( 16 );
		hireling.SetTag( "origPosX", hireling.x );
		hireling.SetTag( "origPosY", hireling.y );
		hireling.SetTag( "origPosZ", hireling.z );
		hireling.WalkTo( targX, targY, targZ );
	}
}

function onPathfindEnd( hireling, pathfindResult )
{
	if( ValidateObject( hireling ))
	{
		var pathMode = hireling.GetTag( "pathMode" );
		if( pathMode == "patrolling" && hireling.GetTag( "patrolStop" ))
		{
			hireling.SetTag( "pathMode", null );
			hireling.SetTag( "patrolStop", null );
			hireling.SetTag( "origPosX", null );
			hireling.SetTag( "origPosY", null );
			hireling.SetTag( "origPosZ", null );
			return;
		}

		var hirelingOwner = hireling.owner;
		switch( pathfindResult )
		{
			case -1: // Pathfinding failed
				hireling.SetTag( "pathMode", null );
				if( ValidateObject( hirelingOwner ) && hirelingOwner.socket != null )
				{
					hireling.Follow( hirelingOwner );
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2365, hirelingOwner.socket.language )); // I'm unable to reach that, sorry!
					}
				}
				break;
			case 0: // Pathfinding partially succeeded, but didn't make it all the way to target destination
				hireling.SetTag( "pathMode", null );
				if( ValidateObject( hirelingOwner ) && hirelingOwner.socket != null )
				{
					hireling.Follow( hirelingOwner );
					if( hireling.isHuman )
					{
						hireling.TextMessage( GetDictionaryEntry( 2366, hirelingOwner.socket.language )); // Argh, something is blocking my way!
					}
				}
				break;
			case 1: // Reached end of the path
				if( pathMode == "fetchItem" )
				{
					var itemToFetchSerial = hireling.GetTag( "itemToFetch" );
					var itemToFetch = CalcItemFromSer( itemToFetchSerial );
					if( ValidateObject( itemToFetch ) && hirelingOwner.socket != null )
					{
						if( itemToFetch.weight >= 100 )
						{
							if( hireling.isHuman )
							{
								hireling.TextMessage( GetDictionaryEntry( 2367, hirelingOwner.socket.language )); // Nnngh! The item is too heavy!
							}
						}
						else if( itemToFetch.movable >= 2 )
						{
							if( hireling.isHuman )
							{
								hireling.TextMessage( GetDictionaryEntry( 2368, hirelingOwner.socket.language )); // Gggnn... It won't budge!
							}
						}
						else if( !hireling.InRange( itemToFetch, 12 ))
						{
							if( hireling.isHuman )
							{
								hireling.TextMessage( GetDictionaryEntry( 393, hirelingOwner.socket.language )); // That is too far away.
							}
						}
						else if( ValidateObject( hireling.pack ))
						{
							hireling.EmoteMessage( "Yoink!" );
							itemToFetch.container = hireling.pack;
							itemToFetch.Refresh();
						}

						if( ValidateObject( hirelingOwner ))
						{
							hireling.Follow( hirelingOwner );
						}
						hireling.SetTag( "pathMode", null );
						hireling.SetTag( "itemToFetch", null );
					}
				}
				else if( pathMode == "patrolling" )
				{
					// Continue patrolling
					var targX = hireling.GetTag( "origPosX" );
					var targY = hireling.GetTag( "origPosY" );
					var targZ = hireling.GetTag( "origPosY" );
					hireling.SetTag( "origPosX", hireling.x );
					hireling.SetTag( "origPosY", hireling.y );
					hireling.SetTag( "origPosZ", hireling.z );
					hireling.WalkTo( targX, targY, targZ );
				}
				else if( pathMode == "guarding" )
				{
					// Stay at object being guarded
					hireling.SetTag( "pathMode", null );
					hireling.Follow( null );
				}
				break;
			default:
				hireling.SetTag( "pathMode", null );
				break;
		}

		if( pathMode == "returnToOwner" )
		{
			if( ValidateObject( hirelingOwner ))
			{
				hireling.SetTag( "pathMode", null );
				hireling.Follow( hirelingOwner );
			}
		}
	}
}

function inRange( hireling, objInRange )
{
	if( ValidateObject( hireling.owner ) && hireling.owner.socket != null )
	{
		// Send these TextMessages only to hireling owner
		if( objInRange.murderer )
		{
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2369, hireling.owner.socket.language ), false, 0, 0, hireling.owner.serial ); // Look out, there's evil afoot!
			}
		}
		else if( objInRange.criminal )
		{
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2370, hireling.owner.socket.language ), false, 0, 0, hireling.owner.serial ); // Beware, unsavory figures are skulking about!
			}
		}
	}
}

function TransferOwnership( socket, pChar, myTarget )
{
	if( !socket.GetWord( 1 ) && myTarget.isChar )
	{
		var hireling = socket.tempObj2;
		if( ValidateObject( hireling ) && hireling.owner == pChar )
		{
			if( myTarget == pChar )
			{
				socket.SysMessage( GetDictionaryEntry( 2409, socket.language )); // That would be pointless - you are already their master!
				return;
			}

			if( myTarget.npc )
			{
				socket.SysMessage( GetDictionaryEntry( 2371, socket.language )); // Ownership can only be transferred to other players.
				return;
			}

			if( hireling.isDispellable )
			{
				socket.SysMessage( GetDictionaryEntry( 2386, socket.language )); // You cannot transfer ownership of a summoned creature.
				return;
			}

			if( myTarget.dead )
			{
				socket.SysMessage( GetDictionaryEntry( 2384, socket.language )); // You cannot transfer to someone that is dead.
				return;
			}

			if( pChar.criminal )
			{
				socket.SysMessage( GetDictionaryEntry( 2375, socket.language )); // The pet refuses to be transferred because it will not obey you sufficiently.
				if( myTarget.online && myTarget.socket != null )
				{
					var tempMsg = GetDictionaryEntry( 2378, myTarget.socket.language ); // The pet will not accept you as a master because it does not trust %s.
					myTarget.socket.SysMessage( tempMsg.replace( /%s/gi, pChar.name ));
				}
				return;
			}

			if( myTarget.criminal )
			{
				var tempMsg = GetDictionaryEntry( 2376, socket.language ); // The pet refuses to be transferred because it will not obey %s.
				socket.SysMessage( tempMsg.replace( /%s/gi, myTarget.name ));
				if( myTarget.online && myTarget.socket != null )
				{
					myTarget.SysMessage( GetDictionaryEntry( 2377, myTarget.socket.language )); // The pet will not accept you as a master because it does not trust you.
				}
				return;
			}

			if( maxControlSlots > 0 && ( myTarget.controlSlotsUsed + hireling.controlSlots > maxControlSlots ))
			{
				pChar.socket.SysMessage( GetDictionaryEntry( 2391, pChar.socket.language )); // That would exceed the other player's maximum pet control slots.
				if( myTarget.online && myTarget.socket != null )
				{
					myTarget.socket.SysMessage( GetDictionaryEntry( 2390, myTarget.socket.language )); // That would exceed your maximum pet control slots.
				}
				return false;
			}

			var hasBeenOwner = hireling.HasBeenOwner( myTarget );
			if( !hasBeenOwner && maxPetOwners > 0 && ( hireling.ownerCount >= maxPetOwners ))
			{
				pChar.socket.SysMessage( GetDictionaryEntry( 2401, pChar.socket.language )); // The hireling has had too many masters and is not willing to do the bidding of another one!
				if( myTarget.online && myTarget.socket != null )
				{
					myTarget.socket.SysMessage( GetDictionaryEntry( 2401, myTarget.socket.language )); // The hireling has had too many masters and is not willing to do the bidding of another one!
				}
				return false;
			}

			// If hireling is guarding something, stop guarding that something
			if( ValidateObject( hireling.guarding ))
			{
				var guardedObj = hireling.guarding;
				hireling.guarding = null;
				guardedObj.isGuarded = false;
			}

			// Clear old friend list
			hireling.ClearFriendList();

			// Transfer the hireling!
			hireling.owner = myTarget;
			if( hireling.isHuman )
			{
				hireling.TextMessage( GetDictionaryEntry( 2372, socket.language )); // Very well, I transfer my allegiance.
			}
			hireling.Follow( myTarget );

			// Update control slot tracking for both characters involved
			pChar.controlSlotsUsed = Math.max( 0, pChar.controlSlotsUsed - hireling.controlSlots );
			myTarget.controlSlotsUsed = pChar.controlSlotsUsed + hireling.controlSlots;
		}
	}
}

function GetRandomString( socket, string_array )
{
	//returns random string from given array
	return GetDictionaryEntry( string_array[RandomInt( string_array.length - 1 )], socket.language );
	//return string_array[RandomInt(string_array.length - 1)];
}

function RandomInt( rvalue )
{
	//returns random integer with maximum given
	return Math.floor( Math.random() * rvalue );
}

