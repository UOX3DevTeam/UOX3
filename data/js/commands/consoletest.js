function CommandRegistration()
{
	RegisterCommand( "consoletest", 2, true );
}

function command_CONSOLETEST( socket, cmdString )
{
	// Time to do a console functionality test!
	Console.ClearScreen();
	Console.PrintSectionBegin();
	Console.TurnYellow();
	Console.Print( "This is a yellow test\n" );
	Console.TurnBlue();
	Console.Print( "This is a blue test\n" );
	Console.TurnGreen();
	Console.Print( "This is a green test\n" );
	Console.TurnRed();
	Console.Print( "This is a red test\n" );
	Console.TurnNormal();
	Console.Print( "This is a normal test\n" );
	Console.TurnBrightWhite();
	Console.Print( "This is a bright white test\n" );
	Console.PrintSectionBegin();
	Console.PrintDone();
	Console.PrintFailed();
	Console.PrintPassed();
	Console.PrintBasedOnVal( true );
	Console.PrintSpecial( 4, "PrintSpecial test" );
	Console.MoveTo( 33, -1 );
	Console.Print( "MoveTo test" );
	Console.Error( "Hello world!" );
	Console.Warning( "Hello world!" );
	Console.Log( "Doh, a deer, a female deer!" );
	Console.Log( "Doh, a deer, a female deer 2!", "simpsons.log" );
	Console.Print( "Current mode : " + Console.mode );
	Console.Print( "Current level: " + Console.level );
	Console.Print( "Left         : " + Console.left );
	Console.Print( "Top          : " + Console.top );
	Console.Print( "Height       : " + Console.height );
	Console.Print( "Width        : " + Console.width );
	Console.Print( "Filter       : " + Console.filter );
	Console.Print( "Log Echo     : " + Console.logEcho );
}

