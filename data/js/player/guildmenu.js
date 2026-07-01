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

function GetClassicOffset(pUser, tagName)
{
	if (!pUser || !pUser.GetTag)
		return 0;

	var off = parseInt(pUser.GetTag(tagName), 10);
	if (isNaN(off) || off < 0)
		off = 0;
	return off;
}

function SetClassicOffset(pUser, tagName, off)
{
	if (pUser && pUser.SetTag)
		pUser.SetTag(tagName, Math.max(0, off | 0));
}

function AddClassicPaging(gump, x, y, offset, pageSize, total, prevButton, nextButton)
{
	if (offset > 0)
	{
		gump.AddButton(x, y, 4014, 4016, 1, 0, prevButton);
		gump.AddHTMLGump(x + 35, y - 2, 70, 20, false, false, ClassicGuildText("Previous"));
	}

	if (offset + pageSize < total)
	{
		gump.AddButton(x + 120, y, 4005, 4007, 1, 0, nextButton);
		gump.AddHTMLGump(x + 155, y - 2, 70, 20, false, false, ClassicGuildText("Next"));
	}
}

function ClassicGuildText(text)
{
	return "<basefont color=#111111>" + String(text || "") + "</basefont>";
}

var classicGuildRanks = [
	{ name: "Ronin", prio: 0 },
	{ name: "Member", prio: 20 },
	{ name: "Emissary", prio: 40 },
	{ name: "Warlord", prio: 40 },
	{ name: "Guild Master", prio: 50 }
];

var classicWarDefaultMaxKills = 100;
var classicWarDefaultDurationHours = 168;

function IsClassicGuildMode(guild)
{
	var cfg = ReadGuildConfig(guild);
	return cfg.CLASSIC_MODE === "1";
}

function SetClassicGuildMode(guild, enabled)
{
	var cfg = ReadGuildConfig(guild);
	cfg.CLASSIC_MODE = enabled ? "1" : "0";
	return SaveGuildConfig(guild, cfg);
}

function EnsureClassicGuildRanks(guild, pMaster)
{
	if (!guild || !guild.AddRank)
		return false;

	for (var i = 0; i < classicGuildRanks.length; i++)
		guild.AddRank(classicGuildRanks[i].name, classicGuildRanks[i].prio);

	if (pMaster && guild.SetRank)
	{
		guild.SetRank(pMaster, "Guild Master");
		pMaster.guildTitle = "Guild Master";
	}

	SetClassicGuildMode(guild, true);
	return true;
}

function GetClassicRankName(guild, pChar)
{
	if (!guild || !pChar)
		return "";

	var rankName = GetRankName(guild, pChar);
	if (rankName && rankName.length)
		return rankName;

	return pChar.guildTitle || "";
}

function HasClassicRank(guild, pChar, rankName)
{
	return GetClassicRankName(guild, pChar).toLowerCase() === String(rankName || "").toLowerCase();
}

function CanClassicInvite(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true;

	if (!IsClassicGuildMode(guild))
		return CanInvite(guild, pChar);

	return HasClassicRank(guild, pChar, "Emissary") || HasClassicRank(guild, pChar, "Guild Master");
}

function CanClassicManageRoster(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true;

	if (!IsClassicGuildMode(guild))
		return CanPromoteDemote(guild, pChar);

	return HasClassicRank(guild, pChar, "Emissary") || HasClassicRank(guild, pChar, "Guild Master");
}

function CanClassicControlWar(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true;

	if (!IsClassicGuildMode(guild))
		return CanEditGuildWars(guild, pChar);

	return HasClassicRank(guild, pChar, "Warlord") || HasClassicRank(guild, pChar, "Guild Master");
}

function AddClassicGuildButton(gump, x, y, buttonId, label)
{
	gump.AddButton(x, y, 4005, 4007, 1, 0, buttonId);
	gump.AddHTMLGump(x + 35, y - 2, 260, 24, false, false, ClassicGuildText(label));
}

function AddClassicGuildFrame(gump, title)
{
	gump.AddBackground(0, 0, 550, 400, 5054);
	gump.AddBackground(10, 10, 530, 380, 3000);
	gump.AddPicture(28, 26, 0x0ED4);
	gump.AddHTMLGump(72, 32, 360, 28, false, false, ClassicGuildText(title));
	AddClassicGuildButton(gump, 40, 350, 30011, "Return to guild menu");
	gump.AddButton(455, 350, 4017, 4019, 1, 0, 0);
	gump.AddHTMLGump(395, 352, 55, 22, false, false, ClassicGuildText("Exit"));
}

