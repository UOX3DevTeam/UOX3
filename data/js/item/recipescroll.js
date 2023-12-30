function onUseChecked( pUser, iUsed )
{
	var recipeID = iUsed.GetTag( "recipeID" )

	if( !recipeID )
		return false;

	var recipeSectionID = iUsed.sectionID;

	TriggerEvent( 4038, "AddRecipe", pUser, iUsed, recipeID, recipeSectionID );
	return false;
}

function onTooltip( recipe, pSocket )
{
	var recipeName = recipe.GetTag( "recipeName" )
	var tooltipText = "[" + recipeName.toString() + "]"; // name of the recipe
	return tooltipText;
}