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