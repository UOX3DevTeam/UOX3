/// <reference path="../definitions.d.ts" />
// @ts-check

const levelGumpScriptId = 2509;
const classesPerPage = 4;
const abilitiesPerPage = 4;
const professionsPerPage = 4;

/** Load display and progression data from dfndata/race/classes.dfn. */
function GetClassProfile( classId )
{
	return {
		id: classId,
		name: String( GetLevelClassProperty( classId, "NAME" ) || ( "Class " + classId )),
		description: String( GetLevelClassProperty( classId, "DESCRIPTION" ) || "" ),
		primary: String( GetLevelClassProperty( classId, "PRIMARYSKILLS" ) || "None" ),
		core: String( GetLevelClassProperty( classId, "CORESKILLS" ) || "None" ),
		support: String( GetLevelClassProperty( classId, "SUPPORTSKILLS" ) || "None" ),
		healthPerLevel: Number( GetLevelClassProperty( classId, "HEALTHPERLEVEL" ) || 0 ),
		manaPerLevel: Number( GetLevelClassProperty( classId, "MANAPERLEVEL" ) || 0 ),
		staminaPerLevel: Number( GetLevelClassProperty( classId, "STAMINAPERLEVEL" ) || 0 ),
		primaryCap: Number( GetLevelClassProperty( classId, "PRIMARYCAP" ) || 0 ),
		coreCap: Number( GetLevelClassProperty( classId, "CORECAP" ) || 0 ),
		supportCap: Number( GetLevelClassProperty( classId, "SUPPORTCAP" ) || 0 ),
		otherCap: Number( GetLevelClassProperty( classId, "OTHERCAP" ) || 0 )
	};
}

/** Load one zero-based class ability entry from classes.dfn. */
function GetClassAbilityProfile( classId, abilityIndex )
{
	return {
		id: Number( GetLevelClassAbilityProperty( classId, abilityIndex, "ID" ) || 0 ),
		name: String( GetLevelClassAbilityProperty( classId, abilityIndex, "NAME" ) || ( "Ability " + abilityIndex )),
		description: String( GetLevelClassAbilityProperty( classId, abilityIndex, "DESCRIPTION" ) || "" ),
		action: String( GetLevelClassAbilityProperty( classId, abilityIndex, "ACTION" ) || "PASSIVE" ).toUpperCase(),
		value: Number( GetLevelClassAbilityProperty( classId, abilityIndex, "VALUE" ) || 0 ),
		level: Number( GetLevelClassAbilityProperty( classId, abilityIndex, "LEVEL" ) || 1 ),
		manaCost: Number( GetLevelClassAbilityProperty( classId, abilityIndex, "MANACOST" ) || 0 ),
		staminaCost: Number( GetLevelClassAbilityProperty( classId, abilityIndex, "STAMINACOST" ) || 0 ),
		cooldown: Number( GetLevelClassAbilityProperty( classId, abilityIndex, "COOLDOWN" ) || 0 ),
		scriptId: Number( GetLevelClassAbilityProperty( classId, abilityIndex, "SCRIPT" ) || 0 ),
		functionName: String( GetLevelClassAbilityProperty( classId, abilityIndex, "FUNCTION" ) || "UseClassAbility" )
	};
}

/** Load one crafting profession from dfndata/race/professions.dfn. */
function GetProfessionProfile( professionId )
{
	return {
		id: professionId,
		name: String( GetLevelProfessionProperty( professionId, "NAME" ) || ( "Profession " + professionId )),
		description: String( GetLevelProfessionProperty( professionId, "DESCRIPTION" ) || "" ),
		type: String( GetLevelProfessionProperty( professionId, "TYPE" ) || "CRAFTING" ),
		primary: Boolean( GetLevelProfessionProperty( professionId, "PRIMARY" )),
		skill: String( GetLevelProfessionProperty( professionId, "SKILL" ) || "Unknown" ),
		skillId: Number( GetLevelProfessionProperty( professionId, "SKILLID" ) || 0 ),
		startValue: Number( GetLevelProfessionProperty( professionId, "STARTVALUE" ) || 0 ),
		maxValue: Number( GetLevelProfessionProperty( professionId, "MAXVALUE" ) || 1000 ),
		experiencePerPoint: Number( GetLevelProfessionProperty( professionId, "EXPERIENCEPERPOINT" ) || 100 ),
		maxExperience: Number( GetLevelProfessionProperty( professionId, "MAXEXPERIENCE" ) || 0 )
	};
}

