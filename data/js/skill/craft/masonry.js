/// <reference path="../../definitions.d.ts" />
// @ts-check
const textHue = 0x480;					// Color of the text.
const masonryID = 4023;					//  Script ID used to identify and close this gump
const gumpDelay = 2000;					// Timer for the gump to reappear after crafting.
const ingotDelay = 200;					// Timer for the gump to reappear after selecting an ingot.
const repairDelay = 200;				// Timer for the gump to reappear after repairing an item
const craftGumpID = 4027;
const itemDetailsScriptID = 4026;

const itemsPerPage = 10;				// Number of craftable items shown per gump subpage
const displayUnlearnedRecipes = true;	// Show recipes player has not learned (if we add any later)
const coreShardEra = EraStringToNum( GetServerSetting( "CoreShardEra" ));

// If enabled, players can craft coloured variants of weapons, though unless the craftItems array
// is updated with specific create entries for the coloured weapon variants, they will just be
// regular weapons with granite colour applied
const allowColouredWeapons = GetServerSetting( "CraftColouredWeapons" );
const allowColouredBuildings = false; // Set to true if you want coloured stone walls, stairs, floors

// Optional: if you later decide to make some masonry items recipe-locked, we will use this map:
// MasonryMap[buttonID] = { dictID, page, timerID, graniteMake: [makeIDByOre], recipeID?, minEra?, maxEra? }
// o--------------------------------------------------------------------------o
// | Script        - masonry.js                                               |
// | System        - Masonry Crafting Gump							          |
// o--------------------------------------------------------------------------o
// | Purpose       -                                                          |
// |   Provides the blacksmith crafting menu using the same data-driven       |
// |   system used by the tailoring script.                                   |
// |                                                                          |
// |   All craftable items are defined in tables (myPage, craftItems) and     |
// |   then mapped into a MasonryMap structure that controls:                 |
// |     - Which dictionary entry is shown per row                            |
// |     - Which "makeID" entry is used for each ore type                     |
// |     - Which page and timer ID to use when reopening the gump             |
// |     - Optional per-item recipe and era requirements                      |
// |     - Optional per-item custom names for display                         |
// |                                                                          |
// |   The script also handles:                                               |
// |     - Granite selection (iron / colored ores) with skill requirements    |
// |     - Smelting stone items back into Granite                             |
// |     - Repairing stone armor and weapons at an anvil                      |
// |     - Tool wear and runic hammer handling                                |
// |     - A "Make Last" feature                                              |
// |     - A "Last Ten Masonry" list (optional)                               |
// o--------------------------------------------------------------------------o
// | Data Tables                                                              |
// o--------------------------------------------------------------------------o
// | myPage                                                                   |
// |   myPage[pageIndex] = [ dictID1, dictID2, ... ]                          |
// |     pageIndex 0 => Page 1: Metal Armor                                   |
// |     pageIndex 1 => Page 2: Helmets                                       |
// |     pageIndex 2 => Page 3: Shields                                       |
// |     pageIndex 3 => Page 4: Bladed weapons                                |
// |     pageIndex 4 => Page 5: Axes                                          |
// |     pageIndex 5 => Page 6: Polearms                                      |
// |     pageIndex 6 => Page 7: Bashing weapons                               |
// |                                                                          |
// |   Each entry is a dictionary ID that will be used to look up the text    |
// |   for that row, unless a customName is defined for that button in        |
// |   MasonryMap.                                                            |
// |                                                                          |
// | craftItems                                                               |
// |   craftItems[graniteIndex][pageIndex][itemIndex] = makeID                |
// |     graniteIndex 0 = Iron                                                |
// |     graniteIndex 1 = Dull Copper                                         |
// |     graniteIndex 2 = Shadow Iron                                         |
// |     graniteIndex 3 = Copper                                              |
// |     graniteIndex 4 = Bronze                                              |
// |     graniteIndex 5 = Gold                                                |
// |     graniteIndex 6 = Agapite                                             |
// |     graniteIndex 7 = Verite                                              |
// |     graniteIndex 8 = Valorite                                            |
// |                                                                          |
// |   For each granite type and page, this holds the createEntry ID used by  |
// |   MakeItem when the player crafts that item. The same index positions    |
// |   on each page line up with the matching entries in myPage.              |
// o--------------------------------------------------------------------------o
// | MasonryMap                                                            |
// o--------------------------------------------------------------------------o
// | MasonryMap is built automatically from myPage and craftItems.            |
// |                                                                          |
// |   MasonryMap[buttonID] = {                                               |
// |       dictID    : number,     // Base dictionary entry for the row       |
// |       page      : number,     // Main page (1..7, or 999 for Last Ten)   |
// |       timerID   : number,     // Timer ID to reopen same page            |
// |       graniteMake : number[],   // graniteMake[graniteIndex] = makeID    |
// |       customName: string?,    // Optional override for display text      |
// |       recipeID  : number?,    // Optional recipe requirement             |
// |       minEra    : string?,    // Optional minimum shard era              |
// |       maxEra    : string?     // Optional maximum shard era              |
// |   };                                                                     |
// |                                                                          |
// | Button ID mapping (same as original script):                             |
// |   Page 1 (Decorations) : 100..108                                        |
// |   Page 2 (Furniture)   : 200..205                                        |
// |   Page 3 (Statues)     : 300..305                                        |
// |   Page 4 (Misc Addons) : 400..407                                        |
// |   Page 5 (Stone Armor) : 500..506                                        |
// |   Page 6 (Stone Weapons) : 600..604                                      |
// |   Page 7 (Stone Walls): 700..704										  |
// |   Page 8 (Stone Stairs): 800..804										  |
// |   Page 9 (Stone Floors): 900..904										  |
// |                                                                          |
// | Custom Names                                                             |
// |   To override the display name for a specific row, set customName after  |
// |   the MasonryMap has been initialized, for example:                      |
// |                                                                          |
// |       MasonryMap[100].customName = "Hump vase";		                  |
// |                                                                          |
// |   PageX() will use this order of preference for text:                    |
// |     1. entry.customName (if set)                                         |
// |     2. GetDictionaryEntry(entry.dictID)                                  |
// |     3. A fallback "[Unnamed Item: buttonID]"                             |
// |                                                                          |
// | Recipes                                                                  |
// |   If recipeID is set on a MasonryMap entry, onGumpPress will call:       |
// |       TriggerEvent(4022, "NeedRecipe", pUser, recipeID)                  |
// |   to check if the player has learned that recipe. If not, the craft      |
// |   attempt is blocked and a message is shown.                             |
// |                                                                          |
// | Era Gating                                                               |
// |   The script reads the shard era using:                                  |
// |       const coreShardEra = EraStringToNum(GetServerSetting("CoreShardEra")); |
// |                                                                          |
// |   If an entry defines minEra or maxEra (strings like "lbr","aos","ml",   |
// |   "sa","hs","tol"), eraOK(entry) will ensure the current server era is   |
// |   within that range before allowing craft or display.                    |
// o--------------------------------------------------------------------------o
// | Notes                                                                    |
// o--------------------------------------------------------------------------o
// | - To add new Masonry items, update myPage and craftItems, then           |
// |   optionally decorate their MasonryMap entries with customName,          |
// |   recipeID, minEra, and maxEra.                                          |
// o--------------------------------------------------------------------------o

