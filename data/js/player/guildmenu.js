// --- Ranks page constants ---
var PAGE_RANKS = 5;

// text entries on ranks page
var TE_NEW_RANK_NAME = 100;  // text entry id for new rank name
var TE_NEW_RANK_PRIO = 101;  // text entry id for new rank priority
var TE_SET_RANK_NAME = 102;  // per-member set rank name (shared single box)

// buttons on ranks page
var BTN_ADD_RANK      = 15000;
var BTN_REFRESH       = 15001;

// per-rank delete buttons: 15100 + row
var BTN_DEL_RANK_BASE = 15100;

// per-member controls:
var BTN_PROMOTE_BASE  = 15200; // 15200 + row
var BTN_DEMOTE_BASE   = 15300; // 15300 + row
var BTN_SET_RANK_BASE = 15400; // 15400 + row

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
	if( !guild || !mChar )
		return "";

	// Check recruits list
	var recs = guild.recruits || [];
	for( var i = 0; i < recs.length; i++ )
	{
		if( recs[i] && recs[i].serial === mChar.serial )
			return "Recruit";
	}

	// Check members list
	var mems = guild.members || guild.member || [];
	for( var j = 0; j < mems.length; j++ )
	{
		if( mems[j] && mems[j].serial === mChar.serial )
			return "Member";
	}

	// Not found in either list
	return "";
}

function GetOnlineStatus( mChar )
{
	// Typical check: if socket exists, consider online
	// Adjust if your shard exposes a direct online flag
	return ( mChar && mChar.socket ) ? "Online" : "Offline";
}

