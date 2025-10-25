/// <reference path="../definitions.d.ts" />
// @ts-check
function ConsoleRegistration()
{
	RegisterKey( 'r', "restart" );
	RegisterKey( 'R', "restart" );
}

function restart()
{
	Console.BeginRestart();
}
