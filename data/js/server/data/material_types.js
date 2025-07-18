// This is a generic script for determining the (primary) type of material an item is composed of,
// such as "wood", "metal", "stone", "plant", etc. 
// Initially used to check for items made of metal for the purpose of smelting, but can also be
// used by other scripts for other purposes
//
// You can call this script from an external script by using the following line: 
//		var itemMaterialType = TriggerEvent( XXXX "GetItemMaterialType", itemObj );
//		var mapMaterialType = TriggerEvent( XXXX, "GetGroundMaterialType", mapTileID );

const itemTileIDList = [
	[ "metal", [
		// Weapons 
		0x08fe, 0x08fe, 0x08ff, 0x0901, 0x0902, 0x0903, 0x0904, 0x0907, 0x0908, 0x0909, 0x090a,
		0x090b, 0x0e85, 0x0e86, 0x0e87, 0x0e88, 0x0ec2, 0x0ec3, 0x0ec4, 0x0ec5, 0x0f39, 0x0f3a,
		0x0f43, 0x0f44, 0x0f45, 0x0f46, 0x0f47, 0x0f48, 0x0f49, 0x0f4a, 0x0f4b, 0x0f4c, 0x0f4d,
		0x0f4e, 0x0f4f, 0x0f50, 0x0f51, 0x0f52, 0x0f5c, 0x0f5d, 0x0f5e, 0x0f5f, 0x0f60, 0x0f61,
		0x0f62, 0x0f63, 0x0fb4, 0x0fb5, 0x13af, 0x13b0, 0x13b5, 0x13b6, 0x13b7, 0x13b8, 0x13b9,
		0x13ba, 0x13e3, 0x13e4, 0x13f6, 0x13f7, 0x13fa, 0x13fb, 0x13fe, 0x13ff, 0x1400, 0x1401,
		0x1402, 0x1403, 0x1404, 0x1405, 0x1406, 0x1407, 0x1438, 0x1439, 0x143a, 0x143b, 0x143c,
		0x143d, 0x143e, 0x143f, 0x1440, 0x1441, 0x1442, 0x1443, 0x26ba, 0x26bb, 0x26bc, 0x26bd,
		0x26be, 0x26bf, 0x26c0, 0x26c1, 0x26c4, 0x26c5, 0x26c6, 0x26c7, 0x26c8, 0x26c9, 0x26ca,
		0x26cb, 0x26ce, 0x26cf, 0x277d, 0x27a2, 0x27a3, 0x27a4, 0x27a6, 0x27a7, 0x27a9, 0x27ab,
		0x27ac, 0x27ad, 0x27af, 0x27ed, 0x27ee, 0x27ef, 0x27f1, 0x27f2, 0x27f4, 0x27f6, 0x27f7,
		0x27f8, 0x27fa, 0x2b06, 0x2b07, 0x2b08, 0x2b09, 0x2b0a, 0x2b0b, 0x2b0c, 0x2b0d, 0x2b0e,
		0x2b0f, 0x2b10, 0x2b11, 0x2b12, 0x2b13, 0x2d20, 0x2d21, 0x2d22, 0x2d23, 0x2d24, 0x2d26,
		0x2d27, 0x2d28, 0x2d29, 0x2d2c, 0x2d2d, 0x2d2e, 0x2d2f, 0x2d30, 0x2d32, 0x2d33, 0x2d34,
		0x2d35, 0x4067, 0x4068, 0x4069, 0x406a, 0x406b, 0x406c, 0x406d, 0x406e, 0x4072, 0x4074,
		0x4075, 0x4076, 0x48ae, 0x48af, 0x48b0, 0x48b1, 0x48b2, 0x48b3, 0x48b4, 0x48b5, 0x48b6,
		0x48b7, 0x48ba, 0x48bb, 0x48bc, 0x48bd, 0x48be, 0x48bf, 0x48c0, 0x48c1, 0x48c2, 0x48c3,
		0x48c4, 0x48c5, 0x48c6, 0x48c7, 0x48c8, 0x48c9, 0x48ca, 0x48cb, 0x48cc, 0x48cd, 0x48ce,
		0x48cf, 0x48d0, 0x48d1, 0x48d2, 0x48d3, 0xa341, 0xa342, 0xa345, 0xa346, 0x0fbb, 0x0fbc,
		0x1EBC, 0x0f9e, 0x0f9f, 0x08FD, 

		// Armor
		0x13bb, 0x13be, 0x13bf, 0x13c0, 0x13c3, 0x13c4, 0x13eb, 0x13ec, 0x13ed, 0x13ee, 0x13ef,
		0x13f0, 0x13f1, 0x13f2, 0x1408, 0x1409, 0x140a, 0x140b, 0x140c, 0x140d, 0x140e, 0x140f,
		0x1410, 0x1411, 0x1412, 0x1413, 0x1414, 0x1415, 0x1416, 0x1417, 0x1418, 0x1419, 0x141a,
		0x1c04, 0x1c05, 0x264b, 0x264c, 0x2689, 0x268a, 0x268d, 0x268e, 0x2774, 0x2775, 0x2777,
		0x2778, 0x2779, 0x2780, 0x2781, 0x2784, 0x2785, 0x2788, 0x2789, 0x278d, 0x27bf, 0x27c0,
		0x27c2, 0x27c3, 0x27c4, 0x27c8, 0x27cb, 0x27cc, 0x27cf, 0x27d0, 0x27d3, 0x27d4, 0x27d8,
		0x2b6e, 0x2b6f, 0x2b70, 0x3165, 0x3166, 0x3167, 0x404f, 0x4050, 0x4051, 0x4052, 0x4053,
		0x4054, 0x4055, 0x4056, 0x4210, 0x4211, 0x4212, 0x4213, 0x42de, 0x42df, 0x4d0a, 0x4d0b,
		0x9985, 0x9986, 0x236C, 0x236D, 0x2690, 0x268F,

		// Shields
		0x1b72, 0x1b73, 0x1b74, 0x1b75, 0x1b76, 0x1b77, 0x1b7b, 0x2b01, 0x4201, 0x4202, 0x4203,
		0x4204, 0x4206, 0x4208, 0x4209, 0x420a, 0xA649, 0xA64A, 0xA831, 0xA832, 0x7818, 0x7817,
		0x4228, 0x4229, 0x422A
	]],
	[ "leather", [
		// Armor
		0x13c5, 0x13c6, 0x13c7, 0x13cb, 0x13cc, 0x13cd, 0x13ce, 0x13d2, 0x13d3, 0x13d4, 0x13d5,
		0x13d6, 0x13da, 0x13db, 0x13dc, 0x13dd, 0x13e1, 0x13e2, 0x144e, 0x144f, 0x1454, 0x170b,
		0x170c, 0x170d, 0x170e, 0x170f, 0x1710, 0x1711, 0x1712, 0x1c00, 0x1c01, 0x1c02, 0x1c03,
		0x1c06, 0x1c07, 0x1c08, 0x1c09, 0x1c0a, 0x1c0b, 0x1c0c, 0x1c0d, 0x25e4, 0x25e5, 0x25e6,
		0x25e7, 0x25e8, 0x25e9, 0x2776, 0x277a, 0x277b, 0x277c, 0x277e, 0x277f, 0x2786, 0x2787,
		0x278a, 0x278b, 0x278e, 0x278f, 0x2790, 0x2791, 0x2792, 0x2793, 0x2796, 0x279d, 0x27c1,
		0x27c5, 0x27c6, 0x27c7, 0x27c9, 0x27ca, 0x27d1, 0x27d2, 0x27d5, 0x27d6, 0x27d9, 0x27da,
		0x27db, 0x27dc, 0x27dd, 0x27de, 0x27e1, 0x27e8, 0x2b02, 0x2b03, 0x2b68, 0x2b74, 0x2b75,
		0x2b76, 0x2b77, 0x2b78, 0x2b79, 0x2b74, 0x2b75, 0x2b76, 0x2b77, 0x2b78, 0x2b79, 0x2fb7,
		0x2fc3, 0x2fc4, 0x2fc5, 0x2fc6, 0x2fc7, 0x2fc8, 0x2fc9, 0x2fca, 0x2fcb, 0x315f, 0x316b,
		0x316c, 0x316d, 0x316e, 0x316f, 0x3170, 0x3171, 0x3179, 0x317a, 0x317b, 0x317c, 0x317d,
		0x317e, 0x317f, 0x3180, 0x3181, 0x4047, 0x4048, 0x4049, 0x404a, 0x404b, 0x404c, 0x404d,
		0x404e, 0x450d, 0x450e, 0x7822, 0x7823, 0x7824, 0x7825, 0x7826, 0x7827, 0x7828, 0x7829,
		0x782a, 0x782b, 0x782c, 0x782d, 0x782e, 0xa40c, 0xa40d, 0xa40e, 0xa40f, 0xAF05, 0x1F0B,
		0x1F0C, 0x2307, 0x2308, 0x2309, 0x230A, 0x230B, 0x230C, 0x2659, 0x265A, 0x265B, 0x265C,
		0x265D, 0x265E, 0x2691, 0x2692,

		// Weapons
		0xa289, 0xa28a, 0xa28b, 0xa291, 0xa292, 0xa293,
	]],
	[ "wood", [
		// Weapons
		0x0906, 0x0df0, 0x0df1, 0x0e81, 0x0e82, 0x0e89, 0x0e8a, 0x13b1, 0x13b2, 0x13b3, 0x13b4,
		0x13f4, 0x13f5, 0x13f8, 0x13f9, 0x13fc, 0x13fd, 0x26c2, 0x26c3, 0x26cc, 0x26cd, 0x27a5,
		0x27a8, 0x27aa, 0x27ae, 0x27f0, 0x27f3, 0x27f5, 0x27f9, 0x2d1e, 0x2d1f, 0x2d25, 0x2d2a,
		0x2d2b, 0x2d31, 0x406f, 0x48b8, 0x48b9, 0xa343, 0xa344, 0xa347, 0xa348,

		// Armor
		0x2b67, 0x2b69, 0x2b6a, 0x2b6b, 0x2b6c, 0x2b6d, 0x2b71, 0x2b72, 0x2b73, 0x315e, 0x3160,
		0x3161, 0x3162, 0x3163, 0x3164, 0x3168, 0x3169, 0x316a,


		// Shields
		0x1b78, 0x1b79, 0x1b7a, 0x4200, 0x4207,
	]],
	[ "cloth", [
		// Clothes
		0x1515, 0x1516, 0x1517, 0x1518, 0x152e, 0x152f, 0x1530, 0x1531, 0x1535, 0x1536, 0x1537,
		0x1538, 0x1539, 0x153a, 0x153b, 0x153c, 0x153d, 0x153e, 0x153f, 0x1540, 0x1541, 0x1542,
		0x1543, 0x1544, 0x1545, 0x1546, 0x1547, 0x1548, 0x1549, 0x154a, 0x154b, 0x154c, 0x1713,
		0x1714, 0x1715, 0x1716, 0x1717, 0x1718, 0x1719, 0x171a, 0x171b, 0x171c, 0x25ea, 0x25eb,
		0x25ec, 0x25ed, 0x267b, 0x267c, 0x267d, 0x267e, 0x267f, 0x2680, 0x2683, 0x2684, 0x26ad,
		0x26ae, 0x26af, 0x26b0, 0x2782, 0x2783, 0x2799, 0x279a, 0x279b, 0x279c, 0x27a0, 0x27a1,
		0x27cd, 0x27ce, 0x27df, 0x27e2, 0x27e3, 0x27e4, 0x27e5, 0x27e6, 0x27e7, 0x27eb, 0x27ec,
		0x2b04, 0x2b05, 0x2fb9, 0x2fba, 0x3173, 0x3174, 0x3175, 0x3176, 0x3177, 0x3178, 0x4000,
		0x4001, 0x4002, 0x4003, 0x46b4, 0x46b5, 0x4b9d, 0x4b9e, 0x4b9f, 0x4ba0, 0x7816, 0x7819,
		0x781a, 0x781b, 0x781c, 0x781e, 0x781f, 0x9eef, 0x9ef0, 0x9ef7, 0x9ef8, 0x9ef9, 0x9efa,
		0x9efb, 0x9efc, 0x9f3f, 0x9f40, 0xa0ab, 0xa0ac, 0xa0ad, 0xa0ae, 0xa0af, 0xa28d, 0xa28e,
		0xa294, 0xa295, 0xa28f, 0xa290, 0xa410, 0xa411, 0xa412, 0xa413, 0xA42B, 0xA1F6, 0x7821,
		0x781D, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040A, 0x172E, 0x1EFD,
		0x1EFE, 0x1EFF, 0x1F00, 0x1F01, 0x1F02, 0x1F03, 0x1F04, 0x1F9F, 0x1FA0, 0x1FA1, 0x1FA2,
		0x1FFD, 0x1FFE, 0x2305, 0x2306, 0x230D, 0x230E, 0x230F, 0x2310, 0x25F2, 0x25F3, 0x25F1,
		0x25F0, 0x25EF, 0x25EE, 0x2649, 0x264A, 0x264D, 0x264E, 0x264F, 0x2650, 0x2651, 0x2652,
		0x2653, 0x2654, 0x2655, 0x2656, 0x265F, 0x2660, 0x2661, 0x2662, 0x2663, 0x2664, 0x2665,
		0x2666, 0x2667, 0x2668, 0x266B, 0x266C, 0x266D, 0x266E, 0x2671, 0x2672, 0x2673, 0x2674,
		0x2677, 0x2678, 0x2679, 0x267A, 0x2681, 0x2682, 0x268B, 0x268C, 0x269D, 0x269E, 0x26A1,
		0x26A2, 0x26A3, 0x26A4, 0x1F7B, 0x1F7C,

		// Armor
		0x2794, 0x2797, 0x2798, 0x405f, 0x4060, 0x4061, 0x4062, 0x4063, 0x4064, 0x4065, 0x4066
	]],
	[ "scales", [
		// Armor
		0x2641, 0x2642, 0x2643, 0x2644, 0x2645, 0x2646, 0x2647, 0x2648, 0x2657, 0x2658
	]],
	[ "granite", [
		// Weapons
		0x0900, 0x4071,

		// Armor
		0x4057, 0x4058, 0x4059, 0x405a, 0x405b, 0x405c, 0x405d, 0x405e,

		// Shields
		0x4205, 0x420b
	]],
	[ "sand", [ 
		// Weapons
		0x0905, 0x090c, 0x4070, 0x4073
	]],
	["dragonturtlescute", [
		// Armors
		0x782E, 0x782D, 0x782C, 0x782B, 0x782A
	]],
	["tigerpelt", [
		// Armors
		0x7829, 0x7828, 0x7827, 0x7825, 0x7824, 0x7823, 0x7822
	]]
];

