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

function NewGuildCreation(pUser)
{
	if (!pUser || !pUser.socket)
		return;

	var create = new Gump;
	var ignoreInvites = IsIgnoringNewGuildInvites(pUser);

	create.AddPage(0);
	create.AddBackground(0, 0, 500, 390, 0x24AE);
	create.AddBackground(65, 50, 370, 30, 0x2486);
	create.AddHTMLGump(75, 55, 350, 26, false, false, NewGuildText("<center><i>Create Guild</i></center>"));
	create.AddHTMLGump(65, 95, 370, 40, true, false, NewGuildText("Enter a guild name and abbreviation to establish a new guild."));
	create.AddHTMLGump(65, 150, 140, 20, false, false, NewGuildText("Guild Name"));
	create.AddGump(65, 173, 1803);
	create.AddHTMLGump(65, 210, 140, 20, false, false, NewGuildText("Abbreviation"));
	create.AddGump(65, 233, 1803);
	create.AddHTMLGump(65, 255, 220, 18, false, false, NewGuildText("Registration Fee: " + GetNewGuildRegistrationFeeText(pUser)));
	create.AddHTMLGump(65, 275, 220, 22, false, false, NewGuildText("Guild invitations: " + (ignoreInvites ? "Off" : "On")));
	create.AddBackground(285, 270, 150, 26, 0x2486);
	create.AddButton(290, 275, 0x845, 0x846, 1, 0, newGuildInviteToggleButton);
	create.AddHTMLGump(315, 273, 110, 24, false, false, NewGuildText(ignoreInvites ? "Allow Invites" : "Ignore Invites"));
	create.AddBackground(220, 335, 110, 26, 0x2486);
	create.AddButton(225, 340, 0x845, 0x846, 1, 0, 0);
	create.AddHTMLGump(250, 338, 65, 24, false, false, NewGuildText("Cancel"));
	create.AddBackground(345, 335, 110, 26, 0x2486);
	create.AddButton(350, 340, 0x845, 0x846, 1, 0, newGuildCreateButton);
	create.AddHTMLGump(375, 338, 70, 24, false, false, NewGuildText("Create"));
	create.AddTextEntryLimited(70, 175, 250, 20, 0, 0, 9, SafeTextEntryValue((pUser.name || "New") + "'s Guild"), 33);
	create.AddTextEntryLimited(70, 235, 80, 20, 0, 1, 10, "NEW", 3);

	create.Send(pUser.socket);
	create.Free();
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

function SafeTextEntryValue(text)
{
	var value = String(text == null ? "" : text);
	return value.length ? value : " ";
}

var classicGuildRanks = [
	{ name: "Ronin", prio: 0 },
	{ name: "Member", prio: 20 },
	{ name: "Emissary", prio: 40 },
	{ name: "Warlord", prio: 40 },
	{ name: "Guild Master", prio: 50 }
];

var classicWarDefaultMaxKills = GetGuildSettingInt("ClassicGuildWarMaxKills", 100, 1);
var classicWarDefaultDurationHours = GetGuildSettingInt("ClassicGuildWarDurationHours", 168, 1);
var classicFealtyCheckIntervalSeconds = GetGuildSettingInt("ClassicGuildFealtyCheckSeconds", 86400, 1);
var classicGuildTypeChangeIntervalSeconds = GetGuildSettingInt("ClassicGuildTypeChangeSeconds", 604800, 1);
var classicGuildstoneRange = GetGuildSettingInt("ClassicGuildstoneRange", 2, 0);
var newGuildRegistrationFeeDefault = 25000;
var guildMenuSystemCustom = 0;
var guildMenuSystemClassicOSI = 1;
var guildMenuSystemNewOSI = 2;

function GetConfiguredGuildMenuSystem()
{
	var menuSystem = parseInt(GetServerSetting("GuildMenuSystem"), 10);
	if (isNaN(menuSystem))
		menuSystem = GetServerSetting("ClassicOSIGuildMenu") ? guildMenuSystemClassicOSI : guildMenuSystemCustom;

	if (menuSystem < guildMenuSystemCustom || menuSystem > guildMenuSystemNewOSI)
		menuSystem = guildMenuSystemCustom;

	return menuSystem;
}

function UseClassicOSIGuildMenu()
{
	return GetConfiguredGuildMenuSystem() === guildMenuSystemClassicOSI;
}

function UseNewOSIGuildMenu()
{
	return GetConfiguredGuildMenuSystem() === guildMenuSystemNewOSI;
}

function GetGuildSettingInt(settingName, fallbackValue, minValue)
{
	var value = parseInt(GetServerSetting(settingName), 10);
	if (isNaN(value) || value < minValue)
		return fallbackValue;

	return value;
}

function GetNewGuildRegistrationFee()
{
	return GetGuildSettingInt("GuildRegistrationFee", newGuildRegistrationFeeDefault, 0);
}

function GetNewGuildRegistrationFeeText(pUser)
{
	if (pUser && pUser.isGM)
		return "Free";

	return String(GetNewGuildRegistrationFee());
}

function TryPayNewGuildRegistrationFee(pSock, pUser)
{
	if (!pUser || pUser.isGM)
		return true;

	var newGuildRegistrationFee = GetNewGuildRegistrationFee();
	if (newGuildRegistrationFee <= 0)
		return true;

	var gold = pUser.ResourceCount ? (pUser.ResourceCount(0x0EED, 0) | 0) : 0;
	if (gold < newGuildRegistrationFee)
	{
		if (pSock)
			pSock.SysMessage("You need " + newGuildRegistrationFee + " gold to register a guild.");
		return false;
	}

	var spent = pUser.UseResource ? (pUser.UseResource(newGuildRegistrationFee, 0x0EED, 0) | 0) : 0;
	if (spent < newGuildRegistrationFee)
	{
		if (pSock)
			pSock.SysMessage("Unable to collect the guild registration fee.");
		return false;
	}

	return true;
}

function RequireGuildstoneForConfiguredGuildMenu(guild)
{
	var menuSystem = GetConfiguredGuildMenuSystem();
	return menuSystem === guildMenuSystemClassicOSI || (menuSystem === guildMenuSystemCustom && IsClassicGuildMode(guild));
}

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

function IsNearClassicGuildstone(pUser, guild)
{
	if (!pUser || !guild)
		return false;

	var stone = guild.stone;
	if (!ValidateObject(stone) || !stone.isItem)
		return false;

	if (pUser.InRange)
		return pUser.InRange(stone, classicGuildstoneRange);

	if (pUser.DistanceTo)
		return pUser.DistanceTo(stone) <= classicGuildstoneRange;

	return false;
}

function RequireClassicGuildstoneRange(pUser, guild)
{
	if (IsNearClassicGuildstone(pUser, guild))
		return true;

	if (pUser && pUser.socket)
		pUser.socket.SysMessage("You must be near your guildstone to use classic guild functions.");

	return false;
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

function NewGuildText(text)
{
	return "<basefont color=#111111>" + String(text || "") + "</basefont>";
}

var newGuildInviteAcceptButton = 33510;
var newGuildInviteDeclineButton = 33511;
var newGuildInviteIgnoreButton = 33512;
var newGuildCreateButton = 32990;
var newGuildInviteToggleButton = 32991;

function IsIgnoringNewGuildInvites(pUser)
{
	return !!(pUser && pUser.GetTag && String(pUser.GetTag("newGuildIgnoreInvites") || "") === "1");
}

function SetIgnoringNewGuildInvites(pUser, ignoreInvites)
{
	if (pUser && pUser.SetTag)
		pUser.SetTag("newGuildIgnoreInvites", ignoreInvites ? "1" : null);
}

function ClearLocalGuildState(pChar)
{
	if (!pChar)
		return;

	pChar.guild = null;
	pChar.guildTitle = "";
	if (pChar.SetGuildFealty)
		pChar.SetGuildFealty(0);
	if (pChar.Refresh)
		pChar.Refresh();
}

function ClearNewGuildInvitation(pUser)
{
	if (!pUser || !pUser.SetTag)
		return;

	pUser.SetTag("newGuildInviteGuildId", null);
	pUser.SetTag("newGuildInviteInviterSerial", null);
	pUser.SetTag("newGuildInviteInviterName", null);
}

function NewGuildInvitationRequest(pTarget, guild, inviter)
{
	if (!pTarget || !pTarget.socket || !guild)
		return false;

	var invite = new Gump;
	var guildName = guild.name || "this guild";
	var inviterName = inviter && inviter.name ? inviter.name : "A guild member";

	ClearNewGuildInvitation(pTarget);
	pTarget.SetTag("newGuildInviteGuildId", String(guild.id | 0));
	pTarget.SetTag("newGuildInviteInviterSerial", inviter ? String(inviter.serial) : "0");
	pTarget.SetTag("newGuildInviteInviterName", inviterName);

	invite.AddPage(0);
	invite.AddBackground(0, 0, 350, 170, 0x242C);
	invite.AddHTMLGump(20, 15, 310, 24, false, false, NewGuildText("<center><i>Guild Invitation</i></center>"));
	invite.AddTiledGump(20, 42, 310, 2, 0x2711);
	invite.AddHTMLGump(20, 55, 310, 38, true, false, NewGuildText(inviterName + " has invited you to join " + guildName + "."));
	invite.AddHTMLGump(20, 95, 310, 20, false, false, NewGuildText("Do you wish to join this guild?"));

	invite.AddBackground(20, 125, 90, 26, 0x2486);
	invite.AddButton(25, 130, 0x845, 0x846, 1, 0, newGuildInviteAcceptButton);
	invite.AddHTMLGump(50, 128, 55, 24, false, false, NewGuildText("Accept"));
	invite.AddBackground(130, 125, 90, 26, 0x2486);
	invite.AddButton(135, 130, 0x845, 0x846, 1, 0, newGuildInviteDeclineButton);
	invite.AddHTMLGump(160, 128, 55, 24, false, false, NewGuildText("Decline"));
	invite.AddBackground(240, 125, 90, 26, 0x2486);
	invite.AddButton(245, 130, 0x845, 0x846, 1, 0, newGuildInviteIgnoreButton);
	invite.AddHTMLGump(270, 128, 55, 24, false, false, NewGuildText("Ignore"));

	invite.Send(pTarget.socket);
	invite.Free();
	return true;
}

function GetNewGuildInvitationGuild(pUser)
{
	if (!pUser || !pUser.GetTag)
		return null;

	var guildId = parseInt(pUser.GetTag("newGuildInviteGuildId"), 10);
	if (isNaN(guildId) || guildId < 0)
		return null;

	return FindGuildById(guildId);
}

function GetNewGuildInvitationInviter(pUser)
{
	if (!pUser || !pUser.GetTag)
		return null;

	var serial = parseInt(pUser.GetTag("newGuildInviteInviterSerial"), 10);
	if (isNaN(serial) || serial <= 0)
		return null;

	return CalcCharFromSer(serial);
}

function HandleNewGuildInvitationResponse(pSock, pUser, pButton)
{
	if (pButton !== newGuildInviteAcceptButton && pButton !== newGuildInviteDeclineButton && pButton !== newGuildInviteIgnoreButton)
		return false;

	if (GetConfiguredGuildMenuSystem() !== guildMenuSystemNewOSI)
	{
		ClearNewGuildInvitation(pUser);
		return true;
	}

	var guild = GetNewGuildInvitationGuild(pUser);
	var inviter = GetNewGuildInvitationInviter(pUser);
	var inviterName = String((pUser.GetTag && pUser.GetTag("newGuildInviteInviterName")) || "The inviter");

	if (pButton === newGuildInviteIgnoreButton)
		SetIgnoringNewGuildInvites(pUser, true);

	if (!guild)
	{
		pSock.SysMessage("That guild invitation is no longer valid.");
		ClearNewGuildInvitation(pUser);
		return true;
	}

	if (pButton === newGuildInviteDeclineButton || pButton === newGuildInviteIgnoreButton)
	{
		pSock.SysMessage("You decline the invitation to join " + (guild.name || "that guild") + ".");
		if (inviter && inviter.socket)
			inviter.socket.SysMessage((pUser.name || "The player") + " declined your guild invitation.");
		ClearNewGuildInvitation(pUser);
		return true;
	}

	if (pUser.guild)
	{
		pSock.SysMessage("You are already in a guild.");
		if (inviter && inviter.socket)
			inviter.socket.SysMessage((pUser.name || "The player") + " could not join because they are already in a guild.");
		ClearNewGuildInvitation(pUser);
		return true;
	}

	if (!guild.AddMember || !guild.AddMember(pUser))
	{
		pSock.SysMessage("You could not join that guild.");
		if (inviter && inviter.socket)
			inviter.socket.SysMessage((pUser.name || "The player") + " could not join your guild.");
		ClearNewGuildInvitation(pUser);
		return true;
	}

	var startRankName = GetLowestRankName(guild) || "Recruit";
	try
	{
		if (guild.SetRank && startRankName.length)
		{
			guild.SetRank(pUser, startRankName);
			pUser.guildTitle = startRankName;
		}
	}
	catch (e) { /* ignore rank setup failures */ }

	if (pUser.Refresh)
		pUser.Refresh();

	pSock.SysMessage("You have joined " + (guild.name || "the guild") + ".");
	if (inviter && inviter.socket)
		inviter.socket.SysMessage((pUser.name || "The player") + " has accepted your guild invitation.");
	else if (inviterName.length)
		pSock.SysMessage(inviterName + " is not available to receive your response.");

	ClearNewGuildInvitation(pUser);
	NewGuildMenu(pUser);
	return true;
}

function AddNewGuildTab(gump, x, buttonId, label, active)
{
	gump.AddBackground(x, 40, 150, 26, 0x2486);
	gump.AddButton(x + 5, 45, 0x845, 0x846, 1, 0, buttonId);
	gump.AddHTMLGump(x + 30, 43, 110, 26, false, false, active ? "<basefont color=#0000AA>" + label + "</basefont>" : NewGuildText(label));
}

function AddNewGuildFrame(gump, activeTab)
{
	gump.AddPage(0);
	gump.AddBackground(0, 0, 600, 440, 0x24AE);
	AddNewGuildTab(gump, 66, 33001, "My Guild", activeTab === 1);
	AddNewGuildTab(gump, 236, 33002, "Guild Roster", activeTab === 2);
	AddNewGuildTab(gump, 401, 33003, "Diplomacy", activeTab === 3);
}

function NewGuildMenu(pUser)
{
	NewGuildInfoMenu(pUser, false);
}

function NewGuildInfoMenu(pUser, confirmResign, confirmDisband)
{
	if (!pUser || !pUser.socket)
		return;

	var guildinfo = pUser.guild;
	if (!guildinfo)
	{
		NewGuildCreation(pUser);
		return;
	}

	var info = new Gump;
	var isLeader = IsGuildMaster(guildinfo, pUser);
	var charter = guildinfo.charter && guildinfo.charter.length ? guildinfo.charter : "The guild leader has not yet set the guild charter.";
	var webPage = guildinfo.webPage && guildinfo.webPage.length ? guildinfo.webPage : "Guild website not yet set.";
	var alliance = guildinfo.alliance || "";

	AddNewGuildFrame(info, 1);
	info.AddTiledGump(65, 80, 160, 26, 0xA40);
	info.AddTiledGump(67, 82, 156, 22, 0xBBC);
	info.AddHTMLGump(70, 83, 150, 20, false, false, NewGuildText("Guild Name"));
	info.AddHTMLGump(233, 84, 320, 26, false, false, NewGuildText(guildinfo.name || "Unnamed Guild"));

	info.AddTiledGump(65, 114, 160, 26, 0xA40);
	info.AddTiledGump(67, 116, 156, 22, 0xBBC);
	info.AddHTMLGump(70, 117, 150, 20, false, false, NewGuildText("Alliance"));
	info.AddHTMLGump(233, 118, 320, 26, false, false, NewGuildText(alliance || "None"));

	info.AddTiledGump(65, 148, 160, 26, 0xA40);
	info.AddTiledGump(67, 150, 156, 22, 0xBBC);
	info.AddHTMLGump(70, 151, 150, 20, false, false, NewGuildText("Guild Type"));
	info.AddHTMLGump(233, 152, 320, 26, false, false, NewGuildText(GetGuildTypeName(guildinfo.type | 0)));

	info.AddTiledGump(65, 196, 480, 4, 0x238D);
	info.AddHTMLGump(65, 216, 480, 80, true, true, NewGuildText(charter));
	info.AddHTMLGump(65, 306, 480, 30, true, false, NewGuildText(webPage));

	if (isLeader)
	{
		info.AddBackground(65, 350, 170, 26, 0x2486);
		info.AddButton(70, 355, 0x845, 0x846, 1, 0, 33004);
		info.AddHTMLGump(95, 353, 130, 26, false, false, NewGuildText("Edit Guild Info"));
		info.AddBackground(245, 350, 170, 26, 0x2486);
		info.AddButton(250, 355, 0x845, 0x846, 1, 0, confirmDisband ? 33008 : 33007);
		info.AddHTMLGump(275, 353, 130, 26, false, false, confirmDisband ? "<basefont color=#990000>Confirm Disband</basefont>" : NewGuildText("Disband Guild"));
	}

	info.AddBackground(445, 350, 100, 26, 0x2486);
	info.AddButton(450, 355, 0x845, 0x846, 1, 0, confirmResign ? 33006 : 33005);
	info.AddHTMLGump(475, 353, 60, 26, false, false, confirmResign ? "<basefont color=#990000>Confirm</basefont>" : NewGuildText("Resign"));

	info.Send(pUser.socket);
	info.Free();
}

function NewGuildInfoEdit(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var edit = new Gump;

	AddNewGuildFrame(edit, 1);
	edit.AddBackground(65, 75, 480, 30, 0xBB8);
	edit.AddHTMLGump(75, 80, 460, 24, false, false, NewGuildText("<center><i>Edit Guild Information</i></center>"));

	edit.AddTiledGump(65, 120, 140, 26, 0xA40);
	edit.AddTiledGump(207, 120, 338, 26, 0xBBC);
	edit.AddHTMLGump(70, 123, 130, 20, false, false, "<basefont color=#ffffff>Guild Name</basefont>");

	edit.AddTiledGump(65, 155, 140, 26, 0xA40);
	edit.AddTiledGump(207, 155, 120, 26, 0xBBC);
	edit.AddHTMLGump(70, 158, 130, 20, false, false, "<basefont color=#ffffff>Abbreviation</basefont>");

	edit.AddTiledGump(65, 190, 140, 26, 0xA40);
	edit.AddTiledGump(207, 190, 338, 26, 0xBBC);
	edit.AddHTMLGump(70, 193, 130, 20, false, false, "<basefont color=#ffffff>Web Page</basefont>");

	edit.AddTiledGump(65, 225, 140, 78, 0xA40);
	edit.AddTiledGump(207, 225, 338, 78, 0xBBC);
	edit.AddHTMLGump(70, 228, 130, 20, false, false, "<basefont color=#ffffff>Charter</basefont>");

	edit.AddBackground(245, 350, 135, 26, 0x2486);
	edit.AddButton(250, 355, 0x845, 0x846, 1, 0, 33009);
	edit.AddHTMLGump(275, 353, 95, 24, false, false, NewGuildText("Save"));
	edit.AddBackground(400, 350, 135, 26, 0x2486);
	edit.AddButton(405, 355, 0x845, 0x846, 1, 0, 33001);
	edit.AddHTMLGump(430, 353, 95, 24, false, false, NewGuildText("Cancel"));
	edit.AddTextEntryLimited(212, 123, 328, 20, 0, 0, 10, SafeTextEntryValue(guildinfo.name), 33);
	edit.AddTextEntryLimited(212, 158, 110, 20, 0, 1, 11, SafeTextEntryValue(guildinfo.abbreviation), 3);
	edit.AddTextEntryLimited(212, 193, 328, 20, 0, 2, 12, SafeTextEntryValue(guildinfo.webPage), 60);
	edit.AddTextEntryLimited(212, 228, 328, 72, 0, 3, 13, SafeTextEntryValue(guildinfo.charter), 168);

	edit.Send(pUser.socket);
	edit.Free();
}

function NewGuildRosterMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var members = (guildinfo.member || guildinfo.members) || [];
	var offset = GetClassicOffset(pUser, "newGuildRosterOffset");
	var pageSize = 8;
	var roster = new Gump;

	if (offset >= members.length)
		offset = 0;

	AddNewGuildFrame(roster, 2);
	roster.AddBackground(130, 75, 385, 30, 0xBB8);
	roster.AddHTMLGump(140, 80, 360, 24, false, false, NewGuildText("Members: " + members.length));

	roster.AddTiledGump(65, 110, 140, 26, 0xA40);
	roster.AddTiledGump(207, 110, 90, 26, 0xA40);
	roster.AddTiledGump(299, 110, 90, 26, 0xA40);
	roster.AddTiledGump(391, 110, 155, 26, 0xA40);
	roster.AddHTMLGump(70, 113, 130, 20, false, false, "<basefont color=#ffffff>Name</basefont>");
	roster.AddHTMLGump(212, 113, 80, 20, false, false, "<basefont color=#ffffff>Rank</basefont>");
	roster.AddHTMLGump(304, 113, 80, 20, false, false, "<basefont color=#ffffff>Status</basefont>");
	roster.AddHTMLGump(396, 113, 145, 20, false, false, "<basefont color=#ffffff>Guild Title</basefont>");

	for (var i = 0; i < pageSize && offset + i < members.length; i++)
	{
		var member = members[offset + i];
		if (!member)
			continue;

		var y = 138 + i * 28;
		var name = member.name || ("0x" + member.serial.toString(16).toUpperCase());
		var rank = GetRankName(guildinfo, member) || member.guildTitle || "";
		var status = member.online ? "Online" : "Offline";

		roster.AddButton(40, y + 5, 0x4B9, 0x4BA, 1, 0, 33100 + i);
		roster.AddTiledGump(65, y, 140, 26, 0xBBC);
		roster.AddTiledGump(207, y, 90, 26, 0xBBC);
		roster.AddTiledGump(299, y, 90, 26, 0xBBC);
		roster.AddTiledGump(391, y, 155, 26, 0xBBC);
		roster.AddHTMLGump(70, y + 3, 130, 20, false, false, NewGuildText(name));
		roster.AddHTMLGump(212, y + 3, 80, 20, false, false, NewGuildText(rank || "Member"));
		roster.AddHTMLGump(304, y + 3, 80, 20, false, false, NewGuildText(status));
		roster.AddHTMLGump(396, y + 3, 145, 20, false, false, NewGuildText(member.guildTitle || ""));
	}

	AddClassicPaging(roster, 65, 365, offset, pageSize, members.length, 33020, 33021);
	if (CanClassicInvite(guildinfo, pUser))
	{
		roster.AddButton(230, 370, 0x845, 0x846, 1, 0, 33022);
		roster.AddHTMLGump(255, 368, 110, 26, false, false, NewGuildText("Invite Player"));
	}

	roster.Send(pUser.socket);
	roster.Free();
}

function NewGuildMemberInfoMenu(pUser, memberIndex, confirmKick)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var members = (guildinfo.member || guildinfo.members) || [];
	var member = members[memberIndex];
	if (!member)
	{
		pUser.socket.SysMessage("That guild member is no longer available.");
		NewGuildRosterMenu(pUser);
		return;
	}

	SetClassicOffset(pUser, "newGuildMemberIndex", memberIndex);

	var memberInfo = new Gump;
	var name = member.name || ("0x" + member.serial.toString(16).toUpperCase());
	var rank = GetRankName(guildinfo, member) || member.guildTitle || "Member";

	memberInfo.AddPage(0);
	memberInfo.AddBackground(0, 0, 350, 255, 0x242C);
	memberInfo.AddHTMLGump(20, 15, 310, 26, false, false, NewGuildText("<center><i>Guild Member Information</i></center>"));
	memberInfo.AddTiledGump(20, 40, 310, 2, 0x2711);
	memberInfo.AddHTMLGump(20, 50, 150, 26, true, false, NewGuildText("Name"));
	memberInfo.AddHTMLGump(180, 53, 150, 26, false, false, NewGuildText(name));
	memberInfo.AddHTMLGump(20, 80, 150, 26, true, false, NewGuildText("Rank"));
	memberInfo.AddHTMLGump(180, 83, 150, 26, false, false, NewGuildText(rank));
	memberInfo.AddHTMLGump(20, 110, 150, 26, true, false, NewGuildText("Guild Title"));
	memberInfo.AddHTMLGump(180, 113, 150, 26, false, false, NewGuildText(member.guildTitle || ""));
	memberInfo.AddTiledGump(20, 142, 310, 2, 0x2711);

	memberInfo.AddBackground(20, 150, 310, 26, 0x2486);
	memberInfo.AddButton(25, 155, 0x845, 0x846, 1, 0, 33150);
	memberInfo.AddHTMLGump(50, 153, 270, 26, false, false, NewGuildText("Cast Vote For This Member"));

	memberInfo.AddBackground(20, 180, 150, 26, 0x2486);
	memberInfo.AddButton(25, 185, 0x845, 0x846, 1, 0, 33180);
	memberInfo.AddHTMLGump(50, 183, 110, 26, false, false, NewGuildText("Promote"));

	memberInfo.AddBackground(180, 180, 150, 26, 0x2486);
	memberInfo.AddButton(185, 185, 0x845, 0x846, 1, 0, 33210);
	memberInfo.AddHTMLGump(210, 183, 110, 26, false, false, NewGuildText("Set Title"));

	memberInfo.AddBackground(20, 210, 150, 26, 0x2486);
	memberInfo.AddButton(25, 215, 0x845, 0x846, 1, 0, 33240);
	memberInfo.AddHTMLGump(50, 213, 110, 26, false, false, NewGuildText("Demote"));

	memberInfo.AddBackground(180, 210, 150, 26, 0x2486);
	memberInfo.AddButton(185, 215, 0x845, 0x846, 1, 0, confirmKick ? 33300 : 33270);
	memberInfo.AddHTMLGump(210, 213, 110, 26, false, false, confirmKick ? "<basefont color=#990000>Confirm Kick</basefont>" : NewGuildText("Kick"));

	memberInfo.Send(pUser.socket);
	memberInfo.Free();
}

