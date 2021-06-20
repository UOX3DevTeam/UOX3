function onUseChecked(pUser, iUsed)
{
	if (iUsed.container != null)
	{
		var persMulti = FindMulti(pUser.x, pUser.y, pUser.z, pUser.worldnumber);
		var itemMulti = FindMulti(iUsed.x, iUsed.y, iUsed.z, iUsed.worldnumber);

		if (persMulti != itemMulti)
		{
			var ret = displaygump(iUsed, pUser);
		}
		else
			pUser.SysMessage("You must be in your house to do this.");
		return false;
	}
	else
		pUser.SysMessage("That must be in your pack for you to use it.");
	return false;
}

function displaygump(socket, pUser)
{
	var myGump = new Gump;
	myGump.AddPage(0);
	myGump.AddBackground(0, 0, 200, 200, 2600);
	myGump.AddButton(50, 45, 0x868, 1, 0, 1);
	myGump.AddXMFHTMLGump(90, 50, 70, 40, 1018323, false, false);//turn
	myGump.AddButton(50, 95, 0x868, 1, 0, 2);
	myGump.AddXMFHTMLGump(90, 100, 70, 40, 1018324, false, false);//up
	myGump.AddButton(50, 145, 0x868, 1, 0, 3);
	myGump.AddXMFHTMLGump(90, 150, 70, 40, 1018325, false, false);//down
	myGump.Send(pUser);
	myGump.Free();
	return false;
}

function onGumpPress(pSock, pButton, gumpData)
{
	var pUser = pSock.currentChar;
	switch (pButton)
	{
		case 0:
			break;//close
		case 1:
			pSock.CustomTarget(3);
			var ret = displaygump(pSock, pUser);
			break;//turn
		case 2:
			pSock.CustomTarget(2);
			var ret = displaygump(pSock, pUser);
			break;//Up
		case 3:
			pSock.CustomTarget(1);
			var ret = displaygump(pSock, pUser);
			break;//down
	}
}

function onCallback1(socket, ourObj)
{
	var tChar = socket.currentChar;
	if (!ValidateObject(ourObj) || ourObj.movable == 2)
	{
		socket.SysMessage("You cannot use the house decorator on that object.");
		return;
	}
	if (!ourObj.isItem)
		return;

	if (ourObj.z == 7 || ourObj.z == 27)
	{
		socket.SysMessage("You cannot lower it down any further.");
		return false;
	}
	if (ourObj.weight == 10000)
	{
		socket.SysMessage("That is too heavy.");
		return false;
	}
	if (ourObj.movable !== 3)
	{
		socket.SysMessage("That is not locked down.");
		return false;
	}
	socket.CustomTarget(1);
	if (!socket.GetWord(1))
		ourObj.z -= 1;
}

function onCallback2(socket, ourObj)
{
	var tChar = socket.currentChar;
	if (!ValidateObject(ourObj) || ourObj.movable == 2)
	{
		socket.SysMessage("You cannot use the house decorator on that object.");
		return;
	}
	if (!ourObj.isItem)
		return;

	if (ourObj.z == 22 || ourObj.z == 42)
	{
		socket.SysMessage("You cannot raise it up any higher.");
		return false;
	}
	if (ourObj.weight == 10000)
	{
		socket.SysMessage("That is too heavy.");
		return false;
	}
	if (ourObj.movable !== 3)
	{
		socket.SysMessage("That is not locked down.");
		return false;
	}
	socket.CustomTarget(2);
	if (!socket.GetWord(1))
		ourObj.z += 1;
}

function onCallback3(socket, ourObj)
{
	var tChar = socket.currentChar;
	if (!ValidateObject(ourObj) || ourObj.movable == 2)
	{
		socket.SysMessage("You cannot use the house decorator on that object.");
		return;
	}
	if (!ourObj.isItem)
		return;

	if (ourObj.weight == 10000)
	{
		socket.SysMessage("That is too heavy.");
		return false;
	}
	if (ourObj.movable !== 3)
	{
		socket.SysMessage("That is not locked down.");
		return false;
	}
	socket.CustomTarget(3);
	TriggerEvent(51, "onDrop", ourObj, tChar);
}