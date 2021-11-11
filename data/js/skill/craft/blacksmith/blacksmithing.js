var textHue = 0x480;				// Color of the text.
var blacksmithID = 4023;			// Use this to tell the gump what script to close.
var bronzeID = 4015;
var copperID = 4016;
var agapiteID = 4017;
var dullcopperID = 4018;
var goldID = 4019;
var shadowironID = 4020;
var valoriteID = 4021;
var veriteID = 4022;
const gumpDelay = 2000;				// Timer for the gump to reapear after crafting.
const ingotDelay = 200;				// Timer for the gump to reapear after selecting a ingot.
var craftGumpID = 4027;
var itemDetailsID = 4026;

//////////////////////////////////////////////////////////////////////////////////////////
//  The section below is the tables for each page.
//  All you have to do is add the armor or weapon to your dictionary 
//  and then list the dictionary number in the right page and it will
//  add it to the crafting gump.
///////////////////////////////////////////////////////////////////////////////////////////

var myPage1 = [10217, 10218, 10219, 10220, 10221, 10222, 10223, 10224, 10225, 10226, 10228, 10229];	// Metal Armors
var myPage2 = [10230, 10231, 10232, 10233, 10234]; // Helmets
var myPage3 = [10235, 10236, 10237, 10238, 10239, 10293]; // Shields
var myPage4 = [10240, 10241, 10242, 10243, 10244, 10245, 10246, 10247]; // Bladed
var myPage5 = [10248, 10249, 10250, 10251, 10252, 10253, 10254]; // Axes
var myPage6 = [10255, 10256, 10257, 10258, 10259]; // PoleArms
var myPage7 = [10260, 10261, 10262, 10263, 10264]; // Bashing

function page1(socket, pUser)
{// Metal Armors
	var myGump = new Gump;
	pUser.SetTempTag("page", 1);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for (var i = 0; i < myPage1.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}
		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 100 + i);

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry(myPage1[i], socket.language));

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2100 + i);
	}
	myGump.Send(socket);
	myGump.Free();
}

function page2(socket, pUser)
{// helmets
	var myGump = new Gump;
	pUser.SetTempTag("page", 2);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for (var i = 0; i < myPage2.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}
		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 200 + i);

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry(myPage2[i], socket.language));

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2200 + i);
	}
	myGump.Send(socket);
	myGump.Free();
}

function page3(socket, pUser)
{//Shields
	var myGump = new Gump;
	pUser.SetTempTag("page", 3);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for (var i = 0; i < myPage3.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}
		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 300 + i);

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry(myPage3[i], socket.language));

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2300 + i);
	}
	myGump.Send(socket);
	myGump.Free();
}

function page4(socket, pUser)
{//Bladed
	var myGump = new Gump;
	pUser.SetTempTag("page", 4);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for (var i = 0; i < myPage4.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}
		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 400 + i);

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry(myPage4[i], socket.language));

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2400 + i);
	}
	myGump.Send(socket);
	myGump.Free();
}

function page5(socket, pUser)
{//Axes
	var myGump = new Gump;
	pUser.SetTempTag("page", 5);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for (var i = 0; i < myPage5.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}
		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 500 + i);

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry(myPage5[i], socket.language));

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2500 + i);
	}
	myGump.Send(socket);
	myGump.Free();
}

function page6(socket, pUser)
{//Polearms
	var myGump = new Gump;
	pUser.SetTempTag("page", 6);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for (var i = 0; i < myPage6.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}
		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 600 + i);

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry(myPage6[i], socket.language));

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2600 + i);
	}
	myGump.Send(socket);
	myGump.Free();
}

function page7(socket, pUser)
{//Bashing
	var myGump = new Gump;
	pUser.SetTempTag("page", 7);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	for (var i = 0; i < myPage7.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}
		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 700 + i);

		myGump.AddText(255, 60 + (index * 20), textHue, GetDictionaryEntry(myPage7[i], socket.language));

		myGump.AddButton(480, 60 + (index * 20), 4011, 4012, 1, 0, 2700 + i);
	}
	myGump.Send(socket);
	myGump.Free();
}

