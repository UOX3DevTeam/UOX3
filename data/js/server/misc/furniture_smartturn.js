// Furniture Smart Turn
// Will turn supported furniture clockwise when said furniture is double-clicked or dropped on the ground

// Turn the furniture when double-clicked
function onUseChecked( pUser, iUsed )
{
	// Don't override double-click type for item
	if( iUsed.type != 0 )
		return true;

	// Is item locked down, or set as immovable by GMs?
	if( iUsed.movable >= 2 )
	{
		var iMulti = iUsed.multi;
		if( ValidateObject( iMulti ))
		{
			if( iMulti.owner != pUser )
			{
				pUser.socket.SysMessage( GetDictionaryEntry( 2700, pUser.socket.language )); // Only the house owner can rotate furniture.
				return true;
			}
		}
		else
		{
			pUser.socket.SysMessage( GetDictionaryEntry( 2701, pUser.socket.language )); // That does not appear to work.
			return true;
		}
	}

	// All is well, rotate the furniture!
	SmartTurn( iUsed );
	return false;
}

// Turn the furniture when dropped
function onDrop( iDropped, pDropper )
{
	SmartTurn( iDropped );
	return true;
}

// Turn supported furniture clockwise, based on previous direction
function SmartTurn( iTurn )
{
	switch( iTurn.id )
	{
		// Straw chair
		case 0x0B5A: iTurn.id++; break;
		case 0x0B5B: iTurn.id = 0x0B5D; break;
		case 0x0B5C: iTurn.id = 0x0B5A; break;
		case 0x0B5D: iTurn.id--; break;

		// Fine wooden chair
		case 0x0b56: iTurn.id++; break;
		case 0x0b57: iTurn.id++; break;
		case 0x0b58: iTurn.id++; break;
		case 0x0b59: iTurn.id = 0x0b56; break;

		// Cushioned wooden chair
		case 0x0b52: iTurn.id++; break;
		case 0x0b53: iTurn.id = 0x0b55; break;
		case 0x0b54: iTurn.id = 0x0b52; break;
		case 0x0b55: iTurn.id--; break;

		// Cushioned fancy wooden chair
		case 0x0b4e: iTurn.id++; break;
		case 0x0b4f: iTurn.id = 0x0b51; break;
		case 0x0b50: iTurn.id = 0x0b4e; break;
		case 0x0b51: iTurn.id--; break;

		// Cushioned throne
		case 0x0b32: iTurn.id++; break;
		case 0x0b33: iTurn.id = 0x0b32; break;

		// Wooden throne
		case 0x0b2e: iTurn.id = 0x0b30; break;
		case 0x0b2f: iTurn.id--; break;
		case 0x0b30: iTurn.id++; break;
		case 0x0b31: iTurn.id = 0x0b2f; break;

		// Wooden bench
		case 0x0b2c: iTurn.id++; break;
		case 0x0b2d: iTurn.id = 0x0b2c; break;

		// Stone throne
		case 0x1218: iTurn.id = 0x121b; break;
		case 0x1219: iTurn.id--; break;
		case 0x121a: iTurn.id--; break;
		case 0x121b: iTurn.id--; break;

		// Stone bench
		case 0x3dff: iTurn.id++; break;
		case 0x3e00: iTurn.id = 0x3dff; break;

		// Fancy metal chest
		case 0x0e40: iTurn.id++; break;
		case 0x0e41: iTurn.id = 0x0e40; break;

		// Metal Chest
		case 0x09ab: iTurn.id = 0x0e7c; break;
		case 0x0e7c: iTurn.id = 0x09ab; break;

		// Gold box
		case 0x09a8: iTurn.id = 0x0e80; break;
		case 0x0e80: iTurn.id = 0x09a8; break;

		// Small wooden crate
		case 0x09a9: iTurn.id = 0x0e7e; break;
		case 0x0e7e: iTurn.id = 0x09a9; break;

		// Wooden box red
		case 0x09aa: iTurn.id = 0x0e7d; break;
		case 0x0e7d: iTurn.id = 0x09aa; break;

		// Chest of drawers
		case 0x0a2c: iTurn.id = 0x0a34; break;
		case 0x0a34: iTurn.id = 0x0a2c; break;

		// Chest of drawers red
		case 0x0a30: iTurn.id = 0x0a38; break;
		case 0x0a38: iTurn.id = 0x0a30; break;

		// Armoire
		case 0x0a4f: iTurn.id = 0x0a53; break;
		case 0x0a53: iTurn.id = 0x0a4f; break;

		// Armoire red
		case 0x0a4d: iTurn.id = 0x0a51; break;
		case 0x0a51: iTurn.id = 0x0a4d; break;

		// Wooden shelf
		case 0x0a9d: iTurn.id = 0x0a9e; break;
		case 0x0a9e: iTurn.id = 0x0a9d; break;

		// Large crate
		case 0x0e3c: iTurn.id = 0x0e3d; break;
		case 0x0e3d: iTurn.id = 0x0e3c; break;

		// Wooden box medium
		case 0x0e3e: iTurn.id = 0x0e3f; break;
		case 0x0e3f: iTurn.id = 0x0e3e; break;

		// Wooden chest
		case 0x0e42: iTurn.id = 0x0e43; break;
		case 0x0e43: iTurn.id = 0x0e42; break;

		// Wooden table
		case 0x0b90: iTurn.id = 0x0b7d; break;
		case 0x0b7d: iTurn.id = 0x0b90; break;

		// Wooden table
		case 0x0b8f: iTurn.id = 0x0b7c; break;
		case 0x0b7c: iTurn.id = 0x0b8f; break;

		// Writing table
		case 0x0b49: iTurn.id++; break;
		case 0x0b4a: iTurn.id = 0x0b4c; break;
		case 0x0b4b: iTurn.id = 0x0b49; break;
		case 0x0b4c: iTurn.id--; break;

		// Chest (Samurai Empire)
		case 0x280b: iTurn.id++; break;
		case 0x280c: iTurn.id--; break;

		// Chest (Samurai Empire)
		case 0x280d: iTurn.id++; break;
		case 0x280e: iTurn.id--; break;

		// Fancy chest (Samurai Empire)
		case 0x280f: iTurn.id++; break;
		case 0x2810: iTurn.id--; break;

		// Chest, wide (Samurai Empire)
		case 0x2811: iTurn.id++; break;
		case 0x2812: iTurn.id--; break;

		// Chest, wide (Samurai Empire)
		case 0x2813: iTurn.id++; break;
		case 0x2814: iTurn.id--; break;

		// Cabinet, tall (Samurai Empire)
		case 0x2815: iTurn.id++; break;
		case 0x2816: iTurn.id--; break;

		// Cabinate, low (Samurai Empire)
		case 0x2817: iTurn.id++; break;
		case 0x2818: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2851: iTurn.id++; break;
		case 0x2852: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2853: iTurn.id++; break;
		case 0x2854: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2855: iTurn.id++; break;
		case 0x2856: iTurn.id--; break;

		// Armoire, red (Samurai Empire)
		case 0x2857: iTurn.id++; break;
		case 0x2858: iTurn.id--; break;

		// Armoire, beige (Samurai Empire)
		case 0x2859: iTurn.id++; break;
		case 0x285a: iTurn.id--; break;

		// Armoire, wood (Samurai Empire)
		case 0x285b: iTurn.id++; break;
		case 0x285c: iTurn.id--; break;

		// Armoire, wood (Samurai Empire)
		case 0x285d: iTurn.id++; break;
		case 0x285e: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2a45: iTurn.id++; break;
		case 0x2a46: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2a47: iTurn.id++; break;
		case 0x2a48: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2a49: iTurn.id++; break;
		case 0x2a4a: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2a4b: iTurn.id++; break;
		case 0x2a4c: iTurn.id--; break;

		// Skeletal throne (Samurai Empire)
		case 0x2a58: iTurn.id++; break;
		case 0x2a59: iTurn.id--; break;

		// Sword Display (Samurai Empire)
		case 0x2a4b: iTurn.id++; break;
		case 0x2a4c: iTurn.id--; break;

		// Simple armoire (Mondain's Legacy)
		case 0x2d05: iTurn.id++; break;
		case 0x2d06: iTurn.id--; break;

		// Fancy armoire (Mondain's Legacy)
		case 0x2d07: iTurn.id++; break;
		case 0x2d08: iTurn.id--; break;

		// Elven dresser (Mondain's Legacy)
		case 0x2d09: iTurn.id++; break;
		case 0x2d0a: iTurn.id--; break;

		// Elven wash basin (Mondain's Legacy)
		case 0x2d0b: iTurn.id++; break;
		case 0x2d0c: iTurn.id--; break;

		// Elven alchemy table (Mondain's Legacy)
		case 0x2dd3: iTurn.id++; break;
		case 0x2dd4: iTurn.id--; break;

		// Elven spinning wheel (Mondain's Legacy)
		case 0x2dd9: iTurn.id++; break;
		case 0x2dda: iTurn.id--; break;

		// Elven love seat (Mondain's Legacy)
		case 0x2ddf: iTurn.id++; break;
		case 0x2de0: iTurn.id--; break;

		// Elven ornate table (Mondain's Legacy)
		case 0x2de1: iTurn.id++; break;
		case 0x2de2: iTurn.id--; break;

		// Elven ornate chair (Mondain's Legacy)
		case 0x2de3: iTurn.id++; break;
		case 0x2de4: iTurn.id++; break;
		case 0x2de5: iTurn.id++; break;
		case 0x2de6: iTurn.id = 0x2de3; break;

		// Elven fancy table (Mondain's Legacy)
		case 0x2de7: iTurn.id++; break;
		case 0x2de8: iTurn.id--; break;

		// Elven storage table (Mondain's Legacy)
		case 0x2de9: iTurn.id++; break;
		case 0x2dea: iTurn.id--; break;

		// Elven big chair (Mondain's Legacy)
		case 0x2deb: iTurn.id = 0x2ded; break;
		case 0x2dec: iTurn.id = 0x2dee; break;
		case 0x2ded: iTurn.id--; break;
		case 0x2dee: iTurn.id = 0x2deb; break;

		// Elven book shelf (Mondain's Legacy)
		case 0x2def: iTurn.id++; break;
		case 0x2df0: iTurn.id--; break;

		// Elven ornate chest (Mondain's Legacy)
		case 0x2df1: iTurn.id++; break;
		case 0x2df2: iTurn.id--; break;

		// Elven ornate chest (Mondain's Legacy)
		case 0x2df3: iTurn.id++; break;
		case 0x2df4: iTurn.id--; break;

		// Elven reading desk (Mondain's Legacy)
		case 0x2df5: iTurn.id++; break;
		case 0x2df6: iTurn.id--; break;

		// Display case (Mondain's Legacy)
		case 0x2fea: iTurn.id++; break;
		case 0x2feb: iTurn.id--; break;

		// Wooden bench
		case 0x319a: iTurn.id++; break;
		case 0x319b: iTurn.id--; break;

		// Incubator
		case 0x407c: iTurn.id++; break;
		case 0x407d: iTurn.id--; break;

		// Welcome mat (Stygian Abyss)
		case 0x47da: iTurn.id++; break;
		case 0x47db: iTurn.id--; break;

		// Gothic chest (Stygian Abyss)
		case 0x4910: iTurn.id++; break;
		case 0x4911: iTurn.id--; break;

		// Plague Banner (Stygian Abyss)
		case 0x4b8d: iTurn.id++; break;
		case 0x4b8e: iTurn.id--; break;

		// Alchemist Bookcase (Stygian Abyss)
		case 0x4c24: iTurn.id++; break;
		case 0x4c25: iTurn.id--; break;

		// Chair simple (Stygian Abyss)
		case 0x4c80: iTurn.id++; break;
		case 0x4c81: iTurn.id--; break;

		// Safe
		case 0x9c18: iTurn.id++; break;
		case 0x9c19: iTurn.id--; break;

		// Metal chest
		case 0xa304: iTurn.id++; break;
		case 0xa305: iTurn.id--; break;

		// Rusty metal chest
		case 0xa306: iTurn.id++; break;
		case 0xa307: iTurn.id--; break;

		// Gold chest
		case 0xa308: iTurn.id++; break;
		case 0xa309: iTurn.id--; break;

		// Barnacle metal chest
		case 0xa30a: iTurn.id++; break;
		case 0xa30b: iTurn.id--; break;
	}
}