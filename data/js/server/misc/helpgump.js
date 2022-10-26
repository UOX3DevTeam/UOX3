const shardName = GetServerSetting( "ServerName" );
const shardURL = "https://myshard.example.com";
const maxSafeTeleportsPerDay = GetServerSetting( "MaxSafeTeleportsPerDay" ); // Determines amount of "safe teleports" players get per day
const teleportToNearestSafeLocation = GetServerSetting( "teleportToNearestSafeLocation" ); // If enabled, teleports to nearest safe location. If disabled, random safe location
const murderersCanTeleport = false; // If enabled, murderers can make use of the stuck teleport feature, which will teleport them to a Chaos shrine/another "safe" location for murderers
const instantTeleport = false; // If enabled, players who use the stuck teleport feature will be instantly teleported. Otherwise, they need to wait ~30 seconds

// Website links
const ShardWebsite = shardName + ' - <A HREF=' + shardURL + '>' + shardURL + '</a>';
const UOX3Website = 'Ultima Offline eXperiment 3- <A HREF="https://www.uox3.org">https://www.uox3.org</a>';
const UOWebsite = 'Ultima Online - <A HREF="https://www.uo.com">UO Website</a>';

// Triggered through onHelpButton() Event in global.js
function DisplayHelpMenu( pUser )
{
	var pSock = pUser.socket;
	if( pSock == null )
		return;

	var sLanguage = pSock.language;

	// Set up dictionary messages
	// Gump Header
	const gumpTitle = "<BIG><CENTER><U>" + shardName + " " + GetDictionaryEntry( 17000, sLanguage ) + "</U></CENTER></BIG>"; // Help Menu

	// General Gameplay Questions
	const generalTitle = "<H3>" + GetDictionaryEntry( 17001, sLanguage ) + "</H3>"; // General Gameplay Questions
	const generalText = "<H3>" + GetDictionaryEntry( 17002, sLanguage ) + "</H3>"; // Select this option if you have general gameplay questions, which are normally answered by Counselors.

	// Character stuck
	const stuckTitle = "<H3>" + GetDictionaryEntry( 17003, sLanguage ) + "</H3>"; // Character is Stuck and Unable to Move
	const stuckText = "<H3>" + GetDictionaryEntry( 17004, sLanguage ) + "</H3>"; // Select this option if your character is physically stuck in a location they cannot move out of.

	// GM Page main selection
	const assistanceTitle = "<h3>" + GetDictionaryEntry( 17005, sLanguage ) + "</H3>"; // Help! I need GM assistance!
	const assistanceText = "<H3>" + GetDictionaryEntry( 17006, sLanguage ) + "</H3>"; // Select this option if you need assistance from GMs to deal with harassment, bugged items, or other problems.

	// Harassment-related GM Page
	const harassmentTitle = "<H3>" + GetDictionaryEntry( 17007, sLanguage ) + "</H3>"; // Another player is harassing me
	const harassmentText = "<H3>" + GetDictionaryEntry( 17008, sLanguage ) + "</H3>"; // Another player is harassing me verbally/physically in-game!

	// Item-related GM Page
	const itemProblemTitle = "<H3>" + GetDictionaryEntry( 17009, sLanguage ) + "</H3>"; // I'm having item-related trouble
	const itemProblemText = "<H3>" + GetDictionaryEntry( 17010, sLanguage ) + "</H3>"; // I have an item-related issue, which requires GM assistance.

	// Exploit-related GM Page
	const exploitTitle = "<H3>" + GetDictionaryEntry( 17011, sLanguage ) + "</H3>"; // I wish to report a shard exploit
	const exploitText = "<H3>" + GetDictionaryEntry( 17012, sLanguage ) + "</H3>"; // I've come across an exploit that I'd like to report to GMs!

	// Additional options
	const otherTitle = "<H3>" + GetDictionaryEntry( 17013, sLanguage ) + "</H3>"; // Other
	const otherText = "<H3>" + GetDictionaryEntry( 17014, sLanguage ) + "</H3>"; // Select to see additional options for bug reports/suggestions, to change your password or to view shard information.

	// Reporting bugs
	const reportBugTitle = "<H3>" + GetDictionaryEntry( 17015, sLanguage ) + "</H3>"; // Report a Bug
	const reportBugText = '<H3>' + GetDictionaryEntry( 17016, sLanguage ) + '</H3>'; // Submit a short and quick bug report from in-game. For more detailed bug reports, post <a href="https://www.uox3.org">in the forums</a> instead.

	// Making suggestions
	const suggestionTitle = "<H3>" + GetDictionaryEntry( 17017, sLanguage ) + "</H3>"; // Provide a Gameplay Suggestion
	const suggestionText = "<H3>" + GetDictionaryEntry( 17018, sLanguage ) + "<H3>"; // If you'd like to make a suggestion for the game, Choosing this option will take you to the Discussion Forums.

	// Password management
	const accManagementTitle = "<H3>" + GetDictionaryEntry( 17019, sLanguage ) + "</H3>"; // Password Management
	const accManagementText = "<H3>" + GetDictionaryEntry( 17020, sLanguage ) + "</H3>"; // Change your account password to something else

	// Server info
	const serverInfoTitle ="<H3>" + GetDictionaryEntry( 17027, sLanguage ) + "</H3>"; // Server Info
	const serverInfoText = "<H3>" + GetDictionaryEntry( 17028, sLanguage ) + "</H3>"; // Here you will find the server information

	// Other problem
	const otherProblemTitle = GetDictionaryEntry( 17030, sLanguage ); //Other Problem
	const otherProblemText = GetDictionaryEntry( 17031, sLanguage ); // I've got another problem that doesn't fit any of these categories.

	const serverStatus = GetDictionaryEntry( 17048, sLanguage ); // SERVER STATUS

	const passwordRule1 = GetDictionaryEntry( 17023, sLanguage ); // Passwords may only consist of letters (a-z, A-Z) and digits (0-9).
	const passwordRule2 = GetDictionaryEntry( 17037, sLanguage ); // Your password must be 5 (min) to 17 (max) characters long.
	const passwordRule3 = GetDictionaryEntry( 17038, sLanguage ); // Confirm New Password' does not match 'New Password'! Note that passwords are cAsE sEnSiTiVe.
	const passwordStatus = GetDictionaryEntry( 17039, sLanguage ); // The new password has been saved to your account.

	// Return to previous menu
	const wrongProblem = GetDictionaryEntry( 17029, sLanguage );

	var helpGump = new Gump();

	// Add shared background
	helpGump.AddBackground( 50, 25, 540, 430, 5054 );
	helpGump.AddCheckerTrans( 50, 25, 540, 430 );
	helpGump.AddPage( 0 );
	helpGump.AddGump( 50, 25, 5545 );
	helpGump.AddGump( 530, 25, 5545 );
	helpGump.AddHTMLGump( 50, 25, 540, 40, false, false, "<center> <BASEFONT color=#ffffff>" + gumpTitle + "</BASEFONT> </center>");

	// A different button must be shown if connected client is of version 4.x or lower
	if( pSock.clientMajorVer < 5 )
	{
		helpGump.AddButton( 425, 415, 2073, 2072, 1, 0, 0 );// Close Button
	}
	else
	{
		helpGump.AddButton( 425, 415, 12006, 12007, 1, 0, 0 );// Close Button
	}

	// Add first page
	helpGump.AddPage( 1 );

	helpGump.AddButton( 80, 90, 5540, 5541, 1, 0, 1 );
	helpGump.AddHTMLGump( 110, 90, 450, 74, false, false, "<BASEFONT color=#ffffff>" + generalTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 110, 450, 45, true, false, generalText );

	helpGump.AddButton( 80, 170, 5540, 5541, 1, 0, 2 );
	helpGump.AddHTMLGump( 110, 170, 450, 74, false, false, "<BASEFONT color=#ffffff>" + stuckTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 190, 450, 45, true, false, stuckText );

	helpGump.AddPageButton( 80, 250, 5540, 5541, 3 );
	helpGump.AddHTMLGump( 110, 250, 450, 74, false, false, "<BASEFONT color=#ffffff>" + assistanceTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 270, 450, 45, true, false, assistanceText );

	helpGump.AddPageButton( 80, 330, 5540, 5541, 2 );
	helpGump.AddHTMLGump( 110, 330, 450, 74, false, false, "<BASEFONT color=#ffffff>" + otherTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 350, 450, 45, true, false, otherText );

	// Add second page
	helpGump.AddPage( 2 );

	helpGump.AddButton( 80, 90, 5540, 5541, 1, 0, 3 );
	helpGump.AddHTMLGump( 110, 90, 450, 74, false, false, "<BASEFONT color=#ffffff>" + reportBugTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 110, 450, 45, true, false, reportBugText );

	helpGump.AddButton( 80, 170, 5540, 5541, 1, 0, 4 );
	helpGump.AddHTMLGump( 110, 170, 450, 74, false, false, "<BASEFONT color=#ffffff>" + suggestionTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 190, 450, 45, true, false, suggestionText );

	helpGump.AddButton( 80, 250, 5540, 5541, 1, 0, 9 );
	helpGump.AddHTMLGump( 110, 250, 450, 74, false, false, "<BASEFONT color=#ffffff>" + accManagementTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 270, 450, 30, true, false, accManagementText );

	helpGump.AddPageButton( 80, 330, 5540, 5541, 4 );
	helpGump.AddHTMLGump( 110, 330, 450, 74, false, false, "<BASEFONT color=#ffffff>" + serverInfoTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 350, 450, 30, true, false, serverInfoText );

	helpGump.AddPageButton( 150, 390, 5537, 5538, 1 );
	helpGump.AddHTMLGump( 180, 390, 335, 40, false, false, "<BASEFONT color=#ffffff>" + wrongProblem + "</BASEFONT>" );

	// Add third page
	helpGump.AddPage( 3 );

	helpGump.AddButton( 80, 90, 5540, 5541, 1, 0, 5 );
	helpGump.AddHTMLGump( 110, 90, 450, 20, false, false, "<BASEFONT color=#ffffff>" + harassmentTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 110, 450, 30, true, false, harassmentText );

	helpGump.AddButton( 80, 170, 5540, 5541, 1, 0, 6 );
	helpGump.AddHTMLGump( 110, 170, 450, 20, false, false, "<BASEFONT color=#ffffff>" + itemProblemTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 190, 450, 30, true, false, itemProblemText );

	helpGump.AddButton( 80, 250, 5540, 5541, 1, 0, 7 );
	helpGump.AddHTMLGump( 110, 250, 450, 20, false, false, "<BASEFONT color=#ffffff>" + exploitTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 270, 450, 30, true, false, exploitText );

	helpGump.AddButton( 80, 330, 5540, 5541, 1, 0, 8 );
	helpGump.AddHTMLGump( 110, 330, 450, 20, false, false, "<BASEFONT color=#ffffff>" + otherProblemTitle + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 350, 450, 30, true, false, otherProblemText );

	helpGump.AddPageButton( 150, 390, 5537, 5538, 1 );
	helpGump.AddHTMLGump( 180, 390, 335, 40, false, false, "<BASEFONT color=#ffffff>" + wrongProblem + "</BASEFONT>" );

	// Add fourth page
	helpGump.AddPage( 4 );

	helpGump.AddHTMLGump( 110, 90, 250, 145, false, false, "<BASEFONT color=#ffffff>" + serverStatus + "</BASEFONT>" );
	// Server Version
	var serverVersionString = GetServerVersionString();
	helpGump.AddHTMLGump( 110, 130, 250, 145, false, false, "<BASEFONT color=#ffffff>" + "UOX3 Version:" + "</BASEFONT>" );
	helpGump.AddHTMLGump( 250, 130, 200, 145, false, false, "<BASEFONT color=#abcdef>" + serverVersionString + "</BASEFONT>" );

	// Uptime
	var uptimeInSeconds = Math.floor( GetCurrentClock() / 1000 ) - Math.floor( GetStartTime() / 1000 );
	var d = Math.floor( uptimeInSeconds / ( 3600 * 24 ));
	var h = Math.floor( uptimeInSeconds % ( 3600 * 24 ) / 3600 );
	var m = Math.floor( uptimeInSeconds % 3600 / 60 );
	var dDisplay = d > 0 ? d + "d : " : "0d : ";
	var hDisplay = h > 0 ? h + "h : " : "0h : ";
	var mDisplay = m + "m";

	helpGump.AddHTMLGump( 110, 150, 250, 145, false, false, "<BASEFONT color=#ffffff>" + "Uptime:" + "</BASEFONT>" );
	helpGump.AddHTMLGump( 250, 150, 200, 145, false, false, "<BASEFONT color=#abcdef>" + dDisplay + hDisplay + mDisplay + "</BASEFONT>" );

	// Accounts
	helpGump.AddHTMLGump( 110, 170, 250, 145, false, false, "<BASEFONT color=#ffffff>" + "Accounts:" + "</BASEFONT>" );
	helpGump.AddHTMLGump( 250, 170, 200, 145, false, false, "<BASEFONT color=#abcdef>" + GetAccountCount().toString() + "</BASEFONT>" );

	// Items
	helpGump.AddHTMLGump( 110, 190, 250, 145, false, false, "<BASEFONT color=#ffffff>" + "Items:" + "</BASEFONT>" );
	helpGump.AddHTMLGump( 250, 190, 200, 145, false, false, "<BASEFONT color=#abcdef>" + GetItemCount().toString() + "</BASEFONT>" );

	// Multis
	helpGump.AddHTMLGump( 110, 210, 250, 145, false, false, "<BASEFONT color=#ffffff>" + "Multis:" + "</BASEFONT>" );
	helpGump.AddHTMLGump( 250, 210, 200, 145, false, false, "<BASEFONT color=#abcdef>" + GetMultiCount().toString() + "</BASEFONT>" );

	// Characters
	helpGump.AddHTMLGump( 110, 230, 250, 145, false, false, "<BASEFONT color=#ffffff>" + "Characters:" + "</BASEFONT>" );
	helpGump.AddHTMLGump( 250, 230, 200, 145, false, false, "<BASEFONT color=#abcdef>" + GetCharacterCount().toString() + "</BASEFONT>" );

	// Players Online
	helpGump.AddHTMLGump( 110, 250, 250, 145, false, false, "<BASEFONT color=#ffffff>" + "Players Online:" + "</BASEFONT>" );
	helpGump.AddHTMLGump( 250, 250, 200, 145, false, false, "<BASEFONT color=#abcdef>" + GetPlayerCount().toString() + "</BASEFONT>" );

	// Useful websites
	helpGump.AddHTMLGump( 110, 300, 450, 74, false, false, "<BASEFONT color=#ffffff>" + ShardWebsite + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 325, 450, 74, false, false, "<BASEFONT color=#ffffff>" + UOX3Website + "</BASEFONT>" );
	helpGump.AddHTMLGump( 110, 350, 450, 74, false, false, "<BASEFONT color=#ffffff>" + UOWebsite + "</BASEFONT>" );

	// Go back
	helpGump.AddPageButton( 150, 390, 5537, 5538, 1 );
	helpGump.AddHTMLGump( 180, 390, 335, 40, false, false, "<BASEFONT color=#ffffff>" + wrongProblem + "</BASEFONT>" );

	helpGump.Send( pSock );
	helpGump.Free();
}

