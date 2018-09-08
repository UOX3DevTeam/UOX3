// Moongate.js (v1.3) 
// Gump driven gates for location and facet travel 
// Rais 7th Feb 02 
// Contact: uox_rais@hotmail.com

// Example usage: 
// In game, do the following 
// 'add 0xf6c    (add's a gate) 
// 'setscptric 5011  (Set the script to run on the gate) 
// (if you changed the ID in jse_fileassociations.scp, then use the ID you assigned) 

// Issue's: 
// Pets dont follow, but thats the same of item type 89 (world gates) 

// Tag's Used: 
// cchar.ItemUsed hold the serial of the item used (the gate) 

// Updates: 
// 5/3/02: Made the actual gump routine a function, save's having the exact same code twice 
// 15/2/06: Included script with UOX3 by default (Xuri)
// 20/5/07: v1.3 - Added locations in Malas & Tokuno (Xuri)

//Set to 0 to disable teleport locations for a certain map from appearing in the moongate menu
var enableFelucca = 1;
var enableTrammel = 1;
var enableIlshenar = 1;
var enableMalas = 1;
var enableTokuno = 1;

function onUseChecked( pUser, iUsed ) 
{ 
  // get users socket 
  var srcSock = pUser.socket; 

  //Are we using the gate allready? 
  //Get the serial of the item we have just used 
  var Serial = new Array; 
  Serial[0]=iUsed.GetSerial(1); 
  Serial[1]=iUsed.GetSerial(2); 
  Serial[2]=iUsed.GetSerial(3); 
  Serial[3]=iUsed.GetSerial(4); 

  //Get the stored serial from tag 
  var oldSerial = new Array; 
  var iCheck = new String(pUser.GetTag("ItemUsed")); 
  var oldSerial = iCheck.split(','); 

  //Compare the 2, if the same, then we are allready using this gate 
  if( Serial[0] == oldSerial[0] && Serial[1] == oldSerial[1] && Serial[2] == oldSerial[2] && Serial[3] == oldSerial[3] ) 
    return false; 
  else 
    pUser.SetTag("ItemUsed", Serial[0] + "," + Serial[1] + "," + Serial[2] + "," + Serial[3]); 

  // this Creates our gump 
  var ret=displaygump(srcSock, pUser);
  return false;
} 

function onCollide( srcSock, pUser, iUsed ) 
{ 
  var Serial = new Array; 
  Serial[0]=iUsed.GetSerial(1); 
  Serial[1]=iUsed.GetSerial(2); 
  Serial[2]=iUsed.GetSerial(3); 
  Serial[3]=iUsed.GetSerial(4); 
  var oldSerial = new Array; 
  var iCheck = new String(pUser.GetTag("ItemUsed")); 
  var oldSerial = iCheck.split(','); 
  if( Serial[0] == oldSerial[0] && Serial[1] == oldSerial[1] && Serial[2] == oldSerial[2] && Serial[3] == oldSerial[3] ) 
    return; 
  else 
    pUser.SetTag("ItemUsed", Serial[0] + "," + Serial[1] + "," + Serial[2] + "," + Serial[3]); 
  var ret = displaygump(srcSock, pUser); 
} 