function ClassicGuildMenu(pUser)
{
	if (!pUser || !pUser.socket)
		return;

	var socket = pUser.socket;
	var guildinfo = pUser.guild;
	if (!guildinfo)
	{
		GuildCreation(pUser);
		return;
	}

	if (GetServerSetting( "ClassicOSIGuildMenu" ) && IsGuildMaster(guildinfo, pUser) && !IsClassicGuildMode(guildinfo))
		EnsureClassicGuildRanks(guildinfo, pUser);

	var classicMenu = new Gump;
	var leader = guildinfo.master;
	var leaderName = leader && leader.name ? leader.name : "None";
	var fealtyTarget = GetGuildFealtyTarget(guildinfo, pUser);
	var fealtyName = fealtyTarget && fealtyTarget.name ? fealtyTarget.name : "None";
	var recruits = guildinfo.recruits || [];
	var isMaster = IsGuildMaster(guildinfo, pUser);
	var classicMode = IsClassicGuildMode(guildinfo);
	var guildName = guildinfo.name || "Guild";
	var abbr = guildinfo.abbreviation ? " [" + guildinfo.abbreviation + "]" : "";
	var displayState = pUser.guildToggle ? "shown" : "hidden";

	classicMenu.AddBackground(0, 0, 550, 400, 5054);
	classicMenu.AddBackground(10, 10, 530, 380, 3000);
	classicMenu.AddPicture(28, 26, 0x0ED4);

	classicMenu.AddHTMLGump(72, 26, 330, 28, false, false, ClassicGuildText(guildName + abbr));
	classicMenu.AddHTMLGump(72, 52, 360, 22, false, false, ClassicGuildText("Guildmaster: " + leaderName));
	classicMenu.AddHTMLGump(72, 76, 360, 22, false, false, ClassicGuildText("Thy fealty is to: " + fealtyName));
	classicMenu.AddHTMLGump(300, 76, 190, 22, false, false, ClassicGuildText("Title display: " + displayState));
	if (!classicMode && isMaster)
		classicMenu.AddHTMLGump(72, 98, 430, 18, false, false, ClassicGuildText("Guildmaster functions can initialize OSI-style ranks for this guild."));

	AddClassicGuildButton(classicMenu, 40, 120, 30001, "Declare fealty");
	AddClassicGuildButton(classicMenu, 40, 150, 30002, "Toggle display abbreviation");
	AddClassicGuildButton(classicMenu, 40, 180, 30003, "View roster");
	AddClassicGuildButton(classicMenu, 40, 210, 30004, "Recruit someone into the guild");

	if (recruits.length)
		AddClassicGuildButton(classicMenu, 40, 240, 30005, "View candidates");
	else
	{
		classicMenu.AddGump(40, 240, 4020);
		classicMenu.AddHTMLGump(75, 238, 260, 24, false, false, ClassicGuildText("View candidates"));
	}

	AddClassicGuildButton(classicMenu, 300, 120, 30006, "View charter");
	AddClassicGuildButton(classicMenu, 300, 150, 30007, "Resign from guild");
	AddClassicGuildButton(classicMenu, 300, 180, 30008, "Wars and diplomacy");

	if (isMaster)
		AddClassicGuildButton(classicMenu, 300, 210, 30009, "Guildmaster functions");
	else
	{
		classicMenu.AddGump(300, 210, 4020);
		classicMenu.AddHTMLGump(335, 208, 180, 24, false, false, ClassicGuildText("Guildmaster functions"));
	}

	AddClassicGuildButton(classicMenu, 300, 300, 30010, "Custom guild menu");
	classicMenu.AddButton(455, 350, 4017, 4019, 1, 0, 0);
	classicMenu.AddHTMLGump(395, 352, 55, 22, false, false, ClassicGuildText("Exit"));

	classicMenu.Send(socket);
	classicMenu.Free();
}

function ClassicGuildRoster(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var members = (guildinfo.member || guildinfo.members) || [];
	var classicMode = IsClassicGuildMode(guildinfo);
	var canManage = CanClassicManageRoster(guildinfo, pUser);
	var roster = new Gump;
	var offset = GetClassicOffset(pUser, "classicRosterOffset");
	var pageSize = 12;

	AddClassicGuildFrame(roster, "Guild Roster");
	roster.AddHTMLGump(55, 88, 150, 22, false, false, ClassicGuildText("Name"));
	roster.AddHTMLGump(220, 88, 120, 22, false, false, ClassicGuildText("Rank"));
	roster.AddHTMLGump(360, 88, 80, 22, false, false, ClassicGuildText("Status"));
	if (canManage)
		roster.AddHTMLGump(435, 88, 80, 22, false, false, ClassicGuildText("Manage"));

	if (!members.length)
	{
		roster.AddHTMLGump(55, 125, 360, 22, false, false, ClassicGuildText("No members found."));
	}
	else
	{
		if (offset >= members.length)
			offset = 0;

		var end = Math.min(offset + pageSize, members.length);
		for (var i = offset; i < end; i++)
		{
			var m = members[i];
			if (!m)
				continue;

			var row = i - offset;
			var y = 120 + row * 18;
			var name = m.name || ("0x" + m.serial.toString(16).toUpperCase());
			var rank = classicMode ? GetClassicRankName(guildinfo, m) : (GetRankName(guildinfo, m) || m.guildTitle || "Member");
			var status = GetOnlineStatus(m);
			roster.AddHTMLGump(55, y, 150, 18, false, false, ClassicGuildText(name));
			roster.AddHTMLGump(220, y, 120, 18, false, false, ClassicGuildText(rank));
			roster.AddHTMLGump(360, y, 80, 18, false, false, ClassicGuildText(status));
			if (canManage && !IsGuildMaster(guildinfo, m))
			{
				roster.AddButton(435, y, 4005, 4007, 1, 0, 30800 + i);
				roster.AddHTMLGump(470, y - 2, 60, 18, false, false, ClassicGuildText("Edit"));
			}
		}

		roster.AddHTMLGump(55, 332, 220, 18, false, false, ClassicGuildText("Showing " + (offset + 1) + "-" + end + " of " + members.length));
		AddClassicPaging(roster, 290, 332, offset, pageSize, members.length, 31700, 31701);
	}

	roster.Send(pUser.socket);
	roster.Free();
}

function GetClassicRosterMember(guild, memberIndex)
{
	if (!guild)
		return null;

	var members = (guild.member || guild.members) || [];
	if (memberIndex < 0 || memberIndex >= members.length)
		return null;

	return members[memberIndex];
}

function ClassicGuildMemberMenu(pUser, memberIndex)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var member = GetClassicRosterMember(guildinfo, memberIndex);
	if (!member)
	{
		pUser.socket.SysMessage("That member is no longer available.");
		ClassicGuildRoster(pUser);
		return;
	}

	var memberName = member.name || ("0x" + member.serial.toString(16).toUpperCase());
	var rankName = GetClassicRankName(guildinfo, member) || "Member";
	var memberMenu = new Gump;

	AddClassicGuildFrame(memberMenu, "Manage Guild Member");
	memberMenu.AddHTMLGump(55, 88, 420, 22, false, false, ClassicGuildText(memberName + " - " + rankName));
	memberMenu.AddHTMLGump(55, 118, 420, 22, false, false, ClassicGuildText("Set classic rank"));

	memberMenu.AddButton(55, 150, 4005, 4007, 1, 0, 31100 + memberIndex * 10);
	memberMenu.AddHTMLGump(90, 148, 90, 20, false, false, ClassicGuildText("Ronin"));
	memberMenu.AddButton(180, 150, 4005, 4007, 1, 0, 31101 + memberIndex * 10);
	memberMenu.AddHTMLGump(215, 148, 90, 20, false, false, ClassicGuildText("Member"));
	memberMenu.AddButton(305, 150, 4005, 4007, 1, 0, 31102 + memberIndex * 10);
	memberMenu.AddHTMLGump(340, 148, 90, 20, false, false, ClassicGuildText("Emissary"));
	memberMenu.AddButton(55, 180, 4005, 4007, 1, 0, 31103 + memberIndex * 10);
	memberMenu.AddHTMLGump(90, 178, 90, 20, false, false, ClassicGuildText("Warlord"));

	memberMenu.AddHTMLGump(55, 225, 420, 20, false, false, ClassicGuildText("Set guild title"));
	memberMenu.AddGump(55, 250, 1803);
	memberMenu.AddTextEntryLimited(60, 252, 220, 20, 0, 0, 61020, member.guildTitle || "", 32);
	memberMenu.AddButton(300, 250, 4005, 4007, 1, 0, 31600 + memberIndex);
	memberMenu.AddHTMLGump(335, 248, 80, 20, false, false, ClassicGuildText("Set title"));

	memberMenu.AddButton(55, 300, 4017, 4019, 1, 0, 30900 + memberIndex);
	memberMenu.AddHTMLGump(90, 298, 180, 20, false, false, ClassicGuildText("Remove from guild"));

	memberMenu.Send(pUser.socket);
	memberMenu.Free();
}