function ChangePasswordGump( pSock )
{
	var sLanguage = pSock.language;

	const gumpTitle = "<BIG><CENTER><U>" + shardName + " " + GetDictionaryEntry( 17000, sLanguage ) + "</U></CENTER></BIG>"; // Help Menu
	const passwordManageTitle = GetDictionaryEntry( 17049, sLanguage ); // PASSWORD MANAGEMENT
	const passwordManageText = GetDictionaryEntry( 17032, sLanguage ); // You can change the password to your shard account from this page. Take note that the password must be between 5 and 17 characters long, and should be unique to this shard. Don't reuse passwords you already used elsewhere! Also, don't share your account credentials with anyone!
	const newPasswordText = GetDictionaryEntry( 17021, sLanguage ); // Enter New Password
	const confirmNewPasswordText = GetDictionaryEntry( 17022, sLanguage ); // Confirm New Password
	const wrongProblem = GetDictionaryEntry( 17029, sLanguage ); // Previous Menu

	var changePassGump = new Gump();

	// Add shared background
	changePassGump.AddBackground( 50, 25, 540, 430, 5054 );
	changePassGump.AddCheckerTrans( 50, 25, 540, 430 );
	changePassGump.AddPage( 0 );
	changePassGump.AddGump( 50, 25, 5545 );
	changePassGump.AddGump( 530, 25, 5545 );
	changePassGump.AddHTMLGump( 50, 25, 540, 40, false, false, "<center> <BASEFONT color=#ffffff>" + gumpTitle + "</BASEFONT> </center>");

	// A different button must be shown if connected client is of version 4.x or lower
	if( pSock.clientMajorVer < 5 )
	{
		changePassGump.AddButton( 425, 415, 2073, 2072, 1, 0, 0 );// Close Button
	}
	else
	{
		changePassGump.AddButton( 425, 415, 12006, 12007, 1, 0, 0 );// Close Button
	}

	changePassGump.AddText( 147, 90, 2727, passwordManageTitle ); // Account Management - Password Change
	changePassGump.AddHTMLGump( 147, 120, 400, 100, false, false, "<BASEFONT color=#abcdef>" + passwordManageText + "</BASEFONT>");

	changePassGump.AddButton( 150, 390, 5537, 5538, 1, 0, 100 );
	changePassGump.AddHTMLGump( 180, 390, 335, 40, false, false, "<BASEFONT color=#ffffff>" + wrongProblem + "</BASEFONT>" );

	changePassGump.AddText( 147, 228, 2727, newPasswordText ); // New Password:
	changePassGump.AddText( 149, 256, 2727, confirmNewPasswordText ); // Confirm New Password:
	changePassGump.AddBackground( 335, 229, 200, 20, 9200);
	changePassGump.AddTextEntry( 335, 229, 200, 20, 1175, 1, 40, " " );
	changePassGump.AddBackground( 334, 255, 200, 20, 9200 );
	changePassGump.AddTextEntry( 334, 255, 200, 20, 1175, 1, 41, " " );
	changePassGump.AddButton( 144, 291, 247, 248, 1, 0, 20 ); // Okay Button

	changePassGump.Send( pSock );
	changePassGump.Free();
}

