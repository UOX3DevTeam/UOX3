function ConsoleRegistration()
{
//	RegisterKey( '0', "reloadall" );
	RegisterKey( '1', "reloadini" );
	RegisterKey( '2', "reloadaccts" );
	RegisterKey( '3', "reloadregions" );
	RegisterKey( '4', "reloadspawn" );
	RegisterKey( '5', "reloadspells" );
	RegisterKey( '6', "reloadcommands" );
	RegisterKey( '7', "reloaddfns" );
	RegisterKey( '8', "reloadjs" );
	RegisterKey( '9', "reloadhtml" );
}

/*function reloadall()
{
	Console.Print( "CMD: Loading Server INI ..." );
	Console.Reload( 0 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading Accounts ..." );
	Console.Reload( 1 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading Regions ..." );
	Console.Reload( 2 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading Spawn Regions ..." );
	Console.Reload( 3 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading Spells ..." );
	Console.Reload( 4 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading Commands ..." );
	Console.Reload( 5 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading Definition Files ..." );
	Console.Reload( 6 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading JS ..." );
	Console.Reload( 7 );
	Console.PrintDone( false );
	Console.Print( "CMD: Loading HTML ..." );
	Console.Reload( 8 );
	Console.PrintDone( false );
}*/

function reloadini()
{
	Console.Print( "CMD: Loading Server INI ..." );
	Console.Reload( 0 );
	Console.PrintDone( false );
}

function reloadaccts()
{
	Console.Print( "CMD: Loading Accounts ..." );
	Console.Reload( 1 );
	Console.PrintDone( false );
}
function reloadregions()
{
	Console.Print( "CMD: Loading Regions ..." );
	Console.Reload( 2 );
	Console.PrintDone( false );
}

function reloadspawn()
{
	Console.Print( "CMD: Loading Spawn Regions ..." );
	Console.Reload( 3 );
	Console.PrintDone( false );
}

function reloadspells()
{
	Console.Print( "CMD: Loading Spells ..." );
	Console.Reload( 4 );
	Console.PrintDone( false );
}

function reloadcommands()
{
	Console.Print( "CMD: Loading Commands ..." );
	Console.Reload( 5 );
	Console.PrintDone( false );
}

function reloaddfns()
{
	Console.Print( "CMD: Loading Definition Files ..." );
	Console.Reload( 6 );
	Console.PrintDone( false );
}

function reloadjs()
{
	Console.Print( "CMD: Loading JS ..." );
	Console.Reload( 7 );
	Console.PrintDone( false );
}

function reloadhtml()
{
	Console.Print( "CMD: Loading HTML ..." );
	Console.Reload( 8 );
	Console.PrintDone( false );
}
