/// <reference path="../definitions.d.ts" />
// @ts-check

function CommandRegistration()
{
	RegisterCommand( "testjsbindings", 2, true );
	RegisterCommand( "testjsobjects", 2, true );
	RegisterCommand( "testjsitem", 2, true );
	RegisterCommand( "testjscontainer", 2, true );
	RegisterCommand( "testjsextended", 2, true );
	RegisterCommand( "testjsgc", 8, true );
	RegisterCommand( "testjstimer", 8, true );
	RegisterCommand( "testjsarguments", 2, true );
	RegisterCommand( "testjserror", 2, true );
	RegisterCommand( "testjsall", 2, true );
}

var gcTimerTestState = null;
var gcTimerTestId = 32760;

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSALL( socket, cmdString )
{
	command_TESTJSBINDINGS( socket, cmdString );
	command_TESTJSOBJECTS( socket, cmdString );
	command_TESTJSEXTENDED( socket, cmdString );
	socket.SysMessage( "Non-targeted tests finished. Run testjsitem and testjscontainer for targeted tests." );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSBINDINGS( socket, cmdString )
{
	var failures = [];

	TestGlobal( failures, "Console", Console, [
		"Print", "Log", "Error", "Warning", "PrintSectionBegin",
		"TurnYellow", "TurnRed", "TurnGreen", "TurnBlue", "TurnNormal",
		"TurnBrightWhite", "PrintDone", "PrintFailed", "PrintPassed",
		"ClearScreen", "PrintBasedOnVal", "MoveTo", "PrintSpecial",
		"BeginRestart", "BeginShutdown", "Reload"
	]);
	TestGlobal( failures, "Accounts", Accounts, [
		"AddAccount", "DelAccount"
	]);
	TestGlobal( failures, "Timer", Timer, [] );
	TestGlobal( failures, "Skills", Skills, [] );
	TestGlobal( failures, "Spells", Spells, [] );
	TestGlobal( failures, "CreateEntries", CreateEntries, [] );
	TestGlobal( failures, "SCRIPT", SCRIPT, [] );
	TestConstructor( failures, "Gump", Gump );
	TestConstructor( failures, "Packet", Packet );
	TestConstructor( failures, "UOXCFile", UOXCFile );

	ReportBindingResults( socket, failures, "global JavaScript binding checks", "All global JavaScript binding checks passed." );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSOBJECTS( socket, cmdString )
{
	var failures = [];
	var pUser = socket.currentChar;

	RunBindingTest( failures, "Socket.currentChar", function()
	{
		RequireBinding( ValidateObject( pUser ), "did not return a valid character" );
	});
	RunBindingTest( failures, "Socket properties", function()
	{
		RequireType( socket.account, "object", "Socket.account" );
		RequireType( socket.bytesSent, "number", "Socket.bytesSent" );
		RequireType( socket.bytesReceived, "number", "Socket.bytesReceived" );
	});
	RunBindingTest( failures, "Socket.SysMessage", function()
	{
		socket.SysMessage( "Socket.SysMessage binding passed." );
	});
	RunBindingTest( failures, "Socket methods", function()
	{
		TestMethods( failures, "Socket", socket, [
			"SysMessage", "Disconnect", "SoundEffect", "CustomTarget", "PopUpTarget",
			"GetByte", "GetWord", "GetDWord", "GetSByte", "GetSWord", "GetSDWord",
			"GetString", "SetByte", "SetWord", "SetDWord", "SetString", "ReadBytes",
			"OpenContainer", "OpenGump", "CloseGump", "OpenURL", "BuyFrom", "SellTo",
			"WhoList", "Music", "GetTimer", "SetTimer", "SendAddMenu", "Page",
			"MakeMenu", "Send", "CanSee", "DisplayDamage", "FirstTriggerWord",
			"NextTriggerWord", "FinishedTriggerWords"
		]);
	});
	RunBindingTest( failures, "Character properties", function()
	{
		RequireType( pUser.name, "string", "Character.name" );
		RequireType( pUser.serial, "number", "Character.serial" );
		RequireType( pUser.x, "number", "Character.x" );
		RequireType( pUser.y, "number", "Character.y" );
		RequireType( pUser.z, "number", "Character.z" );
		RequireType( pUser.worldnumber, "number", "Character.worldnumber" );
	});
	RunBindingTest( failures, "Character tag methods", function()
	{
		pUser.GetTag( "__binding_test_missing__" );
		pUser.GetTempTag( "__binding_test_missing__" );
		RequireType( pUser.GetNumTags(), "number", "Character.GetNumTags()" );
	});
	RunBindingTest( failures, "Character methods", function()
	{
		TestMethods( failures, "Character", pUser, [
			"KillTimers", "GetJSTimer", "SetJSTimer", "KillJSTimer", "TextMessage",
			"YellMessage", "WhisperMessage", "EmoteMessage", "Delete", "DoAction",
			"StaticEffect", "Teleport", "SetLocation", "SoundEffect", "GetTag",
			"SetTag", "GetTempTag", "SetTempTag", "GetNumTags", "GetTagMap",
			"GetTempTagMap", "DirectionTo", "TurnToward", "ResourceCount",
			"UseResource", "CustomTarget", "PopUpTarget", "InRange", "FindItemLayer",
			"StartTimer", "CheckSkill", "CastSpell", "SysMessage", "GetSerial",
			"UpdateStats", "DistanceTo"
		]);
	});
	RunBindingTest( failures, "Skills[0]", function()
	{
		var skill = Skills[0];
		RequireType( skill, "object", "Skills[0]" );
		RequireType( skill.name, "string", "Skills[0].name" );
		RequireType( skill.scriptID, "number", "Skills[0].scriptID" );
	});
	RunBindingTest( failures, "Spells[1]", function()
	{
		var spell = Spells[1];
		RequireType( spell, "object", "Spells[1]" );
		RequireType( spell.id, "number", "Spells[1].id" );
		RequireType( spell.name, "string", "Spells[1].name" );
		RequireType( spell.enabled, "boolean", "Spells[1].enabled" );
	});
	RunBindingTest( failures, "Timer constants", function()
	{
		RequireType( Timer.TIMEOUT, "number", "Timer.TIMEOUT" );
		RequireType( Timer.SPELLTIME, "number", "Timer.SPELLTIME" );
	});
	RunBindingTest( failures, "Console.Print", function()
	{
		Console.Print( "Console.Print live binding passed.\n" );
	});

	ReportBindingResults( socket, failures, "live JavaScript object tests", "All live JavaScript object tests passed." );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSEXTENDED( socket, cmdString )
{
	var failures = [];
	var pUser = socket.currentChar;

	RunBindingTest( failures, "Account properties", function()
	{
		var account = socket.account;
		RequireType( account, "object", "Socket.account" );
		RequireType( account.id, "number", "Account.id" );
		RequireType( account.username, "string", "Account.username" );
		RequireType( account.flags, "number", "Account.flags" );
		RequireType( account.totalPlayTime, "number", "Account.totalPlayTime" );
		RequireType( account.isOnline, "boolean", "Account.isOnline" );
		RequireType( account.isGM, "boolean", "Account.isGM" );
	});
	RunBindingTest( failures, "Region bindings", function()
	{
		var region = pUser.region;
		RequireType( region, "object", "Character.region" );
		RequireType( region.id, "number", "Region.id" );
		RequireType( region.name, "string", "Region.name" );
		RequireType( region.isGuarded, "boolean", "Region.isGuarded" );
		RequireType( region.canMark, "boolean", "Region.canMark" );
		RequireType( region.canRecall, "boolean", "Region.canRecall" );
		RequireType( region.canGate, "boolean", "Region.canGate" );
		TestMethods( failures, "Region", region, [
			"AddScriptTrigger", "RemoveScriptTrigger", "GetOrePref", "GetOreChance"
		]);
	});
	RunBindingTest( failures, "Optional Guild bindings", function()
	{
		if( pUser.guild != null )
		{
			RequireType( pUser.guild.name, "string", "Guild.name" );
			RequireType( pUser.guild.id, "number", "Guild.id" );
			RequireType( pUser.guild.numMembers, "number", "Guild.numMembers" );
			TestMethods( failures, "Guild", pUser.guild, [
				"AcceptRecruit", "IsAtPeace", "AddMember", "AddRecruit", "RemoveRecruit",
				"RemoveMember", "RecruitToMember", "IsAtWar", "IsAlly", "IsNeutral"
			]);
		}
	});
	RunBindingTest( failures, "Optional Party bindings", function()
	{
		if( pUser.party != null )
		{
			RequireType( pUser.party.memberCount, "number", "Party.memberCount" );
			RequireType( pUser.party.isNPC, "boolean", "Party.isNPC" );
			TestMethods( failures, "Party", pUser.party, [ "GetMember", "Add", "Remove" ] );
		}
	});
	RunBindingTest( failures, "Gump constructor", function()
	{
		var gump = new Gump();
		TestMethods( failures, "Gump", gump, [
			"Free", "AddBackground", "AddButton", "AddButtonTileArt", "AddPageButton",
			"AddCheckbox", "AddCheckerTrans", "AddCroppedText", "AddGroup", "EndGroup",
			"AddGump", "AddGumpColor", "AddHTMLGump", "AddPage", "AddPicture",
			"AddPictureColor", "AddPicInPic", "AddItemProperty", "AddRadio", "AddText",
			"AddTextEntry", "AddTextEntryLimited", "AddTiledGump", "AddToolTip",
			"AddXMFHTMLGump", "AddXMFHTMLGumpColor", "AddXMFHTMLTok", "MasterGump",
			"NoClose", "NoDispose", "NoMove", "NoResize", "Send"
		]);
		gump.Free();
	});
	RunBindingTest( failures, "Packet constructor", function()
	{
		var packet = new Packet();
		TestMethods( failures, "Packet", packet, [
			"Free", "WriteByte", "WriteShort", "WriteLong", "WriteString", "ReserveSize"
		]);
		packet.Free();
	});
	RunBindingTest( failures, "UOXCFile constructor", function()
	{
		var file = new UOXCFile();
		TestMethods( failures, "UOXCFile", file, [
			"Free", "Open", "Close", "Write", "Read", "ReadUntil", "EOF", "Length", "Pos"
		]);
		file.Free();
	});

	ReportBindingResults( socket, failures, "extended JavaScript binding tests", "All extended JavaScript binding tests passed." );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSGC( socket, cmdString )
{
	var failures = [];
	var pUser = socket.currentChar;
	var account = socket.account;
	var region = pUser.region;
	var characterSerial = pUser.serial;
	var accountId = account.id;
	var regionId = region.id;
	var retainedValues = [];

	RunBindingTest( failures, "GC setup", function()
	{
		RequireBinding( ValidateObject( pUser ), "Socket.currentChar was invalid before collection" );
		RequireType( account, "object", "Socket.account" );
		RequireType( region, "object", "Character.region" );
	});

	for( var cycle = 0; cycle < 3; cycle++ )
	{
		RunBindingTest( failures, "GC allocation cycle " + ( cycle + 1 ), function()
		{
			var temporaryValues = [];
			for( var i = 0; i < 10000; i++ )
			{
				temporaryValues.push({
					index: i,
					text: "JavaScript garbage collection test value " + i,
					values: [i, i + 1, i + 2]
				});
			}
			retainedValues.push( temporaryValues[cycle * 1000] );
		});

		RunBindingTest( failures, "Forced GC cycle " + ( cycle + 1 ), function()
		{
			pUser.ExecuteCommand( "gcollect" );
			RequireBinding( ValidateObject( pUser ), "retained character became invalid" );
			RequireBinding( pUser.serial == characterSerial, "retained character serial changed" );
			RequireBinding( socket.currentChar === pUser, "character wrapper identity changed" );
			RequireBinding( socket.account === account, "account wrapper identity changed" );
			RequireBinding( account.id == accountId, "retained account ID changed" );
			RequireBinding( pUser.region === region, "region wrapper identity changed" );
			RequireBinding( region.id == regionId, "retained region ID changed" );
			RequireType( retainedValues[cycle].text, "string", "Retained JavaScript value" );
		});
	}

	ReportBindingResults( socket, failures, "JavaScript garbage-collection tests", "All JavaScript garbage-collection tests passed." );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSTIMER( socket, cmdString )
{
	var pUser = socket.currentChar;

	gcTimerTestState = {
		socket: socket,
		character: pUser,
		account: socket.account,
		region: pUser.region,
		characterSerial: pUser.serial,
		accountId: socket.account.id,
		regionId: pUser.region.id,
		value: "retained timer test value"
	};

	pUser.StartTimer( 1000, gcTimerTestId, true );
	pUser.ExecuteCommand( "gcollect" );
	socket.SysMessage( "JavaScript timer/rooting test started. Results will follow in one second." );
}

/** @type { ( timerObj: Character | Item, timerID: number ) => void } */
function onTimer( timerObj, timerID )
{
	if( timerID != gcTimerTestId )
	{
		return;
	}

	var failures = [];
	var state = gcTimerTestState;

	RunBindingTest( failures, "Retained timer state", function()
	{
		RequireType( state, "object", "Timer test state" );
		RequireBinding( ValidateObject( timerObj ), "timer callback object was invalid" );
		RequireBinding( timerObj === state.character, "character wrapper identity changed across callback" );
		RequireBinding( timerObj.serial == state.characterSerial, "character serial changed across callback" );
		RequireBinding( state.socket.currentChar === state.character, "socket character wrapper identity changed" );
		RequireBinding( state.socket.account === state.account, "account wrapper identity changed across callback" );
		RequireBinding( state.account.id == state.accountId, "account ID changed across callback" );
		RequireBinding( state.character.region === state.region, "region wrapper identity changed across callback" );
		RequireBinding( state.region.id == state.regionId, "region ID changed across callback" );
		RequireBinding( state.value == "retained timer test value", "retained JavaScript value changed" );
	});

	if( state != null && state.socket != null )
	{
		ReportBindingResults( state.socket, failures, "JavaScript timer/rooting tests", "All JavaScript timer/rooting tests passed." );
	}
	gcTimerTestState = null;
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSARGUMENTS( socket, cmdString )
{
	var failures = [];
	var pUser = socket.currentChar;

	ExpectBindingError( failures, "RandomNumber missing argument", function()
	{
		RandomNumber( 1 );
	});
	ExpectBindingError( failures, "GetDictionaryEntry missing argument", function()
	{
		GetDictionaryEntry();
	});
	ExpectBindingError( failures, "CalcCharFromSer missing argument", function()
	{
		CalcCharFromSer();
	});
	ExpectBindingError( failures, "CalcItemFromSer missing argument", function()
	{
		CalcItemFromSer();
	});
	ExpectBindingError( failures, "DistanceBetween null object", function()
	{
		DistanceBetween( null, pUser );
	});
	ExpectBindingError( failures, "DistanceBetween wrong wrapper", function()
	{
		DistanceBetween({}, pUser );
	});
	ExpectBindingError( failures, "DoTempEffect wrong target wrapper", function()
	{
		DoTempEffect( 0, null, {}, 0, 0, 0, 0 );
	});
	ExpectBindingError( failures, "MakeItem wrong wrappers", function()
	{
		MakeItem({}, {}, 0 );
	});
	ExpectBindingError( failures, "Character.GetTag missing argument", function()
	{
		pUser.GetTag();
	});
	ExpectBindingError( failures, "Character.GetTempTag extra argument", function()
	{
		pUser.GetTempTag( "__binding_test_missing__", "extra" );
	});
	ExpectBindingError( failures, "Character.GetTagMap extra argument", function()
	{
		pUser.GetTagMap( true );
	});
	ExpectBindingError( failures, "Character.GetTempTagMap extra argument", function()
	{
		pUser.GetTempTagMap( true );
	});
	ExpectBindingError( failures, "Character.DistanceTo wrong wrapper", function()
	{
		pUser.DistanceTo({});
	});
	ExpectBindingError( failures, "Character.InRange wrong wrapper", function()
	{
		pUser.InRange({}, 1 );
	});
	ExpectBindingError( failures, "Character.DirectionTo wrong wrapper", function()
	{
		pUser.DirectionTo({});
	});
	ExpectBindingError( failures, "Character.TurnToward wrong wrapper", function()
	{
		pUser.TurnToward({});
	});
	ExpectBindingError( failures, "Socket.OpenContainer wrong wrapper", function()
	{
		socket.OpenContainer({});
	});
	ExpectBindingError( failures, "Socket.Send wrong wrapper", function()
	{
		socket.Send({});
	});

	ReportBindingResults( socket, failures, "JavaScript argument-validation tests", "All JavaScript argument-validation tests passed." );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSERROR( socket, cmdString )
{
	socket.SysMessage( "An intentional JavaScript error will follow. This is expected." );
	BindingErrorLevelOne();
}

function BindingErrorLevelOne()
{
	BindingErrorLevelTwo();
}

function BindingErrorLevelTwo()
{
	throw new Error( "Intentional JavaScript binding error-reporting test" );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSITEM( socket, cmdString )
{
	socket.CustomTarget( 0, "Select an item to test its JavaScript bindings." );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback0( socket, target )
{
	if( !ValidateObject( target ) || !target.isItem )
	{
		socket.SysMessage( "You must select a valid item." );
		return;
	}

	var failures = [];

	RunBindingTest( failures, "Item identity properties", function()
	{
		RequireType( target.name, "string", "Item.name" );
		RequireType( target.serial, "number", "Item.serial" );
		RequireType( target.id, "number", "Item.id" );
		RequireType( target.colour, "number", "Item.colour" );
		RequireType( target.isItem, "boolean", "Item.isItem" );
	});
	RunBindingTest( failures, "Item location properties", function()
	{
		RequireType( target.x, "number", "Item.x" );
		RequireType( target.y, "number", "Item.y" );
		RequireType( target.z, "number", "Item.z" );
		RequireType( target.worldnumber, "number", "Item.worldnumber" );
		RequireType( target.instanceID, "number", "Item.instanceID" );
	});
	RunBindingTest( failures, "Item value properties", function()
	{
		RequireType( target.amount, "number", "Item.amount" );
		RequireType( target.type, "number", "Item.type" );
		RequireType( target.weight, "number", "Item.weight" );
		RequireType( target.movable, "number", "Item.movable" );
	});
	RunBindingTest( failures, "Item methods", function()
	{
		RequireType( target.GetTag, "function", "Item.GetTag" );
		RequireType( target.GetTempTag, "function", "Item.GetTempTag" );
		RequireType( target.GetNumTags, "function", "Item.GetNumTags" );
		RequireType( target.GetTileName, "function", "Item.GetTileName" );
		RequireType( target.IsMulti, "function", "Item.IsMulti" );
	});
	RunBindingTest( failures, "Item method calls", function()
	{
		target.GetTag( "__binding_test_missing__" );
		target.GetTempTag( "__binding_test_missing__" );
		RequireType( target.GetNumTags(), "number", "Item.GetNumTags()" );
		RequireType( target.GetTileName(), "string", "Item.GetTileName()" );
		RequireType( target.IsMulti(), "boolean", "Item.IsMulti()" );
	});

	ReportBindingResults( socket, failures, "live JavaScript item tests", "All live JavaScript item tests passed." );
}

/** @type { ( socket: Socket, cmdString: string ) => void } */
function command_TESTJSCONTAINER( socket, cmdString )
{
	socket.CustomTarget( 1, "Select a container to test its JavaScript iterator bindings." );
}

/** @type { ( socket: Socket, target: Character | Item | null ) => void } */
function onCallback1( socket, target )
{
	if( !ValidateObject( target ) || !target.isItem || !target.isContType )
	{
		socket.SysMessage( "You must select a valid container." );
		return;
	}

	var failures = [];
	var itemCount = 0;
	var maximumIterations = 10000;

	RunBindingTest( failures, "Container properties", function()
	{
		RequireType( target.itemsinside, "number", "Item.itemsinside" );
		RequireType( target.totalItemCount, "number", "Item.totalItemCount" );
		RequireType( target.maxItems, "number", "Item.maxItems" );
		RequireType( target.weight, "number", "Item.weight" );
		RequireType( target.weightMax, "number", "Item.weightMax" );
	});
	RunBindingTest( failures, "Container iterator methods", function()
	{
		RequireType( target.FirstItem, "function", "Item.FirstItem" );
		RequireType( target.NextItem, "function", "Item.NextItem" );
		RequireType( target.FinishedItems, "function", "Item.FinishedItems" );
	});
	RunBindingTest( failures, "Container iteration", function()
	{
		for( var item = target.FirstItem(); !target.FinishedItems(); item = target.NextItem() )
		{
			if( itemCount >= maximumIterations )
			{
				throw new Error( "iterator exceeded the safety limit" );
			}
			if( !ValidateObject( item ) || !item.isItem )
			{
				throw new Error( "iterator returned an invalid item" );
			}
			RequireType( item.serial, "number", "Contained Item.serial" );
			RequireType( item.name, "string", "Contained Item.name" );
			itemCount++;
		}
	});
	RunBindingTest( failures, "Container item count", function()
	{
		if( itemCount != target.itemsinside )
		{
			throw new Error( "iterator found " + itemCount + " items, but itemsinside reported " + target.itemsinside );
		}
	});

	ReportBindingResults(
		socket,
		failures,
		"live JavaScript container tests",
		"All live JavaScript container tests passed. Items found: " + itemCount + "."
	);
}

function TestGlobal( failures, globalName, globalObject, expectedMethods )
{
	if( typeof globalObject != "object" || globalObject == null )
	{
		failures.push( globalName + " is unavailable." );
		return;
	}

	for( var i = 0; i < expectedMethods.length; i++ )
	{
		var methodName = expectedMethods[i];
		if( typeof globalObject[methodName] != "function" )
		{
			failures.push( globalName + "." + methodName + " is not a function." );
		}
	}
}

function TestConstructor( failures, constructorName, constructorValue )
{
	if( typeof constructorValue != "function" )
	{
		failures.push( constructorName + " is not a constructor." );
	}
}

function TestMethods( failures, objectName, objectToTest, expectedMethods )
{
	for( var i = 0; i < expectedMethods.length; i++ )
	{
		var methodName = expectedMethods[i];
		if( typeof objectToTest[methodName] != "function" )
		{
			failures.push( objectName + "." + methodName + " is not a function." );
		}
	}
}

function RunBindingTest( failures, testName, testFunction )
{
	try
	{
		testFunction();
	}
	catch( error )
	{
		failures.push( testName + ": " + error.message );
	}
}

function ExpectBindingError( failures, testName, testFunction )
{
	var errorWasThrown = false;

	try
	{
		testFunction();
	}
	catch( error )
	{
		errorWasThrown = true;
		RequireType( error.message, "string", testName + " error message" );
	}

	if( !errorWasThrown )
	{
		failures.push( testName + ": expected a JavaScript exception" );
	}
}

function RequireType( value, expectedType, valueName )
{
	RequireBinding(
		typeof value == expectedType && value != null,
		valueName + " expected " + expectedType + ", received " + typeof value
	);
}

function RequireBinding( condition, message )
{
	if( !condition )
	{
		throw new Error( message );
	}
}

function ReportBindingResults( socket, failures, testName, successMessage )
{
	if( failures.length == 0 )
	{
		socket.SysMessage( successMessage );
		return;
	}

	socket.SysMessage( failures.length + " " + testName + " failed:" );
	for( var i = 0; i < failures.length; i++ )
	{
		socket.SysMessage( failures[i] );
	}
}