function BugReportGump( pSock )
{
	var sLanguage = pSock.language;

	const gumpTitle = "<BIG><CENTER><U>" + shardName + " " + GetDictionaryEntry( 17000, sLanguage ) + "</U></CENTER></BIG>"; // Help Menu
	const reportBugHeader = GetDictionaryEntry( 17033, sLanguage ); // REPORT BUG
	const reportBugMainText = GetDictionaryEntry( 17034, sLanguage ); // Please describe the bug you're experiencing with as much detail as the character limit of 240 characters allows. If your problem requires a more lengthy explanation, please post in the shard forums and/or discord instead.
	const enterBugReportText = GetDictionaryEntry( 17035, sLanguage ); // Enter your bug-report below (max 240 chars):
	const wrongProblem = GetDictionaryEntry( 17029, sLanguage );

	var bugreportGump = new Gump();

	// Add shared background
	bugreportGump.AddBackground( 50, 25, 540, 430, 5054 );
	bugreportGump.AddCheckerTrans( 50, 25, 540, 430 );
	bugreportGump.AddPage( 0 );
	bugreportGump.AddGump( 50, 25, 5545 );
	bugreportGump.AddGump( 530, 25, 5545 );
	bugreportGump.AddHTMLGump( 50, 25, 540, 40, false, false, "<center> <BASEFONT color=#ffffff>" + gumpTitle + "</BASEFONT> </center>");

	// A different button must be shown if connected client is of version 4.x or lower
	if( pSock.clientMajorVer < 5 )
	{
		bugreportGump.AddButton( 425, 415, 2073, 2072, 1, 0, 0 );// Close Button
	}
	else
	{
		bugreportGump.AddButton( 425, 415, 12006, 12007, 1, 0, 0 );// Close Button
	}

	bugreportGump.AddText( 147, 90, 2727, reportBugHeader ); // Report Bug
	bugreportGump.AddHTMLGump( 147, 120, 400, 100, false, false, "<BASEFONT color=#abcdef>" + reportBugMainText + "</BASEFONT>");

	bugreportGump.AddButton( 150, 390, 5537, 5538, 1, 0, 100 );
	bugreportGump.AddHTMLGump( 180, 390, 335, 40, false, false, "<BASEFONT color=#ffffff>" + wrongProblem + "</BASEFONT>" );

	bugreportGump.AddText( 147, 228, 2727, enterBugReportText );
	bugreportGump.AddBackground( 147, 248, 400, 80, 9200 );
	bugreportGump.AddTextEntry( 147, 248, 400, 80, 0, 1, 40, " " ); // Bug Report
	bugreportGump.AddButton( 144, 340, 247, 248, 1, 0, 50 ); // Okay Button
	bugreportGump.AddButton( 425, 340, 2073, 2072, 1, 0, 0 );// Close Button

	bugreportGump.Send( pSock );
	bugreportGump.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	switch( pButton )
	{
		case 0:
			break;
		case 1: // General question for Counselors
			pSock.Page( 0 );
			break;
		case 2: // Stuck
			HandleStuckCharacter( pSock );
			break;
		case 3: // Report a bug
			BugReportGump( pSock );
			break;
		case 4: // Make suggestion
			pSock.OpenURL( shardURL );
			break;
		case 5: // Harassment
			pSock.Page( 4 ); // Harassment Page
			break;
		case 6: // Item Problem
			pSock.Page( 3 ); // Item Problem Page
			break;
		case 7: // Report exploit
			pSock.Page( 5 ); // Report Exploit Page
			break;
		case 8: // Other Problem
			pSock.Page( 1 ); // Free text
			break;
		case 9: // Account Management Gump
			ChangePasswordGump( pSock );
			break;
		case 20: // Change account password
		{
			var userAccount = pUser.account;
			var password = gumpData.getEdit( 0 );
			var confirmpassword = gumpData.getEdit( 1 );
			var letterNumber = /^[0-9a-zA-Z]+$/;
			var passwordmatch = password.match( letterNumber );

			if( !passwordmatch )
			{
				pUser.SysMessage( passwordRule1 ) // Passwords may only consist of letters (a-z, A-Z) and digits (0-9).
				DisplayHelpMenu( pUser )
				break;
			}
			if( password.length < 5 || password.length > 17 )
			{
				pUser.SysMessage( passwordRule2 ); // Your password must be 5 (min) to 17 (max) characters long.
				DisplayHelpMenu( pUser )
				break;
			}
			if( password != confirmpassword )
			{
				pUser.SysMessage( passwordRule3 ); // 'Confirm New Password' does not match 'New Password'! Note that passwords are cAsE sEnSiTiVe.
				DisplayHelpMenu( pUser )
				break;
			}
			if( password == confirmpassword )
			{
				pUser.SysMessage( passwordStatus ); // The new password has been saved to your account.
				userAccount.password = password;
				break;
			}
			break;
		}
		case 30: // Teleport to random safe location
			TeleportToRandomSafeLocation( pSock );
			break;
		case 31: // Page GM about being stuck
			pSock.Page( 2 ) // Stuck
			break;
		case 50: // Submit bug report
			var bugreportText = gumpData.getEdit( 0 );
			pUser.ExecuteCommand( "reportbug " + bugreportText );
			break;
		case 100: // Reopen main help gump
			DisplayHelpMenu( pUser );
			break;
	}
}

