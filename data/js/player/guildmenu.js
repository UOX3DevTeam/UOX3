function GuildCreation( pUser )
{
	var guildMenu = new Gump;
	var socket = pUser.socket;

	guildMenu.AddBackground( 0, 0, 380, 380, 0x6DB );
	guildMenu.AddCheckerTrans( 8, 8, 360, 360 );
	guildMenu.AddHTMLGump( 140, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Creation</basefont>" );
	guildMenu.AddPicture( 80, 15, 0x0ED4 );

	guildMenu.AddHTMLGump( 30, 100, 300, 140, false, false, "<basefont color=#ffffff>Thou art not bound to any guild. To establish thine own, provide a unique name and tender the standard guild registration fee.</basefont>" );
	
	guildMenu.AddPicture( 180, 170, 0x0EEF );
	guildMenu.AddHTMLGump( 30, 180, 300, 140, false, false, "<basefont color=#ffffff>Registration Fee:</basefont> <basefont color=#0fffff>25000</basefont>" );

	guildMenu.AddGump( 140, 220, 1803 );
	guildMenu.AddGump( 140, 260, 1803 );

	guildMenu.AddHTMLGump( 30, 220, 110, 70, false, false, "<basefont color=#ffffff>Enter Guild Name:</basefont>" );
	guildMenu.AddHTMLGump( 30, 240, 300, 140, false, false, "<SMALL><basefont color=#0fffff>Guild names may be up to 33 characters.</basefont></SMALL>" );

	guildMenu.AddHTMLGump( 30, 260, 110, 70, false, false, "<basefont color=#ffffff>Abbrevation:</basefont>" );
	guildMenu.AddHTMLGump( 30, 280, 300, 140, false, false, "<SMALL><basefont color=#0fffff>Guild Abbrevation may be up to 3 characters.</basefont></SMALL>" );

	guildMenu.AddTextEntryLimited( 140, 220, 200, 20, 0, 1, 9, pUser.name + "'s Guild", 33 );
	guildMenu.AddTextEntryLimited( 140, 260, 200, 20, 0, 2, 10, "NEW", 3 );

	guildMenu.AddButton( 210, 340, 0xF1, 0xF3, 1, 0, 0 );
	guildMenu.AddButton( 100, 340, 0xF7, 0xF8, 1, 0, 1 );

	guildMenu.Send( socket );
	guildMenu.Free();
}

function GetRankName( guild, mChar )
{
	// Try common places first; fall back to "Member"
	// Replace these getters if your shard stores rank differently.
	var r = ( mChar && mChar.GetTag ) ? mChar.GetTag( "guildRankName" ) : null;
	if( r && r.length )
		return r;

	// Example: if you have a guild API like GetMemberRankName(serial)
	if( guild && guild.GetMemberRankName )
	{
		try { r = guild.GetMemberRankName( mChar.serial ); } catch( e ) { /* ignore */ }
		if( r && r.length )
			return r;
	}

	// Example: if rank numeric tag exists
	var rn =  (mChar && mChar.GetTag ) ? mChar.GetTag( "guildRank" ) : null;
	if( rn != null )
	{
		switch( rn|0 )
		{
			case 3: return "Leader";
			case 2: return "Officer";
			case 1: return "Veteran";
			default: return "Member";
		}
	}

	return "Member";
}

function GetOnlineStatus( mChar )
{
	// Typical check: if socket exists, consider online
	// Adjust if your shard exposes a direct online flag
	return ( mChar && mChar.socket ) ? "Online" : "Offline";
}

// ---- Constants (top of script or near GuildMenu) ----
var BTN_ACCEPT_BASE = 12000;   // Accept buttons: 12000 + rowIndex
var BTN_REJECT_BASE = 13000;   // Reject buttons: 13000 + rowIndex
var BTN_PREV_RECR   = 14001;   // Prev page
var BTN_NEXT_RECR   = 14002;   // Next page
var BTN_INVITE      = 14003;   // Invite button (your existing one can use this id)

function GetRosterOffset( pUser )
{
    var off = pUser.GetTag ? ( pUser.GetTag( "recruitOffset" )|0 ) : 0;
    if (off < 0)
		off = 0;
    return off;
}
function SetRosterOffset(pUser, off)
{
    if( pUser.SetTag )
		pUser.SetTag( "recruitOffset", off|0 );
}

function RenderRecruitList( guildMenu, pUser )
{
    var guildinfo = pUser.guild;
    var recruits = ( guildinfo && guildinfo.recruits ) ? guildinfo.recruits : [];
    var rowStartY = 160;
    var rowHeight = 24;
    var maxRows = 14; // fits 600px page; tweak as needed

    // Header row (you already have main labels above this)
    guildMenu.AddHTMLGump( 200, 120, 240, 24, false, false, "<basefont color=#ffffff>View recruits</basefont>" );
    guildMenu.AddHTMLGump( 420, 120, 160, 24, false, false, "<basefont color=#ffffff>Accept / Reject</basefont>" );

    // Invite UI
    guildMenu.AddHTMLGump( 200, 520, 180, 24, false, false, "<basefont color=#ffffff>Invite new recruit</basefont>" );
    guildMenu.AddButton( 520, 520, 0xFAE, 0xFB0, 1, 0, BTN_INVITE ); // Invite button

    if (!recruits.length)
    {
        guildMenu.AddHTMLGump( 200, rowStartY, 320, 24, false, false, "<basefont color=#c0c0c0>No pending recruits</basefont>" );
        return;
    }

    var offset = GetRosterOffset( pUser );
    if ( offset >= recruits.length )
		offset = 0; // clamp if list shrank
    var end = Math.min( offset + maxRows, recruits.length );

    // Prev/Next page buttons
    if( offset > 0 )
        guildMenu.AddButton(200, 520, 0xFA5, 0xFA7, 1, 0, BTN_PREV_RECR );
    if( end < recruits.length )
        guildMenu.AddButton(250, 520, 0xFA5, 0xFA7, 1, 0, BTN_NEXT_RECR );

    // Rows
    for( var i = offset, row = 0; i < end; i++, row++ )
    {
        var c = recruits[i];
        var name = ( c && c.name ) ? c.name : ( "0x" + (c ? c.serial.toString(16).toUpperCase() : "00000000") );

        // name
        guildMenu.AddHTMLGump( 200, rowStartY + row*rowHeight, 200, 22, false, false, "<basefont color=#ffffff>" + name + "</basefont>" );

        // Accept
        guildMenu.AddButton( 420, rowStartY + row*rowHeight, 0xFB7, 0xFB9, 1, 0, BTN_ACCEPT_BASE + row );

        // Reject
        guildMenu.AddButton( 465, rowStartY + row*rowHeight, 0xFB4, 0xFB6, 1, 0, BTN_REJECT_BASE + row );
    }
}

function GuildMenu( pUser )
{
	var guildMenu = new Gump;
	var socket = pUser.socket;
	var guildinfo = pUser.guild;
	var members = ( guildinfo && ( guildinfo.member || guildinfo.members )) || [];
	var rowStartY = 150;
	var rowHeight = 22;
	var maxRows = 16; // fits within your 600px height; tweak as needed

	guildMenu.AddPage( 1 );
	guildMenu.AddBackground( 0, 0, 600, 600, 0x6DB );
	guildMenu.AddBackground( 0, 0, 600, 100, 0x6DB );
	guildMenu.AddHTMLGump( 280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu</basefont>" );
	guildMenu.AddPicture( 240, 15, 0x0ED4 );
	guildMenu.AddBackground( 0, 0, 180, 600, 0x6DB );
	guildMenu.AddHTMLGump( 30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>" );
	guildMenu.AddPageButton( 135, 20, 0xFA5, 2 );
	guildMenu.AddPicture( -5, 20, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 60, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>" );
	guildMenu.AddPageButton( 135, 60, 0xFA5, 3 );
	guildMenu.AddPicture( -5, 60, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 100, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>" );
	guildMenu.AddPageButton( 135, 100, 0xFA5, 4 );
	guildMenu.AddPicture( -5, 100, 0x0FC0 );

	guildMenu.AddHTMLGump( 350, 120, 300, 35, false, false, "<basefont color=#ffffff>Guild News</basefont>" );
	guildMenu.AddBackground( 230, 150, 320, 300, 0x2486 );
	guildMenu.AddHTMLGump( 240, 160, 300, 35, false, false, "<basefont color=#1111111>news Test hey look i posted news</basefont>" );
	guildMenu.AddHTMLGump( 310, 500, 300, 35, false, false, "<basefont color=#ffffff>Guild Message of the Day</basefont>" );
	guildMenu.AddBackground( 230, 520, 320, 50, 0x2486 );
	guildMenu.AddHTMLGump( 240, 530, 300, 35, false, false, "<basefont color=#1111111>I own you today my slaves</basefont>" );

	guildMenu.AddPage( 2 );
	guildMenu.AddBackground( 0, 0, 600, 600, 0x6DB );
	guildMenu.AddBackground( 0, 0, 600, 100, 0x6DB );
	guildMenu.AddHTMLGump( 280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu Page 2</basefont>" );
	guildMenu.AddPicture( 240, 15, 0x0ED4 );
	guildMenu.AddBackground( 0, 0, 180, 600, 0x6DB );
	guildMenu.AddHTMLGump( 30, 20, 300, 35, false, false, "<basefont color=#0fffff>Guild Roster</basefont>" );
	guildMenu.AddPageButton( 135, 20, 0xFA5, 1 );
	guildMenu.AddPicture( -5, 20, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 60, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>" );
	guildMenu.AddPageButton( 135, 60, 0xFA5, 3 );
	guildMenu.AddPicture( -5, 60, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 100, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>" );
	guildMenu.AddPageButton( 135, 100, 0xFA5, 4 );
	guildMenu.AddPicture( -5, 100, 0x0FC0 );
	guildMenu.AddHTMLGump( 240, 120, 300, 35, false, false, "<basefont color=#ffffff>Name</basefont>" );
	guildMenu.AddHTMLGump( 380, 120, 300, 35, false, false, "<basefont color=#ffffff>Rank</basefont>" );
	guildMenu.AddHTMLGump( 500, 120, 300, 35, false, false, "<basefont color=#ffffff>Status</basefont>" );
	// Header labels are already added above; now print rows
	if( members.length === 0 )
	{
		guildMenu.AddHTMLGump( 240, rowStartY, 320, 22, false, false,
			"<basefont color=#c0c0c0>No members found</basefont>" );
	}
	else
	{
		var showCount = Math.min( maxRows, members.length );
		for( var i = 0; i < showCount; i++ )
		{
			var m = members[i];
			var name = (m && m.name) ? m.name : ("0x" + (m ? m.serial.toString(16).toUpperCase() : "00000000"));
			var rank = GetRankName( guildinfo, m );
			var status = GetOnlineStatus( m );

			// Name column
			guildMenu.AddHTMLGump( 240, rowStartY + (i * rowHeight), 120, 22, false, false, "<basefont color=#ffffff>" + name + "</basefont>" );
			// Rank column
			guildMenu.AddHTMLGump( 380, rowStartY + (i * rowHeight), 100, 22, false, false, "<basefont color=#ffffff>" + rank + "</basefont>" );
		// Status column (green/red for quick scanning)
		var statusColor = (status === "Online") ? "#00ff00" : "#ff4040";
		guildMenu.AddHTMLGump( 500, rowStartY + (i * rowHeight), 80, 22, false, false, "<basefont color=" + statusColor + ">" + status + "</basefont>" );
		}
	}

	guildMenu.AddPage( 3 );
	guildMenu.AddBackground( 0, 0, 600, 600, 0x6DB );
	guildMenu.AddBackground( 0, 0, 600, 100, 0x6DB );
	guildMenu.AddHTMLGump( 280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu Page 3</basefont>" );
	guildMenu.AddPicture( 240, 15, 0x0ED4 );
	guildMenu.AddBackground( 0, 0, 180, 600, 0x6DB );
	guildMenu.AddHTMLGump( 30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>" );
	guildMenu.AddPageButton( 135, 20, 0xFA5, 2 );
	guildMenu.AddPicture( -5, 20, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 60, 300, 35, false, false, "<basefont color=#0fffff>Guild Information</basefont>" );
	guildMenu.AddPageButton( 135, 60, 0xFA5, 1 );
	guildMenu.AddPicture( -5, 60, 0x0FC0 );
	guildMenu.AddHTMLGump( 30, 100, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>" );
	guildMenu.AddPageButton( 135, 100, 0xFA5, 4 );
	guildMenu.AddPicture( -5, 100, 0x0FC0 );
	guildMenu.AddPicture( 220, 120, 0x0FBD );
	guildMenu.AddHTMLGump( 280, 120, 300, 35, false, false, "<basefont color=#ffffff>Guild Name: " + guildinfo.name.toString() +  "</basefont>" );
	guildMenu.AddHTMLGump( 280, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Abbreviation: " + guildinfo.abbreviation.toString() +  "</basefont>" );
	var guildType = guildinfo.type;
	var type = "";
	var typeID = 0;
	switch( guildType )
	{
		case 0: type = "Standard"; typeID = 0x1BC7; break;
		case 1: type = "Order"; typeID = 0x1BC4; break;
		case 2: type = "Chaos"; typeID = 0x1BC3; break;
	}
	guildMenu.AddPicture( 220, 160, typeID );
	guildMenu.AddHTMLGump( 280, 160, 300, 35, false, false, "<basefont color=#ffffff>Guild Type: " + type +  "</basefont>" );
	guildMenu.AddHTMLGump( 280, 180, 300, 35, false, false, "<basefont color=#ffffff>Guild Member Count: " + guildinfo.numMembers.toString() +  "</basefont>" );
	guildMenu.AddHTMLGump( 280, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruit Count: " + guildinfo.numRecruits.toString() +  "</basefont>" );

	guildMenu.AddPage(4);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu Page 2</basefont>");
	guildMenu.AddPicture(240, 15, 0x0ED4);

	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);

	guildMenu.AddHTMLGump(30, 60, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 60, 0xFA5, 3);
	guildMenu.AddPicture(-5, 60, 0x0FC0);

	guildMenu.AddHTMLGump(30, 100, 300, 35, false, false, "<basefont color=#0fffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 100, 0xFA5, 1);
	guildMenu.AddPicture(-5, 100, 0x0FC0);

	// Render recruits list + per-row buttons
	RenderRecruitList(guildMenu, pUser);

	guildMenu.Send( socket );
	guildMenu.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	switch( pButton )
	{
		case 0:// abort and do nothing
			break;
		case 1:
		{
			var Text1 = gumpData.getEdit(0);
			var Text2 = gumpData.getEdit(1);
			if( pUser.guild == null )
			{
				pUser.TextMessage( "Not currently in a guild... Creating new guild...", false, 0x3b2, 0, pUser.serial );
				var newGuild = CreateNewGuild();
				if( newGuild )
				{
					newGuild.name = Text1;
					newGuild.abbreviation = Text2;
					newGuild.type = 0;
					newGuild.AddMember( pUser );
					newGuild.master = pUser;
					pUser.guildTitle = "Guild Master";
					pUser.TextMessage( "Guild automatically created: " + newGuild.name, false, 0x3b2, 0, pUser.serial );
				}
				break;
			}
			else
			{
			   pUser.TextMessage( "Currently member of: " + pUser.guild.name, false, 0x3b2, 0, pUser.serial );
			   break;
			}
		}
	}
	var guildinfo = pUser.guild;

    // Not our page/buttons? early out.
    if( !guildinfo )
		return;

    // Paging
    if( pButton === BTN_PREV_RECR || pButton === BTN_NEXT_RECR )
    {
        var recruits = guildinfo.recruits || [];
        var maxRows = 14;
        var offset = GetRosterOffset( pUser );

        if( pButton === BTN_PREV_RECR )
            offset = Math.max( 0, offset - maxRows );
        else
            offset = Math.min(Math.max( 0, recruits.length - 1 ), offset + maxRows );

        SetRosterOffset( pUser, offset );

        // Refresh gump
        GuildMenu( pUser );
        return;
    }

    // Invite
    if( pButton === BTN_INVITE )
    {
        // If you later add a text-entry for name, read it here:
        // var name = gumpData.getText(0).trim();
        // Do your invite flow here.
		pSock.CustomTarget( 0, "Select a player to invite." );
        pSock.SysMessage( "Invite flow not implemented yet." );
        GuildMenu( pUser );
        return;
    }

    // Accept / Reject ranges
    if( pButton >= BTN_ACCEPT_BASE && pButton < ( BTN_ACCEPT_BASE + 1000 ))
    {
        var row = pButton - BTN_ACCEPT_BASE;
        HandleRecruitAction( pSock, pUser, guildinfo, row, true );
        return;
    }
    if( pButton >= BTN_REJECT_BASE && pButton < ( BTN_REJECT_BASE + 1000 ))
    {
        var row = pButton - BTN_REJECT_BASE;
        HandleRecruitAction( pSock, pUser, guildinfo, row, false );
        return;
    }
}

function onCallback0( socket, ourObj )
{
	var pUser = socket.currentChar;
	// Fetch myChar's guild
	var myGuild = pUser.guild;
	if( myGuild != null )
	{
		// Add ourObj as recruit in myChar's guild
		myGuild.AddRecruit( ourObj );
	}
}

function HandleRecruitAction( pSock, pUser, guildinfo, row, doAccept )
{
    var recruits = guildinfo.recruits || [];
    var offset = GetRosterOffset( pUser );
    var idx = offset + row;

    if( idx < 0 || idx >= recruits.length )
    {
        pSock.SysMessage( "Recruit entry no longer available." );
        GuildMenu( pUser );
        return;
    }

    var c = recruits[idx];

    if( doAccept )
    {
        // Your API: AcceptRecruit(pChar)
        try
		{
            guildinfo.AcceptRecruit( c );
            pSock.SysMessage( "Accepted " + ( c.name || "recruit" ) + "." );
        }
		catch (e)
		{
            pSock.SysMessage( "Failed to accept recruit." );
        }
    }
    else
    {
        // Adjust to your API; commonly something like RemoveRecruit(serial) or DeclineRecruit(char)
        try 
		{
			guildinfo.RemoveRecruit( c );
            pSock.SysMessage("Rejected " + (c.name || "recruit") + ".");
        } 
		catch (e)
		{
            pSock.SysMessage( "Failed to reject recruit." );
        }
    }

    // Optional: keep offset sane if list shrank
    if( offset >= ( recruits.length - 1 ) && offset > 0 )
        SetRosterOffset(pUser, offset - 1 );

    // Refresh gump so the row list updates immediately
    GuildMenu( pUser );
}