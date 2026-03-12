/// <reference path="../../definitions.d.ts" />
// @ts-check
// Version: 1.1.5

// Max amount of money the vendor can hold in the bank.
// Held gold is not included in this limit, but will
// be used first to pay upkeep before bank funds are used.
var vendorMaxFunds = 1000000;

// If true, returned vendor belongings skip player backpack entirely.
// Items, gold and vendor deed will try bank box first, then drop at vendor location.
// If false, belongings try backpack first, then bank box, then ground.
var onlyReturnToBank = true;

// Vendor upkeep settings
var vendorChargesEnabled = true; // Master toggle for vendor upkeep charges
var vendorBaseCharge = 60;       // Flat fee per charge period
var vendorChargeHours = 24;      // Charge every X real hours
var vendorUseItemFees = true;    // Add item-based fee from listed item prices
var vendorItemFeeDivisor = 500;  // 3 gold per 500 worth of one item
var vendorItemFeeAmount = 3;     // Fee added per divisor step

var layerOneHand        = 0x01;
var layerTwoHand        = 0x02;
var layerShoes          = 0x03;
var layerPants          = 0x04;
var layerShirt          = 0x05;
var layerHelm           = 0x06;
var layerGloves         = 0x07;
var layerRing           = 0x08;
var layerNeck           = 0x0A;
var layerHair           = 0x0B;
var layerWaist          = 0x0C;
var layerInnerTorso     = 0x0D;
var layerBracelet       = 0x0E;
var layerBeard          = 0x10;
var layerMiddleTorso    = 0x11;
var layerEarrings       = 0x12;
var layerArms           = 0x13;
var layerCloak          = 0x14;
var layerOuterTorso     = 0x16;
var layerOuterLegs      = 0x17;

/** @constructor @param {string|null} name @param {number[] } hues @param {number|null} dictID */
function VendorHueCategory( name, hues, dictID )
{
	this.name = name;
	this.hues = hues;
	this.dictID = dictID;
}

/** @param {VendorHueCategory} entry @param {Socket|null} socket @returns {string} */
function GetVendorHueCategoryName( entry, socket )
{
	if( entry.dictID && socket )
		return GetDictionaryEntry( entry.dictID, socket.language );

	return entry.name || "";
}

var vendorHairHueCategories = [
	new VendorHueCategory( null, [ 0x044E, 0x044F, 0x0450, 0x0451, 0x0452, 0x0453, 0x0454 ], 40092 ), // Black
	new VendorHueCategory( null, [ 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045A, 0x045B, 0x045C ], 40093 ), // Beige
	new VendorHueCategory( null, [ 0x045D, 0x045E, 0x045F, 0x0460, 0x0461, 0x0462, 0x0463, 0x0464, 0x0465, 0x0466, 0x0467, 0x0468, 0x0469, 0x046A, 0x046B, 0x046C ], 40094 ), // Golden
	new VendorHueCategory( null, [ 0x046D, 0x046E, 0x046F, 0x0470, 0x0471, 0x0472, 0x0473, 0x0474, 0x0475, 0x0476, 0x0477, 0x0478, 0x0479, 0x047A, 0x047B, 0x047C ], 40095 ), // Dark Brown
	new VendorHueCategory( null, [ 0x04B1, 0x04B2, 0x04B3, 0x04B4, 0x04B5, 0x04B6, 0x04B7, 0x04B8, 0x04B9, 0x04BA, 0x04BB, 0x04BC, 0x04BD, 0x04BE, 0x04BF, 0x04C0 ], 40096 ), // Red
	new VendorHueCategory( null, [ 0x05DD, 0x05DE, 0x05DF, 0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7, 0x05E8, 0x05E9, 0x05EA, 0x05EB, 0x05EC ], 40097 ), // Auburn
	new VendorHueCategory( null, [ 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649, 0x064A, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652, 0x0653, 0x0654, 0x0655, 0x0656, 0x0657, 0x0658, 0x0659, 0x065A, 0x065B, 0x065C, 0x065D, 0x065E, 0x065F, 0x0660 ], 40098 ), // Brown
	new VendorHueCategory( null, [ 0x08A5, 0x08A6, 0x08A7, 0x08A8, 0x08A9, 0x0961, 0x0962, 0x0963, 0x0964, 0x0965, 0x0966, 0x0967, 0x0968 ], 40099 ), // Blonde
	new VendorHueCategory( null, [ 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 ], 40100 ), // Special Purple
	new VendorHueCategory( null, [ 32, 33, 34, 35, 36, 37 ], 40101 ), // Special Red
	new VendorHueCategory( null, [ 38, 39, 40, 41, 42, 43, 44, 45, 46 ], 40102 ), // Special Orange
	new VendorHueCategory( null, [ 54, 55, 56, 57 ], 40103 ), // Special Yellow
	new VendorHueCategory( null, [ 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72 ], 40104 ), // Special Lime
	new VendorHueCategory( null, [ 81, 82, 83 ], 40105 ), // Special Dark Lime
	new VendorHueCategory( null, [ 89, 90, 91 ], 40106 ), // Special Ice Blue
	new VendorHueCategory( null, [ 1153, 1154, 1155 ], 40107 ) // Special Ice White
];

/** @constructor @param {number} itemID @param {string} name */
function HairOrBeard( itemID, name, dictID )
{
	this.itemID = itemID;
	this.name = name;
	this.dictID = dictID;
}

/** @param {HairOrBeard} entry @param {Socket} socket @returns {string} */
function GetHairOrBeardName( entry, socket )
{
	if( entry.dictID )
		return GetDictionaryEntry( entry.dictID, socket.language );

	return entry.name || "";
}

var femaleElfHairStyles = [
	new HairOrBeard( 0x2FCC, null, 40060 ), // Flower
	new HairOrBeard( 0x2FC0, null, 40061 ), // Long Feather
	new HairOrBeard( 0x2FC1, null, 40048 ), // Short
	new HairOrBeard( 0x2FC2, null, 40062 ), // Mullet
	new HairOrBeard( 0x2FCE, null, 40063 ), // Topknot
	new HairOrBeard( 0x2FCF, null, 40064 ), // Long Braid
	new HairOrBeard( 0x2FD0, null, 40065 ), // Buns
	new HairOrBeard( 0x2FD1, null, 40066 )  // Spiked
];

var maleElfHairStyles = [
	new HairOrBeard( 0x2FBF, null, 40067 ), // Mid Long
	new HairOrBeard( 0x2FC0, null, 40061 ), // Long Feather
	new HairOrBeard( 0x2FC1, null, 40048 ), // Short
	new HairOrBeard( 0x2FC2, null, 40062 ), // Mullet
	new HairOrBeard( 0x2FCE, null, 40063 ), // Topknot
	new HairOrBeard( 0x2FCF, null, 40064 ), // Long Braid
	new HairOrBeard( 0x2FCD, null, 40049 ), // Long
	new HairOrBeard( 0x2FD1, null, 40066 )  // Spiked
];

var humanHairStyles = [
	new HairOrBeard( 0x203B, null, 40048 ), // Short
	new HairOrBeard( 0x203C, null, 40049 ), // Long
	new HairOrBeard( 0x203D, null, 40050 ), // Ponytail
	new HairOrBeard( 0x2044, null, 40051 ), // Mohawk
	new HairOrBeard( 0x2045, null, 40052 ), // Pageboy
	new HairOrBeard( 0x204A, null, 40063 ), // Topknot
	new HairOrBeard( 0x2047, null, 40053 ), // Curly
	new HairOrBeard( 0x2048, null, 40054 ), // Receding
	new HairOrBeard( 0x2049, null, 40055 )  // 2-Tails
];

var humanBeardStyles = [
	new HairOrBeard( 0x2041, null, 40056 ), // Mustache
	new HairOrBeard( 0x203F, null, 40057 ), // Short Beard
	new HairOrBeard( 0x204B, null, 40058 ), // Short Beard and Mustache
	new HairOrBeard( 0x203E, null, 40059 ), // Long Beard
	new HairOrBeard( 0x204C, null, 40068 ), // Long Beard and Mustache
	new HairOrBeard( 0x2040, null, 40069 ), // Goatee
	new HairOrBeard( 0x204D, null, 40070 )  // Vandyke
];

/** @constructor @param {number} itemID @param {string} name */
function VendorClothingEntry( itemID, name )
{
	this.itemID = itemID;
	this.name = name;
}

/** @constructor @param {number} layer @param {string} name @param {boolean} canDye @param {VendorClothingEntry[]} entries @param {boolean=} compactLayout*/
function VendorClothingCategory( layer, name, canDye, entries, compactLayout )
{
	this.layer = layer;
	this.name = name;
	this.canDye = canDye;
	this.entries = entries;
	this.compactLayout = compactLayout ? true : false;
}