function HandleStuckCharacter( pSock )
{
	var sLanguage = pSock.language;
	var pUser = pSock.currentChar;
	var stuckTeleEnabled = false;
	var stuckTeleExhausted = false;

	const gumpTitle = "<BIG><CENTER><U>" + shardName + " " + GetDictionaryEntry( 17000, sLanguage ) + "</U></CENTER></BIG>"; // Help Menu
	const teleToRndSafeLoc = GetDictionaryEntry( 17040, sLanguage ); // Teleport to Random Safe Location
	var teleToRndSafeLocText = GetDictionaryEntry( 17042, sLanguage ); // Selecting this option will teleport you to a random safe location (town or shrine) within your current facet. This can only be used a limited amount of times (%i) per day.
	teleToRndSafeLocText = ( teleToRndSafeLocText.replace( /%i/gi, maxSafeTeleportsPerDay.toString() ));
	const teleToNearestSafeLoc = GetDictionaryEntry( 17041, sLanguage ); // Teleport to Nearest Safe Location
	var teleToNearestSafeLocText = GetDictionaryEntry( 17043, sLanguage ); //Selecting this option will teleport you to the nearest safe location (town or shrine) within your current facet. This can only be used a limited amount of times (%i) per day.
	teleToNearestSafeLocText = ( teleToNearestSafeLocText.replace( /%i/gi, maxSafeTeleportsPerDay.toString() ));
	const submitStuckPage = GetDictionaryEntry( 17044, sLanguage ); // Submit GM Page about Being Stuck
	const submitStuckPageText = GetDictionaryEntry( 17045, sLanguage ); // Selecting this option will submit a support ticket about your situation. Please note that response time might vary depending on the number of GMs available.
	const safeTeleExhausted = GetDictionaryEntry( 17046, sLanguage ); // Automatic teleports exhausted for today. Please rely on GM support system for now.
	const wrongProblem = GetDictionaryEntry( 17029, sLanguage );

	// Check if auto-teleport system is enabled
	if( maxSafeTeleportsPerDay > 0 )
	{
		if( pUser.innocent || ( pUser.murderer && murderersCanTeleport ))
		{
			// Mark auto-teleport system as enabled
			stuckTeleEnabled = true;

			// Check when player's last teleport happened
			var lastStuckTeleTime = new Date( parseInt( pUser.GetTag( "lastStuckTeleTime" )) * 1000 );
			var today = new Date();
			if( lastStuckTeleTime.getUTCDate() != today.getUTCDate() )
			{
				// Last teleport happened on some day in the past. Reset teleport count!
				pUser.SetTag( "stuckTeleCount", null );
			}
			else
			{
				// Check if player has exhausted the available teleports for the day
				var stuckTeleCount = pUser.GetTag( "stuckTeleCount" );
				if( stuckTeleCount >= maxSafeTeleportsPerDay )
				{
					stuckTeleExhausted = true;
				}
			}
		}
	}

	// Display gump allowing player to choose between teleporting to a random safe
	// location within their facet, or calling a GM for assistance
	var stuckCharGump = new Gump();

	// Add shared background
	stuckCharGump.AddBackground( 50, 25, 540, 240, 5054 );
	stuckCharGump.AddCheckerTrans( 50, 25, 540, 240 );
	stuckCharGump.AddPage( 0 );
	stuckCharGump.AddGump( 50, 25, 5545 );
	stuckCharGump.AddGump( 530, 25, 5545 );
	stuckCharGump.AddHTMLGump( 50, 25, 540, 40, false, false, "<center> <BASEFONT color=#ffffff>" + gumpTitle + "</BASEFONT> </center>");

	// A different button must be shown if connected client is of version 4.x or lower
	if( pSock.clientMajorVer < 5 )
	{
		stuckCharGump.AddButton( 425, 230, 2073, 2072, 1, 0, 0 );// Close Button
	}
	else
	{
		stuckCharGump.AddButton( 425, 230, 12006, 12007, 1, 0, 0 );// Close Button
	}

	stuckCharGump.AddButton( 150, 220, 5537, 5538, 1, 0, 100 );
	stuckCharGump.AddHTMLGump( 180, 220, 335, 40, false, false, "<BASEFONT color=#ffffff>" + wrongProblem + "</BASEFONT>" );

	if( stuckTeleEnabled && !stuckTeleExhausted )
	{
		// Auto-teleport option
		stuckCharGump.AddButton( 80, 110, 5540, 5541, 1, 0, 30 );
		if( teleportToNearestSafeLocation )
		{
			stuckCharGump.AddHTMLGump( 110, 110, 450, 74, false, false, "<BASEFONT color=#ffffff>" + teleToNearestSafeLoc + "</BASEFONT>" );
			stuckCharGump.AddHTMLGump( 110, 130, 450, 67, true, false, teleToNearestSafeLocText );
		}
		else
		{
			stuckCharGump.AddHTMLGump( 110, 110, 450, 74, false, false, "<BASEFONT color=#ffffff>" + teleToRndSafeLoc + "</BASEFONT>" );
			stuckCharGump.AddHTMLGump( 110, 130, 450, 67, true, false, teleToRndSafeLocText );
		}
	}
	else
	{
		stuckCharGump.AddButton( 80, 110, 5540, 5541, 1, 0, 31 );
		stuckCharGump.AddHTMLGump( 110, 110, 450, 74, false, false, "<BASEFONT color=#ffffff>" + submitStuckPage + "</BASEFONT>" );
		stuckCharGump.AddHTMLGump( 110, 130, 450, 67, true, false, submitStuckPageText );

		if( stuckTeleEnabled )
		{
			// Player has exhausted daily teleports, show GM Page option only
			stuckCharGump.AddHTMLGump( 110, 90, 450, 18, false, false, "<BASEFONT color=red>" + safeTeleExhausted + "</BASEFONT>" );
		}
	}

	stuckCharGump.Send( pSock );
	stuckCharGump.Free();
}