/** Replace the OSI skill window with the level progression window. */
function onSkillGump( pUser )
{
	if( !GetServerSetting( "LevelSystemEnabled" ))
		return true;
	DisplayLevelSystem( pUser );
	return false;
}

/** Called by the global login script after the class-system trigger is attached. */
function ShowLevelSystemOnLogin( pUser )
{
	if( GetServerSetting( "LevelSystemEnabled" ) && ValidateObject( pUser ) && pUser.levelClass == 0 )
		DisplayClassSelectionGump( pUser, 0 );
}

/** @param {Character} pUser */
function DisplayLevelSystem( pUser )
{
	if( !ValidateObject( pUser ) || pUser.socket == null )
		return;
	if( pUser.levelClass == 0 )
		DisplayClassSelectionGump( pUser, 0 );
	else
		DisplayLevelGump( pUser );
}

/** @param {Character} pUser @param {number} page */
function DisplayClassSelectionGump( pUser, page )
{
	const pSocket = pUser.socket;
	if( pSocket == null )
		return;

	const classCount = GetLevelClassCount();
	const pageCount = Math.max( 1, Math.ceil( classCount / classesPerPage ));
	page = Math.max( 0, Math.min( page, pageCount - 1 ));
	pUser.SetTempTag( "levelClassPage", page );
	pSocket.CloseGump( levelGumpScriptId + 0xffff, 0 );

	const classGump = new Gump();
	classGump.AddPage( 0 );
	classGump.AddBackground( 0, 0, 560, 430, 5054 );
	classGump.AddCheckerTrans( 12, 12, 536, 406 );
	classGump.AddButton( 522, 12, 4017, 4018, 1, 0, 0 );
	classGump.AddHTMLGump( 20, 16, 510, 30, false, false, "<CENTER><BASEFONT COLOR=#E8C96B>Choose Your Class</BASEFONT></CENTER>" );
	classGump.AddHTMLGump( 28, 50, 500, 42, false, false, "<CENTER><BASEFONT COLOR=#C0C0C0>Your class controls skill growth, resources, and equipment. The choice is permanent unless reset by a GM.</BASEFONT></CENTER>" );

	const firstClass = page * classesPerPage + 1;
	const lastClass = Math.min( classCount, firstClass + classesPerPage - 1 );
	let y = 95;
	for( let classId = firstClass; classId <= lastClass; ++classId )
	{
		AddClassChoice( classGump, y, GetClassProfile( classId ));
		y += 73;
	}

	if( page > 0 )
	{
		classGump.AddButton( 30, 395, 4014, 4016, 1, 0, 9001 );
		classGump.AddText( 62, 396, 1152, "Previous" );
	}
	if( page + 1 < pageCount )
	{
		classGump.AddButton( 445, 395, 4005, 4007, 1, 0, 9002 );
		classGump.AddText( 475, 396, 1152, "Next" );
	}
	classGump.Send( pSocket );
	classGump.Free();
}

/** @param {Gump} classGump @param {number} y @param {{id:number,name:string,description:string}} profile */
function AddClassChoice( classGump, y, profile )
{
	classGump.AddBackground( 24, y, 512, 66, 3000 );
	classGump.AddButton( 38, y + 18, 4005, 4007, 1, 0, 1000 + profile.id );
	classGump.AddText( 72, y + 8, 68, profile.name );
	classGump.AddHTMLGump( 72, y + 29, 445, 30, false, false, "<BASEFONT COLOR=#202020>" + profile.description + "</BASEFONT>" );
}