function onGumpPress(srcSock,myButton) 
{ 
  //Calc char 
  var srcChar = srcSock.currentChar; 

  //Range check. Its possible to call up the gate and run away 
  //get the items's serial 
  var oldSerial = new Array; 
  var iCheck = new String(srcChar.GetTag("ItemUsed")); 
  var oldSerial = iCheck.split(','); 
  //Convert serial into actual numbers not strings 
  oldSerial[0] = parseInt(oldSerial[0]);
  oldSerial[1] = parseInt(oldSerial[1]);
  oldSerial[2] = parseInt(oldSerial[2]);
  oldSerial[3] = parseInt(oldSerial[3]);
  //Find out what the item is from the serial 
  var iUsed=CalcItemFromSer(oldSerial[0], oldSerial[1], oldSerial[2], oldSerial[3]); 
  ret=iUsed.InRange( srcChar, 5); 

  //Clear the tag so we can use gate again? 
  srcChar.SetTag("ItemUsed", "0,0,0,0");  

  //Check ret and button (dont say anything if canceled) 
  if( ret == false && myButton >= 0) 
  { 
    srcSock.SysMessage("You have moved too far away to use this."); 
    return;
  } 

  //Where we going... Fel or tram 
  if(myButton >=5 && myButton <= 12) 
    tWorld=0;  //Going to Fel 
  if(myButton >=13 && myButton <= 20) 
    tWorld=1;  //Going to Tram 

  switch(myButton) 
  { 
    //Fel Tram (world 0 and 1) 
    case 5: 
    case 13: 
    { 
      srcChar.Teleport( 1336,1998,5,tWorld );    //Brit 
      break; 
    } 
    case 6: 
    case 14: 
    { 
      srcChar.Teleport( 3564,2141,33,tWorld );  //Magincia 
      break; 
    } 
    case 7: 
    case 15: 
    { 
      srcChar.Teleport( 4467,1284,5,tWorld );    //Moonglow 
      break; 
    } 
    case 8: 
    case 16: 
    { 
      srcChar.Teleport( 643,2068,5,tWorld );    //Skara 
      break; 
    } 

    case 9: 
    case 17: 
    { 
      srcChar.Teleport( 1828,2949,-20,tWorld );  //Trinsic 
      break; 
    } 
    case 10: 
    case 18: 
    { 
      srcChar.Teleport( 2701,694,4,tWorld );    //Vesper 
      break; 
    } 
    case 11: 
    case 19: 
    { 
      srcChar.Teleport( 771,754,4,tWorld );    //Yew 
      break; 
    } 
    case 12: 
    case 20: 
    { 
      srcChar.Teleport( 1499,3772,5,tWorld );    //Jhelom 
      break; 
    } 

    //Iishenar 
    case 21: 
    { 
      srcChar.Teleport( 1216,468,-13,2 );  //Compassion 
      break; 
    } 
    case 22: 
    { 
      srcChar.Teleport( 722,1364,-60,2 );  //Honesty 
      break; 
    } 
    case 23: 
    { 
      srcChar.Teleport( 750,724,-28,2 );  //Honor 
      break; 
    } 
    case 24: 
    { 
      srcChar.Teleport( 282,1015,0 ,2 );  //Humility 
      break; 
    } 
    case 25: 
    { 
      srcChar.Teleport( 987,1010,-32 ,2 );  //Justice 
      break; 
    } 
    case 26: 
    { 
      srcChar.Teleport( 1174,1286,-30 ,2 );  //Sacrifice 
      break; 
    } 
    case 27: 
    { 
      srcChar.Teleport( 1532,1340,-4 ,2 );  //Sprirituality 
      break; 
    } 
    case 28: 
    { 
      srcChar.Teleport( 528,219,-42 ,2 );  //Valor 
      break; 
    } 
    case 29: 
    { 
      srcChar.Teleport( 1721,218,96 ,2 );  //Chaos 
      break; 
    }
    //Malas
    case 30: 
    { 
      srcChar.Teleport( 1015,527,-65 ,3 );  //New Luna
      break; 
    }
    case 31: 
    { 
      srcChar.Teleport( 1054,358,-86 ,3 );  //Old Luna
      break; 
    }
    case 32: 
    { 
      srcChar.Teleport( 1997,1386,-85 ,3 );  //Umbra
      break; 
    }
    //Tokuno Islands
    case 33: 
    { 
      srcChar.Teleport( 1169,998,42 ,4 );  //Isamu~Jima 
      break; 
    }
    case 34: 
    { 
      srcChar.Teleport( 801,1204,25 ,4 );  //Makoto~Jima 
      break; 
    }
    case 35: 
    { 
      srcChar.Teleport( 270,628,15 ,4 );  //Homare~Jima 
      break; 
    }
  } 

  //Default Stuff, if you have clicked on a button >=5 then you have gone somwhere 
} 