function page8(socket, pUser)
{//Ingot Choices
	var myGump = new Gump;
	pUser.SetTempTag("page", 8);
	TriggerEvent(craftGumpID, "craftinggump", myGump, socket);
	var iron = pUser.ResourceCount(0x1BF2);
	var bronze = pUser.ResourceCount(0x1BF2, 0x06d6);
	var copper = pUser.ResourceCount(0x1BF2, 0x07dd);
	var agapite = pUser.ResourceCount(0x1BF2, 0x0979);
	var dullcopper = pUser.ResourceCount(0x1BF2, 0x0973);
	var gold = pUser.ResourceCount(0x1BF2, 0x08a5);
	var shadowiron = pUser.ResourceCount(0x1BF2, 0x0966);
	var valorlite = pUser.ResourceCount(0x1BF2, 0x08ab);
	var verite = pUser.ResourceCount(0x1BF2, 0x089f);
	var myPage8 = [GetDictionaryEntry(10291, socket.language) + " (" + iron.toString() + ")", GetDictionaryEntry(10203, socket.language) + " (" + bronze.toString() + ")",
		GetDictionaryEntry(10204, socket.language) + " (" + copper.toString() + ")", GetDictionaryEntry(10205, socket.language) + " (" + agapite.toString() + ")",
		GetDictionaryEntry(10206, socket.language) + " (" + dullcopper.toString() + ")", GetDictionaryEntry(10207, socket.language) + " (" + gold.toString() + ")",
		GetDictionaryEntry(10208, socket.language) + " (" + shadowiron.toString() + ")", GetDictionaryEntry(10209, socket.language) + " (" + valorlite.toString() + ")",
		GetDictionaryEntry(10210, socket.language) + " (" + verite.toString() + ")"]

	for (var i = 0; i < myPage8.length; i++)
	{
		var index = i % 10;
		if (index == 0)
		{
			if (i > 0)
			{
				myGump.AddButton(370, 260, 4005, 4007, 0, (i / 10) + 1, 0);
				myGump.AddHTMLGump(405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10100, socket.Language) + "</basefont>");// NEXT PAGE
			}

			myGump.AddPage((i / 10) + 1);

			if (i > 0)
			{
				myGump.AddButton(220, 260, 4014, 4015, 0, i / 10, 0);
				myGump.AddHTMLGump(255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry(10101, socket.Language) + "</basefont>");// PREV PAGE
			}
		}

		myGump.AddButton(220, 60 + (index * 20), 4005, 4007, 1, 0, 1000 + i);
		myGump.AddText(255, 60 + (index * 20), textHue, myPage8[i]);
	}
	myGump.Send(socket);
	myGump.Free();
}

function findNearbyItems(pUser, trgItem, pSock)
{
	var isAnvilForge = (trgItem.id == 0x0faf || trgItem.id == 0x0fb0 || trgItem.id == 0x2DD5 || trgItem.id == 0x2DD6 ||
		trgItem.id == 0x197A || trgItem.id == 0x0FB1 || trgItem.id == 0x19A9 || trgItem.id == 0x2DD8)

	if (trgItem && trgItem.isItem && isAnvilForge)
		return true;
	else
		return false;
}

function target( pSock )
{
	var targMsg = GetDictionaryEntry( 485, pSock.Language );
	pSock.CustomTarget( 2, targMsg );
}

