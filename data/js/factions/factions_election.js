/// <reference path="../definitions.d.ts" />
// @ts-check

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

const factionElectionTimerId = 1;
const factionElectionTimerDelay = 3600000; // 1 hour
let factionElectionController = null;
const factionElectionPlayerDataScriptId = 8513;

const electionPendingMs = parseInt( GetServerSetting( "FACTIONELECTIONPENDINGHOURS" ), 10 ) * 3600000;
const electionCampaignMs = parseInt( GetServerSetting( "FACTIONELECTIONCAMPAIGNHOURS" ), 10 ) * 3600000;
const electionVotingMs = parseInt( GetServerSetting( "FACTIONELECTIONVOTINGHOURS" ), 10 ) * 3600000;
const electionMaxCandidates = parseInt( GetServerSetting( "FACTIONELECTIONMAXCANDIDATES" ), 10 );
const electionCandidateRank = Math.max( 0, parseInt( GetServerSetting( "FACTIONELECTIONCANDIDATERANK" ), 10 ) - 1 );

const electionStateNone = "none";
const electionStatePending = "pending";
const electionStateRunning = "running";
const electionStateVoting = "voting";
const electionStateFinished = "finished";

// ---------------------------------------------------------------------------
// HELPERS
// ---------------------------------------------------------------------------

function _GetCtrl()
{
	if( ValidateObject( factionElectionController ) )
		return factionElectionController;

	factionElectionController = null;
	IterateOver( "ITEM" );
	return factionElectionController;
}

function _EnsureTimer( ctrl )
{
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.KillJSTimer( factionElectionTimerId, 8508 );
	ctrl.StartTimer( factionElectionTimerDelay, factionElectionTimerId, 8508 );
	return true;
}

function onIterate( toCheck )
{
	if( ValidateObject( toCheck ) && toCheck.isItem && toCheck.GetTag( "faction_controller" ) == 1 )
	{
		factionElectionController = toCheck;
		return true;
	}

	return false;
}

function onCreateDFN( objMade, objType )
{
	if( ValidateObject( objMade ) && objMade.isItem && objMade.GetTag( "faction_controller" ) == 1 )
	{
		factionElectionController = objMade;
		_EnsureTimer( objMade );
	}
}

function onTimer( timerObj, timerID )
{
	if( timerID != factionElectionTimerId )
		return;
	if( !ValidateObject( timerObj ) || timerObj.GetTag( "faction_controller" ) != 1 )
		return;

	factionElectionController = timerObj;
	CheckElectionTimers();
	_EnsureTimer( timerObj );
}

function RegisterController( ctrl )
{
	if( !ValidateObject( ctrl ) || ctrl.GetTag( "faction_controller" ) != 1 )
		return false;

	factionElectionController = ctrl;
	return _EnsureTimer( ctrl );
}

function _GetElecState( fkey )
{
	const ctrl = _GetCtrl();
	if( !ctrl ) return electionStateNone;
	return ctrl.GetTag( "elec_" + fkey + "_state" ) || electionStateNone;
}

function _SetElecState( fkey, state )
{
	const ctrl = _GetCtrl();
	if( ctrl ) ctrl.SetTag( "elec_" + fkey + "_state", state );
}

function _GetElecStart( fkey )
{
	const ctrl = _GetCtrl();
	if( !ctrl ) return 0;
	return parseInt( ctrl.GetTag( "elec_" + fkey + "_start" ), 10 ) || 0;
}

function _SetElecStart( fkey, time )
{
	const ctrl = _GetCtrl();
	if( ctrl ) ctrl.SetTag( "elec_" + fkey + "_start", time );
}

function _GetCandidates( fkey )
{
	const ctrl = _GetCtrl();
	if( !ctrl ) return [];
	const raw = ctrl.GetTag( "elec_" + fkey + "_candidates" );
	if( !raw || raw.length === 0 ) return [];
	return raw.split( "," );
}

function _AddCandidate( fkey, serial )
{
	const ctrl = _GetCtrl();
	if( !ctrl ) return false;
	const list = _GetCandidates( fkey );
	// Avoid duplicates
	for( let i = 0; i < list.length; i++ )
		if( list[i] === String( serial ) ) return false;
	if( list.length >= electionMaxCandidates )
		return false;
	list.push( String( serial ) );
	ctrl.SetTag( "elec_" + fkey + "_candidates", list.join( "," ) );
	// init vote count
	ctrl.SetTag( "elec_" + fkey + "_votes_" + serial, 0 );
	return true;
}

