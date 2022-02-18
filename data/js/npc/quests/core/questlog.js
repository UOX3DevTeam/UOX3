function questlog(mKiller)
{
	var questGump = new Gump; // create a new gump
	questGump.AddPage(0);

	questGump.AddGump(20, 5, 1417);
	questGump.AddTiledGump(0, 78, 120, 40, 2624);
	questGump.AddCheckerTrans(0, 78, 120, 40);
	questGump.AddXMFHTMLGumpColor(0, 78, 120, 40, 1049079, false, false, 0x7FFF); // Quest Log Updated

	questGump.AddButton(30, 15, 5575, 5576, 1, 0, 1);
	questGump.Send(mKiller); // send this gump to client now 
	questGump.Free(); // clear this gump from uox-memory 
}

function progress(pUser)
{
	var questGump = new Gump; // create a new gump
	questGump.AddPage(0);

	questGump.AddGump(0, 0, 3600);
	questGump.AddTiledGump(0, 14, 15, 375, 3603);
	questGump.AddTiledGump(380, 14, 14, 375, 3605);
	questGump.AddGump(0, 376, 3606);
	questGump.AddTiledGump(15, 376, 370, 16, 3607);
	questGump.AddTiledGump(15, 0, 370, 16, 3601);
	questGump.AddGump(380, 0, 3602);
	questGump.AddGump(380, 376, 3608);

	questGump.AddTiledGump(15, 15, 365, 365, 2624);
	questGump.AddCheckerTrans(15, 15, 365, 365);

	questGump.AddGump(20, 87, 1231);
	questGump.AddGump(75, 62, 9307);

	questGump.AddXMFHTMLGumpColor(117, 35, 230, 20, 1046026, false, false, 19777215); // Quest Log
	questGump.AddGump(77, 33, 9781);
	questGump.AddGump(65, 110, 2104);

	questGump.AddXMFHTMLGumpColor(79, 106, 230, 20, 1049073, false, false, 19777215); // Objective:

	questGump.AddTiledGump(68, 125, 120, 1, 9101);
	questGump.AddGump(65, 240, 2104);

	questGump.AddXMFHTMLGumpColor(79, 237, 230, 20, 1049076, false, false, 19777215); // Progress details:

	questGump.AddTiledGump(68, 255, 120, 1, 9101);
	questGump.AddButton(175, 355, 2313, 2312, 1, 0, 0);

	questGump.AddGump(341, 15, 10450);
	questGump.AddGump(341, 330, 10450);
	questGump.AddGump(15, 330, 10450);
	questGump.AddGump(15, 15, 10450);

	questGump.AddPage(1);

	//Objectives
	switch (pUser.GetTag("QuestTracker")) 
	{
		case "1":TriggerEvent(20000, "questobjective", questGump);break;
		case "2":TriggerEvent(20001, "questobjective", questGump);break;
	}

	//Progress
	switch (pUser.GetTag("QuestTracker"))
	{
		case "1":TriggerEvent(20000, "questprogress", questGump, pUser);break;
		case "2":TriggerEvent(20001, "questprogress", questGump, pUser);break;
	}

	//questGump.AddButton(55, 346, 9909, 9911, 1, 0, 1);
	//questGump.AddXMFHTMLGumpColor(82, 347, 50, 20, 1043354, false, false, 0x7FFF); // Previous
	//AddButton(317, 346, 9903, 9905, 0, GumpButtonType.Page, i);
	//AddHtmlLocalized(278, 347, 50, 20, 1043353, White, false, false); // Next

	questGump.Send(pUser); // send this gump to client now 
	questGump.Free(); // clear this gump from uox-memory 
}

function onGumpPress(socket, pButton, gumpData)
{
	var pUser = socket.currentChar;
	switch (pButton)
	{
		case 0:break;
		case 1:progress(pUser); break;
	}
}