function onCallback2(socket, ourObj)
{//Repair Item
	var mChar = socket.currentChar;
	var bItem = socket.tempObj;
	var anvil = AreaItemFunction("findNearbyItems", mChar, 4, mChar.socket);
	var gumpID = blacksmithID + 0xffff;
	socket.tempObj = null;

	if (mChar && mChar.isChar && bItem && bItem.isItem)
	{
		if (!ourObj || !ourObj.isItem)
		{
			socket.tempObj = bItem;
			socket.CloseGump(gumpID, 0);
			mChar.SetTempTag("CANTREPAIR", 1);
			mChar.StartTimer(ingotDelay, 1, true);
			return;
		}
		if (anvil > 0)
		{
			var minSkill = 999;
			var maxSkill = 1000;
			var repairID = ourObj.id;
			if (repairids(repairID))
			{
				var ownerObj = GetPackOwner(ourObj, 0);
				if (ownerObj && mChar.serial == ownerObj.serial)
				{
					if (ourObj.health < ourObj.maxhp)
					{
						if (ourObj.def > 0)
						{
							// Items with > 12 def would have impossible skill req's, with this equation
							if (ourObj.def <= 12)
							{
								// Minimum Skill = 61.0 + Defense - 1 / 3 * 100 (0-3 = 61.0, 4-6 = 71.0, ect)
								minSkill = (610 + ((ourObj.def - 1) / 3) * 100);
								// Maximum Skill = 84.9 + Defense - 1 / 3 * 50 (0-3 = 84.9, 4-6 = 89.9, ect)
								maxSkill = (849 + ((ourObj.def - 1) / 3) * 50);
							}
						}
						else if (ourObj.hidamage > 0)
						{
							var offset = ((ourObj.lodamage + ourObj.hidamage) / 2);
							// Items with > 25 Avg Damage would have impossible skill req's, with this equation
							if (offset <= 25)
							{
								minSkill = (510 + ((offset - 1) / 5) * 100);
								maxSkill = (799 + ((offset - 1) / 5) * 50);
							}
						}
						else
						{
							socket.tempObj = bItem;
							socket.CloseGump(gumpID, 0);
							mChar.SetTempTag("CANTREPAIR", 1);
							mChar.StartTimer(ingotDelay, 1, true);
							return;
						}

						if (mChar.CheckSkill(8, minSkill, maxSkill))
						{
							socket.CloseGump(gumpID, 0);
							ourObj.health = ourObj.maxhp;
							mChar.SetTempTag("REPAIRSUCCESS", 1);
							socket.SoundEffect(0x002A, true);
							mChar.StartTimer(ingotDelay, 1, true);
						}
						else
						{
							socket.tempObj = bItem;
							socket.CloseGump(gumpID, 0);
							mChar.SetTempTag("FAILREPAIR", 1);
							mChar.StartTimer(ingotDelay, 1, true);
						}
					}
					else
					{
						socket.tempObj = bItem;
						socket.CloseGump(gumpID, 0);
						mChar.SetTempTag("FULLREPAIR", 1);
						mChar.StartTimer(ingotDelay, 1, true);
					}
				}
				else
				{
					socket.tempObj = bItem;
					socket.CloseGump(gumpID, 0);
					mChar.SetTempTag("CHECKPACK", 1);
					mChar.StartTimer(ingotDelay, 1, true);
				}
			}
			else
			{
				socket.tempObj = bItem;
				socket.CloseGump(gumpID, 0);
				mChar.SetTempTag("CANTREPAIR", 1);
				mChar.StartTimer(ingotDelay, 1, true);
			}
		}
		else
		{
			socket.tempObj = bItem;
			socket.CloseGump(gumpID, 0);
			mChar.SetTempTag("NOANVIL", 1);
			mChar.StartTimer(ingotDelay, 1, true);
		}
	}
}