var vendorClothingCategories = [
	new VendorClothingCategory( layerInnerTorso, "Upper Torso", true, [
		new VendorClothingEntry( 0x1517, "Shirt" ),
		new VendorClothingEntry( 0x1EFD, "Fancy Shirt" ),
		new VendorClothingEntry( 0x1F01, "Plain Dress" ),
		new VendorClothingEntry( 0x1EFF, "Fancy Dress" ),
		new VendorClothingEntry( 0x1F03, "Robe" )
	]),

	new VendorClothingCategory( layerMiddleTorso, "Over Chest", true, [
		new VendorClothingEntry( 0x1F7B, "Doublet" ),
		new VendorClothingEntry( 0x1FA1, "Tunic" ),
		new VendorClothingEntry( 0x1F9F, "Jester Suit" ),
		new VendorClothingEntry( 0x1541, "Body Sash" ),
		new VendorClothingEntry( 0x1FFD, "Surcoat" ),
		new VendorClothingEntry( 0x153B, "Half Apron" ),
		new VendorClothingEntry( 0x153D, "Full Apron" )
	]),

	new VendorClothingCategory( layerShoes, "Footwear", true, [
		new VendorClothingEntry( 0x170D, "Sandals" ),
		new VendorClothingEntry( 0x1710, "Shoes" ),
		new VendorClothingEntry( 0x170B, "Boots" ),
		new VendorClothingEntry( 0x1711, "Thigh Boots" )
	]),

	new VendorClothingCategory( layerHelm, "Hats", true, [
		new VendorClothingEntry( 0x1544, "Skull Cap" ),
		new VendorClothingEntry( 0x1540, "Bandana" ),
		new VendorClothingEntry( 0x1713, "Floppy Hat" ),
		new VendorClothingEntry( 0x1714, "Wide Brim Hat" ),
		new VendorClothingEntry( 0x1715, "Cap" ),
		new VendorClothingEntry( 0x1716, "Tall Straw Hat" ),
		new VendorClothingEntry( 0x1717, "Straw Hat" ),
		new VendorClothingEntry( 0x1718, "Wizard Hat" ),
		new VendorClothingEntry( 0x1719, "Bonnet" ),
		new VendorClothingEntry( 0x171A, "Feathered Hat" ),
		new VendorClothingEntry( 0x171B, "Tricorne Hat" ),
		new VendorClothingEntry( 0x171C, "Jester Hat" )
	]),

	new VendorClothingCategory( layerPants, "Lower Torso", true, [
		new VendorClothingEntry( 0x1539, "Long Pants" ),
		new VendorClothingEntry( 0x1537, "Kilt" ),
		new VendorClothingEntry( 0x1516, "Skirt" )
	]),

	new VendorClothingCategory( layerCloak, "Back", true, [
		new VendorClothingEntry( 0x1515, "Cloak" )
	]),

	new VendorClothingCategory( layerOneHand, "Held Items", false, [
		new VendorClothingEntry( 0x0DBF, "Fishing Pole" ),
		new VendorClothingEntry( 0x0E86, "Pickaxe" ),
		new VendorClothingEntry( 0x0E87, "Pitchfork" ),
		new VendorClothingEntry( 0x0EC3, "Cleaver" ),
		new VendorClothingEntry( 0x0F5C, "Mace" ),
		new VendorClothingEntry( 0x0F6B, "Torch" ),
		new VendorClothingEntry( 0x102A, "Hammer" ),
		new VendorClothingEntry( 0x0F61, "Longsword" ),
		new VendorClothingEntry( 0x13F8, "Gnarled Staff" ),
		new VendorClothingEntry( 0x0F4F, "Crossbow" ),
		new VendorClothingEntry( 0x1407, "War Mace" ),
		new VendorClothingEntry( 0x1443, "Two-Handed Axe" ),
		new VendorClothingEntry( 0x0F62, "Spear" ),
		new VendorClothingEntry( 0x13FF, "Katana" ),
		new VendorClothingEntry( 0x0EFA, "Spellbook" )
	], true)
];

/** @param {Character} vendor @returns {boolean} */
function IsPlayerVendor( vendor )
{
	return ValidateObject( vendor ) && vendor.aitype == 17;
}

/** @param {Character} vendor @param {Character} pUser @returns {boolean} */
function IsVendorOwner( vendor, pUser )
{
	if( !IsPlayerVendor( vendor ) || !ValidateObject( pUser ))
		return false;

	if( !ValidateObject( vendor.owner ))
		return false;

	return vendor.owner.serial == pUser.serial;
}

/** @param {Character} vendor @returns {number} */
function GetVendorHeldGold( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return 0;

	return vendor.vendorGoldHeld || 0;
}

/** @param {Character} vendor @param {number} amount */
function SetVendorHeldGold( vendor, amount )
{
	if( !IsPlayerVendor( vendor ))
		return;

	if( amount < 0 )
		amount = 0;

	vendor.vendorGoldHeld = amount;
}

/** @param {Character} vendor @returns {number} */
function GetVendorBankAccount( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return 0;

	var bankAccount = vendor.GetTag( "VendorBankAccount" );
	if( bankAccount < 0 )
		return 0;

	return bankAccount;
}

/** @param {Character} vendor @param {number} amount */
function SetVendorBankAccount( vendor, amount )
{
	if( !IsPlayerVendor( vendor ))
		return;

	if( amount < 0 )
		amount = 0;

	vendor.SetTag( "VendorBankAccount", amount );
}

/** @returns {number} */
function GetCurrentTimestamp()
{
	return Math.floor( GetCurrentClock() / 1000 );
}

/** @param {Character} vendor @returns {number} */
function GetVendorTotalFunds( vendor )
{
	return GetVendorHeldGold( vendor ) + GetVendorBankAccount( vendor );
}

/** @param {Character} vendor @returns {number} */
function GetVendorLastChargeTime( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return 0;

	return vendor.GetTag( "VendorLastChargeTime" );
}

/** @param {Character} vendor @param {number} timestamp */
function SetVendorLastChargeTime( vendor, timestamp )
{
	if( !IsPlayerVendor( vendor ))
		return;

	vendor.SetTag( "VendorLastChargeTime", timestamp );
}

/** @param {Item} itemObj @returns {number} */
function GetVendorItemPrice( itemObj )
{
	if( !ValidateObject( itemObj ))
		return 0;

	// Adjust this if your vendor sale price is stored elsewhere
	if( itemObj.buyValue > 0 )
		return itemObj.buyValue;

	return 0;
}

/** @param {Item} itemObj @returns {number} */
function GetVendorItemPeriodFee( itemObj )
{
	if( !ValidateObject( itemObj ))
		return 0;

	var itemPrice = GetVendorItemPrice( itemObj );
	if( itemPrice <= 0 )
		return 0;

	return Math.floor( itemPrice / vendorItemFeeDivisor ) * vendorItemFeeAmount;
}

/** @param {Character} vendor @returns {number} */
function GetVendorItemFeeTotal( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return 0;

	var vendorPack = vendor.pack;
	if( !ValidateObject( vendorPack ))
		return 0;

	var totalFee = 0;
	var packItem = vendorPack.FirstItem();

	for( ; !vendorPack.FinishedItems(); packItem = vendorPack.NextItem() )
	{
		if( !ValidateObject( packItem ))
			continue;

		totalFee += GetVendorItemPeriodFee( packItem );
	}

	return totalFee;
}

/** @param {Character} vendor @returns {number} */
function GetVendorChargePerPeriod( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return 0;

	if( !vendorChargesEnabled )
		return 0;

	var overrideValue = vendor.GetTag( "VendorChargePerPeriod" );
	if( overrideValue > 0 )
		return overrideValue;

	var totalCharge = vendorBaseCharge;

	if( vendorUseItemFees )
		totalCharge += GetVendorItemFeeTotal( vendor );

	return totalCharge;
}

/** @param {Character} vendor @param {number} amount @returns {number} */
function DepositVendorBankGold( vendor, amount )
{
	if( !IsPlayerVendor( vendor ) || amount <= 0 )
		return 0;

	var bankAccount = GetVendorBankAccount( vendor );
	var roomLeft = vendorMaxFunds - bankAccount;

	if( roomLeft <= 0 )
		return 0;

	if( amount > roomLeft )
		amount = roomLeft;

	SetVendorBankAccount( vendor, bankAccount + amount );
	return amount;
}

/** @param {Character} vendor @param {number} amount @returns {number} */
function WithdrawVendorBankGold( vendor, amount )
{
	if( !IsPlayerVendor( vendor ) || amount <= 0 )
		return 0;

	var bankAccount = GetVendorBankAccount( vendor );
	if( amount > bankAccount )
		amount = bankAccount;

	SetVendorBankAccount( vendor, bankAccount - amount );
	return amount;
}

/** @param {Character} vendor @param {number} charge @returns {boolean} */
function PayVendorCharge( vendor, charge )
{
	if( !IsPlayerVendor( vendor ) || charge <= 0 )
		return false;

	var bankAccount = GetVendorBankAccount( vendor );
	var heldGold = GetVendorHeldGold( vendor );

	if(( bankAccount + heldGold ) < charge )
		return false;

	if( bankAccount >= charge )
	{
		SetVendorBankAccount( vendor, bankAccount - charge );
		return true;
	}

	charge -= bankAccount;
	SetVendorBankAccount( vendor, 0 );
	SetVendorHeldGold( vendor, heldGold - charge );
	return true;
}

/** @param {Character} vendor @param {number} amount @returns {number} */
function MoveBankToHeldGold( vendor, amount )
{
	if( !IsPlayerVendor( vendor ) || amount <= 0 )
		return 0;

	var bankAccount = GetVendorBankAccount( vendor );
	if( amount > bankAccount )
		amount = bankAccount;

	SetVendorBankAccount( vendor, bankAccount - amount );
	SetVendorHeldGold( vendor, GetVendorHeldGold( vendor ) + amount );
	return amount;
}

/** @param {Character} vendor @returns {number} */
function FlushVendorBankToHeldGold( vendor )
{
	var bankAccount = GetVendorBankAccount( vendor );
	if( bankAccount <= 0 )
		return 0;

	SetVendorBankAccount( vendor, 0 );
	SetVendorHeldGold( vendor, GetVendorHeldGold( vendor ) + bankAccount );
	return bankAccount;
}

/** @param {Character} vendor @returns {number} */
function GetVendorPeriodsAffordable( vendor )
{
	var perPeriod = GetVendorChargePerPeriod( vendor );
	if( perPeriod <= 0 )
		return 0;

	return Math.floor( GetVendorTotalFunds( vendor ) / perPeriod );
}

/** @param {Character} vendor @returns {number} */
function GetVendorHoursAffordable( vendor )
{
	return GetVendorPeriodsAffordable( vendor ) * vendorChargeHours;
}

/** @param {Character} vendor @returns {number} */
function GetVendorDaysAffordable( vendor )
{
	var hoursAffordable = GetVendorHoursAffordable( vendor );
	return Math.floor( hoursAffordable / 24 );
}

/** @param {Item} itemObj @param {Character} vendor */
function DropItemAtVendorLocation( itemObj, vendor )
{
	if( !ValidateObject( itemObj ) || !ValidateObject( vendor ))
		return;

	itemObj.container = null;
	itemObj.x = vendor.x;
	itemObj.y = vendor.y;
	itemObj.z = vendor.z;
	itemObj.worldnumber = vendor.worldnumber;
	itemObj.instanceID = vendor.instanceID;
}