function _GetVotes( fkey, serial )
{
	const ctrl = _GetCtrl();
	if( !ctrl ) return 0;
	return parseInt( ctrl.GetTag( "elec_" + fkey + "_votes_" + serial ), 10 ) || 0;
}

function _AddVote( fkey, candidateSerial )
{
	const ctrl = _GetCtrl();
	if( !ctrl ) return;
	const v = _GetVotes( fkey, candidateSerial );
	ctrl.SetTag( "elec_" + fkey + "_votes_" + candidateSerial, v + 1 );
}

function _HasVoted( pChar, fkey )
{
	const ctrl = _GetCtrl();
	if( !ValidateObject( ctrl ) ) return false;
	let voters = String( ctrl.GetTag( "elec_" + fkey + "_voters" ) || "" ).split( "," );
	for( let voterIndex = 0; voterIndex < voters.length; voterIndex++ )
		if( voters[voterIndex] === String( pChar.serial ) ) return true;
	return false;
}

function _MarkVoted( pChar, fkey )
{
	const ctrl = _GetCtrl();
	if( !ValidateObject( ctrl ) ) return false;
	const tagName = "elec_" + fkey + "_voters";
	let voters = String( ctrl.GetTag( tagName ) || "" );
	ctrl.SetTag( tagName, voters === "" ? String( pChar.serial ) : voters + "," + pChar.serial );
	return true;
}

function _ClearVotedFlags( fkey )
{
	const ctrl = _GetCtrl();
	if( ValidateObject( ctrl ) ) ctrl.SetTag( "elec_" + fkey + "_voters", "" );
}

// ---------------------------------------------------------------------------
// START ELECTION
// ---------------------------------------------------------------------------
function StartElection( fkey )
{
	if( _GetElecState( fkey ) !== electionStateNone &&
	    _GetElecState( fkey ) !== electionStateFinished )
	{
		return false; // already in progress
	}

	const ctrl = _GetCtrl();
	if( !ctrl ) return false;

	_SetElecState( fkey, electionStatePending );
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
	const factionData = TriggerEvent( factionElectionPlayerDataScriptId, "ReadFactionPlayerData", pChar );
	const fkey = factionData.faction;
	if( !fkey || fkey.length === 0 )
	{
		pChar.SysMessage( GetDictionaryEntry( 25281, ( pChar.socket == null ? 0 : pChar.socket.language ) ) );
		return false;
	}

	let state = _GetElecState( fkey );
	if( state !== electionStateRunning )
	{
		pChar.SysMessage( GetDictionaryEntry( 25282, ( pChar.socket == null ? 0 : pChar.socket.language ) ) );
		return false;
	}

	if( factionData.rank < electionCandidateRank )
	{
		pChar.SysMessage( GetDictionaryEntry( 25283, ( pChar.socket == null ? 0 : pChar.socket.language ) ) );
		return false;
	}

	if( !_AddCandidate( fkey, pChar.serial ) )
	{
		pChar.SysMessage( GetDictionaryEntry( 25284, ( pChar.socket == null ? 0 : pChar.socket.language ) ) );
		return false;
	}
	pChar.SysMessage( GetDictionaryEntry( 25285, ( pChar.socket == null ? 0 : pChar.socket.language ) ) );
	_BroadcastFaction( fkey, pChar.name + " has declared candidacy for Commander!" );
	return true;
}

// ---------------------------------------------------------------------------
// BEGIN VOTING PHASE  (called by tick/timer or GM)
// ---------------------------------------------------------------------------
function BeginVoting( fkey )
{
	if( _GetElecState( fkey ) !== electionStateRunning ) return false;
	_SetElecState( fkey, electionStateVoting );
	_BroadcastFaction( fkey, "Voting has begun! Visit a Faction Stone to cast your vote for Commander!" );
	return true;
}

