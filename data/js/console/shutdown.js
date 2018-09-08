function ConsoleRegistration()
{
	RegisterKey( 'q', "shutdown" );
	RegisterKey( 'Q', "shutdown" );
	RegisterKey( 27,  "shutdown" );	// escape
}

function shutdown()
{
	Console.BeginShutdown();
}