/** @param {Character} pUser @returns {Item|null} */
function GetBankBox( pUser )
{
	if( !ValidateObject( pUser ))
		return null;

	var bankBox = pUser.FindItemLayer( 29 );
	if( !ValidateObject( bankBox ))
		return null;

	return bankBox;
}

/** @param {Item} container @param {Item} itemObj @returns {boolean} */
function CanContainerReceiveItem( container, itemObj )
{
	if( !ValidateObject( container ) || !ValidateObject( itemObj ))
		return false;

	if( container.totalItemCount >= container.maxItems )
		return false;

	var itemWeight = itemObj.weight * itemObj.amount;
	if(( container.weight + itemWeight ) > container.weightMax )
		return false;

	return true;
}

/** @param {Item} itemObj @param {Item} container @returns {boolean} */
function TryMoveItemToContainer( itemObj, container )
{
	if( !ValidateObject( itemObj ) || !ValidateObject( container ))
		return false;

	if( !CanContainerReceiveItem( container, itemObj ))
		return false;

	itemObj.container = container;

	return ValidateObject( itemObj.container ) && itemObj.container.serial == container.serial;
}

/** @param {Item} itemObj @param {Character} pUser @param {Character} vendor @returns {number} */
function PlaceItemForOwnerOrDrop( itemObj, pUser, vendor )
{
	if( !ValidateObject( itemObj ) || !ValidateObject( pUser ) || !ValidateObject( vendor ))
		return 0;

	var bankBox;
	var pPack;

	if( onlyReturnToBank )
	{
		bankBox = GetBankBox( pUser );
		if( ValidateObject( bankBox ) && TryMoveItemToContainer( itemObj, bankBox ))
			return 2;

		DropItemAtVendorLocation( itemObj, vendor );
		return 3;
	}

	pPack = pUser.pack;
	if( ValidateObject( pPack ) && TryMoveItemToContainer( itemObj, pPack ))
		return 1;

	bankBox = GetBankBox( pUser );
	if( ValidateObject( bankBox ) && TryMoveItemToContainer( itemObj, bankBox ))
		return 2;

	DropItemAtVendorLocation( itemObj, vendor );
	return 3;
}

/** @param {Character} vendor @param {Character} pUser @returns {{pack:number, bank:number, ground:number}} */
function ReturnVendorItemsToOwner( vendor, pUser )
{
	var results = { pack:0, bank:0, ground:0 };

	if( !IsPlayerVendor( vendor ) || !ValidateObject( pUser ))
		return results;

	var vendorPack = vendor.pack;
	if( !ValidateObject( vendorPack ))
		return results;

	var itemList = [];
	var packItem = vendorPack.FirstItem();

	for( ; !vendorPack.FinishedItems(); packItem = vendorPack.NextItem() )
	{
		if( ValidateObject( packItem ))
			itemList.push( packItem );
	}

	for( var i = 0; i < itemList.length; ++i )
	{
		var moveItem = itemList[i];
		if( !ValidateObject( moveItem ))
			continue;

		var placeResult = PlaceItemForOwnerOrDrop( moveItem, pUser, vendor );
		if( placeResult == 1 )
			results.pack++;
		else if( placeResult == 2 )
			results.bank++;
		else if( placeResult == 3 )
			results.ground++;
	}

	return results;
}

/** @param {Character} vendor @returns {number} */
function DropVendorItemsAtLocation( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return 0;

	var vendorPack = vendor.pack;
	if( !ValidateObject( vendorPack ))
		return 0;

	var itemList = [];
	var packItem = vendorPack.FirstItem();

	for( ; !vendorPack.FinishedItems(); packItem = vendorPack.NextItem() )
	{
		if( ValidateObject( packItem ))
			itemList.push( packItem );
	}

	for( var i = 0; i < itemList.length; ++i )
	{
		if( ValidateObject( itemList[i] ))
			DropItemAtVendorLocation( itemList[i], vendor );
	}

	return itemList.length;
}

/** @param {Character} vendor @returns {Item|null} */
function CreateVendorDismissDeedAtLocation( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return null;

	var tempName = "A vendor deed for " + vendor.name + ".";
	var pvDeed = CreateBlankItem( null, vendor, 1, tempName, 0x14F0, 0x0, "ITEM", false );
	if( !ValidateObject( pvDeed ))
		return null;

	pvDeed.type = 217;
	pvDeed.buyValue = 2000;
	pvDeed.SetTag( "vendorName", vendor.name );

	DropItemAtVendorLocation( pvDeed, vendor );
	return pvDeed;
}

/** @param {Character} pUser @param {Character} vendor @returns {Item|null} */
function CreateVendorDismissDeedForOwner( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return null;

	var socket = pUser.socket;
	var tempName;

	if( socket )
	{
		tempName = GetDictionaryEntry( 580, socket.language );
		tempName = tempName.replace( /%s/gi, vendor.name );
	}
	else
	{
		tempName = "A vendor deed for " + vendor.name + ".";
	}

	var pvDeed = CreateBlankItem( null, pUser, 1, tempName, 0x14F0, 0x0, "ITEM", false );
	if( !ValidateObject( pvDeed ))
		return null;

	pvDeed.type = 217;
	pvDeed.buyValue = 2000;
	pvDeed.SetTag( "vendorName", vendor.name );

	return pvDeed;
}

/** @param {Character} vendor @returns {number} */
function DropVendorGoldAtLocation( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return 0;

	var holdGold = GetVendorHeldGold( vendor );
	var bankGold = GetVendorBankAccount( vendor );
	var totalGold = holdGold + bankGold;

	if( totalGold <= 0 )
		return 0;

	var goldItem = CreateDFNItem( null, vendor, "0x0EED", totalGold, "ITEM", false );
	if( !ValidateObject( goldItem ))
		return 0;

	SetVendorHeldGold( vendor, 0 );
	SetVendorBankAccount( vendor, 0 );

	DropItemAtVendorLocation( goldItem, vendor );
	return totalGold;
}

/** @param {Character} pUser @param {Character} vendor @returns {{amount:number, location:number}} */
function ReturnVendorGoldToOwner( pUser, vendor )
{
	var result = { amount:0, location:0 };

	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return result;

	var holdGold = GetVendorHeldGold( vendor );
	var bankGold = GetVendorBankAccount( vendor );
	var totalGold = holdGold + bankGold;

	if( totalGold <= 0 )
		return result;

	var goldItem = CreateDFNItem( null, pUser, "0x0EED", totalGold, "ITEM", false );
	if( !ValidateObject( goldItem ))
		return result;

	var placeResult = PlaceItemForOwnerOrDrop( goldItem, pUser, vendor );
	if( placeResult == 0 )
		return result;

	SetVendorHeldGold( vendor, 0 );
	SetVendorBankAccount( vendor, 0 );

	result.amount = totalGold;
	result.location = placeResult;
	return result;
}

/** @param {Character} vendor */
function FinalizeVendorRemoval( vendor )
{
	if( !IsPlayerVendor( vendor ))
		return;

	var iMulti = null;
	if( ValidateObject( vendor.multi ))
		iMulti = vendor.multi;

	if( ValidateObject( iMulti ))
		iMulti.RemoveVendor( vendor );

	vendor.Delete();
}

/** @param {Character} vendor @param {Character|null} notifyChar */
function RemoveVendorForNoFunds( vendor, notifyChar )
{
	if( !IsPlayerVendor( vendor ))
		return;

	var ownerChar = null;
	if( ValidateObject( vendor.owner ))
		ownerChar = vendor.owner;

	if( ValidateObject( ownerChar ) && ownerChar.socket )
	{
		var socket = ownerChar.socket;
		socket.SysMessage( GetDictionaryEntry( 40000, socket.language ), vendor.name ); // %s can no longer afford upkeep and has been dismissed.

		var pvDeed = CreateVendorDismissDeedForOwner( ownerChar, vendor );
		if( !ValidateObject( pvDeed ))
		{
			socket.SysMessage( GetDictionaryEntry( 40001, socket.language )); // Unable to create vendor deed. Vendor dismissal aborted.
			return;
		}

		var itemResults = ReturnVendorItemsToOwner( vendor, ownerChar );
		var goldResult = ReturnVendorGoldToOwner( ownerChar, vendor );
		var deedLocation = PlaceItemForOwnerOrDrop( pvDeed, ownerChar, vendor );

		if(( itemResults.pack + itemResults.bank + itemResults.ground ) > 0 )
		{
			if( onlyReturnToBank )
			{
				socket.SysMessage( GetDictionaryEntry( 40002, socket.language ), itemResults.bank, itemResults.ground ); // Vendor items returned - bank: %i, ground: %i.
			}
			else
			{
				socket.SysMessage( GetDictionaryEntry( 40003, socket.language ), itemResults.pack, itemResults.bank, itemResults.ground ); // Vendor items returned - pack: %i, bank: %i, ground: %i.
			}
		}

		if( goldResult.amount > 0 )
		{
			if( goldResult.location == 1 )
				socket.SysMessage( GetDictionaryEntry( 40004, socket.language ), goldResult.amount ); // Returned %i gold to your pack.
			else if( goldResult.location == 2 )
				socket.SysMessage( GetDictionaryEntry( 40005, socket.language ), goldResult.amount ); // Returned %i gold to your bank box.
			else if( goldResult.location == 3 )
				socket.SysMessage( GetDictionaryEntry( 40006, socket.language ), goldResult.amount ); // Dropped %i gold at vendor location.
		}

		if( deedLocation == 1 )
			socket.SysMessage( GetDictionaryEntry( 40007, socket.language )); // Placed vendor deed in your pack.
		else if( deedLocation == 2 )
			socket.SysMessage( GetDictionaryEntry( 40008, socket.language )); // Placed vendor deed in your bank box.
		else if( deedLocation == 3 )
			socket.SysMessage( GetDictionaryEntry( 40009, socket.language )); // Dropped vendor deed at vendor location.

		FinalizeVendorRemoval( vendor );
		return;
	}

	var itemCount = DropVendorItemsAtLocation( vendor );
	var totalGold = DropVendorGoldAtLocation( vendor );
	var pvDeedOffline = CreateVendorDismissDeedAtLocation( vendor );

	if( ValidateObject( notifyChar ) && notifyChar.socket )
	{
		notifyChar.socket.SysMessage( GetDictionaryEntry( 40010, notifyChar.socket.language ), vendor.name ); // %s could not afford upkeep and was dismissed.

		if( itemCount > 0 )
			notifyChar.socket.SysMessage( GetDictionaryEntry( 40011, notifyChar.socket.language ), itemCount ); // Dropped %i vendor item(s) at vendor location.

		if( totalGold > 0 )
			notifyChar.socket.SysMessage( GetDictionaryEntry( 40006, notifyChar.socket.language ), totalGold ); // Dropped %i gold at vendor location.

		if( ValidateObject( pvDeedOffline ))
			notifyChar.socket.SysMessage( GetDictionaryEntry( 40009, notifyChar.socket.language )); // Dropped vendor deed at vendor location.
	}

	FinalizeVendorRemoval( vendor );
}

