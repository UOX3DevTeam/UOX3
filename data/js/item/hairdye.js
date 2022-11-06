function onUseChecked(pUser, iUsed) 
{
	var socket = pUser.socket;
	socket.tempObj = iUsed;
	if( iUsed.container != null )
	{
		FindItemLayer = pUser.FindItemLayer( 0x0B );
		if( FindItemLayer != null )
		{
			display_dyes( brown_dyes, 1, GetDictionaryEntry( 17103, socket.language ), pUser ); // Brown
		}
		else 
		{
			pUser.SysMessage( GetDictionaryEntry( 17100, socket.language ) ); // You have no hair to dye.
			return false;
		}
	}
	else 
	{
		pUser.SysMessage( GetDictionaryEntry( 17115, socket.language ) ); // That must be in your pack for you to use it.
		return false;
	}
}

const brown_dyes = [
	1601, 1602, 1603, 1604, 1605, 1606, 1607, 1608, 1609, 1610,
	1611, 1612, 1613, 1614, 1615, 1616, 1617, 1618, 1619, 1620,
    1621, 1622, 1623, 1624, 1625, 1626];

const chestnut_dyes = [
	1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637,
	1638, 1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648,
	1649, 1650, 1651, 1652];

const auburn_dyes = [
	1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510,
	1511, 1512, 1513, 1514, 1515, 1515, 1517, 1518, 1519, 1520,
    1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528, 1529, 1530, 1531, 1532];

const blue_dyes = [
	1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310,
	1311, 1312, 1313, 1314, 1313, 1313, 1317, 1318, 1319, 1320,
    1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331];

const green_dyes = [
	1410, 1411, 1412, 1414, 1414, 1414, 1414, 1417, 1418, 1419, 1420,
    1421, 1422, 1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431,
	1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441];

const red_dyes = [
	1201, 1202, 1203, 1204, 1205, 1206, 1207, 1208, 1209, 1210,
	1211, 1212, 1213, 1214, 1215, 1212, 1217, 1218, 1219, 1220,
    1221, 1222, 1223, 1224];

const light_dyes = [
	2401, 2402, 2403, 2404, 2405, 2406, 2407, 2408, 2409, 2410,
	2411, 2424, 2413, 2414, 2415, 2424, 2417, 2418, 2419, 2420,
    2421, 2422, 2423, 2424, 2425, 2426, 2427, 2428];

const blonde_dyes = [
	2212, 2213, 2214, 2215, 2216, 2217];

const black_dyes = [
	1102, 1103, 1104, 1105, 1106, 1107, 1108];

const beige_dyes = [
	1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116];

const golden_dyes = [
	1117, 1118, 1119, 1120, 1121, 1122, 1123, 1124, 1125, 1126,
	1127, 1128, 1129, 1130, 1131, 1132];

const dkbrown_dyes = [
	1133, 1134, 1135, 1136, 1137, 1138, 1139, 1140, 1141, 1142,
	1143, 1144, 1145, 1146, 1147, 1148];

function display_dyes( dyes_arr, page_num, color, pUser ) 
{
	var MAX_ROWS = 15;    // maximum number of rows displayed in a page
	var COL_INC = 100;    // increment column by this value when MAX_ROWS is reached
	var COL_SPACING = 15; // spacing between radio control and text
	var col_num = 160;    // starting column number
	var row_offset = 0;   // starting row offset
	var myHairDye = new Gump;

	hairDye( myHairDye, pUser );
	for ( var i = 0; i < dyes_arr.length; i++ )
	{
		if ( i > 0 && i % MAX_ROWS == 0 )
		{
			col_num += COL_INC;
			row_offset = 0;
		}

		myHairDye.AddPage( page_num );
		myHairDye.AddRadio( col_num, 30 + (row_offset * 20 ), 0xa92, 0, dyes_arr[0] + i );
		myHairDye.AddText( col_num + COL_SPACING, 30 + (row_offset * 20), dyes_arr[i], color );
		row_offset++;
	}
	myHairDye.Send( pUser );
	myHairDye.Free();
}

