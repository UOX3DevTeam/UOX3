//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  combat.cpp
//
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
//  This File is part of UOX3
//  Ultima Offline eXperiment III
//  UO Server Emulation Program
//  
//  Copyright 1997 - 2001 by Marcus Rating (Cironian)
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//	
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//   
//""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

#include "uox3.h"
#include "debug.h"

#define DBGFILE "combat.cpp"
#define SWINGAT (unsigned int)1.75 * CLOCKS_PER_SEC


//#define __COMBATDEBUG__		// if this is defined, then the combat debug messages are printed

int cCombat::GetSwingRate( int iNPC, int weapon )
{
	// PRE: Get calculate swing rate
	// POST: The amount of seconds for one swing
	int sRate = 0;
	int speed = 1;

	Skills->GetCombatSkill( iNPC );

	if( weapon==-1) weapon = GetWeapon( iNPC );

	int stamina = chars[iNPC].stm;
#ifdef __COMBATDEBUG__
	printf("Stamina is %i...", stamina );
#endif
	if( weapon != -1 )
	{
		speed = items[weapon].spd;
#ifdef __COMBATDEBUG__
		printf("Item %s [%i]...", items[weapon].name, weapon );
#endif
	}
	else speed = combat.wrestleSpeed;
#ifdef __COMBATDEBUG__
	printf("Speed is %i...", speed );
#endif
	sRate = 16000 / ( ( ( stamina+100 ) * speed ) + 1 );		// Abaddon, was 15000, increased because of slightly slow swing speed
#ifdef __COMBATDEBUG__
	printf("sRate is %i\n", sRate );
#endif
	if( sRate <= 0 ) sRate = 1;

	return sRate;
}


int cCombat::GetBowType(int i)
{
	int j,serial,serhash,ci;
	serial=chars[i].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		j=contsp[serhash].pointer[ci];
		if (j!=-1)
		if ((items[j].contserial==serial) && ((items[j].layer==1) || (items[j].layer==2)))
		{
			if ((items[j].id1==0x13)&&(items[j].id2==0xB1)) return(1); // Bows
			if ((items[j].id1==0x13)&&(items[j].id2==0xB2)) return(1);
			if ((items[j].id1==0x0F)&&(items[j].id2==0x4F)) return(2); // Crossbows
			if ((items[j].id1==0x0F)&&(items[j].id2==0x50)) return(2);
			if ((items[j].id1==0x13)&&(items[j].id2==0xFC)) return(3); // Heavy Crossbows
			if ((items[j].id1==0x13)&&(items[j].id2==0xFD)) return(3);
		}
	}
	return 0;
}

int cCombat::GetArrowType( int j )
{
	if( j == -1 ) return 0;
	if(( items[j].id1 == 0x13 ) && ( items[j].id2 == 0xB1 )) return( ARROW ); // Bows
	if(( items[j].id1 == 0x13 ) && ( items[j].id2 == 0xB2 )) return( ARROW );
	if(( items[j].id1 == 0x0F ) && ( items[j].id2 == 0x4F )) return( BOLT ); // Crossbows
	if(( items[j].id1 == 0x0F ) && ( items[j].id2 == 0x50 )) return( BOLT );
	if(( items[j].id1 == 0x13 ) && ( items[j].id2 == 0xFC )) return( BOLT ); // Heavy Crossbows
	if(( items[j].id1 == 0x13 ) && ( items[j].id2 == 0xFD )) return( BOLT );
	return 0;
}


//NEW FUNCTION -- AntiChrist merging codes -- (24/6/99)
void cCombat::ItemCastSpell(int s, int c, int i)//S=Socket c=Char # Target i=Item // Itemid
{
	if(i==-1) return;
	unsigned short int spellnum=((items[i].morex*8)-8)+items[i].morey;
	unsigned short int tempmana=chars[currchar[s]].mn;//Save their mana so we can give it back.
	unsigned short int tempmage=chars[currchar[s]].skill[MAGERY];//Easier than writing new functions for all these spells
	

	if(items[i].type!=15) return;
	
	if(items[i].morez<=0) return;
	
	chars[currchar[s]].skill[MAGERY]=rand()%400+600;//For damage
	
	switch(spellnum)
	{
	case 1: Magic->NPCClumsyTarget(currchar[s],c); break; //LB
	case 3: Magic->NPCFeebleMindTarget(currchar[s],c); break; //LB
	case 5:	Magic->NPCMagicArrowTarget(currchar[s],c);	break; // lB
	case 8: Magic->NPCWeakenTarget(currchar[s],c); break; //LB
	case 18: Magic->NPCFireballTarget(currchar[s],c); break; //LB
	case 22: Magic->NPCHarmTarget(currchar[s],c); break; //LB
	case 27: Magic->NPCCurseTarget(currchar[s],c); break; //LB
	case 30: Magic->NPCLightningTarget(currchar[s],c); break; //lb
	case 37: Magic->NPCMindBlastTarget(currchar[s],c); break;
	case 38: Magic->NPCParalyzeTarget(currchar[s],c);	break; //lb
	case 42: Magic->NPCEBoltTarget(currchar[s],c); break;
	case 43: Magic->NPCExplosionTarget(currchar[s],c); break;
	case 51: Magic->NPCFlameStrikeTarget(currchar[s],c); break;
	default:
		staticeffect(currchar[s], 0x37, 0x35, 0, 30);
		soundeffect2(currchar[s], 0x00, 0x5C);
		break;
	}
	if( chars[currchar[s]].mn!=tempmana )
		chars[currchar[s]].mn+=tempmana;
	chars[currchar[s]].skill[MAGERY]=tempmage;
	if(chars[currchar[s]].in<chars[currchar[s]].mn) chars[currchar[s]].mn=chars[currchar[s]].in;//Shouldn't happen, but just in case;
	updatestats(s, 1);
	
	items[i].morez--;
	if(items[i].morez==0)//JUST lost it's charge....
		sysmessage(s, "This item is out of charges.");
}
//NEW FUNCTION END -- AntiChrist merging codes --

