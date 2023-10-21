function onCreateDFN( objMade, objType ) 
{
	var randomnamelist2009 = [
		"Acheren", "Aefli", "Aeon", "Aeseer", "Akemai", "Aname", "Autolycus", "Axel", "Balendros", "Bleak",
		"Cachorrao", "Celtious", "Cernunnos", "Chrissay", "Comforl", "Cyno Razik", "Dekru", "Dexter", "Draconi", "Drosst", "Drummergirl",
		"Dudley", "Ealia_tiefwasser", "Eira", "Eldariian", "Emile Layne", "Epic", "Eris", "Evrie", "Faine Morgan", "Feidrid", "Fiorella",
		"Fiper", "Gatermot", "Helios", "Iatu", "Jawbra", "Justel", "Kaenja", "Kanmare", "Kasaven", "Kestrel", "Kilandra",
		"Laurana", "Lillimu", "Lothande", "Lyraa", "Malachi", "Meri", "Mesanna", "Misk", "MrsTroubleMaker", "Nestor", "Othan",
		"Pallando", "Phoenix", "Prume", "Quantum", "Rend", "Ryutaro_obi", "Sakkarrah", "Sangria", "Sarakan", "Sebastian Seino", "Seppo",
		"Shade", "Snark", "Stardancer", "Supreem", "Tashik", "Terya", "TheGrimmOmen", "Thowe", "Truji", "Uriah", "Vespera", "Visrias",
		"Vladimere", "Wasia", "Wilki", "Worver", "Xatryn", "Yossie", "Ziv", "Zoer"
	];

	if( objType == 0 )
	{
		var randomName2009 = Math.floor( Math.random() * randomnamelist2009.length );

		objMade.name = randomnamelist2009[randomName2009] + " Jack O' Lantern";
	}
}