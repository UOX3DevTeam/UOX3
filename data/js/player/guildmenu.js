function GuildCreation(pUser)
{
	var guildCreation = new Gump;
	var socket = pUser.socket;

	guildCreation.AddBackground(0, 0, 380, 380, 0x6DB);
	guildCreation.AddCheckerTrans(8, 8, 360, 360);
	guildCreation.AddHTMLGump(140, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Creation</basefont>");
	guildCreation.AddPicture(80, 15, 0x0ED4);

	guildCreation.AddHTMLGump(30, 100, 300, 140, false, false, "<basefont color=#ffffff>Thou art not bound to any guild. To establish thine own, provide a unique name and tender the standard guild registration fee.</basefont>");

	guildCreation.AddPicture(180, 170, 0x0EEF);
	guildCreation.AddHTMLGump(30, 180, 300, 140, false, false, "<basefont color=#ffffff>Registration Fee:</basefont> <basefont color=#0fffff>25000</basefont>");

	guildCreation.AddGump(140, 220, 1803);
	guildCreation.AddGump(140, 260, 1803);

	guildCreation.AddHTMLGump(30, 220, 110, 70, false, false, "<basefont color=#ffffff>Enter Guild Name:</basefont>");
	guildCreation.AddHTMLGump(30, 240, 300, 140, false, false, "<SMALL><basefont color=#0fffff>Guild names may be up to 33 characters.</basefont></SMALL>");

	guildCreation.AddHTMLGump(30, 260, 110, 70, false, false, "<basefont color=#ffffff>Abbrevation:</basefont>");
	guildCreation.AddHTMLGump(30, 280, 300, 140, false, false, "<SMALL><basefont color=#0fffff>Guild Abbrevation may be up to 3 characters.</basefont></SMALL>");

	guildCreation.AddTextEntryLimited(140, 220, 200, 20, 0, 1, 7, pUser.name + "'s Guild", 33);
	guildCreation.AddTextEntryLimited(140, 260, 200, 20, 0, 2, 8, "NEW", 3);

	guildCreation.AddButton(210, 340, 0xF1, 0xF3, 1, 0, 0);
	guildCreation.AddButton(100, 340, 0xF7, 0xF8, 1, 0, 1);

	guildCreation.Send(socket);
	guildCreation.Free();
}

function GetRankName(guild, mChar)
{
	if (!guild || !mChar)
		return "";

	if (guild.GetRankName)
	{
		var r = guild.GetRankName(mChar);
		if (r && r.length)
			return r;
	}

	// Fallback: old recruit/member detection
	var recs = guild.recruits || [];
	for (var i = 0; i < recs.length; i++)
	{
		if (recs[i] && recs[i].serial === mChar.serial)
			return "Recruit";
	}

	var mems = guild.members || guild.member || [];
	for (var j = 0; j < mems.length; j++)
	{
		if (mems[j] && mems[j].serial === mChar.serial)
			return "Member";
	}

	return "";
}

function GetOnlineStatus(mChar)
{
	return (mChar && mChar.socket) ? "Online" : "Offline";
}

function GetRosterOffset(pUser)
{
	var off = pUser.GetTag ? (pUser.GetTag("recruitOffset") | 0) : 0;
	if (off < 0)
		off = 0;
	return off;
}
function SetRosterOffset(pUser, off)
{
	if (pUser.SetTag)
		pUser.SetTag("recruitOffset", off | 0);
}

function RenderRecruitList(guildMenu, pUser)
{
	var guildinfo = pUser.guild;
	var recruits = (guildinfo && guildinfo.recruits) ? guildinfo.recruits : [];
	var rowStartY = 160;
	var rowHeight = 24;
	var maxRows = 14;

	// Header row (you already have main labels above this)
	guildMenu.AddHTMLGump(200, 120, 240, 24, false, false, "<basefont color=#ffffff>View recruits</basefont>");
	guildMenu.AddHTMLGump(420, 120, 160, 24, false, false, "<basefont color=#ffffff>Accept / Reject</basefont>");

	// Invite UI
	guildMenu.AddHTMLGump(200, 520, 180, 24, false, false, "<basefont color=#ffffff>Invite new recruit</basefont>");
	guildMenu.AddButton(520, 520, 0xFAE, 0xFB0, 1, 0, 14003); // Invite button

	if (!recruits.length)
	{
		guildMenu.AddHTMLGump(200, rowStartY, 320, 24, false, false, "<basefont color=#c0c0c0>No pending recruits</basefont>");
		return;
	}

	var offset = GetRosterOffset(pUser);
	if (offset >= recruits.length)
		offset = 0;
	var end = Math.min(offset + maxRows, recruits.length);

	// Prev/Next page buttons
	if (offset > 0)
		guildMenu.AddButton(200, 520, 0xFA5, 0xFA7, 1, 0, 14001);
	if (end < recruits.length)
		guildMenu.AddButton(250, 520, 0xFA5, 0xFA7, 1, 0, 14002);

	// Rows
	for (var i = offset, row = 0; i < end; i++, row++)
	{
		var c = recruits[i];
		var name = (c && c.name) ? c.name : ("0x" + (c ? c.serial.toString(16).toUpperCase() : "00000000"));

		// name
		guildMenu.AddHTMLGump(200, rowStartY + row * rowHeight, 200, 22, false, false, "<basefont color=#ffffff>" + name + "</basefont>");

		// Accept
		guildMenu.AddButton(420, rowStartY + row * rowHeight, 0xFB7, 0xFB9, 1, 0, 12000 + row);

		// Reject
		guildMenu.AddButton(465, rowStartY + row * rowHeight, 0xFB4, 0xFB6, 1, 0, 13000 + row);
	}
}

function GuildMenu(pUser)
{
	var guildMenu = new Gump;
	var socket = pUser.socket;
	var guildinfo = pUser.guild;
	var motd = ReadGuildMOTD(guildinfo);

	if (!motd.length)
		motd = "No message of the day set.";

	var newsList = ReadGuildNews(guildinfo);
	if (!newsList.length)
		newsList = ["No guild news has been posted yet."];

	var newsBody = newsList.join("<br><br>");

	var members = (guildinfo && (guildinfo.member || guildinfo.members)) || [];
	var rowStartY = 150;
	var rowHeight = 22;
	var maxRows = 16;

	guildMenu.AddPage(1);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Menu</basefont>");
	guildMenu.AddPicture(230, 15, 0x0ED4); // 0xA581
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);
	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 3);
	guildMenu.AddPicture(-5, 50, 0x0FC0);
	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 4);
	guildMenu.AddPicture(-5, 80, 0x0FC0);
	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 5);
	guildMenu.AddPicture(-5, 110, 0x0FC0);
	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 6);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 7);
	guildMenu.AddPicture(-5, 170, 0x0FC0);

	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 8);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	guildMenu.AddHTMLGump(350, 120, 300, 35, false, false, "<basefont color=#ffffff>Guild News</basefont>");
	guildMenu.AddBackground(230, 150, 320, 300, 0x2486);
	guildMenu.AddHTMLGump(240, 160, 300, 800, false, false, "<basefont color=#1111111>" + newsBody + "</basefont>");
	guildMenu.AddHTMLGump(310, 500, 300, 35, false, false, "<basefont color=#ffffff>Guild Message of the Day</basefont>");
	guildMenu.AddBackground(230, 520, 320, 50, 0x2486);
	guildMenu.AddHTMLGump(240, 530, 300, 35, false, false, "<basefont color=#1111111>" + motd + "</basefont>");

	guildMenu.AddPage(2);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Roster</basefont>");
	guildMenu.AddPicture(230, 15, 0x0ED4);
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#0fffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 1);
	guildMenu.AddPicture(-5, 20, 0x0FC0);
	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 3);
	guildMenu.AddPicture(-5, 50, 0x0FC0);
	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 4);
	guildMenu.AddPicture(-5, 80, 0x0FC0);
	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 5);
	guildMenu.AddPicture(-5, 110, 0x0FC0);
	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 6);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 7);
	guildMenu.AddPicture(-5, 170, 0x0FC0);

	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 8);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	guildMenu.AddHTMLGump(240, 120, 300, 35, false, false, "<basefont color=#ffffff>Name</basefont>");
	guildMenu.AddHTMLGump(380, 120, 300, 35, false, false, "<basefont color=#ffffff>Rank</basefont>");
	guildMenu.AddHTMLGump(500, 120, 300, 35, false, false, "<basefont color=#ffffff>Status</basefont>");
	// Header labels are already added above; now print rows
	if (members.length === 0)
	{
		guildMenu.AddHTMLGump(240, rowStartY, 320, 22, false, false, "<basefont color=#c0c0c0>No members found</basefont>");
	}
	else
	{
		var showCount = Math.min(maxRows, members.length);
		for (var i = 0; i < showCount; i++)
		{
			var m = members[i];
			var name = (m && m.name) ? m.name : ("0x" + (m ? m.serial.toString(16).toUpperCase() : "00000000"));
			var rank = GetRankName(guildinfo, m);
			var status = GetOnlineStatus(m);
			var rowY = rowStartY + (i * rowHeight);

			// Name column
			guildMenu.AddHTMLGump(240, rowY, 120, 22, false, false, "<basefont color=#ffffff>" + name + "</basefont>");
			// Rank column
			guildMenu.AddHTMLGump(380, rowY, 100, 22, false, false, "<basefont color=#ffffff>" + rank + "</basefont>");
			// Status column (green/red for quick scanning)
			var statusColor = (status === "Online") ? "#00ff00" : "#ff4040";
			guildMenu.AddHTMLGump(500, rowY, 80, 22, false, false, "<basefont color=" + statusColor + ">" + status + "</basefont>");

			// Kick button (right edge)
			guildMenu.AddButton(560, rowY, 0xFB4, 0xFB6, 1, 0, 15500 + i);
		}
	}

	guildMenu.AddPage(3);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Information</basefont>");
	guildMenu.AddPicture(230, 15, 0x0ED4);
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);
	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#0fffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 1);
	guildMenu.AddPicture(-5, 50, 0x0FC0);
	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 4);
	guildMenu.AddPicture(-5, 80, 0x0FC0);
	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 5);
	guildMenu.AddPicture(-5, 110, 0x0FC0);
	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 6);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 7);
	guildMenu.AddPicture(-5, 170, 0x0FC0);

	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 8);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	guildMenu.AddPicture(220, 120, 0x0FBD);
	guildMenu.AddHTMLGump(280, 120, 300, 35, false, false, "<basefont color=#ffffff>Guild Name: " + guildinfo.name.toString() + "</basefont>");
	guildMenu.AddHTMLGump(280, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Abbreviation: " + guildinfo.abbreviation.toString() + "</basefont>");
	var guildType = guildinfo.type;
	var type = "";
	var typeID = 0;
	switch (guildType)
	{
		case 0: type = "Standard"; typeID = 0x1BC7; break;
		case 1: type = "Order"; typeID = 0x1BC4; break;
		case 2: type = "Chaos"; typeID = 0x1BC3; break;
	}
	guildMenu.AddPicture(220, 160, typeID);
	guildMenu.AddHTMLGump(280, 160, 300, 35, false, false, "<basefont color=#ffffff>Guild Type: " + type + "</basefont>");
	guildMenu.AddHTMLGump(280, 180, 300, 35, false, false, "<basefont color=#ffffff>Guild Member Count: " + guildinfo.numMembers.toString() + "</basefont>");
	guildMenu.AddHTMLGump(280, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruit Count: " + guildinfo.numRecruits.toString() + "</basefont>");

	guildMenu.AddPage(4);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Main Recruitment</basefont>");
	guildMenu.AddPicture(230, 15, 0x0ED4);

	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);

	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 3);
	guildMenu.AddPicture(-5, 50, 0x0FC0);

	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#0fffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 1);
	guildMenu.AddPicture(-5, 80, 0x0FC0);

	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 5);
	guildMenu.AddPicture(-5, 110, 0x0FC0);

	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 6);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 7);
	guildMenu.AddPicture(-5, 170, 0x0FC0);

	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 8);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	RenderRecruitList(guildMenu, pUser);

	guildMenu.AddPage(6);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPicture(230, 15, 0x0ED4);
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);
	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 3);
	guildMenu.AddPicture(-5, 50, 0x0FC0);
	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 4);
	guildMenu.AddPicture(-5, 80, 0x0FC0);
	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 5);
	guildMenu.AddPicture(-5, 110, 0x0FC0);
	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 7);
	guildMenu.AddPicture(-5, 170, 0x0FC0);
	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#0fffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 1);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 8);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	guildMenu.AddBackground(200, 140, 360, 450, 0x2486);
	guildMenu.AddHTMLGump(210, 150, 340, 20, false, false, "<basefont color=#111111>Guild Type</basefont>");

	var typeLabelStandard = "<basefont color=#111111>" + (guildinfo.type === 0 ? "[Standard]" : "Standard") + "</basefont>";
	var typeLabelOrder = "<basefont color=#111111>" + (guildinfo.type === 1 ? "[Order]" : "Order") + "</basefont>";
	var typeLabelChaos = "<basefont color=#111111>" + (guildinfo.type === 2 ? "[Chaos]" : "Chaos") + "</basefont>";

	// Standard
	guildMenu.AddButton(210, 150 + 20, 0xFA5, 0xFA7, 1, 0, 16100);
	guildMenu.AddHTMLGump(240, 150 + 20, 120, 20, false, false, typeLabelStandard);

	// Order
	guildMenu.AddButton(210, 150 + 42, 0xFA5, 0xFA7, 1, 0, 16101);
	guildMenu.AddHTMLGump(240, 150 + 42, 120, 20, false, false, typeLabelOrder);

	// Chaos
	guildMenu.AddButton(210, 150 + 64, 0xFA5, 0xFA7, 1, 0, 16102);
	guildMenu.AddHTMLGump(240, 150 + 64, 120, 20, false, false, typeLabelChaos);

	guildMenu.AddHTMLGump(210, 250, 340, 20, false, false, "<basefont color=#111111>Abbreviation (max 3 chars)</basefont>");
	guildMenu.AddHTMLGump(210, 295, 340, 20, false, false, "<basefont color=#111111>Guild Name</basefont>");
	var lineY = 340;
	guildMenu.AddHTMLGump(210, lineY, 340, 20, false, false, "<basefont color=#111111>Message of the Day</basefont>");
	lineY += 60;
	guildMenu.AddHTMLGump(210, lineY, 340, 20, false, false, "<basefont color=#111111>Guild News (3 Articals)</basefont>");

	guildMenu.AddHTMLGump(210, 490, 340, 20, false, false, "<basefont color=#111111>Artical 2</basefont>");
	guildMenu.AddHTMLGump(210, 530, 340, 20, false, false, "<basefont color=#111111>Artical 3</basefont>");

	//sy += 50;
	//guildMenu.AddHTMLGump(210, sy, 340, 20, false, false, "<basefont color=#111111>Guild Web Page</basefont>");
	//guildMenu.AddTextEntryLimited(210, sy + 18, 300, 20, 0, 0, 61003, " ", 127);

	//sy += 50;
	//guildMenu.AddHTMLGump(210, sy, 340, 20, false, false, "<basefont color=#111111>Guild Charter</basefont>");
	//guildMenu.AddTextEntryLimited(210, sy + 18, 300, 60, 0, 0, 61004, " ", 255);

	// save / cancel
	guildMenu.AddButton(400, 150, 0xFB7, 0xFB9, 1, 0, 16000); // Save
	guildMenu.AddHTMLGump(430, 150, 340, 40, false, false, "<basefont color=#111111>Save</basefont>");
	guildMenu.AddButton(470, 150, 0xFB7, 0xFB9, 1, 0, 16001); // Cancel
	guildMenu.AddHTMLGump(500, 150, 340, 40, false, false, "<basefont color=#111111>Cancel</basefont>");

	// -------------------------------------------------
	// PAGE 7 - Guild Wars
	// -------------------------------------------------
	guildMenu.AddPage(7);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPicture(230, 15, 0x0ED4);

	// Left nav (Wars highlighted)
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);

	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);

	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 3);
	guildMenu.AddPicture(-5, 50, 0x0FC0);

	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 4);
	guildMenu.AddPicture(-5, 80, 0x0FC0);

	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 5);
	guildMenu.AddPicture(-5, 110, 0x0FC0);

	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 6);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	// Wars (active)
	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#0fffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 1);
	guildMenu.AddPicture(-5, 170, 0x0FC0);

	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 8);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	// Main panel
	guildMenu.AddBackground(190, 120, 390, 430, 0x2486);

	// --- search area ---
	var searchFilter = GetWarSearchFilter(pUser);
	guildMenu.AddHTMLGump(200, 130, 360, 20, false, false, "<basefont color=#111111>Search guilds by name or abbreviation:</basefont>");

	// Search button (pButton = 17000)
	guildMenu.AddButton(430, 150, 0xFB7, 0xFB9, 1, 0, 17000);
	guildMenu.AddHTMLGump(460, 150, 80, 20, false, false, "<basefont color=#111111>Search</basefont>");

	// --- search results header ---
	guildMenu.AddHTMLGump(200, 180, 160, 20, false, false, "<basefont color=#111111>Matching Guilds</basefont>");
	guildMenu.AddHTMLGump(380, 180, 80, 20, false, false, "<basefont color=#111111>Relation</basefont>");
	guildMenu.AddHTMLGump(470, 180, 110, 20, false, false, "<basefont color=#111111>Ally / War</basefont>");

	var rowStartY = 205;
	var rowHeight = 22;
	var maxRows = 10;

	// Filtered list
	var matchGuilds = FilterGuildList(guildinfo, searchFilter);

	if (!matchGuilds.length)
	{
		guildMenu.AddHTMLGump(200, rowStartY, 360, 20, false, false, "<basefont color=#111111>No guilds match the current filter.</basefont>");
	}
	else
	{
		var shown = 0;
		for (var i = 0; i < matchGuilds.length && shown < maxRows; i++)
		{
			var g = matchGuilds[i];
			if (!g)
				continue;

			var gy = rowStartY + shown * rowHeight;
			var gName = g.name || ("Guild #" + g.id);
			var gAbbr = g.abbreviation ? " [" + g.abbreviation + "]" : "";

			guildMenu.AddHTMLGump(200, gy, 170, 20, false, false, "<basefont color=#111111>" + gName + gAbbr + "</basefont>");

			// Current relation
			var relText = "Unknown";
			var relColor = "#808080";

			// Debug info
			var debugLine = "";

			if (typeof CompareGuildByGuild !== "undefined")
			{
				var rel = CompareGuildByGuild(guildinfo.id, g.id);

				// 0 - Neutral, 1 - War, 2 - Allied, 3 - Unknown, 4 - Same
				if (rel === 0) { relText = "At Peace"; relColor = "#00a000"; }
				else if (rel === 1) { relText = "At War"; relColor = "#ff4040"; }
				else if (rel === 2) { relText = "Allied"; relColor = "#0080ff"; }
				else if (rel === 4) { relText = "Same"; relColor = "#808080"; }
			}
			else
			{
				if (typeof Console !== "undefined" && Console.Print)
					Console.Print("[GuildWars] CompareGuildByGuild is undefined in JS scope!");
			}

			guildMenu.AddHTMLGump(380, gy, 90, 20, false, false, "<basefont color=" + relColor + ">" + relText + "</basefont>");

			// Buttons: Ally / War
			// We encode the *index in matchGuilds* in the button id.
			guildMenu.AddButton(470, gy, 0xFB7, 0xFB9, 1, 0, 18000 + i); // Offer Alliance
			guildMenu.AddButton(505, gy, 0xFB4, 0xFB6, 1, 0, 19000 + i); // Declare War

			shown++;
		}
	}

	// --- current wars section ---
	var warsHeaderY = 205 + maxRows * rowHeight + 15;
	guildMenu.AddHTMLGump(200, warsHeaderY, 360, 20, false, false, "<basefont color=#111111>Guilds currently at war with you:</basefont>");

	var warsStartY = warsHeaderY + 20;
	var warsRowH = 20;
	var warsMaxRows = 8;

	var allGuilds = GetAllGuilds();
	var warRow = 0;

	if (!allGuilds || !allGuilds.length)
	{
		guildMenu.AddHTMLGump(200, warsStartY, 360, 20, false, false, "<basefont color=#111111>No other guilds found.</basefont>");
	}
	else
	{
		for (var j = 0; j < allGuilds.length && warRow < warsMaxRows; j++)
		{
			var wg = allGuilds[j];
			if (!wg || wg.id === guildinfo.id)
				continue;

			var relation = (typeof CompareGuildByGuild !== "undefined")
				? CompareGuildByGuild(guildinfo.id, wg.id) : 3;

			if (relation !== 1) // only wars
				continue;

			var wy = warsStartY + warRow * warsRowH;
			var wName = wg.name || ("Guild #" + wg.id);
			var wAbbr = wg.abbreviation ? " [" + wg.abbreviation + "]" : "";

			guildMenu.AddHTMLGump(200, wy, 220, 20, false, false, "<basefont color=#ff4040>" + wName + wAbbr + "</basefont>");

			// Offer peace button (encoded with index in allGuilds)
			guildMenu.AddButton(470, wy, 0xFA5, 0xFA7, 1, 0, 20000 + j);
			guildMenu.AddHTMLGump(500, wy, 80, 20, false, false, "<basefont color=#111111>Offer Peace</basefont>");

			warRow++;
		}

		if (warRow === 0)
		{
			guildMenu.AddHTMLGump(200, warsStartY, 360, 20, false, false, "<basefont color=#111111>Your guild is not currently at war with anyone.</basefont>");
		}

		// --- incoming relation requests section ---
		var reqHeaderY = warsStartY + warsMaxRows * warsRowH + 30;
		guildMenu.AddBackground(0, 600, 600, 100, 0x6DB);
		guildMenu.AddBackground(10, 610, 580, 80, 0x2486);
		guildMenu.AddHTMLGump(15, reqHeaderY - 30, 360, 20, false, false, "<basefont color=#111111>Pending relation requests for your guild:</basefont>");

		var reqStartY = reqHeaderY + 20;
		var reqRowH = 20;
		var reqMaxRows = 8;

		var incomingReqs = ReadGuildRelationRequests(guildinfo);

		if (!incomingReqs || !incomingReqs.length)
		{
			guildMenu.AddHTMLGump(15, reqStartY - 30, 360, 20, false, false, "<basefont color=#111111>No pending alliance/war/peace requests.</basefont>");
		}
		else
		{
			for (var ri = 0; ri < incomingReqs.length && ri < reqMaxRows; ri++)
			{
				var r = incomingReqs[ri];
				if (!r)
					continue;

				var fromGuild = FindGuildById(r.fromId);
				var ry = reqStartY + ri * reqRowH;

				var relText = "Unknown";
				if (r.relation === 0) relText = "Peace offer";
				else if (r.relation === 1) relText = "War declaration";
				else if (r.relation === 2) relText = "Alliance proposal";

				var nameText;
				if (fromGuild)
				{
					var n = fromGuild.name || ("Guild #" + fromGuild.id);
					var a = fromGuild.abbreviation ? " [" + fromGuild.abbreviation + "]" : "";
					nameText = n + a;
				}
				else
				{
					nameText = "Guild #" + r.fromId;
				}

				guildMenu.AddHTMLGump(15, ry - 30, 220, 20, false, false, "<basefont color=#111111>" + nameText + " - " + relText + "</basefont>");

				// Accept (button 21000 + ri)
				guildMenu.AddButton(430, ry - 30, 0xFB7, 0xFB9, 1, 0, 21000 + ri);
				guildMenu.AddHTMLGump(460, ry - 30, 40, 20, false, false, "<basefont color=#111111>OK</basefont>");

				// Decline (button 22000 + ri)
				guildMenu.AddButton(510, ry - 30, 0xFB4, 0xFB6, 1, 0, 22000 + ri);
				guildMenu.AddHTMLGump(540, ry - 30, 60, 20, false, false, "<basefont color=#111111>No</basefont>");
			}
		}
	}

	// -------------------------------------------------
	// PAGE 8 - Guild Fealty (vote for next guild master)
	// -------------------------------------------------
	guildMenu.AddPage(8);
	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(260, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPicture(240, 15, 0x0ED4);

	// Left nav Fealty highlighted
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);

	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);

	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 3);
	guildMenu.AddPicture(-5, 50, 0x0FC0);

	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 4);
	guildMenu.AddPicture(-5, 80, 0x0FC0);

	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 5);
	guildMenu.AddPicture(-5, 110, 0x0FC0);

	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 6);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 7);
	guildMenu.AddPicture(-5, 170, 0x0FC0);

	// Active: Guild Fealty
	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#0fffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 1);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	// Main panel
	guildMenu.AddBackground(190, 120, 390, 430, 0x2486);

	// Current fealty
	var currentFealtyTarget = GetGuildFealtyTarget(guildinfo, pUser);
	var currentFealtyText;

	if (currentFealtyTarget)
	{
		var cfName = currentFealtyTarget.name || ("0x" + currentFealtyTarget.serial.toString(16).toUpperCase());
		currentFealtyText = "You currently pledge fealty to: " + cfName;
	}
	else
	{
		currentFealtyText = "You have not pledged fealty to any guild member.";
	}

	guildMenu.AddHTMLGump(200, 130, 360, 40, false, false, "<basefont color=#111111>" + currentFealtyText + "</basefont>");

	// Clear vote button
	guildMenu.AddButton(200, 170, 0xFB4, 0xFB6, 1, 0, 23100);
	guildMenu.AddHTMLGump(235, 170, 200, 20, false, false, "<basefont color=#111111>Clear fealty vote</basefont>");

	// NEW: Change fealty button -> CustomTarget(1, ...)
	guildMenu.AddButton(200, 200, 0xFB7, 0xFB9, 1, 0, 23200);
	guildMenu.AddHTMLGump(235, 200, 320, 20, false, false, "<basefont color=#111111>Click, then target a guild member to pledge fealty to.</basefont>");

	// Member list (display only, no per-row buttons)
	guildMenu.AddHTMLGump(200, 235, 160, 20, false, false, "<basefont color=#111111>Guild Members</basefont>");
	guildMenu.AddHTMLGump(200, 255, 160, 20, false, false, "<basefont color=#111111>Name</basefont>");
	guildMenu.AddHTMLGump(380, 255, 80, 20, false, false, "<basefont color=#111111>Rank</basefont>");

	var fealtyMembers = (guildinfo.member || guildinfo.members) || [];
	var rowStartY = 280;
	var rowHeight = 22;
	var maxRows = 12;

	for (var fi = 0; fi < fealtyMembers.length && fi < maxRows; fi++)
	{
		var m = fealtyMembers[fi];
		if (!m)
			continue;

		var fy = rowStartY + fi * rowHeight;
		var mName = m.name || ("0x" + m.serial.toString(16).toUpperCase());
		var mRank = (m.guildTitle) ||
			(guildinfo.GetRankName ? guildinfo.GetRankName(m) : GetRankName(guildinfo, m)) || "(none)";

		var isCurrent = (currentFealtyTarget && currentFealtyTarget.serial === m.serial);
		var nameColor = isCurrent ? "#00a000" : "#111111";

		guildMenu.AddHTMLGump(200, fy, 170, 20, false, false, "<basefont color=" + nameColor + ">" + mName + "</basefont>");
		guildMenu.AddHTMLGump(380, fy, 80, 20, false, false, "<basefont color=#111111>" + mRank + "</basefont>");
	}

	guildMenu.AddPage(5);

	// --- layout constants ---
	var L = 190;             // left panel X
	var R = 372;             // right panel X
	var T = 140;             // panels top
	var W = 180;             // panel width
	var H = 360;             // panel height
	var ROW_H = 22;          // row height
	var PAD_X = 8;           // inner padding
	var PAD_Y = 6;

	guildMenu.AddBackground(0, 0, 600, 600, 0x6DB);
	guildMenu.AddBackground(0, 0, 600, 100, 0x6DB);
	guildMenu.AddHTMLGump(280, 40, 300, 35, false, false, "<basefont color=#ffffff>Guild Ranks</basefont>");
	guildMenu.AddPicture(230, 15, 0x0ED4);

	// left nav (same as other pages)
	guildMenu.AddBackground(0, 0, 180, 600, 0x6DB);
	guildMenu.AddHTMLGump(30, 20, 300, 35, false, false, "<basefont color=#ffffff>Guild Roster</basefont>");
	guildMenu.AddPageButton(135, 20, 0xFA5, 2);
	guildMenu.AddPicture(-5, 20, 0x0FC0);

	guildMenu.AddHTMLGump(30, 50, 300, 35, false, false, "<basefont color=#ffffff>Guild Information</basefont>");
	guildMenu.AddPageButton(135, 50, 0xFA5, 3);
	guildMenu.AddPicture(-5, 50, 0x0FC0);

	guildMenu.AddHTMLGump(30, 80, 300, 35, false, false, "<basefont color=#ffffff>Guild Recruitment</basefont>");
	guildMenu.AddPageButton(135, 80, 0xFA5, 4);
	guildMenu.AddPicture(-5, 80, 0x0FC0);

	guildMenu.AddHTMLGump(30, 110, 300, 35, false, false, "<basefont color=#0fffff>Guild Ranks</basefont>");
	guildMenu.AddPageButton(135, 110, 0xFA5, 1);
	guildMenu.AddPicture(-5, 110, 0x0FC0);

	guildMenu.AddHTMLGump(30, 140, 300, 35, false, false, "<basefont color=#ffffff>Guild Settings</basefont>");
	guildMenu.AddPageButton(135, 140, 0xFA5, 6);
	guildMenu.AddPicture(-5, 140, 0x0FC0);

	guildMenu.AddHTMLGump(30, 170, 300, 35, false, false, "<basefont color=#ffffff>Guild Wars</basefont>");
	guildMenu.AddPageButton(135, 170, 0xFA5, 7);
	guildMenu.AddPicture(-5, 170, 0x0FC0);

	guildMenu.AddHTMLGump(30, 200, 300, 35, false, false, "<basefont color=#ffffff>Guild Fealty</basefont>");
	guildMenu.AddPageButton(135, 200, 0xFA5, 8);
	guildMenu.AddPicture(-5, 200, 0x0FC0);

	// section headers
	guildMenu.AddHTMLGump(L, 120, 160, 24, false, false, "<basefont color=#ffffff>Ranks</basefont>");
	guildMenu.AddHTMLGump(R, 120, 160, 24, false, false, "<basefont color=#ffffff>Members</basefont>");

	// panels
	guildMenu.AddBackground(L, T, W, H, 0x2486);
	guildMenu.AddBackground(R, T, W + 35, H, 0x2486);

	// --- ranks header row ---
	guildMenu.AddHTMLGump(L + PAD_X, T + PAD_Y, W - 2 * PAD_X, 18, false, false, "<basefont color=#111111>Name</basefont>");
	guildMenu.AddHTMLGump(L + PAD_X + 110, T + PAD_Y, W - 2 * PAD_X, 18, false, false, "<basefont color=#111111>Prio</basefont>");

	var nRanks = (guildinfo && guildinfo.NumRanks) ? guildinfo.NumRanks() : 0;
	var maxRankRows = Math.max(0, Math.min(12, Math.floor((H - 110) / ROW_H)));

	var shown = 0;
	for (var id = 0; id < nRanks && shown < maxRankRows; ++id)
	{
		// pull name/prio (bindings should return "" / -1 for deleted)
		var rName = guildinfo.GetRankNameById ? guildinfo.GetRankNameById(id) : "";
		var rPrio = guildinfo.GetRankPrioById ? guildinfo.GetRankPrioById(id) : -1;

		// skip tombstoned/deleted ranks
		if (!rName || rName === "(deleted)" || rPrio < 0 || rPrio >= 2147480000)
			continue;

		var y = T + 46 + shown * ROW_H;

		guildMenu.AddHTMLGump(L + PAD_X, y, 110, 18, false, false, "<basefont color=#111111>" + rName + "</basefont>");
		guildMenu.AddHTMLGump(L + PAD_X + 115, y, 30, 18, false, false, "<basefont color=#111111>" + rPrio + "</basefont>");

		// IMPORTANT: encode the *rankId* (not the row) in the button id
		guildMenu.AddButton(L + W - PAD_X - 27, y, 0xFB4, 0xFB6, 1, 0, 15100 + id);

		shown++;
	}
	// --- add rank area (bottom of left panel) ---
	var AY = T + H - 84; // start of add-area
	guildMenu.AddHTMLGump(L + PAD_X, AY, W - 2 * PAD_X, 18, false, false, "<basefont color=#111111>Add Rank</basefont>");

	guildMenu.AddHTMLGump(L + PAD_X, AY + 20, 40, 18, false, false, "<basefont color=#111111>Name</basefont>");

	guildMenu.AddHTMLGump(L + PAD_X, AY + 42, 40, 18, false, false, "<basefont color=#111111>Prio</basefont>");

	// add / refresh buttons centered
	guildMenu.AddButton(L + W - PAD_X - 60, AY + 40, 0xFB7, 0xFB9, 1, 0, 15000);
	guildMenu.AddButton(L + W - PAD_X - 28, AY + 40, 0xFA5, 0xFA7, 1, 0, 15001);

	// --- members header & rows ---
	guildMenu.AddHTMLGump(R + PAD_X, T + PAD_Y, 160, 18, false, false, "<basefont color=#111111>Name / Rank</basefont>");
	guildMenu.AddHTMLGump(R + PAD_X + 115, T + PAD_Y, 160, 18, false, false, "<basefont color=#111111>Set / - / + </basefont>");

	var members = (guildinfo && (guildinfo.member || guildinfo.members)) || [];
	var maxMemberRows = Math.floor((H - 64) / ROW_H);
	maxMemberRows = Math.max(0, Math.min(12, maxMemberRows));

	for (var r = 0; r < Math.min(members.length, maxMemberRows); ++r)
	{
		var m = members[r];
		var y2 = T + 34 + r * ROW_H;

		var mName = (m && m.name) ? m.name : ("0x" + (m ? m.serial.toString(16).toUpperCase() : "00000000"));
		// Prefer guildTitle, fall back to engine / helper
		var mRank = (m && m.guildTitle) || (guildinfo.GetRankName ? guildinfo.GetRankName(m) : GetRankName(guildinfo, m)) || "(none)";

		// name
		guildMenu.AddHTMLGump(R + PAD_X, y2, 120, 18, false, false, "<basefont color=#111111>" + mName + "</basefont>");
		// rank (smaller)
		guildMenu.AddHTMLGump(R + PAD_X + 50, y2, 70, 18, false, false, "<small><basefont color=#2000000>" + (mRank || "(none)") + "</basefont></small>");

		// buttons aligned in a neat column on the right
		var bx = R + W + 20 - PAD_X - 20;
		guildMenu.AddButton(bx + 10, y2, 0xFAE, 0xFB0, 1, 0, 15200 + r); // up
		guildMenu.AddButton(bx - 20, y2, 0xFA5, 0xFA7, 1, 0, 15300 + r); // down
		guildMenu.AddButton(bx - 48, y2, 0xFB7, 0xFB9, 1, 0, 15400 + r); // set

	}

	guildMenu.AddHTMLGump(R + PAD_X, T + H - 54, 160, 18, false, false, "<basefont color=#111111>Set Rank (name)</basefont>");

	guildMenu.AddTextEntryLimited(L + PAD_X + 44, AY + 20, W - 2 * PAD_X - 44, 18, 0, 1, 620020, " ", 32);
	guildMenu.AddTextEntryLimited(L + PAD_X + 44, AY + 42, 48, 18, 0, 1, 630200, " ", 6);
	guildMenu.AddTextEntryLimited(R + PAD_X, T + H - 32, W - 2 * PAD_X, 18, 0, 0, 640200, " ", 32);

	guildMenu.AddPage(6);
	guildMenu.AddTextEntryLimited(210, 250 + 18, 250, 20, 0, 0, 61001, " ", 3);
	guildMenu.AddGump(210, 285, 0x60);
	guildMenu.AddTextEntryLimited(210, 295 + 18, 295, 20, 0, 0, 61002, " ", 33);
	guildMenu.AddGump(210, 330, 0x60);
	guildMenu.AddTextEntryLimited(210, 355, 300, 30, 0, 3, 61005, " ", 127);
	guildMenu.AddTextEntryLimited(210, 420, 320, 90, 0, 4, 61011, " ", 168);
	guildMenu.AddTextEntryLimited(210, 505, 320, 30, 0, 4, 61011, " ", 60);
	guildMenu.AddTextEntryLimited(210, 545, 320, 30, 0, 4, 61011, " ", 60);

	guildMenu.AddPage(7);
	guildMenu.AddTextEntryLimited(200, 150, 220, 20, 0, 0, 65001, " ", 32);

	guildMenu.Send(socket);
	guildMenu.Free();
}