void cCombat::CombatHit(int attack, int defend, unsigned int currenttime, signed int arrowType )
// PARAM WARNING: arrowType and currenttime are unreferenced parameters
// Arrowtype was originally meant for bolt vs arrow, and weather based stuff, so a fire ele would be weak to ice arrow
{
	// Do this right away so we can save few 0.00000001 milliseconds on going through other voids like LOS()
	unsigned int distance = 0;	
	chars[attack].swingtarg=-1;

	if (defend==-1) return;
	if (defend==attack) return;
	if( chars[defend].dead || chars[defend].hp <= 0 || chars[defend].priv&0x04 )
	{	// invalidate attacker here!!!
		if( chars[attack].npcaitype==4 ) //LB change from 0x40 to 4
		{
			chars[attack].summontimer = (uiCurrentTime + (CLOCKS_PER_SEC * 20));
			chars[attack].npcWander=2;
			chars[attack].npcmovetime = (unsigned int) ((uiCurrentTime+double(NPCSPEED*CLOCKS_PER_SEC)));
			npctalkall(attack,"Thou have suffered thy punishment, scoundrel.", 0);
		}
		chars[attack].targ = -1;
		if( chars[attack].attacker > -1 && chars[attack].attacker < cmem )
		{
			chars[chars[attack].attacker].attackfirst=0;
			chars[chars[attack].attacker].attacker=-1;
		}
		chars[attack].attacker=-1;
		chars[attack].attackfirst=0;
		if( chars[attack].npc && !chars[attack].dead && chars[attack].npcaitype != 17 && chars[attack].war ) 
			npcToggleCombat( attack ); // ripper
		return;
	}

	// Now we check for LOS, get the weapon and skill and check to see if they are in range.
	char hit;
	signed short s1 = calcSocketFromChar( attack ), s2 = calcSocketFromChar( defend );
	
 	unsigned short fightskill = Skills->GetCombatSkill( attack ), bowtype = GetBowType( attack ), splitnum, splitcount, hitin;
 	//Homey 2/19/2000 lets check distance first, and if NPC
 	//isn't close enough to melle strike or using archery, lets break and avoid the 
 	//huge LOS code, big speed up here...
	distance = chardist( attack, defend );
 	if( distance > 1 && fightskill != ARCHERY )
		return;
	if( distance < 3 && fightskill == ARCHERY ) 
	{
		int m = RandomNum( 1, 3 );
		if( m != 1 )
		{
			sysmessage( s1, "You are too close to your target to aim properly!" );
			return;
		}
	}
	//	Homey	-	End
	unsigned short los = line_of_sight(s1,chars[attack].x,chars[attack].y,chars[attack].z,chars[defend].x,chars[defend].y,chars[defend].z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING);
	unsigned int c, BaseDamage;

	Skills->GetCombatSkill( attack ); // This will get skill and weapon

	int damage; // removed from unsigne by Magius(CHE)
	signed int x;
	// Magius(CHE) - For armour absorbtion system
	char t[512],debabs[512]; 
	int maxabs, maxnohabs, tmpj;

	int weapon=GetWeapon(attack);//AntiChrist - get the weapon item only once

	*debabs='\0'; 
	*t='\0';
	// End here - Magius(CHE) - For armour absorbtion system

	hit = Skills->CheckSkill( attack, fightskill, 0, 1000 );  // increase fighting skill for attacker and defender

	if (!hit)
	{
		if ((fightskill==ARCHERY)&&(los))
		{
			//if (!chars[attack].npc) soundeffect2(attack, 0x2, 0x38);
			if (!chars[attack].npc) doMissedSoundEffect(attack);

			if (rand()%3-1)//-1 0 or 1
			{
				c = Items->SpawnItem(s1, attack, 1,"#",1,0x0E,0x75,0,0,0,0);
				if( c != -1 )
				{
					if (bowtype==1)
					{
						items[c].id1=0x0F; 
						items[c].id2=0x3F;
					} else {
						items[c].id1=0x1B;
						items[c].id2=0xFB;
					}
					
					mapRegions->RemoveItem(c);

					items[c].x=chars[defend].x;
					items[c].y=chars[defend].y;
					items[c].z=chars[defend].z;
					items[c].priv=1;

				
					mapRegions->AddItem(c);
					
					//sendinrange(c);
					RefreshItem(c);//AntiChrist
				}
			}
		//} else if (!chars[attack].npc) soundeffect2(attack, 0x2, 0x38);
		} else if (!chars[attack].npc) doMissedSoundEffect(attack);
	} else {
		if (!(chars[defend].priv&4))
		{
			Skills->CheckSkill(attack, TACTICS, 0, 1000);
			if (chars[defend].xid2==0x91) soundeffect2(defend,0x01,0x4b);
			if (chars[defend].xid2==0x90) soundeffect2(defend,0x01,0x56);
			playmonstersound(defend, chars[defend].id1, chars[defend].id2, SND_DEFEND);
			//AntiChrist -- for poisoned weapons
			if ((chars[attack].poison)&&(chars[defend].poisoned<chars[attack].poison) && ( distance<=1 ))
			{
				if (rand()%3==0 || fightskill==FENCING)//0 1 or 2 //fencing always poisons :) - AntiChrist
				{
					//if(chars[defend].poisoned<chars[attack].poison) chars[defend].poisoned=chars[attack].poison; //this's a double check - look at the if above - AntiChrist
					chars[defend].poisoned=chars[attack].poison;
					chars[defend].poisontime = (uiCurrentTime+(CLOCKS_PER_SEC*(40/chars[defend].poisoned))); // a lev.1 poison takes effect after 40 secs, a deadly pois.(lev.4) takes 40/4 secs - AntiChrist
					//chars[defend].poisonwearofftime=uiCurrentTime+(CLOCKS_PER_SEC*server_data.poisontimer); // lb, poison wear off timer setting
					chars[defend].poisonwearofftime = (chars[defend].poisontime + (CLOCKS_PER_SEC * server_data.poisontimer)); //wear off starts after poison takes effect - AntiChrist
					if (s2!=-1) impowncreate(s2,defend,1); //Lb, sends the green bar ! 
					//npctalkall(s2,"You have been poisoned!");
					if (s2!=-1) sysmessage(s2,"You have been poisoned!");//AntiChrist 
				}
			}			
			//Algaran 11-24-98 chars[k].priv2=chars[k].priv2&0xFD;
			if ((chars[defend].dx>0)) chars[defend].priv2=chars[defend].priv2&0xFD;

			if (fightskill!=WRESTLING)
				if (los) ItemSpell(attack, defend);
			
			if ((chars[defend].poison)&&(chars[attack].poisoned<chars[defend].poison) && ( distance <= 1 ))
			{
				if (rand()%3==0)
				{
					chars[attack].poisoned=chars[defend].poison;
					chars[attack].poisontime= (unsigned int) (uiCurrentTime + (CLOCKS_PER_SEC * (40 / chars[attack].poisoned))); // a lev.1 poison takes effect after 40 secs, a deadly pois.(lev.4) takes 40/4 secs - AntiChrist
					chars[attack].poisonwearofftime = (chars[attack].poisontime+(CLOCKS_PER_SEC*server_data.poisontimer)); //wear off starts after poison takes effect - AntiChrist
					//chars[attack].poisonwearofftime=uiCurrentTime+(CLOCKS_PER_SEC*server_data.poisontimer); // lb, poison wear off timer setting
					if (s1!=-1) impowncreate(s1,attack,1); //Lb, sends the green bar ! 
					//sprintf(temp,"*%s has poisoned %s!*",chars[defend].name,chars[attack].name);
					//npctalkall(a,temp);
					if (s1!=-1) sysmessage(s1,"You have been poisoned!");//AntiChrist
				}
			}
			if (fightskill!=WRESTLING || chars[attack].npc==1) BaseDamage=CalcAtt(attack); // Calc base damage
			else
			{
				if ((chars[attack].skill[WRESTLING]/100) > 0) BaseDamage=rand()%(chars[attack].skill[WRESTLING]/100);
				else BaseDamage=rand()%2;
			}
			// Race Dmg Modification: Bonus percentage.
			int RaceDamage = Races->getFightPercent( fightskill, chars[attack].race );
			if( RaceDamage == 0 )
				RaceDamage = 100;
			BaseDamage+= BaseDamage/RaceDamage;

			// Check to see if weapons affect defender's race.
			if( fightskill!= WRESTLING )
			{
				if( items[weapon].racialEffect == chars[defend].race ) BaseDamage *= 2;
			}
			damage=(int)(BaseDamage*((chars[attack].skill[TACTICS]+500.0)/1000.0)); // Add Tactical bonus
			damage=damage+(int)((BaseDamage*(chars[attack].st/500.0))); // Add Strength bonus

			/////////////////////////////////
			//Adds a BONUS DAMAGE for ANATOMY
			//Anatomy=100 -> Bonus +20% Damage - AntiChrist (11/10/99)
			//printf("NORMAL DAMAGE: %d\n",damage);
			float multiplier=(((chars[attack].skill[ANATOMY]*20)/1000.0)/100)+1;
			//printf("MULTIPLIER: %f\n",multiplier);
			damage=(int)  (damage * multiplier);
			//printf("DAMAGE WITH ANAT: %d\n",damage);
		
			//////////////////////////////////
			//Adds a BONUS DEFENCE for TACTICS
			//Tactics=100 -> Bonus -20% Damage - AntiChrist (11/10/99)
			//printf("NORMAL DAMAGE: %d\n",damage);
			multiplier=1-(((chars[defend].skill[TACTICS]*20)/1000.0)/100);
			//printf("MULTIPLIER: %f\n",multiplier);
			damage=(int)  (damage * multiplier);
			//printf("DAMAGE WITH TAC: %d\n",damage);
	
			//
			// this 3 lines commented out by LB
			// this gives undefeatable super-npcs :(
			//
			// 11/10 AntiChrist note: i reprogrammed the above lines
			// ..now it should work..
			// I tested it and it seems to work...
			// pleaz leave it enabled and tell me what happens! thnx
			////////////////

			x=Skills->GetShield(defend);
			if(x>-1)
			{
				if (Skills->CheckSkill(defend, PARRYING, 0, 1000))// chance to block with shield
				{
					//Algaran 11-24-98 damage-=rand()%(items[j].def/2)+items[j].def/2;  // damage absorbed by shield
					damage-=rand()%(items[x].def);  // damage absorbed by shield
					if(rand()%2) items[x].hp--; //Take off a hit point
					if(items[x].hp<=0)
					{
						sysmessage(s2,"Your shield has been destroyed");
						Items->DeleItem(x);
					}
				}
			}
			x=rand()%100;// determine area of body hit
			if (server_data.combathitmessage!=1)
			{
				if (x<=44) x=1; // body
				else if (x<=58) x=2; // arms
				else if (x<=72) x=3; // head
				else if (x<=86) x=4; // legs
				else if (x<=93) x=5; // neck
				else x=6; // hands
			}
			if (server_data.combathitmessage==1)
			{
				// Removed by Magius(CHE) Was: ((!chars[defend].npc)&& (s2 != -1))
					hitin = rand()%2;
					if (x<=44)
					{
						x=1;       // body
						switch (hitin)
						{
						case 1:
							//later take into account dir facing attacker during battle
							if (damage < 10) strcpy(temp,"hits you in your Chest!");
							if (damage >=10) strcpy(temp,"lands a terrible blow to your Chest!");
							break;
						case 2:
							if (damage < 10) strcpy(temp,"lands a blow to your Stomach!");
							if (damage >=10) strcpy(temp,"knocks the wind out of you!");
							break;
						default:
							if (damage < 10) strcpy(temp,"hits you in your Ribs!");
							if (damage >=10) strcpy(temp,"broken your Rib?!");
							break;
						}
					}
					else if (x<=58)
					{
						x=2;  // arms
						switch (hitin)
						{
						case 1:
							if (damage > 1) strcpy(temp,"hits you in Left Arm!");
							break;
						case 2:
							if (damage > 1) strcpy(temp,"hits you in Right Arm!");
							break;
						default:
							if (damage > 1) strcpy(temp,"hits you in Right Arm!");
							break;
						}
					}
					else if (x<=72)
					{
						x=3;  // head
						switch (hitin)
						{
						case 1:
							if (damage < 10) strcpy(temp,"hits you you straight in the Face!");
							if (damage >=10) strcpy(temp,"lands a stunning blow to your Head!");
							break;
						case 2:
							if (damage < 10) strcpy(temp,"hits you to your Head!"); //kolours - (09/19/98)
							if (damage >=10) strcpy(temp,"smashed a blow across your Face!");
							break;
						default:
							if (damage < 10) strcpy(temp,"hits you you square in the Jaw!");
							if (damage >=10) strcpy(temp,"lands a terrible hit to your Temple!");
							break;
						}
					}
					else if (x<=86) 
					{
						x=4;  // legs
						switch (hitin)
						{
						case 1:
							strcpy(temp,"hits you in Left Thigh!");
							break;
						case 2:
							strcpy(temp,"hits you in Right Thigh!");
							break;
						default:
							strcpy(temp,"hits you in Groin!");
							break;
						}
					}
					else if (x<=93)
					{
						x=5;  // neck
						strcpy(temp,"hits you to your Throat!");
					}
					else
					{
						x=6;  // hands
						switch (hitin)
						{
						case 1:
							if (damage > 1) strcpy(temp,"hits you in Left Hand!");
							break;
						case 2:
							if (damage > 1) strcpy(temp,"hits you in Right Hand!");
							break;
						default:
							if (damage > 1) strcpy(temp,"hits you in Right Hand!");
							break;
						}
					}
					sprintf(temp2,"%s %s",chars[attack].name, temp);//AntiChrist
					if (!chars[defend].npc) sysmessage(s2, temp2); //kolours -- hit display
				//} Removed by Magius(CHE)
			}
			//printf("\n%s Hitten in %i\n",chars[defend].name,x);
			x=CalcDef(defend,x);
			
			// Magius(CHE) - For armour absorbtion system
			maxabs=20; //
			           // there are monsters with DEF >20, this makes them undefeatable
			maxnohabs=100;
			if (server_data.maxabsorbtion>0) maxabs=server_data.maxabsorbtion;
			else {
				printf("SERVER.SCP:CombatHit() Error in MAX_ABSORBTION. Reset to Deafult (20).\n");
				server_data.maxabsorbtion=maxabs;
			}
			if (server_data.maxnohabsorbtion>0) maxnohabs=server_data.maxnohabsorbtion;
			else {
				printf("SERVER.SCP:CombatHit() Error in MAX_NON_HUMAN_ABSORBTION. Reset to Deafult (100).\n");
				server_data.maxnohabsorbtion=maxnohabs;
			}
			if (!ishuman(defend)) maxabs=maxnohabs;
			//printf("Damage Before Abs: %i\n",damage);
			tmpj=(int) (damage*x)/maxabs; // Absorbtion by Magius(CHE)
			damage=damage-tmpj;
			if (damage<0) damage=0;
			//printf("MaxAbs: %i\nArmour: %i\nDamage: %i\nSubstract: %i\n",maxabs,x,damage,tmpj);
			if (chars[defend].npc==0) damage=damage/server_data.npcdamage; // Rate damage against other players
			// End Armour Absorbtion by Magius(CHE) (See alse reactive armour spell damage)

			if (!chars[attack].npc)//Zippy
					ItemCastSpell(s1,defend,weapon);

			//AntiChrist - 26/10/99
			//when hitten and damage >1, defender fails if casting a spell!
			if(damage>1 && !chars[defend].npc)//only if damage>1 and against a player
			{
				if( chars[defend].casting > 0 && currentSpellType[s2] == 0 )
				{//if casting a normal spell (scroll: no concentration loosen)
					chars[defend].spellCast = -1;
					chars[defend].casting = 0;
					chars[defend].priv2 = chars[defend].priv2&0xfd;
					sysmessage( s2, "Your concentration has been broken" );
				}
			}

			if(damage>0)
			{
				if (chars[defend].ra) // For reactive armor spell
				{
					int damage1;
					damage1=(int)( damage*(chars[defend].skill[MAGERY]/2000.0));
					chars[defend].hp -= damage-damage1;
					if (chars[defend].npc) damage1 = damage1 * server_data.npcdamage;
					chars[attack].hp -= damage1;
					staticeffect( defend, 0x37, 0x4A, 0, 15 );
				}
				else chars[defend].hp-=damage;                 // Remove damage
				/////////  For Splitting NPCs ///  McCleod
				if ((chars[defend].split>0)&&(chars[defend].hp>=1))
				{
					if (rand()%100<=chars[defend].splitchnc)
					{
						if (chars[defend].split==1) splitnum=1;
						else splitnum=rand()%chars[defend].split+1;
						
						for (splitcount=0;splitcount<splitnum;splitcount++)
							Npcs->Split(defend);
					}
				}
				////////      End of spliting NPCs
			}
			if (!chars[attack].npc)
				if(((fightskill==ARCHERY)&&(los))||(fightskill!=ARCHERY))
					doSoundEffect(attack, fightskill, weapon);//AntiChrist
			if( chars[defend].hp < 0 ) 
				chars[defend].hp = 0;
			updatestats( defend, 0 );
			x = (chars[defend].id1<<8) + chars[defend].id2;
			if( x >= 0x0190 )
			{
				if( !chars[defend].onhorse ) 
					npcaction( defend, 0x14 );
			}
		}
	}
}