/** @param {Character} vendor @param {Character|null} notifyChar @returns {boolean} */
function CheckVendorUpkeep( vendor, notifyChar )
{
	if( !IsPlayerVendor( vendor ))
		return false;

	if( !vendorChargesEnabled )
		return true;

	var currentTime = GetCurrentTimestamp();
	var lastChargeTime = GetVendorLastChargeTime( vendor );
	var chargeInterval = vendorChargeHours * 60 * 60;

	if( chargeInterval <= 0 )
		return true;

	if( lastChargeTime <= 0 )
	{
		SetVendorLastChargeTime( vendor, currentTime );
		return true;
	}

	if( currentTime <= lastChargeTime )
		return true;

	var elapsed = currentTime - lastChargeTime;
	var periodsOwed = Math.floor( elapsed / chargeInterval );

	if( periodsOwed <= 0 )
		return true;

	for( var i = 0; i < periodsOwed; ++i )
	{
		if( !PayVendorCharge( vendor, GetVendorChargePerPeriod( vendor )))
		{
			RemoveVendorForNoFunds( vendor, notifyChar );
			return false;
		}
	}

	SetVendorLastChargeTime( vendor, lastChargeTime + ( periodsOwed * chargeInterval ));
	return true;
}

/** @param {Character} pUser @param {Character} vendor */
function StoreVendorForGump( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	pUser.SetTempTag( "PlayerVendorGumpSerial", vendor.serial );
}

/** @param {Character} pUser @returns {Character|null} */
function GetStoredVendorForGump( pUser )
{
	if( !ValidateObject( pUser ))
		return null;

	var vendorSerial = pUser.GetTempTag( "PlayerVendorGumpSerial" );
	if( !vendorSerial )
		return null;

	var vendor = CalcCharFromSer( vendorSerial );
	if( !IsPlayerVendor( vendor ))
		return null;

	return vendor;
}

/** @param {Character} pUser @param {Character} vendor */
function OpenVendorBackpack( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	var pPack = vendor.pack;
	if( ValidateObject( pPack ))
		socket.OpenContainer( pPack );
}

/** @param {Socket} socket @param {Character} vendor */
function ShowPlayerVendorOwnerGump( socket, vendor )
{
	if( !socket || !IsPlayerVendor( vendor ))
		return;

	var holdGold = GetVendorHeldGold( vendor );
	var bankAccount = GetVendorBankAccount( vendor );
	var perPeriod = GetVendorChargePerPeriod( vendor );
	var days = GetVendorDaysAffordable( vendor );
	var hours = GetVendorHoursAffordable( vendor );
	var itemFeeTotal = GetVendorItemFeeTotal( vendor );

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 25, 10, 555, 190, 5054 );
	myGump.AddCheckerTrans( 32, 20, 537, 170 );
	myGump.NoResize();

	myGump.AddButton( 390, 25, 4005, 4007, 1, 0, 1 );
	myGump.AddHTMLGump( 425, 25, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40029, socket.language ) + "</basefont>" ); // See goods

	myGump.AddButton( 390, 48, 4005, 4007, 1, 0, 2 );
	myGump.AddHTMLGump( 425, 48, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40030, socket.language ) + "</basefont>" ); // Customize

	myGump.AddButton( 390, 71, 4005, 4007, 1, 0, 3 );
	myGump.AddHTMLGump( 425, 72, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40031, socket.language ) + "</basefont>" ); // Collect Gold

	myGump.AddButton( 390, 94, 4005, 4007, 1, 0, 4 );
	myGump.AddHTMLGump( 425, 95, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40032, socket.language ) + "</basefont>" ); // Deposit 1000

	myGump.AddButton( 390, 117, 4005, 4007, 1, 0, 5 );
	myGump.AddHTMLGump( 425, 118, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40033, socket.language ) + "</basefont>" ); // Flush Bank

	myGump.AddButton( 390, 140, 4005, 4007, 1, 0, 6 );
	myGump.AddHTMLGump( 425, 141, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40034, socket.language ) + "</basefont>" ); // Dismiss

	myGump.AddButton( 390, 163, 4005, 4007, 1, 0, 0 );
	myGump.AddHTMLGump( 425, 164, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 18762, socket.language ) + "</basefont>" ); // CANCEL

	if( !vendorChargesEnabled )
	{
		myGump.AddHTMLGump( 40, 25, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40035, socket.language ) + "</basefont>" ); // Amount of days I can work:
		myGump.AddText( 300, 25, 2603, GetDictionaryEntry( 40043, socket.language ) ); // Unlimited

		myGump.AddHTMLGump( 40, 48, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40036, socket.language ) + "</basefont>" ); // Hours I can work:
		myGump.AddText( 300, 48, 2603, GetDictionaryEntry( 40043, socket.language ) ); // Unlimited

		myGump.AddHTMLGump( 40, 72, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40037, socket.language ) + "</basefont>" ); // Gold held for you:
		myGump.AddText( 300, 72, 1153, "" + holdGold );

		myGump.AddHTMLGump( 40, 96, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40038, socket.language ) + "</basefont>" ); // Gold held in my account:
		myGump.AddText( 300, 96, 1153, "" + bankAccount );

		myGump.AddHTMLGump( 40, 120, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40039, socket.language ) + "</basefont>" ); // Vendor upkeep:
		myGump.AddText( 300, 120, 2603, GetDictionaryEntry( 40044, socket.language ) ); // Disabled
	}
	else
	{
		myGump.AddHTMLGump( 40, 25, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40035, socket.language ) + "</basefont>" ); // Amount of days I can work:
		myGump.AddText( 300, 25, 1153, "" + days );

		myGump.AddHTMLGump( 40, 48, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40036, socket.language ) + "</basefont>" ); // Hours I can work:
		myGump.AddText( 300, 48, 1153, "" + hours );

		myGump.AddHTMLGump( 40, 72, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40037, socket.language ) + "</basefont>" ); // Gold held for you:
		myGump.AddText( 300, 72, 1153, "" + holdGold );

		myGump.AddHTMLGump( 40, 96, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40038, socket.language ) + "</basefont>" ); // Gold held in my account:
		myGump.AddText( 300, 96, 1153, "" + bankAccount );

		myGump.AddHTMLGump( 40, 120, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40040, socket.language ) + "</basefont>" ); // My charge per period is:
		myGump.AddText( 300, 120, 1153, "" + perPeriod );

		myGump.AddHTMLGump( 40, 144, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40041, socket.language ) + "</basefont>" ); // Charge period hours:
		myGump.AddText( 300, 144, 1153, "" + vendorChargeHours );

		myGump.AddHTMLGump( 40, 168, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40042, socket.language ) + "</basefont>" ); // Item fee total:
		myGump.AddText( 300, 168, 1153, "" + itemFeeTotal );
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @param {Character} pUser @returns {number} */
function GetPackGoldAmount( pUser )
{
	if( !ValidateObject( pUser ))
		return 0;

	var pPack = pUser.pack;
	if( !ValidateObject( pPack ))
		return 0;

	var totalGold = 0;
	var goldItem = pPack.FirstItem();

	for( ; !pPack.FinishedItems(); goldItem = pPack.NextItem() )
	{
		if( !ValidateObject( goldItem ))
			continue;

		if( goldItem.id == 0x0EED )
			totalGold += goldItem.amount;
	}

	return totalGold;
}

/** @param {Character} pUser @param {number} amount @returns {boolean} */
function RemoveGoldFromPack( pUser, amount )
{
	if( !ValidateObject( pUser ) || amount <= 0 )
		return false;

	var pPack = pUser.pack;
	if( !ValidateObject( pPack ))
		return false;

	var totalGold = GetPackGoldAmount( pUser );
	if( totalGold < amount )
		return false;

	var goldStacks = [];
	var goldItem = pPack.FirstItem();

	for( ; !pPack.FinishedItems(); goldItem = pPack.NextItem() )
	{
		if( ValidateObject( goldItem ) && goldItem.id == 0x0EED )
			goldStacks.push( goldItem );
	}

	for( var i = 0; i < goldStacks.length && amount > 0; ++i )
	{
		var stack = goldStacks[i];
		if( !ValidateObject( stack ))
			continue;

		if( stack.amount <= amount )
		{
			amount -= stack.amount;
			stack.Delete();
		}
		else
		{
			stack.amount = stack.amount - amount;
			amount = 0;
		}
	}

	return amount <= 0;
}

/** @param {Character} pUser @returns {boolean} */
function CanBankReceiveVendorPayout( pUser )
{
	if( !ValidateObject( pUser ))
		return false;

	var bankBox = GetBankBox( pUser );
	if( !ValidateObject( bankBox ))
		return false;

	if( bankBox.totalItemCount >= bankBox.maxItems )
		return false;

	if( bankBox.weight > bankBox.weightMax )
		return false;

	return true;
}

