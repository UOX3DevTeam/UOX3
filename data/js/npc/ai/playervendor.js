/// <reference path="../../definitions.d.ts" />
// @ts-check
// Version: 1.1.7

const VendorSettings = {
	// Max amount of money the vendor can hold in the bank.
	// Held gold is not included in this limit, but will
	// be used first to pay upkeep before bank funds are used.
    get MaxFunds() { return GetServerSetting( "VendorMaxFunds" ); },
	// If true, returned vendor belongings skip player backpack entirely.
	// Items, gold and vendor deed will try bank box first, then drop at vendor location.
	// If false, belongings try backpack first, then bank box, then ground.
    get OnlyReturnToBank() { return GetServerSetting( "onlyReturnToBank" ); },
    get ChargesEnabled() { return GetServerSetting( "VendorChargesEnabled" ); },// Master toggle for vendor upkeep charges
    get BaseCharge() { return GetServerSetting( "VendorBaseCharge" ); },// Flat fee per charge period
    get ChargeHours() { return GetServerSetting( "VendorChargeHours" ); },// Charge every X real hours
    get ItemFeesEnabled() { return GetServerSetting( "VendorUseItemFeesEnabled" ); },// Add item-based fee from listed item prices
    get ItemFeeDivisor() { return GetServerSetting( "VendorItemFeeDivisor" ); },// 3 gold per 500 worth of one item
    get ItemFeeAmount() { return GetServerSetting( "VendorItemFeeAmount" ); }    // Fee added per divisor step
}

const VendorEquipmentLayer = {
    OneHand: 0x01,
    TwoHand: 0x02,
    Shoes: 0x03,
    Pants: 0x04,
    Shirt: 0x05,
    Helm: 0x06,
    Gloves: 0x07,
    Ring: 0x08,
    Neck: 0x0A,
    Hair: 0x0B,
    Waist: 0x0C,
    InnerTorso: 0x0D,
    Bracelet: 0x0E,
    Beard: 0x10,
    MiddleTorso: 0x11,
    Earrings: 0x12,
    Arms: 0x13,
    Cloak: 0x14,
    OuterTorso: 0x16,
    OuterLegs: 0x17
};

const vendorHairHueCategories = [
	{ name: null, hues: [ 0x044E, 0x044F, 0x0450, 0x0451, 0x0452, 0x0453, 0x0454 ], dictID: 17111 }, // Black
	{ name: null, hues: [ 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045A, 0x045B, 0x045C ], dictID: 17112 }, // Beige
	{ name: null, hues: [ 0x045D, 0x045E, 0x045F, 0x0460, 0x0461, 0x0462, 0x0463, 0x0464, 0x0465, 0x0466, 0x0467, 0x0468, 0x0469, 0x046A, 0x046B, 0x046C ], dictID: 17113 }, // Golden
	{ name: null, hues: [ 0x046D, 0x046E, 0x046F, 0x0470, 0x0471, 0x0472, 0x0473, 0x0474, 0x0475, 0x0476, 0x0477, 0x0478, 0x0479, 0x047A, 0x047B, 0x047C ], dictID: 17114 }, // Dark Brown
	{ name: null, hues: [ 0x04B1, 0x04B2, 0x04B3, 0x04B4, 0x04B5, 0x04B6, 0x04B7, 0x04B8, 0x04B9, 0x04BA, 0x04BB, 0x04BC, 0x04BD, 0x04BE, 0x04BF, 0x04C0 ], dictID: 17108 }, // Red
	{ name: null, hues: [ 0x05DD, 0x05DE, 0x05DF, 0x05E0, 0x05E1, 0x05E2, 0x05E3, 0x05E4, 0x05E5, 0x05E6, 0x05E7, 0x05E8, 0x05E9, 0x05EA, 0x05EB, 0x05EC ], dictID: 17105 }, // Auburn
	{ name: null, hues: [ 0x0641, 0x0642, 0x0643, 0x0644, 0x0645, 0x0646, 0x0647, 0x0648, 0x0649, 0x064A, 0x064B, 0x064C, 0x064D, 0x064E, 0x064F, 0x0650, 0x0651, 0x0652, 0x0653, 0x0654, 0x0655, 0x0656, 0x0657, 0x0658, 0x0659, 0x065A, 0x065B, 0x065C, 0x065D, 0x065E, 0x065F, 0x0660 ], dictID: 17103 }, // Brown
	{ name: null, hues: [ 0x08A5, 0x08A6, 0x08A7, 0x08A8, 0x08A9, 0x0961, 0x0962, 0x0963, 0x0964, 0x0965, 0x0966, 0x0967, 0x0968 ], dictID: 17110 }, // Blonde
	{ name: null, hues: [ 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 ], dictID: 40100 }, // Special Purple
	{ name: null, hues: [ 32, 33, 34, 35, 36, 37 ], dictID: 40101 }, // Special Red
	{ name: null, hues: [ 38, 39, 40, 41, 42, 43, 44, 45, 46 ], dictID: 40102 }, // Special Orange
	{ name: null, hues: [ 54, 55, 56, 57 ], dictID: 40103 }, // Special Yellow
	{ name: null, hues: [ 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72 ], dictID: 40104 }, // Special Lime
	{ name: null, hues: [ 81, 82, 83 ], dictID: 40105 }, // Special Dark Lime
	{ name: null, hues: [ 89, 90, 91 ], dictID: 40106 }, // Special Ice Blue
	{ name: null, hues: [ 1153, 1154, 1155 ], dictID: 40107 } // Special Ice White
];

const femaleElfHairStyles = [
	{ itemID: 0x2FCC, name: null, dictID: 18035 }, // Flower
	{ itemID: 0x2FC0, name: null, dictID: 18028 }, // Long Feather
	{ itemID: 0x2FC1, name: null, dictID: 18029 }, // Short
	{ itemID: 0x2FC2, name: null, dictID: 18030 }, // Mullet
	{ itemID: 0x2FCE, name: null, dictID: 18032 }, // Topknot
	{ itemID: 0x2FCF, name: null, dictID: 18033 }, // Long Braid
	{ itemID: 0x2FD0, name: null, dictID: 18036 }, // Buns
	{ itemID: 0x2FD1, name: null, dictID: 18034 }  // Spiked
];

const maleElfHairStyles = [
	{ itemID: 0x2FBF, name: null, dictID: 18027 }, // Mid Long
	{ itemID: 0x2FC0, name: null, dictID: 18028 }, // Long Feather
	{ itemID: 0x2FC1, name: null, dictID: 18029 }, // Short
	{ itemID: 0x2FC2, name: null, dictID: 18030 }, // Mullet
	{ itemID: 0x2FCE, name: null, dictID: 18032 }, // Topknot
	{ itemID: 0x2FCF, name: null, dictID: 18033 }, // Long Braid
	{ itemID: 0x2FCD, name: null, dictID: 18031 }, // Long
	{ itemID: 0x2FD1, name: null, dictID: 18034 }  // Spiked
];

const humanHairStyles = [
	{ itemID: 0x203B, name: null, dictID: 18029 }, // Short
	{ itemID: 0x203C, name: null, dictID: 18031 }, // Long
	{ itemID: 0x203D, name: null, dictID: 2086 }, // Ponytail
	{ itemID: 0x2044, name: null, dictID: 2087 }, // Mohawk
	{ itemID: 0x2045, name: null, dictID: 2080 }, // Pageboy
	{ itemID: 0x204A, name: null, dictID: 18032 }, // Topknot
	{ itemID: 0x2047, name: null, dictID: 2088 }, // Curly
	{ itemID: 0x2048, name: null, dictID: 2081 }, // Receding
	{ itemID: 0x2049, name: null, dictID: 2082 }  // 2-Tails
];

const humanBeardStyles = [
	{ itemID: 0x2041, name: null, dictID: 18001 }, // Mustache
	{ itemID: 0x203F, name: null, dictID: 18002 }, // Short Beard
	{ itemID: 0x204B, name: null, dictID: 40058 }, // Short Beard and Mustache
	{ itemID: 0x203E, name: null, dictID: 18005 }, // Long Beard
	{ itemID: 0x204C, name: null, dictID: 40068 }, // Long Beard and Mustache
	{ itemID: 0x2040, name: null, dictID: 40069 }, // Goatee
	{ itemID: 0x204D, name: null, dictID: 18000 }  // Vandyke
];