function NewGuildMemberTitleEdit(pUser, memberIndex)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var members = (guildinfo.member || guildinfo.members) || [];
	var member = members[memberIndex];
	if (!member)
	{
		pUser.socket.SysMessage("That guild member is no longer available.");
		NewGuildRosterMenu(pUser);
		return;
	}

	SetClassicOffset(pUser, "newGuildMemberIndex", memberIndex);

	var titleEdit = new Gump;
	var memberName = member.name || ("0x" + member.serial.toString(16).toUpperCase());

	titleEdit.AddPage(0);
	titleEdit.AddBackground(0, 0, 350, 190, 0x242C);
	titleEdit.AddHTMLGump(20, 15, 310, 26, false, false, NewGuildText("<center><i>Set Guild Title</i></center>"));
	titleEdit.AddTiledGump(20, 42, 310, 2, 0x2711);
	titleEdit.AddHTMLGump(20, 55, 90, 24, true, false, NewGuildText("Member"));
	titleEdit.AddHTMLGump(120, 58, 210, 24, false, false, NewGuildText(memberName));
	titleEdit.AddHTMLGump(20, 90, 90, 24, true, false, NewGuildText("Title"));
	titleEdit.AddTiledGump(120, 88, 210, 26, 0xBBC);

	titleEdit.AddBackground(55, 140, 110, 26, 0x2486);
	titleEdit.AddButton(60, 145, 0x845, 0x846, 1, 0, 33211);
	titleEdit.AddHTMLGump(85, 143, 70, 24, false, false, NewGuildText("Save"));
	titleEdit.AddBackground(185, 140, 110, 26, 0x2486);
	titleEdit.AddButton(190, 145, 0x845, 0x846, 1, 0, 33212);
	titleEdit.AddHTMLGump(215, 143, 70, 24, false, false, NewGuildText("Cancel"));
	titleEdit.AddTextEntryLimited(125, 91, 200, 20, 0, 0, 6, SafeTextEntryValue(member.guildTitle), 32);

	titleEdit.Send(pUser.socket);
	titleEdit.Free();
}

function NewGuildDiplomacyMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var offset = GetClassicOffset(pUser, "newGuildDiplomacyOffset");
	var pageSize = 8;
	var filter = GetNewGuildDiplomacyFilter(pUser);
	var displayMode = GetNewGuildDiplomacyDisplay(pUser);
	var list = GetNewGuildDiplomacyList(pUser, guildinfo);
	var diplomacy = new Gump;

	if (offset >= list.length)
		offset = 0;

	AddNewGuildFrame(diplomacy, 3);
	diplomacy.AddBackground(65, 75, 260, 30, 0xBB8);
	AddNewGuildModeButton(diplomacy, 335, 77, 33032, "Filter", false);
	AddNewGuildModeButton(diplomacy, 455, 77, 33033, "Clear", false);
	AddNewGuildModeButton(diplomacy, 65, 110, 33034, "All", displayMode === 0);
	AddNewGuildModeButton(diplomacy, 205, 110, 33035, "Awaiting Action", displayMode === 1);
	AddNewGuildModeButton(diplomacy, 385, 110, 33036, "Relations", displayMode === 2);

	diplomacy.AddHTMLGump(65, 145, 350, 20, false, false, NewGuildText("Showing " + list.length + " guilds"));
	diplomacy.AddTiledGump(65, 170, 290, 26, 0xA40);
	diplomacy.AddTiledGump(357, 170, 60, 26, 0xA40);
	diplomacy.AddTiledGump(419, 170, 120, 26, 0xA40);
	diplomacy.AddHTMLGump(70, 173, 280, 20, false, false, "<basefont color=#ffffff>Guild Name</basefont>");
	diplomacy.AddHTMLGump(362, 173, 50, 20, false, false, "<basefont color=#ffffff>Abbr</basefont>");
	diplomacy.AddHTMLGump(424, 173, 110, 20, false, false, "<basefont color=#ffffff>Status</basefont>");

	for (var row = 0; row < pageSize && offset + row < list.length; row++)
	{
		var otherGuild = list[offset + row];
		var y = 198 + row * 22;
		var rel = otherGuild.id === guildinfo.id ? 4 : ((typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(guildinfo.id, otherGuild.id) : 3);
		var status = rel === 1 ? FormatClassicWarStatus(guildinfo, otherGuild) : GetGuildRelationText(rel);

		diplomacy.AddButton(40, y + 3, 0x4B9, 0x4BA, 1, 0, 33350 + row);
		diplomacy.AddTiledGump(65, y, 290, 22, 0xBBC);
		diplomacy.AddTiledGump(357, y, 60, 22, 0xBBC);
		diplomacy.AddTiledGump(419, y, 120, 22, 0xBBC);
		diplomacy.AddHTMLGump(70, y + 3, 280, 20, false, false, NewGuildText(otherGuild.name || ("Guild #" + otherGuild.id)));
		diplomacy.AddHTMLGump(362, y + 3, 50, 20, false, false, NewGuildText(otherGuild.abbreviation || ""));
		diplomacy.AddHTMLGump(424, y + 3, 110, 20, false, false, NewGuildText(status));
	}

	AddClassicPaging(diplomacy, 65, 365, offset, pageSize, list.length, 33030, 33031);
	var diplomacyEntryIndex = 13 + Math.min(pageSize, Math.max(0, list.length - offset)) * 3;
	if (offset > 0)
		diplomacyEntryIndex++;
	if (offset + pageSize < list.length)
		diplomacyEntryIndex++;
	diplomacy.AddTextEntryLimited(70, 80, 250, 20, 0x481, 0, diplomacyEntryIndex, SafeTextEntryValue(filter), 32);
	diplomacy.Send(pUser.socket);
	diplomacy.Free();
}

function NewOtherGuildInfoMenu(pUser, otherGuildId)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var otherGuild = FindGuildById(otherGuildId);
	if (!otherGuild)
	{
		pUser.socket.SysMessage("That guild is no longer available.");
		NewGuildDiplomacyMenu(pUser);
		return;
	}

	if (otherGuild.id !== guildinfo.id)
		SetNewGuildTarget(pUser, otherGuild.id);

	var detail = new Gump;
	var rel = otherGuild.id === guildinfo.id ? 4 : ((typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(guildinfo.id, otherGuild.id) : 3);
	var status = rel === 1 ? FormatClassicWarStatus(guildinfo, otherGuild) : GetGuildRelationText(rel);

	detail.AddPage(0);
	detail.AddBackground(0, 0, 520, 335, 0x242C);
	detail.AddHTMLGump(20, 15, 480, 26, false, false, NewGuildText("<center><i>Guild Relationship</i></center>"));
	detail.AddTiledGump(20, 40, 480, 2, 0x2711);
	detail.AddHTMLGump(20, 50, 120, 26, true, false, NewGuildText("Guild Name"));
	detail.AddHTMLGump(150, 53, 360, 26, false, false, NewGuildText(otherGuild.name || ("Guild #" + otherGuild.id)));
	detail.AddHTMLGump(20, 80, 120, 26, true, false, NewGuildText("Alliance"));
	detail.AddHTMLGump(150, 83, 360, 26, false, false, NewGuildText(otherGuild.alliance || "None"));
	detail.AddHTMLGump(20, 110, 120, 26, true, false, NewGuildText("Abbreviation"));
	detail.AddHTMLGump(150, 113, 120, 26, false, false, NewGuildText(otherGuild.abbreviation || ""));
	detail.AddHTMLGump(280, 110, 120, 26, true, false, NewGuildText("Status"));
	detail.AddHTMLGump(410, 113, 120, 26, false, false, NewGuildText(status));
	detail.AddTiledGump(20, 172, 480, 2, 0x2711);
	detail.AddHTMLGump(20, 180, 480, 40, true, false, NewGuildText(GetNewGuildRelationshipText(guildinfo, otherGuild, rel)));
	detail.AddTiledGump(20, 245, 480, 2, 0x2711);

	if (otherGuild.id !== guildinfo.id && CanClassicControlWar(guildinfo, pUser))
	{
		if (rel === 1)
		{
			AddNewGuildAction(detail, 20, 290, 33402, "Surrender / Offer Peace");
		}
		else
		{
			AddNewGuildAction(detail, 20, 260, 33403, "Request Alliance");
			AddNewGuildAction(detail, 20, 290, 33401, "Declare War");
		}
	}

	AddNewGuildAction(detail, 275, 290, 33003, "Cancel");
	detail.Send(pUser.socket);
	detail.Free();
}

function AddNewGuildAction(gump, x, y, buttonId, label)
{
	gump.AddBackground(x, y, 225, 26, 0x2486);
	gump.AddButton(x + 5, y + 5, 0x845, 0x846, 1, 0, buttonId);
	gump.AddHTMLGump(x + 30, y + 3, 185, 26, false, false, NewGuildText(label));
}

function AddNewGuildModeButton(gump, x, y, buttonId, label, active)
{
	gump.AddBackground(x, y, label.length > 8 ? 170 : 120, 26, 0x2486);
	gump.AddButton(x + 5, y + 5, 0x845, 0x846, 1, 0, buttonId);
	gump.AddHTMLGump(x + 30, y + 3, label.length > 8 ? 130 : 80, 26, false, false, active ? "<basefont color=#0000AA>" + label + "</basefont>" : NewGuildText(label));
}

function GetNewGuildRelationshipText(guild, otherGuild, rel)
{
	if (guild && otherGuild && guild.id === otherGuild.id)
		return "This is your guild.";
	if (rel === 1)
		return "You are at war with this guild.";
	if (rel === 2)
		return "You are allied with this guild.";
	return "You are at peace with this guild.";
}

function SetNewGuildTarget(pUser, guildId)
{
	if (pUser && pUser.SetTag)
		pUser.SetTag("newGuildTargetId", String(guildId | 0));
}

function GetNewGuildTarget(pUser)
{
	if (!pUser || !pUser.GetTag)
		return null;

	var guildId = parseInt(pUser.GetTag("newGuildTargetId"), 10);
	if (isNaN(guildId) || guildId <= 0)
		return null;

	return FindGuildById(guildId);
}

function GetNewGuildDiplomacyFilter(pUser)
{
	if (!pUser || !pUser.GetTag)
		return "";

	return String(pUser.GetTag("newGuildDiplomacyFilter") || "");
}

function SetNewGuildDiplomacyFilter(pUser, filter)
{
	if (pUser && pUser.SetTag)
		pUser.SetTag("newGuildDiplomacyFilter", manualTrim(filter || ""));
}

function GetNewGuildDiplomacyDisplay(pUser)
{
	if (!pUser || !pUser.GetTag)
		return 0;

	var mode = parseInt(pUser.GetTag("newGuildDiplomacyDisplay"), 10);
	if (isNaN(mode) || mode < 0 || mode > 2)
		return 0;

	return mode;
}

function SetNewGuildDiplomacyDisplay(pUser, mode)
{
	if (pUser && pUser.SetTag)
		pUser.SetTag("newGuildDiplomacyDisplay", String(Math.max(0, Math.min(mode | 0, 2))));
}

function NewGuildHasPendingDiplomacy(guild, otherGuild)
{
	if (!guild || !otherGuild)
		return false;

	var incoming = ReadGuildRelationRequests(guild);
	for (var i = 0; i < incoming.length; i++)
	{
		if (incoming[i] && (incoming[i].fromId | 0) === (otherGuild.id | 0))
			return true;
	}

	return HasOutgoingRelationRequest(guild, otherGuild);
}