/** @param {Character} pUser @param {number} checkSize @returns {Item|null} */
function CreateVendorBankCheck( pUser, checkSize )
{
	if( !ValidateObject( pUser ) || checkSize <= 0 )
		return null;

	var socket = pUser.socket;
	var bankBox = GetBankBox( pUser );
	if( !ValidateObject( bankBox ))
		return null;

	var newCheck = CreateDFNItem( socket, pUser, "bankcheck", 1, "ITEM", false );
	if( !ValidateObject( newCheck ))
		return null;

	newCheck.SetTag( "CheckSize", checkSize );
	newCheck.AddScriptTrigger( 5015 );
	newCheck.container = bankBox;

	return newCheck;
}

/** @param {Character} pUser @param {number} goldAmount @returns {boolean} */
function DepositVendorGoldToBankBox( pUser, goldAmount )
{
	if( !ValidateObject( pUser ) || goldAmount <= 0 )
		return false;

	var bankBox = GetBankBox( pUser );
	if( !ValidateObject( bankBox ))
		return false;

	if( bankBox.totalItemCount >= bankBox.maxItems )
		return false;

	if( bankBox.weight > bankBox.weightMax )
		return false;

	DivideDepositedGold( pUser, bankBox, goldAmount );
	return true;
}

/** @param {Character} pTalking @param {Item} bankBox @param {number} depositAmt @returns {void} */
function DivideDepositedGold( pTalking, bankBox, depositAmt )
{
	var numOfGoldPiles = ( depositAmt / 65535 );
	var i = 0;
	var newGoldPile;
	var remainingGold = depositAmt;
	for( i = 1; i < numOfGoldPiles + 1; i++ )
	{
		if( remainingGold >= 65535 )
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", 65535, "ITEM", false );
			remainingGold = remainingGold - 65535;
			newGoldPile.container = bankBox;
		}
		else
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", remainingGold, "ITEM", false );
			newGoldPile.container = bankBox;
			continue;
		}
	}
}

/** @param {Character} pUser @param {Character} vendor */
function CollectVendorGold( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	var holdGold = GetVendorHeldGold( vendor );
	if( holdGold <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 40012, socket.language )); // This vendor has no held gold to collect.
		return;
	}

	if( !CanBankReceiveVendorPayout( pUser ))
	{
		socket.SysMessage( GetDictionaryEntry( 40013, socket.language )); // Your bank box cannot receive vendor payouts right now.
		return;
	}

	if( holdGold > 2000 )
	{
		var vendorCheck = CreateVendorBankCheck( pUser, holdGold );
		if( !ValidateObject( vendorCheck ))
		{
			socket.SysMessage( GetDictionaryEntry( 40014, socket.language )); // Unable to create a bank check for vendor gold.
			return;
		}

		SetVendorHeldGold( vendor, 0 );
		socket.SysMessage( GetDictionaryEntry( 40015, socket.language ), holdGold ); // Collected %i gold as a bank check in your bank box.
		return;
	}

	if( !DepositVendorGoldToBankBox( pUser, holdGold ))
	{
		socket.SysMessage( GetDictionaryEntry( 40016, socket.language )); // Unable to deposit vendor gold into your bank box.
		return;
	}

	SetVendorHeldGold( vendor, 0 );
	socket.SysMessage( GetDictionaryEntry( 40017, socket.language ), holdGold ); // Collected %i gold to your bank box.
}

/** @param {Character} pUser @param {Character} vendor @param {number} amount */
function DepositToVendorBank( pUser, vendor, amount )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	if( amount <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 40018, socket.language )); // Invalid deposit amount.
		return;
	}

	var pPack = pUser.pack;
	if( !ValidateObject( pPack ))
	{
		socket.SysMessage( GetDictionaryEntry( 40019, socket.language )); // You have no backpack.
		return;
	}

	var totalPackGold = GetPackGoldAmount( pUser );
	if( totalPackGold < amount )
	{
		socket.SysMessage( GetDictionaryEntry( 40020, socket.language )); // You do not have enough gold in your pack.
		return;
	}

	var deposited = DepositVendorBankGold( vendor, amount );
	if( deposited <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 40021, socket.language )); // Vendor bank account is full.
		return;
	}

	if( !RemoveGoldFromPack( pUser, deposited ))
	{
		WithdrawVendorBankGold( vendor, deposited );
		socket.SysMessage( GetDictionaryEntry( 40022, socket.language )); // Unable to remove gold from your pack. Deposit cancelled.
		return;
	}

	socket.SysMessage( GetDictionaryEntry( 40023, socket.language ), deposited ); // Deposited %i gold into vendor bank account.

	if( deposited < amount )
		socket.SysMessage( GetDictionaryEntry( 40024, socket.language )); // Vendor bank reached its maximum balance.
}

/** @param {Character} pUser @param {Character} vendor */
function FlushBankToHeld( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	var moved = FlushVendorBankToHeldGold( vendor );
	if( moved <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 40025, socket.language )); // Vendor bank account is empty.
		return;
	}

	socket.SysMessage( GetDictionaryEntry( 40026, socket.language ), moved ); // Moved %i gold from vendor bank account to held gold.
}

/** @param {Character} pUser @param {Character} vendor */
function DismissVendor( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	if( !IsVendorOwner( vendor, pUser ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	var pvDeed = CreateVendorDismissDeedForOwner( pUser, vendor );
	if( !ValidateObject( pvDeed ))
	{
		socket.SysMessage( GetDictionaryEntry( 40027, socket.language )); // Unable to create vendor deed. Dismiss cancelled.
		return;
	}

	var itemResults = ReturnVendorItemsToOwner( vendor, pUser );
	var goldResult = ReturnVendorGoldToOwner( pUser, vendor );
	var deedLocation = PlaceItemForOwnerOrDrop( pvDeed, pUser, vendor );

	socket.SysMessage( GetDictionaryEntry( 581, socket.language ), pvDeed.name ); // Thy deed for %s has been placed in thy pack.

	if(( itemResults.pack + itemResults.bank + itemResults.ground ) > 0 )
	{
		if( onlyReturnToBank )
		{
			socket.SysMessage( GetDictionaryEntry( 40002, socket.language ), itemResults.bank, itemResults.ground ); // Vendor items returned - bank: %i, ground: %i.
		}
		else
		{
			socket.SysMessage( GetDictionaryEntry( 40003, socket.language ), itemResults.pack, itemResults.bank, itemResults.ground ); // Vendor items returned - pack: %i, bank: %i, ground: %i.
		}
	}

	if( goldResult.amount > 0 )
	{
		if( goldResult.location == 1 )
			socket.SysMessage( GetDictionaryEntry( 40004, socket.language ), goldResult.amount ); // Returned %i gold to your pack.
		else if( goldResult.location == 2 )
			socket.SysMessage( GetDictionaryEntry( 40005, socket.language ), goldResult.amount ); // Returned %i gold to your bank box.
		else if( goldResult.location == 3 )
			socket.SysMessage( GetDictionaryEntry( 40006, socket.language ), goldResult.amount ); // Dropped %i gold at vendor location.
	}

	if( deedLocation == 1 )
		socket.SysMessage( GetDictionaryEntry( 40007, socket.language )); // Placed vendor deed in your pack.
	else if( deedLocation == 2 )
		socket.SysMessage( GetDictionaryEntry( 40008, socket.language )); // Placed vendor deed in your bank box.
	else if( deedLocation == 3 )
		socket.SysMessage( GetDictionaryEntry( 40009, socket.language )); // Dropped vendor deed at vendor location.

	FinalizeVendorRemoval( vendor );
}

/** @param {string} speech @returns {string} */
function NormalizeSpeech( speech )
{
	if( !speech )
		return "";

	return ( "" + speech ).toLowerCase().replace( /^\s+|\s+$/g, "" );
}

/** @param {Character} vendor @param {string} speech @returns {string} */
function StripVendorNamePrefix( vendor, speech )
{
	if( !IsPlayerVendor( vendor ))
		return speech;

	var vendorName = "";
	if( vendor.name )
		vendorName = ( "" + vendor.name ).toLowerCase();

	if( vendorName == "" )
		return speech;

	if( speech.indexOf( vendorName + " " ) == 0 )
		return speech.substring( vendorName.length + 1 );

	if( speech.indexOf( vendorName + "," ) == 0 )
		return speech.substring( vendorName.length + 1 ).replace( /^\s+|\s+$/g, "" );

	return speech;
}

/** @param {Character} pUser @param {Character} vendor @param {string} speech @returns {boolean} */
function MatchVendorCommand( pUser, vendor, speech )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return false;

	var cmd = NormalizeSpeech( speech );
	cmd = StripVendorNamePrefix( vendor, cmd );

	if( cmd == "view" || cmd == "browse" || cmd == "look" )
		return true;

	if( cmd == "buy" || cmd == "purchase" )
		return true;

	if( cmd == "gold" || cmd == "collect" || cmd == "get" )
		return true;

	if( cmd == "status" || cmd == "info" )
		return true;

	if( cmd == "dismiss" || cmd == "replace" )
		return true;

	return false;
}

/** @param {Character} pUser @param {Character} vendor @param {string} speech @returns {number} */
function HandleVendorSpeechCommand( pUser, vendor, speech )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return 0;

	var socket = pUser.socket;
	var cmd = NormalizeSpeech( speech );
	cmd = StripVendorNamePrefix( vendor, cmd );

	if( cmd == "view" || cmd == "browse" || cmd == "look" )
	{
		if( !CheckVendorUpkeep( vendor, pUser ))
			return 2;

		OpenVendorBackpack( pUser, vendor );
		return 2;
	}

	if( cmd == "buy" || cmd == "purchase" )
	{
		if( !CheckVendorUpkeep( vendor, pUser ))
			return 2;

		pUser.BuyFrom( vendor );
		return 2;
	}

	if( cmd == "gold" || cmd == "collect" || cmd == "get" )
	{
		if( !IsVendorOwner( vendor, pUser ))
		{
			vendor.TextMessage( GetDictionaryEntry( 40028 )); // I don't work for you!
			return 2;
		}

		if( !CheckVendorUpkeep( vendor, pUser ))
			return 2;

		CollectVendorGold( pUser, vendor );
		return 2;
	}

	if( cmd == "status" || cmd == "info" )
	{
		if( !IsVendorOwner( vendor, pUser ))
		{
			vendor.TextMessage( GetDictionaryEntry( 40028 )); // I don't work for you!
			return 2;
		}

		if( !CheckVendorUpkeep( vendor, pUser ))
			return 2;

		if( socket )
		{
			StoreVendorForGump( pUser, vendor );
			ShowPlayerVendorOwnerGump( socket, vendor );
		}
		return 2;
	}

	if( cmd == "dismiss" || cmd == "replace" )
	{
		if( !IsVendorOwner( vendor, pUser ))
		{
			vendor.TextMessage( GetDictionaryEntry( 40028 )); // I don't work for you!
			return 2;
		}

		DismissVendor( pUser, vendor );
		return 2;
	}

	return 0;
}