void cCombat::DoCombat( int attack, unsigned int currenttime )
{
	int x, defend, fightskill, spattacks, spattackbit, arrowsquant, bowtype = 0;
	int j, los;
	int weapon = GetWeapon( attack );

	if (attack < 0 || attack >= cmem)
	{
		LogMessage("combat() - bad attacker (%i)\n"_ attack);
		return;
	}
	int s1 = calcSocketFromChar( attack ), s2 = -1;
	if( chars[attack].free ) return;
	defend = chars[attack].targ;
	if( defend == -1 )		// we may as well dump completely out of the this routine if there's no defender!
	{
		if( chars[attack].war )
		{
			chars[attack].war = 0;
			chars[attack].attacker = -1;
			chars[attack].attackfirst = 0;
		}
		return;
	}

	if( chars[defend].dead || chars[defend].hp <= 0 || chars[defend].priv&0x04 )
	{	// invalidate attacker here!!!
		if( chars[defend].npc && chars[defend].war && chars[defend].priv&0x04 )
			npcToggleCombat( defend );
		if(chars[attack].npcaitype == 4)
		{
			chars[attack].summontimer = (uiCurrentTime+(CLOCKS_PER_SEC*20));
			chars[attack].npcWander = 2;
			chars[attack].npcmovetime = (unsigned int) ((uiCurrentTime+double(NPCSPEED*CLOCKS_PER_SEC)));
			npctalkall(attack,"Thou have suffered thy punishment, scoundrel.", 0);
		}
		chars[attack].targ = -1;
		if( chars[attack].attacker > -1 && chars[attack].attacker < cmem )
		{
			chars[chars[attack].attacker].attackfirst=0;
			chars[chars[attack].attacker].attacker=-1;
		}
		chars[attack].attacker=-1;
		chars[attack].attackfirst=0;
		if( chars[attack].npc && !chars[attack].dead && chars[attack].npcaitype != 17 && chars[attack].war ) 
			npcToggleCombat( attack );
		return;
	}

	if( ( chars[attack].npc || online( attack ) ) && defend != -1 )
	{			
		if( defend <= -1 || defend >= cmem ) 
			return;
		if( chars[defend].free ) 
			return;

		s2 = calcSocketFromChar( defend );
		int playerDistance = chardist( attack, defend );
		if( playerDistance <= 1 && abs( chars[attack].z = chars[defend].z ) <= MaxZstep )
			los = 1;
		else
			los = line_of_sight( -1, chars[attack].x, chars[attack].y, chars[attack].z, chars[defend].x,chars[defend].y,chars[defend].z,WALLS_CHIMNEYS+DOORS+FLOORS_FLAT_ROOFING);
		
		if( los == 0 )
   		 return;
		
		if ((chars[defend].npc)&&(chars[defend].npcaitype!=17)|| (online(defend)&&(!chars[defend].dead)))
		{
			if( playerDistance >= 20 )
			{
				if ((chars[attack].npcaitype==4)&&((region[chars[attack].region].priv&1)))
				{
					mapRegions->RemoveItem( attack + 1000000 );

					chars[attack].x = chars[defend].x;
					chars[attack].y = chars[defend].y;
					chars[attack].dispz = chars[attack].z = chars[defend].z;

					mapRegions->AddItem( attack + 1000000 );

					teleport( attack );
					soundeffect2( attack, 0x01, 0xFE );
					staticeffect( attack, 0x37, 0x2A, 0x09, 0x06 );
					npctalkall(attack,"Halt, scoundrel!", 1);
					npcattacktarget( defend, attack );
				}
				else 
				{
					chars[attack].targ=-1;
					if (chars[attack].attacker>=0 && chars[attack].attacker<cmem)
					{
						chars[chars[attack].attacker].attackfirst=0;
						chars[chars[attack].attacker].attacker=-1;
					}
					chars[attack].attacker=-1;
					chars[attack].attackfirst=0;
					if( chars[attack].npc && chars[attack].npcaitype!=17 && !chars[attack].dead && chars[attack].war ) npcToggleCombat( attack );  // ripper
				}
			}
			if( playerDistance < combat.maxRange )
			{
				if( chars[defend].targ == -1 )
				{
					npcsimpleattacktarget( attack, defend );
					x = (int) (((( 100 - chars[attack].dx ) * CLOCKS_PER_SEC ) /25 ) + ( 1 * CLOCKS_PER_SEC ));
					chars[attack].timeout=uiCurrentTime + x;
					return;
				}
				if (TimerOk(attack))
				{
					fightskill = Skills->GetCombatSkill( attack );
					x = 0;
					if( fightskill == ARCHERY )
					{
						if (los)
						{
							bowtype = GetBowType( attack );
							if( bowtype == 1 ) arrowsquant = getamount( attack, 0x0F, 0x3F );
							else arrowsquant = getamount( attack, 0x1B, 0xFB );
							if( arrowsquant > 0 ) x = 1;
							if( arrowsquant <= 0 ) sysmessage( s1, "You are out of ammunitions!" );

						}
					}
					if( ( playerDistance < 2 ) && ( fightskill != ARCHERY ) ) x = 1;
					if( x )
					{
						// - Do stamina maths - AntiChrist (6) -
						if(abs(server_data.attackstamina)>0 && !(chars[attack].priv&1))
						{
							if((server_data.attackstamina<0)&&(chars[attack].stm<abs(server_data.attackstamina)))
							{
       							sysmessage(s1,"You are too tired to attack.");
								j = weapon; // Get the number of the weapon
								if (j!=-1) 
								{ 
									if (items[j].spd==0) items[j].spd=35;
								    x = (int) (15000 / ((chars[attack].dx+100) * items[j].spd)*CLOCKS_PER_SEC); //Calculate combat delay
								}
								else 
								{
									j=30;
									if(chars[attack].skill[WRESTLING]>200) j=35;
									if(chars[attack].skill[WRESTLING]>400) j=40;
									if(chars[attack].skill[WRESTLING]>600) j=45;
									if(chars[attack].skill[WRESTLING]>800) j=50;
									x = (int) (15000 / ((chars[attack].dx+100) * j) * CLOCKS_PER_SEC);
								}
								chars[attack].timeout=uiCurrentTime+x;
       							return;
       						}
        					chars[attack].stm += server_data.attackstamina;
							if (chars[attack].stm>chars[attack].dx) chars[attack].stm=chars[attack].dx;
							if (chars[attack].stm<0) chars[attack].stm=0;
        					updatestats(attack,2); 
							//printf("ATTACKSTAMINA %i\n",server_data.attackstamina);
							// --------ATTACK STAMINA END ------
						}

						x=(chars[attack].id1<<8)+chars[attack].id2;
						int cc,aa,bb;
						if (x<0x0190)
						{
							aa=4+(rand()%3); // bugfix, LB, some creatures dont have animation #4
                            cc=(creatures[x].who_am_i)&0x2; // anti blink bit set ?
	                        if (cc==2)
							{
                              aa++;
							  if (x==5) // eagles need special treatment
							  {
                                bb=rand()%3;
								switch (bb)
								{
									case 0: aa=0x1;  break;
									case 1: aa=0x14; break;
									case 2: aa=0x4;  break;
								}
							  }

							}

							npcaction( attack, aa ); 
							playmonstersound( attack, chars[attack].id1, chars[attack].id2, SND_ATTACK );

						}
						else if (chars[attack].onhorse)
							CombatOnHorse( attack );
						else
							CombatOnFoot( attack );
						if (fightskill==ARCHERY)
						{
							if (los)
							{
								if (bowtype==1)
								{
									delequan(attack, 0x0F, 0x3F, 1);
									movingeffect(attack, defend, 0x0F, 0x42, 0x08, 0x00, 0x00);
								}
								else
								{
									delequan(attack, 0x1B, 0xFB, 1);
									movingeffect(attack, defend, 0x1B, 0xFE, 0x08, 0x00, 0x00);
								}
							}
						}
						// Formulas take from OSI's combat formulas
						// attack speed should be determined here.
						// attack speed = 15000 / ((DEX+100) * weapon speed)
						if( ( ( playerDistance < 2 ) || ( fightskill == ARCHERY ) ) && !(chars[attack].npcaitype==4)) // changed from 0x40 to 4
                        {
							if (los)
							{
								if( chars[defend].targ == -1 )
									npcsimpleattacktarget( attack, defend );
							}
						}
						if( fightskill == ARCHERY ) 
							CombatHit( attack, defend, currenttime );
						else 
							chars[attack].swingtarg = defend;
  					}
					j = weapon; //Get the number of the weapon
					if (j!=-1)
					{
					    if (items[j].spd==0) items[j].spd=35;
						x = (int) (15000 / ((chars[attack].dx+100) * items[j].spd)*CLOCKS_PER_SEC); //Calculate combat delay
					}
					
					if (j==-1)
					{
						j=30;
						if(chars[attack].skill[WRESTLING]>200) j = 35;
						if(chars[attack].skill[WRESTLING]>400) j = 40;
						if(chars[attack].skill[WRESTLING]>600) j = 45;
						if(chars[attack].skill[WRESTLING]>800) j = 50;
						x = (int) (15000 / ((chars[attack].dx + 100) * j) * CLOCKS_PER_SEC);
					}
					chars[attack].timeout = uiCurrentTime + x;
					if (fightskill!=ARCHERY) CombatHit(attack,defend,currenttime); 
				}
			}

			if( chars[defend].hp < 1 )//Highlight //Repsys
			{
				Kill( attack, defend );
				return;
			}

			if( !(chars[defend].priv&4) )
			{
				if( chars[attack].spatimer<=currenttime)
				{
					spattacks = numbitsset( chars[attack].spattack );
					if ((!chars[defend].dead)&&(chardist(attack,defend)<combat.maxRangeSpell)&&( spattacks > 0 ))
					{
						if (los)
						{
							spattackbit=rand()%(spattacks) + 1;
							switch(whichbit(chars[attack].spattack, spattackbit))
							{
							case 1:
								if (chars[attack].mn>=4)
								{
									npcaction(attack, 6);
									Magic->NPCMagicArrowTarget(attack,defend);
								}
								break;
							case 2:
								if (chars[attack].mn>=6)
								{
									npcaction(attack, 6);
									Magic->NPCHarmTarget(attack,defend);
								}
								break;
							case 3:
								if (chars[attack].mn>=4)
								{
									npcaction(attack, 6);
									Magic->NPCClumsyTarget(attack,defend);
								}
								break;
							case 4:
								if (chars[attack].mn>=4)
								{
									npcaction(attack,  6);
									Magic->NPCFeebleMindTarget(attack, defend);
								}
								break;
							case 5:
								if (chars[attack].mn>=4)
								{
									npcaction(attack,  6);
									Magic->NPCWeakenTarget(attack, defend);
								}
								break;
							case 6:
								if (chars[attack].mn>=9)
								{
									npcaction(attack,  6);
									Magic->NPCFireballTarget(attack, defend);
								}
								break;
							case 7:
								if (chars[attack].mn>=11)
								{
									npcaction(attack,  6);
									Magic->NPCCurseTarget(attack, defend);
								}
								break;
							case 8:
								if (chars[attack].mn>=11)
								{
									npcaction(attack,  6);
									Magic->NPCLightningTarget(attack, defend);
								}
								break;
							case 9:
								if (chars[attack].mn>=14)
								{
									npcaction(attack,  6);
									Magic->NPCParalyzeTarget(attack, defend);
								}
								break;
							case 10:
								if (chars[attack].mn>=14)
								{
									npcaction(attack,  6);
									Magic->NPCMindBlastTarget(attack, defend);
								}
								break;
							case 11:
								if (chars[attack].mn>=20)
								{
									npcaction(attack,  6);
									Magic->NPCEBoltTarget(attack, defend);
								}
								break;
							case 12:
								if (chars[attack].mn>=20)
								{
									npcaction(attack,  6);
									Magic->NPCExplosionTarget(attack, defend);
								}
								break;
							case 13:
								if (chars[attack].mn>=40)
								{
									npcaction(attack,  6);
									Magic->NPCFlameStrikeTarget(attack, defend);
								}
								break;
							case 14:
								npcaction(attack,  6);
								Magic->PFireballTarget(attack,  defend, 10);
								break;
							case 15:
								npcaction(attack,  6);
								Magic->PFireballTarget(attack,  defend, 20);
								break;
							case 16:
								npcaction(attack,  6);
								Magic->PFireballTarget(attack,  defend, 40);
								break;
							case 17:
								Magic->NPCDispel( attack, defend );
								break;
							}
						}			
					}
					chars[attack].spatimer= (unsigned int) (currenttime + (chars[attack].spadelay * CLOCKS_PER_SEC)); 
				}
			}
		}
	}
}