const myPage = [
	// Page 1 - Decorations
	[ 14050, 14051, 14052, 14053, 14054, 14055, 14056, 14057, 14058 ],
	// Page 2 - Furniture
	[ 14059, 14060, 14061, 14062, 14063, 14064 ],
	// Page 3 - Statues
	[ 14065, 14066, 14067, 14068, 14069, 14070 ],
	// Page 4 - Misc Addons
	[ 14071, 14072, 14073, 14074, 14075, 14076 ],
	// Page 5 - Stone Armor
	[ 14077, 14078, 14079, 14080, 14081, 14082, 14083, 14084, 14085, 14086 ],
	// Page 6 - Stone Weapons
	[ 14087 ],
	// Page 7 - Stone Walls
	[ 14088, 14089, 14090, 14091, 14092, 14093, 14094, 14095, 14096,14097, 14098, 14099 ],
	// Page 8 - Stone Stairs
	[ 14100, 14101, 14102, 14103, 14104, 14105 ],
	// Page 9 - Stone Floors
	[ 14106, 14107, 14108 ]
];

const craftItems = [
	// Iron
	[
		// Decorations
		[ 3500, 3501, 3502, 3503, 3504, 3505, 3506, 3507, 3508 ],
		// Furniture
		[ 3509, 3510, 3511, 3512, 3513, 3514 ],
		// Statues
		[ 3515, 3516, 3517, 3518, 3519, 3520 ],
		// Misc Addons
		[ 3521, 3522, 3523, 3524, 3525, 3526 ],
		// Stone Armor
		[ 3527, 3528, 3529, 3530, 3531, 3532, 3533, 3534, 3535, 3536 ],
		// Stone Weapons
		[ 3537 ],
		// Stone Walls
		[ 3538, 3539, 3540, 3541, 3542, 3543, 3544, 3545, 3546, 3547, 3548, 3549 ],
		// Stone Stairs
		[ 3550, 3551, 3552, 3553, 3554, 3555 ],
		// Stone Floors
		[ 3556, 3557, 3558]
	],

	// Dull Copper
	[
		// Decorations
		[ 3600, 3601, 3602, 3603, 3604, 3605, 3606, 3607, 3608 ],
		// Furniture
		[ 3609, 3610, 3611, 3612, 3613, 3614 ],
		// Statues
		[ 3615, 3616, 3617, 3618, 3619, 3620 ],
		// Misc Addons
		[ 3621, 3622, 3623, 3624, 3625, 3626 ],
		// Stone Armor
		[ 3627, 3628, 3629, 3630, 3631, 3632, 3633, 3634, 3635, 3636 ],
		// Stone Weapons
		[ 3637 ],
		// Stone Walls
		[ 3638, 3639, 3640, 3641, 3642, 3643, 3644, 3645, 3646, 3647, 3648, 3649 ],
		// Stone Stairs
		[ 3650, 3651, 3652, 3653, 3654, 3655 ],
		// Stone Floors
		[ 3656, 3657, 3658]
	],

	// Shadow Iron
	[
		// Decorations
		[ 3700, 3701, 3702, 3703, 3704, 3705, 3706, 3707, 3708 ],
		// Furniture
		[ 3709, 3710, 3711, 3712, 3713, 3714 ],
		// Statues
		[ 3715, 3716, 3717, 3718, 3719, 3720 ],
		// Misc Addons
		[ 3721, 3722, 3723, 3724, 3725, 3726 ],
		// Stone Armor
		[ 3727, 3728, 3729, 3730, 3731, 3732, 3733, 3734, 3735, 3736 ],
		// Stone Weapons
		[ 3737 ],
		// Stone Walls
		[ 3738, 3739, 3740, 3741, 3742, 3743, 3744, 3745, 3746, 3747, 3748, 3749 ],
		// Stone Stairs
		[ 3750, 3751, 3752, 3753, 3754, 3755 ],
		// Stone Floors
		[ 3756, 3757, 3758]
	],

	// Copper
	[
		// Decorations
		[ 3800, 3801, 3802, 3803, 3804, 3805, 3806, 3807, 3808 ],
		// Furniture
		[ 3809, 3810, 3811, 3812, 3813, 3814 ],
		// Statues
		[ 3815, 3816, 3817, 3818, 3819, 3820 ],
		// Misc Addons
		[ 3821, 3822, 3823, 3824, 3825, 3826 ],
		// Stone Armor
		[ 3827, 3828, 3829, 3830, 3831, 3832, 3833, 3834, 3835, 3836 ],
		// Stone Weapons
		[ 3837 ],
		// Stone Walls
		[ 3838, 3839, 3840, 3841, 3842, 3843, 3844, 3845, 3846, 3847, 3848, 3849 ],
		// Stone Stairs
		[ 3850, 3851, 3852, 3853, 3854, 3855 ],
		// Stone Floors
		[ 3856, 3857, 3858]
	],

	// Bronze
	[
		// Decorations
		[ 3900, 3901, 3902, 3903, 3904, 3905, 3906, 3907, 3908 ],
		// Furniture
		[ 3909, 3910, 3911, 3912, 3913, 3914 ],
		// Statues
		[ 3915, 3916, 3917, 3918, 3919, 3920 ],
		// Misc Addons
		[ 3921, 3922, 3923, 3924, 3925, 3926 ],
		// Stone Armor
		[ 3927, 3928, 3929, 3930, 3931, 3932, 3933, 3934, 3935, 3936 ],
		// Stone Weapons
		[ 3937 ],
		// Stone Walls
		[ 3938, 3939, 3940, 3941, 3942, 3943, 3944, 3945, 3946, 3947, 3948, 3949 ],
		// Stone Stairs
		[ 3950, 3951, 3952, 3953, 3954, 3955 ],
		// Stone Floors
		[ 3956, 3957, 3958]
	],

	// Gold
	[
		// Decorations
		[ 4000, 4001, 4002, 4003, 4004, 4005, 4006, 4007, 4008 ],
		// Furniture
		[ 4009, 4010, 4011, 4012, 4013, 4014 ],
		// Statues
		[ 4015, 4016, 4017, 4018, 4019, 4020 ],
		// Misc Addons
		[ 4021, 4022, 4023, 4024, 4025, 4026 ],
		// Stone Armor
		[ 4027, 4028, 4029, 4030, 4031, 4032, 4033, 4034, 4035, 4036 ],
		// Stone Weapons
		[ 4037 ],
		// Stone Walls
		[ 4038, 4039, 4040, 4041, 4042, 4043, 4044, 4045, 4046, 4047, 4048, 4049 ],
		// Stone Stairs
		[ 4050, 4051, 4052, 4053, 4054, 4055 ],
		// Stone Floors
		[ 4056, 4057, 4058]
	],

	// Agapite
	[
		// Decorations
		[ 4100, 4101, 4102, 4103, 4104, 4105, 4106, 4107, 4108 ],
		// Furniture
		[ 4109, 4110, 4111, 4112, 4113, 4114 ],
		// Statues
		[ 4115, 4116, 4117, 4118, 4119, 4120 ],
		// Misc Addons
		[ 4121, 4122, 4123, 4124, 4125, 4126 ],
		// Stone Armor
		[ 4127, 4128, 4129, 4130, 4131, 4132, 4133, 4134, 4135, 4136 ],
		// Stone Weapons
		[ 4137 ],
		// Stone Walls
		[ 4138, 4139, 4140, 4141, 4142, 4143, 4144, 4145, 4146, 4147, 4148, 4149 ],
		// Stone Stairs
		[ 4150, 4151, 4152, 4153, 4154, 4155 ],
		// Stone Floors
		[ 4156, 4157, 4158]
	],

	// Verite
	[
		// Decorations
		[ 4200, 4201, 4202, 4203, 4204, 4205, 4206, 4207, 4208 ],
		// Furniture
		[ 4209, 4210, 4211, 4212, 4213, 4214 ],
		// Statues
		[ 4215, 4216, 4217, 4218, 4219, 4220 ],
		// Misc Addons
		[ 4221, 4222, 4223, 4224, 4225, 4226 ],
		// Stone Armor
		[ 4227, 4228, 4229, 4230, 4231, 4232, 4233, 4234, 4235, 4236 ],
		// Stone Weapons
		[ 4237 ],
		// Stone Walls
		[ 4238, 4239, 4240, 4241, 4242, 4243, 4244, 4245, 4246, 4247, 4248, 4249 ],
		// Stone Stairs
		[ 4250, 4251, 4252, 4253, 4254, 4255 ],
		// Stone Floors
		[ 4256, 4257, 4258]
	],

	// Valorite
	[
		// Decorations
		[ 4300, 4301, 4302, 4303, 4304, 4305, 4306, 4307, 4308 ],
		// Furniture
		[ 4309, 4310, 4311, 4312, 4313, 4314 ],
		// Statues
		[ 4315, 4316, 4317, 4318, 4319, 4320 ],
		// Misc Addons
		[ 4321, 4322, 4323, 4324, 4325, 4326 ],
		// Stone Armor
		[ 4327, 4328, 4329, 4330, 4331, 4332, 4333, 4334, 4335, 4336 ],
		// Stone Weapons
		[ 4337 ],
		// Stone Walls
		[ 4338, 4339, 4340, 4341, 4342, 4343, 4344, 4345, 4346, 4347, 4348, 4349 ],
		// Stone Stairs
		[ 4350, 4351, 4352, 4353, 4354, 4355 ],
		// Stone Floors
		[ 4356, 4357, 4358]
	]
];