/** @param {Character} pUser */
function DisplayLevelGump( pUser )
{
	if( !ValidateObject( pUser ) || pUser.socket == null )
		return;

	const pSocket = pUser.socket;
	const levelCap = Number( GetServerSetting( "LevelCap" ));
	const baseExperience = Number( GetServerSetting( "LevelBaseExperience" ));
	const level = pUser.level;
	const totalExperience = pUser.experience;
	const levelStartExperience = baseExperience * ( level - 1 ) * level / 2;
	const levelCost = baseExperience * level;
	const levelProgress = Math.max( 0, totalExperience - levelStartExperience );
	const progressPercent = level >= levelCap ? 100 : Math.min( 100, Math.floor(( levelProgress / levelCost ) * 100 ));
	const profile = GetClassProfile( pUser.levelClass );
	const resourceGrowth = "+" + profile.healthPerLevel + " health, +" + profile.manaPerLevel + " mana, +" + profile.staminaPerLevel + " stamina per level";
	const descriptionHeight = EstimateWrappedHeight( profile.description, 68, 18, 30 );
	const skillRows = [
		{ label: "Primary", cap: profile.primaryCap, skills: profile.primary, height: 0 },
		{ label: "Core", cap: profile.coreCap, skills: profile.core, height: 0 },
		{ label: "Support", cap: profile.supportCap, skills: profile.support, height: 0 }
	];
	for( let rowIndex = 0; rowIndex < skillRows.length; ++rowIndex )
	{
		const row = skillRows[rowIndex];
		row.height = EstimateWrappedHeight( row.label + " (" + row.cap + "%): " + row.skills, 54, 18, 26 );
	}

	const profileTop = 137;
	const descriptionTop = 169;
	let skillTop = descriptionTop + descriptionHeight + 4;
	let profileContentHeight = skillTop;
	for( let rowIndex = 0; rowIndex < skillRows.length; ++rowIndex )
		profileContentHeight += skillRows[rowIndex].height;
	const resourceTop = profileContentHeight + 4;
	const profileBottom = resourceTop + 28;
	const attributesTop = profileBottom + 13;
	const footerTop = attributesTop + 157;
	const gumpHeight = footerTop + 46;

	pSocket.CloseGump( levelGumpScriptId + 0xffff, 0 );
	const levelGump = new Gump();
	levelGump.AddPage( 0 );
	levelGump.AddBackground( 0, 0, 560, gumpHeight, 5054 );
	levelGump.AddCheckerTrans( 12, 12, 536, gumpHeight - 24 );
	levelGump.AddButton( 522, 12, 4017, 4018, 1, 0, 0 );
	levelGump.AddHTMLGump( 20, 16, 510, 30, false, false, "<CENTER><BASEFONT COLOR=#E8C96B>" + profile.name + " Progression</BASEFONT></CENTER>" );
	levelGump.AddText( 30, 55, 1152, "Level" );
	levelGump.AddText( 125, 55, 88, level + " / " + levelCap );
	levelGump.AddText( 290, 55, 1152, "Attribute Points" );
	levelGump.AddText( 445, 55, 88, pUser.unspentStatPoints.toString() );
	levelGump.AddText( 30, 80, 1152, "Total Experience" );
	levelGump.AddText( 175, 80, 88, totalExperience.toString() );
	levelGump.AddText( 30, 105, 1152, level >= levelCap ? "Maximum level reached" : "Level progress: " + levelProgress + " / " + levelCost + " (" + progressPercent + "%)" );

	levelGump.AddBackground( 25, profileTop, 510, profileBottom - profileTop, 3000 );
	levelGump.AddText( 40, 145, 68, "Class Profile" );
	levelGump.AddHTMLGump( 40, descriptionTop, 475, descriptionHeight, false, false, "<BASEFONT COLOR=#202020>" + profile.description + "</BASEFONT>" );
	for( let rowIndex = 0; rowIndex < skillRows.length; ++rowIndex )
	{
		const row = skillRows[rowIndex];
		levelGump.AddHTMLGump( 40, skillTop, 475, row.height, false, false, "<BASEFONT COLOR=#202020>" + row.label + " (" + row.cap + "%): " + row.skills + "</BASEFONT>" );
		skillTop += row.height;
	}
	levelGump.AddText( 40, resourceTop, 0, resourceGrowth );

	levelGump.AddBackground( 25, attributesTop, 510, 145, 3000 );
	levelGump.AddText( 40, attributesTop + 8, 68, "Attributes" );
	AddStatRow( levelGump, 40, attributesTop + 40, "Strength", pUser.strength, 1, pUser.unspentStatPoints > 0 );
	AddStatRow( levelGump, 40, attributesTop + 70, "Dexterity", pUser.dexterity, 2, pUser.unspentStatPoints > 0 );
	AddStatRow( levelGump, 290, attributesTop + 40, "Intelligence", pUser.intelligence, 3, pUser.unspentStatPoints > 0 );
	levelGump.AddButton( 40, attributesTop + 105, 4005, 4007, 1, 0, 50 );
	levelGump.AddText( 72, attributesTop + 105, 0, "Class Abilities" );
	if( GetServerSetting( "LevelSystemCrafting" ))
	{
		levelGump.AddButton( 290, attributesTop + 105, 4005, 4007, 1, 0, 51 );
		levelGump.AddText( 322, attributesTop + 105, 0, "Professions" );
	}
	levelGump.AddHTMLGump( 25, footerTop, 510, 24, false, false, "<CENTER><BASEFONT COLOR=#C0C0C0>Non-profession Other skills cap at " + profile.otherCap + "% for this class.</BASEFONT></CENTER>" );
	levelGump.Send( pSocket );
	levelGump.Free();
}