const vendorClothingCategories = [
	{ layer: VendorEquipmentLayer.InnerTorso, name: "Upper Torso", canDye: true, compactLayout: false, entries: [
		{ itemID: 0x1517, name: "Shirt" },
		{ itemID: 0x1EFD, name: "Fancy Shirt" },
		{ itemID: 0x1F01, name: "Plain Dress" },
		{ itemID: 0x1EFF, name: "Fancy Dress" },
		{ itemID: 0x1F03, name: "Robe" }
	]},

	{ layer: VendorEquipmentLayer.MiddleTorso, name: "Over Chest", canDye: true, compactLayout: false, entries: [
		{ itemID: 0x1F7B, name: "Doublet" },
		{ itemID: 0x1FA1, name: "Tunic" },
		{ itemID: 0x1F9F, name: "Jester Suit" },
		{ itemID: 0x1541, name: "Body Sash" },
		{ itemID: 0x1FFD, name: "Surcoat" },
		{ itemID: 0x153B, name: "Half Apron" },
		{ itemID: 0x153D, name: "Full Apron" }
	]},

	{ layer: VendorEquipmentLayer.Shoes, name: "Footwear", canDye: true, compactLayout: false, entries: [
		{ itemID: 0x170D, name: "Sandals" },
		{ itemID: 0x1710, name: "Shoes" },
		{ itemID: 0x170B, name: "Boots" },
		{ itemID: 0x1711, name: "Thigh Boots" }
	]},

	{ layer: VendorEquipmentLayer.Helm, name: "Hats", canDye: true, compactLayout: false, entries: [
		{ itemID: 0x1544, name: "Skull Cap" },
		{ itemID: 0x1540, name: "Bandana" },
		{ itemID: 0x1713, name: "Floppy Hat" },
		{ itemID: 0x1714, name: "Wide Brim Hat" },
		{ itemID: 0x1715, name: "Cap" },
		{ itemID: 0x1716, name: "Tall Straw Hat" },
		{ itemID: 0x1717, name: "Straw Hat" },
		{ itemID: 0x1718, name: "Wizard Hat" },
		{ itemID: 0x1719, name: "Bonnet" },
		{ itemID: 0x171A, name: "Feathered Hat" },
		{ itemID: 0x171B, name: "Tricorne Hat" },
		{ itemID: 0x171C, name: "Jester Hat" }
	]},

	{ layer: VendorEquipmentLayer.Pants, name: "Lower Torso", canDye: true, compactLayout: false, entries: [
		{ itemID: 0x1539, name: "Long Pants" },
		{ itemID: 0x1537, name: "Kilt" },
		{ itemID: 0x1516, name: "Skirt" }
	]},

	{ layer: VendorEquipmentLayer.Cloak, name: "Back", canDye: true, compactLayout: false, entries: [
		{ itemID: 0x1515, name: "Cloak" }
	]},

	{ layer: VendorEquipmentLayer.OneHand, name: "Held Items", canDye: false, compactLayout: true, entries: [
		{ itemID: 0x0DBF, name: "Fishing Pole" },
		{ itemID: 0x0E86, name: "Pickaxe" },
		{ itemID: 0x0E87, name: "Pitchfork" },
		{ itemID: 0x0EC3, name: "Cleaver" },
		{ itemID: 0x0F5C, name: "Mace" },
		{ itemID: 0x0F6B, name: "Torch" },
		{ itemID: 0x102A, name: "Hammer" },
		{ itemID: 0x0F61, name: "Longsword" },
		{ itemID: 0x13F8, name: "Gnarled Staff" },
		{ itemID: 0x0F4F, name: "Crossbow" },
		{ itemID: 0x1407, name: "War Mace" },
		{ itemID: 0x1443, name: "Two-Handed Axe" },
		{ itemID: 0x0F62, name: "Spear" },
		{ itemID: 0x13FF, name: "Katana" },
		{ itemID: 0x0EFA, name: "Spellbook" }
	]}
];

/** @param {VendorHueCategory} entry @param {Socket|null} socket @returns {string} */
function GetVendorHueCategoryName( entry, socket )
{
	if( entry.dictID && socket )
		return GetDictionaryEntry( entry.dictID, socket.language );

	return entry.name || "";
}

/** @param {HairOrBeard} entry @param {Socket} socket @returns {string} */
function GetHairOrBeardName( entry, socket )
{
	if( entry.dictID && socket )
		return GetDictionaryEntry( entry.dictID, socket.language );

	return entry.name || "";
}

/** @param {Item} itemObj @returns {number} */
function GetVendorItemPeriodFee( itemObj )
{
	if( itemObj.buyValue <= 0 || !VendorSettings.ItemFeesEnabled || VendorSettings.ItemFeeDivisor <= 0 || VendorSettings.ItemFeeAmount <= 0 )
		return 0;

	return Math.floor( itemObj.buyValue / VendorSettings.ItemFeeDivisor ) * VendorSettings.ItemFeeAmount;
}

/** @type { ( vendor: Character ) => number } */
function GetVendorItemFeeTotal( vendor )
{
	if( vendor.aitype != 17 || !ValidateObject( vendor.pack ) )
		return 0;

	var totalFee = 0;
	var packItem = vendor.pack.FirstItem();
	for( ; !vendor.pack.FinishedItems(); packItem = vendor.pack.NextItem() )
	{
		if( ValidateObject( packItem ))
			totalFee += GetVendorItemPeriodFee( packItem );
	}

	return totalFee;
}

/** @type { ( vendor: Character ) => number } */
function GetVendorChargePerPeriod( vendor )
{
	if(  vendor.aitype != 17 || !VendorSettings.ChargesEnabled )
		return 0;

	var overrideValue = vendor.GetTag( "VendorChargePerPeriod" );
	if( overrideValue > 0 )
		return overrideValue;

	return VendorSettings.BaseCharge + ( VendorSettings.ItemFeesEnabled ? GetVendorItemFeeTotal( vendor ) : 0 );
}

/** @param {Character} vendor @param {number} amount @returns {number} */
function DepositVendorBankGold( vendor, amount )
{
	if( vendor.aitype != 17 || amount <= 0 )
		return 0;

	var bankGold = vendor.GetTag( "VendorBankAccount" );
	if( bankGold < 0 )
		bankGold = 0;

	var roomLeft = VendorSettings.MaxFunds - bankGold;
	if( roomLeft <= 0 )
		return 0;

	if( amount > roomLeft )
		amount = roomLeft;

	vendor.SetTag( "VendorBankAccount", bankGold + amount );
	return amount;
}

/** @param {Character} vendor @param {number} amount @returns {number} */
function WithdrawVendorBankGold( vendor, amount )
{
	if( vendor.aitype != 17 || amount <= 0 )
		return 0;

	var bankGold = vendor.GetTag( "VendorBankAccount" );
	if( bankGold < 0 )
		bankGold = 0;

	if( amount > bankGold )
		amount = bankGold;

	vendor.SetTag( "VendorBankAccount", bankGold - amount );
	return amount;
}

/** @param {Character} vendor @param {number} charge @returns {boolean} */
function PayVendorCharge( vendor, charge )
{
	if( vendor.aitype != 17 || charge <= 0 )
		return false;

	var bankGold = vendor.GetTag( "VendorBankAccount" );
	var heldGold = vendor.vendorGoldHeld || 0;

	if( bankGold < 0 )
		bankGold = 0;

	if(( bankGold + heldGold ) < charge )
		return false;

	if( bankGold >= charge )
	{
		vendor.SetTag( "VendorBankAccount", bankGold - charge );
		return true;
	}

	charge -= bankGold;
	vendor.SetTag( "VendorBankAccount", 0 );
	vendor.vendorGoldHeld = heldGold - charge;
	return true;
}

/** @param {Character} vendor @returns {number} */
function FlushVendorBankToHeldGold( vendor )
{
	if( vendor.aitype != 17 )
		return 0;

	var bankGold = vendor.GetTag( "VendorBankAccount" );
	if( bankGold <= 0 )
		return 0;

	vendor.SetTag( "VendorBankAccount", 0 );
	vendor.vendorGoldHeld = ( vendor.vendorGoldHeld || 0 ) + bankGold;
	return bankGold;
}

/** @param {Character} vendor @returns {number} */
function GetVendorPeriodsAffordable( vendor )
{
	var perPeriod = GetVendorChargePerPeriod( vendor );
	if( perPeriod <= 0 )
		return 0;

	var bankGold = 0;
	var heldGold = 0;

	if( vendor.aitype == 17 )
	{
		bankGold = vendor.GetTag( "VendorBankAccount" );
		heldGold = vendor.vendorGoldHeld || 0;

		if( bankGold < 0 )
			bankGold = 0;
	}

	return Math.floor(( bankGold + heldGold ) / perPeriod );
}

/** @param {Character} vendor @returns {number} */
function GetVendorHoursAffordable( vendor )
{
	return GetVendorPeriodsAffordable( vendor ) * VendorSettings.ChargeHours;
}

/** @param {Character} vendor @returns {number} */
function GetVendorDaysAffordable( vendor )
{
	return Math.floor( GetVendorHoursAffordable( vendor ) / 24 );
}

/** @param {Item} itemObj @param {Character} vendor */
function DropItemAtVendorLocation( itemObj, vendor )
{
	itemObj.container = null;
	itemObj.x = vendor.x;
	itemObj.y = vendor.y;
	itemObj.z = vendor.z;
	itemObj.worldnumber = vendor.worldnumber;
	itemObj.instanceID = vendor.instanceID;
}

/** @param {Item} itemObj @param {Item} container @returns {boolean} */
function TryMoveItemToContainer( itemObj, container )
{
	if( !ValidateObject( container ) )
		return false;

	if( container.totalItemCount >= container.maxItems )
		return false;

	if(( container.weight + ( itemObj.weight * itemObj.amount ) ) > container.weightMax )
		return false;

	itemObj.container = container;
	return ValidateObject( itemObj.container ) && itemObj.container.serial == container.serial;
}

