// Valid map target IDs for dirt
const validMapDirtIDs = [0x9 ,0x15 ,0x71 ,0x7C ,0x82 ,0xA7 ,0xDC ,0xE3 ,0xE8 ,0xEB ||
	tileID == tileID == 0x141 ,0x144 ,0x14C ,0x15C ,0x169 ,0x174 ,0x1DC ,0x1EF ,0x272 ,0x275
	,0x27E ,0x281 ,0x2D0 ,0x2D7 ,0x2E5 ,0x2FF ,0x303 ,0x31F ,0x32C ,0x32F
	,0x33D ,0x340 ,0x345 ,0x34C ,0x355 ,0x358 ,0x367 ,0x36E ,0x377 ,0x37A
	,0x38D ,0x390 ,0x395 ,0x39C ,0x3A5 ,0x3A8 ,0x3F6 ,0x405 ,0x547 ,0x54E
	,0x553 ,0x556 ,0x597 ,0x59E ,0x623 ,0x63A ,0x6F3 ,0x6FA ,0x777 ,0x791
	,0x79A ,0x7A9 ,0x7AE ,0x7B1 ,0x98C ,0x99F ,0x9AC ,0x9BF ,0x5B27 ,0x5B3E ,0x71F4
	,0x71FB ,0x72C9 ,0x72CA
];

function onUseChecked(pUser, iBowl)
{
	var bowlOwner = GetPackOwner(iBowl, 0);
	if (bowlOwner == null || bowlOwner != pUser)
	{
		pUser.SysMessage("You must have the object in your backpack to use it."); //You must have the object in your backpack to use it.
	}
	else
	{
		pUser.socket.tempObj = iBowl;
		pUser.CustomTarget(1, "Choose a patch of dirt to scoop up.");//Choose a patch of dirt to scoop up.
	}
	return false;
}

function onCallback1(socket, ourObj)
{
	if (socket == null)
		return;

	var mChar = socket.currentChar;
	if (ValidateObject(mChar) && mChar.isChar)
	{
		var iBowl = socket.tempObj;
		var tileID = 0;
		var staticTile = true;

		if (socket.GetByte(1))
		{
			tileID = socket.GetWord(17);
			if (!tileID)
			{
				tileID = GetTileIDAtMapCoord(socket.GetWord(11), socket.GetWord(13), mChar.worldNumber);
				staticTile = false;
			}
		}
		else if (ValidateObject(ourObj) && ourObj.isItem)
		{
			// Make sure targeted object is in same world & instance as player
			if (ourObj.worldnumber != mChar.worldnumber || ourObj.instanceID != mChar.instanceID)
				return;

			tileID = ourObj.id;
			if (ourObj.id == 0x0f81)// Fertile Dirt
			{
				if (ourObj.amount < 40) //amount needed
				{
					socket.SysMessage("You need more dirt to fill a plant bowl!")
				}
				else 
				{
					var bowlofdirt = CreateDFNItem(mChar.socket, mChar, "plantbowlOfdirt", 1, "ITEM", true);
					bowlofdirt.SetTag("PlantInfo", 0 + "," + 0 + "," + 0 + "," + 1);
					bowlofdirt.SetTag("PlantStage", 14);
					socket.SysMessage("You fill the bowl with fresh dirt.")
					iBowl.Delete();

					if (ourObj.amount == 40)
						ourObj.Delete();
					else if (ourObj.amount > 40)
						ourObj.amount -= 40;
				}
			}
			else
				socket.SysMessage("You'll want to gather fresh dirt in order to raise a healthy plant!"); // "You'll want to gather fresh dirt in order to raise a healthy plant!"
				return;
		}

		var validTileIDFound = false;
		if (tileID != 0)
		{
			if (staticTile == true || (ourObj && ourObj.isItem)) // tileID is from a dynamic or static item
			{
				validTileIDFound = true;
			}
			else if (staticTile == false) // tileID is from a map tile
			{
				if (validMapDirtIDs.indexOf(tileID) != -1 ) // Dirt
				{
					var bowlofdirt = CreateDFNItem(mChar.socket, mChar, "plantbowlOfdirt", 1, "ITEM", true);
					bowlofdirt.SetTag("PlantInfo", 0 + "," + 0 + "," + 0 + "," + 0);
					bowlofdirt.SetTag("PlantStage", 14);//dirt
					socket.SysMessage("You fill the bowl with fresh dirt.")
					validTileIDFound = true;
					iBowl.Delete();
				}
			}
		}

		if (!validTileIDFound)
		{
			socket.SysMessage("You'll want to gather fresh dirt in order to raise a healthy plant!"); // "You'll want to gather fresh dirt in order to raise a healthy plant!"
		}
	}
}