/** Estimate enough vertical space for wrapped ClassicUO gump text. */
function EstimateWrappedHeight( text, charactersPerLine, lineHeight, minimumHeight )
{
	return Math.max( minimumHeight, Math.ceil( Math.max( 1, String( text ).length ) / charactersPerLine ) * lineHeight );
}

/** @param {Gump} levelGump @param {number} x @param {number} y @param {string} name @param {number} value @param {number} buttonId @param {boolean} canIncrease */
function AddStatRow( levelGump, x, y, name, value, buttonId, canIncrease )
{
	levelGump.AddText( x, y, 0, name );
	levelGump.AddText( x + 105, y, 88, value.toString() );
	if( canIncrease )
		levelGump.AddButton( x + 145, y, 4005, 4007, 1, 0, buttonId );
}

/** @param {Character} pUser @param {number} page */
function DisplayClassAbilityGump( pUser, page )
{
	if( !ValidateObject( pUser ) || pUser.socket == null || pUser.levelClass == 0 )
		return;

	const pSocket = pUser.socket;
	const profile = GetClassProfile( pUser.levelClass );
	const abilityCount = GetLevelClassAbilityCount( pUser.levelClass );
	const pageCount = Math.max( 1, Math.ceil( abilityCount / abilitiesPerPage ));
	page = Math.max( 0, Math.min( page, pageCount - 1 ));
	pUser.SetTempTag( "levelAbilityPage", page );
	pSocket.CloseGump( levelGumpScriptId + 0xffff, 0 );

	const abilityGump = new Gump();
	abilityGump.AddPage( 0 );
	abilityGump.AddBackground( 0, 0, 560, 490, 5054 );
	abilityGump.AddCheckerTrans( 12, 12, 536, 466 );
	abilityGump.AddButton( 522, 12, 4017, 4018, 1, 0, 0 );
	abilityGump.AddHTMLGump( 20, 16, 510, 30, false, false, "<CENTER><BASEFONT COLOR=#E8C96B>" + profile.name + " Abilities</BASEFONT></CENTER>" );
	abilityGump.AddHTMLGump( 30, 45, 500, 24, false, false, "<CENTER><BASEFONT COLOR=#C0C0C0>Level " + pUser.level + " - abilities unlock automatically as you level.</BASEFONT></CENTER>" );

	const firstAbility = page * abilitiesPerPage;
	const lastAbility = Math.min( abilityCount, firstAbility + abilitiesPerPage );
	let y = 72;
	for( let abilityIndex = firstAbility; abilityIndex < lastAbility; ++abilityIndex )
	{
		const ability = GetClassAbilityProfile( pUser.levelClass, abilityIndex );
		const unlocked = pUser.level >= ability.level;
		const cooldownRemaining = GetClassAbilityCooldownRemaining( pUser, ability );
		abilityGump.AddBackground( 25, y, 510, 82, 3000 );
		if( unlocked && ability.action != "PASSIVE" && cooldownRemaining == 0 )
			abilityGump.AddButton( 38, y + 25, 4005, 4007, 1, 0, 10000 + abilityIndex );
		abilityGump.AddText( 72, y + 7, 68, ability.name );
		abilityGump.AddText( 410, y + 7, unlocked ? ( cooldownRemaining > 0 ? 53 : 68 ) : 33,
			unlocked ? ( ability.action == "PASSIVE" ? "Passive" : ( cooldownRemaining > 0 ? cooldownRemaining + "s" : "Ready" )) : "Level " + ability.level );
		abilityGump.AddHTMLGump( 72, y + 29, 445, 30, false, false, "<BASEFONT COLOR=#202020>" + ability.description + "</BASEFONT>" );
		abilityGump.AddText( 72, y + 58, 0, GetClassAbilityCostText( ability ));
		y += 89;
	}

	abilityGump.AddButton( 230, 449, 4014, 4016, 1, 0, 60 );
	abilityGump.AddText( 262, 450, 1152, "Back" );
	if( page > 0 )
	{
		abilityGump.AddButton( 30, 449, 4014, 4016, 1, 0, 9101 );
		abilityGump.AddText( 62, 450, 1152, "Previous" );
	}
	if( page + 1 < pageCount )
	{
		abilityGump.AddButton( 445, 449, 4005, 4007, 1, 0, 9102 );
		abilityGump.AddText( 475, 450, 1152, "Next" );
	}
	abilityGump.Send( pSocket );
	abilityGump.Free();
}

