// =============================================================================
// factions_election.js
// UOX3 Faction System - Election Engine
//
// Drop in:  /js/factions/factions_election.js
// Register: jse_fileassociations.scp  [SCRIPT_LIST]  -> ID e.g. 5002
//
// Elections run on a 7-day cycle.  The last 3 days are open for voting.
// State is persisted on the faction controller item via tags.
//
// Tag layout on controller item:
//   elec_<fkey>_state        : "none" | "running" | "voting" | "finished"
//   elec_<fkey>_start        : epoch ms when current cycle started
//   elec_<fkey>_candidates   : comma-separated serials of candidates
//   elec_<fkey>_votes_<serial>: vote count for that candidate
// =============================================================================

var FactionElectionTimerId = 1;
var FactionElectionTimerDelay = 3600000; // 1 hour
var FactionElectionController = null;
var FactionElectionPlayerDataScriptId = 8513;

var PENDING_MS         = 432000000; // 5 days between elections
var CAMPAIGN_MS        = 86400000;  // 1 day to declare candidacy
var VOTING_MS          = 259200000; // 3 days to vote
var ELECTION_MAX_CANDIDATES = 10;
var ELECTION_CANDIDATE_RANK = 4; // RunUO rank 5, stored as zero-based rank 4

var ELEC_STATE_NONE     = "none";
var ELEC_STATE_PENDING  = "pending";
var ELEC_STATE_RUNNING  = "running";
var ELEC_STATE_VOTING   = "voting";
var ELEC_STATE_FINISHED = "finished";

// ---------------------------------------------------------------------------
// HELPERS
// ---------------------------------------------------------------------------

function _GetCtrl()
{
	if( ValidateObject( FactionElectionController ) )
		return FactionElectionController;

	FactionElectionController = null;
	IterateOver( "ITEM" );
	return FactionElectionController;
}

function _EnsureTimer( ctrl )
{
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.KillJSTimer( FactionElectionTimerId, 8508 );
	ctrl.StartTimer( FactionElectionTimerDelay, FactionElectionTimerId, 8508 );
	return true;
}

function onIterate( toCheck )
{
	if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
	{
		FactionElectionController = toCheck;
		return true;
	}

	return false;
}

function onCreateDFN( objMade, objType )
{
	if( ValidateObject( objMade ) && objMade.isItem && objMade.GetTag( "faction_controller" ) == 1 )
	{
		FactionElectionController = objMade;
		_EnsureTimer( objMade );
	}
}

function onTimer( timerObj, timerID )
{
	if( timerID != FactionElectionTimerId )
		return;
	if( !ValidateObject( timerObj ) || timerObj.GetTag( "faction_controller" ) != 1 )
		return;

	FactionElectionController = timerObj;
	CheckElectionTimers();
	_EnsureTimer( timerObj );
}

function RegisterController( ctrl )
{
	if( !ValidateObject( ctrl ) || ctrl.GetTag( "faction_controller" ) != 1 )
		return false;

	FactionElectionController = ctrl;
	return _EnsureTimer( ctrl );
}

function _GetElecState( fkey )
{
	var ctrl = _GetCtrl();
	if( !ctrl ) return ELEC_STATE_NONE;
	return ctrl.GetTag( "elec_" + fkey + "_state" ) || ELEC_STATE_NONE;
}

function _SetElecState( fkey, state )
{
	var ctrl = _GetCtrl();
	if( ctrl ) ctrl.SetTag( "elec_" + fkey + "_state", state );
}

function _GetElecStart( fkey )
{
	var ctrl = _GetCtrl();
	if( !ctrl ) return 0;
	return parseInt( ctrl.GetTag( "elec_" + fkey + "_start" ), 10 ) || 0;
}

function _SetElecStart( fkey, time )
{
	var ctrl = _GetCtrl();
	if( ctrl ) ctrl.SetTag( "elec_" + fkey + "_start", time );
}

