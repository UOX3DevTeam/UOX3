function onPickup( iPickedUp, pGrabber )
{ 
  var pSock = pGrabber.socket;
  //Check the value of pSock.pickupSpot to determine where the item was picked up from
  switch( pSock.pickupSpot )
  {
    case 0: //nowhere
		  return true;
			break;
    case 1: //ground
		  onPickupEvent( iPickedUp, pGrabber );
		  return true;
			break;
    case 2: //ownpack
		  return true;
			break;
    case 3: //otherpack
		  onPickupEvent( iPickedUp, pGrabber );
		   return true;
			break;
    case 4: //paperdoll
     return false;
    case 5: //bank
     return false;
    default:
      pGrabber.TextMessage( "Error. Redo from Start." );
    break;
   }
}

function onPickupEvent( iPickedUp, pGrabber )
{
	var socket = pGrabber.socket;
	// Read Quests Log
    var myArray = TriggerEvent( 19806, "ReadQuestLog", pGrabber );

	for ( let i = 0; i < myArray.length; i++ )
	{
		var myQuestData = myArray[i].split(",");
		var questSlot = myQuestData[0];
		var playerSerial = myQuestData[1];
		var questName = myQuestData[3];
		var killAmount = myQuestData[4];
		var collectAmount = myQuestData[5];
		var questTrg = myQuestData[6];
		var iNumToGet = myQuestData[7];
		var iLevel = myQuestData[8];
		var nNumToKill = myQuestData[9];
		var nLevel = myQuestData[10];
		var itemId = myQuestData[11];
		var iIdToGet = myQuestData[12];
		var npcId = myQuestData[13];
		var iIdToKill = myQuestData[14];
		var questStatus = myQuestData[15];

		var oldNumToGet = pGrabber.GetTag( iNumToGet );
		var newNumToGet = ( oldNumToGet - 1 );

		if( questSlot == pGrabber.GetTempTag( "QuestSlotTemp" ))
		{
			switch ( parseInt( questSlot ) )
			{
				case parseInt( questSlot ):
					if( oldNumToGet >= 1 )
					{
						if( oldNumToGet && itemId == iPickedUp.id )
						{
							pGrabber.SetTag( iNumToGet, newNumToGet );
							pGrabber.SetTempTag( "QuestSlotTemp", parseInt( questSlot ) );

							if( oldNumToGet >= 1 )
							{
								pGrabber.SysMessage("You have " + NumToString( newNumToGet ) + " more items to collect.");
								TriggerEvent(19800, "questLog", pGrabber);
							}
							else
							{
								pGrabber.SysMessage( "You have completed collecting all the items." );
							}
						}
						break;
					}
					break;
			}
			break;
		}
	}
}