function ClassicGuildRemoveConfirm(pUser, memberIndex)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var member = GetClassicRosterMember(guildinfo, memberIndex);
	if (!member)
	{
		pUser.socket.SysMessage("That member is no longer available.");
		ClassicGuildRoster(pUser);
		return;
	}

	var memberName = member.name || ("0x" + member.serial.toString(16).toUpperCase());
	var confirm = new Gump;

	AddClassicGuildFrame(confirm, "Remove Guild Member");
	confirm.AddHTMLGump(55, 110, 420, 60, true, true, ClassicGuildText("Remove " + memberName + " from the guild?"));
	AddClassicGuildButton(confirm, 90, 210, 31000 + memberIndex, "Yes, remove");
	AddClassicGuildButton(confirm, 300, 210, 30003, "No, return");

	confirm.Send(pUser.socket);
	confirm.Free();
}

function ClassicGuildCandidates(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var recruits = guildinfo.recruits || [];
	var candidates = new Gump;
	var offset = GetClassicOffset(pUser, "classicCandidateOffset");
	var pageSize = 10;

	AddClassicGuildFrame(candidates, "Guild Candidates");
	candidates.AddHTMLGump(55, 88, 170, 22, false, false, ClassicGuildText("Candidate"));
	candidates.AddHTMLGump(300, 88, 150, 22, false, false, ClassicGuildText("Accept / Decline"));

	if (!recruits.length)
	{
		candidates.AddHTMLGump(55, 125, 360, 22, false, false, ClassicGuildText("There are no candidates for membership."));
	}
	else
	{
		if (offset >= recruits.length)
			offset = 0;

		var end = Math.min(offset + pageSize, recruits.length);
		for (var i = offset; i < end; i++)
		{
			var c = recruits[i];
			if (!c)
				continue;

			var row = i - offset;
			var y = 120 + row * 22;
			var name = c.name || ("0x" + c.serial.toString(16).toUpperCase());
			candidates.AddHTMLGump(55, y, 210, 20, false, false, ClassicGuildText(name));
			candidates.AddButton(300, y, 4005, 4007, 1, 0, 30100 + i);
			candidates.AddHTMLGump(335, y - 2, 55, 20, false, false, ClassicGuildText("Accept"));
			candidates.AddButton(400, y, 4017, 4019, 1, 0, 30200 + i);
			candidates.AddHTMLGump(435, y - 2, 55, 20, false, false, ClassicGuildText("Decline"));
		}

		candidates.AddHTMLGump(55, 332, 220, 18, false, false, ClassicGuildText("Showing " + (offset + 1) + "-" + end + " of " + recruits.length));
		AddClassicPaging(candidates, 290, 332, offset, pageSize, recruits.length, 31710, 31711);
	}

	candidates.Send(pUser.socket);
	candidates.Free();
}

function ClassicGuildCharter(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var charter = guildinfo.charter && guildinfo.charter.length ? guildinfo.charter : "This guild has not written a charter.";
	var webpage = guildinfo.webPage && guildinfo.webPage.length ? guildinfo.webPage : "";
	var charterGump = new Gump;

	AddClassicGuildFrame(charterGump, "Guild Charter");
	charterGump.AddHTMLGump(55, 90, 430, 210, true, true, ClassicGuildText(charter));
	if (webpage.length)
		charterGump.AddHTMLGump(55, 310, 430, 22, false, false, ClassicGuildText("Web page: " + webpage));

	charterGump.Send(pUser.socket);
	charterGump.Free();
}

function GetGuildRelationText(rel)
{
	if (rel === 1)
		return "War";
	if (rel === 2)
		return "Ally";
	if (rel === 4)
		return "Same";
	return "Peace";
}