/** @param {Item} itemObj @param {Character} pUser @param {Character} vendor @returns {number} */
function PlaceItemForOwnerOrDrop( itemObj, pUser, vendor )
{
	var pack = pUser.pack;
	var bankBox = pUser.FindItemLayer( 29 );

	if( VendorSettings.OnlyReturnToBank )
	{
		if( TryMoveItemToContainer( itemObj, bankBox ) )
			return 2;

		if( TryMoveItemToContainer( itemObj, pack ) )
			return 1;
	}
	else
	{
		if( TryMoveItemToContainer( itemObj, pack ) )
			return 1;

		if( TryMoveItemToContainer( itemObj, bankBox ) )
			return 2;
	}

	DropItemAtVendorLocation( itemObj, vendor );
	return 3;
}

/** @param {Character} vendor @param {Character} pUser @returns {{pack:number, bank:number, ground:number}} */
function ReturnVendorItemsToOwner( vendor, pUser )
{
	var results = { pack:0, bank:0, ground:0 };

	if( vendor.aitype != 17 || !ValidateObject( vendor.pack ))
		return results;

	var itemList = [];
	var packItem = vendor.pack.FirstItem();
	for( ; !vendor.pack.FinishedItems(); packItem = vendor.pack.NextItem() )
	{
		if( ValidateObject( packItem ) )
			itemList.push( packItem );
	}

	for( var i = 0; i < itemList.length; ++i )
	{
		var placeResult = PlaceItemForOwnerOrDrop( itemList[i], pUser, vendor );
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
	if( vendor.aitype != 17 || !ValidateObject( vendor.pack ) )
		return 0;

	var itemList = [];
	var packItem = vendor.pack.FirstItem();
	for( ; !vendor.pack.FinishedItems(); packItem = vendor.pack.NextItem() )
	{
		if( ValidateObject( packItem ))
			itemList.push( packItem );
	}

	for( var i = 0; i < itemList.length; ++i )
	{
			DropItemAtVendorLocation( itemList[i], vendor );
	}

	return itemList.length;
}

/** @param {Character} vendor @returns {Item|null} */
function CreateVendorDismissDeedAtLocation( vendor )
{
	if( vendor.aitype != 17 )
		return null;

	var deed = CreateBlankItem( null, vendor, 1, "A vendor deed for " + vendor.name + ".", 0x14F0, 0x0, "ITEM", false );
	if( !ValidateObject( deed ) )
		return null;

	deed.type = 217;
	deed.buyValue = 2000;
	deed.SetTag( "vendorName", vendor.name );
	DropItemAtVendorLocation( deed, vendor );
	return deed;
}

/** @param {Character} pUser @param {Character} vendor @returns {Item|null} */
function CreateVendorDismissDeedForOwner( pUser, vendor )
{
	if( vendor.aitype != 17 )
		return null;

	var deedName = "A vendor deed for " + vendor.name + ".";
	if( pUser.socket )
	{
		deedName = GetDictionaryEntry( 580, pUser.socket.language );
		deedName = deedName.replace( /%s/gi, vendor.name );
	}

	var deed = CreateBlankItem( null, pUser, 1, deedName, 0x14F0, 0x0, "ITEM", false );
	if( !ValidateObject( deed ) )
		return null;

	deed.type = 217;
	deed.buyValue = 2000;
	deed.SetTag( "vendorName", vendor.name );
	return deed;
}

/** @param {Character} vendor @returns {number} */
function DropVendorGoldAtLocation( vendor )
{
	if( vendor.aitype != 17 )
		return 0;

	var heldGold = vendor.vendorGoldHeld || 0;
	var bankGold = vendor.GetTag( "VendorBankAccount" );
	if( bankGold < 0 )
		bankGold = 0;

	var totalGold = heldGold + bankGold;
	if( totalGold <= 0 )
		return 0;

	var goldItem = CreateDFNItem( null, vendor, "0x0EED", totalGold, "ITEM", false );
	if( !ValidateObject( goldItem ) )
		return 0;

	vendor.vendorGoldHeld = 0;
	vendor.SetTag( "VendorBankAccount", 0 );
	DropItemAtVendorLocation( goldItem, vendor );
	return totalGold;
}

/** @param {Character} pUser @param {Character} vendor @returns {{amount:number, location:number}} */
function ReturnVendorGoldToOwner( pUser, vendor )
{
	var result = { amount:0, location:0 };

	if( vendor.aitype != 17 )
		return result;

	var heldGold = vendor.vendorGoldHeld || 0;
	var bankGold = vendor.GetTag( "VendorBankAccount" );
	if( bankGold < 0 )
		bankGold = 0;

	var totalGold = heldGold + bankGold;
	if( totalGold <= 0 )
		return result;

	var goldItem = CreateDFNItem( null, pUser, "0x0EED", totalGold, "ITEM", false );
	if( !ValidateObject( goldItem ) )
		return result;

	var placeResult = PlaceItemForOwnerOrDrop( goldItem, pUser, vendor );
	if( placeResult == 0 )
		return result;

	vendor.vendorGoldHeld = 0;
	vendor.SetTag( "VendorBankAccount", 0 );

	result.amount = totalGold;
	result.location = placeResult;
	return result;
}

/** @param {Character} vendor */
function FinalizeVendorRemoval( vendor )
{
	if( vendor.aitype != 17 )
		return;

	if( ValidateObject( vendor.multi ) )
		vendor.multi.RemoveVendor( vendor );

	vendor.Delete();
}

/** @param {Character} vendor @param {Character|null} notifyChar */
function RemoveVendorForNoFunds( vendor, notifyChar )
{
	if( vendor.aitype != 17 )
		return;

	var ownerChar = ValidateObject( vendor.owner ) ? vendor.owner : null;
	if( ValidateObject( ownerChar ) && ownerChar.socket )
	{
		var ownerSocket = ownerChar.socket;
		ownerSocket.SysMessage( GetDictionaryEntry( 40000, ownerSocket.language ), vendor.name );

		var deed = CreateVendorDismissDeedForOwner( ownerChar, vendor );
		if( !ValidateObject( deed ))
		{
			ownerSocket.SysMessage( GetDictionaryEntry( 40001, ownerSocket.language ) );
			return;
		}

		var itemResults = ReturnVendorItemsToOwner( vendor, ownerChar );
		var goldResult = ReturnVendorGoldToOwner( ownerChar, vendor );
		var deedLocation = PlaceItemForOwnerOrDrop( deed, ownerChar, vendor );

		if(( itemResults.pack + itemResults.bank + itemResults.ground ) > 0 )
		{
			if( VendorSettings.OnlyReturnToBank )
				ownerSocket.SysMessage( GetDictionaryEntry( 40002, ownerSocket.language ), itemResults.bank, itemResults.ground );
			else
				ownerSocket.SysMessage( GetDictionaryEntry( 40003, ownerSocket.language ), itemResults.pack, itemResults.bank, itemResults.ground );
		}

		if( goldResult.amount > 0 )
		{
			if( goldResult.location == 1 )
				ownerSocket.SysMessage( GetDictionaryEntry( 40004, ownerSocket.language ), goldResult.amount );
			else if( goldResult.location == 2 )
				ownerSocket.SysMessage( GetDictionaryEntry( 40005, ownerSocket.language ), goldResult.amount );
			else if( goldResult.location == 3 )
				ownerSocket.SysMessage( GetDictionaryEntry( 40006, ownerSocket.language ), goldResult.amount );
		}

		if( deedLocation == 1 )
			ownerSocket.SysMessage( GetDictionaryEntry( 40007, ownerSocket.language ) );
		else if( deedLocation == 2 )
			ownerSocket.SysMessage( GetDictionaryEntry( 40008, ownerSocket.language ) );
		else if( deedLocation == 3 )
			ownerSocket.SysMessage( GetDictionaryEntry( 40009, ownerSocket.language ) );

		FinalizeVendorRemoval( vendor );
		return;
	}

	var itemCount = DropVendorItemsAtLocation( vendor );
	var totalGold = DropVendorGoldAtLocation( vendor );
	var offlineDeed = CreateVendorDismissDeedAtLocation( vendor );

	if( notifyChar.socket )
	{
		var notifySocket = notifyChar.socket;
		notifySocket.SysMessage( GetDictionaryEntry( 40010, notifySocket.language ), vendor.name );

		if( itemCount > 0 )
			notifySocket.SysMessage( GetDictionaryEntry( 40011, notifySocket.language ), itemCount );

		if( totalGold > 0 )
			notifySocket.SysMessage( GetDictionaryEntry( 40006, notifySocket.language ), totalGold );

		if( ValidateObject( offlineDeed ))
			notifySocket.SysMessage( GetDictionaryEntry( 40009, notifySocket.language ) );
	}

	FinalizeVendorRemoval( vendor );
}

/** @param {Character} vendor @param {Character|null} notifyChar @returns {boolean} */
function CheckVendorUpkeep( vendor, notifyChar )
{
	if( vendor.aitype != 17 )
		return false;

	if( !VendorSettings.ChargesEnabled )
		return true;

	var chargeInterval = VendorSettings.ChargeHours * 60 * 60;
	if( chargeInterval <= 0 )
		return true;

	var currentTime = Math.floor( GetCurrentClock() / 1000 );
	var lastChargeTime = vendor.GetTag( "VendorLastChargeTime" );

	if( lastChargeTime <= 0 )
	{
		vendor.SetTag( "VendorLastChargeTime", currentTime );
		return true;
	}

	if( currentTime <= lastChargeTime )
		return true;

	var periodsOwed = Math.floor(( currentTime - lastChargeTime ) / chargeInterval );
	if( periodsOwed <= 0 )
		return true;

	var costPerPeriod = GetVendorChargePerPeriod( vendor );
    for( var i = 0; i < periodsOwed; ++i )
    {
        if( !PayVendorCharge( vendor, costPerPeriod ) )
        {
            RemoveVendorForNoFunds( vendor, notifyChar );
            return false;
        }
    }

	vendor.SetTag( "VendorLastChargeTime", lastChargeTime + ( periodsOwed * chargeInterval ) );
	return true;
}

/** @param {Character} pUser @param {Character} vendor */
function StoreVendorForGump( pUser, vendor )
{
	if( vendor.aitype == 17 )
		pUser.SetTempTag( "PlayerVendorGumpSerial", vendor.serial );
}

/** @param {Character} pUser @returns {Character|null} */
function GetStoredVendorForGump( pUser )
{
	var serial = pUser.GetTempTag( "PlayerVendorGumpSerial" );
	if( !serial )
		return null;

	var vendor = CalcCharFromSer( serial );
	if( !ValidateObject( vendor ) || vendor.aitype != 17 )
		return null;

	return vendor;
}

/** @param {Character} pUser @param {Character} vendor */
function OpenVendorBackpack( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket || !ValidateObject( vendor.pack ) )
		return;

	pUser.socket.OpenContainer( vendor.pack );
}