// MasonryMap[buttonID] = {
//     dictID: <dictionaryID>,
//     page: <pageNumber>,
//     timerID: <timerID>,
//     graniteMake: [ makeIDForIron, makeIDForDullCopper, ... ], // index is graniteID (0..8)
//     // Optional later:
//     // recipeID: <recipeID>,
//     // minEra: "lbr" / "aos" / "ml" / "sa" / "hs" / "tol",
//     // maxEra: ...
// };

const MasonryMap = {};

(function initMasonryMap()
{
	// graniteIndex: 0 = iron, 1 = dull copper, ... 8 = valorite
	for( var graniteIndex = 0; graniteIndex < craftItems.length; graniteIndex++ )
	{
		var graniteRows = craftItems[graniteIndex];

		// pageIdx: 0..6 => pages 1..9
		for( var pageIdx = 0; pageIdx < myPage.length; pageIdx++ )
		{
			var dictList = myPage[pageIdx];
			var makeList = graniteRows[pageIdx];

			for( var i = 0; i < dictList.length && i < makeList.length; i++ )
			{
				// Old script uses:
				// page 1 => 100..112
				// page 2 => 200..204
				// page 3 => 300..305
				// etc.
				var buttonID = ( ( pageIdx + 1 ) * 100 ) + i;
				var dictID = dictList[i];
				var makeID = makeList[i];

				if( !MasonryMap[buttonID] )
				{
					MasonryMap[buttonID] = {
						dictID: dictID,
						page: pageIdx + 1,
						timerID: pageIdx + 1,
						graniteMake: [],
						// recipeID: undefined,
						// minEra: undefined,
						// maxEra: undefined
						skill: 11,               // carpentry / masonry skill ID
						harvest: [14011],        // granite dict
						harvest2: [],             // optional second resource
						harvest3: [],             // optional second resource
						harvest4: []             // optional second resource
					};
				}

				MasonryMap[buttonID].graniteMake[graniteIndex] = makeID;
			}
		}
	}
})();

// 3) AFTER initMasonryMap, you can override entries:
// Page 1 Starts buttonID 100 - 107 for map example
//MasonryMap[100].customName = "Elven Broadsword";
//MasonryMap[100].recipeID = 5101;   // if you want it recipe-locked
//MasonryMap[100].minEra = "ml";     // if you want it ML and later only