function BeginCampaign( fkey )
{
	if( _GetElecState( fkey ) !== electionStatePending ) return false;
	_SetElecState( fkey, electionStateRunning );
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
	const fkey = TriggerEvent( factionElectionPlayerDataScriptId, "GetFactionValue", pVoter, "faction", pVoter.GetTag( "faction" ) );
	if( !fkey || fkey.length === 0 )
	{
		pVoter.SysMessage( GetDictionaryEntry( 25286, ( pVoter.socket == null ? 0 : pVoter.socket.language ) ) );
		return false;
	}

	if( _GetElecState( fkey ) !== electionStateVoting )
	{
		pVoter.SysMessage( GetDictionaryEntry( 25287, ( pVoter.socket == null ? 0 : pVoter.socket.language ) ) );
		return false;
	}

	if( _HasVoted( pVoter, fkey ) )
	{
		pVoter.SysMessage( GetDictionaryEntry( 25288, ( pVoter.socket == null ? 0 : pVoter.socket.language ) ) );
		return false;
	}

	// Validate candidate is in list
	const candidates = _GetCandidates( fkey );
	let found      = false;
	for( let i = 0; i < candidates.length; i++ )
	{
		if( candidates[i] === String( candidateSerial ) ) { found = true; break; }
	}
	if( !found )
	{
		pVoter.SysMessage( GetDictionaryEntry( 25289, ( pVoter.socket == null ? 0 : pVoter.socket.language ) ) );
		return false;
	}

	_AddVote( fkey, candidateSerial );
	_MarkVoted( pVoter, fkey );
	pVoter.SysMessage( GetDictionaryEntry( 25290, ( pVoter.socket == null ? 0 : pVoter.socket.language ) ) );
	return true;
}

// ---------------------------------------------------------------------------
// CONCLUDE ELECTION
// ---------------------------------------------------------------------------
function ConcludeElection( fkey )
{
	if( _GetElecState( fkey ) !== electionStateVoting ) return false;

	const candidates = _GetCandidates( fkey );
	const ctrl = _GetCtrl();
	if( candidates.length === 0 )
	{
		_SetElecState( fkey, electionStateFinished );
		StartElection( fkey );
		_BroadcastFaction( fkey, "The election ended with no candidates. Faction leadership has not changed." );
		return false;
	}

	// Find winner
	let winnerSerial = -1;
	let winnerVotes  = -1;
	for( let i = 0; i < candidates.length; i++ )
	{
		const v = _GetVotes( fkey, candidates[i] );
		if( v > winnerVotes ) { winnerVotes = v; winnerSerial = candidates[i]; }
	}

	// Promote winner
	const winner = CalcCharFromSer( parseInt( winnerSerial, 10 ) );
	if( ValidateObject( winner ) )
	{
		const winnerData = TriggerEvent( factionElectionPlayerDataScriptId, "ReadFactionPlayerData", winner );
		if( winnerData.faction !== fkey )
		{
			_BroadcastFaction( fkey, "The winning candidate is no longer eligible. Faction leadership has not changed." );
			_SetElecState( fkey, electionStateFinished );
			StartElection( fkey );
			return false;
		}
		_ClearCommanderRole( fkey, ctrl );
		winnerData.commander = true;
		winnerData.role = "commander";
		winnerData.roleFaction = fkey;
		winnerData.roleTown = "";
		winnerData.roleSetAt = GetCurrentClock();
		TriggerEvent( factionElectionPlayerDataScriptId, "WriteFactionPlayerData", winner, winnerData );
		if( ctrl ) ctrl.SetTag( "cmd_" + fkey, winnerSerial );
		_BroadcastFaction( fkey, winner.name + " has been elected as the new Commander of the " + fkey + "!" );
	}
	else
	{
		_BroadcastFaction( fkey, "No eligible election winner could be found. Faction leadership has not changed." );
	}

	_SetElecState( fkey, electionStateFinished );
	StartElection( fkey );
	return true;
}