function repairids(repairID)
{
	return (repairID == 0x0F43 || repairID == 0x0F44 || repairID == 0x0F45 || repairID == 0x0F46 ||
		repairID == 0x0F47 || repairID == 0x0F48 || repairID == 0x0F49 || repairID == 0x0F4A ||
		repairID == 0x0F4B || repairID == 0x0F4C || repairID == 0x0F4E || repairID == 0x0F4E ||
		repairID == 0x0F51 || repairID == 0x0F52 || repairID == 0x0F5C || repairID == 0x0F5D ||
		repairID == 0x0F5E || repairID == 0x0F5F || repairID == 0x0F60 || repairID == 0x0F61 ||
		repairID == 0x0F62 || repairID == 0x0F63 || repairID == 0x13AF || repairID == 0x13B0 ||
		repairID == 0x13B5 || repairID == 0x13B6 || repairID == 0x13B7 || repairID == 0x13B8 ||
		repairID == 0x13B9 || repairID == 0x13BA || repairID == 0x13F6 || repairID == 0x13F7 ||
		repairID == 0x13FA || repairID == 0x13FB || repairID == 0x13FC || repairID == 0x13FD ||
		repairID == 0x13FE || repairID == 0x13FF || repairID == 0x1400 || repairID == 0x1401 ||
		repairID == 0x1402 || repairID == 0x1403 || repairID == 0x1404 || repairID == 0x1405 ||
		repairID == 0x1406 || repairID == 0x1407 || repairID == 0x1438 || repairID == 0x1439 ||
		repairID == 0x143A || repairID == 0x143B || repairID == 0x143C || repairID == 0x143D ||
		repairID == 0x143E || repairID == 0x143F || repairID == 0x1440 || repairID == 0x1441 ||
		repairID == 0x1442 || repairID == 0x1443 || repairID == 0x13BB || repairID == 0x13BC ||
		repairID == 0x13BD || repairID == 0x13BE || repairID == 0x13BF || repairID == 0x13C0 ||
		repairID == 0x13C1 || repairID == 0x13C2 || repairID == 0x13C3 || repairID == 0x13C4 ||
		repairID == 0x13E5 || repairID == 0x13E6 || repairID == 0x13E7 || repairID == 0x13E8 ||
		repairID == 0x13E9 || repairID == 0x13EA || repairID == 0x13EB || repairID == 0x13EC ||
		repairID == 0x13ED || repairID == 0x13EE || repairID == 0x13EF || repairID == 0x13F0 ||
		repairID == 0x13F1 || repairID == 0x13F2 || repairID == 0x1408 || repairID == 0x1409 ||
		repairID == 0x140A || repairID == 0x140B || repairID == 0x140C || repairID == 0x140D ||
		repairID == 0x140E || repairID == 0x140F || repairID == 0x1410 || repairID == 0x1411 ||
		repairID == 0x1412 || repairID == 0x1413 || repairID == 0x1414 || repairID == 0x1415 ||
		repairID == 0x1416 || repairID == 0x1417 || repairID == 0x1418 || repairID == 0x1419 ||
		repairID == 0x141A);
}

function onTimer(pUser, timerID)
{
	var socket = pUser.socket;

	switch (timerID)
	{
		case 1:
			TriggerEvent(blacksmithID, "page1", socket, pUser);
			break;
		case 2:
			TriggerEvent(blacksmithID, "page2", socket, pUser);
			break;
		case 3:
			TriggerEvent(blacksmithID, "page3", socket, pUser);
			break;
		case 4:
			TriggerEvent(blacksmithID, "page4", socket, pUser);
			break;
		case 5:
			TriggerEvent(blacksmithID, "page5", socket, pUser);
			break;
		case 6:
			TriggerEvent(blacksmithID, "page6", socket, pUser);
			break;
		case 7:
			TriggerEvent(blacksmithID, "page7", socket, pUser);
			break;
		case 8:
			TriggerEvent(blacksmithID, "page8", socket, pUser);
			break;
	}
}