/** @type { ( pUser: Character, targChar: Character, nonMouseClickEvent: boolean ) => boolean } */
function onCharDoubleClick( pUser, targChar, nonMouseClickEvent )
{
	if( !ValidateObject( pUser ) || !ValidateObject( targChar ))
		return false;

	if( nonMouseClickEvent )
		return true;

	if( !IsPlayerVendor( targChar ))
		return true;

	if( !CheckVendorUpkeep( targChar, pUser ))
		return false;

	if( IsVendorOwner( targChar, pUser ))
	{
		if( pUser.socket )
		{
			StoreVendorForGump( pUser, targChar );
			ShowPlayerVendorOwnerGump( pUser.socket, targChar );
		}
		return false;
	}

	OpenVendorBackpack( pUser, targChar );
	return false;
}

/** @param {string} speech @param {Character} personTalking @param {BaseObject} talkingTo @returns {number} */
function onSpeech( speech, personTalking, talkingTo )
{
	if( !ValidateObject( personTalking ) || !ValidateObject( talkingTo ))
		return 0;

	if( !IsPlayerVendor( talkingTo ))
		return 0;

	if( !MatchVendorCommand( personTalking, talkingTo, speech ))
		return 0;

	return HandleVendorSpeechCommand( personTalking, talkingTo, speech );
}

/** @param {Character} socket @param {Gump} myGump @param {number} menuPage @param {number} categoryPageStart @param {string} title */
function AddHairHueCategoryMenu( socket, myGump, menuPage, categoryPageStart, title )
{
	myGump.AddPage( menuPage );
	myGump.AddHTMLGump( 10, 42, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + title + "</CENTER></basefont>" );

	var columns = 2;
	var startX = 20;
	var startY = 70;
	var colWidth = 160;
	var rowHeight = 20;

	for( var i = 0; i < vendorHairHueCategories.length; ++i )
	{
		var row = Math.floor( i / columns );
		var col = i % columns;
		var x = startX + ( col * colWidth );
		var y = startY + ( row * rowHeight );

		myGump.AddButton( x, y, 0x0A9A, 0x0A9A, 0, categoryPageStart + i, 0 );
		myGump.AddHTMLGump( x + 30, y, 125, 18, false, false, "<basefont color=#ffffff>" + GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) + "</basefont>" );
	}

	myGump.AddButton( 270, 340, 0xFA5, 0xFA7, 1, 1, 5007 );
	myGump.AddHTMLGump( 305, 342, 60, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" ); // BACK
}

/** @param {Character} socket  @param {Gump} myGump @param {number} pageNumber @param {string} title @param {number[]} hueList @param {number} buttonBase @param {number} backPage */
function AddHairHueCategoryPage( socket, myGump, pageNumber, title, hueList, buttonBase, backPage )
{
	myGump.AddPage( pageNumber );
	myGump.AddHTMLGump( 10, 42, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + title + "</CENTER></basefont>" );

	var hueColumns = 3;
	var startX = 20;
	var startY = 70;
	var colWidth = 112;
	var rowHeight = 18;

	for( var i = 0; i < hueList.length; ++i )
	{
		var row = Math.floor( i / hueColumns );
		var col = i % hueColumns;
		var x = startX + ( col * colWidth );
		var y = startY + ( row * rowHeight );
		var hueHex = hueList[i].toString( 16 ).toUpperCase();

		myGump.AddButton( x, y, 0x0A9A, 0x0A9A, 1, pageNumber, buttonBase + hueList[i] );
		myGump.AddText( x + 25, y, hueList[i], "0x" + hueHex );
	}

	myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 0, backPage, 0 );
	myGump.AddHTMLGump( 45, 342, 80, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" );  // BACK

	myGump.AddButton( 270, 340, 0xFA5, 0xFA7, 1, 1, 5007 );
	myGump.AddHTMLGump( 305, 342, 60, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40045, socket.language ) + "</basefont>" ); // MAIN
}

/** @param {Socket} pSock @param {number} pButton @param {GumpData} gumpData */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ))
		return;

	var vendor = GetStoredVendorForGump( pUser );
	if( !IsPlayerVendor( vendor ))
		return;

	if( !IsVendorOwner( vendor, pUser ))
		return;

	if( !CheckVendorUpkeep( vendor, pUser ))
		return;

	if( pButton == 5000 || pButton == 5001 || pButton == 5002 || pButton == 5004 || pButton == 5006 || pButton == 5007 ||
		( pButton >= 0x1000 && pButton < ( 0x1000 + 256 )) ||
		( pButton >= 0x2000 && pButton < ( 0x2000 + 256 )) ||
		( pButton >= 0x3000 && pButton < 0x4000 ) ||
		( pButton >= 0x4000 && pButton < 0x5000 ))
	{
		HandleVendorCustomizeButton( pUser, vendor, pButton );
		return;
	}

	if( pButton == 0x8FFF || pButton == 0x8FFE ||
		( pButton >= 0x5000 && pButton < ( 0x5000 + 10000 )) ||
		( pButton >= 0x7000 && pButton < ( 0x7000 + 100 )) ||
		( pButton >= 0x8000 && pButton < ( 0x8000 + vendorClothingCategories.length )) ||
		( pButton >= 0x9000 && pButton < ( 0x9000 + 0x1000 )))
	{
		HandleVendorClothingButton( pUser, vendor, pButton );
		return;
	}

	switch( pButton )
	{
		case 0:
			break;
		case 1:
			OpenVendorBackpack( pUser, vendor );
			break;
		case 2:
			ShowVendorCustomizeGump( pUser, vendor );
			break;
		case 3:
			CollectVendorGold( pUser, vendor );
			ShowPlayerVendorOwnerGump( pSock, vendor );
			break;
		case 4:
			DepositToVendorBank( pUser, vendor, 1000 );
			ShowPlayerVendorOwnerGump( pSock, vendor );
			break;
		case 5:
			FlushBankToHeld( pUser, vendor );
			ShowPlayerVendorOwnerGump( pSock, vendor );
			break;
		case 6:
			DismissVendor( pUser, vendor );
			break;
		case 7:
			ShowVendorClothingGump( pUser, vendor );
			break;
		default:
			break;
	}
}

/** @param {Character} vendor @returns {boolean} */
function IsFemaleVendor( vendor )
{
	return vendor.id == 0x0191 || vendor.id == 0x025E;
}

/** @param {Character} vendor @returns {boolean} */
function IsElfVendor( vendor )
{
	return vendor.id == 0x025D || vendor.id == 0x025E;
}

/** @param {Character} vendor @param {boolean} isElf @param {boolean} isFemale */
function SetVendorRaceGenderBody( vendor, isElf, isFemale )
{
	if( isElf )
		vendor.id = ( isFemale ? 0x025E : 0x025D );
	else
		vendor.id = ( isFemale ? 0x0191 : 0x0190 );

	if( isFemale )
		RemoveVendorLayerItem( vendor, layerBeard );

	vendor.Teleport();
}

/** @param {Character} vendor @returns {HairOrBeard[]} */
function GetVendorHairStyles( vendor )
{
	if( IsElfVendor( vendor ))
	{
		if( IsFemaleVendor( vendor ))
			return femaleElfHairStyles;

		return maleElfHairStyles;
	}

	return humanHairStyles;
}

/** @param {Character} vendor @returns {boolean} */
function VendorCanHaveBeard( vendor )
{
	return !IsElfVendor( vendor ) && !IsFemaleVendor( vendor );
}

