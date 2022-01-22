const ShardName = "UOX3";
const GumpTitle = "<BIG><CENTER><U>" + ShardName + " Help Menu</U></CENTER></BIG>";

const Websites = "<H3><u>Websites</u> - Provides a list of links to various important websites.</H3>";
const UOX3Website = "<H3><u>UOX3 website link.</u></H3>" + "<A HREF=\" http://www.uox3.org ></a> +";
const ShardWebsite = "<H3><u>Shard website link.</u></H3>" + "<A HREF=\" http://www.uox3.org ></a> +";
const UOWesbite = "<H3><u>UO website link.</u></H3>" + "<A HREF=\" http://www.uo.com ></a> +";
const Harassment = "http://www.uox3.org";
const Support = "http://www.uox3.org";
const exploitbug = "http://www.uox3.org";

const Generaltitle = "<H3><u>General question about " + ShardName + ".</u></H3>";
const General = "Select this option if you have a general gameplay question.</H3>";
const Stucktitle = "<H3><u>My character is physically stuck in the game.</u>";
const Stuck = "This choice only covers cases where your character is physically stuck in a location they cannot move out of.</H3>";
const Harrassmenttitle = "<H3><u>Another player is harassing me.</u><H3>"; 
const Harrassment = "<H3>This will give you a option of what type of harrassment to report.</H3>";
const Othertitle = "<H3><u>Other.</u></H3>";
const Other = "<H3>If you are experiencing a problem in the game that does not fall into one of the other categories</H3>";
const Reportbugtitle = "<H3><u>Report a bug</u></H3>";
const ReportBug = "<H3>Use this option to launch your web browser and take you to the Discussion Forums.</H3>";
const Suggestiontitle = "<H3><u>Suggestion for the Game.</u></H3>";
const Suggestion = "<H3>If you'd like to make a suggestion for the game, Choosing this option will take you to the Discussion Forums.<H3>";
const Accmanagementtitle = "<H3><u>Account Management</u></H3>";
const Accmanagement = "<H3>This is for changing account password</H3>";
const Serverinfotitle ="<H3><u>Server Info.</u></H3>";
const ServerInfo = "<H3>Here you will find the server information</H3>";

const Wrongproblem = "NO  - I meant to ask for help with another matter.";

const Verbalharrasment = "<U><CENTER>Another player is harassing me (or Exploiting).</CENTER></U><BR>" +
            "* VERBAL HARASSMENT<BR>" +
            "* Use this option when another player is verbally harassing your character." +
            "* Verbal harassment behaviors include but are not limited to, using bad language, threats etc.." +
            "* Before you submit a complaint be sure you understand what constitutes harassment" +
            "* <A HREF=\"" + Harassment+"></a>" + "– what is verbal harassment? -" +
            "* and that you have followed these steps:<BR>" +
            "* 1. You have asked the player to stop and they have continued.<BR>" +
            "* 2. You have tried to remove yourself from the situation.<BR>" +
            "* 3. You have done nothing to instigate or further encourage the harassment.<BR>" +
            "* 4. You have added the player to your ignore list." +
            "* <A HREF=\""+ Harassment + "></A>" + "- How do I ignore a player?<BR>" +
            "* 5. You have read and understand Origin’s definition of harassment.<BR>" +
            "* 6. Your account information is up to date. (Including a current email address)<BR>" +
            "* *If these steps have not been taken, GMs may be unable to take action against the offending player.<BR>" +
            "* **A chat log will be review by a GM to assess the validity of this complaint." +
            "* Abuse of this system is a violation of the Rules of Conduct.<BR>" +
            "* EXPLOITING<BR>" +
            "* Use this option to report someone who may be exploiting or cheating." +
            "* <A HREF=\""+ exploitbug +"></a>" + "– What constitutes an exploit?";

const Physicalharrassment = "<U><CENTER>Another player is harassing me using game mechanics.</CENTER></U><BR>" +
            "* <BR>" +
            "* PHYSICAL HARASSMENT<BR>" +
            "* Use this option when another player is harassing your character using game mechanics." +
            "* Physical harassment includes but is not limited to luring, Kill Stealing, and any act that causes a players death in Trammel." +
            "* Before you submit a complaint be sure you understand what constitutes harassment" +
            "* <A HREF=\""+ Harassment + "></A>" + " – what is physical harassment?" +
            "* and that you have followed these steps:<BR>" +
            "* 1. You have asked the player to stop and they have continued.<BR>" +
            "* 2. You have tried to remove yourself from the situation.<BR>" +
            "* 3. You have done nothing to instigate or further encourage the harassment.<BR>" +
            "* 4. You have added the player to your ignore list." +
            "* <A HREF=\""+ Harassment + "></A>" + " - how do I ignore a player?<BR>" +
            "* 5. You have read and understand Origin’s definition of harassment.<BR>" +
            "* 6. Your account information is up to date. (Including a current email address)<BR>" +
            "* *If these steps have not been taken, GMs may be unable to take action against the offending player.<BR>" +
            "* **This issue will be reviewed by a GM to assess the validity of this complaint." +
            "* Abuse of this system is a violation of the Rules of Conduct.";

