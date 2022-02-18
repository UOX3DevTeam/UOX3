function questtitles(pUser)
{
	switch (pUser.GetTag("QuestTracker")) 
	{
		case "1":return "<BASEFONT color=#FFFFFF>Solen Queen Quest</BASEFONT>";break;
		case "2":return "<BASEFONT color=#FFFFFF>New Player Quest</BASEFONT>";break;
		default:return 1054146;break;
	}
}