function AddRecipe( pUser, iUsed, recipeID, recipeSectionID )
{
	var recipeName = iUsed.GetTag( "recipeName" )

	// Read Recipe
	var myData = TriggerEvent( 4038, "ReadRecipeID", pUser );

	for( let i = 0; i < myData.length; i++ )
	{
		var myRecipeData = myData[i].split(",");

		if( myRecipeData[0] == recipeID )
		{
			pUser.SysMessage( "You already know this recipe." );// You already know this recipe.
			return;
		}
	}

	pUser.SysMessage( "You have learned a new recipe: " + recipeName );// You have learned a new recipe:

	// Ok, if recipe wasn't found in the array, store recipe in the recipeID we selected earlier and recipeSectionID
	myData.push( recipeID.toString() + "," + recipeSectionID );
	TriggerEvent( 4038, "WriteRecipeID", pUser, myData )
	iUsed.Delete();
}

function NeedRecipe( pSock, recipeID )
{
	var pUser = pSock.currentChar;

	// Read Recipe
	var myData = TriggerEvent( 4038, "ReadRecipeID", pUser );

	if( myData && myData.length > 0 )
	{
		for( let i = 0; i < myData.length; i++ )
		{
			var myRecipeData = myData[i].split(",");

			// Check if the recipeID is found and not equal to 0
			if( myRecipeData[0] == recipeID && myRecipeData[0] !== "0" )
			{
				// Recipe found, you can proceed with the logic
				// ...
				return true; // Assuming you want to return true when the recipe is found
			}
		}
		// If the loop completes without finding the recipe, display a message
		pSock.SysMessage( "You must learn that recipe from a scroll." );
		return false;
	}
	else
	{
		// If the loop completes without finding the recipe, display a message or data
		pSock.SysMessage( "You must learn that recipe from a scroll." );
		return false;
	}
}

// Write Recipe back to file
function WriteRecipeID( pUser, myData )
{
    // Create a new file object
    var mFile = new UOXCFile();
	var userSerial = pUser.serial.toString();
	var fileName = "Recipes" + userSerial +".jsdata";

	mFile.Open( fileName, "w", "Recipes" ); // Open file for Writing
    if( mFile != null )
    {
        // Loop through each entry in myData and save it to a new line
		for( var i = 0; i < myData.length; i++ )
        {
			mFile.Write( myData[i] + "\n" );
        }

        // Close and free the file
        mFile.Close()
        mFile.Free();
        return true;
    }

    return false;
}

// Read contents of Recipe
function ReadRecipeID( pUser )
{
    // Create a new file object
    var mFile = new UOXCFile();
	var userSerial = pUser.serial;
	var fileName = "Recipes" + userSerial + ".jsdata";

	// Create an array variable to store contents of Recipe
    var myArray = [];

	mFile.Open(fileName, "r", "Recipes" );
    if( mFile && mFile.Length() >= 0 )
    {
        // Read until we reach the end of the file, or until we find a match for the recipe
        while( !mFile.EOF() )
        {
            // Read a line of text from the file
            var line = mFile.ReadUntil( "\n" );
            if( line.length <= 1 || line == "" )
            {
				continue;
			}

            // Store each line from file in myArray
            myArray.push( line );
        }
        mFile.Close();

        // Frees memory allocated by file object
        mFile.Free();
    }
	//pUser.SysMessage(myArray.length)
    return myArray;
}

function _restorecontext_() {}