const ServerStatus = "Server Statues Here";
const ServerVersion = "Server Version info Here";

function helppgump( pUser )
{
	var socket = pUser.socket;
	var helppgump = new Gump;

	background( helppgump );

	helppgump.AddPage( 1 );

	helppgump.AddButton( 80, 90, 5540, 5541, 1, 0, 0 );
    helppgump.AddHTMLGump( 110, 90, 450, 74,  false, false, "<BASEFONT color=#ffffff>" + Generaltitle + "</BASEFONT>" );
	helppgump.AddHTMLGump( 110, 110, 450, 30,  true, false, General );

	helppgump.AddButton( 80, 170, 5540, 5541, 1, 0, 0 );
	helppgump.AddHTMLGump( 110, 170, 450, 74, false, false, "<BASEFONT color=#ffffff>" +Stucktitle + "</BASEFONT>" );
    helppgump.AddHTMLGump( 110, 190, 450, 45, true, false, Stuck );

	helppgump.AddPageButton( 80, 250, 5540, 5541, 3 );
	helppgump.AddHTMLGump( 110, 250, 450, 74, false, false, "<BASEFONT color=#ffffff>" + Harrassmenttitle + "</BASEFONT>" );
	helppgump.AddHTMLGump( 110, 270, 450, 30, true, false, Harrassment);

    helppgump.AddPageButton( 80, 330, 5540, 5541, 2 );
	helppgump.AddHTMLGump( 110, 330, 450, 74, false, false, "<BASEFONT color=#ffffff>" + Othertitle + "</BASEFONT>" );
	helppgump.AddHTMLGump( 110, 350, 450, 45, true, false, Other );

	helppgump.AddPage( 2 );

    helppgump.AddButton( 80, 90, 5540, 5541, 1, 0, 0 );
	helppgump.AddHTMLGump( 110, 90, 450, 74, false, false, "<BASEFONT color=#ffffff>" + Reportbugtitle + "</BASEFONT>" );
    helppgump.AddHTMLGump( 110, 110, 450, 45, true, false, ReportBug );

    helppgump.AddButton( 80, 170, 5540, 5541, 1, 0, 0 );
	helppgump.AddHTMLGump( 110, 170, 450, 74, false, false, "<BASEFONT color=#ffffff>" + Suggestiontitle + "</BASEFONT>" );
    helppgump.AddHTMLGump( 110, 190, 450, 45, true, false, Suggestion );

    helppgump.AddPageButton( 80, 250, 5540, 5541, 6 );
	helppgump.AddHTMLGump( 110, 250, 450, 74, false, false, "<BASEFONT color=#ffffff>" + Accmanagementtitle + "</BASEFONT>" );
    helppgump.AddHTMLGump( 110, 270, 450, 30, true, false, Accmanagement );

    helppgump.AddPageButton( 80, 330, 5540, 5541, 4 );
	helppgump.AddHTMLGump( 110, 330, 450, 74, false, false, "<BASEFONT color=#ffffff>" + Serverinfotitle + "</BASEFONT>" );
    helppgump.AddHTMLGump( 110, 350, 450, 30, true, false, ServerInfo );

	helppgump.AddPageButton( 150, 390, 5540, 5541, 1 );
    helppgump.AddHTMLGump( 180, 390, 335, 40, false, false,"<BASEFONT color=#ffffff>" + Wrongproblem + "</BASEFONT>" );

	helppgump.AddPage( 3 );

	helppgump.AddButton( 80, 90, 5540, 5541, 1, 0, 0 );
    helppgump.AddHTMLGump( 110, 90, 450, 145, true, true, Verbalharrasment );

	helppgump.AddButton( 80, 240, 5540, 5541, 1, 0, 0 );
    helppgump.AddHTMLGump( 110, 240, 450, 145, true, true, Physicalharrassment ); 

    helppgump.AddPageButton( 150, 390, 5540, 5541, 1 );
    helppgump.AddHTMLGump( 180, 390, 335, 40, true, false, Wrongproblem );

	helppgump.AddPage( 4 );

	//helppgump.AddButton( 80, 90, 5540, 5541, 1, 0, 0 );
    helppgump.AddHTMLGump( 110, 90, 450, 145, false, false, "<BASEFONT color=#ffffff>" + ServerVersion + "</BASEFONT>" );

	//helppgump.AddButton( 80, 240, 5540, 5541, 1, 0, 0 );
    helppgump.AddHTMLGump( 110, 240, 450, 145, false, false, "<BASEFONT color=#ffffff>" + ServerStatus + "</BASEFONT>" );

	helppgump.AddPageButton( 150, 390, 5540, 5541, 5 );
    helppgump.AddHTMLGump( 180, 390, 335, 40, false, false, "<BASEFONT color=#ffffff>" + Websites + "</BASEFONT>" );

	helppgump.AddPage( 5 );

	helppgump.AddButton( 80, 90, 5540, 5541, 1, 0, 0 );
    helppgump.AddHTMLGump( 110, 90, 450, 74,  false, false, "<BASEFONT color=#ffffff>" + UOX3Website + "</BASEFONT>" );
	//helppgump.AddHTMLGump( 110, 110, 450, 30,  true, false, UOX3Website );

	helppgump.AddButton( 80, 170, 5540, 5541, 1, 0, 0 );
	helppgump.AddHTMLGump( 110, 170, 450, 74, false, false, "<BASEFONT color=#ffffff>" + ShardWebsite + "</BASEFONT>" );
    //helppgump.AddHTMLGump( 110, 190, 450, 45, true, false, Stuck );

	helppgump.AddPageButton( 80, 250, 5540, 5541, 3 );
	helppgump.AddHTMLGump( 110, 250, 450, 74, false, false, "<BASEFONT color=#ffffff>" + UOX3Website + "</BASEFONT>" );
	//helppgump.AddHTMLGump( 110, 270, 450, 30, true, false, Harrassment);

	helppgump.AddPageButton( 150, 390, 5540, 5541, 1 );
    helppgump.AddHTMLGump( 180, 390, 335, 40, false, false,"<BASEFONT color=#ffffff>" + Wrongproblem + "</BASEFONT>" );

	helppgump.AddPage( 6 );

	helppgump.AddPageButton( 150, 390, 5540, 5541, 1 );
    helppgump.AddHTMLGump( 180, 390, 335, 40, false, false,"<BASEFONT color=#ffffff>" + Wrongproblem + "</BASEFONT>" );

	helppgump.AddText( 147, 218, 2727, "New Password:" );
	helppgump.AddText( 149, 246, 2727, "Confirm New Password:" );
	helppgump.AddBackground( 335, 219, 200, 20, 9200);
	helppgump.AddTextEntry( 335, 219, 200, 20, 1175, 0, 40, " " );//New Password:
	helppgump.AddBackground( 334, 245, 200, 20, 9200 );
	helppgump.AddTextEntry( 334, 245, 200, 20, 1175, 0, 41, " " );//Confirm New Password:
	helppgump.AddButton( 144, 281, 247, 248, 1, 0, 1 );// Okay Button

	helppgump.Send( pUser );
	helppgump.Free();
}