int cCombat::CalcAtt(int p) // Calculate total attack powerer
{
	char p1, p2, p3, p4;
	int total=0, i, j,serial,serhash,ci;
	
	if(chars[p].npc==1) 
	{
		//Compatibility for use with older scripts
		if((chars[p].lodamage==0)&&(chars[p].hidamage==0)) return(chars[p].att);
		
		//Code for new script commands
		if(chars[p].lodamage==chars[p].hidamage) //Avoids divide by zero error
		{
			return(chars[p].lodamage);
		}
		if(!(chars[p].hidamage<=2))
		{
			j=chars[p].hidamage-chars[p].lodamage;
			total=(rand()%(j))+chars[p].lodamage;
		}
		
		if(total<1) total=1;
		return(total);
	}
	
	p1=chars[p].ser1;
	p2=chars[p].ser2;
	p3=chars[p].ser3;
	p4=chars[p].ser4;
	serial=chars[p].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1) 
			if ((items[i].contserial==serial) && ((items[i].layer==1) || (items[i].layer==2)))
		{
			if (items[i].lodamage == items[i].hidamage) total+=items[i].lodamage;
			else if ((items[i].lodamage>0)&&(items[i].hidamage>0))
			{
				total+=(rand()%(items[i].hidamage-items[i].lodamage))+(items[i].lodamage);
			}
			else if ((items[i].lodamage==0)&&(items[i].hidamage==0)) total+=items[i].att;
		}
	}
	if (total<1) total=1;
	return total;
}