/** @param {Character} pUser @returns {number} */
function GetPackGoldAmount( pUser )
{
	if( !ValidateObject( pUser.pack ) )
		return 0;

	var totalGold = 0;
	var itemObj = pUser.pack.FirstItem();
	for( ; !pUser.pack.FinishedItems(); itemObj = pUser.pack.NextItem() )
	{
		if( ValidateObject( itemObj ) && itemObj.id == 0x0EED )
			totalGold += itemObj.amount;
	}

	return totalGold;
}

/** @param {Character} pUser @param {number} amount @returns {boolean} */
function RemoveGoldFromPack( pUser, amount )
{
	if( amount <= 0 || !ValidateObject( pUser.pack ) )
		return false;

	if( GetPackGoldAmount( pUser ) < amount )
		return false;

	var goldStacks = [];
	var itemObj = pUser.pack.FirstItem();
	for( ; !pUser.pack.FinishedItems(); itemObj = pUser.pack.NextItem() )
	{
		if( ValidateObject( itemObj ) && itemObj.id == 0x0EED )
			goldStacks.push( itemObj );
	}

	for( var i = 0; i < goldStacks.length && amount > 0; ++i )
	{
		if( goldStacks[i].amount <= amount )
		{
			amount -= goldStacks[i].amount;
			goldStacks[i].Delete();
		}
		else
		{
			goldStacks[i].amount -= amount;
			amount = 0;
		}
	}

	return amount <= 0;
}

/** @type { ( pUser: Character ) => Item|null } */
function GetBankBox( pUser )
{
	var bankBox = pUser.FindItemLayer( 29 );
	return ValidateObject( bankBox ) ? bankBox : null;
}

/** @param {Character} pUser @returns {boolean} */
function CanBankReceiveVendorPayout( pUser )
{
	var bankBox = GetBankBox( pUser );
	if( !ValidateObject( bankBox ) )
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
	if( checkSize <= 0 )
		return null;

	var bankBox = GetBankBox( pUser );
	if( !ValidateObject( bankBox ))
		return null;

	var newCheck = CreateDFNItem( pUser.socket, pUser, "bankcheck", 1, "ITEM", false );
	if( !ValidateObject( newCheck ))
		return null;

	newCheck.SetTag( "CheckSize", checkSize );
	newCheck.AddScriptTrigger( 5015 );
	newCheck.container = bankBox;
	return newCheck;
}

/** @param {Character} pTalking @param {Item} bankBox @param {number} depositAmt */
function DivideDepositedGold( pTalking, bankBox, depositAmt )
{
	var remainingGold = depositAmt;
	var newGoldPile;

	while( remainingGold > 0 )
	{
		if( remainingGold >= 65535 )
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", 65535, "ITEM", false );
			remainingGold -= 65535;
		}
		else
		{
			newGoldPile = CreateDFNItem( pTalking.socket, pTalking, "0x0EED", remainingGold, "ITEM", false );
			remainingGold = 0;
		}

		if( ValidateObject( newGoldPile ) )
			newGoldPile.container = bankBox;
	}
}

/** @param {Character} pUser @param {number} goldAmount @returns {boolean} */
function DepositVendorGoldToBankBox( pUser, goldAmount )
{
	var bankBox = GetBankBox( pUser );
	if( !ValidateObject( bankBox ) || goldAmount <= 0 )
		return false;

	if( bankBox.totalItemCount >= bankBox.maxItems )
		return false;

	if( bankBox.weight > bankBox.weightMax )
		return false;

	DivideDepositedGold( pUser, bankBox, goldAmount );
	return true;
}

/** @type { ( pUser: Character, vendor: Character ) => void } */
function CollectVendorGold( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	var socket = pUser.socket;
	var heldGold = vendor.vendorGoldHeld || 0;

	if( heldGold <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 40012, socket.language ));
		return;
	}

	if( !CanBankReceiveVendorPayout( pUser ))
	{
		socket.SysMessage( GetDictionaryEntry( 40013, socket.language ));
		return;
	}

	if( heldGold > 2000 )
	{
		var vendorCheck = CreateVendorBankCheck( pUser, heldGold );
		if( !ValidateObject( vendorCheck ))
		{
			socket.SysMessage( GetDictionaryEntry( 40014, socket.language ));
			return;
		}

		vendor.vendorGoldHeld = 0;
		socket.SysMessage( GetDictionaryEntry( 40015, socket.language ), heldGold );
		return;
	}

	if( !DepositVendorGoldToBankBox( pUser, heldGold ))
	{
		socket.SysMessage( GetDictionaryEntry( 40016, socket.language ));
		return;
	}

	vendor.vendorGoldHeld = 0;
	socket.SysMessage( GetDictionaryEntry( 40017, socket.language ), heldGold );
}

/** @param {Character} pUser @param {Character} vendor @param {number} amount */
function DepositToVendorBank( pUser, vendor, amount )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	var socket = pUser.socket;
	if( amount <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 40018, socket.language ));
		return;
	}

	if( !ValidateObject( pUser.pack ))
	{
		socket.SysMessage( GetDictionaryEntry( 40019, socket.language ));
		return;
	}

	if( GetPackGoldAmount( pUser ) < amount )
	{
		socket.SysMessage( GetDictionaryEntry( 40020, socket.language ));
		return;
	}

	var deposited = DepositVendorBankGold( vendor, amount );
	if( deposited <= 0 )
	{
		socket.SysMessage( GetDictionaryEntry( 40021, socket.language ));
		return;
	}

	if( !RemoveGoldFromPack( pUser, deposited ))
	{
		WithdrawVendorBankGold( vendor, deposited );
		socket.SysMessage( GetDictionaryEntry( 40022, socket.language ));
		return;
	}

	socket.SysMessage( GetDictionaryEntry( 40023, socket.language ), deposited );

	if( deposited < amount )
		socket.SysMessage( GetDictionaryEntry( 40024, socket.language ));
}

/** @param {Character} pUser @param {Character} vendor */
function FlushBankToHeld( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	var moved = FlushVendorBankToHeldGold( vendor );
	if( moved <= 0 )
	{
		pUser.socket.SysMessage( GetDictionaryEntry( 40025, pUser.socket.language ));
		return;
	}

	pUser.socket.SysMessage( GetDictionaryEntry( 40026, pUser.socket.language ), moved );
}

