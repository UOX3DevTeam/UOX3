#include "uox3.h"
#include "debug.h"
#include "cSkillClass.h"
#include "ssection.h"

#undef DBGFILE
#define DBGFILE "newbie.cpp"

// Newbie code redone by Thyme
void addNewbieItem( cSocket *socket, CChar *c, char* str)
{
	ScriptSection *newbieData = FileLookup->FindEntry( str, newbie_def );
	if( newbieData == NULL )
		return;
	for( const char *tag = newbieData->First(); !newbieData->AtEnd(); tag = newbieData->Next() )
	{
		const char *data = newbieData->GrabData();
		if( !strcmp( "PACKITEM", tag ) )
		{
			CItem *n = Items->SpawnItemToPack( socket, c, data, false );
			if( n != NULL )
				n->SetNewbie( true );
		}
	}
}

void newbieItems( CChar *c )
{
	vector< cSkillClass > nSkills;
	char whichsect[15];
	for( UI08 sCtr = 0; sCtr < TRUESKILLS; sCtr++ )
		nSkills.push_back( cSkillClass( sCtr, c->GetBaseSkill( sCtr ) ) );

	sort( nSkills.rbegin(), nSkills.rend() );

	cSocket *s = calcSocketObjFromChar( c );
	for( int i = 0; i < 3; i++ )
	{
		if( nSkills[i].value > 0 )
		{
			sprintf( whichsect, "BESTSKILL %i", nSkills[i].skill );
			addNewbieItem( s, c, whichsect );
		}
		else
			continue;
	}
	addNewbieItem( s, c, "DEFAULT" );
}