function CanPlayerTeleport( pSock, pUser )
{
	var sLanguage = pSock.language;
	if( pUser.criminal )
	{
		pSock.SysMessage( GetDictionaryEntry( 17050, sLanguage )); // You cannot use safe teleport while flagged as a criminal!
		return false;
	}
	else if( pUser.atWar )
	{
		pSock.SysMessage( GetDictionaryEntry( 17051, sLanguage )); // You cannot use safe teleport while engaged in combat!
		return false;
	}
	else if( pUser.isJailed )
	{
		pSock.SysMessage( GetDictionaryEntry( 17052, sLanguage )); // You cannot use safe teleport while in jail!
		return false;
	}

	// Are any of the user's pets in combat?
	var petList = pUser.GetPetList();
	for( let i = 0; i < petList.length; i++ )
	{
		var tempPet = petList[i];
		if( ValidateObject( tempPet ) && tempPet.atWar )
		{
			pSock.SysMessage( GetDictionaryEntry( 17053, sLanguage )); // You cannot use safe teleport while your followers are engaged in combat!
			return false;
		}
	}

	return true;
}

function TeleportToRandomSafeLocation( pSock )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( pUser.GetTempTag( "waitingForTeleport" ))
	{
		pSock.SysMessage( GetDictionaryEntry( 17053, pSock.language )); // You have already activated safe teleport! Please wait...
		return;
	}

	if( CanPlayerTeleport( pSock, pUser ))
	{
		pUser.frozen = true;
		if( instantTeleport )
		{
			// Instant teleport, so call onTimer directly
			onTimer( pUser, 1 );
		}
		else
		{
			// Delayed teleport, start some timers
			pUser.SetTempTag( "waitingForTeleport", true );
			pUser.TextMessage( GetDictionaryEntry( 17054, pSock.language ), false, 0x3b2, 0, pUser.serial ); // [Teleporting to safety in about half a minute...]
			pUser.StartTimer( RandomNumber( 27000, 33000 ), 1, true );
			pUser.StartTimer( RandomNumber( 13000, 17000 ), 2, true );
		}
	}
}