int cCombat::CalcDef(int p,int x) // Calculate total defense power
{
	char p1, p2, p3, p4;
	int i=-1, j=-1, k,serial,serhash,ci;
	int total;
	tile_st tile;
	
	total=chars[p].def;
	p1=chars[p].ser1;
	p2=chars[p].ser2;
	p3=chars[p].ser3;
	p4=chars[p].ser4;
	serial=chars[p].serial;
	serhash=serial%HASHMAX;
	if( ishuman(p)) { // Added by Magius(CHE)
		for (ci=0;ci<contsp[serhash].max;ci++)
		{
			i=contsp[serhash].pointer[ci];
			if (i!=-1) 
				if((items[i].contserial==serial) && (items[i].layer>1) && (items[i].layer<25))
				{
					if (x==0) total+=items[i].def;
					else
					{
						switch (items[i].layer)
						{
						case 5:
						case 13:
						case 17:
						case 20:
						case 22: if (x==1 && items[i].def>total) 
								 {
									 total=items[i].def;
									 j=i;
								 }
							break;
						case 19: if (x==2 && items[i].def>total)
								 {
									 total=items[i].def;
									 j=i;
								 }
							break;
						case 6:  if (x==3 && items[i].def>total) 
								 {
									 total=items[i].def; 
									 j=i;
								 }
							break;
						case 3:
						case 4:
						case 12:
						case 23:
						case 24: if (x==4 && items[i].def>total) 
								 {
									 total=items[i].def;
									 j=i;
								 }
							break;
						case 10: if (x==5 && items[i].def>total) 
								 {
									 total=items[i].def;
									 j=i;
								 }
							break;
						case 7:  if (x==6 && items[i].def>total) 
								 {
									 total=items[i].def; 
									 j=i;
								 }
							break;
						default:
							break;
						}
					}
				}
			}
	} else j = -1;
	k=calcSocketFromChar(p);
	if( j != -1 && k != -1 )
	{
		if( items[j].def > 0 )
		{
			if( ( rand()%2 ) == 0 ) 
				items[j].hp--; //Take off a hit point
		}
		if( items[j].hp <= 0 )
		{
			if (items[j].name[0]!='#') //AntiChrist BUGFIX
			{
				sprintf(temp,"Your %s has been destroyed",items[j].name);
			}
			else
			{
				Map->SeekTile((items[j].id1<<8)+items[j].id2, &tile);
				sprintf(temp,"Your %s has been destroyed",tile.name);
			}
			sysmessage(k,temp);
			Items->DeleItem(j);
		}
		statwindow(k,currchar[k]);
	}
	if (total<2) total=2;
	return total;
}

int cCombat::TimerOk(int c)
{
	int d=0;
//	if (chars[c].timeout<getclock()) d=1;
	if( chars[c].timeout < uiCurrentTime ) d=1;
	if (overflow) d=1;
	return d;
}

int cCombat::GetWeapon(int i)
{
	int j,serial,serhash,ci;
	
	serial=chars[i].serial;
	serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		j=contsp[serhash].pointer[ci];
		if (j!=-1)
			if ( ( items[j].contserial == serial ) && ( (items[j].layer == 1) || (( items[j].layer == 2 ) && (Skills->GetShield( i ) == -1)) ) ) // Morrolan don't check for shields
			{
				return (j);
			}
	}
	return(-1);
}