/** @param {Character} pUser @param {Character} vendor */
function DismissVendor( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	var socket = pUser.socket;
	var deed = CreateVendorDismissDeedForOwner( pUser, vendor );
	if( !ValidateObject( deed ))
	{
		socket.SysMessage( GetDictionaryEntry( 40027, socket.language ));
		return;
	}

	var itemResults = ReturnVendorItemsToOwner( vendor, pUser );
	var goldResult = ReturnVendorGoldToOwner( pUser, vendor );
	var deedLocation = PlaceItemForOwnerOrDrop( deed, pUser, vendor );

	socket.SysMessage( GetDictionaryEntry( 581, socket.language ), deed.name );

	if(( itemResults.pack + itemResults.bank + itemResults.ground ) > 0 )
	{
		if( VendorSettings.OnlyReturnToBank )
			socket.SysMessage( GetDictionaryEntry( 40002, socket.language ), itemResults.bank, itemResults.ground );
		else
			socket.SysMessage( GetDictionaryEntry( 40003, socket.language ), itemResults.pack, itemResults.bank, itemResults.ground );
	}

	if( goldResult.amount > 0 )
	{
		if( goldResult.location == 1 )
			socket.SysMessage( GetDictionaryEntry( 40004, socket.language ), goldResult.amount );
		else if( goldResult.location == 2 )
			socket.SysMessage( GetDictionaryEntry( 40005, socket.language ), goldResult.amount );
		else if( goldResult.location == 3 )
			socket.SysMessage( GetDictionaryEntry( 40006, socket.language ), goldResult.amount );
	}

	if( deedLocation == 1 )
		socket.SysMessage( GetDictionaryEntry( 40007, socket.language ) );
	else if( deedLocation == 2 )
		socket.SysMessage( GetDictionaryEntry( 40008, socket.language ) );
	else if( deedLocation == 3 )
		socket.SysMessage( GetDictionaryEntry( 40009, socket.language ) );

	FinalizeVendorRemoval( vendor );
}

/** @param {Character} pUser @param {Character} vendor */
function CycleVendorDirection( pUser, vendor )
{
	if( vendor.aitype != 17 )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	vendor.direction = ( vendor.direction + 1 ) % 8;
}

/** @param {Character} vendor @param {string} speech @returns {string} */
function NormalizeVendorSpeech( vendor, speech )
{
	if( !speech )
		return "";

	var cleanedSpeech = ( "" + speech ).toLowerCase().replace( /^\s+|\s+$/g, "" );
	if( vendor.aitype != 17 || !vendor.name )
		return cleanedSpeech;

	var vendorName = ( "" + vendor.name ).toLowerCase();
	if( cleanedSpeech.indexOf( vendorName + " " ) == 0 )
		return cleanedSpeech.substring( vendorName.length + 1 );

	if( cleanedSpeech.indexOf( vendorName + "," ) == 0 )
		return cleanedSpeech.substring( vendorName.length + 1 ).replace( /^\s+|\s+$/g, "" );

	return cleanedSpeech;
}

/** @param {Character} pUser @param {Character} vendor @returns {boolean} */
function VendorAccessBlocked( pUser, vendor )
{
	if( ValidateObject( vendor.multi ) && vendor.multi.IsOnBanList( pUser ))
	{
		if( pUser.socket )
			pUser.socket.SysMessage( GetDictionaryEntry( 40059, pUser.socket.language ));
		return true;
	}

	if( !CheckVendorUpkeep( vendor, pUser ))
		return true;

	return false;
}

/** @param {Character} pUser @param {Character} vendor @param {string} speech @returns {number} */
function HandleVendorSpeechCommand( pUser, vendor, speech )
{
	if( !ValidateObject( pUser ) || !ValidateObject( vendor ) || vendor.aitype != 17 )
		return 0;

	var cmd = NormalizeVendorSpeech( vendor, speech );

	if( cmd == "view" || cmd == "browse" || cmd == "look" )
	{
		if( VendorAccessBlocked( pUser, vendor ))
			return 2;

		OpenVendorBackpack( pUser, vendor );
		return 2;
	}

	if( cmd == "buy" || cmd == "purchase" )
	{
		if( VendorAccessBlocked( pUser, vendor ))
			return 2;

		pUser.BuyFrom( vendor );
		return 2;
	}

	if( cmd == "gold" || cmd == "collect" || cmd == "get" )
	{
		if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		{
			vendor.TextMessage( GetDictionaryEntry( 40028 ));
			return 2;
		}

		if( !CheckVendorUpkeep( vendor, pUser ))
			return 2;

		CollectVendorGold( pUser, vendor );
		return 2;
	}

	if( cmd == "status" || cmd == "info" )
	{
		if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		{
			vendor.TextMessage( GetDictionaryEntry( 40028 ));
			return 2;
		}

		if( !CheckVendorUpkeep( vendor, pUser ))
			return 2;

		if( pUser.socket )
		{
			StoreVendorForGump( pUser, vendor );
			ShowPlayerVendorOwnerGump( pUser.socket, vendor );
		}
		return 2;
	}

	if( cmd == "dismiss" || cmd == "replace" )
	{
		if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		{
			vendor.TextMessage( GetDictionaryEntry( 40028 ));
			return 2;
		}

		DismissVendor( pUser, vendor );
		return 2;
	}

	if( cmd == "vendor cycle" || cmd == "cycle" )
	{
		if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		{
			vendor.TextMessage( GetDictionaryEntry( 40028 ));
			return 2;
		}

		CycleVendorDirection( pUser, vendor );
		return 2;
	}

	return 0;
}

/** @param {Character} vendor @param {number} layer */
function RemoveVendorLayerItem( vendor, layer )
{
	var layerItem = vendor.FindItemLayer( layer );
	if( ValidateObject( layerItem ))
		layerItem.Delete();
}

/** @param {Character} vendor @param {number} layer @param {number} itemID @param {number} hue @param {string} itemName */
function SetVendorLayerItem( vendor, layer, itemID, hue, itemName )
{
	var oldItem = vendor.FindItemLayer( layer );
	if( ValidateObject( oldItem ))
		oldItem.Delete();

	if( itemID == 0 )
		return;

	var newItem = CreateBlankItem( vendor.socket ? vendor.socket : null, vendor, 1, itemName, itemID, hue, "ITEM", false );
	if( !ValidateObject( newItem ))
		return;

	newItem.container = vendor;
	newItem.layer = layer;
}

/** @param {Character} vendor @param {number} layer @param {number} hue */
function SetVendorLayerHue( vendor, layer, hue )
{
	var layerItem = vendor.FindItemLayer( layer );
	if( ValidateObject( layerItem ))
		layerItem.colour = hue;
}

/** @param {Character} vendor @param {boolean} isElf @param {boolean} isFemale */
function SetVendorRaceGenderBody( vendor, isElf, isFemale )
{
	if( isElf )
		vendor.id = isFemale ? 0x025E : 0x025D;
	else
		vendor.id = isFemale ? 0x0191 : 0x0190;

	if( isFemale )
		RemoveVendorLayerItem( vendor, VendorEquipmentLayer.Beard );

	vendor.Teleport();
}

/** @param {Character} pUser @param {number} categoryIndex */
function StoreClothingDyeCategory( pUser, categoryIndex )
{
	pUser.SetTempTag( "PlayerVendorClothingDyeCategory", categoryIndex );
}

/** @param {Character} pUser @returns {number} */
function GetStoredClothingDyeCategory( pUser )
{
	var categoryIndex = pUser.GetTempTag( "PlayerVendorClothingDyeCategory" );
	return ( categoryIndex === null || categoryIndex === undefined || categoryIndex === "" ) ? -1 : categoryIndex;
}

/** @param {Socket} socket @param {Character} vendor */
function ShowPlayerVendorOwnerGump( socket, vendor )
{
	if( socket == null )
		return;

	if( vendor.aitype != 17 )
		return;

	var heldGold = vendor.vendorGoldHeld || 0;
	var bankGold = vendor.GetTag( "VendorBankAccount" );
	if( bankGold < 0 )
		bankGold = 0;

	var perPeriod = GetVendorChargePerPeriod( vendor );
	var days = GetVendorDaysAffordable( vendor );
	var hours = GetVendorHoursAffordable( vendor );
	var itemFeeTotal = GetVendorItemFeeTotal( vendor );

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 25, 10, 555, 240, 5054 );
	myGump.AddCheckerTrans( 32, 20, 537, 220 );
	myGump.NoResize();

	myGump.AddButton( 390, 25, 4005, 4007, 1, 0, 1 );
	myGump.AddHTMLGump( 425, 25, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40029, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 48, 4005, 4007, 1, 0, 2 );
	myGump.AddHTMLGump( 425, 48, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40030, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 71, 4005, 4007, 1, 0, 7 );
	myGump.AddHTMLGump( 425, 72, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40048, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 94, 4005, 4007, 1, 0, 8 );
	myGump.AddHTMLGump( 425, 95, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40049, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 117, 4005, 4007, 1, 0, 3 );
	myGump.AddHTMLGump( 425, 118, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40031, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 140, 4005, 4007, 1, 0, 4 );
	myGump.AddHTMLGump( 425, 141, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40032, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 163, 4005, 4007, 1, 0, 5 );
	myGump.AddHTMLGump( 425, 164, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40033, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 186, 4005, 4007, 1, 0, 6 );
	myGump.AddHTMLGump( 425, 187, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40034, socket.language ) + "</basefont>" );

	myGump.AddButton( 390, 209, 4005, 4007, 1, 0, 0 );
	myGump.AddHTMLGump( 425, 210, 120, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 18762, socket.language ) + "</basefont>" );

	if( !VendorSettings.ChargesEnabled )
	{
		myGump.AddHTMLGump( 40, 25, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40035, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 25, 2603, GetDictionaryEntry( 40043, socket.language ) );

		myGump.AddHTMLGump( 40, 48, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40036, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 48, 2603, GetDictionaryEntry( 40043, socket.language ) );

		myGump.AddHTMLGump( 40, 72, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40037, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 72, 1153, "" + heldGold );

		myGump.AddHTMLGump( 40, 96, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40038, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 96, 1153, "" + bankGold );

		myGump.AddHTMLGump( 40, 120, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40039, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 120, 2603, GetDictionaryEntry( 40044, socket.language ) );
	}
	else
	{
		myGump.AddHTMLGump( 40, 25, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40035, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 25, 1153, "" + days );

		myGump.AddHTMLGump( 40, 48, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40036, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 48, 1153, "" + hours );

		myGump.AddHTMLGump( 40, 72, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40037, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 72, 1153, "" + heldGold );

		myGump.AddHTMLGump( 40, 96, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40038, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 96, 1153, "" + bankGold );

		myGump.AddHTMLGump( 40, 120, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40040, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 120, 1153, "" + perPeriod );

		myGump.AddHTMLGump( 40, 144, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40041, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 144, 1153, "" + VendorSettings.ChargeHours );

		myGump.AddHTMLGump( 40, 168, 260, 20, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40042, socket.language ) + "</basefont>" );
		myGump.AddText( 300, 168, 1153, "" + itemFeeTotal );
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @param {Socket} socket @param {Gump} myGump @param {number} menuPage @param {number} categoryPageStart @param {string} title */
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
	myGump.AddHTMLGump( 305, 342, 60, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" );
}

