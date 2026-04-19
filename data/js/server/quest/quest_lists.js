// This file contains functions to retrieve quest lists and specific quests based on their IDs. as pointed out in readme and documentation.
function QuestList( questID )
{
	var questList = { };

	// Return the full quest list if no questID is provided
	if( !questID )
	{
		return questList;
	}

	// Return a specific quest if questID is provided
	return questList[questID] || null;
}