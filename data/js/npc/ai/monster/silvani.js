function onDeathBlow( pQueen, pKiller )
{
	let lordSer = parseInt( pQueen.GetTag( "LordOaksSerial" ));
	if( !lordSer )
		return;

	let lord = CalcCharFromSer( lordSer );
	if( ValidateObject( lord ))
	{
		lord.SetTag( "SilvaniSerial", null ); // clear damage-reduction link
	}
}

function SummonPixiesOn( pQueen, pTarget )
{
	if( !ValidateObject( pQueen ) || !ValidateObject( pTarget ))
		return;

	// Count how many of Silvani's Pixies are already nearby
	let existingPixieCount = AreaCharacterFunction( "CountSilvaniPixies", pQueen, 10, null );
	if( existingPixieCount >= 10 )
	{
		return;
	}

	let isPet = pTarget.owner && ValidateObject( pTarget.owner );
	let realTarget = isPet ? pTarget.owner : pTarget;

	let count = RandomNumber( 4, 6 );
	for (let i = 0; i < count; ++i)
	{
		let x = realTarget.x + RandomNumber( -1, 1 );
		let y = realTarget.y + RandomNumber( -1, 1 );
		let z = realTarget.z;
		let map = realTarget.worldnumber;
		let inst = realTarget.instanceID;

		let pixie = SpawnNPC( "pixie", x, y, z, map, inst, false );
		if( ValidateObject( pixie ))
		{
			pixie.SetTag( "SummonedBySilvani", pQueen.serial.toString() );
		}
	}
}

function CountSilvaniPixies( pQueen, pChar, pSock )
{
	let tag = pChar.GetTag( "SummonedBySilvani" );
	if( tag && tag == pQueen.serial.toString() )
		return true;

	return false;
}

function _restorecontext_() {}
