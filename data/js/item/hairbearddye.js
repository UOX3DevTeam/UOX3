// This script handles dying of hairs and beards
function onUseChecked( pUser, iUsed )
{
	// Store iUsed as a custom property on pUser
	pUser.dyeItem = iUsed;
	var socket = pUser.socket;

	// Make sure item is in player's backpack, or disallow use
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner != pUser )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, socket.language )); // That item must be in your backpack before it can be used.
		return false;
	}

	// Are we dealing with hair dye, or beard dye?
	var itemToDye = null;
	var itemToDye2 = null;
	if( iUsed.morex == 0 ) // Hair only dye
	{
		// Find hair equipped on character
		itemToDye = pUser.FindItemLayer( 0x0B );
	}
	else if( iUsed.morex == 1 ) // Beard only dye
	{
		// Find beard equipped on character
		itemToDye = pUser.FindItemLayer( 0x10 );
	}
	else if( iUsed.morex == 2 ) // Hair/Beard dye
	{
		// Find Hair/Beard equipped on character
		itemToDye = pUser.FindItemLayer( 0x0B );
		itemToDye2 = pUser.FindItemLayer( 0x10 );
	}

	if( itemToDye != null || itemToDye2 != null )
	{
		if( iUsed.morey == 1 )
		{
			DisplayDyes( purpleSpecialDyes, 1, "*****", pUser );
		}
		else
		{
			DisplayDyes( brownDyes, 1, GetDictionaryEntry( 17103, socket.language ), pUser ); // Brown
		}
	}
	else
	{
		if( itemToDye == null && iUsed.morex == 0 )
		{
			// No hair!
			pUser.SysMessage( GetDictionaryEntry( 17100, socket.language )); // You have no hair to dye.
		}
		else if( itemToDye == null && iUsed.morex == 1 )
		{
			// No beard!
			pUser.SysMessage( GetDictionaryEntry( 17116, socket.language )); // You have no beard to dye.
		}
		else if( itemToDye == null && itemToDye2 == null )
		{
			// No hair or beard!
			pUser.SysMessage( GetDictionaryEntry( 17120, socket.language )); // You have no hair or beard to dye.
		}
	}
	return false;
}

const brownDyes = [
	1601, 1602, 1603, 1604, 1605, 1606, 1607, 1608, 1609, 1610,
	1611, 1612, 1613, 1614, 1615, 1616, 1617, 1618, 1619, 1620,
    1621, 1622, 1623, 1624, 1625, 1626];

const chestnutDyes = [
	1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637,
	1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648,
	1649, 1650, 1651, 1652];

const auburnDyes = [
	1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510,
	1511, 1512, 1513, 1514, 1515, 1515, 1517, 1518, 1519, 1520,
    1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528, 1529, 1530, 1531, 1532];

const blueDyes = [
	1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310,
	1311, 1312, 1313, 1314, 1313, 1313, 1317, 1318, 1319, 1320,
    1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331];

const greenDyes = [
	1410, 1411, 1412, 1414, 1414, 1414, 1414, 1417, 1418, 1419, 1420,
    1421, 1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431,
	1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441];

const redDyes = [
	1201, 1202, 1203, 1204, 1205, 1206, 1207, 1208, 1209, 1210,
	1211, 1212, 1213, 1214, 1215, 1212, 1217, 1218, 1219, 1220,
    1221, 1222, 1223, 1224];

const lightDyes = [
	2401, 2402, 2403, 2404, 2405, 2406, 2407, 2408, 2409, 2410,
	2411, 2424, 2413, 2414, 2415, 2424, 2417, 2418, 2419, 2420,
    2421, 2422, 2423, 2424, 2425, 2426, 2427, 2428];

const blondeDyes = [
	2212, 2213, 2214, 2215, 2216, 2217];

const blackDyes = [
	1102, 1103, 1104, 1105, 1106, 1107, 1108];

const beigeDyes = [
	1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116];

const goldenDyes = [
	1117, 1118, 1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126,
	1127, 1128, 1129, 1130, 1131, 1132];