function GetClassAbilityCostText( ability )
{
	const parts = [];
	if( ability.manaCost > 0 )
		parts.push( ability.manaCost + " mana" );
	if( ability.staminaCost > 0 )
		parts.push( ability.staminaCost + " stamina" );
	if( ability.cooldown > 0 )
		parts.push( ability.cooldown + "s cooldown" );
	return parts.length > 0 ? parts.join( " - " ) : "No resource cost";
}

function GetClassAbilityCooldownRemaining( pUser, ability )
{
	const cooldownTag = "levelAbilityCooldown_" + ability.id;
	const expiresAt = Number( pUser.GetTag( cooldownTag ) || 0 );
	const remaining = Math.max( 0, Math.ceil(( expiresAt - GetCurrentClock() ) / 1000 ));
	if( expiresAt > 0 && remaining == 0 )
		pUser.SetTag( cooldownTag, null );
	return remaining;
}

function GetProfessionRank( proficiency, maximum )
{
	if( maximum <= 0 )
		return "Untrained";
	const percent = proficiency / maximum;
	if( percent >= 1 ) return "Grandmaster";
	if( percent >= 0.75 ) return "Artisan";
	if( percent >= 0.50 ) return "Expert";
	if( percent >= 0.25 ) return "Journeyman";
	return "Apprentice";
}