/** @param {Character} pUser @param {Character} vendor */
function ShowVendorCustomizeGump( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	if( !IsVendorOwner( vendor, pUser ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	StoreVendorForGump( pUser, vendor );

	var isElf = IsElfVendor( vendor );
	var isFemale = IsFemaleVendor( vendor );
	var hairStyles = GetVendorHairStyles( vendor );

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 370, 370, 0x13BE );
	myGump.AddCheckerTrans( 10, 10, 350, 350 );
	myGump.AddHTMLGump( 10, 12, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40046, socket.language ) + "</CENTER></basefont>" ); // VENDOR CUSTOMIZATION MENU

	myGump.AddPage( 1 );
	myGump.AddHTMLGump( 10, 42, 150, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40047, socket.language ) + "</CENTER></basefont>" ); // HAIR

	var i;
	for( i = 0; i < hairStyles.length; ++i )
	{
		myGump.AddButton( 10, 70 + ( i * 20 ), 0xFA5, 0xFA7, 1, 1, 0x1000 + i );
		myGump.AddHTMLGump( 45, 72 + ( i * 20 ), 110, 18, false, false, "<basefont color=#ffffff>" + GetHairOrBeardName( hairStyles[i], socket ) + "</basefont>" );
	}

	myGump.AddButton( 10, 70 + ( hairStyles.length * 20 ), 0xFB1, 0xFB3, 1, 1, 5002 );
	myGump.AddHTMLGump( 45, 72 + ( hairStyles.length * 20 ), 110, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40071, socket.language ) + "</basefont>" ); // Remove Hair

	myGump.AddButton( 10, 70 + (( hairStyles.length + 1 ) * 20 ), 0xFA5, 0xFA7, 0, 2, 0 );
	myGump.AddHTMLGump( 45, 72 + (( hairStyles.length + 1 ) * 20 ), 110, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40072, socket.language ) + "</basefont>" ); // Color Hair

	if( VendorCanHaveBeard( vendor ))
	{
		myGump.AddHTMLGump( 160, 42, 190, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40073, socket.language ) + "</CENTER></basefont>" ); // BEARD

		for( i = 0; i < humanBeardStyles.length; ++i )
		{
			myGump.AddButton( 160, 70 + ( i * 20 ), 0xFA5, 0xFA7, 1, 1, 0x2000 + i );
			myGump.AddHTMLGump( 195, 72 + ( i * 20 ), 160, 18, false, false, "<basefont color=#ffffff>" + GetHairOrBeardName( hairStyles[i], socket ) + "</basefont>" );
		}

		myGump.AddButton( 160, 70 + ( humanBeardStyles.length * 20 ), 0xFB1, 0xFB3, 1, 1, 5004 );
		myGump.AddHTMLGump( 195, 72 + ( humanBeardStyles.length * 20 ), 160, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40074, socket.language ) + "</basefont>" ); // Remove Beard

		myGump.AddButton( 160, 70 + (( humanBeardStyles.length + 1 ) * 20 ), 0xFA5, 0xFA7, 0, 100, 0 );
		myGump.AddHTMLGump( 195, 72 + (( humanBeardStyles.length + 1 ) * 20 ), 160, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40075, socket.language ) + "</basefont>" ); // Color Beard
	}

	myGump.AddButton( 160, 290, 0xFA5, 0xFA7, 1, 1, 5001 );
	myGump.AddHTMLGump( 195, 292, 160, 18, false, false, isFemale ? "<basefont color=#ffffff>" + GetDictionaryEntry( 40076, socket.language ) + "</basefont>" : "<basefont color=#ffffff>" + GetDictionaryEntry( 40077, socket.language ) + "</basefont>" ); // Change to Male Change to Female

	myGump.AddButton( 160, 310, 0xFA5, 0xFA7, 1, 1, 5006 );
	myGump.AddHTMLGump( 195, 312, 160, 18, false, false, isElf ? "<basefont color=#ffffff>" + GetDictionaryEntry( 40078, socket.language ) + "</basefont>" : "<basefont color=#ffffff>" + GetDictionaryEntry( 40079, socket.language ) + "</basefont>" ); // Change to Human Change to Elf

	myGump.AddButton( 160, 330, 0xFA5, 0xFA7, 1, 0, 7 );
	myGump.AddHTMLGump( 195, 332, 160, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40080, socket.language ) + "</basefont>" ); // Change Clothing

	myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 1, 1, 5000 );
	myGump.AddHTMLGump( 45, 342, 305, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40081, socket.language ) + "</basefont>" ); // CLOSE

	AddHairHueCategoryMenu( socket, myGump, 2, 10, GetDictionaryEntry( 40082, socket.language )); // HAIR COLOR
	AddHairHueCategoryMenu( socket, myGump, 100, 110, GetDictionaryEntry( 40083, socket.language )); // BEARD COLOR

	for( i = 0; i < vendorHairHueCategories.length; ++i )
	{
		var hairTitle = GetDictionaryEntry( 40084, socket.language ); // Hair - %s
		hairTitle = hairTitle.replace( /%s/gi, GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) );

		var beardTitle = GetDictionaryEntry( 40085, socket.language ); // Beard - %s
		beardTitle = beardTitle.replace( /%s/gi, GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) );

		AddHairHueCategoryPage( socket, myGump, 10 + i, hairTitle, vendorHairHueCategories[i].hues, 0x3000, 2 );
		AddHairHueCategoryPage( socket, myGump, 110 + i, beardTitle, vendorHairHueCategories[i].hues, 0x4000, 100 );
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @param {Character} pUser @param {Character} vendor @param {number} pButton */
function HandleVendorCustomizeButton( pUser, vendor, pButton )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	if( !IsVendorOwner( vendor, pUser ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	var isElf = IsElfVendor( vendor );
	var isFemale = IsFemaleVendor( vendor );
	var hairStyles = GetVendorHairStyles( vendor );

	if( pButton == 5000 )
	{
		ShowPlayerVendorOwnerGump( socket, vendor );
		return;
	}

	if( pButton == 5007 )
	{
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton == 5001 )
	{
		RemoveVendorLayerItem( vendor, layerBeard );
		SetVendorRaceGenderBody( vendor, isElf, !isFemale );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton == 5006 )
	{
		RemoveVendorLayerItem( vendor, layerHair );
		RemoveVendorLayerItem( vendor, layerBeard );

		SetVendorRaceGenderBody( vendor, !isElf, isFemale );

		var newHairStyles = GetVendorHairStyles( vendor );
		if( newHairStyles.length > 0 )
			SetVendorHairStyleByLayer( vendor, newHairStyles[0].itemID );

		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton == 5002 )
	{
		RemoveVendorLayerItem( vendor, layerHair );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton == 5004 )
	{
		RemoveVendorLayerItem( vendor, layerBeard );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton >= 0x1000 && pButton < ( 0x1000 + 256 ) )
	{
		var hairIndex = pButton - 0x1000;
		if( hairIndex >= 0 && hairIndex < hairStyles.length )
			SetVendorHairStyleByLayer( vendor, hairStyles[hairIndex].itemID );

		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton >= 0x2000 && pButton < ( 0x2000 + 256 ) )
	{
		var beardIndex = pButton - 0x2000;
		if( VendorCanHaveBeard( vendor ) && beardIndex >= 0 && beardIndex < humanBeardStyles.length )
			SetVendorBeardStyleByLayer( vendor, humanBeardStyles[beardIndex].itemID );

		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton >= 0x3000 && pButton < 0x4000 )
	{
		SetVendorHairHueByLayer( vendor, pButton - 0x3000 );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton >= 0x4000 && pButton < 0x5000 )
	{
		if( VendorCanHaveBeard( vendor ) )
			SetVendorBeardHueByLayer( vendor, pButton - 0x4000 );

		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}
}

/** @param {Character} vendor @param {number} layer */
function RemoveVendorLayerItem( vendor, layer )
{
	if( !ValidateObject( vendor ))
		return;

	var layerItem = vendor.FindItemLayer( layer );
	if( ValidateObject( layerItem ))
		layerItem.Delete();
}

/** @param {Character} vendor @param {number} itemID @param {number} layer @param {number} hue @returns {boolean} */
function EquipVendorLayerItem( vendor, itemID, layer, hue )
{
	if( !ValidateObject( vendor ))
		return false;

	var socket = null;
	if( vendor.socket )
		socket = vendor.socket;

	var itemName = "vendor style item";
	var newItem = CreateBlankItem( socket, vendor, 1, itemName, itemID, hue, "ITEM", false );
	if( !ValidateObject( newItem ))
		return false;

	newItem.container = vendor;
	newItem.layer = layer;
	return true;
}

/** @param {Character} vendor @param {number} hairItemID */
function SetVendorHairStyleByLayer( vendor, hairItemID )
{
	if( !ValidateObject( vendor ))
		return;

	var oldHair = vendor.FindItemLayer( layerHair );
	var oldHue = 0;
	if( ValidateObject( oldHair ))
		oldHue = oldHair.colour;

	RemoveVendorLayerItem( vendor, layerHair );

	if( hairItemID != 0 )
		EquipVendorLayerItem( vendor, hairItemID, layerHair, oldHue );
}

/** @param {Character} vendor @param {number} beardItemID */
function SetVendorBeardStyleByLayer( vendor, beardItemID )
{
	if( !ValidateObject( vendor ))
		return;

	var oldBeard = vendor.FindItemLayer( layerBeard );
	var oldHue = 0;
	if( ValidateObject( oldBeard ))
		oldHue = oldBeard.colour;

	RemoveVendorLayerItem( vendor, layerBeard );

	if( beardItemID != 0 )
		EquipVendorLayerItem( vendor, beardItemID, layerBeard, oldHue );
}

/** @param {Character} vendor @param {number} hue */
function SetVendorHairHueByLayer( vendor, hue )
{
	if( !ValidateObject( vendor ))
		return;

	var hairItem = vendor.FindItemLayer( layerHair );
	if( ValidateObject( hairItem ))
		hairItem.colour = hue;
}

/** @param {Character} vendor @param {number} hue */
function SetVendorBeardHueByLayer( vendor, hue )
{
	if( !ValidateObject( vendor ))
		return;

	var beardItem = vendor.FindItemLayer( layerBeard );
	if( ValidateObject( beardItem ))
		beardItem.colour = hue;
}

/** @param {Character} vendor @param {number} layer */
function RemoveVendorEquippedLayer( vendor, layer )
{
	if( !ValidateObject( vendor ))
		return;

	var oldItem = vendor.FindItemLayer( layer );
	if( ValidateObject( oldItem ))
		oldItem.Delete();
}

/** @param {Character} vendor @param {number} itemID @param {number} layer @param {number} hue @returns {boolean} */
function EquipVendorItemOnLayer( vendor, itemID, layer, hue )
{
	if( !ValidateObject( vendor ))
		return false;

	var socket = vendor.socket ? vendor.socket : null;
	var newItem = CreateBlankItem( socket, vendor, 1, "vendor clothing", itemID, hue, "ITEM", false );
	if( !ValidateObject( newItem ))
		return false;

	newItem.container = vendor;
	newItem.layer = layer;
	return true;
}

/** @param {Character} vendor @param {number} layer @param {number} itemID */
function SetVendorClothingByLayer( vendor, layer, itemID )
{
	if( !ValidateObject( vendor ))
		return;

	var oldItem = vendor.FindItemLayer( layer );
	var oldHue = 0;

	if( ValidateObject( oldItem ))
		oldHue = oldItem.colour;

	RemoveVendorEquippedLayer( vendor, layer );

	if( itemID != 0 )
		EquipVendorItemOnLayer( vendor, itemID, layer, oldHue );
}

/** @param {Character} vendor @param {number} layer @param {number} hue */
function SetVendorClothingHueByLayer( vendor, layer, hue )
{
	if( !ValidateObject( vendor ))
		return;

	var layerItem = vendor.FindItemLayer( layer );
	if( ValidateObject( layerItem ))
		layerItem.colour = hue;
}

/** @param {Character} pUser @param {number} categoryIndex */
function StoreClothingDyeCategory( pUser, categoryIndex )
{
	if( !ValidateObject( pUser ))
		return;

	pUser.SetTempTag( "PlayerVendorClothingDyeCategory", categoryIndex );
}

/** @param {Character} pUser @returns {number} */
function GetStoredClothingDyeCategory( pUser )
{
	if( !ValidateObject( pUser ))
		return -1;

	var categoryIndex = pUser.GetTempTag( "PlayerVendorClothingDyeCategory" );
	if( categoryIndex === null || categoryIndex === undefined || categoryIndex === "" )
		return -1;

	return categoryIndex;
}

/** @param {Character} pUser @param {Character} vendor */
function ShowVendorClothingGump( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	if( !IsVendorOwner( vendor, pUser ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	StoreVendorForGump( pUser, vendor );

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 585, 393, 5054 );
	myGump.AddBackground( 195, 36, 387, 275, 3000 );
	myGump.AddHTMLGump( 10, 10, 565, 18, false, false, "<CENTER>" + GetDictionaryEntry( 40086, socket.language ) + "</CENTER>" ); // VENDOR CLOTHING MENU

	myGump.AddHTMLGump( 60, 355, 150, 18, false, false, GetDictionaryEntry( 18718, socket.language ) ); // OKAY
	myGump.AddButton( 25, 355, 4005, 4007, 1, 0, 5000 );

	myGump.AddHTMLGump( 320, 355, 150, 18, false, false, GetDictionaryEntry( 10267, socket.language )); // BACK
	myGump.AddButton( 285, 355, 4005, 4007, 1, 0, 0x8FFF );

	var i;
	var y = 35;

	for( i = 0; i < vendorClothingCategories.length; ++i )
	{
		myGump.AddHTMLGump( 5, y, 150, 25, false, false, vendorClothingCategories[i].name );
		myGump.AddButton( 155, y, 4005, 4007, 0, 20 + i, 0 );
		y += 25;
	}

	for( i = 0; i < vendorClothingCategories.length; ++i )
	{
		var cat = vendorClothingCategories[i];
		myGump.AddPage( 20 + i );

		var entryIndex;
		for( entryIndex = 0; entryIndex < cat.entries.length; ++entryIndex )
		{
			var entry = cat.entries[entryIndex];
			var cols = 3;
			var startX = 198;
			var startY = 38;
			var colWidth = 129;
			var rowHeight = 67;
			var textWidth = 100;
			var itemOffsetX = 20;
			var itemOffsetY = 25;
			var buttonOffsetY = 45;

			if( cat.name == "Held Items" )
			{
				cols = 4;
				startX = 205;
				startY = 42;
				colWidth = 92;
				rowHeight = 52;
				textWidth = 80;
				itemOffsetX = 18;
				itemOffsetY = 16;
				buttonOffsetY = 30;
			}

			var x = startX + ( entryIndex % cols ) * colWidth;
			var ey = startY + Math.floor( entryIndex / cols ) * rowHeight;

			myGump.AddHTMLGump( x, ey, textWidth, 18, false, false, entry.name );
			myGump.AddPicture( x + itemOffsetX, ey + itemOffsetY, entry.itemID );
			myGump.AddButton( x, ey + buttonOffsetY, 4005, 4007, 1, 20 + i, 0x5000 + ( i * 100 ) + entryIndex );
		}

		if( cat.canDye )
		{
			myGump.AddButton( 300, 315, 4005, 4007, 1, 0, 0x8000 + i );
			myGump.AddHTMLGump( 335, 317, 100, 18, false, false, GetDictionaryEntry( 40087, socket.language )); // Color
		}

		myGump.AddButton( 430, 315, 4005, 4007, 1, 20 + i, 0x7000 + i );
		myGump.AddHTMLGump( 465, 317, 100, 18, false, false, GetDictionaryEntry( 40088, socket.language )); // Remove
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @param {Character} pUser @param {Character} vendor */
function ShowVendorClothingDyeGump( pUser, vendor )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	if( !IsVendorOwner( vendor, pUser ))
		return;

	var socket = pUser.socket;
	if( !socket )
		return;

	StoreVendorForGump( pUser, vendor );

	var categoryIndex = GetStoredClothingDyeCategory( pUser );
	var title = GetDictionaryEntry( 40091, socket.language );

	if( categoryIndex >= 0 && categoryIndex < vendorClothingCategories.length )
		title = vendorClothingCategories[categoryIndex].name + " COLOR";

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 370, 370, 0x13BE );
	myGump.AddCheckerTrans( 10, 10, 350, 350 );
	myGump.AddHTMLGump( 10, 12, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + title + "</CENTER></basefont>" );

	myGump.AddPage( 1 );
	myGump.AddHTMLGump( 10, 42, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40089, socket.language ) + "</CENTER></basefont>" ); // COLOR CATEGORY

	var columns = 2;
	var startX = 20;
	var startY = 70;
	var colWidth = 160;
	var rowHeight = 20;

	var i;
	for( i = 0; i < vendorHairHueCategories.length; ++i )
	{
		var row = Math.floor( i / columns );
		var col = i % columns;
		var x = startX + ( col * colWidth );
		var y = startY + ( row * rowHeight );

		myGump.AddButton( x, y, 0x0A9A, 0x0A9A, 0, 10 + i, 0 );
		myGump.AddHTMLGump( x + 30, y, 125, 18, false, false, "<basefont color=#ffffff>" + GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) + "</basefont>" );
	}

	myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 1, 0, 0x8FFE );
	myGump.AddHTMLGump( 45, 342, 120, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" ); // BACK

	for( i = 0; i < vendorHairHueCategories.length; ++i )
	{
		myGump.AddPage( 10 + i );
		myGump.AddHTMLGump( 10, 42, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) + "</CENTER></basefont>" );

		var hues = vendorHairHueCategories[i].hues;
		var hueColumns = 3;
		var hueStartX = 20;
		var hueStartY = 70;
		var hueColWidth = 112;
		var hueRowHeight = 18;

		var j;
		for( j = 0; j < hues.length; ++j )
		{
			var hueRow = Math.floor( j / hueColumns );
			var hueCol = j % hueColumns;
			var hx = hueStartX + ( hueCol * hueColWidth );
			var hy = hueStartY + ( hueRow * hueRowHeight );
			var hueHex = hues[j].toString( 16 ).toUpperCase();

			myGump.AddButton( hx, hy, 0x0A9A, 0x0A9A, 1, 10 + i, 0x9000 + hues[j] );
			myGump.AddText( hx + 25, hy, hues[j], "0x" + hueHex );
		}

		myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 0, 1, 0 );
		myGump.AddHTMLGump( 45, 342, 80, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" ); // BACK

		myGump.AddButton( 270, 340, 0xFA5, 0xFA7, 1, 0, 0x8FFE );
		myGump.AddHTMLGump( 305, 342, 60, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40090, socket.language ) + "</basefont>" ); // DONE
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @param {Character} pUser @param {Character} vendor @param {number} pButton */
function HandleVendorClothingButton( pUser, vendor, pButton )
{
	if( !ValidateObject( pUser ) || !IsPlayerVendor( vendor ))
		return;

	if( !IsVendorOwner( vendor, pUser ))
		return;

	if( pButton == 0x8FFF )
	{
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	if( pButton == 0x8FFE )
	{
		ShowVendorClothingGump( pUser, vendor );
		return;
	}

	if( pButton >= 0x7000 && pButton < ( 0x7000 + 100 ) )
	{
		var removeCatIndex = pButton - 0x7000;
		if( removeCatIndex >= 0 && removeCatIndex < vendorClothingCategories.length )
		{
			RemoveVendorEquippedLayer( vendor, vendorClothingCategories[removeCatIndex].layer );
			ShowVendorClothingGump( pUser, vendor );
		}
		return;
	}

	if( pButton >= 0x5000 && pButton < ( 0x5000 + 10000 ) )
	{
		var entryVal = pButton - 0x5000;
		var catIndex = Math.floor( entryVal / 100 );
		var entryIndex = entryVal % 100;

		if( catIndex >= 0 && catIndex < vendorClothingCategories.length )
		{
			var cat = vendorClothingCategories[catIndex];
			if( entryIndex >= 0 && entryIndex < cat.entries.length )
			{
				SetVendorClothingByLayer( vendor, cat.layer, cat.entries[entryIndex].itemID );
				ShowVendorClothingGump( pUser, vendor );
			}
		}
		return;
	}

	if( pButton >= 0x8000 && pButton < ( 0x8000 + vendorClothingCategories.length ) )
	{
		var dyeCatIndex = pButton - 0x8000;
		if( dyeCatIndex >= 0 && dyeCatIndex < vendorClothingCategories.length )
		{
			StoreClothingDyeCategory( pUser, dyeCatIndex );
			ShowVendorClothingDyeGump( pUser, vendor );
		}
		return;
	}

	if( pButton >= 0x9000 && pButton < ( 0x9000 + 0x1000 ) )
	{
		var hueValue = pButton - 0x9000;
		var storedCatIndex = GetStoredClothingDyeCategory( pUser );

		if( storedCatIndex >= 0 && storedCatIndex < vendorClothingCategories.length )
		{
			var dyeCat = vendorClothingCategories[storedCatIndex];
			if( dyeCat.canDye )
				SetVendorClothingHueByLayer( vendor, dyeCat.layer, hueValue );
		}

		ShowVendorClothingDyeGump( pUser, vendor );
		return;
	}
}