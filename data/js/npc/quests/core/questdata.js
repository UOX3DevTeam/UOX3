// Write Quest Log back to file
function WriteQuestLog( pUser, myArray )
{
    // Create a new file object
    var mFile = new UOXCFile();
    var userAccount = pUser.account;
    var fileName = "QuestLog_"+userAccount.id+".jsdata";

    mFile.Open(fileName, "w", "Quests"); // Open file for Writing
    if (mFile != null)
    {
        // Loop through each entry in myArray and save it to a new line
        for( let i = 0; i < myArray.length; i++ )
        {
            mFile.Write( myArray[i] + "\n" );
        }

        // Close and free the file
        mFile.Close()
        mFile.Free();
        return true;
    }

    return false;
}

// Read contents of Quest Log
function ReadQuestLog( pUser )
{
    // Create a new file object
    var mFile = new UOXCFile();
    var userAccount = pUser.account;
    var fileName = "QuestLog_"+userAccount.id+".jsdata";

    // Create an array variable to store contents of address book
    var myArray = [];

    mFile.Open( fileName, "r", "Quests" );
    if( mFile && mFile.Length() >= 0 )
    {
        // Read until we reach the end of the file, or until we find a match for the visitor
        while( !mFile.EOF() )
        {
            // Read a line of text from the file
            var line = mFile.ReadUntil( "\n" );
            if( line.length <= 1 || line == "" )
                continue;

            // Store each line from file in myArray
            myArray.push( line );
        }
        mFile.Close();

        // Frees memory allocated by file object
        mFile.Free();
    }

    return myArray;
}