/** @param {Character} pUser @param {number} page */
function DisplayProfessionGump( pUser, page )
{
	if( !ValidateObject( pUser ) || pUser.socket == null || pUser.levelClass == 0 )
		return;

	const pSocket = pUser.socket;
	if( !GetServerSetting( "LevelSystemCrafting" ))
	{
		pSocket.SysMessage( "Level-system professions are disabled." );
		DisplayLevelGump( pUser );
		return;
	}

	const professionCount = GetLevelProfessionCount();
	const pageCount = Math.max( 1, Math.ceil( professionCount / professionsPerPage ));
	page = Math.max( 0, Math.min( page, pageCount - 1 ));
	pUser.SetTempTag( "levelProfessionPage", page );
	pSocket.CloseGump( levelGumpScriptId + 0xffff, 0 );

	const learnedCount = pUser.GetProfessionCount();
	let primaryLearned = 0;
	let secondaryLearned = 0;
	for( let professionId = 1; professionId <= professionCount; ++professionId )
	{
		if( pUser.HasProfession( professionId ))
		{
			if( GetProfessionProfile( professionId ).primary )
				primaryLearned++;
			else
				secondaryLearned++;
		}
	}
	const slotLimit = Number( GetServerSetting( "MaxPrimaryProfessions" ));
	const unlimited = Boolean( GetServerSetting( "AllowAllProfessions" ));
	const primarySlotsAvailable = unlimited || primaryLearned < slotLimit;
	const slotText = unlimited ? learnedCount + " learned - unlimited professions enabled" :
		primaryLearned + " / " + slotLimit + " primary - " + secondaryLearned + " secondary - a GM is required to unlearn";

	const professionGump = new Gump();
	professionGump.AddPage( 0 );
	professionGump.AddBackground( 0, 0, 560, 490, 5054 );
	professionGump.AddCheckerTrans( 12, 12, 536, 466 );
	professionGump.AddButton( 522, 12, 4017, 4018, 1, 0, 0 );
	professionGump.AddHTMLGump( 20, 16, 510, 30, false, false, "<CENTER><BASEFONT COLOR=#E8C96B>Professions</BASEFONT></CENTER>" );
	professionGump.AddHTMLGump( 30, 45, 500, 24, false, false, "<CENTER><BASEFONT COLOR=#C0C0C0>" + slotText + "</BASEFONT></CENTER>" );

	const firstProfession = page * professionsPerPage + 1;
	const lastProfession = Math.min( professionCount, firstProfession + professionsPerPage - 1 );
	let y = 72;
	for( let professionId = firstProfession; professionId <= lastProfession; ++professionId )
	{
		const profile = GetProfessionProfile( professionId );
		const learned = pUser.HasProfession( professionId );
		const canLearn = !profile.primary || primarySlotsAvailable;
		const proficiency = learned ? pUser.GetProfessionProficiency( professionId ) : profile.startValue;
		const displayedValue = ( proficiency / 10 ).toFixed( 1 ) + " / " + ( profile.maxValue / 10 ).toFixed( 1 );
		professionGump.AddBackground( 25, y, 510, 82, 3000 );
		if( !learned && canLearn )
			professionGump.AddButton( 38, y + 25, 4005, 4007, 1, 0, 20000 + professionId );
		professionGump.AddText( 72, y + 7, 68, profile.name );
		professionGump.AddText( 355, y + 7, learned ? 68 : ( canLearn ? 88 : 33 ),
			learned ? displayedValue : ( canLearn ? "Learn" : "Primary slots full" ));
		professionGump.AddHTMLGump( 72, y + 29, 445, 30, false, false, "<BASEFONT COLOR=#202020>" + profile.description + "</BASEFONT>" );
		const typeLabel = profile.primary ? profile.type.charAt( 0 ) + profile.type.slice( 1 ).toLowerCase() : "Secondary";
		professionGump.AddText( 72, y + 58, 0, learned ? GetProfessionRank( proficiency, profile.maxValue ) + " - " + profile.skill : typeLabel + " skill: " + profile.skill );
		y += 89;
	}

	professionGump.AddButton( 230, 449, 4014, 4016, 1, 0, 60 );
	professionGump.AddText( 262, 450, 1152, "Back" );
	if( page > 0 )
	{
		professionGump.AddButton( 30, 449, 4014, 4016, 1, 0, 9201 );
		professionGump.AddText( 62, 450, 1152, "Previous" );
	}
	if( page + 1 < pageCount )
	{
		professionGump.AddButton( 445, 449, 4005, 4007, 1, 0, 9202 );
		professionGump.AddText( 475, 450, 1152, "Next" );
	}
	professionGump.Send( pSocket );
	professionGump.Free();
}