function hairDye( myHairDye, pUser)
{
	var socket = pUser.socket;
	myHairDye.AddPage(0);
	myHairDye.AddBackground( 0, 0, 420, 400, 5054 );
	myHairDye.AddBackground( 10, 10, 400, 380, 3000 );
	myHairDye.AddText( 91, 10, 1000, GetDictionaryEntry( 17101, socket.language ) ); // Hair Color Selection Menu
	myHairDye.AddText( 81, 361, 1000, GetDictionaryEntry( 17102, socket.language ) ); // Dye my hair this color!
	myHairDye.AddBackground( 28, 30, 120, 315, 5054 );
	myHairDye.AddButton( 251, 360, 0xf7, 1, 0, 1 );

	var menuDye = [1601, 1627, 1501, 1301, 1401, 1201, 2401, 2212, 1101, 1109, 1117, 1133];
	var menuNames = [17103, 17104, 17105, 17106, 17107, 17108, 17109, 17110, 17111, 17112, 17113, 17114];

	for ( var i = 0; i < menuDye.length; i++ )
	{
		myHairDye.AddButton( 40, 40 + ( i * 20 ), 0x0a9a, 1, 0, 2 + i );
		myHairDye.AddText( 60, 40 + ( i * 20 ), menuDye[i], GetDictionaryEntry( menuNames[i], socket.language ) );
	}
}