function onTimer( timerObj, timerID )
{
	if( !ValidateObject( timerObj ))
		return;

	var pSock = timerObj.socket;
	if( pSock == null )
		return;

	// Re-run the checks from before start of timer
	if( !CanPlayerTeleport( pSock, timerObj, true ))
	{
		timerObj.frozen = false;
		return;
	}

	if( timerID == 2 )
	{
		timerObj.TextMessage( GetDictionaryEntry( 17055, pSock.language ), false, 0x3b2, 0, timerObj.serial ); // [Teleporting to safety in about 15 seconds...]
		return;
	}

	// Everything seems to be in order, teleport the player to safety!
	PerformTeleportation( pSock, timerObj );
}

function PerformTeleportation( pSock, pUser )
{
	if( !ValidateObject( pUser ))
		return;

	pSock.SysMessage( GetDictionaryEntry( 17047, pSock.language )); // Teleporting you to safe location...
	pUser.SetTempTag( "waitingForTeleport", null );
	pUser.frozen = false;
	var dateNow = ( Date.now() / 1000 | 0 );
	pUser.SetTag( "lastStuckTeleTime", dateNow.toString() );
	pUser.SetTag( "stuckTeleCount", pUser.GetTag( "stuckTeleCount" ) + 1 );

	var worldNum = pUser.worldnumber;
	var closestDist = 0;
	var closestLoc = null;

	// Handle T2A and Dungeons in Felucca/Trammel
	if( worldNum == 0 || worldNum == 1 )
	{
		if( pUser.x >= 5120 )
		{
			if( pUser.x <= 6144 && pUser.y >= 2304 )
			{
				// User is in Lost Lands, teleport them to either Papua or Delucia
				var teleToPapua = true;
				if( teleportToNearestSafeLocation )
				{
					// Check distance to Inn in Papua
					var distance = DistanceBetween( pUser.x, pUser.y, 5575, 3175 );
					closestDist = distance;

					// Now check distance to Inn in Delucia, and if it's closer, use that instead
					distance = DistanceBetween( pUser.x, pUser.y, 5207, 4066 );
					if( distance < closestDist )
					{
						closestDist = distance;
						teleToPapua = false;
					}
				}
				else
				{
					// Randomize which town to teleport to
					teleToPapua = ( RandomNumber( 0, 1 ) ? true : false );
				}

				if( teleToPapua )
				{
					// Inn in Papua
					closestLoc[0] = 5575;
					closestLoc[1] = 3175;
					closestLoc[2] = -3;
				}
				else
				{
					// Inn in Delucia
					closestLoc[0] = 5207;
					closestLoc[1] = 4066;
					closestLoc[2] = 36;
				}
			}
		}
	}

	// Is player in a dungeon?
	if( pUser.region != null && pUser.region.isDungeon )
	{
		 if( pUser.murderer )
		 {
		 	// Teleport murderers in dungeons to a random chaos shrine/out-of-town-area
		 	closestLoc = safeChaosShrineLocations[worldNum][RandomNumber( 0, safeChaosShrineLocations.length - 1 )];
		 }
		 else
		 {
		 	// Teleport players in dungeons to a random shrine/out-of-town area
		 	closestLoc = safeShrineLocations[worldNum][RandomNumber( 0, safeShrineLocations.length - 1 )];
		 }
	}

	// Player is neither in T2A, nor in a dungeon
	var validSafeLocations = safeTownLocations[worldNum];
	if( teleportToNearestSafeLocation )
	{
		if( pUser.murderer )
		{
			// Teleport murderers to a random chaos shrine/out-of-town-area
			closestLoc = safeChaosShrineLocations[worldNum][RandomNumber( 0, safeChaosShrineLocations.length - 1 )];
		}
		else
		{
			for( let i = 0; i < validSafeLocations.length; i++ )
			{
				// Loop through all valid safe locations and determine which one is closer to the player
				var distance = DistanceBetween( pUser.x, pUser.y, validSafeLocations[i][0], validSafeLocations[i][1] );
				if( closestDist == 0 || distance < closestDist )
				{
					// New nearest location found! Update variables
					closestDist = distance;
					closestLoc = validSafeLocations[i];
				}
			}
		}
	}
	else
	{
		if( pUser.murderer )
		{
			// Teleport murderers to a random chaos shrine/out-of-town-area
			closestLoc = safeChaosShrineLocations[worldNum][RandomNumber( 0, safeChaosShrineLocations.length - 1 )];
		}
		else
		{
			// Teleport player to random safe location
			var safeLoc = validSafeLocations[RandomNumber( 0, validSafeLocations.length - 1 )];
			pUser.Teleport( safeLoc[0], safeLoc[1], safeLoc[2] );
		}
	}

	if( closestLoc != null )
	{
		// Teleport player to nearest safe location
		pUser.Teleport( closestLoc[0], closestLoc[1], closestLoc[2] );
	}

	// Play a sound effect
	pSock.SoundEffect( 0x01FC, true );

	// Player is unable to engage in fights for the next 5 minutes after making use of the
	// safe teleport option
	pUser.setPeace = 10;
	pUser.AddScriptTrigger( 7000 ); // Attach script that prevents entering combat mode

	// Also teleport the player's pets/followers
	var petList = pUser.GetPetList();
	for( let i = 0; i < petList.length; i++ )
	{
		var tempPet = petList[i];
		if( ValidateObject( tempPet ))
		{
			tempPet.Teleport( pUser.x, pUser.y, pUser.z );
			tempPet.setPeace = 10;
		}
	}
}