const dkBrownDyes = [
	1133, 1134, 1135, 1136, 1137, 1138, 1139, 1140, 1141, 1142,
	1143, 1144, 1145, 1146, 1147, 1148];

const purpleSpecialDyes = [
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22];

const redSpecialDyes = [
	32, 33, 34, 35, 36, 37];

const orangeSpecialDyes = [
	38, 39, 40, 41, 42, 43, 44, 45, 46];

const yellowSpecialDyes = [
	54, 55, 56, 57];

const limeSpecialDyes = [
	62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72];

const dklimeSpecialDyes = [
	81, 82, 83];

const iceblueSpecialDyes = [
	89, 90, 91];

const icewhiteSpecialDyes = [
	1153, 1154, 1155];

function DisplayDyes( dyesArray, pageNum, color, pUser )
{
	var MAX_ROWS = 15;    	// maximum number of rows displayed in a page
	var COL_INC = 100;    	// increment column by this value when MAX_ROWS is reached
	var COL_SPACING = 15; 	// spacing between radio control and text
	var colNum = 160;    	// starting column number
	var rowOffset = 0;   	// starting row offset
	var myDyeGump = new Gump;

	ShowDyeMenu( myDyeGump, pUser );
	for ( var i = 0; i < dyesArray.length; i++ )
	{
		if( i > 0 && i % MAX_ROWS == 0 )
		{
			colNum += COL_INC;
			rowOffset = 0;
		}

		myDyeGump.AddPage( pageNum );
		myDyeGump.AddRadio( colNum, 30 + ( rowOffset * 20 ), 0xa92, 0, dyesArray[0] + i );
		myDyeGump.AddText( colNum + COL_SPACING, 30 + ( rowOffset * 20 ), dyesArray[i], color );
		rowOffset++;
	}
	myDyeGump.Send( pUser );
	myDyeGump.Free();
}