//play animation for weapon in hand during combat on a horse //kolours - modularized (09/20/98)
void cCombat::CombatOnHorse(int i)
{
	int j = GetWeapon( i ); //find out which weapon the player is holding
	unsigned short realID;
	if( j != -1 )
	{
		realID = (items[j].id1<<8) + items[j].id2;
		if( realID == 0x13B2 )
		{
			npcaction( i, 0x1B );	// bow
			return;
		}
		if( realID == 0x0F4F || realID == 0x13FD ) // xbow, hxbow
		{
			npcaction(i, 0x1C); //crossbow - regular
			return;
		}
		if( ( realID >= 0x13B5 && realID <= 0x13BA ) || //swords 
	 	( realID >= 0x0EC1 && realID <= 0x0EC5 ) || //swords
		( realID >= 0x0F5E && realID <= 0x0F61 ) || //swords
		( realID == 0x13FA || realID == 0x13FB ) || //swords
		( realID == 0x13FE || realID == 0x13FF ) || //swords
		( realID >= 0x0DF0 && realID <= 0x0DF5 ) || //maces
		( realID == 0x13B3 || realID == 0x13B4 ) || //maces
		( realID == 0x0F5C || realID == 0x0F5D ) || //maces
		( realID == 0x0FB4 || realID == 0x0FB5 ) || //maces
		( realID == 0x13AF || realID == 0x13B0 ) || //maces
		( realID == 0x13E3 || realID == 0x13E4 ) || //maces
		( realID == 0x13F4 || realID == 0x13F5 ) || //maces
		( realID == 0x13F8 || realID == 0x13F9 ) || //maces
		( realID == 0x143C ) ||                        //maces - hammer pick
		( realID == 0x1406 || realID == 0x1407 ) || //maces - 1H war mace
		( realID == 0x0E81 || realID == 0x0E82 ) || //maces
		( realID == 0x0E89 ) ||  //maces
		( realID >= 0x1400 && realID <= 0x1405 ) || //fencing
		( realID == 0x0F51 || realID == 0x0F52 ) ) //fencing - kryss
		{
			npcaction(i, 0x1A);
			return;
		}
		if( ( realID >= 0x0F43 && realID <= 0x0F4E ) || //2H axes 
			( realID >= 0x143E && realID <= 0x1443 ) || //halberd
			( realID == 0x0F62 || realID == 0x0F63 ) || //fencing - spear
			( realID == 0x0E87 || realID == 0x0E88 ) ||
			( realID == 0x0E89 || realID == 0x0E8A ) ||
//			(items[j].id2==0xF0) || //Pitchfork	????????
			( realID == 0x1438 ) ||  //War Hammer
			( realID == 0x1439 ) ) //War Hammer
		{
			npcaction(i, 0x1D); //2H axes
			return;
		}
	} // end of !=-1
	else
	{
		npcaction( i, 0x1A ); //fist fighting
		return;
	}
	
}

//play animation for weapon in hand for combat on foot //kolours - (09/20/98)
void cCombat::CombatOnFoot(int i)
{
	int j = GetWeapon( i ); //identify the weapon the player is holding
	int m = rand()%4; //randomize the animation sequence
	if( j != -1 )
	{
		unsigned short realID = (items[j].id1<<8) + items[j].id2;
		if( realID == 0x13B2 ) 
		{
			npcaction( i, 0x12 ); //bow
			return;
		}
		if( realID == 0x0F4F || realID == 0x13FD ) //xbow //xhbow
		{
			npcaction( i, 0x13 ); //crossbow - regular
			return;
		}
		if( ( realID >= 0x13B5 && realID <= 0x13BA ) || //1H swords 
			( realID >= 0x0F5E && realID <= 0x0F61 ) || //1H swords
			( realID >= 0x0EC2 && realID <= 0x0EC3 ) || //1H swords
			( realID >= 0x1440 && realID <= 0x1441 ) || //1H swords
			( realID == 0x13FA)||						//1H swords
			( realID == 0x13FE || realID == 0x13FF ) ) //1H swords
		{
			switch( m ) //swords
			{
				case 0:
				case 1:
					npcaction( i, 0x0A ); //poke
					return;
				default:
					npcaction( i, 0x09 ); //top-down swing
					return;
			}
		}
		if( ( realID >= 0x0DF2 && realID <= 0x0DF5 ) || //maces 
			( realID == 0x13B3 || realID == 0x13B4 ) || //maces
			( realID == 0x0F5C || realID == 0x0F5D ) || //maces
			( realID == 0x0FB4 || realID == 0x0FB5 ) || //maces
			( realID == 0x13E3 || realID == 0x13E4 ) || //maces
			( realID == 0x0E85 || realID == 0x0E86 ) || //pickaxes
			( realID >= 0x143A && realID <= 0x143D ) ||//maces - hammer pick
			( realID == 0x1406 || realID == 0x1407 ) ) //maces
		{
			switch( m ) //maces
			{
			case 0:
				npcaction( i, 0x0B ); //side swing
				return;
			default:
				npcaction( i, 0x09 ); //top-down swing
				return;
			}
		}
		if( ( realID == 0x0E89 || realID == 0x0E8A ) || //QSTAFF - Chaos
			( realID == 0x13AF || realID == 0x13B0 ) || //2 Handed Maces
			( realID == 0x13F4 || realID == 0x13F5 ) || //2 Handed Maces
			( realID == 0x13F8 || realID == 0x13F9 ) || //2 Handed Maces
			( realID == 0x0E81 || realID == 0x0E82 ) || //2 Handed Maces
			( realID >= 0x1438 && realID <= 0x1439 ) || //2 Handed Maces!
			( realID >= 0x143E && realID <= 0x143F ) || //Halberd
			( realID >= 0x1442 && realID <= 0x1443 ) ||
			( realID >= 0x0F43 && realID <= 0x0F4E ) || //2H Axes
			( realID == 0x13FB ) ||	
			( realID == 0x0DF0 || realID == 0x0DF1 ) ) //Black Staff - Chaos
		{
			switch( m ) //pole arms & War Axes
			{
			case 0:
			case 1:
				npcaction( i, 0x0C ); //2H Top Down
				return;
			default:
				npcaction( i, 0x0D ); //2H Side Swing
				return;
			}		
		}
		if( realID >= 0x1400 && realID <=  0x1403 ) //kryss - fencing
		{
			npcaction( i, 0x0A ); //poking :)
			return;
		}
		if( ( realID >= 0x1404 && realID <= 0x1405 ) || //Warforks(Fencing)
			( realID == 0x0EC4 || realID == 0x0EC5 ) || //skinning knife
			( realID == 0x0F51 || realID == 0x0F52 ) || //daggers
			( realID == 0x13F6 || realID == 0x13F7 ) ) //Butchers knife
		{
			switch( m ) //Warforks & Daggers
			{
			case 0:
				npcaction( i, 0x09 ); //top-down swing
				return;
			default:
				npcaction( i, 0x0A ); //poke
				return;
			}
		}
		if( ( realID == 0x0E87 || realID == 0x0E88 ) || //pitchfork 
			( realID == 0x0F62 || realID == 0x0F63 ) ) //spear
		{
			npcaction( i, 0x0E ); //default: 2-handed poke
			return;
		}
	}
	else
	{
		switch( m ) //fist fighting
		{
		case 0:
			npcaction( i, 0x0A ); //fist straight-punch
			return;
		case 1:
			npcaction( i, 0x1F ); //fist top-down
			return;
		default:
			npcaction( i, 0x09 ); //default: //fist over-head
			return;
		}
	}
}
//s: char#
void cCombat::SpawnGuard( CHARACTER s, CHARACTER i, int x, int y, signed char z)
{
	int t,c, j;
	char b;
	//printf("spawnguard called !!\n");

	if( i < 0 || i >= cmem || s < 0 || s >= cmem ) return;
	j = i;
	i=chars[i].region; 
	b=chars[s].priv&4;

	if(!(region[chars[j].region].priv&0x01 == 1 ) ) return;
	if( server_data.guardsactive && !b ) // bugfix, lb
	{
		t=region[i].guardnum[(rand()%10)+1];
		c=Npcs->AddNPCxyz( calcSocketFromChar(s), t, 0, x, y, z); 

		if( c!=-1)
		{
			chars[c].npcaitype=4; // CITY GUARD, LB, bugfix, was 0x40 -> not existing
			chars[c].attackfirst=1;
			chars[c].attacker = s;
			chars[c].targ = s;
			chars[c].npcWander=2;  //set wander mode Tauriel
			npcToggleCombat(c);
			chars[c].npcmovetime = (unsigned int) (uiCurrentTime + (NPCSPEED * CLOCKS_PER_SEC));
			chars[c].summontimer = (int) (uiCurrentTime + CLOCKS_PER_SEC*25);

			soundeffect2(c, 0x01, 0xFE);  //Tauriel 1-9-99 changed to stop crashing used to call soundeffect (expeted socket)
			staticeffect(c, 0x37, 0x2A, 0x09, 0x06);
			
			updatechar(c);
			npctalkall(c,"Thou shalt regret thine actions, swine!", 1);
		}
	}
}