function background( helppgump )
{
	helppgump.AddBackground( 50, 25, 540, 430, 5054 );
	helppgump.AddCheckerTrans( 50, 25, 540, 430 );
    helppgump.AddPage( 0 );
	helppgump.AddGump( 50, 25, 5545 );
	helppgump.AddGump( 530, 25, 5545 );
    helppgump.AddHTMLGump( 250, 50, 360, 40, false, false,"<BASEFONT color=#ffffff>" + GumpTitle + "</BASEFONT>");
    helppgump.AddButton( 425, 415, 2073, 2072, 1, 0, 0 );// Close Button
}

function onGumpPress(pSock, pButton, gumpData)
{
	var pUser = pSock.currentChar;
	var userAccount = pUser.account;
	var password = gumpData.getEdit( 0 );
	var confirmpassword = gumpData.getEdit( 1 );
	var letterNumber = /^[0-9a-zA-Z]+$/;
	var passwordmatch = password.match( letterNumber );
	switch (pButton)
	{
		case 0:
			break;
		case 1:
			if( !passwordmatch )
			{
				pUser.SysMessage( "Passwords may only consist of letters (A - Z) and Digits (0 - 9)." );
				helppgump( pUser )
				break;
			}
			if( password.length <= 6 || password.length > 40 )
			{
				pUser.SysMessage( "Your Password must be at least 7 letters or numbers." );
				helppgump( pUser )
				break;
			}
			if( password != confirmpassword )
			{
				pUser.SysMessage( "The 'Confirm New Password' value does not match the 'New Password'. Remember it is cAsE sEnSiTiVe." );
				helppgump( pUser )
				break;
			}
			if( password == confirmpassword )
			{
				pUser.SysMessage( "You Set the new Password: " + password + "It could take up to 24 hours before new password is stored in the data base.");
				userAccount.password = password;
				break;
			}
			break;
	}
}