const safeTownLocations = [
	// Felucca Britannia Towns
	[
		[1501, 1627, 10], // Britain
		[2250, 1235, 0], // Cove
		[2474, 408, 15], // Minoc
		[626, 2243, 0], // Skara Brae
		[1837, 2745, 0], // Trinsic
		[2778, 985, 0], // Vesper
		[629, 858, 0] // Yew
	],

	// Trammel Britannia Towns
	[
		[1501, 1627, 10], // Britain
		[2250, 1235, 0], // Cove
		[2474, 408, 15], // Minoc
		[626, 2243, 0], // Skara Brae
		[1837, 2745, 0], // Trinsic
		[2778, 985, 0], // Vesper
		[629, 858, 0] // Yew
	],

	// Ilshenar "Towns"
	[
		//[947, 641, -80] // Ver Lor Reg (Inaccessible in modern clients, but existed in 7.0.15.x and earlier)
		[1564, 1058, -6], // Twin Oaks Tavern
		[1518, 625, -16], // Desert Outpost (West)
		[1625, 550, -19] // Desert Outpost (East)
	],

	// Malas Towns
	[
		[1012, 513, -70], // Luna
		[2055, 1320, -90] // Umbra
	],

	// Tokuno Islands Towns
	[
		[672, 1221, 25] // Zento (Makoto-Jima)
	],

	// Ter Mur Towns
	[
		[729, 3488, -20] // Royal City
	]
];