void cCombat::ItemSpell(int attaker, int defender)
{
	int i,serial,serhash,ci;
	currentSpellType[attaker]=2;
	serial=chars[attaker].serial;
    serhash=serial%HASHMAX;
	for (ci=0;ci<contsp[serhash].max;ci++)
	{
		i=contsp[serhash].pointer[ci];
		if (i!=-1)
		if ((items[i].contserial==serial) && 
			((items[i].layer==1 && items[i].type!=9) || (items[i].layer==2)))
		{
			if (items[i].offspell && (items[i].att||items[i].hidamage) && items[i].type == 15)
			{
				switch(items[i].offspell)
				{
				case 1:
					{
						Magic->ClumsySpellItem(attaker,defender);
						break;
					}
				case 2:
					{
						Magic->FeebleMindSpellItem(attaker,defender);
						break;
					}
				case 3:
					{
						Magic->MagicArrowSpellItem(attaker,defender);
						break;
					}
				case 4:
					{
						Magic->WeakenSpellItem(attaker,defender);
						break;
					}
				case 5:
					{
						Magic->HarmSpellItem(attaker,defender);
						break;
					}
				case 6:
					{
						Magic->FireballSpellItem(attaker,defender);
						break;
					}
				case 8:
					{
						Magic->CurseSpellItem(attaker,defender);
						break;
					}
				case 9:
					{
						Magic->LightningSpellItem(attaker,defender);
						break;
					}
				case 11:
					{
						Magic->MindBlastSpellItem(attaker,defender);
						break;
					}
				case 12:
					{
						Magic->ParalyzeSpellItem(attaker,defender);
						break;
					}
				case 14:
					{
						Magic->ExplosionSpellItem(attaker,defender);
						break;
					}
				case 15:
					{
						Magic->FlameStrikeSpellItem(attaker,defender);
						break;
					}
					
				}
				items[i].morez--;
				if (items[i].morez == 0)
				{
					items[i].type = items[i].type2;
					items[i].morex = 0;
					items[i].morey = 0;
					items[i].offspell = 0;
				}
			}
			return;
		}
    }
}

bool cCombat::weapon2Handed( int weapon )
//PRE:			true
//POST:			returns true if weapon 2 handed, otherwise false
//DATE:			16 Sept 1999
//PROGRAMMER:	Abaddon
{
	if( weapon == -1 ) return false;
	if ((items[weapon].id1==0x0F)&&(items[weapon].id2==0x4F) || //xbow 
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xFD)  ) //hxbow
		return true;
	if ((items[weapon].id1==0x13)&&(items[weapon].id2==0xB2))
		return true;
	if ((items[weapon].id1==0x0F)&&(items[weapon].id2>=0x43)&&(items[weapon].id2<=0x4E) || //2H axes 
		(items[weapon].id1==0x14)&&(items[weapon].id2>=0x3E)&&(items[weapon].id2<=0x43) || //halberd
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x62)||(items[weapon].id2==0x63) || //fencing - spear
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xFA)||(items[weapon].id2==0xFB) || 
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0x87)||(items[weapon].id2==0x88)  ) //fencing - pitchfork
		return true;
	return false;
}

//AntiChrist - do the sound effect ( only if HITTEN! )
void cCombat::doSoundEffect(CHARACTER p, int fightskill, ITEM weapon)
{
	int a=rand()%4;
	if( weapon == -1 )
	{
		switch( a )
		{
		case 0:	soundeffect2(p, 0x01, 0x35);	break;
		case 1: soundeffect2(p, 0x01, 0x37);	break;
		case 2: soundeffect2(p, 0x01, 0x3D);	break;
		default:	soundeffect2(p, 0x01, 0x3B);	break;
		}
		return;
	}
	/*
		Ok People...Its my newbie training time.  Ive never ever attempted to rewrite 
		an entire function before, but here goes.

		Dont complain if its messed up, just email me at chaos@legionsofchaos.com
		Cheerio,
		
		-=Chaos=-
	*/

	// Begin Woosh Weapons (If you alter this, please remember ID1 is the second to last 
	// 2 digits from Inside UO, and ID2 are the last 2)
	// I will list all weapons possible from Inside UO.
	// Also, hopefully this is very easy for the future programmers to read....
	if ((items[weapon].id1==0x0E)&&(items[weapon].id2==0x8A) || //Qstaff
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0x89) || //Qstaff
		(items[weapon].id1==0x0D)&&(items[weapon].id2==0xF0) || //Black Staff
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0x81) || //Sheperd's crook
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0x82) || //Sheperd's crook
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0x85) || //Pick Axe
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0x86) || //Pick Axe
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0xC2) || //Cleaver
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0xC3) || //Cleaver
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x43) || //Hatchet
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x44) || //Hatchet
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x45) || //Executioner's Axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x46) || //Executioner's Axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x47) || //Battle Axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x48) || //Battle Axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x49) || //axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x4A) || //axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x4B) || //Dbl Axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x4C) || //Dbl Axe
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x5C) || //Mace
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x5D) || //Mace
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0xB4) || //Sledge Hammer
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0xB5) || //Sledge Hammer
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xAF) || //War Axe
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xB0) || //War Axe
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xB3) || //Club
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xB4) || //Club
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xE3) || //Smith Hammer
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xE4) || //Smith Hammer
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xF4) || //Crook
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xF5) || //Crook
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xF8) || //Gnarled Staff
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xF9) || //Gnarled Staff
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xFA) || //Battle Axe
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xFB) || //Battle Axe
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x06) || //War Mace
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x07) || //War Mace
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x38) || //War Hammer
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x39) || //War Hammer
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x3A) || //Maul
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x3B) || //Maul
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x3C) || //Hammer Pick
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x3D) || //Hammer Pick
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x42) || //2 Handed Axe
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x43)) //2 Handed Axe
	{
		soundeffect2(p, 0x02, 0x32);
	}
	//End Woosh Weapons

	//Begin Stabbing weapons
	if ((items[weapon].id1==0x0E)&&(items[weapon].id2==0x87) || //Pitchfork
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0x88) || //Pitchfork
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x51) || //Dagger
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x52) || //Dagger
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x62) || //Spear
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x63) || //Spear
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x00) || //Kryss
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x01) || //Kryss
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x02) || //Short Spear
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x03) || //Short Spear
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x04) || //War Fork
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x05) || //War Fork
		//Begin Swords that slash 
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0xC4) || //Skinning Knife
		(items[weapon].id1==0x0E)&&(items[weapon].id2==0xC5) || //Skinning Knife
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x60) || //Long Sword
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x61) || //Long Sword
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xB7) || //Long Sword
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xB8) || //Long Sword
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x40) || //Cutlass
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x41) )  //Cutlass
	{
		soundeffect2(p, 0x02, 0x3C);
	}
	//End Stabbing Weapons

	// Begin Bardiche Sounds
	if ((items[weapon].id1==0x0F)&&(items[weapon].id2==0x4D) || //Bardiche
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x4E) )  //Bardiche
	{
		soundeffect2(p, 0x02, 0x36); 
	}
	// End Bardiche Sounds

	// Begin Default Sword Sounds
	if ((items[weapon].id1==0x13)&&(items[weapon].id2==0xFE) || //Katana
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xFF) || //Katana
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xF6) || //Butcher Knife
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xF7) ) //Butcher Knife
	{
		soundeffect2(p, 0x02, 0x3B);
	}
	//End Default Swords

	// Start Viking Sword
	if( ( items[weapon].id1 == 0x13 ) && ( items[weapon].id2 == 0xB9 ) || //Viking Sword
		( items[weapon].id1 == 0x13 ) && ( items[weapon].id2 == 0xBA ) ) // Viking Sword
	{
		soundeffect2( p, 0x02, 0x3C );
	}
	// End Viking Sword // kkung

	//Begin Large Swords
	if ((items[weapon].id1==0x0F)&&(items[weapon].id2==0x5E) || //Broadsword
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x5F) || //Broadsword
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x3E) || //Halberd
		(items[weapon].id1==0x14)&&(items[weapon].id2==0x3F) )  //Halberd
	{
		soundeffect2(p, 0x02, 0x37);
	}

	if ((items[weapon].id1==0x0F)&&(items[weapon].id2==0x4F) || //Xbow
		(items[weapon].id1==0x0F)&&(items[weapon].id2==0x50) || //Xbow
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xB1) || //Bow
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xB2) || //Bow
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xFD) || //Heavy Xbow
		(items[weapon].id1==0x13)&&(items[weapon].id2==0xFE) )  //Heavy Xbow
	{
		soundeffect2(p, 0x02, 0x23);
	}
}


//AntiChrist - do the "MISSED" sound effect
void cCombat::doMissedSoundEffect(CHARACTER p)
{
	int a=rand()%3;

	if (a==0) soundeffect2(p, 0x02, 0x38);
	else if (a==1) soundeffect2(p, 0x02, 0x39);
	else soundeffect2(p, 0x02, 0x3A);
}