/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress(pSock, pButton, gumpData)
{
	var pUser = pSock.currentChar;
	if (!pUser)
		return;

	if (pButton === 0)
		return; // no button pressed, early out

	if (pButton === 1) // Create Guild
	{
		var Text1 = manualTrim(gumpData.getEdit(0) || "");
		var Text2 = manualTrim(gumpData.getEdit(1) || "");
		if (!Text1.length)
		{
			pSock.SysMessage("Enter a guild name first.");
			return;
		}

		if (pUser.guild == null)
		{
			pUser.TextMessage("Not currently in a guild... Creating new guild...", false, 0x3b2, 0, pUser.serial);
			var newGuild = CreateNewGuild(pUser, Text1, Text2);
			if (newGuild)
			{
				pUser.TextMessage("Guild automatically created: " + newGuild.name, false, 0x3b2, 0, pUser.serial);
				pUser.Refresh();
			}
			else
			{
				pSock.SysMessage("Could not create guild.");
			}
			return;
		}
		else
		{
			pUser.TextMessage("Currently member of: " + pUser.guild.name, false, 0x3b2, 0, pUser.serial);
			return;
		}
	}
	var guildinfo = pUser.guild;

	// Not our page/buttons? early out.
	if (!guildinfo)
		return;

	// Paging
	if (pButton === 14001 || pButton === 14002)
	{
		var recruits = guildinfo.recruits || [];
		var maxRows = 14;
		var offset = GetRosterOffset(pUser);

		if (pButton === 14001)
			offset = Math.max(0, offset - maxRows);
		else
			offset = Math.min(Math.max(0, recruits.length - 1), offset + maxRows);

		SetRosterOffset(pUser, offset);

		// Refresh gump
		GuildMenu(pUser);
		return;
	}

	// Invite
	if (pButton === 14003)
	{
		if (!CanInvite(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow inviting new recruits.");
			GuildMenu(pUser);
			return;
		}

		pSock.CustomTarget(0, "Select a player to invite.");
		GuildMenu(pUser);
		return;
	}

	// Accept / Reject ranges
	if (pButton >= 12000 && pButton < (12000 + 1000))
	{
		if (!CanInvite(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow managing recruits.");
			GuildMenu(pUser);
			return;
		}

		var row = pButton - 12000;
		HandleRecruitAction(pSock, pUser, guildinfo, row, true);
		return;
	}
	if (pButton >= 13000 && pButton < (13000 + 1000))
	{
		if (!CanInvite(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow managing recruits.");
			GuildMenu(pUser);
			return;
		}

		var row = pButton - 13000;
		HandleRecruitAction(pSock, pUser, guildinfo, row, false);
		return;
	}

	// ===== RANKS PAGE HANDLERS =====

	// Only allow guild master (optional rule)
	if (pButton === 15000)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can add or update ranks.");
			GuildMenu(pUser);
			return;
		}

		// add rank
		var newName = manualTrim(gumpData.getEdit(0) || "");
		if (!newName.length)
		{
			pSock.SysMessage("Enter a rank name first.");
			GuildMenu(pUser);
			return;
		}

		var prioStr = gumpData.getEdit(1)
		var prio = parseInt(prioStr, 10);
		if (isNaN(prio))
			prio = 0;

		guildinfo.AddRank(newName, prio);
		pSock.SysMessage("Added/updated rank: " + newName + " (prio " + prio + ")");
		GuildMenu(pUser);
		return;
	}

	if (pButton === 15001)
	{
		GuildMenu(pUser);
		return;
	}

	// delete rank (button carries *rankId*)
	else if (pButton >= 15100 && pButton < 15100 + 100)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can remove ranks.");
			GuildMenu(pUser);
			return;
		}

		var rankId = pButton - 15100;

		var ok = false;
		if (guildinfo.RemoveRankById)
		{
			ok = guildinfo.RemoveRankById(rankId);
		}
		else if (guildinfo.GetRankNameById && guildinfo.RemoveRankByName)
		{
			var delName = guildinfo.GetRankNameById(rankId);
			if (delName && delName !== "(deleted)")
			{
				ok = guildinfo.RemoveRankByName(delName);
			}
		}

		pSock.SysMessage(ok ? "Rank removed." : "Could not remove (in use?)");
		GuildMenu(pUser);
		return;
	}

	// promote/demote member by row
	else if (pButton >= 15200 && pButton < 15200 + 100)
	{
		if (!CanPromoteDemote(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow promoting members.");
			GuildMenu(pUser);
			return;
		}

		var r = pButton - 15200;
		var members = (guildinfo.member || guildinfo.members) || [];
		if (r >= 0 && r < members.length)
		{
			var target = members[r];

			// Do not allow the guild master to be changed via promote/demote
			if (IsGuildMaster(guildinfo, target))
			{
				pSock.SysMessage("The guild master cannot be promoted or demoted.");
			}
			else
			{
				guildinfo.Promote(target);
				pSock.SysMessage("Promoted " + (target.name || "member"));
				target.Refresh();
			}
		}
		GuildMenu(pUser);
		return;
	}
	else if (pButton >= 15300 && pButton < 15300 + 100)
	{
		if (!CanPromoteDemote(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow demoting members.");
			GuildMenu(pUser);
			return;
		}

		var r2 = pButton - 15300;
		var members2 = (guildinfo.member || guildinfo.members) || [];
		if (r2 >= 0 && r2 < members2.length)
		{
			var target2 = members2[r2];

			// Prevent guild master from being demoted (including self-demote)
			if (IsGuildMaster(guildinfo, target2))
			{
				pSock.SysMessage("The guild master cannot be demoted.");
			}
			else
			{
				guildinfo.Demote(target2);
				pSock.SysMessage("Demoted " + (target2.name || "member"));
				target2.Refresh();

			}
		}
		GuildMenu(pUser);
		return;
	}


	// Set rank 15400..15499
	else if (pButton >= 15400 && pButton < 15400 + 100)
	{
		if (!CanPromoteDemote(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing member ranks.");
			GuildMenu(pUser);
			return;
		}

		var row = pButton - 15400;
		var members3 = (guildinfo.members || guildinfo.member) || [];

		//  0 = Add Rank name
		//  1 = Add Rank prio
		//  2 = Set Rank (name)
		var rankNameToSet = gumpData.getEdit(2) || "";
		rankNameToSet = rankNameToSet.replace(/^\s+|\s+$/g, "");

		if (!rankNameToSet.length)
		{
			pSock.SysMessage("Enter a rank name first (Recruit, Member, Veteran, Officer, Guild Master, etc).");
			GuildMenu(pUser);
			return;
		}

		if (row < 0 || row >= members3.length || !members3[row])
		{
			pSock.SysMessage("Invalid member row for Set Rank.");
			GuildMenu(pUser);
			return;
		}

		var target = members3[row];

		// Do not allow changing the guild master's rank through this UI
		if (IsGuildMaster(guildinfo, target))
		{
			pSock.SysMessage("The guild master's rank cannot be changed.");
			GuildMenu(pUser);
			return;
		}

		var okSet = false;

		try
		{
			okSet = guildinfo.SetRank(target, rankNameToSet);
			target.guildTitle = rankNameToSet;
			target.Refresh();
		}
		catch (e)
		{
			pSock.SysMessage("SetRank JS error.");
			okSet = false;
		}

		if (!okSet)
		{
			pSock.SysMessage("Could not find rank '" + rankNameToSet + "'.");
		}
		else
		{
			pSock.SysMessage("Set rank '" + rankNameToSet + "' for " + (target.name || "member") + ".");
		}

		GuildMenu(pUser);
		return;
	}
	// Kick member - 15500..15599
	else if (pButton >= 15500 && pButton < 15500 + 100)
	{
		if (!CanPromoteDemote(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow removing members.");
			GuildMenu(pUser);
			return;
		}

		var row = pButton - 15500;
		var membersKick = (guildinfo.member || guildinfo.members) || [];

		if (row < 0 || row >= membersKick.length || !membersKick[row])
		{
			pSock.SysMessage("Invalid member row for remove.");
			GuildMenu(pUser);
			return;
		}

		var target = membersKick[row];

		// Don't let anyone kick the guild master
		if (IsGuildMaster(guildinfo, target))
		{
			pSock.SysMessage("The guild master cannot be removed this way.");
			GuildMenu(pUser);
			return;
		}

		// Optional: block self-kick from here, force them to use /resign or another UI
		if (target.serial === pUser.serial)
		{
			pSock.SysMessage("Cant Kick your self");
			GuildMenu(pUser);
			return;
		}
		// Adjust to your actual API if needed (RemoveMember(target) / RemoveMemberBySerial, etc.)
		if (guildinfo.RemoveMember)
		{
			guildinfo.RemoveMember(target);
		}
		else if (guildinfo.RemoveMemberBySerial)
		{
			guildinfo.RemoveMemberBySerial(target.serial);
		}

		pSock.SysMessage("Removed " + (target.name || "member") + " from the guild.");

		// Clear their local data if still online
		if (target.guild === guildinfo)
			target.guild = null;
		target.guildTitle = "";
		if (target.SetGuildFealty)
			target.SetGuildFealty(0);
		if (target.Refresh)
			target.Refresh();

		GuildMenu(pUser);
		return;
	}
	if (pButton === 16000) // Save
	{
		// Only Officer+ can edit MOTD/News (and GM can also change name/abbr)
		if (!CanEditNewsAndMOTD(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow editing the guild message of the day or news.");
			GuildMenu(pUser);
			return;
		}

		// text entry indexes 3..7 (after ranks entries)
		var abbrStr = gumpData.getEdit(3);
		var nameStr = gumpData.getEdit(4);
		var motdStr = gumpData.getEdit(5);
		var news0Str = gumpData.getEdit(6);
		var news1Str = gumpData.getEdit(7);
		var news2Str = gumpData.getEdit(8);
		//var webStr = gumpData.getEdit(3);
		//var charterStr = gumpData.getEdit(4);

		abbrStr = abbrStr.replace(/^\s+|\s+$/g, "");
		nameStr = nameStr.replace(/^\s+|\s+$/g, "");
		//webStr = webStr.replace(/^\s+|\s+$/g, "");
		//charterStr = charterStr.replace(/^\s+|\s+$/g, "");
		motdStr = motdStr.replace(/^\s+|\s+$/g, "");

		if (IsGuildMaster(guildinfo, pUser))
		{
			if (abbrStr.length)
				guildinfo.abbreviation = abbrStr;
			if (nameStr.length)
				guildinfo.name = nameStr;
		}
		else
		{
			pSock.SysMessage("Only Guild Master can change the name of the Guild and Abbreviation");
			GuildMenu(pUser);
			return;
		}

		SaveGuildMOTD(guildinfo, motdStr);

		var newsArr = [];
		if (news0Str.length) newsArr.push(news0Str);
		if (news1Str.length) newsArr.push(news1Str);
		if (news2Str.length) newsArr.push(news2Str);

		SaveGuildNews(guildinfo, newsArr);

		//guildinfo.webPage = webStr;
		//guildinfo.charter = charterStr;

		pSock.SysMessage("Guild settings updated.");
		GuildMenu(pUser);
		return;
	}
	if (pButton === 16001) // Cancel
	{
		GuildMenu(pUser);
		return;
	}
	if (pButton === 16100 || pButton === 16101 || pButton === 16102)
	{
		// Optional: only guild master can change type
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can change the guild type.");
			GuildMenu(pUser);
			return;
		}

		if (pButton === 16100)
		{
			guildinfo.type = 0;
			pSock.SysMessage("Guild type set to Standard.");
		}
		else if (pButton === 16101)
		{
			guildinfo.type = 1;
			pSock.SysMessage("Guild type set to Order.");
		}
		else if (pButton === 16102)
		{
			guildinfo.type = 2;
			pSock.SysMessage("Guild type set to Chaos.");
		}

		GuildMenu(pUser);
		return;
	}

	if (pButton === 17000)
	{
		// This assumes our search text entry is edit index 9
		var filterStr = gumpData.getEdit(9);
		filterStr = filterStr.replace(/^\s+|\s+$/g, "");
		SetWarSearchFilter(pUser, filterStr);

		GuildMenu(pUser);
		return;
	}

	if ((pButton >= 18000 && pButton < 18000 + 1000) ||
		(pButton >= 19000 && pButton < 19000 + 1000))
	{
		if (!CanEditGuildWars(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild war settings.");
			GuildMenu(pUser);
			return;
		}

		var searchFilter = GetWarSearchFilter(pUser);
		var matches = FilterGuildList(guildinfo, searchFilter);

		var idx, reqType;
		if (pButton >= 18000 && pButton < 18000 + 1000)
		{
			idx = pButton - 18000;
			reqType = "ALLY";
		}
		else
		{
			idx = pButton - 19000;
			reqType = "WAR";
		}

		if (!matches || idx < 0 || idx >= matches.length)
		{
			pSock.SysMessage("That guild is no longer available.");
			GuildMenu(pUser);
			return;
		}

		var targetGuild = matches[idx];
		if (!targetGuild || targetGuild.id === guildinfo.id)
		{
			pSock.SysMessage("Invalid guild selection.");
			GuildMenu(pUser);
			return;
		}

		// map type string -> relation int
		var relInt = 0; // neutral
		if (reqType === "WAR")
			relInt = 1;
		else if (reqType === "ALLY")
			relInt = 2;

		// *** This now calls the native C++ binding ***
		var ok = SendAndRecordRelationRequest(guildinfo, targetGuild, relInt);

		if (!ok)
		{
			pSock.SysMessage("Failed to send relation request. Ask an admin to check GuildSys->SendRelationRequest.");
			GuildMenu(pUser);
			return;
		}

		if (reqType === "ALLY")
			pSock.SysMessage("Alliance request sent to " + (targetGuild.name || "that guild") + ".");
		else
			pSock.SysMessage("War declaration request sent to " + (targetGuild.name || "that guild") + ".");

		GuildMenu(pUser);
		return;
	}

	if (pButton >= 20000 && pButton < 20000 + 1000)
	{
		if (!CanEditGuildWars(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild war settings.");
			GuildMenu(pUser);
			return;
		}

		var allGuilds = GetAllGuilds();
		var idx = pButton - 20000;

		if (!allGuilds || idx < 0 || idx >= allGuilds.length)
		{
			pSock.SysMessage("That guild is no longer available.");
			GuildMenu(pUser);
			return;
		}

		var targetGuild = allGuilds[idx];
		if (!targetGuild || targetGuild.id === guildinfo.id)
		{
			pSock.SysMessage("Invalid guild selection.");
			GuildMenu(pUser);
			return;
		}

		// Only send peace offer if we are actually at war
		var rel = (typeof CompareGuildByGuild !== "undefined")
			? CompareGuildByGuild(guildinfo.id, targetGuild.id) : 3;

		if (rel !== 1)
		{
			pSock.SysMessage("Your guild is not currently at war with that guild.");
			GuildMenu(pUser);
			return;
		}

		var ok = SendAndRecordRelationRequest(guildinfo, targetGuild, 0);
		if (!ok)
		{
			pSock.SysMessage("Failed to send peace offer. Ask an admin to check GuildSys->SendRelationRequest.");
			GuildMenu(pUser);
			return;
		}

		pSock.SysMessage("Peace offer sent to " + (targetGuild.name || "that guild") + ".");
		GuildMenu(pUser);
		return;

	}

	if (pButton >= 21000 && pButton < 21000 + 1000)
	{
		if (!CanEditGuildWars(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild war settings.");
			GuildMenu(pUser);
			return;
		}

		var idx = pButton - 21000;
		var incomingReqs = ReadGuildRelationRequests(guildinfo);

		if (!incomingReqs || idx < 0 || idx >= incomingReqs.length)
		{
			pSock.SysMessage("That relation request is no longer available.");
			GuildMenu(pUser);
			return;
		}

		var req = incomingReqs[idx];
		var otherGuild = FindGuildById(req.fromId);

		if (!otherGuild)
		{
			pSock.SysMessage("The requesting guild no longer exists.");
			RemoveGuildRelationRequestByIndex(guildinfo, idx);
			GuildMenu(pUser);
			return;
		}

		var relInt = req.relation | 0;

		// Apply the relation both ways via native binding
		var ok = SetGuildRelation(guildinfo.id, otherGuild.id, relInt);
		if (!ok)
		{
			pSock.SysMessage("Failed to apply relation. Ask an admin to check SetGuildRelation().");
			GuildMenu(pUser);
			return;
		}

		var relText = "Neutral";
		if (relInt === 1) relText = "War";
		else if (relInt === 2) relText = "Alliance";
		else if (relInt === 0) relText = "Peace";

		pSock.SysMessage("Relation with " + (otherGuild.name || "that guild") + " set to " + relText + ".");
		GuildMenu(pUser);
		return;
	}

	if (pButton >= 22000 && pButton < 22000 + 1000)
	{
		if (!CanEditGuildWars(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild war settings.");
			GuildMenu(pUser);
			return;
		}

		var idx = pButton - 22000;
		var incomingReqs = ReadGuildRelationRequests(guildinfo);

		if (!incomingReqs || idx < 0 || idx >= incomingReqs.length)
		{
			pSock.SysMessage("That relation request is no longer available.");
			GuildMenu(pUser);
			return;
		}

		var req = incomingReqs[idx];
		var otherGuild = FindGuildById(req.fromId);

		if (otherGuild)
			pSock.SysMessage("You decline the request from " + (otherGuild.name || "that guild") + ".");
		else
			pSock.SysMessage("You decline the request from a guild that no longer exists.");

		RemoveGuildRelationRequestByIndex(guildinfo, idx);
		GuildMenu(pUser);
		return;
	}

	if (pButton === 23100)
	{
		if (!pUser.SetGuildFealty)
		{
			pSock.SysMessage("Fealty option is not available. Ask an admin to check JS bindings.");
			GuildMenu(pUser);
			return;
		}

		pUser.SetGuildFealty(0);
		pSock.SysMessage("You withdraw your guild fealty vote.");
		GuildMenu(pUser);
		return;
	}

	if (pButton === 23200)
	{
		if (!guildinfo)
		{
			pSock.SysMessage("You are not in a guild.");
			return;
		}

		// callback id 1 -> onCallback1
		pSock.CustomTarget(1, "Select a guild member to pledge fealty to.");
		// Optionally re-open gump now, or wait until after targeting;
		// re-opening is fine, the target cursor will still be active.
		GuildMenu(pUser);
		return;
	}
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback0(socket, target)
{
	var pUser = socket.currentChar;
	if (!pUser)
		return;

	var guild = pUser.guild;
	if (!guild)
	{
		socket.SysMessage("You are not in a guild.");
		return;
	}

	if (!target || !target.isChar)
	{
		socket.SysMessage("That is not a valid character.");
		return;
	}

	// Check if target already has a guild
	if (target.guild)
	{
		if (target.guild === guild)
		{
			socket.SysMessage(target.name + " is already in your guild.");
		}
		else
		{
			socket.SysMessage(target.name + " is already a member of another guild.");
		}
		return;
	}

	// Already a recruit?
	var recruits = guild.recruits || [];
	for (var i = 0; i < recruits.length; i++)
	{
		if (recruits[i] && recruits[i].serial === target.serial)
		{
			socket.SysMessage(target.name + " is already a recruit.");
			return;
		}
	}

	guild.AddRecruit(target);
	socket.SysMessage("Invited " + (target.name || "player") + " as a recruit.");

	if (target.socket)
		target.socket.SysMessage("You have been invited to join " + (guild.name || "a guild") + " as a recruit.");
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1(socket, target)
{
	var pUser = socket.currentChar;
	if (!pUser)
		return;

	var guild = pUser.guild;
	if (!guild)
	{
		socket.SysMessage("You are not in a guild.");
		return;
	}

	if (!target || !target.isChar)
	{
		socket.SysMessage("That is not a valid character.");
		return;
	}

	// Target must be in the same guild
	if (!target.guild || target.guild !== guild)
	{
		socket.SysMessage("You may only pledge fealty to a member of your own guild.");
		return;
	}

	// Optional: do not allow fealty to self
	if (target.serial === pUser.serial)
	{
		socket.SysMessage("You cannot pledge fealty to yourself.");
		return;
	}

	if (!pUser.SetGuildFealty)
	{
		socket.SysMessage("Fealty option is not available. Ask an admin to check JS bindings.");
		return;
	}

	pUser.SetGuildFealty(target.serial);

	var tName = target.name || ("0x" + target.serial.toString(16).toUpperCase());
	socket.SysMessage("You pledge your guild fealty to " + tName + ".");

	// Optionally refresh the guild menu so current fealty text updates
	GuildMenu(pUser);
}


function HandleRecruitAction(pSock, pUser, guildinfo, row, doAccept)
{
	var recruits = guildinfo.recruits || [];
	var offset = GetRosterOffset(pUser);
	var idx = offset + row;

	if (idx < 0 || idx >= recruits.length)
	{
		pSock.SysMessage("Recruit entry no longer available.");
		GuildMenu(pUser);
		return;
	}

	var c = recruits[idx];

	if (doAccept)
	{
		// Accept recruit and set starting rank
		guildinfo.AcceptRecruit(c);

		// Decide starting rank:
		// - Prefer lowest-priority rank (usually "Recruit")
		// - Fallback to literal "Recruit" if that exists
		var startRankName = GetLowestRankName(guildinfo);

		if (!startRankName || !startRankName.length)
			startRankName = "Recruit";

		// Try to set rank; ignore failures silently
		try
		{
			if (guildinfo.SetRank && startRankName && startRankName.length)
			{
				guildinfo.SetRank(c, startRankName);
				c.guildTitle = startRankName;
			}
		}
		catch (e2) { /* ignore */ }

		pSock.SysMessage("Accepted " + (c.name || "recruit") + " as " + (c.guildTitle || startRankName || "member") + ".");
	}
	else
	{
		// Adjust to your API; commonly something like RemoveRecruit(serial) or DeclineRecruit(char)
		guildinfo.RemoveRecruit(c);
		pSock.SysMessage("Rejected " + (c.name || "recruit") + ".");
	}

	// Optional: keep offset sane if list shrank
	if (offset >= (recruits.length - 1) && offset > 0)
		SetRosterOffset(pUser, offset - 1);

	// Refresh gump so the row list updates immediately
	GuildMenu(pUser);
}

function GetRankPriority(guild, pChar)
{
	if (!guild || !pChar)
		return 0;

	// If bindings provide a direct method, prefer it
	if (guild.GetRankPrio)
	{
		return guild.GetRankPrio(pChar);
	}

	// Slow path: look up rank name, then scan rank list
	var rName = "";
	if (guild.GetRankName)
	{
		rName = guild.GetRankName(pChar) || "";
	}
	else
	{
		rName = GetRankName(guild, pChar) || "";
	}

	if (!rName.length || !guild.NumRanks || !guild.GetRankNameById || !guild.GetRankPrioById)
		return 0;

	var n = guild.NumRanks();
	for (var id = 0; id < n; ++id)
	{
		var name = "";
		var prio = -1;

		name = guild.GetRankNameById(id) || "";
		prio = guild.GetRankPrioById(id);

		if (!name || name === "(deleted)" || prio < 0)
			continue;

		if (name === rName)
			return prio;
	}

	return 0;
}

function IsGuildMaster(guild, pChar)
{
	if (!guild || !pChar)
		return false;

	var master = guild.master;
	if (!master)
		return false;

	if (typeof master === "number")
		return master === pChar.serial;

	return master === pChar || master.serial === pChar.serial;
}

function CanInvite(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true;

	var prio = GetRankPriority(guild, pChar);
	// Veteran (30) and above can invite
	return prio >= 30;
}

function CanPromoteDemote(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true;

	var prio = GetRankPriority(guild, pChar);
	// Officer (40) and above can promote/demote / set rank
	return prio >= 40;
}

function CanEditGuildWars(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true; // GM always allowed

	var prio = GetRankPriority(guild, pChar);
	// Officer (40) and above can change war/ally/peace
	return prio >= 40;
}

function GetLowestRankName(guild)
{
	if (!guild || !guild.NumRanks || !guild.GetRankNameById || !guild.GetRankPrioById)
		return "";

	var n = guild.NumRanks();
	var bestName = "";
	var bestPrio = 2147480000; // big number

	for (var id = 0; id < n; ++id)
	{
		var rName = guild.GetRankNameById(id);
		var rPrio = guild.GetRankPrioById(id);

		// same validity checks we used when drawing the ranks list
		if (!rName || rName === "(deleted)" || rPrio < 0 || rPrio >= 2147480000)
			continue;

		if (rPrio < bestPrio)
		{
			bestPrio = rPrio;
			bestName = rName;
		}
	}

	return bestName;
}

function manualTrim(str)
{
	return String(str).replace(/^\s+|\s+$/g, "");
}

function GetGuildConfigFileName(guild)
{
	return "Guild_" + guild.id + ".jsdata";
}

function ReadGuildConfig(guild)
{
	var cfg = {};

	if (!guild)
		return cfg;

	var mFile = new UOXCFile();
	var fileName = GetGuildConfigFileName(guild);

	mFile.Open(fileName, "r", "Guilds");
	if (!mFile || mFile.Length() <= 0)
	{
		if (mFile) mFile.Free();
		return cfg;
	}

	while (!mFile.EOF())
	{
		var line = manualTrim(mFile.ReadUntil("\n"));
		if (line == "")
			continue;

		var parts = line.split("=");
		if (parts.length != 2)
			continue;

		var key = manualTrim(parts[0]);
		var value = manualTrim(parts[1]);

		cfg[key] = value;
	}

	mFile.Close();
	mFile.Free();
	return cfg;
}

function SaveGuildConfig(guild, cfg)
{
	if (!guild)
		return false;

	var mFile = new UOXCFile();
	var fileName = GetGuildConfigFileName(guild);

	// IMPORTANT: check return value of Open()
	var opened = mFile.Open(fileName, "w", "Guilds");
	if (!opened)
	{
		if (typeof Console !== "undefined" && Console.Print)
		{
			Console.Print("[GuildConfig] Failed to open '" + fileName +
				"' in root 'Guilds' for writing.");
		}
		mFile.Free();
		return false;
	}

	for (var key in cfg)
	{
		if (!cfg.hasOwnProperty(key))
			continue;

		var value = cfg[key];
		if (value == null)
			value = "";

		mFile.Write(key + "=" + String(value) + "\n");
	}

	mFile.Close();
	mFile.Free();
	return true;
}

function SaveGuildMOTD(guild, motd)
{
	if (!guild) return false;

	var cfg = ReadGuildConfig(guild);
	var cleanMOTD = String(motd || "");

	cfg.MOTD = cleanMOTD;

	return SaveGuildConfig(guild, cfg);
}

function ReadGuildMOTD(guild)
{
	if (!guild) return "";

	var cfg = ReadGuildConfig(guild);
	return String(cfg.MOTD || "");
}

function SaveGuildNews(guild, newsArray)
{
	if (!guild) return false;

	var cfg = ReadGuildConfig(guild);

	// Clear old NEWS* keys
	for (var i = 0; i < 10; i++)
	{
		var k = "NEWS" + i;
		if (cfg.hasOwnProperty(k))
			delete cfg[k];
	}

	// Write new ones (max 10)
	if (!newsArray || !newsArray.length)
	{
		// no news - just save cleared state
		return SaveGuildConfig(guild, cfg);
	}

	var max = Math.min(newsArray.length, 10);
	for (var i = 0; i < max; i++)
	{
		var line = String(newsArray[i] || "");
		if (line.length)
			cfg["NEWS" + i] = line;
	}

	return SaveGuildConfig(guild, cfg);
}

function ReadGuildNews(guild)
{
	if (!guild) return [];

	var cfg = ReadGuildConfig(guild);
	var news = [];

	for (var i = 0; i < 10; i++)
	{
		var key = "NEWS" + i;
		if (cfg.hasOwnProperty(key) && cfg[key].length)
			news.push(String(cfg[key]));
	}

	return news;
}

function CanEditNewsAndMOTD(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true; // GM always allowed

	var prio = GetRankPriority(guild, pChar);
	// Officer (40) and above can edit MOTD / News
	return prio >= 40;
}

function FilterGuildList(myGuild, filterText)
{
	var all = GetAllGuilds(); // use native binding
	if (!all || !all.length || !myGuild)
		return [];

	var f = String(filterText || "").toLowerCase();
	var out = [];

	for (var i = 0; i < all.length; i++)
	{
		var g = all[i];
		if (!g || g.id === myGuild.id)
			continue;

		if (f.length)
		{
			var n = String(g.name || "").toLowerCase();
			var a = String(g.abbreviation || "").toLowerCase();
			if (n.indexOf(f) === -1 && a.indexOf(f) === -1)
				continue;
		}

		out.push(g);
	}
	return out;
}

function GetWarSearchFilter(pUser)
{
	if (!pUser || !pUser.GetTag)
		return "";
	return String(pUser.GetTag("guildWarFilter") || "");
}

function SetWarSearchFilter(pUser, txt)
{
	if (!pUser || !pUser.SetTag)
		return;
	pUser.SetTag("guildWarFilter", String(txt || ""));
}

function FindGuildById(guildId)
{
	var all = GetAllGuilds();
	if (!all || !all.length)
		return null;

	for (var i = 0; i < all.length; i++)
	{
		var g = all[i];
		if (g && g.id === (guildId | 0))
			return g;
	}
	return null;
}

function ReadGuildRelationRequests(guild)
{
	if (!guild || typeof GetGuildRelationRequests === "undefined")
		return [];

	var raw = GetGuildRelationRequests(guild.id);
	if (!raw || !raw.length)
		return [];

	var list = [];
	for (var i = 0; i < raw.length; i++)
	{
		var r = raw[i];
		if (!r)
			continue;

		list.push({
			fromId: r.fromId | 0,
			relation: r.relation | 0
		});
	}
	return list;
}

function RemoveGuildRelationRequestByIndex(guild, index)
{
	if (!guild || typeof RemoveGuildRelationRequest === "undefined")
		return false;

	RemoveGuildRelationRequest(guild.id, index | 0);
	return true;
}

function SendAndRecordRelationRequest(srcGuild, trgGuild, relationInt)
{
	if (!srcGuild || !trgGuild || typeof SendGuildRelationRequest === "undefined")
		return false;

	return SendGuildRelationRequest(srcGuild.id, trgGuild.id, relationInt | 0);
}

function GetGuildFealtyTarget(guild, voter)
{
	if (!guild || !voter || !voter.GetGuildFealty)
		return null;

	var fealtySerial = voter.GetGuildFealty() | 0;
	if (!fealtySerial)
		return null;

	var members = (guild.member || guild.members) || [];
	for (var i = 0; i < members.length; i++)
	{
		var m = members[i];
		if (m && m.serial === fealtySerial)
			return m;
	}
	return null;
}