/** Validate and dispatch one unlocked class ability. */
function ActivateClassAbility( pUser, abilityIndex )
{
	if( !ValidateObject( pUser ) || pUser.socket == null || pUser.levelClass == 0 || pUser.dead )
		return false;
	if( abilityIndex < 0 || abilityIndex >= GetLevelClassAbilityCount( pUser.levelClass ))
		return false;

	const ability = GetClassAbilityProfile( pUser.levelClass, abilityIndex );
	if( pUser.level < ability.level )
	{
		pUser.socket.SysMessage( ability.name + " unlocks at level " + ability.level + "." );
		return false;
	}
	const cooldownRemaining = GetClassAbilityCooldownRemaining( pUser, ability );
	if( cooldownRemaining > 0 )
	{
		pUser.socket.SysMessage( ability.name + " is available in " + cooldownRemaining + " second(s)." );
		return false;
	}
	if( pUser.mana < ability.manaCost )
	{
		pUser.socket.SysMessage( "You need " + ability.manaCost + " mana to use " + ability.name + "." );
		return false;
	}
	if( pUser.stamina < ability.staminaCost )
	{
		pUser.socket.SysMessage( "You need " + ability.staminaCost + " stamina to use " + ability.name + "." );
		return false;
	}

	let succeeded = false;
	switch( ability.action )
	{
		case "RESTOREHEALTH":
			if( pUser.health < pUser.maxhp && ability.value > 0 )
			{
				pUser.health = Math.min( pUser.maxhp, pUser.health + ability.value );
				pUser.UpdateStats( 0 );
				succeeded = true;
			}
			break;
		case "RESTOREMANA":
			if( pUser.mana < pUser.maxmana && ability.value > 0 )
			{
				pUser.mana = Math.min( pUser.maxmana, pUser.mana + ability.value );
				pUser.UpdateStats( 1 );
				succeeded = true;
			}
			break;
		case "RESTORESTAMINA":
			if( pUser.stamina < pUser.maxstamina && ability.value > 0 )
			{
				pUser.stamina = Math.min( pUser.maxstamina, pUser.stamina + ability.value );
				pUser.UpdateStats( 2 );
				succeeded = true;
			}
			break;
		case "SPELL":
			// Class spell abilities are normal character casts, never stale scroll/wand casts.
			pUser.socket.currentSpellType = 0;
			succeeded = pUser.CastSpell( ability.value ) !== false;
			break;
		case "SCRIPT":
			if( ability.scriptId > 0 && ability.functionName.length > 0 )
				succeeded = TriggerEvent( ability.scriptId, ability.functionName, pUser, ability.id, ability.value ) === true;
			break;
		case "PASSIVE":
			pUser.socket.SysMessage( ability.name + " is a passive ability." );
			return false;
		default:
			pUser.socket.SysMessage( "Unknown class ability action: " + ability.action );
			return false;
	}

	if( !succeeded )
	{
		if( ability.action == "RESTOREHEALTH" || ability.action == "RESTOREMANA" || ability.action == "RESTORESTAMINA" )
			pUser.socket.SysMessage( ability.name + " is not needed right now." );
		return false;
	}

	pUser.mana -= ability.manaCost;
	pUser.stamina -= ability.staminaCost;
	if( ability.manaCost > 0 )
		pUser.UpdateStats( 1 );
	if( ability.staminaCost > 0 )
		pUser.UpdateStats( 2 );
	if( ability.cooldown > 0 )
		pUser.SetTag( "levelAbilityCooldown_" + ability.id, ( GetCurrentClock() + ability.cooldown * 1000 ).toString() );
	pUser.socket.SysMessage( "You use " + ability.name + "." );
	return true;
}