function _ClearCommanderRole( fkey, ctrl )
{
	if( !ValidateObject( ctrl ) )
		return false;

	const oldSerial = parseInt( ctrl.GetTag( "cmd_" + fkey ), 10 );
	if( oldSerial > 0 )
	{
		const oldCmd = CalcCharFromSer( oldSerial );
		if( ValidateObject( oldCmd ) )
		{
			const oldData = TriggerEvent( factionElectionPlayerDataScriptId, "ReadFactionPlayerData", oldCmd );
			oldData.commander = false;
			if( oldData.role === "commander" && oldData.roleFaction === fkey )
			{
				oldData.role = "";
				oldData.roleFaction = "";
				oldData.roleTown = "";
				oldData.roleSetAt = 0;
			}
			TriggerEvent( factionElectionPlayerDataScriptId, "WriteFactionPlayerData", oldCmd, oldData );
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
	const FACTION_KEYS = [ "TB", "COM", "MIN", "SL" ];
	const now = GetCurrentClock();

	for( let i = 0; i < FACTION_KEYS.length; i++ )
	{
		const fkey = FACTION_KEYS[i];
		let state = _GetElecState( fkey );
		const start = _GetElecStart( fkey );

		if( state === electionStateNone || state === electionStateFinished )
		{
			StartElection( fkey );
		}
		else if( state === electionStatePending )
		{
			if( ( now - start ) >= electionPendingMs ) BeginCampaign( fkey );
		}
		else if( state === electionStateRunning )
		{
			// Switch to voting after campaign period
			if( ( now - start ) >= electionCampaignMs )
			{
				const candidates = _GetCandidates( fkey );
				if( candidates.length === 0 )
				{
					_SetElecState( fkey, electionStateFinished );
					StartElection( fkey );
				}
				else if( candidates.length === 1 )
				{
					_SetElecState( fkey, electionStateVoting );
					ConcludeElection( fkey );
				}
				else
				{
					BeginVoting( fkey );
					_SetElecStart( fkey, now );
				}
			}
		}
		else if( state === electionStateVoting )
		{
			// Conclude after voting period
			if( ( now - start ) >= electionVotingMs )
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
	let state      = _GetElecState( fkey );
	const candidates = _GetCandidates( fkey );

	const myGump = new Gump();
	myGump.AddBackground( 0, 0, 400, 340, 9200 );
	myGump.AddHTMLGump( 10, 10, 380, 25, 0, 0, "<CENTER><b>Faction Election</b></CENTER>" );

	myGump.AddHTMLGump( 10, 40, 380, 20, 0, 0, "State: <b>" + state + "</b>" );

	if( state === electionStateRunning )
	{
		myGump.AddButton( 10, 70, 0xFA5, 0xFA7, 1, 0, 1 );
		myGump.AddHTMLGump( 50, 70, 300, 20, 0, 0, "Declare Candidacy" );
	}
	else if( state === electionStateVoting )
	{
		myGump.AddHTMLGump( 10, 65, 380, 20, 0, 0, "<b>Candidates:</b>" );
		let yPos = 90;
		for( let i = 0; i < candidates.length; i++ )
		{
			const cSerial = parseInt( candidates[i], 10 );
			const cChar = CalcCharFromSer( cSerial );
			const cName = ValidateObject( cChar ) ? cChar.name : "Unknown (" + cSerial + ")";
			const votes = _GetVotes( fkey, cSerial );

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
	const pChar = pSock.currentChar;
	if( !ValidateObject( pChar ) )
		return;

	const fkey = TriggerEvent( factionElectionPlayerDataScriptId, "GetFactionValue", pChar, "faction", pChar.GetTag( "faction" ) );
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
		const candidates = _GetCandidates( fkey );
		const idx = buttonID - 100;
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

	pSock.SysMessage( GetDictionaryEntry( 25291, pSock.language ).replace( /%s/, String( factionKey ) ).replace( /%s/, String( _GetElecState( factionKey ) ) ) );
	const candidates = _GetCandidates( factionKey );
	for( let candidateIndex = 0; candidateIndex < candidates.length; candidateIndex++ )
	{
		const candidateSerial = Number( candidates[candidateIndex] );
		const candidate = CalcCharFromSer( candidateSerial );
		const candidateName = ValidateObject( candidate ) ? candidate.name : "Serial " + candidateSerial;
		pSock.SysMessage( GetDictionaryEntry( 25292, pSock.language ).replace( /%s/, String( candidateName ) ).replace( /%s/, String( _GetVotes( factionKey, candidateSerial ) ) ) );
	}

	return true;
}

function ResetElection( factionKey )
{
	const ctrl = _GetCtrl();
	if( !ValidateObject( ctrl ) )
		return false;

	ctrl.SetTag( "elec_" + factionKey + "_state", electionStateNone );
	ctrl.SetTag( "elec_" + factionKey + "_start", 0 );
	ctrl.SetTag( "elec_" + factionKey + "_candidates", "" );
	ctrl.SetTag( "elec_" + factionKey + "_voters", "" );
	return true;
}