function _GetCandidates( fkey )
{
	var ctrl = _GetCtrl();
	if( !ctrl ) return [];
	var raw = ctrl.GetTag( "elec_" + fkey + "_candidates" );
	if( !raw || raw.length === 0 ) return [];
	return raw.split( "," );
}

function _AddCandidate( fkey, serial )
{
	var ctrl = _GetCtrl();
	if( !ctrl ) return false;
	var list = _GetCandidates( fkey );
	// Avoid duplicates
	for( var i = 0; i < list.length; i++ )
		if( list[i] === String( serial ) ) return false;
	if( list.length >= ELECTION_MAX_CANDIDATES )
		return false;
	list.push( String( serial ) );
	ctrl.SetTag( "elec_" + fkey + "_candidates", list.join( "," ) );
	// init vote count
	ctrl.SetTag( "elec_" + fkey + "_votes_" + serial, 0 );
	return true;
}

function _GetVotes( fkey, serial )
{
	var ctrl = _GetCtrl();
	if( !ctrl ) return 0;
	return parseInt( ctrl.GetTag( "elec_" + fkey + "_votes_" + serial ), 10 ) || 0;
}

function _AddVote( fkey, candidateSerial )
{
	var ctrl = _GetCtrl();
	if( !ctrl ) return;
	var v = _GetVotes( fkey, candidateSerial );
	ctrl.SetTag( "elec_" + fkey + "_votes_" + candidateSerial, v + 1 );
}

function _HasVoted( pChar, fkey )
{
	var ctrl = _GetCtrl();
	if( !ValidateObject( ctrl ) ) return false;
	var voters = String( ctrl.GetTag( "elec_" + fkey + "_voters" ) || "" ).split( "," );
	for( var voterIndex = 0; voterIndex < voters.length; voterIndex++ )
		if( voters[voterIndex] === String( pChar.serial ) ) return true;
	return false;
}

function _MarkVoted( pChar, fkey )
{
	var ctrl = _GetCtrl();
	if( !ValidateObject( ctrl ) ) return false;
	var tagName = "elec_" + fkey + "_voters";
	var voters = String( ctrl.GetTag( tagName ) || "" );
	ctrl.SetTag( tagName, voters === "" ? String( pChar.serial ) : voters + "," + pChar.serial );
	return true;
}

function _ClearVotedFlags( fkey )
{
	var ctrl = _GetCtrl();
	if( ValidateObject( ctrl ) ) ctrl.SetTag( "elec_" + fkey + "_voters", "" );
}

// ---------------------------------------------------------------------------
// START ELECTION
// ---------------------------------------------------------------------------
function StartElection( fkey )
{
	if( _GetElecState( fkey ) !== ELEC_STATE_NONE &&
	    _GetElecState( fkey ) !== ELEC_STATE_FINISHED )
	{
		return false; // already in progress
	}

	var ctrl = _GetCtrl();
	if( !ctrl ) return false;

	_SetElecState( fkey, ELEC_STATE_PENDING );
	_SetElecStart( fkey, GetCurrentClock() );
	ctrl.SetTag( "elec_" + fkey + "_candidates", "" );
	_ClearVotedFlags( fkey );

	// Broadcast
	_BroadcastFaction( fkey, "The next faction commander campaign begins in five days." );
	return true;
}