function GetRosterOffset( pUser )
{
    var off = pUser.GetTag ? ( pUser.GetTag( "recruitOffset" )|0 ) : 0;
    if( off < 0 )
		off = 0;
    return off;
}
function SetRosterOffset( pUser, off )
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
    guildMenu.AddButton( 520, 520, 0xFAE, 0xFB0, 1, 0, 14003 ); // Invite button

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
        guildMenu.AddButton(200, 520, 0xFA5, 0xFA7, 1, 0, 14001 );
    if( end < recruits.length )
        guildMenu.AddButton(250, 520, 0xFA5, 0xFA7, 1, 0, 14002 );

    // Rows
    for( var i = offset, row = 0; i < end; i++, row++ )
    {
        var c = recruits[i];
        var name = ( c && c.name ) ? c.name : ( "0x" + (c ? c.serial.toString(16).toUpperCase() : "00000000") );

        // name
        guildMenu.AddHTMLGump( 200, rowStartY + row*rowHeight, 200, 22, false, false, "<basefont color=#ffffff>" + name + "</basefont>" );

        // Accept
        guildMenu.AddButton( 420, rowStartY + row*rowHeight, 0xFB7, 0xFB9, 1, 0, 12000 + row );

        // Reject
        guildMenu.AddButton( 465, rowStartY + row*rowHeight, 0xFB4, 0xFB6, 1, 0, 13000 + row );
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
	guildMenu.AddHTMLGump( 30, 130, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>" );
	guildMenu.AddPageButton( 135, 130, 0xFA5, 5 );
	guildMenu.AddPicture( -5, 130, 0x0FC0 );

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
	guildMenu.AddHTMLGump( 30, 130, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>" );
	guildMenu.AddPageButton( 135, 130, 0xFA5, 5 );
	guildMenu.AddPicture( -5, 130, 0x0FC0 );
	guildMenu.AddHTMLGump( 240, 120, 300, 35, false, false, "<basefont color=#ffffff>Name</basefont>" );
	guildMenu.AddHTMLGump( 380, 120, 300, 35, false, false, "<basefont color=#ffffff>Rank</basefont>" );
	guildMenu.AddHTMLGump( 500, 120, 300, 35, false, false, "<basefont color=#ffffff>Status</basefont>" );
	// Header labels are already added above; now print rows
	if( members.length === 0 )
	{
		guildMenu.AddHTMLGump( 240, rowStartY, 320, 22, false, false, "<basefont color=#c0c0c0>No members found</basefont>" );
	}
	else
	{
		var showCount = Math.min( maxRows, members.length );
		for( var i = 0; i < showCount; i++ )
		{
			var m = members[i];
			var name = ( m && m.name ) ? m.name : ( "0x" + ( m ? m.serial.toString( 16 ).toUpperCase() : "00000000" ));
			var rank = GetRankName( guildinfo, m );
			var status = GetOnlineStatus( m );

			// Name column
			guildMenu.AddHTMLGump( 240, rowStartY + (i * rowHeight), 120, 22, false, false, "<basefont color=#ffffff>" + name + "</basefont>" );
			// Rank column
			guildMenu.AddHTMLGump( 380, rowStartY + (i * rowHeight), 100, 22, false, false, "<basefont color=#ffffff>" + rank + "</basefont>" );
		// Status column (green/red for quick scanning)
		var statusColor = ( status === "Online" ) ? "#00ff00" : "#ff4040";
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
	RenderRecruitList( guildMenu, pUser );

	// ===== Page 5: RANKS (fixed layout) =====
	guildMenu.AddPage(PAGE_RANKS);

	// --- layout constants ---
	var L = 190;             // left panel X
	var R = 390;             // right panel X
	var T = 140;             // panels top
	var W = 180;             // panel width
	var H = 360;             // panel height
	var ROW_H = 22;          // row height
	var PAD_X = 8;           // inner padding
	var PAD_Y = 6;

	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPicture(240, 15, 0x0ED4);

	// left nav (same as other pages)
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);

	guildMenu.AddHTMLGump(30, 60, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 60, 0xFA5, 3);
	guildMenu.AddPicture(-5, 60, 0x0FC0);

	guildMenu.AddHTMLGump(30, 100, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 100, 0xFA5, 4);
	guildMenu.AddPicture(-5, 100, 0x0FC0);

	// section headers
	guildMenu.AddHTMLGump(L, 120, 160, 24, false, false, "<basefont color=#ffffff>Ranks</basefont>");
	guildMenu.AddHTMLGump(R, 120, 160, 24, false, false, "<basefont color=#ffffff>Members</basefont>");

	// panels
	guildMenu.AddBackground(L, T, W, H, 0x2486);
	guildMenu.AddBackground(R, T, W+20, H, 0x2486);

// --- ranks header row ---
guildMenu.AddHTMLGump(L + PAD_X, T + PAD_Y, W - 2*PAD_X, 18, false, false,
    "<basefont color=#111111>Name</basefont>");
guildMenu.AddHTMLGump(L + PAD_X, T + PAD_Y + 16, 60, 18, false, false,
    "<basefont color=#111111>Prio</basefont>");

var nRanks = (guildinfo && guildinfo.NumRanks) ? guildinfo.NumRanks() : 0;
var maxRankRows = Math.max(0, Math.min(12, Math.floor((H - 110) / ROW_H)));

var shown = 0;
for (var id = 0; id < nRanks && shown < maxRankRows; ++id) {
    // pull name/prio (bindings should return "" / -1 for deleted)
    var rName = guildinfo.GetRankNameById ? guildinfo.GetRankNameById(id) : "";
    var rPrio = guildinfo.GetRankPrioById ? guildinfo.GetRankPrioById(id) : -1;

    // skip tombstoned/deleted ranks
    if (!rName || rName === "(deleted)" || rPrio < 0 || rPrio >= 2147480000)
        continue;

    var y = T + 46 + shown * ROW_H;

    guildMenu.AddHTMLGump(L + PAD_X, y, 110, 18, false, false,
        "<basefont color=#111111>" + rName + "</basefont>");
    guildMenu.AddHTMLGump(L + PAD_X + 115, y, 30, 18, false, false,
        "<basefont color=#111111>" + rPrio + "</basefont>");

    // IMPORTANT: encode the *rankId* (not the row) in the button id
    guildMenu.AddButton(L + W - PAD_X - 20, y, 0xFB4, 0xFB6, 1, 0,
        BTN_DEL_RANK_BASE + id);

    shown++;
}


	// --- add rank area (bottom of left panel) ---
	var AY = T + H - 84; // start of add-area
	guildMenu.AddHTMLGump(L + PAD_X, AY, W - 2*PAD_X, 18, false, false,"<basefont color=#111111>Add Rank</basefont>");

	guildMenu.AddHTMLGump(L + PAD_X, AY + 20, 40, 18, false, false,"<basefont color=#111111>Name</basefont>");

	guildMenu.AddHTMLGump(L + PAD_X, AY + 42, 40, 18, false, false,"<basefont color=#111111>Prio</basefont>");

	// add / refresh buttons centered
	guildMenu.AddButton(L + W - PAD_X - 42, AY + 40, 0xFB7, 0xFB9, 1, 0, BTN_ADD_RANK);
	guildMenu.AddButton(L + W - PAD_X - 20, AY + 40, 0xFA5, 0xFA7, 1, 0, BTN_REFRESH);

	// --- members header & rows ---
	guildMenu.AddHTMLGump(R + PAD_X, T + PAD_Y, 160, 18, false, false,"<basefont color=#111111>Name / Rank</basefont>");

	var members = (guildinfo && (guildinfo.member || guildinfo.members)) || [];
	var maxMemberRows = Math.floor((H - 64) / ROW_H);
	maxMemberRows = Math.max(0, Math.min(12, maxMemberRows));

	for (var r = 0; r < Math.min(members.length, maxMemberRows); ++r)
	{
	   var m = members[r];
	   var y2 = T + 34 + r*ROW_H;

		var mName = (m && m.name) ? m.name
              : ("0x" + (m ? m.serial.toString(16).toUpperCase() : "00000000"));
		var mRank = (guildinfo.GetRankName) ? guildinfo.GetRankName(m)
              : GetRankName(guildinfo, m);

		// name
		guildMenu.AddHTMLGump(R + PAD_X, y2, 120, 18, false, false,"<basefont color=#111111>" + mName + "</basefont>");
		// rank (smaller)
		guildMenu.AddHTMLGump(R + PAD_X + 120, y2, 70, 18, false, false,"<small><basefont color=#666666>" + (mRank || "(none)") + "</basefont></small>");

		// buttons aligned in a neat column on the right
		var bx = R + W + 20 - PAD_X - 20;  // inside right panel
		guildMenu.AddButton(bx, y2,     0xFAE, 0xFB0, 1, 0, BTN_PROMOTE_BASE + r); // up
		guildMenu.AddButton(bx-22, y2,  0xFA5, 0xFA7, 1, 0, BTN_DEMOTE_BASE + r); // down
		guildMenu.AddButton(bx-44, y2,  0xFB7, 0xFB9, 1, 0, BTN_SET_RANK_BASE + r); // set
	}

	// single “Set Rank (name)” entry at bottom right
	guildMenu.AddHTMLGump(R + PAD_X, T + H - 54, 160, 18, false, false,"<basefont color=#111111>Set Rank (name)</basefont>");


	guildMenu.AddTextEntryLimited(L + PAD_X + 44, AY + 20, W - 2*PAD_X - 44, 18, 0, 1, 100, "test", 32);
	guildMenu.AddTextEntryLimited(L + PAD_X + 44, AY + 42, 48, 18, 0, 1, 101, "1", 6);
	guildMenu.AddTextEntryLimited(R + PAD_X, T + H - 32, W - 2*PAD_X, 18, 0, 0, 103, "test", 32);

	guildMenu.Send( socket );
	guildMenu.Free();
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	if( pButton === 0 )
		return; // no button pressed, early out

	if( pButton === 1 ) // Create Guild
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
			return;
		}
		else
		{
			pUser.TextMessage( "Currently member of: " + pUser.guild.name, false, 0x3b2, 0, pUser.serial );
			return;
		}
	}
	var guildinfo = pUser.guild;

    // Not our page/buttons? early out.
    if( !guildinfo )
		return;

    // Paging
    if( pButton === 14001 || pButton === 14002 )
    {
        var recruits = guildinfo.recruits || [];
        var maxRows = 14;
        var offset = GetRosterOffset( pUser );

        if( pButton === 14001 )
            offset = Math.max( 0, offset - maxRows );
        else
            offset = Math.min(Math.max( 0, recruits.length - 1 ), offset + maxRows );

        SetRosterOffset( pUser, offset );

        // Refresh gump
        GuildMenu( pUser );
        return;
    }

    // Invite
    if( pButton === 14003 )
    {
		pSock.CustomTarget( 0, "Select a player to invite." );
        GuildMenu( pUser );
        return;
    }

    // Accept / Reject ranges
    if( pButton >= 12000 && pButton < ( 12000 + 1000 ))
    {
        var row = pButton - 12000;
        HandleRecruitAction( pSock, pUser, guildinfo, row, true );
        return;
    }
    if( pButton >= 13000 && pButton < ( 13000 + 1000 ))
    {
        var row = pButton - 13000;
        HandleRecruitAction( pSock, pUser, guildinfo, row, false );
        return;
    }

	// ===== RANKS PAGE HANDLERS =====

	// Only allow guild master (optional rule)
	if (pButton === BTN_ADD_RANK)
	{
    // add rank
	var newName = gumpData.getEdit(0);
	var prioStr = gumpData.getEdit(1)
	var prio = parseInt(prioStr, 10);
	if (isNaN(prio))
		prio = 0;

    //if (!newName.length)
	//{
     //   pSock.SysMessage("Enter a rank name.");
    //}
	//else
	//{
        //try
		//{
            guildinfo.AddRank(newName, prio);
            pSock.SysMessage("Added/updated rank: " + newName + " (prio " + prio + ")");
        //} 
		//catch (e) 
		//{
        //    pSock.SysMessage("Failed to add rank.");
        //}
    //}
		GuildMenu(pUser);
		return;
	}

	if (pButton === BTN_REFRESH)
	{
		GuildMenu(pUser);
		return;
	}

// delete rank (button carries *rankId*)
else if (pButton >= BTN_DEL_RANK_BASE && pButton < BTN_DEL_RANK_BASE + 100000) {
    var rankId = pButton - BTN_DEL_RANK_BASE;

    var ok = false;
    if (guildinfo.RemoveRankById) {
        try { ok = guildinfo.RemoveRankById(rankId); } catch (e) {}
    } else if (guildinfo.GetRankNameById && guildinfo.RemoveRankByName) {
        var delName = guildinfo.GetRankNameById(rankId);
        if (delName && delName !== "(deleted)") {
            try { ok = guildinfo.RemoveRankByName(delName); } catch (e) {}
        }
    }

    pSock.SysMessage(ok ? "Rank removed." : "Could not remove (in use?)");
    GuildMenu(pUser);
    return;
}

// promote/demote member by row
else if (pButton >= BTN_PROMOTE_BASE && pButton < BTN_PROMOTE_BASE + 1000)
{
    var r = pButton - BTN_PROMOTE_BASE;
    var members = (guildinfo.member || guildinfo.members) || [];
    if (r >= 0 && r < members.length)
	{
        try { guildinfo.Promote(members[r]); pSock.SysMessage("Promoted " + (members[r].name||"member")); }
        catch(e){ pSock.SysMessage("Promote failed."); }
    }
    GuildMenu(pUser);
    return;
}
else if (pButton >= BTN_DEMOTE_BASE && pButton < BTN_DEMOTE_BASE + 1000) {
    var r2 = pButton - BTN_DEMOTE_BASE;
    var members2 = (guildinfo.member || guildinfo.members) || [];
    if (r2 >= 0 && r2 < members2.length) {
        try { guildinfo.Demote(members2[r2]); pSock.SysMessage("Demoted " + (members2[r2].name||"member")); }
        catch(e){ pSock.SysMessage("Demote failed."); }
    }
    GuildMenu(pUser);
    return;
}

// Set rank — 15400..15499
else if (pButton >= BTN_SET_RANK_BASE && pButton < BTN_SET_RANK_BASE + 100) {
    var r3 = pButton - BTN_SET_RANK_BASE;
    var members3 = (guildinfo.member || guildinfo.members) || [];

    // On this page: 0=new rank name, 1=new rank prio, 2="Set Rank (name)"
    var rankNameToSet = (gumpData.getEdit ? gumpData.getEdit(2) : "") || "";
    rankNameToSet = rankNameToSet.replace(/^\s+|\s+$/g, "");

    if (!rankNameToSet.length) {
        pSock.SysMessage("Enter a rank name first.");
        GuildMenu(pUser);
        return;
    }
    if (r3 >= 0 && r3 < members3.length) {
        try {
            var okSet = guildinfo.SetRank(members3[r3], rankNameToSet);
            pSock.SysMessage(okSet ? ("Set rank '" + rankNameToSet + "' for " + (members3[r3].name || "member"))
                                   : "SetRank failed (unknown rank?)");
        } catch(e) {
            pSock.SysMessage("SetRank failed.");
        }
    }
    GuildMenu(pUser);
    return;
}
}

function onCallback0( socket, target )
{
	var pUser = socket.currentChar;
	if( !pUser )
		return;

	var guild = pUser.guild;
	if( !guild )
	{
		socket.SysMessage( "You are not in a guild." );
		return;
	}

	if( !target || !target.isChar )
	{
		socket.SysMessage( "That is not a valid character." );
		return;
	}

	// Check if target already has a guild
	if( target.guild )
	{
		if( target.guild === guild )
		{
			socket.SysMessage( target.name + " is already in your guild." );
		}
		else
		{
			socket.SysMessage( target.name + " is already a member of another guild." );
		}
		return;
	}

	// Already a recruit?
	var recruits = guild.recruits || [];
	for( var i = 0; i < recruits.length; i++ )
	{
		if( recruits[i] && recruits[i].serial === target.serial )
		{
			socket.SysMessage( target.name + " is already a recruit." );
			return;
		}
	}

	// Passed checks: add as recruit
	try
	{
		guild.AddRecruit( target );
		socket.SysMessage( "Invited " + ( target.name || "player" ) + " as a recruit." );

		if( target.socket )
			target.socket.SysMessage( "You have been invited to join " + ( guild.name || "a guild" ) + " as a recruit." );
	}
	catch( e )
	{
		socket.SysMessage( "Failed to invite recruit." );
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
        SetRosterOffset( pUser, offset - 1 );

    // Refresh gump so the row list updates immediately
    GuildMenu( pUser );
}