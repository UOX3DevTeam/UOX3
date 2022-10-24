// This script is attached to magical weapons generated as loot/treasure by the magic_items.js script
// It handles attaching/removing necessary scripts to the player as they equip/unequip those
// magic weapons

// ID of script that handles spell effects on magic weapons
const magicWeaponSpellScriptID = 3305;

// When equipped, add script trigger for magic weapon spells to player
function onEquip( pEquipper, iEquipped )
{
	pEquipper.AddScriptTrigger( magicWeaponSpellScriptID );
}

// Remove script trigger on unequip
function onUnequip( pUnequipper, iUnequipped )
{
	pUnequipper.RemoveScriptTrigger( magicWeaponSpellScriptID );
}

// Display amount of charges left on magic weapon in item tooltips
function onTooltip( myObj )
{
	var tooltipText = "";
	if( myObj.type != 15 && ( myObj.name2 == "#" || myObj.name2 == "" ))
	{
		tooltipText = GetDictionaryEntry( 9403 ); // uses remaining: %i
		tooltipText = ( tooltipText.replace( /%i/gi, ( myObj.morez ).toString()) );
	}
	return tooltipText;
}