//Page 1 starts at 100
MasonryMap[102].minEra = "se"; // small urn
MasonryMap[103].minEra = "se"; // Tower Sculpture
MasonryMap[104].minEra = "sa"; // gargoyle painting
MasonryMap[105].minEra = "sa"; // gargoyle sculpture
MasonryMap[106].minEra = "sa"; // gargoyle vase
MasonryMap[107].minEra = "tol"; // Tall 18th Anniversary Vase
MasonryMap[107].recipeID = 3500;
MasonryMap[108].minEra = "tol"; // Short 18th Anniversary Vase
MasonryMap[108].recipeID = 3501;
//Page 2 starts at 200
MasonryMap[205].minEra = "sa"; // ritual table
//Page 3 starts at 300
MasonryMap[304].minEra = "sa"; // gargoyle statue
MasonryMap[305].minEra = "sa"; // gryphon statue
//Page 4 starts at 400
MasonryMap[400].minEra = "ml"; // stone anvil (east)
MasonryMap[400].recipeID = 3520;
MasonryMap[401].minEra = "ml"; // stone anvil (south)
MasonryMap[401].recipeID = 3521;
MasonryMap[402].minEra = "sa"; // large gargish bed (east)
MasonryMap[402].harvest2 = [10016]; // Cloth
MasonryMap[403].minEra = "sa"; // large gargish bed (south)
MasonryMap[403].harvest2 = [10016]; // Cloth
MasonryMap[404].minEra = "sa"; // gargish cot (east)
MasonryMap[404].harvest2 = [10016]; // Cloth
MasonryMap[405].minEra = "sa"; // gargish cot (south)
MasonryMap[405].harvest2 = [10016]; // Cloth
//Page 5 starts at 500
MasonryMap[500].minEra = "sa"; // gargish stone arms
MasonryMap[501].minEra = "sa"; // gargish stone chest
MasonryMap[502].minEra = "sa"; // gargish stone leggings
MasonryMap[503].minEra = "sa"; // gargish stone kilt
MasonryMap[504].minEra = "sa"; // gargish stone arms
MasonryMap[505].minEra = "sa"; // gargish stone chest
MasonryMap[506].minEra = "sa"; // gargish stone leggings
MasonryMap[507].minEra = "sa"; // gargish stone kilt
MasonryMap[508].minEra = "sa"; // large stone shield
MasonryMap[509].minEra = "sa"; // gargish stone amulet
//Page 6 starts at 600
MasonryMap[600].minEra = "sa"; // stone war sword
//Page 7 starts at 700
MasonryMap[700].minEra = "tol"; // Rough Windowless
MasonryMap[701].minEra = "tol"; // Rough Window
MasonryMap[702].minEra = "tol"; // Rough Arch
MasonryMap[703].minEra = "tol"; // Rough Pillar
MasonryMap[704].minEra = "tol"; // Rough Rounded Arch
MasonryMap[705].minEra = "tol"; // Rough Small Arch
MasonryMap[706].minEra = "tol"; // Rough Angled Pillar
MasonryMap[707].minEra = "tol"; // Short Rough
MasonryMap[708].minEra = "tol"; // Stone Door (S In)
MasonryMap[709].minEra = "tol"; // Stone Door (E Out)
MasonryMap[710].minEra = "tol"; // Left Metal Door (S In)
MasonryMap[711].minEra = "tol"; // Right Metal Door (S In)
//Page 8 starts at 800
MasonryMap[800].minEra = "tol"; // short rough
MasonryMap[801].minEra = "tol"; // rough steps
MasonryMap[802].minEra = "tol"; // rough corner steps
MasonryMap[803].minEra = "tol"; // rough rounded corner step
MasonryMap[804].minEra = "tol"; // rough inset steps
MasonryMap[805].minEra = "tol"; // rough rounded inset steps
//Page 9 starts at 900
MasonryMap[900].minEra = "tol"; // light paver
MasonryMap[901].minEra = "tol"; // medium paver
MasonryMap[902].minEra = "tol"; // dark paver

function PageX( socket, pUser, pageNum )
{
	if( !ValidateObject( pUser ))
		return;

	// Pages 1 - 9: normal crafting pages
	// Page 999: optional "Last Ten Blacksmith" (if you decide to use it later)

	var subPage = pUser.GetTempTag( "subPage" );
	var pageItems = [];

	if( pageNum == 999 )
	{
		var lastTenRaw = pUser.GetTempTag( "LastTenMasonry" ) || "";
		var split = lastTenRaw.split( "," );
		for( var i = 0; i < split.length; i++ )
		{
			var val = parseInt( split[i] );
			if( !isNaN( val ) && MasonryMap[val] )
				pageItems.push( val ); // here val is the buttonID itself
		}
	}
	else
	{
		// Build list of buttonIDs for this page from MasonryMap
		for( var buttonID in MasonryMap )
		{
			var data = MasonryMap[buttonID];
			if( data.page == pageNum && eraOK( data ))
			{
				// If we later add recipes and want to hide unknown ones:
				var needsRecipe = data.recipeID;
				var showAll = displayUnlearnedRecipes;
				if( !needsRecipe || showAll || HasLearnedRecipe( pUser, needsRecipe ))
				{
					pageItems.push( parseInt( buttonID ) );
				}
			}
		}

		// Sort by buttonID to keep consistent ordering
		pageItems.sort( function( a, b ){ return a - b; } );
	}

	if( pageItems.length == 0 )
	{
		var emptyGump = new Gump;
		TriggerEvent( craftGumpID, "CraftingGumpMenu", emptyGump, socket );
		emptyGump.AddPage( 1 );
		emptyGump.AddText( 220, 60, textHue, "No items available on this page." );
		emptyGump.Send( socket );
		emptyGump.Free();
		return;
	}

	var totalSubPages = Math.ceil( pageItems.length / itemsPerPage );

	if( subPage < 1 )
		subPage = 1;
	if( subPage > totalSubPages )
		subPage = totalSubPages;

	pUser.SetTempTag( "page", pageNum );
	pUser.SetTempTag( "subPage", subPage );

	var startIndex = ( subPage - 1 ) * itemsPerPage;
	var endIndex = Math.min( startIndex + itemsPerPage, pageItems.length );

	if( startIndex >= pageItems.length )
	{
		subPage = 1;
		startIndex = 0;
		endIndex = Math.min( itemsPerPage, pageItems.length );
		pUser.SetTempTag( "subPage", subPage );
	}

	var resourceHue = pUser.GetTempTag( "resourceHue" );
	var blacksmithMenu = new Gump;
	TriggerEvent( craftGumpID, "CraftingGumpMenu", blacksmithMenu, socket );
	blacksmithMenu.AddPage( 1 );

	var drawIndex = 0;

	for( var i = startIndex; i < endIndex; i++ )
	{
		var buttonID = pageItems[i];
		var data = MasonryMap[buttonID];

		// Do not show weapons when colored granited ingots selected and colored granited weapons are disabled
		if( !allowColouredWeapons && resourceHue > 0 && data.page == 6 )
			continue;

		// Do not show walls when colored granited ingots selected and colored granited walls are disabled
		if( !allowColouredBuildings && resourceHue > 0 && data.page > 6 )
			continue;

		var entryText = "";
		if( data.customName )
		{
			entryText = data.customName;
		}
		else if( data.dictID )
		{
			entryText = GetDictionaryEntry( data.dictID, socket.language );
			if( !entryText || entryText === "" )
				entryText = "[Missing EntryID: " + data.dictID + "]";
		}
		else
		{
			entryText = "[Unnamed Item: " + buttonID + "]";
		}

		// Same layout as tailoring: button, text, details button
		blacksmithMenu.AddButton( 220, 60 + ( drawIndex * 20 ), 4005, 4007, 1, 0, buttonID );
		blacksmithMenu.AddText( 255, 60 + ( drawIndex * 20 ), textHue, entryText );
		blacksmithMenu.AddButton( 480, 60 + ( drawIndex * 20 ), 4011, 4012, 1, 0, 2000 + buttonID );

		drawIndex++;
	}

	// Prev subpage
	if( subPage > 1 )
	{
		blacksmithMenu.AddButton( 220, 260, 4014, 4015, 1, 0, 8000 + ( subPage - 1 ));
		blacksmithMenu.AddHTMLGump( 255, 263, 100, 18, 0, 0,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );
	}

	// Next subpage
	if( subPage < totalSubPages )
	{
		blacksmithMenu.AddButton( 370, 260, 4005, 4007, 1, 0, 9000 + ( subPage + 1 ));
		blacksmithMenu.AddHTMLGump( 405, 263, 100, 18, 0, 0,
			"<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );
	}

	blacksmithMenu.Send( socket );
	blacksmithMenu.Free();
}

function Page20( socket, pUser )
{
	//granite Choices
	var myGump = new Gump;
	pUser.SetTempTag( "page", 20 );
	TriggerEvent( craftGumpID, "CraftingGumpMenu", myGump, socket );
	var iron 		= pUser.ResourceCount( 0x1779 );
	var bronze 		= pUser.ResourceCount( 0x1779, 0x06d6 );
	var copper 		= pUser.ResourceCount( 0x1779, 0x07dd );
	var agapite 	= pUser.ResourceCount( 0x1779, 0x0979 );
	var dullcopper 	= pUser.ResourceCount( 0x1779, 0x0973 );
	var gold 		= pUser.ResourceCount( 0x1779, 0x08a5 );
	var shadowiron 	= pUser.ResourceCount( 0x1779, 0x0966 );
	var valorite 	= pUser.ResourceCount( 0x1779, 0x08ab );
	var verite 		= pUser.ResourceCount( 0x1779, 0x089f );
	var myPage20 = [
		GetDictionaryEntry( 14011, socket.language ) + " (" + iron.toString() + ")",
		GetDictionaryEntry( 14012, socket.language ) + " (" + dullcopper.toString() + ")",
		GetDictionaryEntry( 14013, socket.language ) + " (" + shadowiron.toString() + ")",
		GetDictionaryEntry( 14014, socket.language ) + " (" + copper.toString() + ")",
		GetDictionaryEntry( 14015, socket.language ) + " (" + bronze.toString() + ")",
		GetDictionaryEntry( 14016, socket.language ) + " (" + gold.toString() + ")",
		GetDictionaryEntry( 14017, socket.language ) + " (" + agapite.toString() + ")",
		GetDictionaryEntry( 14018, socket.language ) + " (" + verite.toString() + ")",
		GetDictionaryEntry( 14019, socket.language ) + " (" + valorite.toString() + ")"
	];

	for( var i = 0; i < myPage20.length; i++ )
	{
		var index = i % 10;
		if( index == 0 )
		{
			if( i > 0 )
			{
				myGump.AddButton( 370, 260, 4005, 4007, 0, ( i / 10 ) + 1, 0 );
				myGump.AddHTMLGump( 405, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10100, socket.language ) + "</basefont>" );// NEXT PAGE
			}

			myGump.AddPage(( i / 10 ) + 1 );

			if( i > 0 )
			{
				myGump.AddButton( 220, 260, 4014, 4015, 0, i / 10, 0 );
				myGump.AddHTMLGump( 255, 263, 100, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10101, socket.language ) + "</basefont>" );// PREV PAGE
			}
		}

		myGump.AddButton( 220, 60 + ( index * 20), 4005, 4007, 1, 0, 1000 + i );
		myGump.AddText( 255, 60 + ( index * 20), textHue, myPage20[i] );
	}
	myGump.Send( socket );
	myGump.Free();
}

