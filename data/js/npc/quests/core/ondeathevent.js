function onDeathBlow( mKilled, mKiller ) 
{
	var socket = mKiller.socket;
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", mKiller );

	for (let i = 0; i < myArray.length; i++) 
	{
		var myQuestData = myArray[i].split(",");
		var questSlot = myQuestData[0];
		var QnNumToKill = myQuestData[9];

		if ( questSlot == mKiller.GetTempTag( "QuestSlotTemp" ) ) 
		{
			switch ( parseInt( questSlot ) ) 
			{
				case parseInt(questSlot):
					if (mKiller.GetTag(QnNumToKill.toString()) >= 1)
					{
						var oldSqToKill = mKiller.GetTag(QnNumToKill.toString());
						var newNumToKill = (oldSqToKill - 1);

						mKiller.SetTag(QnNumToKill.toString(), newNumToKill);
						mKiller.SetTempTag("QuestSlotTemp", parseInt(questSlot));

						socket.SysMessage("You have " + NumToString(newNumToKill) + " more creatures to kill.");
						TriggerEvent(19800, "questlog", mKiller);
						break;
					}
					else
						break;
			}
			break;
		}
	}
	return true;
}