function onGumpPress( pSock, pButton, gumpData )
{
	var bItem = pSock.tempObj;
	var pUser = pSock.currentChar;
	var scriptID = 5040;
	var gumpID = scriptID + 0xffff;
	switch( pButton )
	{
		case 0:
			// abort and do nothing
			break;
		case 1:
			FindItemLayer = pUser.FindItemLayer( 0x0B );
			var OtherButton = gumpData.getButton(0);
			switch ( OtherButton )
			{
				case 1601:
					FindItemLayer.colour = 0x641;
					bItem.Delete(); break;
				case 1602:
					FindItemLayer.colour = 0x642;
					bItem.Delete(); break;
				case 1603:
					FindItemLayer.colour = 0x643;
					bItem.Delete(); break;
				case 1604:
					FindItemLayer.colour = 0x644;
					bItem.Delete(); break;
				case 1605:
					FindItemLayer.colour = 0x645;
					bItem.Delete(); break;
				case 1606:
					FindItemLayer.colour = 0x646;
					bItem.Delete(); break;
				case 1607:
					FindItemLayer.colour = 0x647;
					bItem.Delete(); break;
				case 1608:
					FindItemLayer.colour = 0x648;
					bItem.Delete(); break;
				case 1609:
					FindItemLayer.colour = 0x649;
					bItem.Delete(); break;
				case 1610:
					FindItemLayer.colour = 0x64A;
					bItem.Delete(); break;
				case 1611:
					FindItemLayer.colour = 0x64B;
					bItem.Delete(); break;
				case 1612:
					FindItemLayer.colour = 0x64C;
					bItem.Delete(); break;
				case 1613:
					FindItemLayer.colour = 0x64D;
					bItem.Delete(); break;
				case 1614:
					FindItemLayer.colour = 0x64E;
					bItem.Delete(); break;
				case 1615:
					FindItemLayer.colour = 0x64F;
					bItem.Delete(); break;
				case 1616:
					FindItemLayer.colour = 0x650;
					bItem.Delete(); break;
				case 1617:
					FindItemLayer.colour = 0x651;
					bItem.Delete(); break;
				case 1618:
					FindItemLayer.colour = 0x652;
					bItem.Delete(); break;
				case 1619:
					FindItemLayer.colour = 0x653;
					bItem.Delete(); break;
				case 1620:
					FindItemLayer.colour = 0x654;
					bItem.Delete(); break;
				case 1621:
					FindItemLayer.colour = 0x655;
					bItem.Delete(); break;
				case 1622:
					FindItemLayer.colour = 0x656;
					bItem.Delete(); break;
				case 1623:
					FindItemLayer.colour = 0x657;
					bItem.Delete(); break;
				case 1624:
					FindItemLayer.colour = 0x658;
					bItem.Delete(); break;
				case 1625:
					FindItemLayer.colour = 0x659;
					bItem.Delete(); break;
				case 1626:
					FindItemLayer.colour = 0x65A;
					bItem.Delete(); break;
				//Start Chestnut
				case 1627:
					FindItemLayer.colour = 0x65B;
					bItem.Delete(); break;
				case 1628:
					FindItemLayer.colour = 0x65C;
					bItem.Delete(); break;
				case 1629:
					FindItemLayer.colour = 0x65D;
					bItem.Delete(); break;
				case 1630:
					FindItemLayer.colour = 0x65E;
					bItem.Delete(); break;
				case 1631:
					FindItemLayer.colour = 0x65F;
					bItem.Delete(); break;
				case 1632:
					FindItemLayer.colour = 0x660;
					bItem.Delete(); break;
				case 1633:
					FindItemLayer.colour = 0x661;
					bItem.Delete(); break;
				case 1634:
					FindItemLayer.colour = 0x662;
					bItem.Delete(); break;
				case 1635:
					FindItemLayer.colour = 0x663;
					bItem.Delete(); break;
				case 1636:
					FindItemLayer.colour = 0x664;
					bItem.Delete(); break;
				case 1637:
					FindItemLayer.colour = 0x665;
					bItem.Delete(); break;
				case 1638:
					FindItemLayer.colour = 0x666;
					bItem.Delete(); break;
				case 1639:
					FindItemLayer.colour = 0x667;
					bItem.Delete(); break;
				case 1640:
					FindItemLayer.colour = 0x668;
					bItem.Delete(); break;
				case 1641:
					FindItemLayer.colour = 0x669;
					bItem.Delete(); break;
				case 1642:
					FindItemLayer.colour = 0x66A;
					bItem.Delete(); break;
				case 1643:
					FindItemLayer.colour = 0x66B;
					bItem.Delete(); break;
				case 1644:
					FindItemLayer.colour = 0x66C;
					bItem.Delete(); break;
				case 1645:
					FindItemLayer.colour = 0x66D;
					bItem.Delete(); break;
				case 1646:
					FindItemLayer.colour = 0x66E;
					bItem.Delete(); break;
				case 1647:
					FindItemLayer.colour = 0x66F;
					bItem.Delete(); break;
				case 1648:
					FindItemLayer.colour = 0x670;
					bItem.Delete(); break;
				case 1649:
					FindItemLayer.colour = 0x671;
					bItem.Delete(); break;
				case 1650:
					FindItemLayer.colour = 0x672;
					bItem.Delete(); break;
				case 1651:
					FindItemLayer.colour = 0x673;
					bItem.Delete(); break;
				case 1652:
					FindItemLayer.colour = 0x674;
					bItem.Delete(); break;
				//Start Auburn
				case 1501:
					FindItemLayer.colour = 0x5DD;
					bItem.Delete(); break;
				case 1502:
					FindItemLayer.colour = 0x5DE;
					bItem.Delete(); break;
				case 1503:
					FindItemLayer.colour = 0x5DF;
					bItem.Delete(); break;
				case 1504:
					FindItemLayer.colour = 0x5E0;
					bItem.Delete(); break;
				case 1505:
					FindItemLayer.colour = 0x5E1;
					bItem.Delete(); break;
				case 1506:
					FindItemLayer.colour = 0x5E2;
					bItem.Delete(); break;
				case 1507:
					FindItemLayer.colour = 0x5E3;
					bItem.Delete(); break;
				case 1508:
					FindItemLayer.colour = 0x5E4;
					bItem.Delete(); break;
				case 1509:
					FindItemLayer.colour = 0x5E5;
					bItem.Delete(); break;
				case 1510:
					FindItemLayer.colour = 0x5E6;
					bItem.Delete(); break;
				case 1511:
					FindItemLayer.colour = 0x5E7;
					bItem.Delete(); break;
				case 1512:
					FindItemLayer.colour = 0x5E8;
					bItem.Delete(); break;
				case 1513:
					FindItemLayer.colour = 0x5E9;
					bItem.Delete(); break;
				case 1514:
					FindItemLayer.colour = 0x5EA;
					bItem.Delete(); break;
				case 1515:
					FindItemLayer.colour = 0x5EB;
					bItem.Delete(); break;
				case 1516:
					FindItemLayer.colour = 0x5EC;
					bItem.Delete(); break;
				case 1517:
					FindItemLayer.colour = 0x5ED;
					bItem.Delete(); break;
				case 1518:
					FindItemLayer.colour = 0x5EE;
					bItem.Delete(); break;
				case 1519:
					FindItemLayer.colour = 0x5DF;
					bItem.Delete(); break;
				case 1520:
					FindItemLayer.colour = 0x5F0;
					bItem.Delete(); break;
				case 1521:
					FindItemLayer.colour = 0x5F1;
					bItem.Delete(); break;
				case 1522:
					FindItemLayer.colour = 0x5F2;
					bItem.Delete(); break;
				case 1523:
					FindItemLayer.colour = 0x5F3;
					bItem.Delete(); break;
				case 1524:
					FindItemLayer.colour = 0x5F4;
					bItem.Delete(); break;
				case 1525:
					FindItemLayer.colour = 0x5F5;
					bItem.Delete(); break;
				case 1526:
					FindItemLayer.colour = 0x5F6;
					bItem.Delete(); break;
				case 1527:
					FindItemLayer.colour = 0x5F7;
					bItem.Delete(); break;
				case 1528:
					FindItemLayer.colour = 0x5F8;
					bItem.Delete(); break;
				case 1529:
					FindItemLayer.colour = 0x5F9;
					bItem.Delete(); break;
				case 1530:
					FindItemLayer.colour = 0x5FA;
					bItem.Delete(); break;
				case 1531:
					FindItemLayer.colour = 0x5FB;
					bItem.Delete(); break;
				case 1532:
					FindItemLayer.colour = 0x5FC;
					bItem.Delete(); break;
				//START Blue
				case 1301:
					FindItemLayer.colour = 0x515;
					bItem.Delete(); break;
				case 1302:
					FindItemLayer.colour = 0x516;
					bItem.Delete(); break;
				case 1303:
					FindItemLayer.colour = 0x517;
					bItem.Delete(); break;
				case 1304:
					FindItemLayer.colour = 0x518;
					bItem.Delete(); break;
				case 1305:
					FindItemLayer.colour = 0x519;
					bItem.Delete(); break;
				case 1306:
					FindItemLayer.colour = 0x51A;
					bItem.Delete(); break;
				case 1307:
					FindItemLayer.colour = 0x51B;
					bItem.Delete(); break;
				case 1308:
					FindItemLayer.colour = 0x51C;
					bItem.Delete(); break;
				case 1309:
					FindItemLayer.colour = 0x51D;
					bItem.Delete(); break;
				case 1310:
					FindItemLayer.colour = 0x51E;
					bItem.Delete(); break;
				case 1311:
					FindItemLayer.colour = 0x51F;
					bItem.Delete(); break;
				case 1312:
					FindItemLayer.colour = 0x520;
					bItem.Delete(); break;
				case 1313:
					FindItemLayer.colour = 0x521;
					bItem.Delete(); break;
				case 1314:
					FindItemLayer.colour = 0x522;
					bItem.Delete(); break;
				case 1315:
					FindItemLayer.colour = 0x523;
					bItem.Delete(); break;
				case 1316:
					FindItemLayer.colour = 0x524;
					bItem.Delete(); break;
				case 1317:
					FindItemLayer.colour = 0x525;
					bItem.Delete(); break;
				case 1318:
					FindItemLayer.colour = 0x526;
					bItem.Delete(); break;
				case 1319:
					FindItemLayer.colour = 0x527;
					bItem.Delete(); break;
				case 1320:
					FindItemLayer.colour = 0x528;
					bItem.Delete(); break;
				case 1321:
					FindItemLayer.colour = 0x529;
					bItem.Delete(); break;
				case 1322:
					FindItemLayer.colour = 0x52A;
					bItem.Delete(); break;
				case 1323:
					FindItemLayer.colour = 0x52B;
					bItem.Delete(); break;
				case 1324:
					FindItemLayer.colour = 0x52C;
					bItem.Delete(); break;
				case 1325:
					FindItemLayer.colour = 0x52D;
					bItem.Delete(); break;
				case 1326:
					FindItemLayer.colour = 0x52E;
					bItem.Delete(); break;
				case 1327:
					FindItemLayer.colour = 0x52F;
					bItem.Delete(); break;
				case 1328:
					FindItemLayer.colour = 0x530;
					bItem.Delete(); break;
				case 1329:
					FindItemLayer.colour = 0x531;
					bItem.Delete(); break;
				case 1330:
					FindItemLayer.colour = 0x532;
					bItem.Delete(); break;
				case 1331:
					FindItemLayer.colour = 0x533;
					bItem.Delete(); break;
				case 1332:
					FindItemLayer.colour = 0x534;
					bItem.Delete(); break;
				//START Green
				case 1410:
					FindItemLayer.colour = 0x582;
					bItem.Delete(); break;
				case 1411:
					FindItemLayer.colour = 0x583;
					bItem.Delete(); break;
				case 1412:
					FindItemLayer.colour = 0x584;
					bItem.Delete(); break;
				case 1413:
					FindItemLayer.colour = 0x585;
					bItem.Delete(); break;
				case 1414:
					FindItemLayer.colour = 0x586;
					bItem.Delete(); break;
				case 1415:
					FindItemLayer.colour = 0x587;
					bItem.Delete(); break;
				case 1416:
					FindItemLayer.colour = 0x588;
					bItem.Delete(); break;
				case 1417:
					FindItemLayer.colour = 0x589;
					bItem.Delete(); break;
				case 1418:
					FindItemLayer.colour = 0x58A;
					bItem.Delete(); break;
				case 1419:
					FindItemLayer.colour = 0x58B;
					bItem.Delete(); break;
				case 1420:
					FindItemLayer.colour = 0x58C;
					bItem.Delete(); break;
				case 1421:
					FindItemLayer.colour = 0x58D;
					bItem.Delete(); break;
				case 1422:
					FindItemLayer.colour = 0x58E;
					bItem.Delete(); break;
				case 1423:
					FindItemLayer.colour = 0x58F;
					bItem.Delete(); break;
				case 1424:
					FindItemLayer.colour = 0x590;
					bItem.Delete(); break;
				case 1425:
					FindItemLayer.colour = 0x591;
					bItem.Delete(); break;
				case 1426:
					FindItemLayer.colour = 0x592;
					bItem.Delete(); break;
				case 1427:
					FindItemLayer.colour = 0x593;
					bItem.Delete(); break;
				case 1428:
					FindItemLayer.colour = 0x594;
					bItem.Delete(); break;
				case 1429:
					FindItemLayer.colour = 0x595;
					bItem.Delete(); break;
				case 1430:
					FindItemLayer.colour = 0x596;
					bItem.Delete(); break;
				case 1431:
					FindItemLayer.colour = 0x597;
					bItem.Delete(); break;
				case 1432:
					FindItemLayer.colour = 0x598;
					bItem.Delete(); break;
				case 1433:
					FindItemLayer.colour = 0x599;
					bItem.Delete(); break;
				case 1434:
					FindItemLayer.colour = 0x59A;
					bItem.Delete(); break;
				case 1435:
					FindItemLayer.colour = 0x59B;
					bItem.Delete(); break;
				case 1436:
					FindItemLayer.colour = 0x59C;
					bItem.Delete(); break;
				case 1437:
					FindItemLayer.colour = 0x59D;
					bItem.Delete(); break;
				case 1438:
					FindItemLayer.colour = 0x59E;
					bItem.Delete(); break;
				case 1439:
					FindItemLayer.colour = 0x59F;
					bItem.Delete(); break;
				case 1440:
					FindItemLayer.colour = 0x5A0;
					bItem.Delete(); break;
				case 1441:
					FindItemLayer.colour = 0x5A1;
					bItem.Delete(); break;
				//START Red
				case 1201:
					FindItemLayer.colour = 0x4B1;
					bItem.Delete(); break;
				case 1202:
					FindItemLayer.colour = 0x4B2;
					bItem.Delete(); break;
				case 1203:
					FindItemLayer.colour = 0x4B3;
					bItem.Delete(); break;
				case 1204:
					FindItemLayer.colour = 0x4B4;
					bItem.Delete(); break;
				case 1205:
					FindItemLayer.colour = 0x4B5;
					bItem.Delete(); break;
				case 1206:
					FindItemLayer.colour = 0x4B6;
					bItem.Delete(); break;
				case 1207:
					FindItemLayer.colour = 0x4B7;
					bItem.Delete(); break;
				case 1208:
					FindItemLayer.colour = 0x4B8;
					bItem.Delete(); break;
				case 1209:
					FindItemLayer.colour = 0x4B9;
					bItem.Delete(); break;
				case 1210:
					FindItemLayer.colour = 0x4BA;
					bItem.Delete(); break;
				case 1211:
					FindItemLayer.colour = 0x4BB;
					bItem.Delete(); break;
				case 1212:
					FindItemLayer.colour = 0x4BC;
					bItem.Delete(); break;
				case 1213:
					FindItemLayer.colour = 0x4BD;
					bItem.Delete(); break;
				case 1214:
					FindItemLayer.colour = 0x4BE;
					bItem.Delete(); break;
				case 1215:
					FindItemLayer.colour = 0x4BF;
					bItem.Delete(); break;
				case 1216:
					FindItemLayer.colour = 0x4C0;
					bItem.Delete(); break;
				case 1217:
					FindItemLayer.colour = 0x4C1;
					bItem.Delete(); break;
				case 1218:
					FindItemLayer.colour = 0x4C2;
					bItem.Delete(); break;
				case 1219:
					FindItemLayer.colour = 0x4C3;
					bItem.Delete(); break;
				case 1220:
					FindItemLayer.colour = 0x4C4;
					bItem.Delete(); break;
				case 1221:
					FindItemLayer.colour = 0x4C5;
					bItem.Delete(); break;
				case 1222:
					FindItemLayer.colour = 0x4C6;
					bItem.Delete(); break;
				case 1223:
					FindItemLayer.colour = 0x4C7;
					bItem.Delete(); break;
				case 1224:
					FindItemLayer.colour = 0x4C8;
					bItem.Delete(); break;
				//START Light
				case 2401:
					FindItemLayer.colour = 0x961;
					bItem.Delete(); break;
				case 2402:
					FindItemLayer.colour = 0x962;
					bItem.Delete(); break;
				case 2403:
					FindItemLayer.colour = 0x963;
					bItem.Delete(); break;
				case 2404:
					FindItemLayer.colour = 0x964;
					bItem.Delete(); break;
				case 2405:
					FindItemLayer.colour = 0x965;
					bItem.Delete(); break;
				case 2406:
					FindItemLayer.colour = 0x966;
					bItem.Delete(); break;
				case 2407:
					FindItemLayer.colour = 0x967;
					bItem.Delete(); break;
				case 2408:
					FindItemLayer.colour = 0x968;
					bItem.Delete(); break;
				case 2409:
					FindItemLayer.colour = 0x969;
					bItem.Delete(); break;
				case 2410:
					FindItemLayer.colour = 0x96A;
					bItem.Delete(); break;
				case 2411:
					FindItemLayer.colour = 0x96B;
					bItem.Delete(); break;
				case 2412:
					FindItemLayer.colour = 0x96C;
					bItem.Delete(); break;
				case 2413:
					FindItemLayer.colour = 0x96D;
					bItem.Delete(); break;
				case 2414:
					FindItemLayer.colour = 0x96E;
					bItem.Delete(); break;
				case 2415:
					FindItemLayer.colour = 0x96F;
					bItem.Delete(); break;
				case 2416:
					FindItemLayer.colour = 0x970;
					bItem.Delete(); break;
				case 2417:
					FindItemLayer.colour = 0x971;
					bItem.Delete(); break;
				case 2418:
					FindItemLayer.colour = 0x972;
					bItem.Delete(); break;
				case 2419:
					FindItemLayer.colour = 0x973;
					bItem.Delete(); break;
				case 2420:
					FindItemLayer.colour = 0x974;
					bItem.Delete(); break;
				case 2421:
					FindItemLayer.colour = 0x975;
					bItem.Delete(); break;
				case 2422:
					FindItemLayer.colour = 0x976;
					bItem.Delete(); break;
				case 2423:
					FindItemLayer.colour = 0x977;
					bItem.Delete(); break;
				case 2424:
					FindItemLayer.colour = 0x978;
					bItem.Delete(); break;
				case 2425:
					FindItemLayer.colour = 0x979;
					bItem.Delete(); break;
				case 2426:
					FindItemLayer.colour = 0x97A;
					bItem.Delete(); break;
				case 2427:
					FindItemLayer.colour = 0x97B;
					bItem.Delete(); break;
				case 2428:
					FindItemLayer.colour = 0x97C;
					bItem.Delete(); break;
				//START Blonde
				case 2212:
					FindItemLayer.colour = 0x8A5;
					bItem.Delete(); break;
				case 2213:
					FindItemLayer.colour = 0x8A5;
					bItem.Delete(); break;
				case 2214:
					FindItemLayer.colour = 0x8A6;
					bItem.Delete(); break;
				case 2215:
					FindItemLayer.colour = 0x8A7;
					bItem.Delete(); break;
				case 2216:
					FindItemLayer.colour = 0x8A8;
					bItem.Delete(); break;
				case 2217:
					FindItemLayer.colour = 0x8A9;
					bItem.Delete(); break;
				//START Black
				case 1102:
					FindItemLayer.colour = 0x44E;
					bItem.Delete(); break;
				case 1103:
					FindItemLayer.colour = 0x44F;
					bItem.Delete(); break;
				case 1104:
					FindItemLayer.colour = 0x450;
					bItem.Delete(); break;
				case 1105:
					FindItemLayer.colour = 0x451;
					bItem.Delete(); break;
				case 1106:
					FindItemLayer.colour = 0x452;
					bItem.Delete(); break;
				case 1107:
					FindItemLayer.colour = 0x453;
					bItem.Delete(); break;
				case 1108:
					FindItemLayer.colour = 0x454;
					bItem.Delete(); break;
				//START Beige
				case 1109:
					FindItemLayer.colour = 0x455;
					bItem.Delete(); break;
				case 1110:
					FindItemLayer.colour = 0x456;
					bItem.Delete(); break;
				case 1111:
					FindItemLayer.colour = 0x457;
					bItem.Delete(); break;
				case 1112:
					FindItemLayer.colour = 0x458;
					bItem.Delete(); break;
				case 1113:
					FindItemLayer.colour = 0x459;
					bItem.Delete(); break;
				case 1114:
					FindItemLayer.colour = 0x45A;
					bItem.Delete(); break;
				case 1115:
					FindItemLayer.colour = 0x45B;
					bItem.Delete(); break;
				case 1116:
					FindItemLayer.colour = 0x45C;
					bItem.Delete(); break;
				//START Golden
				case 1117:
					FindItemLayer.colour = 0x45D;
					bItem.Delete(); break;
				case 1118:
					FindItemLayer.colour = 0x45E;
					bItem.Delete(); break;
				case 1119:
					FindItemLayer.colour = 0x45F;
					bItem.Delete(); break;
				case 1120:
					FindItemLayer.colour = 0x460;
					bItem.Delete(); break;
				case 1121:
					FindItemLayer.colour = 0x461;
					bItem.Delete(); break;
				case 1122:
					FindItemLayer.colour = 0x462;
					bItem.Delete(); break;
				case 1123:
					FindItemLayer.colour = 0x463;
					bItem.Delete(); break;
				case 1124:
					FindItemLayer.colour = 0x464;
					bItem.Delete(); break;
				case 1125:
					FindItemLayer.colour = 0x465;
					bItem.Delete(); break;
				case 1126:
					FindItemLayer.colour = 0x466;
					bItem.Delete(); break;
				case 1127:
					FindItemLayer.colour = 0x467;
					bItem.Delete(); break;
				case 1128:
					FindItemLayer.colour = 0x468;
					bItem.Delete(); break;
				case 1129:
					FindItemLayer.colour = 0x469;
					bItem.Delete(); break;
				case 1130:
					FindItemLayer.colour = 0x46A;
					bItem.Delete(); break;
				case 1131:
					FindItemLayer.colour = 0x46B;
					bItem.Delete(); break;
				case 1132:
					FindItemLayer.colour = 0x46C;
					bItem.Delete(); break;
				//START Dk Brown
				case 1133:
					FindItemLayer.colour = 0x46D;
					bItem.Delete(); break;
				case 1134:
					FindItemLayer.colour = 0x46E;
					bItem.Delete(); break;
				case 1135:
					FindItemLayer.colour = 0x46F;
					bItem.Delete(); break;
				case 1136:
					FindItemLayer.colour = 0x470;
					bItem.Delete(); break;
				case 1137:
					FindItemLayer.colour = 0x471;
					bItem.Delete(); break;
				case 1138:
					FindItemLayer.colour = 0x472;
					bItem.Delete(); break;
				case 1139:
					FindItemLayer.colour = 0x473;
					bItem.Delete(); break;
				case 1140:
					FindItemLayer.colour = 0x474;
					bItem.Delete(); break;
				case 1141:
					FindItemLayer.colour = 0x475;
					bItem.Delete(); break;
				case 1142:
					FindItemLayer.colour = 0x476;
					bItem.Delete(); break;
				case 1143:
					FindItemLayer.colour = 0x477;
					bItem.Delete(); break;
				case 1144:
					FindItemLayer.colour = 0x478;
					bItem.Delete(); break;
				case 1145:
					FindItemLayer.colour = 0x479;
					bItem.Delete(); break;
				case 1146:
					FindItemLayer.colour = 0x47A;
					bItem.Delete(); break;
				case 1147:
					FindItemLayer.colour = 0x47B;
					bItem.Delete(); break;
				case 1148:
					FindItemLayer.colour = 0x47C;
					bItem.Delete(); break;
			}
			break;
		case 2:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", brown_dyes, 1, GetDictionaryEntry( 17103, pSock.language ), pUser ); // Brown
			break;
		case 3:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", chestnut_dyes, 1, GetDictionaryEntry( 17104, pSock.language ), pUser ); // Chestnut
			break;
		case 4:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", auburn_dyes, 1, GetDictionaryEntry( 17105, pSock.language ), pUser ); // Auburn
			break;
		case 5:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", blue_dyes, 1, GetDictionaryEntry( 17106, pSock.language ), pUser ); // Blue
			break;
		case 6:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", green_dyes, 1, GetDictionaryEntry( 17107, pSock.language ), pUser ); // Green
			break;
		case 7:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", red_dyes, 1, GetDictionaryEntry( 17108, pSock.language ), pUser ); // Red
			break;
		case 8:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", light_dyes, 1, GetDictionaryEntry( 17109, pSock.language ), pUser ); // Light
			break;
		case 9:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", blonde_dyes, 1, GetDictionaryEntry( 17110, pSock.language ), pUser ); // Blonde
			break;
		case 10:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", black_dyes, 1, GetDictionaryEntry( 17111, pSock.language ), pUser ); // Black
			break;
		case 11:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", beige_dyes, 1, GetDictionaryEntry( 17112, pSock.language ), pUser ); // Beige
			break;
		case 12:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", golden_dyes, 1, GetDictionaryEntry( 17113, pSock.language ), pUser ); // Golden
			break;
		case 13:
			pSock.CloseGump( gumpID, 0 );
			TriggerEvent( scriptID, "display_dyes", dkbrown_dyes, 1, GetDictionaryEntry( 17114, pSock.language ), pUser ); // Dk Brown
			break;
	}
}