const safeShrineLocations = [
	// Felucca Britannia Shrines
	[
		[1856, 872, -1], // Compassion
		[1730, 3528, 0], // Honor
		[1301, 639, 15], // Justice
		[3355, 299, 9], // Sacrifice
		[1602, 2486, 4] // Spirituality
	],

	// Trammel Britannia Shrines
	[
		[1856, 872, -1], // Compassion
		[1730, 3528, 0], // Honor
		[1301, 639, 15], // Justice
		[3355, 299, 9], // Sacrifice
		[1602, 2486, 4] // Spirituality
	],

	// Ilshenar Shrines
	[
		[1210, 477, -13], // Compassion
		[1106, 404, -80], // Gargoyle
		[722, 1354, -60], // Honesty
		[747, 730, -29], // Honor
		[287, 1013, 0], // Humility
		[986, 1004, -35], // Justice
		[1173, 1288, -30], // Sacrifice
		[1526, 1340, 0], // Spirituality
		[529, 213, -41] // Valor
	],

	// Malas Shrines
	[
		[1066, 1437, -90] // Only shrine in Malas is inside the city of Luna, so let's teleport players to Hansel's hostel instead
	],

	// Tokuno Islands Shrines
	[
		[296, 711, 55], // Homare
		[1045, 517, 15], // Isamu
		[718, 1160, 25] // Makoto
	],

	// Ter Mur Shrines
	[
		// Only shrine in Ter Mur is inside the Royal City, so teleport player to some beach in the north instead
		[693, 2917, 39]
	]
];

const safeChaosShrineLocations = [
	// Felucca Britannia Chaos Shrines
	[
		[1456, 852, 0] // Chaos
	],

	// Trammel Britannia Chaos Shrines
	[
		[1456, 852, 0] // Chaos
	],

	// Ilshenar Chaos Shrines
	[
		[1749, 234, 58] // Chaos
	],

	// Malas Chaos Shrines
	[
		[2418, 258, -90] // No Chaos shrines, so just teleport player to a remote area on the map
	],

	// Tokuno Islands Chaos Shrines
	[
		[278, 699, 44] // No Chaos shrines, so just teleport player to a remote area on the map
	],

	// Ter Mur Chaos Shrines
	[
		[674, 4013, -43] // No Chaos shrines, so just teleport player to a remote area on the map
	]
];