function GetNewGuildDiplomacyList(pUser, guild)
{
	var allGuilds = GetAllGuilds() || [];
	var filter = GetNewGuildDiplomacyFilter(pUser).toLowerCase();
	var displayMode = GetNewGuildDiplomacyDisplay(pUser);
	var list = [];

	for (var i = 0; i < allGuilds.length; i++)
	{
		var otherGuild = allGuilds[i];
		if (!otherGuild)
			continue;

		var name = String(otherGuild.name || "").toLowerCase();
		var abbr = String(otherGuild.abbreviation || "").toLowerCase();
		if (filter.length && name.indexOf(filter) === -1 && abbr.indexOf(filter) === -1)
			continue;

		var rel = otherGuild.id === guild.id ? 4 : ((typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(guild.id, otherGuild.id) : 3);
		if (displayMode === 1 && !NewGuildHasPendingDiplomacy(guild, otherGuild))
			continue;
		if (displayMode === 2 && rel !== 1 && rel !== 2)
			continue;

		list.push(otherGuild);
	}

	return list;
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

	if (!RequireClassicGuildstoneRange(pUser, guildinfo))
		return;

	if (UseClassicOSIGuildMenu() && IsGuildMaster(guildinfo, pUser) && !IsClassicGuildMode(guildinfo))
		EnsureClassicGuildRanks(guildinfo, pUser);

	if (IsClassicGuildMode(guildinfo))
		EvaluateClassicFealty(guildinfo, false);

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
	memberMenu.AddButton(300, 250, 4005, 4007, 1, 0, 31600 + memberIndex);
	memberMenu.AddHTMLGump(335, 248, 80, 20, false, false, ClassicGuildText("Set title"));

	memberMenu.AddButton(55, 300, 4017, 4019, 1, 0, 30900 + memberIndex);
	memberMenu.AddHTMLGump(90, 298, 180, 20, false, false, ClassicGuildText("Remove from guild"));
	memberMenu.AddTextEntryLimited(60, 252, 220, 20, 0, 0, 12, SafeTextEntryValue(member.guildTitle), 32);

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

function GetGuildTypeName(typeValue)
{
	if (typeValue === 1)
		return "Order";
	if (typeValue === 2)
		return "Chaos";
	return "Standard";
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

function ClassicGuildWarAdmin(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var warAdmin = new Gump;
	var activeWars = GetClassicWarEnemies(guildinfo);
	var incomingWars = GetIncomingRelationRequests(guildinfo, 1);
	var outgoingWars = GetOutgoingRelationRequests(guildinfo, 1);

	AddClassicGuildFrame(warAdmin, "Guild War Administration");
	AddClassicGuildButton(warAdmin, 55, 85, 30030, "View active wars");
	warAdmin.AddHTMLGump(315, 83, 80, 22, false, false, ClassicGuildText("(" + activeWars.length + ")"));
	AddClassicGuildButton(warAdmin, 55, 125, 30029, "Declare war");
	AddClassicGuildButton(warAdmin, 55, 165, 30026, "Accept or reject war invitations");
	warAdmin.AddHTMLGump(315, 163, 80, 22, false, false, ClassicGuildText("(" + incomingWars.length + ")"));
	AddClassicGuildButton(warAdmin, 55, 205, 30027, "Rescind war declarations");
	warAdmin.AddHTMLGump(315, 203, 80, 22, false, false, ClassicGuildText("(" + outgoingWars.length + ")"));
	AddClassicGuildButton(warAdmin, 55, 245, 30028, "Declare peace with enemies");
	AddClassicGuildButton(warAdmin, 55, 285, 30025, "View guild diplomacy");

	warAdmin.Send(pUser.socket);
	warAdmin.Free();
}

function ClassicIncomingWarInvites(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var incoming = GetIncomingRelationRequests(guildinfo, 1);
	var invites = new Gump;

	AddClassicGuildFrame(invites, "War Invitations");
	invites.AddHTMLGump(55, 85, 280, 22, false, false, ClassicGuildText("Guild"));
	invites.AddHTMLGump(335, 85, 160, 22, false, false, ClassicGuildText("Actions"));

	if (!incoming.length)
	{
		invites.AddHTMLGump(55, 120, 360, 22, false, false, ClassicGuildText("There are no war invitations."));
	}
	else
	{
		var maxRows = Math.min(10, incoming.length);
		for (var i = 0; i < maxRows; i++)
		{
			var row = incoming[i];
			var fromGuild = row.guild;
			var y = 120 + i * 22;
			var name = fromGuild ? (fromGuild.name || ("Guild #" + fromGuild.id)) : ("Guild #" + row.fromId);

			invites.AddHTMLGump(55, y, 260, 20, false, false, ClassicGuildText(name));
			invites.AddButton(325, y, 4005, 4007, 1, 0, 31800 + row.index);
			invites.AddHTMLGump(360, y - 2, 55, 20, false, false, ClassicGuildText("Accept"));
			invites.AddButton(415, y, 4017, 4019, 1, 0, 31900 + row.index);
			invites.AddHTMLGump(450, y - 2, 55, 20, false, false, ClassicGuildText("Reject"));
		}
	}

	invites.Send(pUser.socket);
	invites.Free();
}

function ClassicOutgoingWarDeclarations(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var outgoing = GetOutgoingRelationRequests(guildinfo, 1);
	var declarations = new Gump;

	AddClassicGuildFrame(declarations, "War Declarations");
	declarations.AddHTMLGump(55, 85, 280, 22, false, false, ClassicGuildText("Guild"));
	declarations.AddHTMLGump(335, 85, 160, 22, false, false, ClassicGuildText("Actions"));

	if (!outgoing.length)
	{
		declarations.AddHTMLGump(55, 120, 360, 22, false, false, ClassicGuildText("There are no outgoing war declarations."));
	}
	else
	{
		var maxRows = Math.min(10, outgoing.length);
		for (var i = 0; i < maxRows; i++)
		{
			var row = outgoing[i];
			var targetGuild = row.guild;
			var y = 120 + i * 22;
			var name = targetGuild ? (targetGuild.name || ("Guild #" + targetGuild.id)) : ("Guild #" + row.targetId);

			declarations.AddHTMLGump(55, y, 260, 20, false, false, ClassicGuildText(name));
			declarations.AddButton(325, y, 4017, 4019, 1, 0, 32000 + i);
			declarations.AddHTMLGump(360, y - 2, 90, 20, false, false, ClassicGuildText("Rescind"));
		}
	}

	declarations.Send(pUser.socket);
	declarations.Free();
}

function ClassicActiveWarsMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var enemies = GetClassicWarEnemies(guildinfo);
	var activeWars = new Gump;

	AddClassicGuildFrame(activeWars, "Active Wars");
	activeWars.AddHTMLGump(55, 85, 190, 22, false, false, ClassicGuildText("Enemy guild"));
	activeWars.AddHTMLGump(245, 85, 150, 22, false, false, ClassicGuildText("War status"));
	activeWars.AddHTMLGump(405, 85, 90, 22, false, false, ClassicGuildText("Action"));

	if (!enemies.length)
	{
		activeWars.AddHTMLGump(55, 120, 360, 22, false, false, ClassicGuildText("Your guild is not at war with anyone."));
	}
	else
	{
		var maxRows = Math.min(10, enemies.length);
		for (var i = 0; i < maxRows; i++)
		{
			var enemy = enemies[i];
			var enemyGuild = enemy.guild;
			var y = 120 + i * 22;
			var name = enemyGuild.name || ("Guild #" + enemyGuild.id);
			var status = FormatClassicWarStatus(guildinfo, enemyGuild);

			activeWars.AddHTMLGump(55, y, 185, 20, false, false, ClassicGuildText(name));
			activeWars.AddHTMLGump(245, y, 150, 20, false, false, ClassicGuildText(status));
			activeWars.AddButton(405, y, 4005, 4007, 1, 0, 32100 + i);
			activeWars.AddHTMLGump(440, y - 2, 90, 20, false, false, ClassicGuildText("Peace"));
		}
	}

	activeWars.Send(pUser.socket);
	activeWars.Free();
}

function ClassicDeclarePeaceMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var enemies = GetClassicWarEnemies(guildinfo);
	var peace = new Gump;

	AddClassicGuildFrame(peace, "Declare Peace");
	peace.AddHTMLGump(55, 85, 190, 22, false, false, ClassicGuildText("Enemy guild"));
	peace.AddHTMLGump(245, 85, 150, 22, false, false, ClassicGuildText("War status"));
	peace.AddHTMLGump(405, 85, 90, 22, false, false, ClassicGuildText("Action"));

	if (!enemies.length)
	{
		peace.AddHTMLGump(55, 120, 360, 22, false, false, ClassicGuildText("Your guild is not at war with anyone."));
	}
	else
	{
		var maxRows = Math.min(10, enemies.length);
		for (var i = 0; i < maxRows; i++)
		{
			var enemy = enemies[i];
			var enemyGuild = enemy.guild;
			var y = 120 + i * 22;
			var name = enemyGuild.name || ("Guild #" + enemyGuild.id);
			var status = FormatClassicWarStatus(guildinfo, enemyGuild);

			peace.AddHTMLGump(55, y, 185, 20, false, false, ClassicGuildText(name));
			peace.AddHTMLGump(245, y, 150, 20, false, false, ClassicGuildText(status));
			peace.AddButton(405, y, 4005, 4007, 1, 0, 32100 + i);
			peace.AddHTMLGump(440, y - 2, 90, 20, false, false, ClassicGuildText("Peace"));
		}
	}

	peace.Send(pUser.socket);
	peace.Free();
}

function ClassicDeclareWarMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var searchFilter = manualTrim(GetWarSearchFilter(pUser));
	var targets = GetClassicWarDeclarationTargets(guildinfo, searchFilter);
	var declareWar = new Gump;

	AddClassicGuildFrame(declareWar, "Declare War");
	declareWar.AddHTMLGump(55, 82, 210, 20, false, false, ClassicGuildText("Search guilds"));
	declareWar.AddGump(55, 105, 1803);
	AddClassicGuildButton(declareWar, 270, 105, 30031, "Search");
	AddClassicGuildButton(declareWar, 385, 105, 30032, "Clear");

	declareWar.AddHTMLGump(55, 150, 220, 22, false, false, ClassicGuildText("Guild"));
	declareWar.AddHTMLGump(280, 150, 120, 22, false, false, ClassicGuildText("Status"));
	declareWar.AddHTMLGump(405, 150, 90, 22, false, false, ClassicGuildText("Action"));

	if (!targets.length)
	{
		var emptyText = searchFilter.length ? "No guilds match that war search." : "There are no guilds available for a war declaration.";
		declareWar.AddHTMLGump(55, 185, 390, 22, false, false, ClassicGuildText(emptyText));
	}
	else
	{
		var maxRows = Math.min(8, targets.length);
		for (var i = 0; i < maxRows; i++)
		{
			var row = targets[i];
			var targetGuild = row.guild;
			var y = 185 + i * 22;
			var name = targetGuild.name || ("Guild #" + targetGuild.id);
			if (targetGuild.abbreviation && targetGuild.abbreviation.length)
				name += " [" + targetGuild.abbreviation + "]";

			declareWar.AddHTMLGump(55, y, 220, 20, false, false, ClassicGuildText(name));
			declareWar.AddHTMLGump(280, y, 120, 20, false, false, ClassicGuildText(row.status));
			declareWar.AddButton(405, y, 4017, 4019, 1, 0, 32200 + i);
			declareWar.AddHTMLGump(440, y - 2, 90, 20, false, false, ClassicGuildText("War"));
		}
	}

	var declareWarTextIndex = targets.length ? (9 + Math.min(8, targets.length) * 3) : 10;
	declareWar.AddTextEntryLimited(60, 107, 190, 20, 0, 0, declareWarTextIndex, SafeTextEntryValue(searchFilter), 32);

	declareWar.Send(pUser.socket);
	declareWar.Free();
}

