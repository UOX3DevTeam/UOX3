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
	if( divorcedPairs.indexOf( iTurn.id ) != -1 )
	{
		var indexNum = divorcedPairs.indexOf( iTurn.id );
		var id = iTurn.id;
		iTurn.id = id + divorcedPairs[indexNum + 1];
	}
	else if( explicitAdd.indexOf( iTurn.id ) != -1 )
	{
		// true, it's an add operation
		iTurn.id++;
	}
	else if( explicitAdd.indexOf( iTurn.id - 0x1 ) != -1 )
	{
		// our partner is listed! this is an implicit subtraction operation
		iTurn.id--;
	}
}

var divorcedPairs = [ // they are paired up, but more than one index apart and out of order, [hex ID, add or subtract distance]
	0x2deb, 2, 0x2dec, 2, 0x2ded, -1, 0x2dee, -3, // Elven big chair (Mondain's Legacy)
	0x0b4a, 2, 0x0b4b, -2, 0x0b4c, -1,// Writing table
	0x2de6, -3,// Elven ornate chair (Mondain's Legacy)
	0x0b2e, 2, 0x0b2f, -1, 0x0b31, -2,// Wooden throne
	0x0b4f, 2, 0x0b50, -2, 0x0b51, -1,// Cushioned fancy wooden chair
	0x0b53, 2, 0x0b54, -2, 0x0b55, -1,// Cushioned wooden chair
	0x0b59, -3,// Fine wooden chair
	0x1218, 3, 0x1219, -1, 0x121a, -1, 0x121b, -1,// Stone throne
	0x0B5B, 2, 0x0B5C, -2, 0x0B5D, -1,// Straw chair
	0x0b8f, -13, 0x0b7c, 13, // Wooden table
	0x0a4d, 4, 0x0a51, -4, // Armoire red
	0x0a4f, 4, 0x0a53, -4, // Armoire
	0x0a30, 8, 0x0a38, -8, // Chest
	0x0a34, -8, 0x0a2c, 8, // Chest of drawers
	0x0e7d, -1235, 0x09aa, 1235, // Wooden box red
	0x09ab, 1233, 0x0e7c, -1233, // Metal Chest
	0x09a8, 1240, 0x0e80, -1240, // Gold box
	0x09a9, 1237, 0x0e7e, -1237, // Small wooden crate
];

var explicitAdd = [
	0x0e40, 0x0b30, 0x0b4e, 0x0B5A,  // Fancy metal chest// Wooden throne// Cushioned fancy wooden chair// Straw chair
	0x0b56, 0x0b57, 0x0b58, // Fine wooden chair// Fine wooden chair// Fine wooden chair
	0x0b52, 0x0b32, 0x0b2c, 0x319a, // Cushioned wooden chair// Cushioned throne// Wooden bench// Wooden bench
	0x3dff, 0x0a9d, 0x0e3c, 0x0e3e, // Stone bench// Wooden shelf// Large crate// Wooden box medium
	0x0e42, 0x0b49, 0x280b, 0x280d, // Wooden chest// Writing table// Chest (Samurai Empire)// Chest (Samurai Empire)
	0x280f, 0x2811, 0x2813, 0x2815, 0x2817,// Fancy chest (Samurai Empire)// Chest, wide (Samurai Empire)// Cabinet, tall (Samurai Empire)// Cabinate, low (Samurai Empire)
	0x2851, 0x2853, 0x2855, 0x2a47, 0x2a45, 0x2a49, 0x2a4b, // Sword Display (Samurai Empire)
	0x2857, 0x2859, // Armoire, red (Samurai Empire)// Armoire, beige (Samurai Empire)
	0x285b, 0x285d, 0x2a58, 0x2a4b, // Armoire, wood (Samurai Empire)// Skeletal throne (Samurai Empire)// Sword Display (Samurai Empire)
	0x2d05, 0x2d07, 0x2d09, 0x2dd3, 0x2dd9, 0x2ddf, 0x2de1, // (Mondain's Legacy - Simple armoire// Fancy armoire// Elven dresser// Elven alchemy table// Elven spinning wheel// Elven love seat// Elven ornate table )
	0x2de3, 0x2de4, 0x2de5, 0x2de7, 0x2de9, 0x2def, //(Mondain's Legacy - Elven ornate chair // Elven fancy table// Elven storage table// Elven book shelf)
	0x2df1, 0x2df3, 0x2df5, 0x2fea, 0x2d0b, // Elven ornate chest (Mondain's Legacy)// Elven reading desk (Mondain's Legacy)// Display  (Mondain's Legacy)// Elven wash basin (Mondain's Legacy)
	0x407c, 0x47da, // Incubator// Welcome mat (Stygian Abyss)
	0x4910, 0x4b8d, 0x4c24, 0x4c80, // (Stygian Abyss - Gothic chest //-Plague Banner // Alchemist Book// Chair simple)
	0x9c18, 0xa304, 0xa306, 0xa308, // Safe// Metal chest// Rusty metal chest// Gold chest
	0xa30a
];

function _restorecontext_() {}