/** @type { ( socket: Socket, button: number, gumpData: GumpData ) => void } */
function onGumpPress( pSocket, buttonId, gumpData )
{
	if( !GetServerSetting( "LevelSystemEnabled" ) || pSocket == null )
		return;
	const pUser = pSocket.currentChar;
	if( !ValidateObject( pUser ))
		return;

	if( buttonId == 50 )
	{
		DisplayClassAbilityGump( pUser, 0 );
		return;
	}
	if( buttonId == 51 )
	{
		DisplayProfessionGump( pUser, 0 );
		return;
	}
	if( buttonId == 60 )
	{
		DisplayLevelGump( pUser );
		return;
	}
	if( buttonId == 9101 || buttonId == 9102 )
	{
		const currentPage = Number( pUser.GetTempTag( "levelAbilityPage" ) || 0 );
		DisplayClassAbilityGump( pUser, currentPage + ( buttonId == 9101 ? -1 : 1 ));
		return;
	}
	if( buttonId == 9201 || buttonId == 9202 )
	{
		const currentPage = Number( pUser.GetTempTag( "levelProfessionPage" ) || 0 );
		DisplayProfessionGump( pUser, currentPage + ( buttonId == 9201 ? -1 : 1 ));
		return;
	}
	if( buttonId >= 20001 && buttonId <= 20255 )
	{
		const professionId = buttonId - 20000;
		const profile = GetProfessionProfile( professionId );
		if( pUser.LearnProfession( professionId ))
			pSocket.SysMessage( "You have learned the " + profile.name + " profession." );
		else
			pSocket.SysMessage( "You cannot learn another profession." );
		DisplayProfessionGump( pUser, Number( pUser.GetTempTag( "levelProfessionPage" ) || 0 ));
		return;
	}
	if( buttonId >= 10000 && buttonId < 10000 + GetLevelClassAbilityCount( pUser.levelClass ))
	{
		const abilityIndex = buttonId - 10000;
		if( !ActivateClassAbility( pUser, abilityIndex ))
			DisplayClassAbilityGump( pUser, Number( pUser.GetTempTag( "levelAbilityPage" ) || 0 ));
		return;
	}

	if( buttonId == 9001 || buttonId == 9002 )
	{
		const currentPage = Number( pUser.GetTempTag( "levelClassPage" ) || 0 );
		DisplayClassSelectionGump( pUser, currentPage + ( buttonId == 9001 ? -1 : 1 ));
		return;
	}

	if( buttonId >= 1001 && buttonId <= 1255 )
	{
		const classId = buttonId - 1000;
		const profile = GetClassProfile( classId );
		if( pUser.SetLevelClass( classId ))
		{
			pSocket.SysMessage( "You are now a " + profile.name + "." );
			DisplayLevelGump( pUser );
		}
		else
		{
			pSocket.SysMessage( "Your class could not be changed." );
			DisplayLevelSystem( pUser );
		}
		return;
	}

	if( buttonId >= 1 && buttonId <= 3 )
	{
		if( !pUser.SpendLevelStatPoint( buttonId - 1 ))
			pSocket.SysMessage( "That attribute cannot be increased." );
		DisplayLevelGump( pUser );
	}
}