function displaygump(srcSock, pUser) 
{ 
  // Make a noise as the gump opens 
  pUser.SoundEffect( 0x58, false ); 

  var myGump = new Gump; 

  // add a background 
  myGump.AddPage(1);  //Page 1 Felucca 
  myGump.AddBackground(20, 20, 260, 245, 0x23f0); 
  myGump.AddText( 40, 40, 0, "Pick your destination:" ); 
  //Cancel Button 
  myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 ); 
  myGump.AddText( 80, 210, 0, "CANCEL" ); 
  //Facet Buttons 
  if( enableFelucca == 1 )
  {
  	myGump.AddText( 70, 70, 12, "Felucca" ); 
  	myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 );
  //Location Buttons 
  myGump.AddText( 170, 70, 0, "Britian" ); 
  myGump.AddButton( 150, 75, 0x837, 1, 0, 5 ); 
  myGump.AddText( 170, 90, 0, "Magincia" ); 
  myGump.AddButton( 150, 95, 0x837, 1, 0, 6 ); 
  myGump.AddText( 170, 110, 0, "Moonglow" ); 
  myGump.AddButton( 150, 115, 0x837, 1, 0, 7 ); 
  myGump.AddText( 170, 130, 0, "Skara Brea" ); 
  myGump.AddButton( 150, 135, 0x837, 1, 0, 8 ); 
  myGump.AddText( 170, 150, 0, "Trinsic" ); 
  myGump.AddButton( 150, 155, 0x837, 1, 0, 9 ); 
  myGump.AddText( 170, 170, 0, "Vesper" ); 
  myGump.AddButton( 150, 175, 0x837, 1, 0, 10 ); 
  myGump.AddText( 170, 190, 0, "Yew" ); 
  myGump.AddButton( 150, 195, 0x837, 1, 0, 11 ); 
  myGump.AddText( 170, 210, 0, "Jhelom" ); 
  myGump.AddButton( 150, 215, 0x837, 1, 0, 12 );   	
  }
  if( enableTrammel == 1 )
  {
  	myGump.AddText( 70, 90, 0, "Trammel" ); 
  	myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
  }
  if( enableIlshenar == 1 )
  {
  	myGump.AddText( 70, 110, 0, "Iishenar" ); 
  	myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
  }
  if( enableMalas == 1 )
  {
  	myGump.AddText( 70, 130, 0, "Malas" );
  	myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );    
  }
  if( enableTokuno == 1 )
  {
  	myGump.AddText( 70, 150, 0, "Tokuno Islands" );
  	myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
  }

  //PAGE 2
  myGump.AddPage (2);  //Page 2 Trammel 
  myGump.AddBackground(20, 20, 260, 245, 0x23f0); 
  myGump.AddText( 40, 40, 0, "Pick your destination:" ); 

  myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 ); 
  myGump.AddText( 80, 210, 0, "CANCEL" ); 

  if( enableFelucca == 1 )
  {
  	myGump.AddText( 70, 70, 0, "Felucca" ); 
  	myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 ); 
  }
  if( enableTrammel == 1 )
  {
  	myGump.AddText( 70, 90, 12, "Trammel" ); 
  	myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
  	myGump.AddText( 170, 70, 0, "Britian" ); 
  	myGump.AddButton( 150, 75, 0x837, 1, 0, 13 ); 
  	myGump.AddText( 170, 90, 0, "Magincia" ); 
  	myGump.AddButton( 150, 95, 0x837, 1, 0, 14 ); 
  	myGump.AddText( 170, 110, 0, "Moonglow" ); 
  	myGump.AddButton( 150, 115, 0x837, 1, 0, 15 ); 
  	myGump.AddText( 170, 130, 0, "Skara Brea" ); 
  	myGump.AddButton( 150, 135, 0x837, 1, 0, 16 ); 
  	myGump.AddText( 170, 150, 0, "Trinsic" ); 
  	myGump.AddButton( 150, 155, 0x837, 1, 0, 17 ); 
  	myGump.AddText( 170, 170, 0, "Vesper" ); 
  	myGump.AddButton( 150, 175, 0x837, 1, 0, 18 ); 
  	myGump.AddText( 170, 190, 0, "Yew" ); 
  	myGump.AddButton( 150, 195, 0x837, 1, 0, 19 ); 
  	myGump.AddText( 170, 210, 0, "Jhelom" ); 
  	myGump.AddButton( 150, 215, 0x837, 1, 0, 20 );  	
  }
  if( enableIlshenar == 1 )
  {
  	myGump.AddText( 70, 110, 0, "Iishenar" ); 
  	myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
  }
  if( enableMalas == 1 )
  {
  	myGump.AddText( 70, 130, 0, "Malas" );
  	myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );    
  }
  if( enableTokuno == 1 )
  {
  	myGump.AddText( 70, 150, 0, "Tokuno Islands" );
  	myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
  }
 
  //PAGE 3
  myGump.AddPage (3);  //Page 3 IIsh 
  myGump.AddBackground(20, 20, 260, 245, 0x23f0); 
  myGump.AddText( 40, 40, 0, "Pick your destination:" ); 

  myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 ); 
  myGump.AddText( 80, 210, 0, "CANCEL" ); 

  if( enableFelucca == 1 )
  {
  	myGump.AddText( 70, 70, 0, "Felucca" ); 
  	myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 ); 
  }
  if( enableTrammel == 1 )
  {
  	myGump.AddText( 70, 90, 0, "Trammel" ); 
  	myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
  }
  if( enableIlshenar == 1 )
  {
  	myGump.AddText( 70, 110, 12, "Iishenar" ); 
  	myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );

  	myGump.AddText( 170, 70, 0, "Compassion" ); 
  	myGump.AddButton( 150, 75, 0x837, 1, 0, 21 ); 
  	myGump.AddText( 170, 90, 0, "Honesty" ); 
  	myGump.AddButton( 150, 95, 0x837, 1, 0, 22 ); 
  	myGump.AddText( 170, 110, 0, "Honor" ); 
  	myGump.AddButton( 150, 115, 0x837, 1, 0, 23 ); 
  	myGump.AddText( 170, 130, 0, "Humility" ); 
  	myGump.AddButton( 150, 135, 0x837, 1, 0, 24 ); 
  	myGump.AddText( 170, 150, 0, "Justice" ); 
  	myGump.AddButton( 150, 155, 0x837, 1, 0, 25 ); 
  	myGump.AddText( 170, 170, 0, "Sacrafice" ); 
  	myGump.AddButton( 150, 175, 0x837, 1, 0, 26 ); 
  	myGump.AddText( 170, 190, 0, "Sprirituality" ); 
  	myGump.AddButton( 150, 195, 0x837, 1, 0, 27 ); 
  	myGump.AddText( 170, 210, 0, "Valor" ); 
  	myGump.AddButton( 150, 215, 0x837, 1, 0, 28 ); 
  	myGump.AddText( 170, 230, 0, "Chaos" ); 
  	myGump.AddButton( 150, 235, 0x837, 1, 0, 29 );   	  	
  }
  if( enableMalas == 1 )
  {
  	myGump.AddText( 70, 130, 0, "Malas" );
  	myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );    
  }
  if( enableTokuno == 1 )
  {
  	myGump.AddText( 70, 150, 0, "Tokuno Islands" );
  	myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
  }

  //PAGE 4
  myGump.AddPage (4);  //Page 4 Malas
  myGump.AddBackground(20, 20, 260, 245, 0x23f0); 
  myGump.AddText( 40, 40, 0, "Pick your destination:" ); 
	
  myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 ); 
  myGump.AddText( 80, 210, 0, "CANCEL" ); 
	
  if( enableFelucca == 1 )
  {
  	myGump.AddText( 70, 70, 0, "Felucca" ); 
  	myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 ); 
  }
  if( enableTrammel == 1 )
  {
  	myGump.AddText( 70, 90, 0, "Trammel" ); 
  	myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
  }
  if( enableIlshenar == 1 )
  {
  	myGump.AddText( 70, 110, 0, "Iishenar" ); 
  	myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
  }
  if( enableMalas == 1 )
  {
  	myGump.AddText( 70, 130, 12, "Malas" );
  	myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );

  	myGump.AddText( 170, 70, 0, "New Luna" ); 
  	myGump.AddButton( 150, 75, 0x837, 1, 0, 30 ); 
  	myGump.AddText( 170, 90, 0, "Old Luna" ); 
  	myGump.AddButton( 150, 95, 0x837, 1, 0, 31 ); 
  	myGump.AddText( 170, 110, 0, "Umbra" ); 
  	myGump.AddButton( 150, 115, 0x837, 1, 0, 32 ); 
  }
  if( enableTokuno == 1 )
  {
  	myGump.AddText( 70, 150, 0, "Tokuno Islands" );
  	myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
  }

  //PAGE 5
  myGump.AddPage (5);  //Page 4 Tokuno Islands
  myGump.AddBackground(20, 20, 260, 245, 0x23f0); 
  myGump.AddText( 40, 40, 0, "Pick your destination:" ); 

  myGump.AddButton( 47, 207, 0xfa5, 1, 0, 0 ); 
  myGump.AddText( 80, 210, 0, "CANCEL" ); 

  if( enableFelucca == 1 )
  {
  	myGump.AddText( 70, 70, 0, "Felucca" ); 
  	myGump.AddButton( 48, 72, 0x4b9, 0, 1, 0 ); 
  }
  if( enableTrammel == 1 )
  {
  	myGump.AddText( 70, 90, 0, "Trammel" ); 
  	myGump.AddButton( 48, 95, 0x4b9, 0, 2, 0 );
  }
  if( enableIlshenar == 1 )
  {
  	myGump.AddText( 70, 110, 0, "Iishenar" ); 
  	myGump.AddButton( 48, 115, 0x4b9, 0, 3, 0 );
  }
  if( enableMalas == 1 )
  {
  	myGump.AddText( 70, 130, 0, "Malas" );
  	myGump.AddButton( 48, 135, 0x4b9, 0, 4, 0 );    
  }
  if( enableTokuno == 1 )
  {
  	myGump.AddText( 70, 150, 12, "Tokuno Islands" );
  	myGump.AddButton( 48, 155, 0x4b9, 0, 5, 0 );
  	myGump.AddText( 170, 70, 0, "Isamu~Jima" ); 
  	myGump.AddButton( 150, 75, 0x837, 1, 0, 33 ); 
  	myGump.AddText( 170, 90, 0, "Makoto~Jima" ); 
  	myGump.AddButton( 150, 95, 0x837, 1, 0, 34 ); 
  	myGump.AddText( 170, 110, 0, "Homare~Jima" ); 
  	myGump.AddButton( 150, 115, 0x837, 1, 0, 35 );  	
  }
  myGump.Send( srcSock ); 
}