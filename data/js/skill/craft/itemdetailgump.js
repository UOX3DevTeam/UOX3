const textHue = 0x480;				// Color of the text.
const Blacksmithing = 4023;
const Carpentry = 4025;
const Alchemy = 4028;
const Fletching = 4029;
const Tailoring = 4030;
const Tinkering = 4032;
const scriptID = 4026; // This script
const Cooking = 4034;
const Cartography = 4035;
const Glassblowing = 4036;

const exceptionalWearablesOnly = true;

function ItemDetailGump( pUser )
{
	var socket = pUser.socket;
	var itemGump = new Gump;
	var createEntry = null;
	var HARVEST;
	var mainSkill;
	switch( pUser.GetTempTag( "ITEMDETAILS" ))
	{
		//Start Blacksmith
		case 1:
			createEntry = CreateEntries[1];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 2:
			createEntry = CreateEntries[2];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 3:
			createEntry = CreateEntries[3];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 4:
			createEntry = CreateEntries[4];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 5:
			createEntry = CreateEntries[5];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 6:
			createEntry = CreateEntries[6];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 7:
			createEntry = CreateEntries[7];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 8:
			createEntry = CreateEntries[8];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 9:
			createEntry = CreateEntries[9];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 10:
			createEntry = CreateEntries[10];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 11:
			createEntry = CreateEntries[11];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 12:
			createEntry = CreateEntries[12];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 13:
			createEntry = CreateEntries[13];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 14:
			createEntry = CreateEntries[14];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 15:
			createEntry = CreateEntries[15];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 16:
			createEntry = CreateEntries[16];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 17:
			createEntry = CreateEntries[17];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 18:
			createEntry = CreateEntries[18];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 19:
			createEntry = CreateEntries[19];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 20:
			createEntry = CreateEntries[20];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 21:
			createEntry = CreateEntries[21];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 22:
			createEntry = CreateEntries[22];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 23:
			createEntry = CreateEntries[23];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 24:
			createEntry = CreateEntries[24];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 25:
			createEntry = CreateEntries[25];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 26:
			createEntry = CreateEntries[26];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 27:
			createEntry = CreateEntries[27];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 28:
			createEntry = CreateEntries[28];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 29:
			createEntry = CreateEntries[29];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 30:
			createEntry = CreateEntries[30];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 31:
			createEntry = CreateEntries[31];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 32:
			createEntry = CreateEntries[32];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 33:
			createEntry = CreateEntries[33];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 34:
			createEntry = CreateEntries[34];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 35:
			createEntry = CreateEntries[35];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 36:
			createEntry = CreateEntries[36];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 37:
			createEntry = CreateEntries[37];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 38:
			createEntry = CreateEntries[38];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 39:
			createEntry = CreateEntries[39];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 40:
			createEntry = CreateEntries[40];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 41:
			createEntry = CreateEntries[41];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 42:
			createEntry = CreateEntries[42];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 43:
			createEntry = CreateEntries[43];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 44:
			createEntry = CreateEntries[44];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 45:
			createEntry = CreateEntries[45];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 46:
			createEntry = CreateEntries[46];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 47:
			createEntry = CreateEntries[47];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 48:
			createEntry = CreateEntries[48];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		case 49:
			createEntry = CreateEntries[49];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.blacksmithing );
			break;
		//Start Carpentry
		case 50:
			createEntry = CreateEntries[50];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 51:
			createEntry = CreateEntries[51];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 52:
			createEntry = CreateEntries[52];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 53:
			createEntry = CreateEntries[53];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 54:
			createEntry = CreateEntries[54];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 55:
			createEntry = CreateEntries[55];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 56:
			createEntry = CreateEntries[56];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 57:
			createEntry = CreateEntries[57];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 58:
			createEntry = CreateEntries[58];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 59:
			createEntry = CreateEntries[59];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 60:
			createEntry = CreateEntries[60];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 61:
			createEntry = CreateEntries[61];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 62:
			createEntry = CreateEntries[62];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 63:
			createEntry = CreateEntries[63];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 64:
			createEntry = CreateEntries[64];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 65:
			createEntry = CreateEntries[65];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 66:
			createEntry = CreateEntries[66];
			HARVEST = [10611, 10612];//missing loops
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 67:
			createEntry = CreateEntries[67];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 68:
			createEntry = CreateEntries[68];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 69:
			createEntry = CreateEntries[69];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 70:
			createEntry = CreateEntries[70];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 71:
			createEntry = CreateEntries[71];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 72:
			createEntry = CreateEntries[72];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 73:
			createEntry = CreateEntries[73];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 74:
			createEntry = CreateEntries[74];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 75:
			createEntry = CreateEntries[75];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 76:
			createEntry = CreateEntries[76];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 77:
			createEntry = CreateEntries[77];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 78:
			createEntry = CreateEntries[78];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 79:
			createEntry = CreateEntries[79];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 80:
			createEntry = CreateEntries[80];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 81:
			createEntry = CreateEntries[81];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 82:
			createEntry = CreateEntries[82];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 83:
			createEntry = CreateEntries[83];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 84:
			createEntry = CreateEntries[84];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 85:
			createEntry = CreateEntries[85];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 86:
			createEntry = CreateEntries[86];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 87:
			createEntry = CreateEntries[87];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 88:
			createEntry = CreateEntries[8];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 89:
			createEntry = CreateEntries[89];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 90:
			createEntry = CreateEntries[90];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 91:
			createEntry = CreateEntries[91];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 92:
			createEntry = CreateEntries[92];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 93:
			createEntry = CreateEntries[93];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 94:
			createEntry = CreateEntries[94];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 95:
			createEntry = CreateEntries[95];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 96:
			createEntry = CreateEntries[96];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 97:
			createEntry = CreateEntries[97];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 98:
			createEntry = CreateEntries[98];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 99:
			createEntry = CreateEntries[99];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 100:
			createEntry = CreateEntries[100];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 101:
			createEntry = CreateEntries[101];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 102:
			createEntry = CreateEntries[102];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 103:
			createEntry = CreateEntries[103];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 104:
			createEntry = CreateEntries[104];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 105:
			createEntry = CreateEntries[105];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 106:
			createEntry = CreateEntries[106];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 107:
			createEntry = CreateEntries[107];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 108:
			createEntry = CreateEntries[108];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 109:
			createEntry = CreateEntries[109];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 110:
			pUser.TextMessage( "Case 110!" );
			createEntry = CreateEntries[110];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 111:
			createEntry = CreateEntries[111];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 112:
			createEntry = CreateEntries[112];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 113:
			createEntry = CreateEntries[113];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 114:
			createEntry = CreateEntries[114];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 115:
			createEntry = CreateEntries[115];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 116:
			createEntry = CreateEntries[116];
			HARVEST = [10014, 10016];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 117:
			createEntry = CreateEntries[117];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 118:
			createEntry = CreateEntries[118];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 119:
			createEntry = CreateEntries[119];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 120:
			createEntry = CreateEntries[120];
			HARVEST = [10014, 10015];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 121:
			createEntry = CreateEntries[121];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 122:
			createEntry = CreateEntries[122];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 123:
			createEntry = CreateEntries[123];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		case 124:
			createEntry = CreateEntries[124];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.carpentry );
			break;
		// Start Tailoring
		case 130:
			createEntry = CreateEntries[130];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 131:
			createEntry = CreateEntries[131];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 132:
			createEntry = CreateEntries[132];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 133:
			createEntry = CreateEntries[133];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 134:
			createEntry = CreateEntries[134];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 135:
			createEntry = CreateEntries[135];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 136:
			createEntry = CreateEntries[136];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 137:
			createEntry = CreateEntries[137];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 138:
			createEntry = CreateEntries[138];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 139:
			createEntry = CreateEntries[139];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 140:
			createEntry = CreateEntries[140];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 141:
			createEntry = CreateEntries[141];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 142:
			createEntry = CreateEntries[142];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 143:
			createEntry = CreateEntries[143];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 144:
			createEntry = CreateEntries[144];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 145:
			createEntry = CreateEntries[145];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 146:
			createEntry = CreateEntries[146];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 147:
			createEntry = CreateEntries[147];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 148:
			createEntry = CreateEntries[148];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 149:
			createEntry = CreateEntries[149];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 150:
			createEntry = CreateEntries[150];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 151:
			createEntry = CreateEntries[151];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 152:
			createEntry = CreateEntries[152];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 153:
			createEntry = CreateEntries[153];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 154:
			createEntry = CreateEntries[154];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 155:
			createEntry = CreateEntries[155];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 156:
			createEntry = CreateEntries[156];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 157:
			createEntry = CreateEntries[157];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 158:
			createEntry = CreateEntries[158];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break
		case 159:
			createEntry = CreateEntries[159];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 160:
			createEntry = CreateEntries[160];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 161:
			createEntry = CreateEntries[161];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 162:
			createEntry = CreateEntries[162];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 163:
			createEntry = CreateEntries[163];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 164:
			createEntry = CreateEntries[164];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 165:
			createEntry = CreateEntries[165];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 166:
			createEntry = CreateEntries[166];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 167:
			createEntry = CreateEntries[167];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 168:
			createEntry = CreateEntries[168];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 169:
			createEntry = CreateEntries[169];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 170:
			createEntry = CreateEntries[170];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 171:
			createEntry = CreateEntries[171];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 172:
			createEntry = CreateEntries[172];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 173:
			createEntry = CreateEntries[173];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 174:
			createEntry = CreateEntries[174];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 175:
			createEntry = CreateEntries[175];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 176:
			createEntry = CreateEntries[176];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 177:
			createEntry = CreateEntries[177];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 178:
			createEntry = CreateEntries[178];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 179:
			createEntry = CreateEntries[179];
			HARVEST = [10007];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 180:
			createEntry = CreateEntries[180];
			HARVEST = [10016];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 181:
			createEntry = CreateEntries[181];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 182:
			createEntry = CreateEntries[182];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 183:
			createEntry = CreateEntries[184];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 184:
			createEntry = CreateEntries[184];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		case 185:
			createEntry = CreateEntries[185];
			HARVEST = [10007, 10008];
			mainSkill = parseInt( pUser.skills.tailoring );
			break;
		// Start Fletching
		case 190:
			createEntry = CreateEntries[190];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 190: // Kindling
			createEntry = CreateEntries[190];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 191: // Bow
			createEntry = CreateEntries[191];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 192: // Crossbow
			createEntry = CreateEntries[192];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 193: // Heavy Crossbow
			createEntry = CreateEntries[193];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 194: // Shaft
			createEntry = CreateEntries[194];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 195: // Five Shafts
			createEntry = CreateEntries[195];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 196: // Twenty Shafts
			createEntry = CreateEntries[196];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 197: // Fifty Shafts
			createEntry = CreateEntries[197];
			HARVEST = [10014]; // Boards or Logs
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 198: // Arrow
			createEntry = CreateEntries[198];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 199: // Five Arrows
			createEntry = CreateEntries[199];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 200: // Twenty Arrows
			createEntry = CreateEntries[200];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 201: // Fifty Arrows
			createEntry = CreateEntries[201];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 202: // Bolt
			createEntry = CreateEntries[202];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 203: // Five Bolts
			createEntry = CreateEntries[203];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 204: // Twenty Bolts
			createEntry = CreateEntries[204];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 205: // Fifty Bolts
			createEntry = CreateEntries[205];
			HARVEST = [10029, 10028]; // Shaft, Feather
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 206:
			createEntry = CreateEntries[206];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 207:
			createEntry = CreateEntries[207];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		case 208:
			createEntry = CreateEntries[208];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.bowcraft );
			break;
		// Start Tinkering
		case 274: // Axle
			createEntry = CreateEntries[274];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 273: // Clock Frame
			createEntry = CreateEntries[273];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 270: // Jointing Plane
			createEntry = CreateEntries[270];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 271: // Moulding Plane
			createEntry = CreateEntries[271];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 272: // Smoothing Plane
			createEntry = CreateEntries[272];
			HARVEST = [10014];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 2 - Tools
		case 218: // Dovetail Saw
			createEntry = CreateEntries[218];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 215: // Draw Knife
			createEntry = CreateEntries[215];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 252: // Froe
			createEntry = CreateEntries[252];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 255: // Hammer
			createEntry = CreateEntries[255];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 214: // Hatchet
			createEntry = CreateEntries[214];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 258: // Inshave
			createEntry = CreateEntries[258];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 260: // Lockpick
			createEntry = CreateEntries[260];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 211: // Mortar and Pestle
			createEntry = CreateEntries[211];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 259: // Pick Axe
			createEntry = CreateEntries[259];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 217: // Saw
			createEntry = CreateEntries[217];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 210: // Scissors
			createEntry = CreateEntries[210];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 212: // Scorp
			createEntry = CreateEntries[212];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 216: // Sewing Kit
			createEntry = CreateEntries[216];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 254: // Shovel
			createEntry = CreateEntries[254];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 257: // Sledge Hammer
			createEntry = CreateEntries[257];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 256: // Smith's Hammer
			createEntry = CreateEntries[256];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 253: // Tongs
			createEntry = CreateEntries[253];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 213: // Tool Kit (Tinker's tools)
			createEntry = CreateEntries[213];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 214: // Fletcher's Tools
			createEntry = CreateEntries[284];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 3 - Parts
		case 224: // Barrel Hoops
			createEntry = CreateEntries[224];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 221: // Barrel Tap
			createEntry = CreateEntries[221];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 220: // Clock parts
			createEntry = CreateEntries[220];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 219: // Gears
			createEntry = CreateEntries[219];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 225: // Hinge
			createEntry = CreateEntries[225];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 223: // Sextant parts
			createEntry = CreateEntries[223];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 222: // Springs
			createEntry = CreateEntries[222];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 4 - Utensils
		case 226: // Butcher Knife
			createEntry = CreateEntries[226];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 232: // Cleaver
			createEntry = CreateEntries[232];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 230: // Fork
			createEntry = CreateEntries[230];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 231: // Fork
			createEntry = CreateEntries[231];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 235: // Goblet
			createEntry = CreateEntries[235];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 233: // Knife
			createEntry = CreateEntries[233];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 234: // Knife
			createEntry = CreateEntries[234];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 236: // Pewter Mug
			createEntry = CreateEntries[236];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 229: // Plate
			createEntry = CreateEntries[229];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 237: // Skinning Knife
			createEntry = CreateEntries[237];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 227: // Spoon
			createEntry = CreateEntries[227];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 228: // Spoon
			createEntry = CreateEntries[228];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 5 - Jewelry
		case 243: // Bracelet
			createEntry = CreateEntries[243];
			HARVEST = [10015, 12005];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 241: // Earrings
			createEntry = CreateEntries[241];
			HARVEST = [10015, 12005];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 239: // Necklage (Golden beads)
			createEntry = CreateEntries[239];
			HARVEST = [10015, 12005];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 240: // Necklace (Silver beads)
			createEntry = CreateEntries[240];
			HARVEST = [10015, 12005];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 242: // Necklace (Round)
			createEntry = CreateEntries[242];
			HARVEST = [10015, 12005];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 238: // Weddingband (newbiefied)
			createEntry = CreateEntries[238];
			HARVEST = [10015, 12006];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 6 - Miscellaneous
		case 248: // Globe
			createEntry = CreateEntries[248];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 251: // Heating stand
			createEntry = CreateEntries[251];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 247: // Iron Key
			createEntry = CreateEntries[247];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 244: // Keyring
			createEntry = CreateEntries[244];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 250: // Lantern
			createEntry = CreateEntries[250];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 246: // Scales
			createEntry = CreateEntries[246];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 249: // Spy glass
			createEntry = CreateEntries[249];
			HARVEST = [10015];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 7 - Multi-Component Items
		case 275: // Axle and Gears
			createEntry = CreateEntries[275];
			HARVEST = [11801, 11863];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 276: // Clock
			createEntry = CreateEntries[276];
			HARVEST = [11802, 11862];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 277: // Clock
			createEntry = CreateEntries[277];
			HARVEST = [11802, 11862];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 278: // Clock Parts
			createEntry = CreateEntries[278];
			HARVEST = [11801, 11863];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 279: // Locked Box
			createEntry = CreateEntries[279];
			HARVEST = [10634];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 280: // Locked Chest
			createEntry = CreateEntries[280];
			HARVEST = [10638];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 281: // Potion Keg
			createEntry = CreateEntries[281];
			HARVEST = [10642, 11861, 10612, 10928];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 282: // Sextant
			createEntry = CreateEntries[282];
			HARVEST = [11948];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 283: // Sextant Parts
			createEntry = CreateEntries[283];
			HARVEST = [11801, 11863];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 8 - Candles
		case 245: // Candelabra
			createEntry = CreateEntries[245];
			HARVEST = [10015, 12000];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 310: // Standing Candelabra
			createEntry = CreateEntries[310];
			HARVEST = [10015, 12000];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 315: // Regular Candle
			createEntry = CreateEntries[315];
			HARVEST = [10015, 12000];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 312: // Round Candle
			createEntry = CreateEntries[312];
			HARVEST = [12000];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 316: // Skull with Candle
			createEntry = CreateEntries[316];
			HARVEST = [12000, 12004];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 314: // Small Candle
			createEntry = CreateEntries[314];
			HARVEST = [12000];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 311: // Tall Candle
			createEntry = CreateEntries[311];
			HARVEST = [10015, 12000];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 313: // Thick Candle
			createEntry = CreateEntries[313];
			HARVEST = [12000];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		// Page 9 - Traps
		case 261: // Dart Trap
			createEntry = CreateEntries[261];
			HARVEST = [10015, 12001];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 263: // Explosion Trap
			createEntry = CreateEntries[263];
			HARVEST = [10015, 12003];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		case 262: // Poison Trap
			createEntry = CreateEntries[262];
			HARVEST = [10015, 12002];
			mainSkill = parseInt( pUser.skills.tinkering );
			break;
		//Start Alchemy
		case 290:
			createEntry = CreateEntries[290];
			HARVEST = [10019];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 291:
			createEntry = CreateEntries[291];
			HARVEST = [10019];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 292:
			createEntry = CreateEntries[292];
			HARVEST = [10020];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 293:
			createEntry = CreateEntries[293];
			HARVEST = [10020];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 294:
			createEntry = CreateEntries[294];
			HARVEST = [10020];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 295:
			createEntry = CreateEntries[295];
			HARVEST = [10021];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 296:
			createEntry = CreateEntries[296];
			HARVEST = [10021];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 297:
			createEntry = CreateEntries[297];
			HARVEST = [10021];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 298:
			createEntry = CreateEntries[298];
			HARVEST = [10022];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 299:
			createEntry = CreateEntries[299];
			HARVEST = [10022];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 300:
			createEntry = CreateEntries[300];
			HARVEST = [10022];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 301:
			createEntry = CreateEntries[301];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 302:
			createEntry = CreateEntries[302];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 303:
			createEntry = CreateEntries[303];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 304:
			createEntry = CreateEntries[304];
			HARVEST = [10024];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 305:
			createEntry = CreateEntries[305];
			HARVEST = [10025];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 306:
			createEntry = CreateEntries[306];
			HARVEST = [10025];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 307:
			createEntry = CreateEntries[307];
			HARVEST = [10026];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 308:
			createEntry = CreateEntries[308];
			HARVEST = [10026];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		case 309:
			createEntry = CreateEntries[309];
			HARVEST = [10023];
			mainSkill = parseInt( pUser.skills.alchemy );
			break;
		// Start Cooking
		// Page 1 - Ingredients
		case 1500: // Sack of Flour
			createEntry = CreateEntries[1500];
			HARVEST = [11636];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1501: // Dough
			createEntry = CreateEntries[1501];
			HARVEST = [11637, 11638];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1502: // Sweet Dough
			createEntry = CreateEntries[1502];
			HARVEST = [11607, 11639];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1503: // Cake Mix
			createEntry = CreateEntries[1503];
			HARVEST = [11637, 11608];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1504: // Cookie Mix
			createEntry = CreateEntries[1504];
			HARVEST = [11639, 11608];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		// Page 2 - Preparation
		case 1550: // Unbaked Quiche
			createEntry = CreateEntries[1550];
			HARVEST = [11607, 11640];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1551: // Unbaked Meat Pie
			createEntry = CreateEntries[1551];
			HARVEST = [11607, 11641];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1552: // Uncooked Sausage Pizza
			createEntry = CreateEntries[1552];
			HARVEST = [11607, 11642];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1553: // Uncooked Cheese Pizza
			createEntry = CreateEntries[1553];
			HARVEST = [11607, 11643];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1554: // Unbaked Fruit Pie
			createEntry = CreateEntries[1554];
			HARVEST = [11607, 11644];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1555: // Unbaked Peach Cobbler
			createEntry = CreateEntries[1555];
			HARVEST = [11607, 11645];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1556: // Unbaked Apple Pie
			createEntry = CreateEntries[1556];
			HARVEST = [11607, 11646];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1557: // Unbaked Pumpkin Pie
			createEntry = CreateEntries[1557];
			HARVEST = [11607, 11647];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		// Page 3 - Baking
		case 1600: // Bread Loaf
			createEntry = CreateEntries[1600];
			HARVEST = [11607];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1601: // Pan of Cookies
			createEntry = CreateEntries[1601];
			HARVEST = [11610];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1602: // Cake
			createEntry = CreateEntries[1602];
			HARVEST = [11609];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1603: // Muffins
			createEntry = CreateEntries[1603];
			HARVEST = [11608];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1604: // Baked Quiche
			createEntry = CreateEntries[1604];
			HARVEST = [11611];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1605: // Baked Meat Pie
			createEntry = CreateEntries[1605];
			HARVEST = [11612];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1606: // Sausage Pizza
			createEntry = CreateEntries[1606];
			HARVEST = [11613];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1607: // Cheese Pizza
			createEntry = CreateEntries[1607];
			HARVEST = [11614];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1608: // Baked Fruit Pie
			createEntry = CreateEntries[1608];
			HARVEST = [11615];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1609: // Baked Peach Cobbler
			createEntry = CreateEntries[1609];
			HARVEST = [11616];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1610: // Baked Apple Pie
			createEntry = CreateEntries[1610];
			HARVEST = [11617];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1611: // Baked Pumpkin Pie
			createEntry = CreateEntries[1611];
			HARVEST = [11618];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		// Page 4 - Barbecue
		case 1650: // Cooked Bird
			createEntry = CreateEntries[1650];
			HARVEST = [11648];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1651: // Chicken Leg
			createEntry = CreateEntries[1651];
			HARVEST = [11649];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1652: // Fish Steak
			createEntry = CreateEntries[1652];
			HARVEST = [11650];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1653: // Fried Eggs
			createEntry = CreateEntries[1653];
			HARVEST = [11651];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1654: // Leg of Lamb
			createEntry = CreateEntries[1654];
			HARVEST = [11652];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
		case 1655: // Cut of Ribs
			createEntry = CreateEntries[1655];
			HARVEST = [11653];
			mainSkill = parseInt( pUser.skills.cooking );
			break;
    // Cartography
		case 2000: // Local Map
			createEntry = CreateEntries[2000];
			HARVEST = [13004];
			mainSkill = parseInt( pUser.skills.cartography );
			break;
		case 2001: // City Map
			createEntry = CreateEntries[2001];
			HARVEST = [13004];
			mainSkill = parseInt( pUser.skills.cartography );
			break;
		case 2002: // Sea Chart
			createEntry = CreateEntries[2002];
			HARVEST = [13004];
			mainSkill = parseInt( pUser.skills.cartography );
			break;
		case 2003: // World Map
			createEntry = CreateEntries[2003];
			HARVEST = [13004];
			mainSkill = parseInt( pUser.skills.cartography );
			break;
		// Glassblowing
		case 3000: // empty bottle
			createEntry = CreateEntries[3000];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3001: // flask (small)
			createEntry = CreateEntries[3001];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3002: // flask (medium)
			createEntry = CreateEntries[3002];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3003: // flask (curved)
			createEntry = CreateEntries[3003];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3004: // flask (large #1)
			createEntry = CreateEntries[3004];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3005: // flask (large #2)
			createEntry = CreateEntries[3005];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3006: // flask (bubbling blue)
			createEntry = CreateEntries[3006];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3007: // flask (bubbling purple)
			createEntry = CreateEntries[3007];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3008: // flask (bubbling red)
			createEntry = CreateEntries[3008];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3009: // empty vials
			createEntry = CreateEntries[3009];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3010: // full vials
			createEntry = CreateEntries[3010];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
			break;
		case 3011: // spinning hourglass
			createEntry = CreateEntries[3011];
			HARVEST = [13504];
			mainSkill = parseInt(pUser.skills.alchemy);
      break;
		default:
			break;
	}

	if( createEntry == null )
	{
		return;
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
	var minRank = createEntry.minRank;
	var maxRank = createEntry.maxRank;

	// List out resource requirements
	for( var i = 0; i < resources.length; i++ )
	{
		var resource = resources[i]
		var amountNeeded = resource[0];
		var resourceColour = resource[1];
        var resourceIDs = resource[2];
	}
	ItemDetailsGump( itemGump, pUser );
	itemGump.AddText( 330, 40, textHue, createName );
	itemGump.AddPicture( 20, 50, createID );
	var minSkillReq = 0;
	var maxSkillReq = 0;

	// List out skill requirements
	var primaryCraftSkill = -1;
	var primaryCraftSkillVal = -1;
	var exceptionalChance = 0;
	var rankSum = 0;
	var rank = 0;
	var rndNum1 = 0;
	var supportSkillTooLow = false;
    for( var i = 0; i < skills.length; i++ )
    {
        var skillReq = skills[i];
        var skillNumber = skillReq[0];
		var minSkill = skillReq[1];
		var maxSkill = skillReq[2];

		itemGump.AddHTMLGump( 170, 132 + ( i * 20 ), 200, 18, 0, 0, "<center> <basefont color=#ffffff>" + GetDictionaryEntry( 15000 + skills[i][0], socket.language ) + "</basefont> </center>" );
		itemGump.AddText( 430, 132 + ( i * 20 ), textHue, skills[i][1]/10 );

		if( i == 0 )
		{
			primaryCraftSkill = Skills[skillNumber];
			primaryCraftSkillVal = ( pUser.skills[skillNumber] / 10 );
			minSkillReq = minSkill;
			maxSkillReq = maxSkill;
		}
		else
		{
			if( pUser.skills[skillNumber] < minSkill )
			{
				// Support skill doesn't meet minimum requirements!
				supportSkillTooLow = true;
				break;
			}
		}

		if( GetServerSetting( "RankSystem" ))
		{
			// Rough estimate of exceptional chance based on rank system
			var rankRange = maxRank - minRank;
			var skillRange = pUser.skills[skillNumber] - minSkill;
			if( skillRange < 0 )
			{
				skillRange = minRank * 10;
			}
			else if( skillRange >= 1000 )
			{
				skillRange = maxRank * 10;
			}

			// Calculate estimated exceptional chance for current skill with the help of a few
			// constants that represent the average of some random numbers used in source code
			// calculations, modified by the SkillLevel setting in UOX.INI, which is a modifier
			// for how hard it is to craft items of exceptional quality (1 = easy, 10 = hard)
			exceptionalChance += 499.5 - (( 499.5 - skillRange ) / ( 11 - parseInt( GetServerSetting( "SkillLevel" )))); // 436 at skill level 10
		}
	}

	// Calculate success rate of crafting based on primary skill
	var chance = (( mainSkill - minSkillReq ) / ( maxSkillReq - minSkillReq ));

	if( GetServerSetting( "RankSystem" ))
	{
		// Estimate for exceptional chance, based on average exceptionalChance of all skills required
		exceptionalChance = ( exceptionalChance / skills.length ) / 10;

		// Modify exceptionalChance by base success rate of crafting
		exceptionalChance *= chance;
	}

	// Give player a minimum chance of crafting at 50% since they met skill requirement
	var chance = Math.max( 0.5, chance );

	if( GetServerSetting( "StatsAffectSkillChecks" ))
	{
		// Modify base chance of success with bonuses from stats, if this feature is enabled in ini
		chance += (( pUser.strength * ( primaryCraftSkill.strength / 10 )) / 10000 );
		chance += (( pUser.dexterity * ( primaryCraftSkill.dexterity / 10 )) / 10000 );
		chance += (( pUser.intelligence * ( primaryCraftSkill.intelligence / 10 )) / 10000 );
	}

	for( var i = 0; i < HARVEST.length; i++ )
	{
		itemGump.AddText( 170, 219 + ( i * 20 ), textHue, GetDictionaryEntry( HARVEST[i], socket.language ));
		itemGump.AddText( 430, 219 + ( i * 20 ), textHue, resources[i][0] );
	}

	if( supportSkillTooLow || chance < 0.0 || primaryCraftSkillVal < ( minSkillReq / 10 ))
	{
		chance = 0;
		exceptionalChance = 0;
	}
	else if( chance > 1.0 )
		chance = 1.0; // Cap chance at 100%

	itemGump.AddText( 430, 80, textHue, ( chance * 100 ).toFixed( 1 ) + "%" ); // Success Chance:
	if( !exceptionalWearablesOnly || CheckTileFlag( createID, 22 )) // TF_WEARABLE
	{
		if( exceptionalChance == 0 )
		{
			itemGump.AddText( 430, 100, textHue, "0%" ); // Exceptional Chance:
		}
		else
		{
			itemGump.AddText( 430, 100, textHue, Math.min( 100, Math.max( 0, ( exceptionalChance - 5 ))).toFixed( 0 ) + "%" + " - " + Math.min( 100, ( exceptionalChance + 5 )).toFixed( 0 ) + "%" ); // Exceptional Chance:
		}
	}
	else if( exceptionalWearablesOnly || !CheckTileFlag( createID, 22 )) // TF_WEARABLE?
	{
		itemGump.AddText( 430, 100, textHue, "-" ); // No chance of exceptional, not a wearable item!
	}
	itemGump.Send( pUser );
	itemGump.Free();
}