function ClassicGuildMasterMenu(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var masterMenu = new Gump;
	var lastFealtyCheck = GetClassicLastFealtyCheck(guildinfo);
	var fealtyText = lastFealtyCheck ? ("Last fealty check: " + FormatClassicFealtyTimeAgo(GetClassicWarNow() - lastFealtyCheck) + " ago") : "Fealty has not been checked yet.";

	AddClassicGuildFrame(masterMenu, "Guildmaster Functions");
	AddClassicGuildButton(masterMenu, 55, 80, 30023, "Set guildmaster title");
	AddClassicGuildButton(masterMenu, 55, 110, 30013, "Pack guildstone");
	AddClassicGuildButton(masterMenu, 55, 140, 30014, "Initialize classic ranks");
	AddClassicGuildButton(masterMenu, 55, 170, 30015, "Edit guild information");
	AddClassicGuildButton(masterMenu, 55, 200, 30016, "Set guild type");
	AddClassicGuildButton(masterMenu, 55, 230, 30021, "Check fealty votes");
	AddClassicGuildButton(masterMenu, 55, 260, 30022, "Transfer guildmaster");
	AddClassicGuildButton(masterMenu, 300, 260, 30033, "Disband guild");
	if (IsClassicGuildMode(guildinfo))
		masterMenu.AddHTMLGump(55, 295, 420, 40, true, true, ClassicGuildText("Classic rank mode is enabled. " + fealtyText));
	else
		masterMenu.AddHTMLGump(55, 295, 420, 40, true, true, ClassicGuildText("Classic rank mode has not been initialized for this guild yet."));

	masterMenu.Send(pUser.socket);
	masterMenu.Free();
}

function ClassicGuildMasterTitleEdit(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var leader = guildinfo.master || pUser;
	var titleEdit = new Gump;

	AddClassicGuildFrame(titleEdit, "Set Guildmaster Title");
	titleEdit.AddHTMLGump(55, 95, 420, 40, true, true, ClassicGuildText("Enter the title to display before the guildmaster's name."));
	titleEdit.AddHTMLGump(55, 150, 140, 20, false, false, ClassicGuildText("Guildmaster title"));
	titleEdit.AddGump(55, 173, 1803);
	AddClassicGuildButton(titleEdit, 300, 225, 30024, "Save title");
	titleEdit.AddTextEntryLimited(60, 175, 260, 20, 0, 0, 6, SafeTextEntryValue(leader.guildTitle), 20);

	titleEdit.Send(pUser.socket);
	titleEdit.Free();
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

	edit.AddHTMLGump(330, 82, 140, 20, false, false, ClassicGuildText("Abbreviation"));
	edit.AddGump(330, 105, 1803);

	edit.AddHTMLGump(55, 140, 420, 20, false, false, ClassicGuildText("Web page"));
	edit.AddGump(55, 163, 1803);

	edit.AddHTMLGump(55, 198, 420, 20, false, false, ClassicGuildText("Charter"));

	AddClassicGuildButton(edit, 300, 315, 30017, "Save information");
	edit.AddTextEntryLimited(60, 107, 240, 20, 0, 0, 8, SafeTextEntryValue(guildinfo.name), 33);
	edit.AddTextEntryLimited(335, 107, 80, 20, 0, 1, 9, SafeTextEntryValue(guildinfo.abbreviation), 3);
	edit.AddTextEntryLimited(60, 165, 360, 20, 0, 2, 10, SafeTextEntryValue(guildinfo.webPage), 60);
	edit.AddTextEntryLimited(55, 225, 420, 70, 0, 3, 11, SafeTextEntryValue(guildinfo.charter), 168);

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
	var remaining = GetClassicTypeChangeRemaining(guildinfo);
	var statusText = remaining > 0 ? ("Guild type may be changed again in " + FormatClassicFealtyTimeAgo(remaining) + ".") : "Guild type may be changed now.";

	AddClassicGuildFrame(typeMenu, "Set Guild Type");
	typeMenu.AddHTMLGump(55, 95, 420, 40, true, true, ClassicGuildText("Choose the public guild type shown by the guild system."));
	typeMenu.AddHTMLGump(55, 130, 420, 25, false, false, ClassicGuildText(statusText));
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

function ClassicGuildDisbandConfirm(pUser)
{
	if (!pUser || !pUser.socket || !pUser.guild)
		return;

	var guildinfo = pUser.guild;
	var guildName = guildinfo.name || "thy guild";
	var disband = new Gump;

	AddClassicGuildFrame(disband, "Disband Guild");
	disband.AddHTMLGump(55, 95, 420, 90, true, true, ClassicGuildText("Art thou certain thou wishest to disband " + guildName + "? This will remove all members and destroy the guild."));
	AddClassicGuildButton(disband, 90, 220, 30034, "Yes, disband");
	AddClassicGuildButton(disband, 300, 220, 30009, "No, return");

	disband.Send(pUser.socket);
	disband.Free();
}

function HandleNewGuildButton(pSock, pUser, guildinfo, pButton, gumpData)
{
	if (pButton === 33001)
	{
		NewGuildInfoMenu(pUser, false);
		return true;
	}
	if (pButton === 33002)
	{
		NewGuildRosterMenu(pUser);
		return true;
	}
	if (pButton === 33003)
	{
		NewGuildDiplomacyMenu(pUser);
		return true;
	}
	if (pButton === 33004)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can edit guild information.");
			NewGuildInfoMenu(pUser, false);
			return true;
		}
		NewGuildInfoEdit(pUser);
		return true;
	}
	if (pButton === 33005)
	{
		NewGuildInfoMenu(pUser, true);
		return true;
	}
	if (pButton === 33006)
	{
		if (IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("The guild master must transfer leadership before resigning.");
			NewGuildInfoMenu(pUser, false);
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
	if (pButton === 33007)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can disband the guild.");
			NewGuildInfoMenu(pUser, false, false);
			return true;
		}
		NewGuildInfoMenu(pUser, false, true);
		return true;
	}
	if (pButton === 33008)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can disband the guild.");
			NewGuildInfoMenu(pUser, false, false);
			return true;
		}

		var disbandName = guildinfo.name || "the guild";
		var disbandOk = (typeof DisbandGuild !== "undefined") && DisbandGuild(guildinfo.id | 0);
		if (!disbandOk)
		{
			pSock.SysMessage("Unable to disband the guild.");
			NewGuildInfoMenu(pUser, false, false);
			return true;
		}

		ClearLocalGuildState(pUser);
		pSock.SysMessage("You have disbanded " + disbandName + ".");
		NewGuildCreation(pUser);
		return true;
	}
	if (pButton === 33009)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can edit guild information.");
			NewGuildInfoMenu(pUser, false, false);
			return true;
		}

		var newInfoName = manualTrim(gumpData.getEdit(0) || "");
		var newInfoAbbr = manualTrim(gumpData.getEdit(1) || "");
		var newInfoWeb = manualTrim(gumpData.getEdit(2) || "");
		var newInfoCharter = manualTrim(gumpData.getEdit(3) || "");

		if (!newInfoName.length)
		{
			pSock.SysMessage("Guild name cannot be blank.");
			NewGuildInfoEdit(pUser);
			return true;
		}

		if (!newInfoAbbr.length)
		{
			pSock.SysMessage("Guild abbreviation cannot be blank.");
			NewGuildInfoEdit(pUser);
			return true;
		}

		if (newInfoName.length > 33)
			newInfoName = newInfoName.substring(0, 33);
		if (newInfoAbbr.length > 3)
			newInfoAbbr = newInfoAbbr.substring(0, 3);
		if (newInfoWeb.length > 60)
			newInfoWeb = newInfoWeb.substring(0, 60);
		if (newInfoCharter.length > 168)
			newInfoCharter = newInfoCharter.substring(0, 168);

		if (!IsSameText(guildinfo.name, newInfoName) && FindGuildByNameOrAbbreviation(newInfoName, false, guildinfo))
		{
			pSock.SysMessage("Another guild already uses that name.");
			NewGuildInfoEdit(pUser);
			return true;
		}

		if (!IsSameText(guildinfo.abbreviation, newInfoAbbr) && FindGuildByNameOrAbbreviation(newInfoAbbr, true, guildinfo))
		{
			pSock.SysMessage("Another guild already uses that abbreviation.");
			NewGuildInfoEdit(pUser);
			return true;
		}

		guildinfo.name = newInfoName;
		guildinfo.abbreviation = newInfoAbbr;
		guildinfo.webPage = newInfoWeb;
		guildinfo.charter = newInfoCharter;

		pSock.SysMessage("Guild information updated.");
		NewGuildInfoMenu(pUser, false, false);
		return true;
	}
	if (pButton === 33020 || pButton === 33021)
	{
		var members = (guildinfo.member || guildinfo.members) || [];
		var offset = GetClassicOffset(pUser, "newGuildRosterOffset") + ((pButton === 33020) ? -8 : 8);
		SetClassicOffset(pUser, "newGuildRosterOffset", Math.max(0, Math.min(offset, Math.max(0, members.length - 8))));
		NewGuildRosterMenu(pUser);
		return true;
	}
	if (pButton === 33022)
	{
		if (!CanClassicInvite(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow inviting new recruits.");
			NewGuildRosterMenu(pUser);
			return true;
		}
		pSock.CustomTarget(0, "Select a player to invite.");
		NewGuildRosterMenu(pUser);
		return true;
	}
	if (pButton === 33030 || pButton === 33031)
	{
		var diplomacyList = GetNewGuildDiplomacyList(pUser, guildinfo);
		var guildOffset = GetClassicOffset(pUser, "newGuildDiplomacyOffset") + ((pButton === 33030) ? -8 : 8);
		SetClassicOffset(pUser, "newGuildDiplomacyOffset", Math.max(0, Math.min(guildOffset, Math.max(0, diplomacyList.length - 8))));
		NewGuildDiplomacyMenu(pUser);
		return true;
	}
	if (pButton === 33032)
	{
		SetNewGuildDiplomacyFilter(pUser, gumpData.getEdit(0) || "");
		SetClassicOffset(pUser, "newGuildDiplomacyOffset", 0);
		NewGuildDiplomacyMenu(pUser);
		return true;
	}
	if (pButton === 33033)
	{
		SetNewGuildDiplomacyFilter(pUser, "");
		SetClassicOffset(pUser, "newGuildDiplomacyOffset", 0);
		NewGuildDiplomacyMenu(pUser);
		return true;
	}
	if (pButton === 33034 || pButton === 33035 || pButton === 33036)
	{
		SetNewGuildDiplomacyDisplay(pUser, pButton - 33034);
		SetClassicOffset(pUser, "newGuildDiplomacyOffset", 0);
		NewGuildDiplomacyMenu(pUser);
		return true;
	}
	if (pButton >= 33100 && pButton < 33150)
	{
		var selectedMemberIndex = GetClassicOffset(pUser, "newGuildRosterOffset") + (pButton - 33100);
		NewGuildMemberInfoMenu(pUser, selectedMemberIndex, false);
		return true;
	}
	if (pButton === 33150)
	{
		var voteIndex = GetClassicOffset(pUser, "newGuildMemberIndex");
		var voteMembers = (guildinfo.member || guildinfo.members) || [];
		var voteTarget = voteMembers[voteIndex];
		if (!voteTarget || !pUser.SetGuildFealty)
		{
			pSock.SysMessage("That guild vote cannot be changed.");
			NewGuildRosterMenu(pUser);
			return true;
		}
		if (IsGuildMaster(guildinfo, voteTarget))
			pSock.SysMessage("You cannot vote for the current guild master.");
		else
		{
			pUser.SetGuildFealty(voteTarget.serial);
			pSock.SysMessage("You cast your vote for " + (voteTarget.name || "that guild member") + " for guild leader.");
		}
		NewGuildMemberInfoMenu(pUser, voteIndex, false);
		return true;
	}
	if (pButton === 33180)
	{
		var promoteIndex = GetClassicOffset(pUser, "newGuildMemberIndex");
		var promoteMembers = (guildinfo.member || guildinfo.members) || [];
		var promoteTarget = promoteMembers[promoteIndex];
		if (!promoteTarget)
		{
			pSock.SysMessage("That guild member is no longer available.");
			NewGuildRosterMenu(pUser);
			return true;
		}

		if (!CanPromoteDemote(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow promoting guild members.");
			NewGuildMemberInfoMenu(pUser, promoteIndex, false);
			return true;
		}

		var promoteRank = GetAdjacentRankName(guildinfo, promoteTarget, true);
		if (!promoteRank.length)
		{
			pSock.SysMessage("That guild member cannot be promoted further.");
			NewGuildMemberInfoMenu(pUser, promoteIndex, false);
			return true;
		}

		if (!CanChangeTargetRank(guildinfo, pUser, promoteTarget, promoteRank))
		{
			pSock.SysMessage("Your rank does not allow promoting that guild member.");
			NewGuildMemberInfoMenu(pUser, promoteIndex, false);
			return true;
		}

		if (!guildinfo.SetRank || !guildinfo.SetRank(promoteTarget, promoteRank))
		{
			pSock.SysMessage("Unable to promote that guild member.");
			NewGuildMemberInfoMenu(pUser, promoteIndex, false);
			return true;
		}

		promoteTarget.guildTitle = promoteRank;
		if (promoteTarget.Refresh)
			promoteTarget.Refresh();
		pSock.SysMessage((promoteTarget.name || "That guild member") + " has been promoted to " + promoteRank + ".");
		NewGuildMemberInfoMenu(pUser, promoteIndex, false);
		return true;
	}
	if (pButton === 33210)
	{
		var titleIndex = GetClassicOffset(pUser, "newGuildMemberIndex");
		var titleMembers = (guildinfo.member || guildinfo.members) || [];
		var titleTarget = titleMembers[titleIndex];
		if (!titleTarget)
		{
			pSock.SysMessage("That guild member is no longer available.");
			NewGuildRosterMenu(pUser);
			return true;
		}
		if (!CanClassicManageRoster(guildinfo, pUser) && titleTarget.serial !== pUser.serial)
		{
			pSock.SysMessage("Your rank does not allow setting that guild title.");
			NewGuildMemberInfoMenu(pUser, titleIndex, false);
			return true;
		}
		NewGuildMemberTitleEdit(pUser, titleIndex);
		return true;
	}
	if (pButton === 33211)
	{
		var newTitleIndex = GetClassicOffset(pUser, "newGuildMemberIndex");
		var newTitleMembers = (guildinfo.member || guildinfo.members) || [];
		var newTitleTarget = newTitleMembers[newTitleIndex];
		if (!newTitleTarget)
		{
			pSock.SysMessage("That guild member is no longer available.");
			NewGuildRosterMenu(pUser);
			return true;
		}
		if (!CanClassicManageRoster(guildinfo, pUser) && newTitleTarget.serial !== pUser.serial)
		{
			pSock.SysMessage("Your rank does not allow setting that guild title.");
			NewGuildMemberInfoMenu(pUser, newTitleIndex, false);
			return true;
		}

		var newMemberTitle = manualTrim(gumpData.getEdit(0) || "");
		if (newMemberTitle.length > 32)
			newMemberTitle = newMemberTitle.substring(0, 32);

		newTitleTarget.guildTitle = newMemberTitle;
		if (newTitleTarget.Refresh)
			newTitleTarget.Refresh();

		pSock.SysMessage("Updated guild title for " + (newTitleTarget.name || "member") + ".");
		NewGuildMemberInfoMenu(pUser, newTitleIndex, false);
		return true;
	}
	if (pButton === 33212)
	{
		NewGuildMemberInfoMenu(pUser, GetClassicOffset(pUser, "newGuildMemberIndex"), false);
		return true;
	}
	if (pButton === 33240)
	{
		var demoteIndex = GetClassicOffset(pUser, "newGuildMemberIndex");
		var demoteMembers = (guildinfo.member || guildinfo.members) || [];
		var demoteTarget = demoteMembers[demoteIndex];
		if (!demoteTarget)
		{
			pSock.SysMessage("That guild member is no longer available.");
			NewGuildRosterMenu(pUser);
			return true;
		}

		if (!CanPromoteDemote(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow demoting guild members.");
			NewGuildMemberInfoMenu(pUser, demoteIndex, false);
			return true;
		}

		if (IsGuildMaster(guildinfo, demoteTarget))
		{
			pSock.SysMessage("The guild master cannot be demoted this way.");
			NewGuildMemberInfoMenu(pUser, demoteIndex, false);
			return true;
		}

		var demoteRank = GetAdjacentRankName(guildinfo, demoteTarget, false);
		if (!demoteRank.length)
		{
			pSock.SysMessage("That guild member cannot be demoted further.");
			NewGuildMemberInfoMenu(pUser, demoteIndex, false);
			return true;
		}

		if (!CanChangeTargetRank(guildinfo, pUser, demoteTarget, demoteRank))
		{
			pSock.SysMessage("Your rank does not allow demoting that guild member.");
			NewGuildMemberInfoMenu(pUser, demoteIndex, false);
			return true;
		}

		if (!guildinfo.SetRank || !guildinfo.SetRank(demoteTarget, demoteRank))
		{
			pSock.SysMessage("Unable to demote that guild member.");
			NewGuildMemberInfoMenu(pUser, demoteIndex, false);
			return true;
		}

		demoteTarget.guildTitle = demoteRank;
		if (demoteTarget.Refresh)
			demoteTarget.Refresh();
		pSock.SysMessage((demoteTarget.name || "That guild member") + " has been demoted to " + demoteRank + ".");
		NewGuildMemberInfoMenu(pUser, demoteIndex, false);
		return true;
	}
	if (pButton === 33270)
	{
		NewGuildMemberInfoMenu(pUser, GetClassicOffset(pUser, "newGuildMemberIndex"), true);
		return true;
	}
	if (pButton === 33300)
	{
		var kickIndex = GetClassicOffset(pUser, "newGuildMemberIndex");
		var kickMembers = (guildinfo.member || guildinfo.members) || [];
		var kickTarget = kickMembers[kickIndex];
		if (!kickTarget || !CanClassicManageRoster(guildinfo, pUser) || IsGuildMaster(guildinfo, kickTarget))
		{
			pSock.SysMessage("Your rank does not allow removing that guild member.");
			NewGuildRosterMenu(pUser);
			return true;
		}
		if (guildinfo.RemoveMember)
			guildinfo.RemoveMember(kickTarget);
		else if (guildinfo.RemoveMemberBySerial)
			guildinfo.RemoveMemberBySerial(kickTarget.serial);
		kickTarget.guild = null;
		kickTarget.guildTitle = "";
		if (kickTarget.SetGuildFealty)
			kickTarget.SetGuildFealty(0);
		if (kickTarget.Refresh)
			kickTarget.Refresh();
		pSock.SysMessage("The member has been removed from your guild.");
		NewGuildRosterMenu(pUser);
		return true;
	}
	if (pButton >= 33350 && pButton < 33400)
	{
		var selectedGuildIndex = GetClassicOffset(pUser, "newGuildDiplomacyOffset") + (pButton - 33350);
		var selectedGuilds = GetNewGuildDiplomacyList(pUser, guildinfo);
		if (!selectedGuilds || selectedGuildIndex < 0 || selectedGuildIndex >= selectedGuilds.length)
		{
			pSock.SysMessage("That guild is no longer available.");
			NewGuildDiplomacyMenu(pUser);
			return true;
		}

		NewOtherGuildInfoMenu(pUser, selectedGuilds[selectedGuildIndex].id);
		return true;
	}
	if (pButton === 33401 || pButton === 33402 || pButton === 33403)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			NewGuildDiplomacyMenu(pUser);
			return true;
		}
		var otherGuild = GetNewGuildTarget(pUser);
		if (!otherGuild || otherGuild.id === guildinfo.id)
		{
			pSock.SysMessage("That guild is no longer available.");
			NewGuildDiplomacyMenu(pUser);
			return true;
		}
		var relation = (pButton === 33401) ? 1 : ((pButton === 33403) ? 2 : 0);
		var ok = SendAndRecordRelationRequest(guildinfo, otherGuild, relation);
		if (ok)
			pSock.SysMessage((relation === 1 ? "War proposal" : (relation === 2 ? "Alliance request" : "Peace offer")) + " sent to " + (otherGuild.name || "that guild") + ".");
		else
			pSock.SysMessage("Failed to send that guild proposal.");
		NewOtherGuildInfoMenu(pUser, otherGuild.id);
		return true;
	}
	return false;
}