function FindNearbyAnvils( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return ( trgItem.id == 0x0faf || trgItem.id == 0x0fb0 || trgItem.id == 0x2dd5 || trgItem.id == 0x2dd6 );
}

function FindNearbyForges( pUser, trgItem, pSock )
{
	if( !ValidateObject( trgItem ) || !trgItem.isItem )
		return false;

	return (( trgItem.id >= 0x197a && trgItem.id <= 0x19a9 ) || trgItem.id == 0x0Fb1 || trgItem.id == 0x2db0 || trgItem.id == 0x2dd8 );
}

function SmeltTarget( pSock )
{
	pSock.CustomTarget( 1, GetDictionaryEntry( 440, pSock.language )); // What item would you like to smelt?
}

// Armor and weapons can be smelted back into ingots.
/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback1( pSock, ourObj )
{
	// Smelt item, get ingots in return
	var mChar = pSock.currentChar;

	if( !ValidateObject( ourObj ) || !ourObj.isItem )
	{
		// Targeted object is not an item that can be smelted
		mChar.SetTempTag( "prevActionResult", "CANTSMELT" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	var nearbyAnvil = AreaItemFunction( "FindNearbyAnvils", mChar, 3, pSock );
	var nearbyForge = AreaItemFunction( "FindNearbyForges", mChar, 3, pSock );
	if( nearbyForge == 0 || nearbyAnvil == 0)
	{
		// No forge nearby
		mChar.SetTempTag( "prevActionResult", "NOFORGEORANVIL" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	var creatorSerial = ourObj.creator;
	var entryMadeFrom = ourObj.entryMadeFrom;
	var createEntry;
	if( entryMadeFrom != null && entryMadeFrom != 0 )
	{
		createEntry = CreateEntries[entryMadeFrom];
	}
	if( createEntry != null && createEntry.id != ourObj.id )
	{
		createEntry = null;
	}

	var resourceName = "granite";
	var resourceAmount = 0;
	var maxResourceAmount = 1;
	var resourceHue = ourObj.colour;

	if( creatorSerial == -1 || entryMadeFrom == 0 || createEntry == null )
	{
		// Not a player-crafted item, return 1 ingot if item is made of metal
		var materialType = TriggerEvent( 2506, "GetItemMaterialType", ourObj );
		if( materialType == "metal" )
		{
			resourceAmount = 1;
		}
	}
	else
	{
		if( createEntry.avgMinSkill > mChar.skills.mining )
		{
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOSMELTSKILL" );
			mChar.StartTimer( gumpDelay, 1, true );
			return;
		}

		// Loop through resources used to craft item, see how many ingots were used
		var resourcesUsed = createEntry.resources;
		for( var i = 0; i < resourcesUsed.length; i++ )
		{
			var resource = resourcesUsed[i];
			var amountNeeded = resource[0];
			var colorNeeded = resource[1];
			var resourceIDs = resource[2];

			// Loop through list of resourceIDs that were valid for crafting this item, see if ANY
			// were a match for the resource we're trying to return
			for( var j = 0; j <= resourceIDs.length; j++ )
			{
				// If both resource needed matches up, and resource color matches up, go for it
				if( resourceIDs[j] == 0x1bf2 && colorNeeded == resourceHue )
				{
					maxResourceAmount = amountNeeded;
					break;
				}
			}
		}

		if( maxResourceAmount > 1 )
		{
			// Calculate amount of resources returned based on player's mining skill, item's wear and tear,
			// and amount of resources that went into making the item in the first place
			if ( ourObj.health >= 1 || ourObj.usesLeft >= 1 ) 
			{
				var healthPercentage = 0;
				if( ourObj.health >= 1 )
				{
					healthPercentage = Math.floor( ( ourObj.health * 100) / ourObj.maxhp );
				}

				var usesPercentage = 0;
				if( ourObj.usesLeft >= 1 ) 
				{
					usesPercentage = Math.floor( ( ourObj.usesLeft * 100 ) / ourObj.maxUses );
				}

				var itemPercentage = usesPercentage > 0 ? Math.min( healthPercentage, usesPercentage ) : healthPercentage;

				resourceAmount = Math.floor( ( maxResourceAmount * itemPercentage ) / 100 );
			}

			// Halve the amount of resources returned
			resourceAmount = Math.max( Math.floor( resourceAmount / 2 ), 1 );

			// Fetch player's Mining skill
			var playerSkill = mChar.skills.mining;

			// Based on player's Mining skill, return between 1 to maxResourceAmount
			resourceAmount = Math.min( Math.max( Math.floor( resourceAmount * ( playerSkill / 1000 )), 1 ), resourceAmount );
		}
		else
		{
			resourceAmount = 1;
		}
	}

	if( resourceAmount == 0 )
	{
		// Targeted object is not an item that can be smelted
		mChar.SetTempTag( "prevActionResult", "CANTSMELT" );
		mChar.StartTimer( ingotDelay, 1, true );
		return;
	}

	if( ourObj.isDyeable )
	{
		// Dyeable items should return regular iron ingots
		resourceHue = 0;
	}

	switch( resourceHue )
	{
		case 0: // Iron granite
		default:
			break;
		case 0x0973: // Dull Copper
			resourceName = "dull copper granite";
			break;
		case 0x0966: // Shadow Iron
			resourceName = "shadow iron granite";
			break;
		case 0x07dd: // Copper
			resourceName = "copper granite";
			break;
		case 0x06d6: // Bronze
			resourceName = "bronze granite";
			break;
		case 0x08a5: // Gold
			resourceName = "gold granite";
			break;
		case 0x0979: // Agapite
			resourceName = "agapite granite";
			break;
		case 0x089f: // Verite
			resourceName = "verite granite";
			break;
		case 0x08ab: // Valorite
			resourceName = "valorite granite";
			break;
	}

	// Delete the melted item
	ourObj.Delete();

	// Run a generic skill check to give player a chance to increase their mining skill
	mChar.CheckSkill( 45, 0, mChar.skillCaps.mining );

	var newResource = CreateDFNItem( pSock, mChar, "0x1779", resourceAmount, "ITEM", true, resourceHue );
	newResource.name = resourceName;

	mChar.SetTempTag( "ingotsFromSmelting", resourceAmount );
	mChar.SetTempTag( "prevActionResult", "SMELTITEMSUCCESS" );
	mChar.StartTimer( gumpDelay, 1, true );
}

function RepairTarget( pSock )
{
	pSock.CustomTarget( 2, GetDictionaryEntry( 485, pSock.language )); // What item would you like to repair?
}

/** @type { ( tSock: Socket, target: Character | Item | null ) => void } */
function onCallback2( pSock, ourObj )
{
	// Repair Item
	var mChar = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( mChar ) || mChar.dead )
		return;

	var bItem = pSock.tempObj;
	var anvil = AreaItemFunction( "FindNearbyAnvils", mChar, 3, pSock );
	var gumpID = masonryID + 0xffff;
	pSock.tempObj = null;

	if( ValidateObject( mChar ) && mChar.isChar && ValidateObject( bItem ) && bItem.isItem )
	{
		if( !ValidateObject( ourObj ) || !ourObj.isItem
			|| TriggerEvent( 2506, "GetItemMaterialType", ourObj ) != "metal"
			|| !CheckTileFlag( ourObj.id, 22 )) // TF_WEARABLE
		{
			// Targeted object is not an item that can be repaired
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}

		if( anvil == 0 )
		{
			// No anvil nearby
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "NOANVIL" );
			mChar.StartTimer( repairDelay, 1, true );
			return;
		}

		var itemDurabilityLossEnabled = GetServerSetting( "ItemRepairDurabilityLoss" );
		var repairID = ourObj.id;
		var ownerObj = GetPackOwner( ourObj, 0 );
		if( ownerObj && mChar.serial == ownerObj.serial )
		{
			var maxHitpoints = ourObj.maxhp;
			var currentHitpoints = ourObj.health;
			if( currentHitpoints < maxHitpoints )
			{
				// Get base repair difficulty based on amount of HP missing and max hitpoints
				var deltaHP = maxHitpoints - currentHitpoints;
				var repairDifficulty = (( deltaHP / maxHitpoints ) * 1000 );
				var minDifficulty = repairDifficulty - 250;
				var skillBonus = 0;
				var repairSkill = mChar.skills.blacksmithing;
				if( minDifficulty < 0 )
				{
					// If minDifficulty is negative, add the negative value as a bonus to player's skill
					skillBonus = minDifficulty * -1;
					minDifficulty = 0;
				}
				else if( minDifficulty > repairSkill )
				{
					// Player skill below minimum repair difficulty, Too difficult to make the attempt!
					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "CANTREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
					return;
				}
				var maxDifficulty = Math.min( repairDifficulty + 250, mChar.skillCaps.blacksmithing );

				// Allow repair if random number between min and base difficulty is under player's skill
				if( RandomNumber( minDifficulty, 1000 ) < ( Math.max( repairSkill + skillBonus, 999 )))
				{
					// Give player a chance every now and then to gain skill from repairing
					if( RandomNumber( 1, 5 ) == 1 )
					{
						// Run a skill-check, which might trigger a skill-gain if player passes
						mChar.CheckSkill( 8, minDifficulty, maxDifficulty ); // Skill 8 = blacksmithing
					}

					// Reduce object's max durability by 1
					if( itemDurabilityLossEnabled )
					{
						ourObj.maxhp -= 1;
					}

					// Repair item here
					ourObj.health = ourObj.maxhp;
					pSock.SoundEffect( 0x002A, true );

					// Reopen gump after a short delay
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "REPAIRSUCCESS" );
					mChar.StartTimer( repairDelay, 1, true );

					// GM skill (100.0 skillpoints)
					// 	Item with 51 HP max
					// 		item with 2 hp left - 99.65% chance to repair
					// 		item with 25 hp left - 99.86% chance to repair
					// 		item with 40 hp left - 99.9% chance to repair

					// Expert Smith (71.5 skill points)
					//	Item with 51 HP max
					// 		item with 2 hp left - 1.45% chance to repair
					// 		item with 25 hp left - 61.49% chance to repair
					// 		item with 40 hp left - 74.9% chance to repair
					// 		item with 48 hp left - 90.6% chance to repair

					// Apprentice Smith (51.5 skill points)
					// 	Item with 51 HP max
					// 		item with 2 hp left - 0% chance to repair
					// 		item with 25 hp left - 34.5% chance to repair
					// 		item with 40 hp left - 54.9% chance to repair
					// 		item with 48 hp left - 70.6% chance to repair
				}
				else
				{
					// Failed to repair item - decrease item health!
					if( repairSkill >= 1000 ) // GM Smith
					{
						ourObj.health -= 1;
					}
					else if( repairSkill >= 715 ) // Expert Smith
					{
						ourObj.health -= 2;
					}
					else // Below Expert Smith
					{
						ourObj.health -= 3;
					}

					if( ourObj.health <= 0 )
					{
						// Item has been destroyed!
						pSock.SysMessage( GetDictionaryEntry( 311, pSock.language ).replace(/%s/gi, ourObj.name )); // Your %s has been destroyed.
						ourObj.Delete();
					}

					pSock.tempObj = bItem;
					pSock.CloseGump( gumpID, 0 );
					mChar.SetTempTag( "prevActionResult", "FAILREPAIR" );
					mChar.StartTimer( repairDelay, 1, true );
				}
			}
			else
			{
				pSock.tempObj = bItem;
				pSock.CloseGump( gumpID, 0 );
				mChar.SetTempTag( "prevActionResult", "FULLREPAIR" );
				mChar.StartTimer( repairDelay, 1, true );
			}
		}
		else
		{
			pSock.tempObj = bItem;
			pSock.CloseGump( gumpID, 0 );
			mChar.SetTempTag( "prevActionResult", "CHECKPACK" );
			mChar.StartTimer( repairDelay, 1, true );
		}
	}
}

/** @type { ( tObject: BaseObject, timerId: number ) => void } */
function onTimer( pUser, timerID )
{
	if( !ValidateObject( pUser ))
		return;

	var socket = pUser.socket;

	if( timerID >= 1 && timerID <= 9 )
	{
		PageX( socket, pUser, timerID ); // Pages 1 - 9
	}
	else if( timerID == 20 )
	{
		Page20( socket, pUser );          // Ingot selection
	}
	else if( timerID == 999 )
	{
		PageX( socket, pUser, 999 );     // Last Ten Blacksmith (if used)
	}
}


/** @type { ( myObj: Socket, pressed: number, gump: GumpData ) => void } */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var usedMakeLast = false;

	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = pSock.tempObj;
	if( !ValidateObject( bItem ) || !pUser.InRange( bItem, 3 ))
	{
		pSock.SysMessage( GetDictionaryEntry( 461, pSock.language )); // You are too far away.
		return;
	}

	if( bItem.movable == 3 )
	{
		pSock.SysMessage( GetDictionaryEntry( 6031, pSock.language )); // Locked down resources cannot be used!
		return;
	}

	var iPackOwner = GetPackOwner( bItem, 0 );
	if( ValidateObject( iPackOwner ))
	{
		if( iPackOwner.serial != pUser.serial )
		{
			pSock.SysMessage( GetDictionaryEntry( 6032, pSock.language )); // That resource is in someone else's backpack!
			return;
		}
	}
	else
	{
		pSock.SysMessage( GetDictionaryEntry( 6022, pSock.language )); // This has to be in your backpack befgranite you can use it.
		return;
	}

	var gumpID = masonryID + 0xffff;

	// Close / Exit
	if( pButton == 0 )
	{
		pUser.SetTempTag( "prevActionResult", null );
		pUser.SetTempTag( "MAKELAST", null );
		pSock.CloseGump( gumpID, 0 );
		return;
	}

	// Repair Item
	if( pButton == 49 )
	{
		RepairTarget( pSock );
		return;
	}

	// Select Materials (ingots)
	if( pButton == 50 )
	{
		pSock.CloseGump( gumpID, 0 );
		Page20( pSock, pUser );
		return;
	}

	// Smelt Item
	if( pButton == 52 )
	{
		SmeltTarget( pSock );
		return;
	}

	// Page buttons (1..9)
	if( pButton >= 1 && pButton <= 9 )
	{
		pUser.SetTempTag( "page", pButton );
		pUser.SetTempTag( "subPage", 1 );
		pSock.CloseGump( gumpID, 0 );
		PageX( pSock, pUser, pButton );
		return;
	}

	// Last Ten page (if you wire it into the gump)
	if( pButton == 11000 )
	{
		pUser.SetTempTag( "page", 999 );
		pUser.SetTempTag( "subPage", 1 );
		PageX( pSock, pUser, 999 );
		return;
	}

	// Subpage navigation (8000 = prev, 9000 = next)
	if( pButton >= 8000 && pButton < 9000 )
	{
		var prevSub = pButton - 8000;
		var curPage = pUser.GetTempTag( "page" );
		pUser.SetTempTag( "subPage", prevSub );
		PageX( pSock, pUser, curPage );
		return;
	}

	if( pButton >= 9000 && pButton < 10000 )
	{
		var nextSub = pButton - 9000;
		var curPage2 = pUser.GetTempTag( "page" );
		pUser.SetTempTag( "subPage", nextSub );
		PageX( pSock, pUser, curPage2 );
		return;
	}

	// Handle "Make Last"
	if(( pButton >= 100 && pButton <= 998 ) || pButton == 5000 )
	{
		if( pButton == 5000 )
		{
			pButton = pUser.GetTempTag( "MAKELAST" );
			usedMakeLast = true;
		}
		else
		{
			pUser.SetTempTag( "MAKELAST", pButton );
		}
	}

	// Item detail buttons (2000 + buttonID)
	if( pButton >= 2000 && pButton < 3000 )
	{
		var detailButtonID = pButton - 2000;
		var entry = MasonryMap[detailButtonID];
		if( entry )
		{
			// For details we just pass the granite version (granite index 0) to 4026
			var graniteMakeID = entry.graniteMake[0];
			if( graniteMakeID > 0 )
			{
				// Masonry uses Carpentry skill
				 pUser.SetTempTag("Skill", entry.skill | 0);

				if( entry.harvest && entry.harvest.length > 0 )
					pUser.SetTempTag("Harvest",  entry.harvest[0]);

				if( entry.harvest2 && entry.harvest2.length > 0 )
					pUser.SetTempTag("Harvest2", entry.harvest2[0]);

				if( entry.harvest3 && entry.harvest3.length > 0 )
					pUser.SetTempTag("Harvest3", entry.harvest3[0]);

				if( entry.harvest4 && entry.harvest4.length > 0 )
					pUser.SetTempTag("Harvest4", entry.harvest4[0]);

				if( entry.recipeID && entry.recipeID > 0 )
					pUser.SetTempTag( "needRecipeID", entry.recipeID );
				else
					pUser.SetTempTag( "needRecipeID", 0 ); 

				pUser.SetTempTag( "ITEMDETAILS", graniteMakeID );
				TriggerEvent( itemDetailsScriptID, "ItemDetailGump", pUser );
			}
		}
		return;
	}

	// If this is a craft button in our map:
	if( MasonryMap[pButton] != undefined )
	{
		var entry2 = MasonryMap[pButton];
		var graniteID = pUser.GetTempTag( "Granite" );
		var resourceHue = pUser.GetTempTag( "resourceHue" );

		// Ensure graniteID within range
		if( graniteID < 0 || graniteID >= craftItems.length )
			graniteID = 0;

		// Era / recipe gating
		if( !eraOK( entry2 ))
		{
			pSock.SysMessage( "That item is not available in this era." );
			return;
		}

		if( entry2.recipeID && !TriggerEvent( 4022, "NeedRecipe", pUser, entry2.recipeID ))
		{
			pSock.SysMessage( "You must learn that recipe from a scroll." );
			return;
		}

		// No colored granited weapons if disabled and using non-iron ingots
		if( !allowColouredWeapons && resourceHue > 0 && entry2.page > 3 )
		{
			pSock.SysMessage( "You cannot use colored granited ingots for weapons on this shard." );
			return;
		}

		var makeID = entry2.graniteMake[graniteID];
		if( !makeID || makeID == 0 )
		{
			// Fallback to iron version if for some reason we did not get a specific granite entry
			makeID = entry2.graniteMake[0];
		}

		if( !makeID || makeID == 0 )
		{
			pSock.SysMessage( "That item is not properly configured." );
			return;
		}

		// Runic hammer bonus logic (unchanged from your original)
		pUser.AddScriptTrigger( 4033 );

		MakeItem( pSock, pUser, makeID, resourceHue );

		// Tool wear
		var toolUseLimit = GetServerSetting( "ToolUseLimit" );
		var toolUseBreak = GetServerSetting( "ToolUseBreak" );

		var runicHammer = pUser.FindItemLayer( 0x01 ); // Right Hand
		if( ValidateObject( runicHammer ) && runicHammer.GetTag( "runicHammer" ) && runicHammer.usesLeft > 0 )
		{
			pUser.SetTempTag( "usedRunicHammer", true );
			pUser.SetTempTag( "runicHammerType", runicHammer.color );

			if( toolUseLimit && runicHammer != bItem )
			{
				runicHammer.usesLeft -= 1;
				if( runicHammer.usesLeft == 0 && toolUseBreak )
				{
					runicHammer.Delete();
					pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language ));
				}
			}
		}

		if( toolUseLimit )
		{
			bItem.usesLeft -= 1;
			if( bItem.usesLeft == 0 && toolUseBreak )
			{
				bItem.Delete();
				pSock.SysMessage( GetDictionaryEntry( 10202, pSock.language ));
			}
		}

		// Track in last ten list for blacksmith
		AddToLastTenmMasonry( pUser, pButton );

		// Reopen page after delay
		pUser.StartTimer( gumpDelay, entry2.timerID, true );
		return;
	}

	// Granite selection buttons (Page20)
	if( pButton >= 1000 && pButton <= 1008 )
	{
		var index = pButton - 1000; // 0..8
		var newGraniteID = index;
		var newResourceHue = 0;

		// Optional: use Mining skill gating like ingots
		var miningSkill = pUser.skills.mining | 0;

		switch( index )
		{
			case 0: // Iron
				newResourceHue = 0;
				break;

			case 1: // Dull Copper
				if( miningSkill < 650 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x0973;
				break;

			case 2: // Shadow Iron
				if( miningSkill < 700 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x0966;
				break;

			case 3: // Copper
				if( miningSkill < 750 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x07dd;
				break;

			case 4: // Bronze
				if( miningSkill < 800 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x06d6;
				break;

			case 5: // Gold
				if( miningSkill < 850 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x08a5;
				break;

			case 6: // Agapite
				if( miningSkill < 900 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x0979;
				break;

			case 7: // Verite
				if( miningSkill < 950 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x089f;
				break;

			case 8: // Valorite
				if( miningSkill < 990 )
				{
					pSock.CloseGump( gumpID, 0 );
					pUser.StartTimer( ingotDelay, 8, true );
					pUser.SetTempTag( "prevActionResult", "FAILED" );
					return;
				}
				newResourceHue = 0x08ab;
				break;
		}

		// Store selection
		pUser.SetTempTag( "Granite", newGraniteID );
		pUser.SetTempTag( "resourceHue", newResourceHue );
		pUser.SetTempTag( "prevActionResult", null );
		pUser.SetTempTag( "MAKELAST", null );

		// Close the material select gump
		pSock.CloseGump( gumpID, 0 );

		// Go back to the last craft page, or default to page 1
		var curPage = pUser.GetTempTag( "page" );
		if( !curPage || curPage == 20 )
			curPage = 1;

		pUser.SetTempTag( "page", curPage );
		pUser.SetTempTag( "subPage", 1 );
		PageX( pSock, pUser, curPage );
		return;
	}
}

function AddToLastTenmMasonry( pUser, buttonID )
{
	var raw = pUser.GetTempTag( "LastTenMasonry" ) || "";
	var list = raw.split( "," );

	// Remove if already in list
	for( var i = 0; i < list.length; i++ )
	{
		if( parseInt( list[i] ) == buttonID )
		{
			list.splice( i, 1 );
			break;
		}
	}

	var newList = [buttonID];
	for( var j = 0; j < list.length && newList.length < 10; j++ )
	{
		var entry = parseInt( list[j] );
		if( !isNaN( entry ) && entry > 0 )
			newList.push( entry );
	}

	pUser.SetTempTag( "LastTenMasonry", newList.join( "," ) );
}

function HasLearnedRecipe( pUser, recipeID )
{
	var myData = TriggerEvent( 4022, "ReadRecipeID", pUser );
	if( !myData || myData.length == 0 )
		return false;

	for( var i = 0; i < myData.length; i++ )
	{
		var data = myData[i].split( "," );
		if( data[0] == recipeID )
			return true;
	}
	return false;
}

function eraOK( entry )
{
	// Optional per-entry gates. Strings like "lbr","aos","ml","sa","hs","tol".
	// If not present, the entry is valid for all eras.
	if( entry.minEra && coreShardEra < EraStringToNum( entry.minEra ))
		return false;
	if( entry.maxEra && coreShardEra > EraStringToNum( entry.maxEra ))
		return false;
	return true;
}