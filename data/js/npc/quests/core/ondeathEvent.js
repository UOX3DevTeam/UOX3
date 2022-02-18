function onDeathBlow(mKilled, mKiller) 
{
	var socket = mKiller.socket;
	//Solen Quest
	var oldSqToKill = mKiller.GetTag("SQ_NUMTOKILL");
	var oldNpqToKill = mKiller.GetTag("NPQ_NUMTOKILL");

	if (oldSqToKill && mKiller.GetTag("SQ_IDTOKILL") == mKilled.id)
	{
		var newNumToKill = (oldSqToKill - 1);
		mKiller.SetTag("SQ_NUMTOKILL", newNumToKill);
		if (newNumToKill)
		{
			socket.SysMessage("You have " + NumToString(newNumToKill) + " more creatures to kill.");
			TriggerEvent(19800, "questlog", mKiller);
			return true;
		}
		else
		{
			mKiller.SysMessage("You've completed your task of slaying solen queens. Return to the ambitious queen who asked for your help.");
			TriggerEvent(19800, "questlog", mKiller);
		}
		return true;
	}

	if (oldNpqToKill && mKiller.GetTag("NPQ_IDTOKILL") == mKilled.id)
	{
		var newNumToKill = (oldNpqToKill - 1);
		mKiller.SetTag("NPQ_NUMTOKILL", newNumToKill);
		if (newNumToKill)
		{
			socket.SysMessage("You have " + NumToString(newNumToKill) + " more creatures to kill.");
			TriggerEvent(19800, "questlog", mKiller);
			return true;
		}
		else
		{
			mKiller.SysMessage("You've completed your task of slaying solen queens. Return to the ambitious queen who asked for your help.");
			TriggerEvent(19800, "questlog", mKiller);
		}
		return true;
	}
	return true;
}