function ClassicGuildDiplomacy(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var allGuilds = GetAllGuilds() || [];
	var diplomacy = new Gump;
	var rowY = 88;
	var shown = 0;
	var offset = GetClassicOffset(pUser, "classicDiplomacyOffset");
	var pageSize = 7;
	var otherGuildCount = 0;

	AddClassicGuildFrame(diplomacy, "Guild Diplomacy");
	diplomacy.AddHTMLGump(55, 68, 210, 20, false, false, ClassicGuildText("Guild"));
	diplomacy.AddHTMLGump(235, 68, 105, 20, false, false, ClassicGuildText("Status"));
	diplomacy.AddHTMLGump(350, 68, 150, 20, false, false, ClassicGuildText("Actions"));

	if (!allGuilds.length)
	{
		diplomacy.AddHTMLGump(55, rowY, 360, 20, false, false, ClassicGuildText("No guilds found."));
	}
	else
	{
		for (var countIndex = 0; countIndex < allGuilds.length; countIndex++)
		{
			if (allGuilds[countIndex] && allGuilds[countIndex].id !== guildinfo.id)
				otherGuildCount++;
		}

		if (offset >= otherGuildCount)
			offset = 0;

		var skipped = 0;
		for (var i = 0; i < allGuilds.length && shown < pageSize; i++)
		{
			var g = allGuilds[i];
			if (!g || g.id === guildinfo.id)
				continue;

			if (skipped < offset)
			{
				skipped++;
				continue;
			}

			var y = rowY + shown * 24;
			var rel = (typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(guildinfo.id, g.id) : 3;
			var name = g.name || ("Guild #" + g.id);
			if (g.abbreviation && g.abbreviation.length)
				name += " [" + g.abbreviation + "]";

			diplomacy.AddHTMLGump(55, y, 185, 20, false, false, ClassicGuildText(name));
			if (rel === 1)
				CheckClassicWarExpired(guildinfo, g);

			var statusText = (rel === 1) ? FormatClassicWarStatus(guildinfo, g) : GetGuildRelationText(rel);
			diplomacy.AddHTMLGump(235, y, 105, 20, false, false, ClassicGuildText(statusText));

			if (rel === 1)
			{
				diplomacy.AddButton(350, y, 4005, 4007, 1, 0, 30300 + i);
				diplomacy.AddHTMLGump(385, y - 2, 110, 20, false, false, ClassicGuildText("Offer peace"));
			}
			else
			{
				diplomacy.AddButton(350, y, 4005, 4007, 1, 0, 30400 + i);
				diplomacy.AddHTMLGump(385, y - 2, 55, 20, false, false, ClassicGuildText("Ally"));
				diplomacy.AddButton(440, y, 4017, 4019, 1, 0, 30500 + i);
				diplomacy.AddHTMLGump(475, y - 2, 55, 20, false, false, ClassicGuildText("War"));
			}

			shown++;
		}

		if (shown === 0)
			diplomacy.AddHTMLGump(55, rowY, 360, 20, false, false, ClassicGuildText("No other guilds found."));
		else
		{
			diplomacy.AddHTMLGump(55, 232, 220, 18, false, false, ClassicGuildText("Showing " + (offset + 1) + "-" + (offset + shown) + " of " + otherGuildCount));
			AddClassicPaging(diplomacy, 290, 232, offset, pageSize, otherGuildCount, 31720, 31721);
		}
	}

	var requestY = 270;
	var incomingReqs = ReadGuildRelationRequests(guildinfo);
	diplomacy.AddHTMLGump(55, requestY - 24, 360, 20, false, false, ClassicGuildText("Pending relation requests"));

	if (!incomingReqs || !incomingReqs.length)
	{
		diplomacy.AddHTMLGump(55, requestY, 360, 20, false, false, ClassicGuildText("No pending requests."));
	}
	else
	{
		var maxReqs = Math.min(4, incomingReqs.length);
		for (var ri = 0; ri < maxReqs; ri++)
		{
			var req = incomingReqs[ri];
			var fromGuild = req ? FindGuildById(req.fromId) : null;
			var ry = requestY + ri * 22;
			var fromName = fromGuild ? (fromGuild.name || ("Guild #" + fromGuild.id)) : ("Guild #" + (req ? req.fromId : 0));
			var relText = req ? GetGuildRelationText(req.relation | 0) : "Unknown";

			diplomacy.AddHTMLGump(55, ry, 250, 20, false, false, ClassicGuildText(fromName + " asks: " + relText));
			diplomacy.AddButton(325, ry, 4005, 4007, 1, 0, 30600 + ri);
			diplomacy.AddHTMLGump(360, ry - 2, 55, 20, false, false, ClassicGuildText("Accept"));
			diplomacy.AddButton(415, ry, 4017, 4019, 1, 0, 30700 + ri);
			diplomacy.AddHTMLGump(450, ry - 2, 55, 20, false, false, ClassicGuildText("Decline"));
		}
	}

	diplomacy.Send(pUser.socket);
	diplomacy.Free();
}

function ClassicGuildMasterMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var masterMenu = new Gump;

	AddClassicGuildFrame(masterMenu, "Guildmaster Functions");
	AddClassicGuildButton(masterMenu, 55, 105, 30013, "Pack guildstone");
	AddClassicGuildButton(masterMenu, 55, 140, 30014, "Initialize classic ranks");
	AddClassicGuildButton(masterMenu, 55, 175, 30015, "Edit guild information");
	AddClassicGuildButton(masterMenu, 55, 210, 30016, "Set guild type");
	AddClassicGuildButton(masterMenu, 55, 245, 30010, "Open custom guild settings");
	if (IsClassicGuildMode(guildinfo))
		masterMenu.AddHTMLGump(55, 295, 420, 40, true, true, ClassicGuildText("Classic rank mode is enabled. Emissaries manage members; Warlords control diplomacy."));
	else
		masterMenu.AddHTMLGump(55, 295, 420, 40, true, true, ClassicGuildText("Classic rank mode has not been initialized for this guild yet."));

	masterMenu.Send(pUser.socket);
	masterMenu.Free();
}

function ClassicGuildInfoEdit(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var edit = new Gump;

	AddClassicGuildFrame(edit, "Edit Guild Information");
	edit.AddHTMLGump(55, 82, 420, 20, false, false, ClassicGuildText("Guild name"));
	edit.AddGump(55, 105, 1803);
	edit.AddTextEntryLimited(60, 107, 240, 20, 0, 0, 61030, guildinfo.name || "", 33);

	edit.AddHTMLGump(330, 82, 140, 20, false, false, ClassicGuildText("Abbreviation"));
	edit.AddGump(330, 105, 1803);
	edit.AddTextEntryLimited(335, 107, 80, 20, 0, 1, 61031, guildinfo.abbreviation || "", 3);

	edit.AddHTMLGump(55, 140, 420, 20, false, false, ClassicGuildText("Web page"));
	edit.AddGump(55, 163, 1803);
	edit.AddTextEntryLimited(60, 165, 360, 20, 0, 2, 61032, guildinfo.webPage || "", 60);

	edit.AddHTMLGump(55, 198, 420, 20, false, false, ClassicGuildText("Charter"));
	edit.AddTextEntryLimited(55, 225, 420, 70, 0, 3, 61033, guildinfo.charter || "", 168);

	AddClassicGuildButton(edit, 300, 315, 30017, "Save information");

	edit.Send(pUser.socket);
	edit.Free();
}

function ClassicGuildTypeMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var typeMenu = new Gump;
	var currentType = guildinfo.type | 0;

	AddClassicGuildFrame(typeMenu, "Set Guild Type");
	typeMenu.AddHTMLGump(55, 95, 420, 40, true, true, ClassicGuildText("Choose the public guild type shown by the guild system."));
	AddClassicGuildButton(typeMenu, 90, 160, 30018, currentType === 0 ? "Standard [current]" : "Standard");
	AddClassicGuildButton(typeMenu, 90, 200, 30019, currentType === 1 ? "Order [current]" : "Order");
	AddClassicGuildButton(typeMenu, 90, 240, 30020, currentType === 2 ? "Chaos [current]" : "Chaos");

	typeMenu.Send(pUser.socket);
	typeMenu.Free();
}

function ClassicGuildResignConfirm(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var resign = new Gump;

	AddClassicGuildFrame(resign, "Resign From Guild");
	resign.AddHTMLGump(55, 105, 420, 80, true, true, ClassicGuildText("Art thou certain thou wishest to resign from thy guild?"));
	AddClassicGuildButton(resign, 90, 210, 30070, "Yes, resign");
	AddClassicGuildButton(resign, 300, 210, 30071, "No, remain");

	resign.Send(pUser.socket);
	resign.Free();
}

