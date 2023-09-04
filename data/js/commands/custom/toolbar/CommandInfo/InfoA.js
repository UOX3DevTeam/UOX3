function InfoA(pUser, uox3gump)
{
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddPageButton(270, 150, 2446, 2445, 103);
	uox3gump.AddText(330, 150, 0, "addPack");
	uox3gump.AddPageButton(270, 180, 2446, 2445, 104);
	uox3gump.AddText(330, 180, 0, "addX");
	uox3gump.AddPageButton(270, 210, 2446, 2445, 105);
	uox3gump.AddText(330, 210, 0, "adminYell");
	uox3gump.AddPageButton(270, 240, 2446, 2445, 106);
	uox3gump.AddText(330, 240, 0, "allMove");
	uox3gump.AddPageButton(270, 270, 2446, 2445, 107);
	uox3gump.AddText(330, 270, 0, "announce");
	uox3gump.AddPageButton(270, 300, 2446, 2445, 108);
	uox3gump.AddText(330, 300, 0, "areaCommand");

	uox3gump.AddPage(100);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddBackground(270, 90, 592, 160, 9300);
	uox3gump.AddText(280, 100, 0, "The character of the player using this command performs the specified animation.");
	uox3gump.AddText(280, 120, 0, "See NPCAction list for possible actions!");
	uox3gump.AddText(280, 150, 0, "Command:");
	uox3gump.AddText(340, 150, 32, "'action");
	uox3gump.AddText(280, 170, 0, "Parameters:");
	uox3gump.AddText(350, 170, 32, "(h/d)");
	uox3gump.AddText(390, 170, 0, "(hex or decimal)");
	uox3gump.AddText(280, 210, 0, "Example:");
	uox3gump.AddText(340, 210, 3, "'action 1");
	uox3gump.AddPageButton(270, 260, 2446, 2445, 101);
	uox3gump.AddText(340, 260, 0, "Add");
	uox3gump.AddPageButton(270, 290, 2446, 2445, 102);
	uox3gump.AddText(320, 290, 0, "AddAccount");
	uox3gump.AddPageButton(270, 320, 2446, 2445, 103);
	uox3gump.AddText(330, 320, 0, "addPack");
	uox3gump.AddPageButton(270, 350, 2446, 2445, 104);
	uox3gump.AddText(330, 350, 0, "addX");
	uox3gump.AddPageButton(270, 380, 2446, 2445, 105);
	uox3gump.AddText(330, 380, 0, "adminYell");
	uox3gump.AddPageButton(270, 410, 2446, 2445, 106);
	uox3gump.AddText(330, 410, 0, "allMove");
	uox3gump.AddPageButton(270, 440, 2446, 2445, 107);
	uox3gump.AddText(330, 440, 0, "announce");
	uox3gump.AddPageButton(270, 470, 2446, 2445, 108);
	uox3gump.AddText(330, 470, 0, "areaCommand");

	uox3gump.AddPage(101);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddBackground(270, 120, 592, 315, 9300);
	uox3gump.AddHTMLGump(280, 130, 562, 112, false, false, "Opens GM Add - menu if no parameters provided, or adds raw item if provided with hex - id, while item adds specified item from item - dfns, npc adds specified NPC from npc - dfns, and spawner adds a specified item from the item - dfns, to function as a spawner.Accepts a value after numerical / hexdecimal item IDs to randomize(using provided value) item being added.");
	uox3gump.AddText(280, 250, 0, "Command:");
	uox3gump.AddText(340, 250, 32, "'add");
	uox3gump.AddText(280, 270, 0, "Parameters:");
	uox3gump.AddText(350, 270, 32, " (nothing) / (h) / (h rndVal) / item (s) / item (s rndVal) /npc (s) / spawner (s)");
	uox3gump.AddHTMLGump(280, 310, 598, 129, false, false,"Examples:"
		+ "<BR>'add npc orc"
		+ "<BR>'add 0x04a9 (add base wooden floor tile)"
		+ "<BR>'add 0x04a9 3 (add random base wooden floor tile between 0x04a9 and 0x04ac)"
		+ "<BR>'add item 0x171a (add scripted feathered hat)"
		+ "<BR>'add item 0x04a9 3 (add random scripted wooden floor between 0x04a9 and 0x04ac)");
	uox3gump.AddPageButton(270, 440, 2446, 2445, 102);
	uox3gump.AddText(320, 440, 0, "AddAccount");
	uox3gump.AddPageButton(270, 470, 2446, 2445, 103);
	uox3gump.AddText(330, 470, 0, "addPack");
	uox3gump.AddPageButton(270, 500, 2446, 2445, 104);
	uox3gump.AddText(330, 500, 0, "addX");
	uox3gump.AddPageButton(270, 530, 2446, 2445, 105);
	uox3gump.AddText(330, 530, 0, "adminYell");
	uox3gump.AddPageButton(270, 560, 2446, 2445, 106);
	uox3gump.AddText(330, 560, 0, "allMove");
	uox3gump.AddPageButton(470, 440, 2446, 2445, 107);
	uox3gump.AddText(530, 440, 0, "announce");
	uox3gump.AddPageButton(470, 470, 2446, 2445, 108);
	uox3gump.AddText(530, 470, 0, "areaCommand");

	uox3gump.AddPage(102);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddBackground(270, 150, 592, 98, 9300);
	uox3gump.AddText(290, 160, 0, "Adds an account. First string is username, second is password, hex parameter is privs (if any).");
	uox3gump.AddText(290, 180, 0, "Command:");
	uox3gump.AddText(350, 180, 32, "'addAccount");
	uox3gump.AddText(290, 200, 0, "Parameters:");
	uox3gump.AddText(360, 200, 32, " (s s h)");
	uox3gump.AddText(290, 220, 0, "Example: 'addAccount testuser testpass 0x0");
	uox3gump.AddPageButton(270, 260, 2446, 2445, 103);
	uox3gump.AddText(330, 260, 0, "addPack");
	uox3gump.AddPageButton(270, 290, 2446, 2445, 104);
	uox3gump.AddText(330, 290, 0, "addX");
	uox3gump.AddPageButton(270, 320, 2446, 2445, 105);
	uox3gump.AddText(330, 320, 0, "adminYell");
	uox3gump.AddPageButton(270, 350, 2446, 2445, 106);
	uox3gump.AddText(330, 350, 0, "allMove");
	uox3gump.AddPageButton(270, 380, 2446, 2445, 107);
	uox3gump.AddText(330, 380, 0, "announce");
	uox3gump.AddPageButton(270, 410, 2446, 2445, 108);
	uox3gump.AddText(330, 410, 0, "areaCommand");

	uox3gump.AddPage(103);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddBackground(270, 180, 592, 216, 9300);
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddPageButton(270, 150, 2446, 2445, 103);
	uox3gump.AddPageButton(270, 410, 2446, 2445, 104);
	uox3gump.AddPageButton(270, 440, 2446, 2445, 105);
	uox3gump.AddPageButton(270, 470, 2446, 2445, 106);
	uox3gump.AddPageButton(270, 500, 2446, 2445, 107);
	uox3gump.AddPageButton(270, 530, 2446, 2445, 108);
	uox3gump.AddText(330, 150, 0, "addPack");
	uox3gump.AddText(320, 410, 0, "addX");
	uox3gump.AddText(320, 440, 0, "adminYell");
	uox3gump.AddText(320, 470, 0, "allMove");
	uox3gump.AddText(320, 500, 0, "announce");
	uox3gump.AddText(320, 530, 0, "areaCommand");
	uox3gump.AddBackground(270, 180, 592, 216, 9300);
	uox3gump.AddHTMLGump(280, 190, 557, 207, false, false, "With no parameters, it adds a backpack(if missing) to any targeted character. Using either string or HEX hexid will add the specified item(string = from dfns, hex = from tiledata) to targeted player's backpack.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'addPack</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>(target) / (s) / HEX(h)</BASEFONT><BR><BR>"
		+ "<BR>Examples:"
		+ "<BR>'addPack (add backpack to targeted character)"
		+ "<BR>'addPack 0x04a9 (add raw wooden tile in backpack)");

	uox3gump.AddPage(104);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddPageButton(270, 150, 2446, 2445, 103);
	uox3gump.AddPageButton(270, 180, 2446, 2445, 104);
	uox3gump.AddPageButton(270, 400, 2446, 2445, 105);
	uox3gump.AddPageButton(270, 430, 2446, 2445, 106);
	uox3gump.AddPageButton(270, 460, 2446, 2445, 107);
	uox3gump.AddPageButton(270, 490, 2446, 2445, 108);
	uox3gump.AddText(330, 150, 0, "addPack");
	uox3gump.AddText(330, 180, 0, "addX");
	uox3gump.AddText(320, 400, 0, "adminYell");
	uox3gump.AddText(320, 430, 0, "allMove");
	uox3gump.AddText(320, 460, 0, "announce");
	uox3gump.AddText(320, 490, 0, "areaCommand");
	uox3gump.AddBackground(270, 210, 592, 178, 9300);
	uox3gump.AddHTMLGump(280, 220, 550, 158, false, false, "Adds a new raw item to your current location.Accepts a value after the itemID to specify Z height to spawn item at.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'addX</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>(h) / (h z)</BASEFONT><BR><BR>"
		+ "<BR>	Example: 'addX 0x04a9"
		+ "<BR>	Example: 'addX 0x04a9 10");

	uox3gump.AddPage(105);
	uox3gump.AddBackground(270, 240, 592, 178, 9300);
	uox3gump.AddPageButton(270, 90, 2446, 2445, 100);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddPageButton(270, 60, 2446, 2445, 101);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddPageButton(270, 150, 2446, 2445, 103);
	uox3gump.AddPageButton(270, 180, 2446, 2445, 104);
	uox3gump.AddPageButton(270, 210, 2446, 2445, 105);
	uox3gump.AddPageButton(270, 430, 2446, 2445, 106);
	uox3gump.AddPageButton(270, 460, 2446, 2445, 107);
	uox3gump.AddPageButton(270, 490, 2446, 2445, 108);
	uox3gump.AddText(330, 150, 0, "addPack");
	uox3gump.AddText(330, 180, 0, "addX");
	uox3gump.AddText(330, 210, 0, "adminYell");
	uox3gump.AddText(320, 430, 0, "allMove");
	uox3gump.AddText(320, 460, 0, "announce");
	uox3gump.AddText(320, 490, 0, "areaCommand");
	uox3gump.AddHTMLGump(280, 250, 550, 158, false, false, "Broadcasts text to all players online with command level of 3 and above.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'adminYell</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>(text)</BASEFONT><BR><BR>"
		+ "<BR>	Example: 'adminYell Hello fellow admins!");

	uox3gump.AddPage(106);
	uox3gump.AddBackground(270, 270, 592, 158, 9300);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddPageButton(270, 150, 2446, 2445, 103);
	uox3gump.AddPageButton(270, 180, 2446, 2445, 104);
	uox3gump.AddPageButton(270, 210, 2446, 2445, 105);
	uox3gump.AddPageButton(270, 240, 2446, 2445, 106);
	uox3gump.AddPageButton(270, 440, 2446, 2445, 107);
	uox3gump.AddPageButton(270, 470, 2446, 2445, 108);
	uox3gump.AddText(330, 150, 0, "addPack");
	uox3gump.AddText(330, 180, 0, "addX");
	uox3gump.AddText(330, 210, 0, "adminYell");
	uox3gump.AddText(330, 240, 0, "allMove");
	uox3gump.AddText(320, 440, 0, "announce");
	uox3gump.AddText(320, 470, 0, "areaCommand");
	uox3gump.AddHTMLGump(280, 280, 550, 137, false, false, "Enables or disables a GMs ability to pick up all objects.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'allMove</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>on / off</BASEFONT><BR>"
		+ "<BR>	Example: 'allMove on");

	uox3gump.AddPage(107);
	uox3gump.AddBackground(270, 300, 592, 158, 9300);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddPageButton(270, 150, 2446, 2445, 103);
	uox3gump.AddPageButton(270, 180, 2446, 2445, 104);
	uox3gump.AddPageButton(270, 210, 2446, 2445, 105);
	uox3gump.AddPageButton(270, 240, 2446, 2445, 106);
	uox3gump.AddPageButton(270, 270, 2446, 2445, 107);
	uox3gump.AddPageButton(270, 470, 2446, 2445, 108);
	uox3gump.AddText(330, 150, 0, "addPack");
	uox3gump.AddText(330, 180, 0, "addX");
	uox3gump.AddText(330, 210, 0, "adminYell");
	uox3gump.AddText(330, 240, 0, "allMove");
	uox3gump.AddText(330, 270, 0, "announce");
	uox3gump.AddText(320, 470, 0, "areaCommand");
	uox3gump.AddHTMLGump(280, 310, 550, 137, false, false, "Enables or disables global announcement of worldsaves.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'announce</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>on / off</BASEFONT><BR>"
		+ "<BR>	Example: 'announce off");

	uox3gump.AddPage(108);
	uox3gump.AddBackground(270, 330, 592, 158, 9300);
	uox3gump.AddPageButton(270, 60, 2446, 2445, 100);
	uox3gump.AddText(340, 60, 0, "Action");
	uox3gump.AddPageButton(270, 90, 2446, 2445, 101);
	uox3gump.AddText(340, 90, 0, "Add");
	uox3gump.AddPageButton(270, 120, 2446, 2445, 102);
	uox3gump.AddText(320, 120, 0, "AddAccount");
	uox3gump.AddPageButton(270, 150, 2446, 2445, 103);
	uox3gump.AddPageButton(270, 180, 2446, 2445, 104);
	uox3gump.AddPageButton(270, 210, 2446, 2445, 105);
	uox3gump.AddPageButton(270, 240, 2446, 2445, 106);
	uox3gump.AddPageButton(270, 270, 2446, 2445, 107);
	uox3gump.AddPageButton(270, 300, 2446, 2445, 108);
	uox3gump.AddText(330, 150, 0, "addPack");
	uox3gump.AddText(330, 180, 0, "addX");
	uox3gump.AddText(330, 210, 0, "adminYell");
	uox3gump.AddText(330, 240, 0, "allMove");
	uox3gump.AddText(330, 270, 0, "announce");
	uox3gump.AddText(330, 300, 0, "areaCommand");
	uox3gump.AddHTMLGump(280, 340, 550, 137, false, false, "Used to perform other commands on all items within a targeted area.<BR>"
		+ "<BR>Command: <BASEFONT color=#ff0000>'areaCommand</BASEFONT>"
		+ "<BR>Parameters: <BASEFONT color=#ff0000>dye (h) / wipe / incX (d) / incY (d) / incZ (d) / setX (d) / setY (d) / setZ (d) / setType (d) / newbie (d) / setScpTrig (d) / movable (d)</BASEFONT><BR>"
		+ "<BR>	Example: 'areaCommand dye 0x83ea");
}

function _restorecontext_() {}