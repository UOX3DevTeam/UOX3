/** @type { ( user: Character, iUsing: Item ) => boolean } */
function onUseChecked( pUser, iUsed )
{
	var recipeID = iUsed.GetTag( "recipeID" )

	if( !recipeID )
		return false;

	var recipeSectionID = iUsed.sectionID;

	TriggerEvent( 4022, "AddRecipe", pUser, iUsed, recipeID, recipeSectionID );
	return false;
}

/** @type { ( myObj: BaseObject, pSocket: Socket ) => string } */
function onTooltip( recipe, pSocket )
{
	var recipeName = recipe.GetTag( "recipeName" )
	var tooltipText = "[" + recipeName.toString() + "]"; // name of the recipe
	return tooltipText;
}