function onGumpPress(pSock, pButton, gumpData)
{
	var pUser = pSock.currentChar;
	var bItem = pSock.tempObj;
	var gumpID = blacksmithID + 0xffff;
	var anvil = AreaItemFunction("findNearbyItems", pUser, 2, pUser.socket);
	switch (pButton)
	{
		case 0:
			pUser.SetTempTag("FAILED", null);
			pUser.SetTempTag("MAKELAST", null);
			pSock.CloseGump(gumpID, 0);
			break;// abort and do nothing
		case 1:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page1", pSock, pUser);
			break;
		case 2:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page2", pSock, pUser);
			break;
		case 3:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page3", pSock, pUser);
			break;
		case 4:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page4", pSock, pUser);
			break;
		case 5:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page5", pSock, pUser);
			break;
		case 6:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page6", pSock, pUser);
			break;
		case 7:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page7", pSock, pUser);
			break;
		case 49:
			var targMsg = GetDictionaryEntry(485, pSock.Language);
			pSock.CustomTarget(2, targMsg);
			break;
		case 50:
			pSock.CloseGump(gumpID, 0);
			TriggerEvent(blacksmithID, "page8", pSock, pUser);
			break;
		// [100-199]
		case 100:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 7);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 100);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Ringmail Gloves
			}
		case 101:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 9);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 101);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Ringmail Legs
			}
		case 102:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 8);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 102);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Ringmail Arms
			}
		case 103:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 10);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 103);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Ringmail Chest
			}
		case 104:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 11);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 104);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Chain Coif
			}
		case 105:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 12);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 105);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Chain Legs
			}
		case 106:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 13);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 106);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Chain Chest
			}
		case 107:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 16);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 107);
				page1
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Plate Arms
			}
		case 108:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 15);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 108);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Plate Gloves
			}
		case 109:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 14);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 109);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Plate Gorget
			}
		case 110:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 17);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 110);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Plate Legs
			}
		case 111:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 18);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 111);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Plate Chest
			}
		case 112:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 19);
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("MAKELAST", 112);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 1, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Female Plate Chest
			}
		// [200-299]
		case 200:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 46);
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("MAKELAST", 200);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Bascinet
			}
		case 201:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 48);
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("MAKELAST", 201);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Close Helm
			}
		case 202:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 45);
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("MAKELAST", 202);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Helmet
			}
		case 203:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 47);
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("MAKELAST", 203);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Norse Helm
			}
		case 204:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 49);
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("MAKELAST", 204);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 2, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Plate Helm
			}
		// [300-399]
		case 300:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 1);
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("MAKELAST", 300);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Buckler
			}
		case 301:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 2);
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("MAKELAST", 301);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Bronze Shield
			}
		case 302:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 6);
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("MAKELAST", 302);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Heater Shield
			}
		case 303:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 3);
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("MAKELAST", 304);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Metal Shield
			}
		case 304:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 5);
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("MAKELAST", 305);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Metal Kite Shield
			}
		case 305:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 4);
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("MAKELAST", 305);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 3, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//tear kite shield
			}
		// [400-499]
		case 400:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 25);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 400);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Broadsword
			}
		case 401:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 21);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 401);
				break
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Cutlass
			}
		case 402:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 20);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 402);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Dagger
			}
		case 403:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 22);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 403);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Katana
			}
		case 404:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 23);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 404);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Kryss
			}
		case 405:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 26);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 405);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Longsword
			}
		case 406:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 24);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 406);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Scimitar
			}
		case 407:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 27);
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("MAKELAST", 407);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 4, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Viking Sword
			}
		// [500-599]
		case 500:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 29);
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("MAKELAST", 500);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Axe
			}
		case 501:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 28);
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("MAKELAST", 501);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Battle Axe
			}
		case 502:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 32);
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("MAKELAST", 502);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Double Axe
			}
		case 503:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 30);
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("MAKELAST", 503);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Executioners Axe
			}
		case 504:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 33);
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("MAKELAST", 504);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Large Battle Axe
			}
		case 505:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 31);
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("MAKELAST", 505);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Two Handed Axe
			}
		case 506:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 34);
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("MAKELAST", 506);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 5, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//War Axe
			}
		// [600-699]
		case 600:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 38);
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("MAKELAST", 600);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Bardiche
			}
		case 601:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 39);
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("MAKELAST", 601);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Halberd
			}
		case 602:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 35);
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("MAKELAST", 602);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Short Spear
			}
		case 603:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 36);
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("MAKELAST", 603);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Spear
			}
		case 604:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 37);
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("MAKELAST", 700);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 6, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//WarFork
			}
		// [700-799]
		case 700:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 44);
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("MAKELAST", 701);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Hammer Pick
			}
		case 701:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 40);
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("MAKELAST", 702);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Mace
			}
		case 702:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 41);
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("MAKELAST", 703);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//Maul
			}
		case 703:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 42);
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("MAKELAST", 704);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//War Mace
			}
		case 704:
			if (anvil > 0)
			{
				MakeItem(pSock, pUser, 43);
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("MAKELAST", 705);
				break;
			}
			else
			{
				pUser.StartTimer(gumpDelay, 7, true);
				pUser.SetTempTag("NOANVIL", 1);
				break;//War Hammer
			}
		case 1000:
			pSock.CloseGump(gumpID, 0);
			pUser.SetTempTag("ORE", 1);
			TriggerEvent(blacksmithID, "page8", pSock, pUser);
			break;
		case 1001:
			if (pUser.skills[7] < 800)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 1);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 2);
				TriggerEvent(bronzeID, "page8", pSock, pUser);
			}
			break;
		case 1002:
			if (pUser.skills[7] < 750)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 2);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 3);
				TriggerEvent(copperID, "page8", pSock, pUser);
			}
			break;
		case 1003:
			if (pUser.skills[7] < 900)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 3);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 4);
				TriggerEvent(agapiteID, "page8", pSock, pUser);
			}
			break;
		case 1004:
			if (pUser.skills[7] < 650)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 4);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 5);
				TriggerEvent(dullcopperID, "page8", pSock, pUser);
			}
			break;
		case 1005:
			if (pUser.skills[7] < 850)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 5);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 6);
				TriggerEvent(goldID, "page8", pSock, pUser);
			}
			break;
		case 1006:
			if (pUser.skills[7] < 700)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 6);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 7);
				TriggerEvent(shadowironID, "page8", pSock, pUser);
			}
			break;
		case 1007:
			if (pUser.skills[7] < 990)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 7);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 8);
				TriggerEvent(valoriteID, "page8", pSock, pUser);
			}
			break;
		case 1008:
			if (pUser.skills[7] < 995)
			{
				pSock.CloseGump(gumpID, 0);
				pUser.StartTimer(ingotDelay, 8, true);
				pUser.SetTempTag("FAILED", 8);
				break;
			}
			else
			{
				pSock.CloseGump(gumpID, 0);
				pUser.SetTempTag("MAKELAST", null);
				pUser.SetTempTag("FAILED", null);
				pUser.SetTempTag("ORE", 9);
				TriggerEvent(veriteID, "page8", pSock, pUser);
			}
			break;
		case 2100:
			pUser.SetTempTag("ITEMDETAILS", 7);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Ringmail Gloves
		case 2101:
			pUser.SetTempTag("ITEMDETAILS", 9);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Ringmail Legs
		case 2102:
			pUser.SetTempTag("ITEMDETAILS", 8);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Ringmail Arms
		case 2103:
			pUser.SetTempTag("ITEMDETAILS", 10);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Ringmail Chest
		case 2104:
			pUser.SetTempTag("ITEMDETAILS", 11);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Chain Coif
		case 2105:
			pUser.SetTempTag("ITEMDETAILS", 12);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Chain Legs
		case 2106:
			pUser.SetTempTag("ITEMDETAILS", 13);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Chain Chest
		case 2107:
			pUser.SetTempTag("ITEMDETAILS", 16);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Plate Arms
		case 2108:
			pUser.SetTempTag("ITEMDETAILS", 15);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Plate Gloves
		case 2109:
			pUser.SetTempTag("ITEMDETAILS", 14);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Plate Gorget
		case 2110:
			pUser.SetTempTag("ITEMDETAILS", 17);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Plate Legs
		case 2111:
			pUser.SetTempTag("ITEMDETAILS", 18);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Plate Chest
		case 2112:
			pUser.SetTempTag("ITEMDETAILS", 19);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Female Plate Chest
		// [200-299]
		case 2200:
			pUser.SetTempTag("ITEMDETAILS", 46);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Bascinet
		case 2201:
			pUser.SetTempTag("ITEMDETAILS", 48);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Close Helm
		case 2202:
			pUser.SetTempTag("ITEMDETAILS", 45);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Helmet
		case 2203:
			pUser.SetTempTag("ITEMDETAILS", 47);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Norse Helm
		case 2204:
			pUser.SetTempTag("ITEMDETAILS", 49);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Plate Helm
		// [300-399]
		case 2300:
			pUser.SetTempTag("ITEMDETAILS", 1);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Buckler
		case 2301:
			pUser.SetTempTag("ITEMDETAILS", 2);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Bronze Shield
		case 2302:
			pUser.SetTempTag("ITEMDETAILS", 6);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Heater Shield
		case 2303:
			pUser.SetTempTag("ITEMDETAILS", 3);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Metal Shield
		case 2304:
			pUser.SetTempTag("ITEMDETAILS", 5);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//MetalKiteShield
		case 2305:
			pUser.SetTempTag("ITEMDETAILS", 4);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//tear kite shield
		// [400-499]
		case 2400:
			pUser.SetTempTag("ITEMDETAILS", 25);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Broadsword
		case 2401:
			pUser.SetTempTag("ITEMDETAILS", 21);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Cutlass
		case 2402:
			pUser.SetTempTag("ITEMDETAILS", 20);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Dagger
		case 2403:
			pUser.SetTempTag("ITEMDETAILS", 22);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Katana
		case 2404:
			pUser.SetTempTag("ITEMDETAILS", 23);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Kryss
		case 2405:
			pUser.SetTempTag("ITEMDETAILS", 26);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Longsword
		case 2406:
			pUser.SetTempTag("ITEMDETAILS", 24);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Scimitar
		case 2407:
			pUser.SetTempTag("ITEMDETAILS", 27);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Viking Sword
		// [500-599]
		case 2500:
			pUser.SetTempTag("ITEMDETAILS", 29);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Axe
		case 2501:
			pUser.SetTempTag("ITEMDETAILS", 28);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Battle Axe
		case 2502:
			pUser.SetTempTag("ITEMDETAILS", 32);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Double Axe
		case 2503:
			pUser.SetTempTag("ITEMDETAILS", 30);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Executioners Axe
		case 2504:
			pUser.SetTempTag("ITEMDETAILS", 33);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Large Battle Axe
		case 2505:
			pUser.SetTempTag("ITEMDETAILS", 31);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Two Handed Axe
		case 2506:
			pUser.SetTempTag("ITEMDETAILS", 34);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//War Axe
		// [600-699]
		case 2600:
			pUser.SetTempTag("ITEMDETAILS", 38);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Bardiche
		case 2601:
			pUser.SetTempTag("ITEMDETAILS", 39);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Halberd
		case 2602:
			pUser.SetTempTag("ITEMDETAILS", 35);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Short Spear
		case 2603:
			pUser.SetTempTag("ITEMDETAILS", 36);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Spear
		case 2604:
			pUser.SetTempTag("ITEMDETAILS", 37);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//WarFork
		// [700-799]
		case 2700:
			pUser.SetTempTag("ITEMDETAILS", 44);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Hammer Pick
		case 2701:
			pUser.SetTempTag("ITEMDETAILS", 40);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Mace
		case 2702:
			pUser.SetTempTag("ITEMDETAILS", 41);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//Maul
		case 2703:
			pUser.SetTempTag("ITEMDETAILS", 42);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//War Mace
		case 2704:
			pUser.SetTempTag("ITEMDETAILS", 43);
			TriggerEvent(itemDetailsID, "ItemDetailGump", pUser);
			break;//War Hammer
		case 5000:
			switch (pUser.SetTempTag("MAKELAST"))
			{
				// [100-199]
				case 100:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 7);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 100);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Ringmail Gloves
					}
				case 101:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 9);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 101);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Ringmail Legs
					}
				case 102:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 8);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 102);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Ringmail Arms
					}
				case 103:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 10);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 103);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Ringmail Chest
					}
				case 104:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 11);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 104);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Chain Coif
					}
				case 105:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 12);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 105);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Chain Legs
					}
				case 106:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 13);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 106);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Chain Chest
					}
				case 107:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 16);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 107);
						page1
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Plate Arms
					}
				case 108:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 15);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 108);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Plate Gloves
					}
				case 109:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 14);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 109);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Plate Gorget
					}
				case 110:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 17);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 110);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Plate Legs
					}
				case 111:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 18);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 111);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Plate Chest
					}
				case 112:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 19);
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("MAKELAST", 112);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 1, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Female Plate Chest
					}
				// [200-299]
				case 200:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 46);
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("MAKELAST", 200);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Bascinet
					}
				case 201:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 48);
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("MAKELAST", 201);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Close Helm
					}
				case 202:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 45);
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("MAKELAST", 202);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Helmet
					}
				case 203:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 47);
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("MAKELAST", 203);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Norse Helm
					}
				case 204:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 49);
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("MAKELAST", 204);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 2, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Plate Helm
					}
				// [300-399]
				case 300:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 367);
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("MAKELAST", 300);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Buckler
					}
				case 301:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 2);
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("MAKELAST", 301);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Bronze Shield
					}
				case 302:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 6);
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("MAKELAST", 302);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Heater Shield
					}
				case 303:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 3);
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("MAKELAST", 304);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Metal Shield
					}
				case 304:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 5);
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("MAKELAST", 305);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//MetalKiteShield
					}
				case 305:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 4);
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("MAKELAST", 305);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 3, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//tear kite shield
					}
				// [400-499]
				case 400:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 25);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 400);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Broadsword
					}
				case 401:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 21);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 401);
						break
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Cutlass
					}
				case 402:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 20);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 402);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Dagger
					}
				case 403:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 22);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 403);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Katana
					}
				case 404:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 23);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 404);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Kryss
					}
				case 405:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 26);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 405);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Longsword
					}
				case 406:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 24);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 406);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Scimitar
					}
				case 407:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 27);
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("MAKELAST", 407);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 4, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Viking Sword
					}
				// [500-599]
				case 500:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 29);
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("MAKELAST", 500);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Axe
					}
				case 501:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 28);
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("MAKELAST", 501);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Battle Axe
					}
				case 502:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 32);
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("MAKELAST", 502);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Double Axe
					}
				case 503:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 30);
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("MAKELAST", 503);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Executioners Axe
					}
				case 504:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 33);
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("MAKELAST", 504);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Large Battle Axe
					}
				case 505:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 31);
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("MAKELAST", 505);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Two Handed Axe
					}
				case 506:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 34);
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("MAKELAST", 506);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 5, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//War Axe
					}
				// [600-699]
				case 600:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 38);
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("MAKELAST", 600);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Bardiche
					}
				case 601:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 39);
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("MAKELAST", 601);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Halberd
					}
				case 602:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 35);
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("MAKELAST", 602);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Short Spear
					}
				case 603:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 36);
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("MAKELAST", 603);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Spear
					}
				case 604:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 37);
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("MAKELAST", 700);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 6, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//WarFork
					}
				// [700-799]
				case 700:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 44);
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("MAKELAST", 701);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Hammer Pick
					}
				case 701:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 40);
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("MAKELAST", 702);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Mace
					}
				case 702:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 41);
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("MAKELAST", 703);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//Maul
					}
				case 703:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 42);
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("MAKELAST", 704);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//War Mace
					}
				case 704:
					if (anvil > 0)
					{
						MakeItem(pSock, pUser, 43);
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("MAKELAST", 705);
						break;
					}
					else
					{
						pUser.StartTimer(gumpDelay, 7, true);
						pUser.SetTempTag("NOANVIL", 1);
						break;//War Hammer
					}
			}
	}
}