void Kill( CHARACTER attack, CHARACTER defend )
{
	Karma( attack, defend, ( 0 - ( chars[defend].karma ) ) );
	Fame( attack, chars[defend].fame );
	if( chars[attack].npcaitype == 4 && chars[defend].npc )  // changed from 0x40 to 4
	{
		unsigned short xid = (chars[defend].xid1<<8) + chars[defend].xid2;
		npcaction( defend, 0x15 );
		if( xid == 0x0191 )	// Female death
			soundeffect2( defend, 0x01, 0x51 ); 
		else if( xid == 0x0190 )	// Male death
			soundeffect2( defend, 0x01, 0x5A ); // Male Death
		else 
			playmonstersound( defend, chars[defend].xid1, chars[defend].xid2, SND_DIE );

		Npcs->DeleteChar( defend );//Guards, don't give body
		npcToggleCombat( attack );
		return;	// this would prove bad if it was deleted, and then we tried to do logic to it!
	} 
	//murder count \/

	if( !chars[attack].npc && chars[attack].account != -1 )	// if the attacker
	{
		if( (chars[defend].flag&0x04) && Guilds->Compare( attack, defend ) == 0 && Races->Compare( attack, defend ) == 0 && chars[attack].attackfirst )
		{
			chars[attack].kills++;
			UOXSOCKET attSocket = calcSocketFromChar( attack );
			sprintf( temp, "You have killed %i innocent people.", chars[attack].kills );
			sysmessage( attSocket, temp );
			if( chars[attack].kills == repsys.maxkills + 1 )
				sysmessage( attSocket, "You are now a murderer!" );
		}
		
		sprintf( temp, "%s was killed by %s!\n", chars[defend].name, chars[attack].name );
		savelog( temp, "PvP.log");
	}
}

void PlayerAttack( UOXSOCKET s )
{
	SERIAL serial;
	long i;
	int j, k;
	CHARACTER ourChar = currchar[s];
	serial = calcserial( buffer[s][1], buffer[s][2], buffer[s][3], buffer[s][4] );
	if( serial == -1 ) 
	{
		chars[ourChar].targ = -1;
		return;
	}
	i = calcCharFromSer( serial );
	if( i == -1 )
	{
		chars[ourChar].targ = -1;
		return;
	}
	if( chars[ourChar].dead )
	{
		if( chars[i].npc )
		{	// if target is a npc..
			switch( chars[i].npcaitype )
			{
			case 1:	// good healer
				if( !(chars[ourChar].flag&0x01) && !(chars[ourChar].flag&0x02) ) // changed from 0x01 so you cant attact a healer to get resed  -- eagle
				{	//if character isn't red(bad guy)
					if( chardist( i, ourChar ) <= 3 )
					{	//let's resurrect him!
						npcaction( i, 0x10 );
						Targ->NpcResurrectTarget( ourChar );
						staticeffect( ourChar, 0x37, 0x6A, 0x09, 0x06 );
						switch( rand()%5 )
						{
						case 0: npctalkall( i, "Thou art dead, but 'tis within my power to resurrect thee.  Live!", 0 ); break;
						case 1: npctalkall( i, "Allow me to resurrect thee ghost.  Thy time of true death has not yet come.", 0 ); break;
						case 2: npctalkall( i, "Perhaps thou shouldst be more careful.  Here, I shall resurrect thee.", 0 ); break;
						case 3: npctalkall( i, "Live again, ghost!  Thy time in this world is not yet done.", 0 ); break;
						case 4: npctalkall( i, "I shall attempt to resurrect thee.", 0 ); break;
						}
					} 
					else //if dist>3
						npctalkall( i, "Come nearer, ghost, and i'll give you life!",1 );
				} 
				else //if a bad guy
					npctalkall( i, "I will not give life to a schodrel like thee!", 1 );
				break;
			case 666:	// evil healer
				if( chars[ourChar].flag&0x01 )
				{//if character is red(bad guy)
					if( chardist( i, ourChar ) <= 3 )	// let's resurrect him
					{
						npcaction( i, 0x10 );
						Targ->NpcResurrectTarget( ourChar );
						staticeffect( ourChar, 0x37, 0x09, 0x09, 0x19 ); //Flamestrike effect
						switch( rand()%5 )
						{
						case 0: npctalkall( i, "Fellow minion of Mondain, Live!!", 0 ); break;
						case 1: npctalkall( i, "Thou has evil flowing through your veins, so I will bring you back to life.", 0 ); break;
						case 2: npctalkall( i, "If I res thee, promise to raise more hell!.", 0 ); break;
						case 3: npctalkall( i, "From hell to Britannia, come alive!.", 0 ); break;
						case 4: npctalkall( i, "Since you are Evil, I will bring you back to consciouness.", 0 ); break;
						}
					} 
					else //if dist >3
						npctalkall(i, "Come nearer, evil soul, and i'll give you life!",1);
				} 
				else //if player is a good guy
					npctalkall( i, "I despise all things good. I shall not give thee another chance!", 1 );
				break;
			default:
				sysmessage( s, "You are dead and cannot do that." );
				break;
			}
			return;
		} 
		else
		{//if this not a npc but a player
			if( server_data.persecute )
			{//start persecute stuff - AntiChrist
				chars[ourChar].targ = i;
				Skills->Persecute( s );
				return;
			} 
			else
			{
				sysmessage(s,"You are dead and cannot do that.");
				return;
			}
		}//if npc
	}
	else
	{
		chars[ourChar].targ = i;
		if( ( chars[ourChar].hidden ) && ( !( chars[ourChar].priv2&8 ) ) )
		{
			chars[ourChar].hidden = 0;
			chars[ourChar].stealth = -1;
			updatechar( ourChar );
		}
		if( chars[i].dead || chars[i].hp <= 0 )//AntiChrist
		{
			sysmessage( s, "That person is already dead!" );
			return;
		}
		
		if( chars[i].npcaitype == 17 )//PlayerVendors
		{
			sprintf( temp, "%s cannot be harmed.", chars[i].name );
			sysmessage( s, temp );
			return;
		}
		if( chars[i].priv&0x04 )
		{
			sysmessage( s, "You cannot fight the invincible" );
			return;
		}
		if( chars[i].guarded )
		{
			for( j = 0; j < cownsp[chars[ourChar].serial%HASHMAX].max; j++ )
			{
				k = cownsp[chars[ourChar].serial%HASHMAX].pointer[j];
				if( k != -1 )
				{
					if( chars[k].ownserial == chars[ourChar].serial && chars[k].npcaitype == 32 && chardist( i, k ) <= 20 )
					{
						npcattacktarget( ourChar, k );				// think this is the way to attack the attacker
					}
				}
			}
		}
		sprintf( temp, "You see %s attacking %s!", chars[ourChar].name, chars[i].name );
		
		// Dupois pointed out the for loop was changing i which would drive stuff nuts later
		for( j = 0; j < now; j++ )
		{
			if((inrange1(s, j) && perm[j]) && (s!=j))
			{
				npcemote(j, ourChar, temp, 1);
			}
		}
		int gCompare = Guilds->Compare( ourChar, i );
		int rCompare = Races->Compare( ourChar, i );
		if( ( chars[i].flag&0x04 ) && gCompare == 0 && rCompare == 0 ) //REPSYS
		{	// npcaitype 2 cannot be innocent
			bool regionGuarded = ( ( region[chars[i].region].priv&0x01 ) == 0x01 );
			unsigned short charID = (chars[i].id1<<8) + chars[i].id2;
			if( server_data.guardsactive && regionGuarded && chars[i].npc && chars[i].npcaitype != 4 && charID >= 0x0190 && charID <= 0x0193 )
				npctalkall( i, "Help! Guards! I've been attacked!", 1 );
			criminal( ourChar );
		}
		
		//AntiChrist!
		// keep the target highlighted
		// so that we know who we're attacking =)
		// 26/10/99
		attackok[1] = chars[i].ser1;
		attackok[2] = chars[i].ser2;
		attackok[3] = chars[i].ser3;
		attackok[4] = chars[i].ser4;
		Network->xSend( s, attackok, 5, 0 );
		
		if( chars[i].npcaitype != 4 && chars[i].targ == -1 )
		{
			chars[i].attacker = ourChar;
			chars[i].attackfirst = 0;
		}
		chars[ourChar].attackfirst = 1;
		chars[ourChar].attacker = i;
		npcattacktarget( ourChar, i );
	}
}