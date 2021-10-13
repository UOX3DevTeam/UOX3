var textHue = 0x480;				// Color of the text.
var Blacksmithing = 4023;
var Carpentry = 4025;
var Alchemy = 4028;
var Fletching = 4029;
var Tailoring = 4030;

function ItemDetailGump( pUser )
{
	var socket = pUser.socket;
	var itemGump = new Gump;
	switch ( pUser.GetTag( "ITEMDETAILS" ) )
	{
		//Start Blacksmith
		case 1:
			var createEntry = CreateEntries[1];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 2:
			var createEntry = CreateEntries[2];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 3:
			var createEntry = CreateEntries[3];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 4:
			var createEntry = CreateEntries[4];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 5:
			var createEntry = CreateEntries[5];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 6:
			var createEntry = CreateEntries[6];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 7:
			var createEntry = CreateEntries[7];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 8:
			var createEntry = CreateEntries[8];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 9:
			var createEntry = CreateEntries[9];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 10:
			var createEntry = CreateEntries[10];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 11:
			var createEntry = CreateEntries[11];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 12:
			var createEntry = CreateEntries[12];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 13:
			var createEntry = CreateEntries[13];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 14:
			var createEntry = CreateEntries[14];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 15:
			var createEntry = CreateEntries[15];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 16:
			var createEntry = CreateEntries[16];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 17:
			var createEntry = CreateEntries[17];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 18:
			var createEntry = CreateEntries[18];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 19:
			var createEntry = CreateEntries[19];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 20:
			var createEntry = CreateEntries[20];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 21:
			var createEntry = CreateEntries[21];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 22:
			var createEntry = CreateEntries[22];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 23:
			var createEntry = CreateEntries[23];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 24:
			var createEntry = CreateEntries[24];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 25:
			var createEntry = CreateEntries[25];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 26:
			var createEntry = CreateEntries[26];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 27:
			var createEntry = CreateEntries[27];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 28:
			var createEntry = CreateEntries[28];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 29:
			var createEntry = CreateEntries[29];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 30:
			var createEntry = CreateEntries[30];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 31:
			var createEntry = CreateEntries[31];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 32:
			var createEntry = CreateEntries[32];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 33:
			var createEntry = CreateEntries[33];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 34:
			var createEntry = CreateEntries[34];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 35:
			var createEntry = CreateEntries[35];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 36:
			var createEntry = CreateEntries[36];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 37:
			var createEntry = CreateEntries[37];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 38:
			var createEntry = CreateEntries[38];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 39:
			var createEntry = CreateEntries[39];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 40:
			var createEntry = CreateEntries[40];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 41:
			var createEntry = CreateEntries[41];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 42:
			var createEntry = CreateEntries[42];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 43:
			var createEntry = CreateEntries[43];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 44:
			var createEntry = CreateEntries[44];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 45:
			var createEntry = CreateEntries[45];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 46:
			var createEntry = CreateEntries[46];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 47:
			var createEntry = CreateEntries[47];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 48:
			var createEntry = CreateEntries[48];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 49:
			var createEntry = CreateEntries[49];
			var HARVEST = [10015];
			var MainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		//Start Carpentry
		case 50:
			var createEntry = CreateEntries[50];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 51:
			var createEntry = CreateEntries[51];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 52:
			var createEntry = CreateEntries[52];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 53:
			var createEntry = CreateEntries[53];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 54:
			var createEntry = CreateEntries[54];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 55:
			var createEntry = CreateEntries[55];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 56:
			var createEntry = CreateEntries[56];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 57:
			var createEntry = CreateEntries[57];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 58:
			var createEntry = CreateEntries[58];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 59:
			var createEntry = CreateEntries[59];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 60:
			var createEntry = CreateEntries[60];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 61:
			var createEntry = CreateEntries[61];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 62:
			var createEntry = CreateEntries[62];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 63:
			var createEntry = CreateEntries[63];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 64:
			var createEntry = CreateEntries[64];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 65:
			var createEntry = CreateEntries[65];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 66:
			var createEntry = CreateEntries[66];
			var HARVEST = [10611, 10612];//missing loops
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 67:
			var createEntry = CreateEntries[67];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 68:
			var createEntry = CreateEntries[68];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 69:
			var createEntry = CreateEntries[69];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 70:
			var createEntry = CreateEntries[70];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 71:
			var createEntry = CreateEntries[71];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 72:
			var createEntry = CreateEntries[72];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 73:
			var createEntry = CreateEntries[73];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 74:
			var createEntry = CreateEntries[74];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 75:
			var createEntry = CreateEntries[75];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 76:
			var createEntry = CreateEntries[76];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 77:
			var createEntry = CreateEntries[77];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 78:
			var createEntry = CreateEntries[78];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 79:
			var createEntry = CreateEntries[79];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 80:
			var createEntry = CreateEntries[80];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 81:
			var createEntry = CreateEntries[81];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 82:
			var createEntry = CreateEntries[82];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 83:
			var createEntry = CreateEntries[83];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 84:
			var createEntry = CreateEntries[84];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 85:
			var createEntry = CreateEntries[85];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 86:
			var createEntry = CreateEntries[86];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 87:
			var createEntry = CreateEntries[87];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 88:
			var createEntry = CreateEntries[8];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 89:
			var createEntry = CreateEntries[89];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 90:
			var createEntry = CreateEntries[90];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 91:
			var createEntry = CreateEntries[91];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 92:
			var createEntry = CreateEntries[92];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 93:
			var createEntry = CreateEntries[93];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 94:
			var createEntry = CreateEntries[94];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 95:
			var createEntry = CreateEntries[95];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 96:
			var createEntry = CreateEntries[96];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 97:
			var createEntry = CreateEntries[97];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 98:
			var createEntry = CreateEntries[98];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 99:
			var createEntry = CreateEntries[99];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 100:
			var createEntry = CreateEntries[100];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 101:
			var createEntry = CreateEntries[101];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 102:
			var createEntry = CreateEntries[102];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 103:
			var createEntry = CreateEntries[103];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 104:
			var createEntry = CreateEntries[104];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 105:
			var createEntry = CreateEntries[105];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 106:
			var createEntry = CreateEntries[106];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 107:
			var createEntry = CreateEntries[107];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 108:
			var createEntry = CreateEntries[108];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 109:
			var createEntry = CreateEntries[109];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 110:
			var createEntry = CreateEntries[110];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 111:
			var createEntry = CreateEntries[111];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 112:
			var createEntry = CreateEntries[112];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 113:
			var createEntry = CreateEntries[113];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 114:
			var createEntry = CreateEntries[114];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 115:
			var createEntry = CreateEntries[115];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 116:
			var createEntry = CreateEntries[116];
			var HARVEST = [10014, 10016];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 117:
			var createEntry = CreateEntries[117];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 118:
			var createEntry = CreateEntries[118];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 119:
			var createEntry = CreateEntries[119];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 120:
			var createEntry = CreateEntries[120];
			var HARVEST = [10014, 10015];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 121:
			var createEntry = CreateEntries[121];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 122:
			var createEntry = CreateEntries[122];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 123:
			var createEntry = CreateEntries[123];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 124:
			var createEntry = CreateEntries[124];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.carpentry );
			break;
		// Start Tailoring
		case 130:
			var createEntry = CreateEntries[130];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 131:
			var createEntry = CreateEntries[131];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 132:
			var createEntry = CreateEntries[132];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 133:
			var createEntry = CreateEntries[133];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 134:
			var createEntry = CreateEntries[134];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 135:
			var createEntry = CreateEntries[135];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 136:
			var createEntry = CreateEntries[136];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 137:
			var createEntry = CreateEntries[137];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 138:
			var createEntry = CreateEntries[138];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 139:
			var createEntry = CreateEntries[139];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 140:
			var createEntry = CreateEntries[140];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 141:
			var createEntry = CreateEntries[141];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 142:
			var createEntry = CreateEntries[142];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 143:
			var createEntry = CreateEntries[143];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 144:
			var createEntry = CreateEntries[144];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 145:
			var createEntry = CreateEntries[145];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 146:
			var createEntry = CreateEntries[146];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 147:
			var createEntry = CreateEntries[147];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 148:
			var createEntry = CreateEntries[148];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 149:
			var createEntry = CreateEntries[149];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 150:
			var createEntry = CreateEntries[150];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 151:
			var createEntry = CreateEntries[151];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 152:
			var createEntry = CreateEntries[152];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 153:
			var createEntry = CreateEntries[153];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 154:
			var createEntry = CreateEntries[154];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 155:
			var createEntry = CreateEntries[155];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 156:
			var createEntry = CreateEntries[156];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 157:
			var createEntry = CreateEntries[157];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 158:
			var createEntry = CreateEntries[158];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break
		case 159:
			var createEntry = CreateEntries[159];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 160:
			var createEntry = CreateEntries[160];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 161:
			var createEntry = CreateEntries[161];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 162:
			var createEntry = CreateEntries[162];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 163:
			var createEntry = CreateEntries[163];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 164:
			var createEntry = CreateEntries[164];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 165:
			var createEntry = CreateEntries[165];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 166:
			var createEntry = CreateEntries[166];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 167:
			var createEntry = CreateEntries[167];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 168:
			var createEntry = CreateEntries[168];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 169:
			var createEntry = CreateEntries[169];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 170:
			var createEntry = CreateEntries[170];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 171:
			var createEntry = CreateEntries[171];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 172:
			var createEntry = CreateEntries[172];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 173:
			var createEntry = CreateEntries[173];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 174:
			var createEntry = CreateEntries[174];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 175:
			var createEntry = CreateEntries[175];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 176:
			var createEntry = CreateEntries[176];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 177:
			var createEntry = CreateEntries[177];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 178:
			var createEntry = CreateEntries[178];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 179:
			var createEntry = CreateEntries[179];
			var HARVEST = [10007];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 180:
			var createEntry = CreateEntries[180];
			var HARVEST = [10016];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 181:
			var createEntry = CreateEntries[181];
			var HARVEST = [10007, 10008];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 182:
			var createEntry = CreateEntries[182];
			var HARVEST = [10007, 10008];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 183:
			var createEntry = CreateEntries[184];
			var HARVEST = [10007, 10008];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 184:
			var createEntry = CreateEntries[184];
			var HARVEST = [10007, 10008];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 185:
			var createEntry = CreateEntries[185];
			var HARVEST = [10007, 10008];
			var MainSkill = parseInt( pUser.skills.tailoring );
			break;
		// Start Fletching
		case 190:
			var createEntry = CreateEntries[190];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 190:
			var createEntry = CreateEntries[190];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 191:
			var createEntry = CreateEntries[191];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 192:
			var createEntry = CreateEntries[192];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 193:
			var createEntry = CreateEntries[193];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 194:
			var createEntry = CreateEntries[194];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 195:
			var createEntry = CreateEntries[195];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 196:
			var createEntry = CreateEntries[196];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 197:
			var createEntry = CreateEntries[197];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 198:
			var createEntry = CreateEntries[198];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 199:
			var createEntry = CreateEntries[199];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 200:
			var createEntry = CreateEntries[200];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 201:
			var createEntry = CreateEntries[201];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 202:
			var createEntry = CreateEntries[202];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 203:
			var createEntry = CreateEntries[203];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 204:
			var createEntry = CreateEntries[204];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 205:
			var createEntry = CreateEntries[205];
			var HARVEST = [10014, 10028];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 206:
			var createEntry = CreateEntries[206];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 207:
			var createEntry = CreateEntries[207];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 208:
			var createEntry = CreateEntries[208];
			var HARVEST = [10014];
			var MainSkill = parseInt( pUser.skills.bowcraft );
			break;
		//Start Alchemy
		case 290:
			var createEntry = CreateEntries[290];
			var HARVEST = [10019];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 291:
			var createEntry = CreateEntries[291];
			var HARVEST = [10019];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 292:
			var createEntry = CreateEntries[292];
			var HARVEST = [10020];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 293:
			var createEntry = CreateEntries[293];
			var HARVEST = [10020];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 294:
			var createEntry = CreateEntries[294];
			var HARVEST = [10020];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 295:
			var createEntry = CreateEntries[295];
			var HARVEST = [10021];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 296:
			var createEntry = CreateEntries[296];
			var HARVEST = [10021];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 297:
			var createEntry = CreateEntries[297];
			var HARVEST = [10021];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 298:
			var createEntry = CreateEntries[298];
			var HARVEST = [10022];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 299:
			var createEntry = CreateEntries[299];
			var HARVEST = [10022];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 300:
			var createEntry = CreateEntries[300];
			var HARVEST = [10022];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 301:
			var createEntry = CreateEntries[301];
			var HARVEST = [10024];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 302:
			var createEntry = CreateEntries[302];
			var HARVEST = [10024];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 303:
			var createEntry = CreateEntries[303];
			var HARVEST = [10024];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 304:
			var createEntry = CreateEntries[304];
			var HARVEST = [10024];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 305:
			var createEntry = CreateEntries[305];
			var HARVEST = [10025];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 306:
			var createEntry = CreateEntries[306];
			var HARVEST = [10025];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 307:
			var createEntry = CreateEntries[307];
			var HARVEST = [10026];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 308:
			var createEntry = CreateEntries[308];
			var HARVEST = [10026];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 309:
			var createEntry = CreateEntries[309];
			var HARVEST = [10023];
			var MainSkill = parseInt( pUser.skills.alchemy );
			break;
	}

	// Fetch properties of create entry
	var createName = createEntry.name; // name of the create entry
	var createID = createEntry.id; // section id of item to craft
	var addItem = createEntry.addItem; // section header of item to craft
	var sound = createEntry.sound; // sound to play when crafting item
    var delay = createEntry.delay; // how long it takes to craft the item
	var spell = createEntry.spell; // spell requirement to craft the item
	var resources = createEntry.resources; // list of resources needed
	var skills = createEntry.skills; // list of skill requirements
	var avgMinSkill = createEntry.avgMinSkill; // average min skill required
	var avgMaxSkill = createEntry.avgMaxSkill; // average max skill required
	var chance = ( MainSkill - avgMinSkill ) / ( avgMaxSkill - avgMinSkill ) * 2.0;
	var Exceptional = 0.60 - ( MainSkill*0.1 ) - 95.0 * 0.03;
	// List out resource requirements
	for ( var i = 0; i < resources.length; i++ )
	{
		var resource = resources[i]
		var amountNeeded = resource[0];
		var resourceColour = resource[1];
        var resourceIDs = resource[2];
	}
	itemdetails( itemGump, pUser );
	itemGump.AddText( 330, 40, textHue, createName );
	itemGump.AddPicture( 20, 50, createID );

	// List out skill requirements
    for(  var i = 0; i < skills.length; i++  )
    {
        var skillReq = skills[i];
        var skillNumber = skillReq[0];
		var minSkill = skillReq[1];
		var maxSkill = skillReq[2];
		itemGump.AddHTMLGump( 170, 132 + ( i * 20 ), 200, 18, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 15000 + skills[i][0], socket.language ) + "</basefont> </center>" );
		itemGump.AddText( 430, 132 + ( i * 20 ), textHue, skills[i][1]/10 );
	}

	for ( var i = 0; i < HARVEST.length; i++ )
	{
		itemGump.AddText( 170, 219 + ( i * 20 ), textHue, GetDictionaryEntry( HARVEST[i], socket.language ) );
		itemGump.AddText( 430, 219 + ( i * 20 ), textHue, resources[i][0]  );
	}

	if ( chance < 0.0 )
		chance = 0.0;
	else if ( chance > 1.0 )
		chance = 1.0;

	if ( Exceptional < 0.45 )
		Exceptional = 0.45;
	else if ( Exceptional > 0.60 )
		Exceptional = 0.60;

	itemGump.AddText( 430, 80, textHue, chance.toFixed( 1 ) * 100 + "%" );// Success Chance:
	itemGump.AddText( 430, 100, textHue, Exceptional.toFixed( 1 ) * 100 + "%" );// Exceptional Chance:
	itemGump.Send( pUser );
	itemGump.Free();
}

