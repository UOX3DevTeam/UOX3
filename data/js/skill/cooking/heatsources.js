function HeatSource( tileID )
{
	// In case its an oven, fireplace, campfire or fire pit
	return (( tileID >= 0x0461 && tileID <= 0x0480 ) || ( tileID >= 0x092B && tileID <= 0x0933 ) ||
		( tileID >= 0x0937 && tileID <= 0x0942 ) || ( tileID >= 0x0945 && tileID <= 0x0950 ) ||
		( tileID >= 0x0953 && tileID <= 0x095e ) || ( tileID >= 0x0961 && tileID <= 0x096c ) ||
		( tileID >= 0x0de3 && tileID <= 0x0de8 ) || tileID == 0x0fac );
}

function Ovens( tileID )
{
	return ( tileID >= 0x461 && tileID <= 0x46F ) || ( tileID >= 0x92B && tileID <= 0x93F );
}