function HandleClassicGuildButton(pSock, pUser, guildinfo, pButton, gumpData)
{
	if (!RequireClassicGuildstoneRange(pUser, guildinfo))
		return true;

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
		ClassicGuildWarAdmin(pUser);
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
		if (ValidateObject(guildinfo.stone) && guildinfo.stone.isItem)
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

		if (!newName.length)
		{
			pSock.SysMessage("Guild name cannot be blank.");
			ClassicGuildInfoEdit(pUser);
			return true;
		}

		if (!newAbbr.length)
		{
			pSock.SysMessage("Guild abbreviation cannot be blank.");
			ClassicGuildInfoEdit(pUser);
			return true;
		}

		if (newName.length > 33)
			newName = newName.substring(0, 33);
		if (newAbbr.length > 3)
			newAbbr = newAbbr.substring(0, 3);
		if (newWeb.length > 60)
			newWeb = newWeb.substring(0, 60);
		if (newCharter.length > 168)
			newCharter = newCharter.substring(0, 168);

		if (!IsSameText(guildinfo.name, newName) && FindGuildByNameOrAbbreviation(newName, false, guildinfo))
		{
			pSock.SysMessage("Another guild already uses that name.");
			ClassicGuildInfoEdit(pUser);
			return true;
		}

		if (!IsSameText(guildinfo.abbreviation, newAbbr) && FindGuildByNameOrAbbreviation(newAbbr, true, guildinfo))
		{
			pSock.SysMessage("Another guild already uses that abbreviation.");
			ClassicGuildInfoEdit(pUser);
			return true;
		}

		guildinfo.name = newName;
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

		var newGuildType = (pButton === 30018) ? 0 : ((pButton === 30019) ? 1 : 2);
		var typeName = (newGuildType === 0) ? "Standard" : ((newGuildType === 1) ? "Order" : "Chaos");
		if ((guildinfo.type | 0) === newGuildType)
		{
			pSock.SysMessage("Guild type is already " + typeName + ".");
			ClassicGuildTypeMenu(pUser);
			return true;
		}

		var remaining = GetClassicTypeChangeRemaining(guildinfo);
		if (remaining > 0)
		{
			pSock.SysMessage("Guild type cannot be changed again for " + FormatClassicFealtyTimeAgo(remaining) + ".");
			ClassicGuildTypeMenu(pUser);
			return true;
		}

		guildinfo.type = newGuildType;
		SetClassicLastTypeChange(guildinfo, GetClassicWarNow());
		pSock.SysMessage("Guild type set to " + typeName + ".");
		NotifyGuildMembers(guildinfo, "Your guild type has changed to " + typeName + ".");

		ClassicGuildTypeMenu(pUser);
		return true;
	}

	if (pButton === 30021)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can check fealty votes.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		var fealtyResult = EvaluateClassicFealty(guildinfo, true);
		pSock.SysMessage(fealtyResult.message);
		ClassicGuildMasterMenu(pUser);
		return true;
	}

	if (pButton === 30022)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can transfer guild leadership.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		pSock.CustomTarget(2, "Select the guild member who shall become guild master.");
		ClassicGuildMasterMenu(pUser);
		return true;
	}

	if (pButton === 30033)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can disband the guild.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		ClassicGuildDisbandConfirm(pUser);
		return true;
	}

	if (pButton === 30034)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can disband the guild.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		var classicDisbandName = guildinfo.name || "the guild";
		var classicDisbandOk = (typeof DisbandGuild !== "undefined") && DisbandGuild(guildinfo.id | 0);
		if (!classicDisbandOk)
		{
			pSock.SysMessage("Unable to disband the guild.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		ClearLocalGuildState(pUser);
		pSock.SysMessage("You have disbanded " + classicDisbandName + ".");
		GuildCreation(pUser);
		return true;
	}

	if (pButton === 30023)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can set the guildmaster title.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		ClassicGuildMasterTitleEdit(pUser);
		return true;
	}

	if (pButton === 30024)
	{
		if (!IsGuildMaster(guildinfo, pUser))
		{
			pSock.SysMessage("Only the guild master can set the guildmaster title.");
			ClassicGuildMasterMenu(pUser);
			return true;
		}

		var leader = guildinfo.master || pUser;
		if (!leader || !IsSameGuild(guildinfo, leader.guild))
			leader = pUser;

		var masterTitle = manualTrim(gumpData.getEdit(0) || "");
		if (masterTitle.length > 20)
			masterTitle = masterTitle.substring(0, 20);

		leader.guildTitle = masterTitle;
		if (leader.Refresh)
			leader.Refresh();

		pSock.SysMessage("Guildmaster title updated.");
		ClassicGuildMasterMenu(pUser);
		return true;
	}

	if (pButton === 30025)
	{
		ClassicGuildDiplomacy(pUser);
		return true;
	}

	if (pButton === 30026)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		ClassicIncomingWarInvites(pUser);
		return true;
	}

	if (pButton === 30027)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		ClassicOutgoingWarDeclarations(pUser);
		return true;
	}

	if (pButton === 30028)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		ClassicDeclarePeaceMenu(pUser);
		return true;
	}

	if (pButton === 30029)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		ClassicDeclareWarMenu(pUser);
		return true;
	}

	if (pButton === 30030)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		ClassicActiveWarsMenu(pUser);
		return true;
	}

	if (pButton === 30031)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		var warSearch = manualTrim(gumpData.getEdit(0) || "");
		if (warSearch.length > 0 && warSearch.length < 3)
		{
			pSock.SysMessage("Enter at least 3 characters to search for a guild.");
			ClassicDeclareWarMenu(pUser);
			return true;
		}

		SetWarSearchFilter(pUser, warSearch);
		ClassicDeclareWarMenu(pUser);
		return true;
	}

	if (pButton === 30032)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		SetWarSearchFilter(pUser, "");
		ClassicDeclareWarMenu(pUser);
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

	if (pButton >= 31800 && pButton < 32000)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		var acceptWarInvite = pButton >= 31800 && pButton < 31900;
		var warReqIndex = acceptWarInvite ? (pButton - 31800) : (pButton - 31900);
		var incomingReqs = ReadGuildRelationRequests(guildinfo);
		if (!incomingReqs || warReqIndex < 0 || warReqIndex >= incomingReqs.length || (incomingReqs[warReqIndex].relation | 0) !== 1)
		{
			pSock.SysMessage("That war invitation is no longer available.");
			ClassicIncomingWarInvites(pUser);
			return true;
		}

		var warReq = incomingReqs[warReqIndex];
		var enemyGuild = FindGuildById(warReq.fromId);
		if (!enemyGuild)
		{
			pSock.SysMessage("The declaring guild no longer exists.");
			RemoveGuildRelationRequestByIndex(guildinfo, warReqIndex);
			ClassicIncomingWarInvites(pUser);
			return true;
		}

		if (acceptWarInvite)
		{
			var warOk = SetGuildRelation(guildinfo.id, enemyGuild.id, 1);
			if (warOk)
			{
				StartClassicWar(guildinfo, enemyGuild, classicWarDefaultMaxKills, classicWarDefaultDurationHours);
				RemoveGuildRelationRequestByIndex(guildinfo, warReqIndex);
				pSock.SysMessage("You accept the war declaration from " + (enemyGuild.name || "that guild") + ".");
			}
			else
			{
				pSock.SysMessage("Failed to accept that war declaration.");
			}
		}
		else
		{
			RemoveGuildRelationRequestByIndex(guildinfo, warReqIndex);
			pSock.SysMessage("You reject the war declaration from " + (enemyGuild.name || "that guild") + ".");
		}

		ClassicIncomingWarInvites(pUser);
		return true;
	}

	if (pButton >= 32000 && pButton < 32100)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		var outgoingIndex = pButton - 32000;
		var outgoingWars = GetOutgoingRelationRequests(guildinfo, 1);
		if (!outgoingWars || outgoingIndex < 0 || outgoingIndex >= outgoingWars.length)
		{
			pSock.SysMessage("That war declaration is no longer available.");
			ClassicOutgoingWarDeclarations(pUser);
			return true;
		}

		var outgoing = outgoingWars[outgoingIndex];
		if (outgoing.guild)
			pSock.SysMessage("You rescind your war declaration to " + (outgoing.guild.name || "that guild") + ".");
		else
			pSock.SysMessage("You rescind that war declaration.");

		if (outgoing.guild)
			RemoveGuildRelationRequestByIndex(outgoing.guild, outgoing.index);
		ClassicOutgoingWarDeclarations(pUser);
		return true;
	}

	if (pButton >= 32100 && pButton < 32200)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		var peaceIndex = pButton - 32100;
		var enemies = GetClassicWarEnemies(guildinfo);
		if (!enemies || peaceIndex < 0 || peaceIndex >= enemies.length)
		{
			pSock.SysMessage("That enemy guild is no longer available.");
			ClassicDeclarePeaceMenu(pUser);
			return true;
		}

		var enemyGuild = enemies[peaceIndex].guild;
		if (!enemyGuild)
		{
			pSock.SysMessage("That enemy guild no longer exists.");
			ClassicDeclarePeaceMenu(pUser);
			return true;
		}

		var okPeace = SendAndRecordRelationRequest(guildinfo, enemyGuild, 0);
		if (okPeace)
			pSock.SysMessage("Peace offer sent to " + (enemyGuild.name || "that guild") + ".");
		else
			pSock.SysMessage("Failed to send peace offer.");

		ClassicDeclarePeaceMenu(pUser);
		return true;
	}

	if (pButton >= 32200 && pButton < 32300)
	{
		if (!CanClassicControlWar(guildinfo, pUser))
		{
			pSock.SysMessage("Your rank does not allow changing guild diplomacy.");
			ClassicGuildWarAdmin(pUser);
			return true;
		}

		var declareIndex = pButton - 32200;
		var targets = GetClassicWarDeclarationTargets(guildinfo, GetWarSearchFilter(pUser));
		if (!targets || declareIndex < 0 || declareIndex >= targets.length)
		{
			pSock.SysMessage("That guild is no longer available.");
			ClassicDeclareWarMenu(pUser);
			return true;
		}

		var targetGuild = targets[declareIndex].guild;
		if (!targetGuild)
		{
			pSock.SysMessage("That guild no longer exists.");
			ClassicDeclareWarMenu(pUser);
			return true;
		}

		if (HasOutgoingRelationRequest(guildinfo, targetGuild, 1))
		{
			pSock.SysMessage("A war declaration is already pending with that guild.");
			ClassicDeclareWarMenu(pUser);
			return true;
		}

		var okWar = SendAndRecordRelationRequest(guildinfo, targetGuild, 1);
		if (okWar)
			pSock.SysMessage("War declaration sent to " + (targetGuild.name || "that guild") + ".");
		else
			pSock.SysMessage("Failed to send war declaration.");

		ClassicDeclareWarMenu(pUser);
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

	if (pButton === newGuildInviteToggleButton)
	{
		if (GetConfiguredGuildMenuSystem() !== guildMenuSystemNewOSI)
			return;

		var ignoreInvites = !IsIgnoringNewGuildInvites(pUser);
		SetIgnoringNewGuildInvites(pUser, ignoreInvites);
		pSock.SysMessage(ignoreInvites ? "You are now ignoring guild invitations." : "You are now accepting guild invitations.");

		if (!pUser.guild)
			NewGuildCreation(pUser);
		else
			NewGuildMenu(pUser);
		return;
	}

	if (pButton === newGuildCreateButton)
	{
		if (GetConfiguredGuildMenuSystem() !== guildMenuSystemNewOSI)
			return;

		var newGuildName = manualTrim(gumpData.getEdit(0) || "");
		var newGuildAbbr = manualTrim(gumpData.getEdit(1) || "");
		if (!newGuildName.length)
		{
			pSock.SysMessage("Enter a guild name first.");
			NewGuildCreation(pUser);
			return;
		}

		if (!newGuildAbbr.length)
		{
			pSock.SysMessage("Enter a guild abbreviation first.");
			NewGuildCreation(pUser);
			return;
		}

		if (pUser.guild != null)
		{
			pSock.SysMessage("You are already in a guild.");
			NewGuildMenu(pUser);
			return;
		}

		var newOSIGuild = CreateNewGuild(pUser, newGuildName, newGuildAbbr);
		if (!newOSIGuild)
		{
			pSock.SysMessage("Could not create guild.");
			NewGuildCreation(pUser);
			return;
		}

		if (!TryPayNewGuildRegistrationFee(pSock, pUser))
		{
			if (typeof DisbandGuild !== "undefined")
				DisbandGuild(newOSIGuild.id | 0);
			ClearLocalGuildState(pUser);
			NewGuildCreation(pUser);
			return;
		}

		pSock.SysMessage("Guild created: " + (newOSIGuild.name || newGuildName) + ".");
		if (pUser.Refresh)
			pUser.Refresh();
		NewGuildMenu(pUser);
		return;
	}

	if (HandleNewGuildInvitationResponse(pSock, pUser, pButton))
		return;

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
			if (!UseNewOSIGuildMenu() && TriggerEvent(5022, "CanPlacePendingGuildstone", pUser) === false)
			{
				pSock.SysMessage("Unable to place the guildstone deed.");
				return;
			}

			pUser.TextMessage("Not currently in a guild... Creating new guild...", false, 0x3b2, 0, pUser.serial);
			var newGuild = CreateNewGuild(pUser, Text1, Text2);
			if (newGuild)
			{
				if (!TryPayNewGuildRegistrationFee(pSock, pUser))
				{
					if (typeof DisbandGuild !== "undefined")
						DisbandGuild(newGuild.id | 0);
					ClearLocalGuildState(pUser);
					return;
				}

				if (UseClassicOSIGuildMenu())
					EnsureClassicGuildRanks(newGuild, pUser);

				if (!UseNewOSIGuildMenu())
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

	if ((pButton >= 30001 && pButton <= 30099) || (pButton >= 30100 && pButton < 32300))
	{
		if (HandleClassicGuildButton(pSock, pUser, guildinfo, pButton, gumpData))
			return;
	}

	if (pButton >= 33001 && pButton < 33500)
	{
		if (HandleNewGuildButton(pSock, pUser, guildinfo, pButton, gumpData))
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

	if (RequireGuildstoneForConfiguredGuildMenu(guild) && !RequireClassicGuildstoneRange(pUser, guild))
		return;

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

	if (UseNewOSIGuildMenu())
	{
		if (!target.socket)
		{
			socket.SysMessage((target.name || "That player") + " must be online to receive a guild invitation.");
			return;
		}

		if (IsIgnoringNewGuildInvites(target))
		{
			socket.SysMessage((target.name || "That player") + " is not accepting guild invitations.");
			return;
		}

		if (target.GetTag && parseInt(target.GetTag("newGuildInviteGuildId"), 10) > 0)
		{
			socket.SysMessage((target.name || "That player") + " is already considering a guild invitation.");
			return;
		}

		if (NewGuildInvitationRequest(target, guild, pUser))
		{
			socket.SysMessage("Invitation sent to " + (target.name || "player") + ".");
			target.socket.SysMessage("You have received a guild invitation from " + (pUser.name || "a guild member") + ".");
		}
		else
		{
			socket.SysMessage("Could not send the guild invitation.");
		}
		return;
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

	if (RequireGuildstoneForConfiguredGuildMenu(guild) && !RequireClassicGuildstoneRange(pUser, guild))
		return;

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

	ShowConfiguredGuildMenu(pUser);
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback2(socket, target)
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

	if (RequireGuildstoneForConfiguredGuildMenu(guild) && !RequireClassicGuildstoneRange(pUser, guild))
		return;

	if (!IsGuildMaster(guild, pUser))
	{
		socket.SysMessage("Only the guild master can transfer guild leadership.");
		ShowConfiguredGuildMenu(pUser);
		return;
	}

	if (!target || !target.isChar)
	{
		socket.SysMessage("That is not a valid character.");
		ShowConfiguredGuildMenu(pUser);
		return;
	}

	if (!target.guild || !IsSameGuild(guild, target.guild))
	{
		socket.SysMessage("You may only transfer leadership to a member of your own guild.");
		ShowConfiguredGuildMenu(pUser);
		return;
	}

	if (target.serial === pUser.serial)
	{
		socket.SysMessage("You already lead this guild.");
		ShowConfiguredGuildMenu(pUser);
		return;
	}

	if (!SetClassicGuildMaster(guild, target))
	{
		socket.SysMessage("Unable to transfer guild leadership.");
		ShowConfiguredGuildMenu(pUser);
		return;
	}

	var targetName = target.name || ("0x" + target.serial.toString(16).toUpperCase());
	NotifyGuildMembers(guild, targetName + " is now guild master.");
	ShowConfiguredGuildMenu(pUser);
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

function GetAdjacentRankName(guild, pChar, promote)
{
	if (!guild || !pChar || !guild.NumRanks || !guild.GetRankNameById || !guild.GetRankPrioById)
		return "";

	var currentName = GetRankName(guild, pChar) || pChar.guildTitle || "";
	var currentPrio = GetRankPriority(guild, pChar);
	var bestName = "";
	var bestPrio = promote ? 2147480000 : -1;
	var n = guild.NumRanks();

	for (var id = 0; id < n; id++)
	{
		var rankName = guild.GetRankNameById(id) || "";
		var rankPrio = guild.GetRankPrioById(id);
		if (!rankName || rankName === "(deleted)" || rankPrio < 0 || rankPrio >= 2147480000)
			continue;

		if (rankName === currentName)
			continue;

		if (promote)
		{
			if (rankPrio > currentPrio && rankPrio < bestPrio)
			{
				bestPrio = rankPrio;
				bestName = rankName;
			}
		}
		else if (rankPrio < currentPrio && rankPrio > bestPrio)
		{
			bestPrio = rankPrio;
			bestName = rankName;
		}
	}

	return bestName;
}

function GetRankPriorityByName(guild, rankName)
{
	if (!guild || !rankName || !guild.NumRanks || !guild.GetRankNameById || !guild.GetRankPrioById)
		return 0;

	var n = guild.NumRanks();
	for (var id = 0; id < n; id++)
	{
		var name = guild.GetRankNameById(id) || "";
		var prio = guild.GetRankPrioById(id);
		if (name === rankName && prio >= 0 && prio < 2147480000)
			return prio;
	}

	return 0;
}

function CanChangeTargetRank(guild, actor, target, newRankName)
{
	if (!guild || !actor || !target || !newRankName)
		return false;

	if (IsGuildMaster(guild, actor))
		return true;

	var actorPrio = GetRankPriority(guild, actor);
	var targetPrio = GetRankPriority(guild, target);
	var newPrio = GetRankPriorityByName(guild, newRankName);
	return actorPrio > targetPrio && actorPrio > newPrio;
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

function IsSameText(leftText, rightText)
{
	return String(leftText || "").toLowerCase() === String(rightText || "").toLowerCase();
}

function FindGuildByNameOrAbbreviation(textValue, checkAbbreviation, ignoreGuild)
{
	var needle = manualTrim(textValue);
	if (!needle.length)
		return null;

	var allGuilds = (typeof GetAllGuilds !== "undefined") ? GetAllGuilds() : [];
	for (var i = 0; i < allGuilds.length; i++)
	{
		var guild = allGuilds[i];
		if (!guild)
			continue;

		if (ignoreGuild && guild.id === ignoreGuild.id)
			continue;

		var compareValue = checkAbbreviation ? guild.abbreviation : guild.name;
		if (IsSameText(compareValue, needle))
			return guild;
	}

	return null;
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

function ShowConfiguredGuildMenu(pUser)
{
	if (!pUser)
		return;

	var menuSystem = GetConfiguredGuildMenuSystem();
	if (pUser.guild && (menuSystem === guildMenuSystemClassicOSI || (menuSystem === guildMenuSystemCustom && IsClassicGuildMode(pUser.guild))))
		ClassicGuildMenu(pUser);
	else if (pUser.guild && menuSystem === guildMenuSystemNewOSI)
		NewGuildMenu(pUser);
	else
		GuildMenu(pUser);
}

function IsSameGuild(guildOne, guildTwo)
{
	if (!guildOne || !guildTwo)
		return false;

	if (guildOne === guildTwo)
		return true;

	return guildOne.id === guildTwo.id;
}

function IsClassicFealtyVoter(guild, pChar)
{
	if (!guild || !pChar || !IsSameGuild(guild, pChar.guild))
		return false;

	if (HasClassicRank(guild, pChar, "Ronin"))
		return false;

	return true;
}

function GetClassicLastFealtyCheck(guild)
{
	var cfg = ReadGuildConfig(guild);
	var lastCheck = parseInt(cfg.CLASSIC_LAST_FEALTY_CHECK, 10);
	if (isNaN(lastCheck) || lastCheck < 0)
		return 0;

	return lastCheck;
}

function SetClassicLastFealtyCheck(guild, timeStamp)
{
	var cfg = ReadGuildConfig(guild);
	cfg.CLASSIC_LAST_FEALTY_CHECK = String(timeStamp | 0);
	return SaveGuildConfig(guild, cfg);
}

function FormatClassicFealtyTimeAgo(seconds)
{
	seconds = Math.max(0, seconds | 0);
	if (seconds < 60)
		return seconds + "s";
	if (seconds < 3600)
		return Math.floor(seconds / 60) + "m";
	if (seconds < 86400)
		return Math.floor(seconds / 3600) + "h";

	return Math.floor(seconds / 86400) + "d";
}

function GetClassicLastTypeChange(guild)
{
	var cfg = ReadGuildConfig(guild);
	var lastChange = parseInt(cfg.CLASSIC_TYPE_LAST_CHANGE, 10);
	if (isNaN(lastChange) || lastChange < 0)
		return 0;

	return lastChange;
}

function SetClassicLastTypeChange(guild, timeStamp)
{
	var cfg = ReadGuildConfig(guild);
	cfg.CLASSIC_TYPE_LAST_CHANGE = String(timeStamp | 0);
	return SaveGuildConfig(guild, cfg);
}

function GetClassicTypeChangeRemaining(guild)
{
	var lastChange = GetClassicLastTypeChange(guild);
	if (!lastChange)
		return 0;

	var remaining = classicGuildTypeChangeIntervalSeconds - (GetClassicWarNow() - lastChange);
	return Math.max(0, remaining | 0);
}

function SetClassicGuildMaster(guild, newMaster)
{
	if (!guild || !newMaster || !IsSameGuild(guild, newMaster.guild))
		return false;

	var oldMaster = guild.master;
	if (oldMaster && oldMaster.serial === newMaster.serial)
		return true;

	EnsureClassicGuildRanks(guild, newMaster);
	guild.master = newMaster;

	if (guild.SetRank)
	{
		if (oldMaster && IsSameGuild(guild, oldMaster.guild))
		{
			guild.SetRank(oldMaster, "Member");
			oldMaster.guildTitle = "Member";
		}

		guild.SetRank(newMaster, "Guild Master");
	}

	newMaster.guildTitle = "Guild Master";
	if (oldMaster && oldMaster.Refresh)
		oldMaster.Refresh();
	if (newMaster.Refresh)
		newMaster.Refresh();

	return true;
}

function EvaluateClassicFealty(guild, forceCheck)
{
	var result = {
		changed: false,
		checked: false,
		message: "Fealty votes were not checked."
	};

	if (!guild || !IsClassicGuildMode(guild))
	{
		result.message = "Classic rank mode is not enabled for this guild.";
		return result;
	}

	var now = GetClassicWarNow();
	var lastCheck = GetClassicLastFealtyCheck(guild);
	if (!forceCheck && lastCheck && (now - lastCheck) < classicFealtyCheckIntervalSeconds)
	{
		result.message = "Fealty votes were checked recently.";
		return result;
	}

	SetClassicLastFealtyCheck(guild, now);
	result.checked = true;

	var members = (guild.member || guild.members) || [];
	var voters = [];
	var candidates = {};
	var candidateNames = {};

	for (var i = 0; i < members.length; i++)
	{
		var voter = members[i];
		if (!IsClassicFealtyVoter(guild, voter))
			continue;

		voters.push(voter);
		var target = GetGuildFealtyTarget(guild, voter);
		if (!target || !IsSameGuild(guild, target.guild))
			continue;

		var serialKey = String(target.serial);
		candidates[serialKey] = (candidates[serialKey] || 0) + 1;
		candidateNames[serialKey] = target.name || ("0x" + target.serial.toString(16).toUpperCase());
	}

	if (!voters.length)
	{
		result.message = "No eligible classic guild voters were found.";
		return result;
	}

	var bestSerial = "";
	var bestVotes = 0;
	var tied = false;
	for (var key in candidates)
	{
		if (!candidates.hasOwnProperty(key))
			continue;

		if (candidates[key] > bestVotes)
		{
			bestVotes = candidates[key];
			bestSerial = key;
			tied = false;
		}
		else if (candidates[key] === bestVotes)
		{
			tied = true;
		}
	}

	var neededVotes = Math.floor(voters.length / 2) + 1;
	if (!bestSerial || bestVotes < neededVotes || tied)
	{
		result.message = "No guild member has majority fealty (" + bestVotes + "/" + voters.length + ", needs " + neededVotes + ").";
		return result;
	}

	var newMaster = null;
	for (var m = 0; m < members.length; m++)
	{
		if (members[m] && String(members[m].serial) === bestSerial)
		{
			newMaster = members[m];
			break;
		}
	}

	if (!newMaster)
	{
		result.message = "The leading fealty target is no longer in the guild.";
		return result;
	}

	if (IsGuildMaster(guild, newMaster))
	{
		result.message = (newMaster.name || "The guild master") + " already holds majority fealty (" + bestVotes + "/" + voters.length + ").";
		return result;
	}

	if (!SetClassicGuildMaster(guild, newMaster))
	{
		result.message = "Unable to transfer guildmaster by fealty vote.";
		return result;
	}

	result.changed = true;
	result.message = (newMaster.name || candidateNames[bestSerial] || "A guild member") + " is now guild master by majority fealty (" + bestVotes + "/" + voters.length + ").";
	NotifyGuildMembers(guild, result.message);
	return result;
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

	if (GetConfiguredGuildMenuSystem() === guildMenuSystemCustom && !IsClassicGuildMode(killer.guild) && !IsClassicGuildMode(pDead.guild))
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

function GetGuildIndexById(guildId)
{
	var all = GetAllGuilds();
	if (!all || !all.length)
		return -1;

	for (var i = 0; i < all.length; i++)
	{
		if (all[i] && all[i].id === (guildId | 0))
			return i;
	}
	return -1;
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

function GetIncomingRelationRequests(guild, relationFilter)
{
	var raw = ReadGuildRelationRequests(guild);
	var list = [];

	for (var i = 0; i < raw.length; i++)
	{
		var req = raw[i];
		if (!req)
			continue;

		if (typeof relationFilter === "number" && (req.relation | 0) !== relationFilter)
			continue;

		list.push({
			index: i,
			fromId: req.fromId | 0,
			relation: req.relation | 0,
			guild: FindGuildById(req.fromId)
		});
	}

	return list;
}

function GetOutgoingRelationRequests(guild, relationFilter)
{
	var list = [];
	var allGuilds = (typeof GetAllGuilds !== "undefined") ? GetAllGuilds() : [];
	if (!guild || !allGuilds)
		return list;

	for (var i = 0; i < allGuilds.length; i++)
	{
		var targetGuild = allGuilds[i];
		if (!targetGuild || targetGuild.id === guild.id)
			continue;

		var targetRequests = ReadGuildRelationRequests(targetGuild);
		for (var r = 0; r < targetRequests.length; r++)
		{
			var req = targetRequests[r];
			if (!req || (req.fromId | 0) !== (guild.id | 0))
				continue;

			if (typeof relationFilter === "number" && (req.relation | 0) !== relationFilter)
				continue;

			list.push({
				index: r,
				targetId: targetGuild.id,
				relation: req.relation | 0,
				guild: targetGuild
			});
		}
	}

	return list;
}

function HasOutgoingRelationRequest(guild, targetGuild, relationFilter)
{
	if (!guild || !targetGuild)
		return false;

	var outgoing = GetOutgoingRelationRequests(guild, relationFilter);
	for (var i = 0; i < outgoing.length; i++)
	{
		if (outgoing[i] && outgoing[i].guild && outgoing[i].guild.id === targetGuild.id)
			return true;
	}

	return false;
}

function GetClassicWarDeclarationTargets(guild, filterText)
{
	var list = [];
	var allGuilds = (typeof GetAllGuilds !== "undefined") ? GetAllGuilds() : [];
	if (!guild || !allGuilds)
		return list;

	var filter = manualTrim(filterText || "").toLowerCase();
	for (var i = 0; i < allGuilds.length; i++)
	{
		var targetGuild = allGuilds[i];
		if (!targetGuild || targetGuild.id === guild.id)
			continue;

		if (filter.length)
		{
			var targetName = String(targetGuild.name || "").toLowerCase();
			var targetAbbr = String(targetGuild.abbreviation || "").toLowerCase();
			if (targetName.indexOf(filter) === -1 && targetAbbr.indexOf(filter) === -1)
				continue;
		}

		var rel = (typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(guild.id, targetGuild.id) : 3;
		if (rel === 1)
			continue;

		if (HasOutgoingRelationRequest(guild, targetGuild, 1))
			continue;

		list.push({
			index: i,
			guild: targetGuild,
			status: GetGuildRelationText(rel)
		});
	}

	return list;
}

function GetClassicWarEnemies(guild)
{
	var list = [];
	var allGuilds = (typeof GetAllGuilds !== "undefined") ? GetAllGuilds() : [];
	if (!guild || !allGuilds)
		return list;

	for (var i = 0; i < allGuilds.length; i++)
	{
		var otherGuild = allGuilds[i];
		if (!otherGuild || otherGuild.id === guild.id)
			continue;

		var rel = (typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(guild.id, otherGuild.id) : 3;
		if (rel !== 1)
			continue;

		CheckClassicWarExpired(guild, otherGuild);
		rel = (typeof CompareGuildByGuild !== "undefined") ? CompareGuildByGuild(guild.id, otherGuild.id) : rel;
		if (rel !== 1)
			continue;

		list.push({
			index: i,
			guild: otherGuild
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