function HandleClassicGuildButton(pSock, pUser, guildinfo, pButton, gumpData)
{
	if (pButton === 30011 || pButton === 30071)
	{
		ClassicGuildMenu(pUser);
		return true;
	}

	if (pButton === 31700 || pButton === 31701)
	{
		var membersForPage = (guildinfo.member || guildinfo.members) || [];
		var rosterOffset = GetClassicOffset(pUser, "classicRosterOffset");
		rosterOffset += (pButton === 31700) ? -12 : 12;
		if (rosterOffset >= membersForPage.length)
			rosterOffset = Math.max(0, membersForPage.length - 12);
		SetClassicOffset(pUser, "classicRosterOffset", rosterOffset);
		ClassicGuildRoster(pUser);
		return true;
	}

	if (pButton === 31710 || pButton === 31711)
	{
		var recruitsForPage = guildinfo.recruits || [];
		var candidateOffset = GetClassicOffset(pUser, "classicCandidateOffset");
		candidateOffset += (pButton === 31710) ? -10 : 10;
		if (candidateOffset >= recruitsForPage.length)
			candidateOffset = Math.max(0, recruitsForPage.length - 10);
		SetClassicOffset(pUser, "classicCandidateOffset", candidateOffset);
		ClassicGuildCandidates(pUser);
		return true;
	}

	if (pButton === 31720 || pButton === 31721)
	{
		var guildsForPage = GetAllGuilds() || [];
		var otherCount = 0;
		for (var pg = 0; pg < guildsForPage.length; pg++)
		{
			if (guildsForPage[pg] && guildsForPage[pg].id !== guildinfo.id)
				otherCount++;
		}

		var diplomacyOffset = GetClassicOffset(pUser, "classicDiplomacyOffset");
		diplomacyOffset += (pButton === 31720) ? -7 : 7;
		if (diplomacyOffset >= otherCount)
			diplomacyOffset = Math.max(0, otherCount - 7);
		SetClassicOffset(pUser, "classicDiplomacyOffset", diplomacyOffset);
		ClassicGuildDiplomacy(pUser);
		return true;
	}

	if (pButton === 30001)
	{
		pSock.CustomTarget(1, "Select the guild member to whom thou shalt swear fealty.");
		ClassicGuildMenu(pUser);
		return true;
	}

	if (pButton === 30002)
	{
		pUser.guildToggle = !pUser.guildToggle;
		if (pUser.Refresh)
			pUser.Refresh();
		pSock.SysMessage(pUser.guildToggle ? "Guild title and abbreviation display enabled." : "Guild title and abbreviation display hidden.");
		ClassicGuildMenu(pUser);
		return true;
	}

	if (pButton === 30003)
	{
		ClassicGuildRoster(pUser);
		return true;
	}

	if (pButton === 30004)
	{
		if (!CanClassicInvite(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow inviting new recruits.");
			ClassicGuildMenu(pUser);
			return true;
		}

		pSock.CustomTarget(0, "Select a player to invite.");
		ClassicGuildMenu(pUser);
		return true;
	}

	if (pButton === 30005)
	{
		ClassicGuildCandidates(pUser);
		return true;
	}

	if (pButton === 30006)
	{
		ClassicGuildCharter(pUser);
		return true;
	}

	if (pButton === 30007)
	{
		ClassicGuildResignConfirm(pUser);
		return true;
	}

	if (pButton === 30008)
	{
		ClassicGuildDiplomacy(pUser);
		return true;
	}

	if (pButton === 30010)
	{
		GuildMenu(pUser);
		return true;
	}

	if (pButton === 30009)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master may use guildmaster functions.");
			ClassicGuildMenu(pUser);
			return true;
		}

		ClassicGuildMasterMenu(pUser);
		return true;
	}

	if (pButton === 30013)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can move the guildstone.");
			ClassicGuildMenu(pUser);
			return true;
		}

		TriggerEvent(5022, "PackGuildstone", pUser, guildinfo);
		ClassicGuildMenu(pUser);
		return true;
	}

	if (pButton === 30014)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can initialize classic ranks.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		if (EnsureClassicGuildRanks(guildinfo, pUser))
			pSock.SysMessage("Classic guild ranks are now initialized.");
		else
			pSock.SysMessage("Classic guild ranks could not be initialized.");

		ClassicGuildMasterMenu(pUser);
		return true;
	}

	if (pButton === 30015)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can edit guild information.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		ClassicGuildInfoEdit(pUser);
		return true;
	}

	if (pButton === 30016)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can change the guild type.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		ClassicGuildTypeMenu(pUser);
		return true;
	}

	if (pButton === 30017)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can edit guild information.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		var newName = manualTrim(gumpData.getEdit(0) || "");
		var newAbbr = manualTrim(gumpData.getEdit(1) || "");
		var newWeb = manualTrim(gumpData.getEdit(2) || "");
		var newCharter = manualTrim(gumpData.getEdit(3) || "");

		if (newName.length)
			guildinfo.name = newName;
		if (newAbbr.length)
			guildinfo.abbreviation = newAbbr;

		guildinfo.webPage = newWeb;
		guildinfo.charter = newCharter;

		pSock.SysMessage("Guild information updated.");
		ClassicGuildMasterMenu(pUser);
		return true;
	}

	if (pButton === 30018 || pButton === 30019 || pButton === 30020)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can change the guild type.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		if (pButton === 30018)
		{
			guildinfo.type = 0;
			pSock.SysMessage("Guild type set to Standard.");
		}
		else if (pButton === 30019)
		{
			guildinfo.type = 1;
			pSock.SysMessage("Guild type set to Order.");
		}
		else
		{
			guildinfo.type = 2;
			pSock.SysMessage("Guild type set to Chaos.");
		}

		ClassicGuildTypeMenu(pUser);
		return true;
	}

	if (pButton === 30070)
	{
		if (IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("The guild master cannot resign from this classic confirmation yet. Transfer leadership or use guild management first.");
			ClassicGuildMenu(pUser);
			return true;
		}

		if (guildinfo.RemoveMember)
			guildinfo.RemoveMember(pUser);
		else if (guildinfo.RemoveMemberBySerial)
			guildinfo.RemoveMemberBySerial(pUser.serial);

		pUser.guild = null;
		pUser.guildTitle = "";
		if (pUser.SetGuildFealty)
			pUser.SetGuildFealty(0);
		if (pUser.Refresh)
			pUser.Refresh();

		pSock.SysMessage("You have resigned from your guild.");
		return true;
	}

	if (pButton >= 30100 && pButton < 30110)
	{
		if (!CanClassicManageRoster(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow managing recruits.");
			ClassicGuildCandidates(pUser);
			return true;
		}

		HandleRecruitAction(pSock, pUser, guildinfo, pButton - 30100, true, true);
		return true;
	}

	if (pButton >= 30200 && pButton < 30210)
	{
		if (!CanClassicManageRoster(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow managing recruits.");
			ClassicGuildCandidates(pUser);
			return true;
		}

		HandleRecruitAction(pSock, pUser, guildinfo, pButton - 30200, false, true);
		return true;
	}

	if (pButton >= 30800 && pButton < 30900)
	{
		if (!CanClassicManageRoster(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow managing guild members.");
			ClassicGuildRoster(pUser);
			return true;
		}

		ClassicGuildMemberMenu(pUser, pButton - 30800);
		return true;
	}

	if (pButton >= 30900 && pButton < 31000)
	{
		if (!CanClassicManageRoster(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow removing guild members.");
			ClassicGuildRoster(pUser);
			return true;
		}

		ClassicGuildRemoveConfirm(pUser, pButton - 30900);
		return true;
	}

	if (pButton >= 31000 && pButton < 31100)
	{
		if (!CanClassicManageRoster(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow removing guild members.");
			ClassicGuildRoster(pUser);
			return true;
		}

		var removeIndex = pButton - 31000;
		var removeTarget = GetClassicRosterMember(guildinfo, removeIndex);
		if (!removeTarget)
		{
			pSock.SysMessage("That member is no longer available.");
			ClassicGuildRoster(pUser);
			return true;
		}

		if (IsGuildMaster(guildinfo, removeTarget))
		{
			pSock.SysMessage("The guild master cannot be removed this way.");
			ClassicGuildRoster(pUser);
			return true;
		}

		if (removeTarget.serial === pUser.serial)
		{
			pSock.SysMessage("Use resign if you wish to leave the guild.");
			ClassicGuildRoster(pUser);
			return true;
		}

		if (guildinfo.RemoveMember)
			guildinfo.RemoveMember(removeTarget);
		else if (guildinfo.RemoveMemberBySerial)
			guildinfo.RemoveMemberBySerial(removeTarget.serial);

		if (removeTarget.guild === guildinfo)
			removeTarget.guild = null;
		removeTarget.guildTitle = "";
		if (removeTarget.SetGuildFealty)
			removeTarget.SetGuildFealty(0);
		if (removeTarget.Refresh)
			removeTarget.Refresh();

		pSock.SysMessage("Removed " + (removeTarget.name || "member") + " from the guild.");
		ClassicGuildRoster(pUser);
		return true;
	}

	if (pButton >= 31100 && pButton < 31600)
	{
		if (!CanClassicManageRoster(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing member ranks.");
			ClassicGuildRoster(pUser);
			return true;
		}

		var rankButton = pButton - 31100;
		var memberIndex = Math.floor(rankButton / 10);
		var rankIndex = rankButton % 10;
		var rankNames = ["Ronin", "Member", "Emissary", "Warlord"];
		var rankTarget = GetClassicRosterMember(guildinfo, memberIndex);
		var newRank = rankNames[rankIndex];

		if (!rankTarget || !newRank)
		{
			pSock.SysMessage("That member or rank is no longer available.");
			ClassicGuildRoster(pUser);
			return true;
		}

		if (IsGuildMaster(guildinfo, rankTarget))
		{
			pSock.SysMessage("The guild master's rank cannot be changed here.");
			ClassicGuildRoster(pUser);
			return true;
		}

		if (guildinfo.SetRank)
		{
			if (!guildinfo.SetRank(rankTarget, newRank))
			{
				pSock.SysMessage("Could not set that rank. Initialize classic ranks first.");
				ClassicGuildMemberMenu(pUser, memberIndex);
				return true;
			}
		}

		rankTarget.guildTitle = newRank;
		if (rankTarget.Refresh)
			rankTarget.Refresh();

		pSock.SysMessage("Set " + (rankTarget.name || "member") + " to " + newRank + ".");
		ClassicGuildMemberMenu(pUser, memberIndex);
		return true;
	}

	if (pButton >= 31600 && pButton < 31700)
	{
		if (!CanClassicManageRoster(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild titles.");
			ClassicGuildRoster(pUser);
			return true;
		}

		var titleIndex = pButton - 31600;
		var titleTarget = GetClassicRosterMember(guildinfo, titleIndex);
		if (!titleTarget)
		{
			pSock.SysMessage("That member is no longer available.");
			ClassicGuildRoster(pUser);
			return true;
		}

		var titleText = manualTrim(gumpData.getEdit(0) || "");
		if (titleText.length > 32)
			titleText = titleText.substring(0, 32);

		titleTarget.guildTitle = titleText;
		if (titleTarget.Refresh)
			titleTarget.Refresh();

		pSock.SysMessage("Updated guild title for " + (titleTarget.name || "member") + ".");
		ClassicGuildMemberMenu(pUser, titleIndex);
		return true;
	}

	if ((pButton >= 30300 && pButton < 30600))
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildDiplomacy(pUser);
			return true;
		}

		var allGuilds = GetAllGuilds() || [];
		var guildIndex;
		var relInt;
		var actionText;

		if (pButton >= 30300 && pButton < 30400)
		{
			guildIndex = pButton - 30300;
			relInt = 0;
			actionText = "Peace offer";
		}
		else if (pButton >= 30400 && pButton < 30500)
		{
			guildIndex = pButton - 30400;
			relInt = 2;
			actionText = "Alliance request";
		}
		else
		{
			guildIndex = pButton - 30500;
			relInt = 1;
			actionText = "War declaration";
		}

		if (!allGuilds || guildIndex < 0 || guildIndex >= allGuilds.length)
		{
			pSock.SysMessage("That guild is no longer available.");
			ClassicGuildDiplomacy(pUser);
			return true;
		}

		var targetGuild = allGuilds[guildIndex];
		if (!targetGuild || targetGuild.id === guildinfo.id)
		{
			pSock.SysMessage("Invalid guild selection.");
			ClassicGuildDiplomacy(pUser);
			return true;
		}

		var ok = SendAndRecordRelationRequest(guildinfo, targetGuild, relInt);
		if (ok)
			pSock.SysMessage(actionText + " sent to " + (targetGuild.name || "that guild") + ".");
		else
			pSock.SysMessage("Failed to send relation request.");

		ClassicGuildDiplomacy(pUser);
		return true;
	}

	if (pButton >= 30600 && pButton < 30800)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildDiplomacy(pUser);
			return true;
		}

		var acceptReq = pButton >= 30600 && pButton < 30700;
		var reqIndex = acceptReq ? (pButton - 30600) : (pButton - 30700);
		var incomingReqs = ReadGuildRelationRequests(guildinfo);
		if (!incomingReqs || reqIndex < 0 || reqIndex >= incomingReqs.length)
		{
			pSock.SysMessage("That relation request is no longer available.");
			ClassicGuildDiplomacy(pUser);
			return true;
		}

		var req = incomingReqs[reqIndex];
		var otherGuild = FindGuildById(req.fromId);
		if (!otherGuild)
		{
			pSock.SysMessage("The requesting guild no longer exists.");
			RemoveGuildRelationRequestByIndex(guildinfo, reqIndex);
			ClassicGuildDiplomacy(pUser);
			return true;
		}

		if (acceptReq)
		{
			var okSet = SetGuildRelation(guildinfo.id, otherGuild.id, req.relation | 0);
			if (okSet)
			{
				if ((req.relation | 0) === 1)
					StartClassicWar(guildinfo, otherGuild, classicWarDefaultMaxKills, classicWarDefaultDurationHours);
				else if ((req.relation | 0) === 0)
					EndClassicWar(guildinfo, otherGuild);

				pSock.SysMessage("Relation with " + (otherGuild.name || "that guild") + " set to " + GetGuildRelationText(req.relation | 0) + ".");
				RemoveGuildRelationRequestByIndex(guildinfo, reqIndex);
			}
			else
			{
				pSock.SysMessage("Failed to apply relation.");
			}
		}
		else
		{
			RemoveGuildRelationRequestByIndex(guildinfo, reqIndex);
			pSock.SysMessage("You decline the request from " + (otherGuild.name || "that guild") + ".");
		}

		ClassicGuildDiplomacy(pUser);
		return true;
	}

	return false;
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
	if (IsGuildMaster(guildinfo, pUser))
	{
		guildMenu.AddButton(280, 240, 0xFA5, 0xFA7, 1, 0, 16002);
		guildMenu.AddHTMLGump(315, 238, 220, 24, false, false, "<basefont color=#ffffff>Pack guildstone</basefont>");
	}

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
			if (TriggerEvent(5022, "CanPlacePendingGuildstone", pUser) === false)
			{
				pSock.SysMessage("Unable to place the guildstone deed.");
				return;
			}

			pUser.TextMessage("Not currently in a guild... Creating new guild...", false, 0x3b2, 0, pUser.serial);
			var newGuild = CreateNewGuild(pUser, Text1, Text2);
			if (newGuild)
			{
				if (GetServerSetting( "ClassicOSIGuildMenu" ))
					EnsureClassicGuildRanks(newGuild, pUser);

				TriggerEvent(5022, "PlacePendingGuildstone", pUser, newGuild);
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

	if ((pButton >= 30001 && pButton <= 30099) || (pButton >= 30100 && pButton < 31800))
	{
		if (HandleClassicGuildButton(pSock, pUser, guildinfo, pButton, gumpData))
			return;
	}

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
	if (pButton === 16002)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can move the guildstone.");
			GuildMenu(pUser);
			return;
		}

		TriggerEvent(5022, "PackGuildstone", pUser, guildinfo);
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


function HandleRecruitAction(pSock, pUser, guildinfo, row, doAccept, refreshClassic)
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
		var startRankName = IsClassicGuildMode(guildinfo) ? "Ronin" : GetLowestRankName(guildinfo);

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
	if (refreshClassic)
		ClassicGuildCandidates(pUser);
	else
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

	if (IsClassicGuildMode(guild))
		return HasClassicRank(guild, pChar, "Emissary") || HasClassicRank(guild, pChar, "Guild Master");

	var prio = GetRankPriority(guild, pChar);
	// Veteran (30) and above can invite
	return prio >= 30;
}

function CanPromoteDemote(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true;

	if (IsClassicGuildMode(guild))
		return HasClassicRank(guild, pChar, "Emissary") || HasClassicRank(guild, pChar, "Guild Master");

	var prio = GetRankPriority(guild, pChar);
	// Officer (40) and above can promote/demote / set rank
	return prio >= 40;
}

function CanEditGuildWars(guild, pChar)
{
	if (IsGuildMaster(guild, pChar))
		return true; // GM always allowed

	if (IsClassicGuildMode(guild))
		return HasClassicRank(guild, pChar, "Warlord") || HasClassicRank(guild, pChar, "Guild Master");

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

function GetClassicWarNow()
{
	return Math.floor((new Date()).getTime() / 1000);
}

function GetClassicWarKey(otherGuildId)
{
	return "CLASSIC_WAR_" + (otherGuildId | 0);
}

function ParseClassicWarState(raw)
{
	var parts = String(raw || "").split(",");
	if (parts.length < 5)
		return null;

	return {
		start: parseInt(parts[0], 10) || 0,
		end: parseInt(parts[1], 10) || 0,
		maxKills: parseInt(parts[2], 10) || classicWarDefaultMaxKills,
		killsFor: parseInt(parts[3], 10) || 0,
		killsAgainst: parseInt(parts[4], 10) || 0
	};
}

function ReadClassicWarState(guild, otherGuild)
{
	if (!guild || !otherGuild)
		return null;

	var cfg = ReadGuildConfig(guild);
	return ParseClassicWarState(cfg[GetClassicWarKey(otherGuild.id)]);
}

function SaveClassicWarState(guild, otherGuild, state)
{
	if (!guild || !otherGuild || !state)
		return false;

	var cfg = ReadGuildConfig(guild);
	cfg[GetClassicWarKey(otherGuild.id)] = [
		state.start | 0,
		state.end | 0,
		state.maxKills | 0,
		state.killsFor | 0,
		state.killsAgainst | 0
	].join(",");
	return SaveGuildConfig(guild, cfg);
}

function ClearClassicWarState(guild, otherGuild)
{
	if (!guild || !otherGuild)
		return false;

	var cfg = ReadGuildConfig(guild);
	delete cfg[GetClassicWarKey(otherGuild.id)];
	return SaveGuildConfig(guild, cfg);
}

function StartClassicWar(guildOne, guildTwo, maxKills, durationHours)
{
	if (!guildOne || !guildTwo)
		return false;

	var now = GetClassicWarNow();
	var max = maxKills || classicWarDefaultMaxKills;
	var hours = durationHours || classicWarDefaultDurationHours;
	var state = {
		start: now,
		end: now + (hours * 3600),
		maxKills: max,
		killsFor: 0,
		killsAgainst: 0
	};

	SaveClassicWarState(guildOne, guildTwo, state);
	SaveClassicWarState(guildTwo, guildOne, state);
	return true;
}

function EndClassicWar(guildOne, guildTwo)
{
	if (!guildOne || !guildTwo)
		return false;

	ClearClassicWarState(guildOne, guildTwo);
	ClearClassicWarState(guildTwo, guildOne);
	return true;
}

function CheckClassicWarExpired(guildOne, guildTwo)
{
	var state = ReadClassicWarState(guildOne, guildTwo);
	if (!state || !state.end)
		return false;

	if (GetClassicWarNow() <= state.end)
		return false;

	EndClassicWar(guildOne, guildTwo);
	if (typeof SetGuildRelation !== "undefined")
		SetGuildRelation(guildOne.id, guildTwo.id, 0);
	return true;
}

function FormatClassicWarStatus(guild, otherGuild)
{
	var state = ReadClassicWarState(guild, otherGuild);
	if (!state)
		return "War";

	var remaining = Math.max(0, state.end - GetClassicWarNow());
	var hoursLeft = Math.ceil(remaining / 3600);
	return "War " + state.killsFor + "/" + state.maxKills + " (" + hoursLeft + "h)";
}

function NotifyGuildMembers(guild, message)
{
	if (!guild || !message)
		return;

	var members = (guild.member || guild.members) || [];
	for (var i = 0; i < members.length; i++)
	{
		var m = members[i];
		if (m && m.socket)
			m.socket.SysMessage(message);
	}
}

function RecordClassicWarKill(killerGuild, victimGuild)
{
	if (!killerGuild || !victimGuild || killerGuild.id === victimGuild.id)
		return false;

	var relation = (typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(killerGuild.id, victimGuild.id) : 3;
	if (relation !== 1)
		return false;

	if (CheckClassicWarExpired(killerGuild, victimGuild))
		return false;

	var killerState = ReadClassicWarState(killerGuild, victimGuild);
	var victimState = ReadClassicWarState(victimGuild, killerGuild);
	if (!killerState || !victimState)
	{
		StartClassicWar(killerGuild, victimGuild, classicWarDefaultMaxKills, classicWarDefaultDurationHours);
		killerState = ReadClassicWarState(killerGuild, victimGuild);
		victimState = ReadClassicWarState(victimGuild, killerGuild);
	}

	killerState.killsFor++;
	killerState.killsAgainst = victimState.killsFor;
	victimState.killsAgainst++;
	victimState.killsFor = killerState.killsAgainst;

	SaveClassicWarState(killerGuild, victimGuild, killerState);
	SaveClassicWarState(victimGuild, killerGuild, victimState);

	if (killerState.killsFor >= killerState.maxKills)
	{
		EndClassicWar(killerGuild, victimGuild);
		if (typeof SetGuildRelation !== "undefined")
			SetGuildRelation(killerGuild.id, victimGuild.id, 0);
		NotifyGuildMembers(killerGuild, "Your guild has won the war against " + (victimGuild.name || "the enemy guild") + ".");
		NotifyGuildMembers(victimGuild, "Your guild has lost the war against " + (killerGuild.name || "the enemy guild") + ".");
	}

	return true;
}

function OnGuildPlayerDeath(pDead, iCorpse)
{
	if (!ValidateObject(pDead) || !ValidateObject(iCorpse) || pDead.npc || !pDead.guild)
		return false;

	var killerSerial = iCorpse.morex | 0;
	if (!killerSerial || killerSerial < 0)
		return false;

	var killer = CalcCharFromSer(killerSerial);
	if (!ValidateObject(killer) || killer.npc || !killer.guild)
		return false;

	if (!GetServerSetting(165) && !IsClassicGuildMode(killer.guild) && !IsClassicGuildMode(pDead.guild))
		return false;

	return RecordClassicWarKill(killer.guild, pDead.guild);
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

	if (IsClassicGuildMode(guild))
		return HasClassicRank(guild, pChar, "Emissary") || HasClassicRank(guild, pChar, "Guild Master");

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