/** @param {Socket} socket @param {Gump} myGump @param {number} pageNumber @param {string} title @param {number[]} hueList @param {number} buttonBase @param {number} backPage */
function AddHairHueCategoryPage( socket, myGump, pageNumber, title, hueList, buttonBase, backPage )
{
	myGump.AddPage( pageNumber );
	myGump.AddHTMLGump( 10, 42, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + title + "</CENTER></basefont>" );

	var columns = 3;
	var startX = 20;
	var startY = 70;
	var colWidth = 112;
	var rowHeight = 18;

	for( var i = 0; i < hueList.length; ++i )
	{
		var row = Math.floor( i / columns );
		var col = i % columns;
		var x = startX + ( col * colWidth );
		var y = startY + ( row * rowHeight );
		var hueHex = hueList[i].toString( 16 ).toUpperCase();

		myGump.AddButton( x, y, 0x0A9A, 0x0A9A, 1, pageNumber, buttonBase + hueList[i] );
		myGump.AddText( x + 25, y, hueList[i], "0x" + hueHex );
	}

	myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 0, backPage, 0 );
	myGump.AddHTMLGump( 45, 342, 80, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" );

	myGump.AddButton( 270, 340, 0xFA5, 0xFA7, 1, 1, 5007 );
	myGump.AddHTMLGump( 305, 342, 60, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40045, socket.language ) + "</basefont>" );
}