function ShowDyeMenu( myDyeGump, pUser )
{
	var socket = pUser.socket;
	var iUsed = pUser.dyeItem;
	myDyeGump.AddPage( 0 );
	myDyeGump.AddBackground( 0, 0, 420, 400, 5054 );
	myDyeGump.AddBackground( 10, 10, 400, 380, 3000 );
	if( iUsed.morex == 0 )
	{
		// Hair
		myDyeGump.AddText( 91, 10, 1000, GetDictionaryEntry( 17101, socket.language )); // Hair Color Selection Menu
		myDyeGump.AddText( 81, 361, 1000, GetDictionaryEntry( 17102, socket.language )); // Dye my hair this color!
	}
	else if( iUsed.morex == 1 )
	{
		// Beards
		myDyeGump.AddText( 91, 10, 1000, GetDictionaryEntry( 17117, socket.language ) ); // Beard Color Selection Menu
		myDyeGump.AddText( 81, 361, 1000, GetDictionaryEntry( 17118, socket.language ) ); // Dye my beard this color!
	}
	else if( iUsed.morex == 2 )
	{
		// Hair/Beards
		myDyeGump.AddText( 91, 10, 1000, GetDictionaryEntry( 17101, socket.language ) ); // Hair Color Selection Menu
		myDyeGump.AddText( 81, 361, 1000, GetDictionaryEntry( 17119, socket.language ) ); // Dye my hair and beard this color!
	}
	myDyeGump.AddBackground( 28, 30, 120, 315, 5054 );
	myDyeGump.AddButton( 251, 360, 0xf7, 1, 0, 1 );

	var menuSpecialDye = [12, 32, 38, 54, 62, 81, 89, 1153];
	var menuDye = [1601, 1627, 1501, 1301, 1401, 1201, 2401, 2212, 1101, 1109, 1117, 1133];
	var menuNames = [17103, 17104, 17105, 17106, 17107, 17108, 17109, 17110, 17111, 17112, 17113, 17114];

	if( iUsed.morey == 1 )
	{
		for( var i = 0; i < menuSpecialDye.length; i++ )
		{
			myDyeGump.AddButton( 40, 40 + ( i * 20 ), 0x0a9a, 1, 0, 2 + i );
			myDyeGump.AddText( 60, 40 + ( i * 20 ), menuSpecialDye[i], "*****" );
		}
	}
	else
	{
		for( var i = 0; i < menuDye.length; i++ ) 
		{
			myDyeGump.AddButton( 40, 40 + ( i * 20 ), 0x0a9a, 1, 0, 2 + i );
			myDyeGump.AddText( 60, 40 + ( i * 20 ), menuDye[i], GetDictionaryEntry( menuNames[i], socket.language ));
		}
	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var iUsed = pUser.dyeItem;

	// Verify that dye item still exists
	if( !ValidateObject( iUsed ))
		return;

	// Make sure dye item is STILL in player's backpack, or disallow use
	var itemOwner = GetPackOwner( iUsed, 0 );
	if( itemOwner == null || itemOwner != pUser )
	{
		pUser.SysMessage( GetDictionaryEntry( 1763, pSock.language )); // That item must be in your backpack before it can be used.
		return false;
	}

	var scriptID = 5040;
	var gumpID = scriptID + 0xffff;
	var colour = 0;
	var itemToDye = null;
	var itemToDye2 = null;
	if( iUsed.morex == 0 )
	{
		// Find hair equipped on character
		itemToDye = pUser.FindItemLayer( 0x0B );
	}
	else if( iUsed.morex == 1 )
	{
		// Find beard equipped on character
		itemToDye = pUser.FindItemLayer( 0x10 );
	}
	else if( iUsed.morex == 2 )
	{
		// Find Hair/beard equipped on character
		itemToDye = pUser.FindItemLayer( 0x0B );
		itemToDye2 = pUser.FindItemLayer( 0x10 );
	}

	// Verify that hair/beard still exists
	if( !ValidateObject( itemToDye ) && !ValidateObject( itemToDye2 ))
		return;

	switch( pButton )
	{
		case 0:
			// abort and do nothing
			break;
		case 1:
			var OtherButton = gumpData.getButton( 0 );
			switch( OtherButton )
			{
				case 12:
                    colour = 0x12; break;
				case 13:
                    colour = 0x13; break;
				case 14:
                    colour = 0x14; break;
				case 15:
                    colour = 0x15; break;
				case 16:
                    colour = 0x16; break;
				case 17:
                    colour = 0x17; break;
				case 18:
                    colour = 0x18; break;
				case 19:
                    colour = 0x19; break;
				case 20:
                    colour = 0x1A; break;
				case 21:
                    colour = 0x1B; break;
				case 22:
                    colour = 0x1C; break;
				case 32:
                    colour = 0x20; break;
				case 33:
                    colour = 0x21; break;
				case 34:
                    colour = 0x22; break;
				case 35:
                    colour = 0x23; break;
				case 36:
                    colour = 0x24; break;
				case 37:
                    colour = 0x25; break;
				case 38:
                    colour = 0x26; break;
				case 39:
                    colour = 0x27; break;
				case 40:
                    colour = 0x28; break;
				case 41:
                    colour = 0x29; break;
				case 42:
                    colour = 0x2A; break;
				case 43:
                    colour = 0x2B; break;
				case 44:
                    colour = 0x2C; break;
				case 45:
                    colour = 0x2D; break;
				case 46:
                    colour = 0x2E; break;
				case 54:
                    colour = 0x36; break;
				case 55:
                    colour = 0x37; break;
				case 56:
                    colour = 0x38; break;
				case 57:
                    colour = 0x39; break;
				case 62:
                    colour = 0x3E; break;
				case 63:
                    colour = 0x3F; break;
				case 64:
                    colour = 0x40; break;
				case 65:
                    colour = 0x41; break;
				case 66:
                    colour = 0x42; break;
				case 67:
                    colour = 0x43; break;
				case 68:
                    colour = 0x44; break;
				case 69:
                    colour = 0x45; break;
				case 70:
                    colour = 0x46; break;
				case 71:
                    colour = 0x47; break;
				case 72:
                    colour = 0x48; break;
				case 81:
                    colour = 0x51; break;
				case 82:
                    colour = 0x52; break;
				case 83:
                    colour = 0x53; break;
				case 89:
                    colour = 0x59; break;
				case 90:
                    colour = 0x5A; break;
				case 91:
                    colour = 0x5B; break;
				case 1153:
                    colour = 0x481; break;
				case 1154:
                    colour = 0x482; break;
				case 1155:
                    colour = 0x483; break;
				case 1601:
					colour = 0x641; break;
				case 1602:
					colour = 0x642; break;
				case 1603:
					colour = 0x643; break;
				case 1604:
					colour = 0x644; break;
				case 1605:
					colour = 0x645; break;
				case 1606:
					colour = 0x646; break;
				case 1607:
					colour = 0x647; break;
				case 1608:
					colour = 0x648; break;
				case 1609:
					colour = 0x649; break;
				case 1610:
					colour = 0x64A; break;
				case 1611:
					colour = 0x64B; break;
				case 1612:
					colour = 0x64C; break;
				case 1613:
					colour = 0x64D; break;
				case 1614:
					colour = 0x64E; break;
				case 1615:
					colour = 0x64F; break;
				case 1616:
					colour = 0x650; break;
				case 1617:
					colour = 0x651; break;
				case 1618:
					colour = 0x652; break;
				case 1619:
					colour = 0x653; break;
				case 1620:
					colour = 0x654; break;
				case 1621:
					colour = 0x655; break;
				case 1622:
					colour = 0x656; break;
				case 1623:
					colour = 0x657; break;
				case 1624:
					colour = 0x658; break;
				case 1625:
					colour = 0x659; break;
				case 1626:
					colour = 0x65A; break;
				//Start Chestnut
				case 1627:
					colour = 0x65B; break;
				case 1628:
					colour = 0x65C; break;
				case 1629:
					colour = 0x65D; break;
				case 1630:
					colour = 0x65E; break;
				case 1631:
					colour = 0x65F; break;
				case 1632:
					colour = 0x660; break;
				case 1633:
					colour = 0x661; break;
				case 1634:
					colour = 0x662; break;
				case 1635:
					colour = 0x663; break;
				case 1636:
					colour = 0x664; break;
				case 1637:
					colour = 0x665; break;
				case 1638:
					colour = 0x666; break;
				case 1639:
					colour = 0x667; break;
				case 1640:
					colour = 0x668; break;
				case 1641:
					colour = 0x669; break;
				case 1642:
					colour = 0x66A; break;
				case 1643:
					colour = 0x66B; break;
				case 1644:
					colour = 0x66C; break;
				case 1645:
					colour = 0x66D; break;
				case 1646:
					colour = 0x66E; break;
				case 1647:
					colour = 0x66F; break;
				case 1648:
					colour = 0x670; break;
				case 1649:
					colour = 0x671; break;
				case 1650:
					colour = 0x672; break;
				case 1651:
					colour = 0x673; break;
				case 1652:
					colour = 0x674; break;
				//Start Auburn
				case 1501:
					colour = 0x5DD; break;
				case 1502:
					colour = 0x5DE; break;
				case 1503:
					colour = 0x5DF; break;
				case 1504:
					colour = 0x5E0; break;
				case 1505:
					colour = 0x5E1; break;
				case 1506:
					colour = 0x5E2; break;
				case 1507:
					colour = 0x5E3; break;
				case 1508:
					colour = 0x5E4; break;
				case 1509:
					colour = 0x5E5; break;
				case 1510:
					colour = 0x5E6; break;
				case 1511:
					colour = 0x5E7; break;
				case 1512:
					colour = 0x5E8; break;
				case 1513:
					colour = 0x5E9; break;
				case 1514:
					colour = 0x5EA; break;
				case 1515:
					colour = 0x5EB; break;
				case 1516:
					colour = 0x5EC; break;
				case 1517:
					colour = 0x5ED; break;
				case 1518:
					colour = 0x5EE; break;
				case 1519:
					colour = 0x5DF; break;
				case 1520:
					colour = 0x5F0; break;
				case 1521:
					colour = 0x5F1; break;
				case 1522:
					colour = 0x5F2; break;
				case 1523:
					colour = 0x5F3; break;
				case 1524:
					colour = 0x5F4; break;
				case 1525:
					colour = 0x5F5; break;
				case 1526:
					colour = 0x5F6; break;
				case 1527:
					colour = 0x5F7; break;
				case 1528:
					colour = 0x5F8; break;
				case 1529:
					colour = 0x5F9; break;
				case 1530:
					colour = 0x5FA; break;
				case 1531:
					colour = 0x5FB; break;
				case 1532:
					colour = 0x5FC; break;
				//START Blue
				case 1301:
					colour = 0x515; break;
				case 1302:
					colour = 0x516; break;
				case 1303:
					colour = 0x517; break;
				case 1304:
					colour = 0x518; break;
				case 1305:
					colour = 0x519; break;
				case 1306:
					colour = 0x51A; break;
				case 1307:
					colour = 0x51B; break;
				case 1308:
					colour = 0x51C; break;
				case 1309:
					colour = 0x51D; break;
				case 1310:
					colour = 0x51E; break;
				case 1311:
					colour = 0x51F; break;
				case 1312:
					colour = 0x520; break;
				case 1313:
					colour = 0x521; break;
				case 1314:
					colour = 0x522; break;
				case 1315:
					colour = 0x523; break;
				case 1316:
					colour = 0x524; break;
				case 1317:
					colour = 0x525; break;
				case 1318:
					colour = 0x526; break;
				case 1319:
					colour = 0x527; break;
				case 1320:
					colour = 0x528; break;
				case 1321:
					colour = 0x529; break;
				case 1322:
					colour = 0x52A; break;
				case 1323:
					colour = 0x52B; break;
				case 1324:
					colour = 0x52C; break;
				case 1325:
					colour = 0x52D; break;
				case 1326:
					colour = 0x52E; break;
				case 1327:
					colour = 0x52F; break;
				case 1328:
					colour = 0x530; break;
				case 1329:
					colour = 0x531; break;
				case 1330:
					colour = 0x532; break;
				case 1331:
					colour = 0x533; break;
				case 1332:
					colour = 0x534; break;
				//START Green
				case 1410:
					colour = 0x582; break;
				case 1411:
					colour = 0x583; break;
				case 1412:
					colour = 0x584; break;
				case 1413:
					colour = 0x585; break;
				case 1414:
					colour = 0x586; break;
				case 1415:
					colour = 0x587; break;
				case 1416:
					colour = 0x588; break;
				case 1417:
					colour = 0x589; break;
				case 1418:
					colour = 0x58A; break;
				case 1419:
					colour = 0x58B; break;
				case 1420:
					colour = 0x58C; break;
				case 1421:
					colour = 0x58D; break;
				case 1422:
					colour = 0x58E; break;
				case 1423:
					colour = 0x58F; break;
				case 1424:
					colour = 0x590; break;
				case 1425:
					colour = 0x591; break;
				case 1426:
					colour = 0x592; break;
				case 1427:
					colour = 0x593; break;
				case 1428:
					colour = 0x594; break;
				case 1429:
					colour = 0x595; break;
				case 1430:
					colour = 0x596; break;
				case 1431:
					colour = 0x597; break;
				case 1432:
					colour = 0x598; break;
				case 1433:
					colour = 0x599; break;
				case 1434:
					colour = 0x59A; break;
				case 1435:
					colour = 0x59B; break;
				case 1436:
					colour = 0x59C; break;
				case 1437:
					colour = 0x59D; break;
				case 1438:
					colour = 0x59E; break;
				case 1439:
					colour = 0x59F; break;
				case 1440:
					colour = 0x5A0; break;
				case 1441:
					colour = 0x5A1; break;
				//START Red
				case 1201:
					colour = 0x4B1; break;
				case 1202:
					colour = 0x4B2; break;
				case 1203:
					colour = 0x4B3; break;
				case 1204:
					colour = 0x4B4; break;
				case 1205:
					colour = 0x4B5; break;
				case 1206:
					colour = 0x4B6; break;
				case 1207:
					colour = 0x4B7; break;
				case 1208:
					colour = 0x4B8; break;
				case 1209:
					colour = 0x4B9; break;
				case 1210:
					colour = 0x4BA; break;
				case 1211:
					colour = 0x4BB; break;
				case 1212:
					colour = 0x4BC; break;
				case 1213:
					colour = 0x4BD; break;
				case 1214:
					colour = 0x4BE; break;
				case 1215:
					colour = 0x4BF; break;
				case 1216:
					colour = 0x4C0; break;
				case 1217:
					colour = 0x4C1; break;
				case 1218:
					colour = 0x4C2; break;
				case 1219:
					colour = 0x4C3; break;
				case 1220:
					colour = 0x4C4; break;
				case 1221:
					colour = 0x4C5; break;
				case 1222:
					colour = 0x4C6; break;
				case 1223:
					colour = 0x4C7; break;
				case 1224:
					colour = 0x4C8; break;
				//START Light
				case 2401:
					colour = 0x961; break;
				case 2402:
					colour = 0x962; break;
				case 2403:
					colour = 0x963; break;
				case 2404:
					colour = 0x964; break;
				case 2405:
					colour = 0x965; break;
				case 2406:
					colour = 0x966; break;
				case 2407:
					colour = 0x967; break;
				case 2408:
					colour = 0x968; break;
				case 2409:
					colour = 0x969; break;
				case 2410:
					colour = 0x96A; break;
				case 2411:
					colour = 0x96B; break;
				case 2412:
					colour = 0x96C; break;
				case 2413:
					colour = 0x96D; break;
				case 2414:
					colour = 0x96E; break;
				case 2415:
					colour = 0x96F; break;
				case 2416:
					colour = 0x970; break;
				case 2417:
					colour = 0x971; break;
				case 2418:
					colour = 0x972; break;
				case 2419:
					colour = 0x973; break;
				case 2420:
					colour = 0x974; break;
				case 2421:
					colour = 0x975; break;
				case 2422:
					colour = 0x976; break;
				case 2423:
					colour = 0x977; break;
				case 2424:
					colour = 0x978; break;
				case 2425:
					colour = 0x979; break;
				case 2426:
					colour = 0x97A; break;
				case 2427:
					colour = 0x97B; break;
				case 2428:
					colour = 0x97C; break;
				//START Blonde
				case 2212:
					colour = 0x8A5; break;
				case 2213:
					colour = 0x8A5; break;
				case 2214:
					colour = 0x8A6; break;
				case 2215:
					colour = 0x8A7; break;
				case 2216:
					colour = 0x8A8; break;
				case 2217:
					colour = 0x8A9; break;
				//START Black
				case 1102:
					colour = 0x44E; break;
				case 1103:
					colour = 0x44F; break;
				case 1104:
					colour = 0x450; break;
				case 1105:
					colour = 0x451; break;
				case 1106:
					colour = 0x452; break;
				case 1107:
					colour = 0x453; break;
				case 1108:
					colour = 0x454; break;
				//START Beige
				case 1109:
					colour = 0x455; break;
				case 1110:
					colour = 0x456; break;
				case 1111:
					colour = 0x457; break;
				case 1112:
					colour = 0x458; break;
				case 1113:
					colour = 0x459; break;
				case 1114:
					colour = 0x45A; break;
				case 1115:
					colour = 0x45B; break;
				case 1116:
					colour = 0x45C; break;
				//START Golden
				case 1117:
					colour = 0x45D; break;
				case 1118:
					colour = 0x45E; break;
				case 1119:
					colour = 0x45F; break;
				case 1120:
					colour = 0x460; break;
				case 1121:
					colour = 0x461; break;
				case 1122:
					colour = 0x462; break;
				case 1123:
					colour = 0x463; break;
				case 1124:
					colour = 0x464; break;
				case 1125:
					colour = 0x465; break;
				case 1126:
					colour = 0x466; break;
				case 1127:
					colour = 0x467; break;
				case 1128:
					colour = 0x468; break;
				case 1129:
					colour = 0x469; break;
				case 1130:
					colour = 0x46A; break;
				case 1131:
					colour = 0x46B; break;
				case 1132:
					colour = 0x46C; break;
				//START DK Brown
				case 1133:
					colour = 0x46D; break;
				case 1134:
					colour = 0x46E; break;
				case 1135:
					colour = 0x46F; break;
				case 1136:
					colour = 0x470; break;
				case 1137:
					colour = 0x471; break;
				case 1138:
					colour = 0x472; break;
				case 1139:
					colour = 0x473; break;
				case 1140:
					colour = 0x474; break;
				case 1141:
					colour = 0x475; break;
				case 1142:
					colour = 0x476; break;
				case 1143:
					colour = 0x477; break;
				case 1144:
					colour = 0x478; break;
				case 1145:
					colour = 0x479; break;
				case 1146:
					colour = 0x47A; break;
				case 1147:
					colour = 0x47B; break;
				case 1148:
					colour = 0x47C; break;
			}
			break;
		case 2:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( purpleSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( brownDyes, 1, GetDictionaryEntry( 17103, pSock.language ), pUser ); // Brown
			}
			break;
		case 3:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( redSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( chestnutDyes, 1, GetDictionaryEntry( 17104, pSock.language ), pUser ); // Chestnut
			}
			break;
		case 4:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( orangeSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( auburnDyes, 1, GetDictionaryEntry( 17105, pSock.language ), pUser ); // Auburn
			}
			break;
		case 5:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( yellowSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( blueDyes, 1, GetDictionaryEntry( 17106, pSock.language ), pUser ); // Blue
			}
			break;
		case 6:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( limeSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( greenDyes, 1, GetDictionaryEntry( 17107, pSock.language ), pUser ); // Green
			}
			break;
		case 7:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( dklimeSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( redDyes, 1, GetDictionaryEntry( 17108, pSock.language ), pUser ); // Red
			}
			break;
		case 8:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( iceblueSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( lightDyes, 1, GetDictionaryEntry( 17109, pSock.language ), pUser ); // Light
			}
			break;
		case 9:
			pSock.CloseGump( gumpID, 0 );
			if( iUsed.morey == 1 )
			{
				DisplayDyes( icewhiteSpecialDyes, 1, "*****", pUser );
			}
			else
			{
				DisplayDyes( blondeDyes, 1, GetDictionaryEntry( 17110, pSock.language ), pUser ); // Blonde
			}
			break;
		case 10:
			pSock.CloseGump( gumpID, 0 );
			DisplayDyes( blackDyes, 1, GetDictionaryEntry( 17111, pSock.language ), pUser ); // Black
			break;
		case 11:
			pSock.CloseGump( gumpID, 0 );
			DisplayDyes( beigeDyes, 1, GetDictionaryEntry( 17112, pSock.language ), pUser ); // Beige
			break;
		case 12:
			pSock.CloseGump( gumpID, 0 );
			DisplayDyes( goldenDyes, 1, GetDictionaryEntry( 17113, pSock.language ), pUser ); // Golden
			break;
		case 13:
			pSock.CloseGump( gumpID, 0 );
			DisplayDyes( dkBrownDyes, 1, GetDictionaryEntry( 17114, pSock.language ), pUser ); // Dk Brown
			break;
	}

	if( colour != 0 )
	{
		if( ValidateObject( itemToDye ))
		{
			itemToDye.colour = colour;
		}

		if( ValidateObject( itemToDye2 ))
		{
			itemToDye2.colour = colour;
		}

		iUsed.Delete();
		pUser.SoundEffect( 0x4E, true );
		return;
	}
}