// ---------------------------------------------------------------------------
// DECLARE CANDIDACY
// ---------------------------------------------------------------------------
function DeclareCandidacy( pChar )
{
	if( !ValidateObject( pChar ) ) return false;
	var factionData = TriggerEvent( FactionElectionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	var fkey = factionData.faction;
	if( !fkey || fkey.length === 0 )
	{
		pChar.SysMessage( "You must be in a faction to run for Commander." );
		return false;
	}

	var state = _GetElecState( fkey );
	if( state !== ELEC_STATE_RUNNING )
	{
		pChar.SysMessage( "There is no active candidacy period for your faction right now." );
		return false;
	}

	if( factionData.rank < ELECTION_CANDIDATE_RANK )
	{
		pChar.SysMessage( "You must hold at least faction rank 5 to run for Commander." );
		return false;
	}

	if( !_AddCandidate( fkey, pChar.serial ) )
	{
		pChar.SysMessage( "You are already a candidate, or the election has reached its ten-candidate limit." );
		return false;
	}
	pChar.SysMessage( "You have declared your candidacy for Commander!" );
	_BroadcastFaction( fkey, pChar.name + " has declared candidacy for Commander!" );
	return true;
}

// ---------------------------------------------------------------------------
// BEGIN VOTING PHASE  (called by tick/timer or GM)
// ---------------------------------------------------------------------------
function BeginVoting( fkey )
{
	if( _GetElecState( fkey ) !== ELEC_STATE_RUNNING ) return false;
	_SetElecState( fkey, ELEC_STATE_VOTING );
	_BroadcastFaction( fkey, "Voting has begun! Visit a Faction Stone to cast your vote for Commander!" );
	return true;
}

function BeginCampaign( fkey )
{
	if( _GetElecState( fkey ) !== ELEC_STATE_PENDING ) return false;
	_SetElecState( fkey, ELEC_STATE_RUNNING );
	_SetElecStart( fkey, GetCurrentClock() );
	_BroadcastFaction( fkey, "Campaigning has begun for Faction Commander." );
	return true;
}

// ---------------------------------------------------------------------------
// CAST VOTE
// ---------------------------------------------------------------------------
function CastVote( pVoter, candidateSerial )
{
	if( !ValidateObject( pVoter ) ) return false;
	var fkey = TriggerEvent( FactionElectionPlayerDataScriptId, "GetFactionValue", pVoter, "faction", pVoter.GetTag( "faction" ) );
	if( !fkey || fkey.length === 0 )
	{
		pVoter.SysMessage( "You must be in a faction to vote." );
		return false;
	}

	if( _GetElecState( fkey ) !== ELEC_STATE_VOTING )
	{
		pVoter.SysMessage( "Voting is not currently open in your faction." );
		return false;
	}

	if( _HasVoted( pVoter, fkey ) )
	{
		pVoter.SysMessage( "You have already cast your vote in this election." );
		return false;
	}

	// Validate candidate is in list
	var candidates = _GetCandidates( fkey );
	var found      = false;
	for( var i = 0; i < candidates.length; i++ )
	{
		if( candidates[i] === String( candidateSerial ) ) { found = true; break; }
	}
	if( !found )
	{
		pVoter.SysMessage( "That character is not a candidate." );
		return false;
	}

	_AddVote( fkey, candidateSerial );
	_MarkVoted( pVoter, fkey );
	pVoter.SysMessage( "Your vote has been cast!" );
	return true;
}

// ---------------------------------------------------------------------------
// CONCLUDE ELECTION
// ---------------------------------------------------------------------------
function ConcludeElection( fkey )
{
	if( _GetElecState( fkey ) !== ELEC_STATE_VOTING ) return false;

	var candidates = _GetCandidates( fkey );
	var ctrl = _GetCtrl();
	if( candidates.length === 0 )
	{
		_SetElecState( fkey, ELEC_STATE_FINISHED );
		StartElection( fkey );
		_BroadcastFaction( fkey, "The election ended with no candidates. Faction leadership has not changed." );
		return false;
	}

	// Find winner
	var winnerSerial = -1;
	var winnerVotes  = -1;
	for( var i = 0; i < candidates.length; i++ )
	{
		var v = _GetVotes( fkey, candidates[i] );
		if( v > winnerVotes ) { winnerVotes = v; winnerSerial = candidates[i]; }
	}

	// Promote winner
	var winner = CalcCharFromSer( parseInt( winnerSerial, 10 ) );
	if( ValidateObject( winner ) )
	{
		var winnerData = TriggerEvent( FactionElectionPlayerDataScriptId, "ReadFactionPlayerData", winner );
		if( winnerData.faction !== fkey )
		{
			_BroadcastFaction( fkey, "The winning candidate is no longer eligible. Faction leadership has not changed." );
			_SetElecState( fkey, ELEC_STATE_FINISHED );
			StartElection( fkey );
			return false;
		}
		_ClearCommanderRole( fkey, ctrl );
		winnerData.commander = true;
		winnerData.role = "commander";
		winnerData.roleFaction = fkey;
		winnerData.roleSetAt = GetCurrentClock();
		TriggerEvent( FactionElectionPlayerDataScriptId, "WriteFactionPlayerData", winner, winnerData );
		if( ctrl ) ctrl.SetTag( "cmd_" + fkey, winnerSerial );
		_BroadcastFaction( fkey, winner.name + " has been elected as the new Commander of the " + fkey + "!" );
	}
	else
	{
		_BroadcastFaction( fkey, "No eligible election winner could be found. Faction leadership has not changed." );
	}

	_SetElecState( fkey, ELEC_STATE_FINISHED );
	StartElection( fkey );
	return true;
}

function _ClearCommanderRole( fkey, ctrl )
{
	if( !ValidateObject( ctrl ) )
		return false;

	var oldSerial = parseInt( ctrl.GetTag( "cmd_" + fkey ), 10 );
	if( oldSerial > 0 )
	{
		var oldCmd = CalcCharFromSer( oldSerial );
		if( ValidateObject( oldCmd ) )
		{
			var oldData = TriggerEvent( FactionElectionPlayerDataScriptId, "ReadFactionPlayerData", oldCmd );
			oldData.commander = false;
			if( oldData.role === "commander" && oldData.roleFaction === fkey )
			{
				oldData.role = "";
				oldData.roleFaction = "";
				oldData.roleSetAt = 0;
			}
			TriggerEvent( FactionElectionPlayerDataScriptId, "WriteFactionPlayerData", oldCmd, oldData );
		}
	}

	ctrl.SetTag( "cmd_" + fkey, 0 );
	return true;
}

// ---------------------------------------------------------------------------
// ELECTION TIMER CHECK (called from onTimer or periodically by a global script)
// ---------------------------------------------------------------------------
function CheckElectionTimers()
{
	var FACTION_KEYS = [ "TB", "COM", "MIN", "SL" ];
	var now          = GetCurrentClock();

	for( var i = 0; i < FACTION_KEYS.length; i++ )
	{
		var fkey  = FACTION_KEYS[i];
		var state = _GetElecState( fkey );
		var start = _GetElecStart( fkey );

		if( state === ELEC_STATE_NONE || state === ELEC_STATE_FINISHED )
		{
			StartElection( fkey );
		}
		else if( state === ELEC_STATE_PENDING )
		{
			if( ( now - start ) >= PENDING_MS ) BeginCampaign( fkey );
		}
		else if( state === ELEC_STATE_RUNNING )
		{
			// Switch to voting after campaign period
			if( ( now - start ) >= CAMPAIGN_MS )
			{
				var candidates = _GetCandidates( fkey );
				if( candidates.length === 0 )
				{
					_SetElecState( fkey, ELEC_STATE_FINISHED );
					StartElection( fkey );
				}
				else if( candidates.length === 1 )
				{
					_SetElecState( fkey, ELEC_STATE_VOTING );
					ConcludeElection( fkey );
				}
				else
				{
					BeginVoting( fkey );
					_SetElecStart( fkey, now );
				}
			}
		}
		else if( state === ELEC_STATE_VOTING )
		{
			// Conclude after voting period
			if( ( now - start ) >= VOTING_MS )
			{
				ConcludeElection( fkey );
			}
		}
	}
}

// ---------------------------------------------------------------------------
// GUMP - Faction Stone election menu
// ---------------------------------------------------------------------------
function ShowElectionGump( pSock, pChar, fkey )
{
	var state      = _GetElecState( fkey );
	var candidates = _GetCandidates( fkey );

	var myGump = new Gump();
	myGump.AddBackground( 0, 0, 400, 340, 9200 );
	myGump.AddHTMLGump( 10, 10, 380, 25, 0, 0, "<CENTER><b>Faction Election</b></CENTER>" );

	myGump.AddHTMLGump( 10, 40, 380, 20, 0, 0, "State: <b>" + state + "</b>" );

	if( state === ELEC_STATE_RUNNING )
	{
		myGump.AddButton( 10, 70, 0xFA5, 0xFA7, 1, 0, 1 );
		myGump.AddHTMLGump( 50, 70, 300, 20, 0, 0, "Declare Candidacy" );
	}
	else if( state === ELEC_STATE_VOTING )
	{
		myGump.AddHTMLGump( 10, 65, 380, 20, 0, 0, "<b>Candidates:</b>" );
		var yPos = 90;
		for( var i = 0; i < candidates.length; i++ )
		{
			var cSerial = parseInt( candidates[i], 10 );
			var cChar   = CalcCharFromSer( cSerial );
			var cName   = ValidateObject( cChar ) ? cChar.name : "Unknown (" + cSerial + ")";
			var votes   = _GetVotes( fkey, cSerial );

			myGump.AddButton( 10, yPos, 0xFA5, 0xFA7, 1, 0, 100 + i );
			myGump.AddHTMLGump( 50, yPos, 280, 20, 0, 0, cName + " - " + votes + " vote(s)" );
			yPos += 30;
		}
	}
	else
	{
		myGump.AddHTMLGump( 10, 70, 380, 40, 0, 0, "No election is currently active or accepting votes." );
	}

	myGump.AddButton( 10, 310, 0xFA5, 0xFA7, 1, 0, 0 );
	myGump.AddHTMLGump( 50, 310, 100, 20, 0, 0, "Close" );

	myGump.Send( pSock );
	myGump.Free();
}

function onGumpPress( pSock, buttonID, gumpData )
{
	var pChar = pSock.currentChar;
	if( !ValidateObject( pChar ) )
		return;

	var fkey = TriggerEvent( FactionElectionPlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
	if( !fkey ) return;

	if( buttonID === 0 ) return; // close

	if( buttonID === 1 )
	{
		// Declare candidacy
		DeclareCandidacy( pChar );
		ShowElectionGump( pSock, pChar, fkey );
	}
	else if( buttonID >= 100 )
	{
		// Vote for candidate at index (buttonID - 100)
		var candidates = _GetCandidates( fkey );
		var idx        = buttonID - 100;
		if( idx < candidates.length )
		{
			CastVote( pChar, parseInt( candidates[idx], 10 ) );
		}
		ShowElectionGump( pSock, pChar, fkey );
	}
}

// ---------------------------------------------------------------------------
// INTERNAL broadcast helper (mirrors factions_core.js version)
// ---------------------------------------------------------------------------
function _BroadcastFaction( fkey, msg )
{
	// Iterate online players — UOX3 doesn't expose a global player list in JS,
	// so we use SysMessage on the character if we can find them.
	// For a proper broadcast, wire this through GetOnlinePlayers() if your
	// UOX3 build supports it, or use Console.Print as a fallback.
	Console.Print( "[FACTION:" + fkey + "] " + msg );
}


function ShowElectionStatus( pSock, factionKey )
{
	if( !pSock )
		return false;

	pSock.SysMessage( "Election state for " + factionKey + ": " + _GetElecState( factionKey ) );
	var candidates = _GetCandidates( factionKey );
	for( var candidateIndex = 0; candidateIndex < candidates.length; candidateIndex++ )
	{
		var candidateSerial = Number( candidates[candidateIndex] );
		var candidate = CalcCharFromSer( candidateSerial );
		var candidateName = ValidateObject( candidate ) ? candidate.name : "Serial " + candidateSerial;
		pSock.SysMessage( candidateName + " - " + _GetVotes( factionKey, candidateSerial ) + " vote(s)" );
	}

	return true;
}

function ResetElection( factionKey )
{
	var ctrl = _GetCtrl();
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.SetTag( "elec_" + factionKey + "_state", ELEC_STATE_NONE );
	ctrl.SetTag( "elec_" + factionKey + "_start", 0 );
	ctrl.SetTag( "elec_" + factionKey + "_candidates", "" );
	ctrl.SetTag( "elec_" + factionKey + "_voters", "" );
	return true;
}
