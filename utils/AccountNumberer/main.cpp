#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void main( void )
{
	ifstream inputFile( "accounts.adm" );
	if( !inputFile.is_open() || inputFile.fail() )
	{
		cout << "Unable to open input file accounts.adm.  Please run in a directory with such a file" << endl;
		return;
	}
	char line[256];
	int sectionNumber = 0;
	ofstream outputFile( "accounts2.adm" );
	if( !outputFile.is_open() )
	{
		cout << "Unable to open output file accounts2.adm" << endl;
		return;
	}
	while( !inputFile.eof() )
	{
		inputFile.getline( line, 256 );
		if( !strcmp( line, "SECTION ACCOUNT" ) )
			outputFile << line << " " << sectionNumber++;
		else
			outputFile << line;
		outputFile << endl;
	}
	cout << "Written out " << sectionNumber << "accounts" << endl;
}