/** @param {Character} pUser @param {Character} vendor */
function ShowVendorCustomizeGump( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	var socket = pUser.socket;
	var isElf = ( vendor.id == 0x025D || vendor.id == 0x025E );
	var isFemale = ( vendor.id == 0x0191 || vendor.id == 0x025E );
	var hairStyles = humanHairStyles;

	if( isElf )
		hairStyles = isFemale ? femaleElfHairStyles : maleElfHairStyles;

	StoreVendorForGump( pUser, vendor );

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 370, 370, 0x13BE );
	myGump.AddCheckerTrans( 10, 10, 350, 350 );
	myGump.AddHTMLGump( 10, 12, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40046, socket.language ) + "</CENTER></basefont>" );

	myGump.AddPage( 1 );
	myGump.AddHTMLGump( 10, 42, 150, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40047, socket.language ) + "</CENTER></basefont>" );

	for( var i = 0; i < hairStyles.length; ++i )
	{
		myGump.AddButton( 10, 70 + ( i * 20 ), 0xFA5, 0xFA7, 1, 1, 0x1000 + i );
		myGump.AddHTMLGump( 45, 72 + ( i * 20 ), 110, 18, false, false, "<basefont color=#ffffff>" + GetHairOrBeardName( hairStyles[i], socket ) + "</basefont>" );
	}

	myGump.AddButton( 10, 70 + ( hairStyles.length * 20 ), 0xFB1, 0xFB3, 1, 1, 5002 );
	myGump.AddHTMLGump( 45, 72 + ( hairStyles.length * 20 ), 110, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40071, socket.language ) + "</basefont>" );

	myGump.AddButton( 10, 70 + (( hairStyles.length + 1 ) * 20 ), 0xFA5, 0xFA7, 0, 2, 0 );
	myGump.AddHTMLGump( 45, 72 + (( hairStyles.length + 1 ) * 20 ), 110, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40072, socket.language ) + "</basefont>" );

	if( !isElf && !isFemale )
	{
		myGump.AddHTMLGump( 160, 42, 190, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40073, socket.language ) + "</CENTER></basefont>" );

		for( i = 0; i < humanBeardStyles.length; ++i )
		{
			myGump.AddButton( 160, 70 + ( i * 20 ), 0xFA5, 0xFA7, 1, 1, 0x2000 + i );
			myGump.AddHTMLGump( 195, 72 + ( i * 20 ), 160, 18, false, false, "<basefont color=#ffffff>" + GetHairOrBeardName( humanBeardStyles[i], socket ) + "</basefont>" );
		}

		myGump.AddButton( 160, 70 + ( humanBeardStyles.length * 20 ), 0xFB1, 0xFB3, 1, 1, 5004 );
		myGump.AddHTMLGump( 195, 72 + ( humanBeardStyles.length * 20 ), 160, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40074, socket.language ) + "</basefont>" );

		myGump.AddButton( 160, 70 + (( humanBeardStyles.length + 1 ) * 20 ), 0xFA5, 0xFA7, 0, 100, 0 );
		myGump.AddHTMLGump( 195, 72 + (( humanBeardStyles.length + 1 ) * 20 ), 160, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40075, socket.language ) + "</basefont>" );
	}

	myGump.AddButton( 160, 290, 0xFA5, 0xFA7, 1, 1, 5001 );
	myGump.AddHTMLGump( 195, 292, 160, 18, false, false, isFemale ? "<basefont color=#ffffff>" + GetDictionaryEntry( 40076, socket.language ) + "</basefont>" : "<basefont color=#ffffff>" + GetDictionaryEntry( 40077, socket.language ) + "</basefont>" );

	myGump.AddButton( 160, 310, 0xFA5, 0xFA7, 1, 1, 5006 );
	myGump.AddHTMLGump( 195, 312, 160, 18, false, false, isElf ? "<basefont color=#ffffff>" + GetDictionaryEntry( 40078, socket.language ) + "</basefont>" : "<basefont color=#ffffff>" + GetDictionaryEntry( 40079, socket.language ) + "</basefont>" );

	myGump.AddButton( 160, 330, 0xFA5, 0xFA7, 1, 0, 9 );
	myGump.AddHTMLGump( 195, 332, 160, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40080, socket.language ) + "</basefont>" );

	myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 1, 1, 5000 );
	myGump.AddHTMLGump( 45, 342, 305, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40081, socket.language ) + "</basefont>" );

	AddHairHueCategoryMenu( socket, myGump, 2, 10, GetDictionaryEntry( 40082, socket.language ) );
	AddHairHueCategoryMenu( socket, myGump, 100, 110, GetDictionaryEntry( 40083, socket.language ) );

	for( i = 0; i < vendorHairHueCategories.length; ++i )
	{
		var hairTitle = GetDictionaryEntry( 40084, socket.language ).replace( /%s/gi, GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) );
		var beardTitle = GetDictionaryEntry( 40085, socket.language ).replace( /%s/gi, GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) );

		AddHairHueCategoryPage( socket, myGump, 10 + i, hairTitle, vendorHairHueCategories[i].hues, 0x3000, 2 );
		AddHairHueCategoryPage( socket, myGump, 110 + i, beardTitle, vendorHairHueCategories[i].hues, 0x4000, 100 );
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @type { ( pUser: Character, vendor: Character, pButton: number ) => void } */
function HandleVendorCustomizeButton( pUser, vendor, pButton )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	var isElf = ( vendor.id == 0x025D || vendor.id == 0x025E );
	var isFemale = ( vendor.id == 0x0191 || vendor.id == 0x025E );
	var hairStyles = humanHairStyles;

	if( isElf )
		hairStyles = isFemale ? femaleElfHairStyles : maleElfHairStyles;

	// Return to vendor owner menu
	if( pButton == 5000 )
	{
		ShowPlayerVendorOwnerGump( pUser.socket, vendor );
		return;
	}

	// Refresh vendor customize menu
	if( pButton == 5007 )
	{
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Toggle vendor gender
	if( pButton == 5001 )
	{
		RemoveVendorLayerItem( vendor, VendorEquipmentLayer.Beard );
		SetVendorRaceGenderBody( vendor, isElf, !isFemale );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Toggle vendor race
	if( pButton == 5006 )
	{
		RemoveVendorLayerItem( vendor, VendorEquipmentLayer.Hair );
		RemoveVendorLayerItem( vendor, VendorEquipmentLayer.Beard );
		SetVendorRaceGenderBody( vendor, !isElf, isFemale );

		var newHairStyles = humanHairStyles;
		var newIsElf = ( vendor.id == 0x025D || vendor.id == 0x025E );
		var newIsFemale = ( vendor.id == 0x0191 || vendor.id == 0x025E );

		if( newIsElf )
			newHairStyles = newIsFemale ? femaleElfHairStyles : maleElfHairStyles;

		if( newHairStyles.length > 0 )
			SetVendorLayerItem( vendor, VendorEquipmentLayer.Hair, newHairStyles[0].itemID, 0, "vendor hair" );

		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Remove vendor hair
	if( pButton == 5002 )
	{
		RemoveVendorLayerItem( vendor, VendorEquipmentLayer.Hair );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Remove vendor beard
	if( pButton == 5004 )
	{
		RemoveVendorLayerItem( vendor, VendorEquipmentLayer.Beard );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Set vendor hair style
	if( pButton >= 0x1000 && pButton < ( 0x1000 + 256 ) )
	{
		var hairIndex = pButton - 0x1000;
		if( hairIndex >= 0 && hairIndex < hairStyles.length )
		{
			var oldHairItem = vendor.FindItemLayer( VendorEquipmentLayer.Hair );
			var oldHairHue = ValidateObject( oldHairItem ) ? oldHairItem.colour : 0;
			SetVendorLayerItem( vendor, VendorEquipmentLayer.Hair, hairStyles[hairIndex].itemID, oldHairHue, "vendor hair" );
		}

		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Set vendor beard style
	if( pButton >= 0x2000 && pButton < ( 0x2000 + 256 ) )
	{
		var beardIndex = pButton - 0x2000;
		if( !isElf && !isFemale && beardIndex >= 0 && beardIndex < humanBeardStyles.length )
		{
			var oldBeardItem = vendor.FindItemLayer( VendorEquipmentLayer.Beard );
			var oldBeardHue = ValidateObject( oldBeardItem ) ? oldBeardItem.colour : 0;
			SetVendorLayerItem( vendor, VendorEquipmentLayer.Beard, humanBeardStyles[beardIndex].itemID, oldBeardHue, "vendor beard" );
		}

		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Set vendor hair color
	if( pButton >= 0x3000 && pButton < 0x4000 )
	{
		SetVendorLayerHue( vendor, VendorEquipmentLayer.Hair, pButton - 0x3000 );
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Set vendor beard color
	if( pButton >= 0x4000 && pButton < 0x5000 )
	{
		if( !isElf && !isFemale )
			SetVendorLayerHue( vendor, VendorEquipmentLayer.Beard, pButton - 0x4000 );

		ShowVendorCustomizeGump( pUser, vendor );
	}
}

/** @param {Character} pUser @param {Character} vendor */
function ShowVendorClothingGump( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	var socket = pUser.socket;
	StoreVendorForGump( pUser, vendor );

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 585, 393, 5054 );
	myGump.AddBackground( 195, 36, 387, 275, 3000 );
	myGump.AddHTMLGump( 10, 10, 565, 18, false, false, "<CENTER>" + GetDictionaryEntry( 40086, socket.language ) + "</CENTER>" );

	myGump.AddHTMLGump( 60, 355, 150, 18, false, false, GetDictionaryEntry( 18718, socket.language ) );
	myGump.AddButton( 25, 355, 4005, 4007, 1, 0, 5000 );

	myGump.AddHTMLGump( 320, 355, 150, 18, false, false, GetDictionaryEntry( 10267, socket.language ) );
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

		for( var entryIndex = 0; entryIndex < cat.entries.length; ++entryIndex )
		{
			var entry = cat.entries[entryIndex];
			var cols = cat.name == "Held Items" ? 4 : 3;
			var startX = cat.name == "Held Items" ? 205 : 198;
			var startY = cat.name == "Held Items" ? 42 : 38;
			var colWidth = cat.name == "Held Items" ? 92 : 129;
			var rowHeight = cat.name == "Held Items" ? 52 : 67;
			var textWidth = cat.name == "Held Items" ? 80 : 100;
			var itemOffsetX = cat.name == "Held Items" ? 18 : 20;
			var itemOffsetY = cat.name == "Held Items" ? 16 : 25;
			var buttonOffsetY = cat.name == "Held Items" ? 30 : 45;

			var x = startX + ( entryIndex % cols ) * colWidth;
			var ey = startY + Math.floor( entryIndex / cols ) * rowHeight;

			myGump.AddHTMLGump( x, ey, textWidth, 18, false, false, entry.name );
			myGump.AddPicture( x + itemOffsetX, ey + itemOffsetY, entry.itemID );
			myGump.AddButton( x, ey + buttonOffsetY, 4005, 4007, 1, 20 + i, 0x5000 + ( i * 100 ) + entryIndex );
		}

		if( cat.canDye )
		{
			myGump.AddButton( 300, 315, 4005, 4007, 1, 0, 0x8000 + i );
			myGump.AddHTMLGump( 335, 317, 100, 18, false, false, GetDictionaryEntry( 40087, socket.language ) );
		}

		myGump.AddButton( 430, 315, 4005, 4007, 1, 20 + i, 0x7000 + i );
		myGump.AddHTMLGump( 465, 317, 100, 18, false, false, GetDictionaryEntry( 40088, socket.language ) );
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @param {Character} pUser @param {Character} vendor */
function ShowVendorClothingDyeGump( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	var socket = pUser.socket;
	var categoryIndex = GetStoredClothingDyeCategory( pUser );
	var title = GetDictionaryEntry( 40091, socket.language );

	if( categoryIndex >= 0 && categoryIndex < vendorClothingCategories.length )
		title = vendorClothingCategories[categoryIndex].name + " COLOR";

	StoreVendorForGump( pUser, vendor );

	var myGump = new Gump;
	myGump.AddPage( 0 );
	myGump.AddBackground( 0, 0, 370, 370, 0x13BE );
	myGump.AddCheckerTrans( 10, 10, 350, 350 );
	myGump.AddHTMLGump( 10, 12, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + title + "</CENTER></basefont>" );

	myGump.AddPage( 1 );
	myGump.AddHTMLGump( 10, 42, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetDictionaryEntry( 40089, socket.language ) + "</CENTER></basefont>" );

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

		myGump.AddButton( x, y, 0x0A9A, 0x0A9A, 0, 10 + i, 0 );
		myGump.AddHTMLGump( x + 30, y, 125, 18, false, false, "<basefont color=#ffffff>" + GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) + "</basefont>" );
	}

	myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 1, 0, 0x8FFE );
	myGump.AddHTMLGump( 45, 342, 120, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" );

	for( i = 0; i < vendorHairHueCategories.length; ++i )
	{
		myGump.AddPage( 10 + i );
		myGump.AddHTMLGump( 10, 42, 350, 18, false, false, "<basefont color=#ffffff><CENTER>" + GetVendorHueCategoryName( vendorHairHueCategories[i], socket ) + "</CENTER></basefont>" );

		var hues = vendorHairHueCategories[i].hues;
		for( var j = 0; j < hues.length; ++j )
		{
			var hueRow = Math.floor( j / 3 );
			var hueCol = j % 3;
			var hx = 20 + ( hueCol * 112 );
			var hy = 70 + ( hueRow * 18 );
			var hueHex = hues[j].toString( 16 ).toUpperCase();

			myGump.AddButton( hx, hy, 0x0A9A, 0x0A9A, 1, 10 + i, 0x9000 + hues[j] );
			myGump.AddText( hx + 25, hy, hues[j], "0x" + hueHex );
		}

		myGump.AddButton( 10, 340, 0xFA5, 0xFA7, 0, 1, 0 );
		myGump.AddHTMLGump( 45, 342, 80, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 10267, socket.language ) + "</basefont>" );

		myGump.AddButton( 270, 340, 0xFA5, 0xFA7, 1, 0, 0x8FFE );
		myGump.AddHTMLGump( 305, 342, 60, 18, false, false, "<basefont color=#ffffff>" + GetDictionaryEntry( 40090, socket.language ) + "</basefont>" );
	}

	myGump.Send( socket );
	myGump.Free();
}

/** @param {Character} pUser @param {Character} vendor @param {number} pButton */
function HandleVendorClothingButton( pUser, vendor, pButton )
{
	if( vendor.aitype != 17 )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	// Return to vendor customize menu
	if( pButton == 0x8FFF )
	{
		ShowVendorCustomizeGump( pUser, vendor );
		return;
	}

	// Return to vendor clothing menu
	if( pButton == 0x8FFE )
	{
		ShowVendorClothingGump( pUser, vendor );
		return;
	}

	// Remove selected clothing layer
	if( pButton >= 0x7000 && pButton < ( 0x7000 + 100 ) )
	{
		var removeCatIndex = pButton - 0x7000;
		if( removeCatIndex >= 0 && removeCatIndex < vendorClothingCategories.length )
		{
			RemoveVendorLayerItem( vendor, vendorClothingCategories[removeCatIndex].layer );
			ShowVendorClothingGump( pUser, vendor );
		}
		return;
	}

	// Equip selected clothing item
	if( pButton >= 0x5000 && pButton < ( 0x5000 + 10000 ) )
	{
		var entryValue = pButton - 0x5000;
		var catIndex = Math.floor( entryValue / 100 );
		var entryIndex = entryValue % 100;

		if( catIndex >= 0 && catIndex < vendorClothingCategories.length )
		{
			var cat = vendorClothingCategories[catIndex];
			if( entryIndex >= 0 && entryIndex < cat.entries.length )
			{
				var oldClothingItem = vendor.FindItemLayer( cat.layer );
				var oldClothingHue = ValidateObject( oldClothingItem ) ? oldClothingItem.colour : 0;
				SetVendorLayerItem( vendor, cat.layer, cat.entries[entryIndex].itemID, oldClothingHue, "vendor clothing" );
				ShowVendorClothingGump( pUser, vendor );
			}
		}
		return;
	}

	// Open clothing dye menu
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

	// Set selected clothing color
	if( pButton >= 0x9000 && pButton < ( 0x9000 + 0x1000 ) )
	{
		var hueValue = pButton - 0x9000;
		var storedCatIndex = GetStoredClothingDyeCategory( pUser );

		if( storedCatIndex >= 0 && storedCatIndex < vendorClothingCategories.length )
		{
			var dyeCat = vendorClothingCategories[storedCatIndex];
			if( dyeCat.canDye )
				SetVendorLayerHue( vendor, dyeCat.layer, hueValue );
		}

		ShowVendorClothingDyeGump( pUser, vendor );
	}
}

/** @param {string} text @returns {string} */
function TrimString( text )
{
	if( text === null || text === undefined )
		return "";

	return ( "" + text ).replace( /^\s+|\s+$/g, "" );
}

/** @param {Character} pUser @param {Character} vendor */
function BeginRenameVendor( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	StoreVendorForGump( pUser, vendor );
	pUser.SpeechInput( 20, vendor );
	pUser.socket.SysMessage( GetDictionaryEntry( 40050, pUser.socket.language ) );
}

/** @param {Character} pUser @param {Character} vendor */
function BeginRenameVendorShop( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	StoreVendorForGump( pUser, vendor );
	pUser.SpeechInput( 21, vendor );
	pUser.socket.SysMessage( GetDictionaryEntry( 40051, pUser.socket.language ) );
}

/** @param {Character} pUser @param {Character} vendor */
function BeginDepositVendorGold( pUser, vendor )
{
	if( vendor.aitype != 17 || !pUser.socket )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	StoreVendorForGump( pUser, vendor );
	pUser.SpeechInput( 22, vendor );
	pUser.socket.SysMessage( GetDictionaryEntry( 40052, pUser.socket.language ) );
}

/** @type { ( pUser: Character, targChar: Character, nonMouseClickEvent: boolean ) => boolean } */
function onCharDoubleClick( pUser, targChar, nonMouseClickEvent )
{
	if( !ValidateObject( pUser ) || !ValidateObject( targChar ) )
		return false;

	if( nonMouseClickEvent )
		return true;

	if( targChar.aitype != 17 )
		return true;

	if( !CheckVendorUpkeep( targChar, pUser ) )
		return false;

	if( ValidateObject( targChar.owner ) && targChar.owner.serial == pUser.serial )
	{
		if( pUser.socket )
		{
			StoreVendorForGump( pUser, targChar );
			ShowPlayerVendorOwnerGump( pUser.socket, targChar );
		}
		return false;
	}

	if( ValidateObject( targChar.multi ) && targChar.multi.IsOnBanList( pUser ) )
	{
		if( pUser.socket )
			pUser.socket.SysMessage( GetDictionaryEntry( 40059, pUser.socket.language ) );
		return false;
	}

	OpenVendorBackpack( pUser, targChar );
	return false;
}

/** @param {string} speech @param {Character} personTalking @param {BaseObject} talkingTo @returns {number} */
function onSpeech( speech, personTalking, talkingTo )
{
	if( !ValidateObject( personTalking ) || !ValidateObject( talkingTo ) )
		return 0;

	if( talkingTo.aitype != 17 )
		return 0;

	return HandleVendorSpeechCommand( personTalking, talkingTo, speech );
}

/** @param {Socket} pSock @param {number} pButton @param {GumpData} gumpData */
function onGumpPress( pSock, pButton, gumpData )
{
	var pUser = pSock.currentChar;
	if( !ValidateObject( pUser ) )
		return;

	var vendor = GetStoredVendorForGump( pUser );
	if( !ValidateObject( vendor ) || vendor.aitype != 17 )
		return;

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
		return;

	if( !CheckVendorUpkeep( vendor, pUser ) )
		return;

	// Handle vendor customize buttons
	if( pButton == 5000 || pButton == 5001 || pButton == 5002 || pButton == 5004 || pButton == 5006 || pButton == 5007 ||
		( pButton >= 0x1000 && pButton < ( 0x1000 + 256 ) ) ||
		( pButton >= 0x2000 && pButton < ( 0x2000 + 256 ) ) ||
		( pButton >= 0x3000 && pButton < 0x4000 ) ||
		( pButton >= 0x4000 && pButton < 0x5000 ) )
	{
		HandleVendorCustomizeButton( pUser, vendor, pButton );
		return;
	}

	// Handle vendor clothing buttons
	if( pButton == 0x8FFF || pButton == 0x8FFE ||
		( pButton >= 0x5000 && pButton < ( 0x5000 + 10000 ) ) ||
		( pButton >= 0x7000 && pButton < ( 0x7000 + 100 ) ) ||
		( pButton >= 0x8000 && pButton < ( 0x8000 + vendorClothingCategories.length ) ) ||
		( pButton >= 0x9000 && pButton < ( 0x9000 + 0x1000 ) ) )
	{
		HandleVendorClothingButton( pUser, vendor, pButton );
		return;
	}

	switch( pButton )
	{
		case 0: break;
		case 1: OpenVendorBackpack( pUser, vendor ); break;
		case 2: ShowVendorCustomizeGump( pUser, vendor ); break;
		case 3:
			CollectVendorGold( pUser, vendor );
			ShowPlayerVendorOwnerGump( pSock, vendor );
			break;
		case 4: BeginDepositVendorGold( pUser, vendor ); break;
		case 5:
			FlushBankToHeld( pUser, vendor );
			ShowPlayerVendorOwnerGump( pSock, vendor );
			break;
		case 6: DismissVendor( pUser, vendor ); break;
		case 7: BeginRenameVendor( pUser, vendor ); break;
		case 8: BeginRenameVendorShop( pUser, vendor ); break;
		case 9: ShowVendorClothingGump( pUser, vendor ); break;
	}
}

/** @type { ( pUser: Character, targObj: BaseObject, pSpeech: string, pSpeechID: number ) => void } */
function onSpeechInput( pUser, targObj, pSpeech, pSpeechID )
{
	if( !ValidateObject( pUser ) || !pUser.socket )
		return;

	var socket = pUser.socket;
	var vendor = GetStoredVendorForGump( pUser );
	if( !ValidateObject( vendor ) || vendor.aitype != 17 )
	{
		socket.SysMessage( GetDictionaryEntry( 40053, socket.language ) );
		return;
	}

	if( !ValidateObject( vendor.owner ) || vendor.owner.serial != pUser.serial )
	{
		socket.SysMessage( GetDictionaryEntry( 40028, socket.language ) );
		return;
	}

	if( !CheckVendorUpkeep( vendor, pUser ) )
		return;

	var newText = TrimString( pSpeech );
	if( newText == "" )
	{
		socket.SysMessage( GetDictionaryEntry( 9270, socket.language ) );
		return;
	}

	switch( pSpeechID )
	{
		case 20:
			if( newText.length > 50 )
			{
				socket.SysMessage( GetDictionaryEntry( 9271, socket.language ) );
				return;
			}

			vendor.name = newText;
			vendor.Refresh();
			socket.SysMessage( GetDictionaryEntry( 40054, socket.language ), vendor.name );
			ShowPlayerVendorOwnerGump( socket, vendor );
			break;

		case 21:
			if( newText.length > 40 )
			{
				socket.SysMessage( GetDictionaryEntry( 40055, socket.language ) );
				return;
			}

			vendor.title = "Shop Name: " + newText;
			vendor.Refresh();
			socket.SysMessage( GetDictionaryEntry( 40056, socket.language ), vendor.title );
			ShowPlayerVendorOwnerGump( socket, vendor );
			break;

		case 22:
			if( !/^\d+$/.test( newText ) )
			{
				socket.SysMessage( GetDictionaryEntry( 40057, socket.language ) );
				return;
			}

			var depositAmount = parseInt( newText, 10 );
			if( isNaN( depositAmount ) || depositAmount <= 0 )
			{
				socket.SysMessage( GetDictionaryEntry( 40018, socket.language ) );
				return;
			}

			DepositToVendorBank( pUser, vendor, depositAmount );
			ShowPlayerVendorOwnerGump( socket, vendor );
			break;
	}
}