function ItemDetailsGump( itemGump, pUser )
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
	if( GetServerSetting( "RankSystem" ))
	{
		itemGump.AddHTMLGump( 170, 100, 250, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10004, socket.language ) + "</basefont>" );  // Exceptional Chance:
	}
	itemGump.AddButton( 15, 387, 0xfa5, 1, 0, 1 );
	itemGump.AddHTMLGump( 50, 390, 150, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10005, socket.language ) + "</basefont>" );  // BACK
	itemGump.AddHTMLGump( 170, ( 302 + 20 ), 310, 18, 0, 0, "<basefont color=#ffffff>" + GetDictionaryEntry( 10006, socket.language ) + "</basefont>" );  // * The item retains the color of this material
	itemGump.AddText(  500, 219, textHue, "*"  );
}

function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;

	// Don't continue if character is invalid, or worse... dead!
	if( !ValidateObject( pUser ) || pUser.dead )
		return;

	var bItem = pSock.tempObj;
	var gumpID = scriptID + 0xffff;
	switch( pButton )
	{
		case 0:
			pUser.SetTempTag( "ITEMDETAILS", null )
			pUser.SetTempTag( "CRAFT", null )
			pSock.CloseGump( gumpID, 0 );
			break;
		case 1:
			switch( pUser.GetTempTag( "CRAFT" ))
			{
				case 1:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
						case 9: // Page 9
						case 10: // Page 10
							TriggerEvent( Carpentry, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Carpentry, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 2:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
							TriggerEvent( Alchemy, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Alchemy, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 3:
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
							TriggerEvent( Fletching, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Fletching, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 4: // Tailoring
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
							TriggerEvent( Tailoring, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Tailoring, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 5: // Blacksmithing
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
							TriggerEvent( Blacksmithing, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Blacksmithing, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 6: // Cooking
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag("page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
							TriggerEvent( Cooking, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Cooking, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 7: // Tinkering
					pUser.SetTempTag( "ITEMDETAILS", null )
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
						case 2: // Page 2
						case 3: // Page 3
						case 4: // Page 4
						case 5: // Page 5
						case 6: // Page 6
						case 7: // Page 7
						case 8: // Page 8
						case 9: // Page 9
							TriggerEvent( Tinkering, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Tinkering, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
        case 8: // Cartography
					pUser.SetTempTag( "ITEMDETAILS", null );
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
							TriggerEvent( Cartography, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Cartography, "PageX", pSock, pUser, 1 );
							break;
					}
					break;
				case 9: // Glassblowing
					pUser.SetTempTag( "ITEMDETAILS", null );
					pSock.CloseGump( gumpID, 0 );
					switch( pUser.GetTempTag( "page" ))
					{
						case 1: // Page 1
							TriggerEvent( Glassblowing, "PageX", pSock, pUser, pUser.GetTempTag( "page" ));
							break;
						default: TriggerEvent( Glassblowing, "PageX", pSock, pUser, 1 );
              break;
          }
          break;
			}
	}
}