/// <reference path="../../definitions.d.ts" />
// @ts-check
//
// Dragon Turtle - Bubble Path + Bubble Attack (FireBreath-style timing)
// - DoMovingEffect coords for tile bubbles + projectile bubbles
// - Bubble Path: 8 rays, 5-11 steps, 1 step/sec, pop on touch -> -25 resist for 30s
// - Bubble Attack: volley in 11 tiles, delayed hit 700ms, reuses affected hue if present

var DT_HUES = [ 0, 33, 44, 9, 63, 53, 117 ];
var DT_DIRS = [
	{ dx:  0, dy: -1 }, { dx:  1, dy: -1 }, { dx:  1, dy:  0 }, { dx:  1, dy:  1 },
	{ dx:  0, dy:  1 }, { dx: -1, dy:  1 }, { dx: -1, dy:  0 }, { dx: -1, dy: -1 }
];

var DT_STATE  = {};
var DT_NEAR   = [];
var DT_VOLLEY = [];

function onCreateDFN( mNPC, objType )
{
	if( objType != 1 || !mNPC.npc )
		return;

	var k = "" + mNPC.serial;
	if( !DT_STATE[k] )
		DT_STATE[k] = {};

	var st  = DT_STATE[k];
	var now = GetCurrentClock();

	// nextPath: 25-35s
	st.nextPath = now + RandomNumber( 25000, 35000 );
	// nextAtk: 40-60s
	st.nextAtk  = now + RandomNumber( 40000, 60000 );

	st.pathActive = 0;
	st.pathLen    = 0;
	st.pathStep   = 0;
	st.nextStep   = 0;
	st.rays       = null;

	st.affected   = {}; // key=serial -> { hue, type, oldRes, exp }
	st.hits       = []; // [{ at, ser, type, dmg }]
}