function itemdetails( itemGump, pUser )
{
	var socket = pUser.socket;
	itemGump.AddPage( 0 );
	itemGump.AddBackground( 0, 0, 530, 417, 5054 );
	itemGump.AddTiledGump( 10, 10, 510, 22, 2624 );
	itemGump.AddTiledGump( 10, 37, 150, 148, 2624 );
	itemGump.AddTiledGump( 165, 37, 355, 90, 2624 );
	itemGump.AddTiledGump( 10, 190, 155, 22, 2624 );
	itemGump.AddTiledGump( 10, 240, 150, 57, 2624 );
	itemGump.AddTiledGump( 165, 132, 355, 80, 2624 );
	itemGump.AddTiledGump( 10, 325, 150, 57, 2624 );
	itemGump.AddTiledGump( 165, 217, 355, 80, 2624 );
	itemGump.AddTiledGump( 165, 302, 355, 80, 2624 );
	itemGump.AddTiledGump( 10, 387, 510, 22, 2624 );
	itemGump.AddCheckerTrans( 10, 10, 510, 399 );
    itemGump.AddHTMLGump( 170, 40, 150, 20, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10000, socket.language ) + "</basefont> </center>" );  // ITEM

	itemGump.AddHTMLGump( 10, 217, 150, 22, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10001, socket.language ) + "</basefont> </center>" );  //<CENTER>MATERIALS</CENTER>
	itemGump.AddHTMLGump( 10, 302, 150, 22, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 10002, socket.language ) + "</basefont> </center>" );  // <CENTER>OTHER</CENTER>
	itemGump.AddHTMLGump( 170, 80, 250, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10003, socket.language ) + "</basefont>" );  // Success Chance:
	itemGump.AddHTMLGump( 170, 100, 250, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10004, socket.language ) + "</basefont>" );  // Exceptional Chance:
	itemGump.AddButton( 15, 387, 0xfa5, 1, 0, 1 );
	itemGump.AddHTMLGump( 50, 390, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10005, socket.language ) + "</basefont>" );  // BACK
	itemGump.AddHTMLGump( 170, ( 302 + 20 ), 310, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10006, socket.language ) + "</basefont>" );  // * The item retains the color of this material
	itemGump.AddText(  500, 219, textHue, "*"  );
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	var bItem = pSock.tempObj;
	var gumpID = scriptID + 0xffff;
	switch ( pButton )
	{
		case 0:
			pUser.SetTag( "ITEMDETAILS", null )
			pUser.SetTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1:
			switch ( pUser.GetTag( "CRAFT" ) )
			{
				case 1:
					pUser.SetTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch ( pUser.GetTag("page" ) )
					{
						case 1:
							TriggerEvent( Carpentry, "page1", pSock, pUser );
							break;
						case 2:
							TriggerEvent( Carpentry, "page2", pSock, pUser );
							break;
						case 3:
							TriggerEvent( Carpentry, "page3", pSock, pUser );
							break;
						case 4:
							TriggerEvent( Carpentry, "page4", pSock, pUser );
							break;
						case 5:
							TriggerEvent( Carpentry, "page5", pSock, pUser );
							break;
						case 6:
							TriggerEvent( Carpentry, "page6", pSock, pUser );
							break;
						case 7:
							TriggerEvent( Carpentry, "page7", pSock, pUser );
							break;
						case 8:
							TriggerEvent( Carpentry, "page8", pSock, pUser );
							break;
						case 9:
							TriggerEvent( Carpentry, "page9", pSock, pUser );
							break;
						case 10:
							TriggerEvent( Carpentry, "page10", pSock, pUser );
							break;
						default: TriggerEvent( Carpentry, "page1", pSock, pUser );
							break;
					}
					break;
				case 2:
					pUser.SetTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch ( pUser.GetTag("page" ) )
					{
						case 1:
							TriggerEvent( Alchemy, "page1", pSock, pUser );
							break;
						case 2:
							TriggerEvent( Alchemy, "page2", pSock, pUser );
							break;
						case 3:
							TriggerEvent( Alchemy, "page3", pSock, pUser );
							break;
						case 4:
							TriggerEvent( Alchemy, "page4", pSock, pUser );
							break;
						default: TriggerEvent( Alchemy, "page1", pSock, pUser );
							break;
					}
					break;
				case 3:
					pUser.SetTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch ( pUser.GetTag("page" ) )
					{
						case 1:
							TriggerEvent( Fletching, "page1", pSock, pUser );
							break;
						case 2:
							TriggerEvent( Fletching, "page2", pSock, pUser );
							break;
						case 3:
							TriggerEvent( Fletching, "page3", pSock, pUser );
							break;
						default: TriggerEvent( Fletching, "page1", pSock, pUser );
							break;
					}
					break;
				case 4:
					pUser.SetTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch ( pUser.GetTag("page" ) )
					{
						case 1:
							TriggerEvent( Tailoring, "page1", pSock, pUser );
							break;
						case 2:
							TriggerEvent( Tailoring, "page2", pSock, pUser );
							break;
						case 3:
							TriggerEvent( Tailoring, "page3", pSock, pUser );
							break;
						case 4:
							TriggerEvent( Tailoring, "page4", pSock, pUser );
							break;
						case 5:
							TriggerEvent( Tailoring, "page5", pSock, pUser );
							break;
						case 6:
							TriggerEvent( Tailoring, "page6", pSock, pUser );
							break;
						case 7:
							TriggerEvent( Tailoring, "page7", pSock, pUser );
							break;
						case 8:
							TriggerEvent( Tailoring, "page8", pSock, pUser );
							break;
						default: TriggerEvent( Tailoring, "page1", pSock, pUser );
							break;
					}
					break;
				case 5:
					pUser.SetTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch ( pUser.GetTag("page" ) )
					{
						case 1:
							TriggerEvent( Blacksmithing, "page1", pSock, pUser );
							break;
						case 2:
							TriggerEvent( Blacksmithing, "page2", pSock, pUser );
							break;
						case 3:
							TriggerEvent( Blacksmithing, "page3", pSock, pUser );
							break;
						case 4:
							TriggerEvent( Blacksmithing, "page4", pSock, pUser );
							break;
						case 5:
							TriggerEvent( Blacksmithing, "page5", pSock, pUser );
							break;
						case 6:
							TriggerEvent( Blacksmithing, "page6", pSock, pUser );
							break;
						case 7:
							TriggerEvent( Blacksmithing, "page7", pSock, pUser );
							break;
						default: TriggerEvent( Blacksmithing, "page1", pSock, pUser );
							break;
					}
					break;
			}
	}
}