function GetItemMaterialType( itemToCheck, idToCheck )
{
	var itemTileID = 0;
	if( ValidateObject( itemToCheck ))
	{
		// Return custom material from item tag, if it exists
		if( itemToCheck.GetTag( "materialType" ))
		{
			return itemToCheck.GetTag( "materialType" );
		}

		// Otherwise return material based on item's ID
		itemTileID = itemToCheck.id;
	}
	else
	{
		itemTileID = idToCheck;
	}

	for( var i = 0; i < itemTileIDList.length; i++ )
	{
		for( var j = 0; j < itemTileIDList[i].length; j++ )
		{
			if( itemTileIDList[i][j].indexOf( itemTileID ) > -1 )
			{
				return itemTileIDList[i][0];
			}
		}
	}
	return "unknown";
}

const resourceTileIDList = [
	[ "metal", [
		0x1bf2, 0x1bef
	]],
	[ "leather", [
		0x1067, 0x1068, 0x1078, 0x1079, 0x1081, 0x1082
	]],
	[ "wood", [
		0x1bd7, 0x1bda, 0x1bdd, 0x1be0
	]],
	[ "logs", [
		0x1bdd, 0x1be0
	]],
	[ "cloth", [
		0x175d, 0x175e, 0x175f, 0x1760, 0x1761, 0x1762, 0x1763, 0x1764, 0x1765, 0x1766, 0x1767, 0x1768
	]],
	[ "bone", [
		0x0f7e
	]],
	[ "gems", [
		// Gems
		0x0f0f, 0x0f10, 0x0f11, 0x0f12, 0x0f13, 0x0f14, 0x0f15, 0x0f16, 0x0f17, 0x0f18, 0x0f19,
		0x0f1a, 0x0f1b, 0x0f1c, 0x0f1d, 0x0f1e, 0x0f1f, 0x0f20, 0x0f21, 0x0f22, 0x0f23, 0x0f24,
		0x0f25, 0x0f26, 0x0f27, 0x0f28, 0x0f29, 0x0f2a, 0x0f2b, 0x0f2c, 0x0f2d, 0x0f2e, 0x0f2f,
		0x0f30
	]]
	/*[ "scales", [
	]],
	[ "granite", [
	]],
	[ "sand", [
	]]*/
];

function GetResourceType( idToCheck )
{
	for( var i = 0; i < resourceTileIDList.length; i++ )
	{
		for( var j = 0; j < resourceTileIDList[i].length; j++ )
		{
			if( resourceTileIDList[i][j].indexOf( idToCheck ) > -1 )
			{
				return resourceTileIDList[i][0];
			}
		}
	}
	return "unknown";
}

const mapTileIDList = [
];

function GetGroundMaterialType( mapTileID )
{
	for( var i = 0; i < mapTileIDListList.length; i++ )
	{
		for( var j = 0; j < mapTileIDListList[i].length; j++ )
		{
			if( mapTileIDListList[i][j].indexOf( mapTileID ) > -1 )
			{
				return mapTileIDListList[i][0];
			}
		}
	}
	return "unknown";
}