function onAISliver( mNPC )
{
	if( !ValidateObject( mNPC ) || !mNPC.npc || mNPC.dead )
		return false;

	var k  = "" + mNPC.serial;
	var st = DT_STATE[k];
	if( !st )
	{
		DT_STATE[k] = {};
		onCreateDFN( mNPC, 1 );
		st = DT_STATE[k];
	}

	var now = GetCurrentClock();

	// -----------------------------
	// expire debuffs (30s)
	// -----------------------------
	for( var dk in st.affected )
	{
		if( !st.affected.hasOwnProperty( dk ))
			continue;

		var rec = st.affected[dk];
		if( now < rec.exp )
			continue;

		var trg = CalcCharFromSer( parseInt( dk, 10 ));
		if( ValidateObject( trg ))
			trg.Resist( rec.type, rec.oldRes );

		delete st.affected[dk];
	}

	// -----------------------------
	// process delayed hits (700ms)
	// -----------------------------
	if( st.hits.length > 0 )
	{
		var out = [];
		for( var hi = 0; hi < st.hits.length; hi++ )
		{
			var h = st.hits[hi];
			if( now < h.at )
			{
				out.push( h );
				continue;
			}

			var ht = CalcCharFromSer( h.ser );
			if( ValidateObject( ht ) && ht.vulnerable && !ht.dead
				&& (( !ht.npc && ht.online ) || ( ht.npc && ( ht.tamed || ht.hireling ))))
			{
				ht.Damage( h.dmg, h.type, mNPC );
			}
		}
		st.hits = out;
	}

	// -----------------------------
	// get combatant
	// -----------------------------
	var combatant = null;
	if( ValidateObject( mNPC.target ))
		combatant = mNPC.target;
	else if( ValidateObject( mNPC.attacker ))
		combatant = mNPC.attacker;

	if( !ValidateObject( combatant ))
		return false;

	if( !mNPC.InRange( combatant, 18 ))
		return false;

	// inline helpers
	function IsVictim( trg )
	{
		return ( ValidateObject( trg ) && trg != mNPC && trg.vulnerable && !trg.dead
			&& (( !trg.npc && trg.online ) || ( trg.npc && ( trg.tamed || trg.hireling ))));
	}
	function EffectHue( hue ) { return ( hue == 0 ? 0 : hue - 1 ); }
	function HueToType( hue )
	{
		// UOX3 WeatherType: 1 phys, 4 cold, 5 fire, 6 energy, 7 poison
		switch( hue )
		{
			case 0:  return 1;
			case 33:
			case 44: return 5;
			case 9:  return 4;
			case 63: return 7;
			default: return 6;
		}
	}

	// -----------------------------
	// start bubble path (25-35s CD)
	// -----------------------------
	if( !st.pathActive && now >= st.nextPath )
	{
		st.pathActive = 1;
		st.pathLen  = RandomNumber( 5, 11 );
		st.pathStep = 0;
		st.nextStep = now; // immediate
		st.rays = [];

		for( var i = 0; i < 8; i++ )
		{
			st.rays.push({
				x: mNPC.x, y: mNPC.y, z: mNPC.z,
				hue: DT_HUES[ RandomNumber( 0, DT_HUES.length - 1 ) ],
				dead: 0
			});
		}

		st.nextPath = now + RandomNumber( 25000, 35000 );
	}

	// -----------------------------
	// step bubble path (1/sec)
	// -----------------------------
	if( st.pathActive && now >= st.nextStep )
	{
		if( st.pathStep >= st.pathLen )
		{
			st.pathActive = 0;
			st.rays = null;
		}
		else
		{
			DT_NEAR.length = 0;
			AreaCharacterFunction( "DT_CollectNearInline", mNPC, 18, null );

			for( var r = 0; r < 8; r++ )
			{
				var ray = st.rays[r];
				if( !ray || ray.dead )
					continue;

				var di = r;

				// slight turning after step>2
				if( st.pathStep > 2 && ( RandomNumber( 0, 9 ) < 4 ))
					di = ( r + 1 ) & 7;

				var nx = ray.x + DT_DIRS[di].dx;
				var ny = ray.y + DT_DIRS[di].dy;
				var nz = ray.z;

				// tile bubble effect (src==trg)
				DoMovingEffect(
					nx, ny, nz,
					nx, ny, nz,
					0x3660,     // bubble fx
					0x03,       // speed
					0x10,       // loop
					false,      // explode
					EffectHue( ray.hue )
				);

				// touch check
				var victim = null;
				for( var vi = 0; vi < DT_NEAR.length; vi++ )
				{
					var c = DT_NEAR[vi];
					if( ValidateObject( c ) && c.x == nx && c.y == ny )
					{
						victim = c;
						break;
					}
				}

				if( IsVictim( victim ))
				{
					// pop on victim
					DoMovingEffect(
						victim.x, victim.y, victim.z,
						victim.x, victim.y, victim.z,
						0x3660,
						0x03,
						0x10,
						false,
						EffectHue( ray.hue )
					);

					// apply -25 resist debuff for 30s
					var key  = "" + victim.serial;
					var hue  = ray.hue;
					var type = HueToType( hue );

					if( st.affected[key] )
					{
						st.affected[key].hue  = hue;
						st.affected[key].type = type;
						st.affected[key].exp  = now + 30000;
					}
					else
					{
						var oldRes = victim.Resist( type );
						if( oldRes === false ) oldRes = 0;

						victim.Resist( type, oldRes - 25 );
						st.affected[key] = { hue: hue, type: type, oldRes: oldRes, exp: now + 30000 };
					}

					ray.dead = 1;
				}
				else
				{
					ray.x = nx;
					ray.y = ny;
					ray.z = nz;
				}
			}

			st.pathStep++;
			st.nextStep = now + 1000;
		}
	}

	// -----------------------------
	// bubble attack volley (40-60s CD)
	// -----------------------------
	if( now >= st.nextAtk )
	{
		st.nextAtk = now + RandomNumber( 40000, 60000 );

		DT_VOLLEY.length = 0;
		AreaCharacterFunction( "DT_CollectVolleyInline", mNPC, 11, null );

		for( var ai = 0; ai < DT_VOLLEY.length; ai++ )
		{
			var trg2 = DT_VOLLEY[ai];
			if( !IsVictim( trg2 ))
				continue;

			// reuse affected hue if present, else random
			var hue2 = -1;
			var k2 = "" + trg2.serial;
			if( st.affected[k2] && now < st.affected[k2].exp )
				hue2 = st.affected[k2].hue;

			if( hue2 == -1 )
				hue2 = DT_HUES[ RandomNumber( 0, DT_HUES.length - 1 ) ];

			var type2 = HueToType( hue2 );
			var dmg2  = RandomNumber( 60, 80 );

			// projectile visual (mouth-ish)
			DoMovingEffect(
				mNPC.x, mNPC.y, mNPC.z + 5,
				trg2.x, trg2.y, trg2.z,
				0x3660,  // bubble fx
				0x06,    // speed
				0x00,    // loop
				true,    // explode
				EffectHue( hue2 )
			);

			// delayed hit record
			st.hits.push({ at: now + 700, ser: trg2.serial, type: type2, dmg: dmg2 });
		}
	}

	return false;
}

function DT_CollectNearInline( src, trg )
{
	if( ValidateObject( trg ) && trg != src && trg.vulnerable && !trg.dead
		&& (( !trg.npc && trg.online ) || ( trg.npc && ( trg.tamed || trg.hireling ))))
	{
		DT_NEAR.push( trg );
	}
	return false;
}

function DT_CollectVolleyInline( src, trg )
{
	if( ValidateObject( trg ) && trg != src && trg.vulnerable && !trg.dead
		&& (( !trg.npc && trg.online ) || ( trg.npc && ( trg.tamed || trg.hireling ))))
	{
		DT_VOLLEY.push( trg );
	}
	return false;
}