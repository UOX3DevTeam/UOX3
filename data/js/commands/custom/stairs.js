// [stairs]  -> click bottom tile, then click top tile (further SOUTH, same X).
// Builds south-facing stairs using itemID 0x03EF.
// SpiderMonkey 1.8.5 (ES5), UOX3 JS APIs only (no const/let, no arrow funcs).

// Config (feel free to adjust)
var STAIRS_ITEM_ID = 0x03EF;      // south-facing stair art
var STAIRS_NAME = "stair (south)";
var IMMOVABLE_FLAG = 2;           // 2 = not movable (typical UOX3 setting)

// Register the GM command
function CommandRegistration() {
	// name, minLevel (GM=9 typical), inGameOnly (true)
	RegisterCommand("stairs", 9, true);
}

// Entry point: [stairs
function command_STAIRS(pSock, cmdString) {
	var pUser = pSock.currentChar;
	if (!ValidateObject(pUser))
		return;

	// Clear any previous session data on this socket
	pSock.SetTempInt("stairs_mode", 0);
	pSock.DeleteTag("stairs_x1");
	pSock.DeleteTag("stairs_y1");
	pSock.DeleteTag("stairs_z1");
	pSock.DeleteTag("stairs_world");
	pSock.DeleteTag("stairs_inst");

	// Ask for first (bottom) point
	pSock.SysMessage("Select the BOTTOM tile (south-facing stairs require same X; Y increases).");
	pSock.SetTempInt("stairs_mode", 1);
	pSock.CustomTarget(6000, "Select the bottom tile for the staircase.");
}

// Helper: read XYZ from target object (prefer targObj)
function _readTargetXYZ(pSock, targObj) {
	if (ValidateObject(targObj)) {
		return { x: targObj.x, y: targObj.y, z: targObj.z };
	}
	// If no object returned, we can’t reliably read ground coords here
	return null;
}

// First click callback: store start point
function onCallback6000(pSock, targObj) {
	var pUser = pSock.currentChar;
	if (!ValidateObject(pUser))
		return;

	if (pSock.GetTempInt("stairs_mode") !== 1)
		return;

	var loc = _readTargetXYZ(pSock, targObj);
	if (!loc) {
		pSock.SysMessage("Could not read target location. Try again with [stairs.");
		pSock.SetTempInt("stairs_mode", 0);
		return;
	}

	// Store start point on socket as tags so it survives script reloads better
	pSock.SetTag("stairs_x1", String(loc.x));
	pSock.SetTag("stairs_y1", String(loc.y));
	pSock.SetTag("stairs_z1", String(loc.z));
	pSock.SetTag("stairs_world", String(pUser.worldNumber));
	pSock.SetTag("stairs_inst", String(pUser.instanceID));

	pSock.SetTempInt("stairs_mode", 2);
	pSock.SysMessage("Bottom set at (" + loc.x + "," + loc.y + "," + loc.z + "). Now select the TOP tile (further SOUTH; same X).");
	pSock.CustomTarget(6001, "Select the TOP tile for the staircase.");
}

// Second click callback: build staircase
function onCallback6001(pSock, targObj) {
	var pUser = pSock.currentChar;
	if (!ValidateObject(pUser))
		return;

	if (pSock.GetTempInt("stairs_mode") !== 2)
		return;

	// Load stored start point
	var sx = parseInt(pSock.GetTag("stairs_x1"), 10);
	var sy = parseInt(pSock.GetTag("stairs_y1"), 10);
	var sz = parseInt(pSock.GetTag("stairs_z1"), 10);
	var sw = parseInt(pSock.GetTag("stairs_world"), 10);
	var si = parseInt(pSock.GetTag("stairs_inst"), 10);

	if (isNaN(sx) || isNaN(sy) || isNaN(sz)) {
		pSock.SysMessage("No stored start point. Use [stairs again.");
		_resetStairsState(pSock);
		return;
	}

	var endLoc = _readTargetXYZ(pSock, targObj);
	if (!endLoc) {
		pSock.SysMessage("Could not read target location. Start over with [stairs.");
		_resetStairsState(pSock);
		return;
	}

	// Enforce south-facing line: same X, higher Y
	if (endLoc.x !== sx) {
		pSock.SysMessage("X must match for south-facing stairs. Start over with [stairs.");
		_resetStairsState(pSock);
		return;
	}
	if (endLoc.y <= sy) {
		pSock.SysMessage("Top tile must be SOUTH (greater Y) than the bottom. Start over with [stairs.");
		_resetStairsState(pSock);
		return;
	}

	// Determine rise per step from tiledata
	var tileH = 0;
	try {
		tileH = GetTileHeight(STAIRS_ITEM_ID) | 0;
	} catch (e) {
		tileH = 0;
	}
	if (tileH <= 0) tileH = 1; // fallback if tiledata missing/0

	var steps = (endLoc.y - sy) + 1; // inclusive
	var placed = 0;
	var y;
	for (y = 0; y < steps; y++) {
		var xx = sx;
		var yy = sy + y;
		var zz = sz + (y * tileH);

		var stair = CreateBlankItem(
			pSock,           // socket (refresh weight/pack, visibility)
			pUser,           // character (context)
			1,               // amount
			STAIRS_NAME,     // item name
			STAIRS_ITEM_ID,  // itemID (hex)
			0x0,             // colour
			"ITEM",          // objectType
			false            // inPack (false -> place in world)
		);

		if (!ValidateObject(stair)) {
			pSock.SysMessage("Failed to create stair at (" + xx + "," + yy + "," + zz + "). Aborting.");
			break;
		}

		// Place it in the world
		if (typeof stair.SetLocation === "function")
			stair.SetLocation(xx, yy, zz, sw, si);
		else if (typeof stair.MoveTo === "function")
			stair.MoveTo(xx, yy, zz, sw, si);
		else {
			// last-resort fallback
			stair.x = xx; stair.y = yy; stair.z = zz;
			stair.worldNumber = sw;
			stair.instanceID = si;
			if (typeof stair.Refresh === "function") stair.Refresh();
		}

		// Lock it down and keep forever (adjust to taste)
		stair.movable = IMMOVABLE_FLAG;
		// Different shards use different flags; set both defensively:
		if (typeof stair.decayable !== "undefined") stair.decayable = false;
		if (typeof stair.decay !== "undefined") stair.decay = false;

		if (typeof stair.Refresh === "function") stair.Refresh();

		placed++;
	}

	if (placed > 0) {
		pSock.SysMessage(
			"Placed " + placed + " south-facing stair(s) from (" + sx + "," + sy + "," + sz +
			"), rise/step=" + tileH + "."
		);
	}

	_resetStairsState(pSock);
}

// Reset temp state on the socket
function _resetStairsState(pSock) {
	pSock.SetTempInt("stairs_mode", 0);
	pSock.SetTag("stairs_x1", null);
	pSock.SetTag("stairs_y1", null);
	pSock.SetTag("stairs_z1", null);
	pSock.SetTag("stairs_world", null);
	pSock.SetTag("stairs_inst", null);
}
