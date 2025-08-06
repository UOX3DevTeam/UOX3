declare global {
// Object Types

  interface Account {
    readonly  character1:     CharOrNull;
    readonly  character2:     CharOrNull;
    readonly  character3:     CharOrNull;
    readonly  character4:     CharOrNull;
    readonly  character5:     CharOrNull;
    readonly  character6:     CharOrNull;
    readonly  character7:     CharOrNull;
              comment:        string;
    readonly  currentChar:    CharOrNull;
              firstLogin:     number;
    readonly  flags:          number;
    readonly  id:             number;
              isBanned:       boolean;
              isCounselor:    boolean;
              isGM:           boolean;
              isOnline:       boolean;
              isPublic:       boolean;
              isSeer:         boolean;
              isSlot1Blocked: boolean;
              isSlot2Blocked: boolean;
              isSlot3Blocked: boolean;
              isSlot4Blocked: boolean;
              isSlot5Blocked: boolean;
              isSlot6Blocked: boolean;
              isSlot7Blocked: boolean;
              isSuspended:    boolean;
              isYoung:        boolean;
              lastIP:         string;
              timeban:        number;
              totalPlayTime:  number;
              unused10:       boolean;
    readonly  username:       string;
    AddAccount( username: string, password: string, comment: string, flags: string | number ): void;
    DelAccount( username: string ): void;
    DelAccount( accountId: number ): void;
  }
  interface Accounts {
  }
  interface BaseObject {
              color:              number;
              colour:             number;
              damageIncrease:     number;
              defenseChance:      number;
              dexterity:          number;
              health:             number;
              healthRegenBonus:   number;
              hidamage:           number;
              hitChance:          number;
              hue:                number;
              id:                 number;
              instanceID:         number;
              intelligence:       number;
              isChar:             boolean;
              isDispellable:      boolean;
              isGuarded:          boolean;
              isItem:             boolean;
              isSpawner:          boolean;
              lodamage:           number;
              luck:               number;
              manaRegenBonus:     number;
              maxhp:              number;
              multi:              MultiOrNull;
              name:               string;
              oldX:               number;
              oldY:               number;
              oldZ:               number;
              origin:             string;
              owner:              CharOrNull;
              poison:             number;
              race:               RaceOrNull;
              region:             TownRegionOrNull;
              scripttrigger:      number;
              scriptTriggers:     number[];
              sectionID:          string;
              serial:             number;
              shouldSave:         boolean;
              skin:               number;
              spawnSerial:        number;
              staminaRegenBonus:  number;
              strength:           number;
              swingSpeedIncrease: number;
              tithing:            number;
              title:              string;
              visible:            VisibleTypes;
              weight:             number;
              worldnumber:        number;
              x:                  number;
              y:                  number;
              z:                  number;
  }
  type BaseOrNull = BaseObject | null | undefined;
  interface BaseSkills extends CertainSetOf {
  }
  type UOXBaseSkills = BaseSkills;
  interface Boat extends Multi {
  }
  interface CertainSetOf extends Array< number > {
              alchemy:         number;
              allskills:       number;
              anatomy:         number;
              animallore:      number;
              archery:         number;
              armslore:        number;
              begging:         number;
              blacksmithing:   number;
              bowcraft:        number;
              bushido:         number;
              camping:         number;
              carpentry:       number;
              cartography:     number;
              chivalry:        number;
              cooking:         number;
              detectinghidden: number;
              enticement:      number;
              evaluatingintel: number;
              fencing:         number;
              fishing:         number;
              focus:           number;
              forensics:       number;
              healing:         number;
              herding:         number;
              hiding:          number;
              imbuing:         number;
              inscription:     number;
              itemid:          number;
              lockpicking:     number;
              lumberjacking:   number;
              macefighting:    number;
              magery:          number;
              magicresistance: number;
              meditation:      number;
              mining:          number;
              musicianship:    number;
              mysticism:       number;
              necromancy:      number;
              ninjitsu:        number;
              parrying:        number;
              peacemaking:     number;
              poisoning:       number;
              provocation:     number;
              removetrap:      number;
              snooping:        number;
              spellweaving:    number;
              spiritspeak:     number;
              stealing:        number;
              stealth:         number;
              swordsmanship:   number;
              tactics:         number;
              tailoring:       number;
              taming:          number;
              tasteid:         number;
              throwing:        number;
              tinkering:       number;
              tracking:        number;
              veterinary:      number;
              wrestling:       number;
  }
  interface Character extends BaseObject {
              account:               Account | null | undefined;
              accountNum:            number;
              actualDexterity:       number;
              actualIntelligence:    number;
              actualStrength:        number;
              aitype:                number;
              allmove:               true;
              attack:                number;
              attacker:              CharOrNull;
              atWar:                 boolean;
              baseskills:            BaseSkills;
              beardColor:            number;
              beardColour:           number;
              beardStyle:            number;
              brkPeaceChance:        number;
              canAttack:             boolean;
              canBroadcast:          boolean;
              canRun:                boolean;
              canSnoop:              boolean;
              cell:                  number;
              commandlevel:          number;
              controlSlots:          number;
              controlSlotsUsed:      number;
              createdOn:             number;
              criminal:              boolean;
              dead:                  boolean;
              deaths:                number;
              direction:             number;
              emoteColour:           number;
              fame:                  number;
              flag:                  number;
              fleeAt:                number;
              followerCount:         number;
              fontType:              number;
              foodList:              string;
              frozen:                boolean;
              fx1:                   number;
              fx2:                   number;
              fy1:                   number;
              fy2:                   number;
              fz:                    number;
              gender:                number;
              guarding:              BaseOrNull;
              guild:                 GuildOrNull;
              guildTitle:            string;
              hairColor:             number;
              hairColour:            number;
              hairStyle:             number;
              hasStolen:             boolean;
              hireling:              boolean;
              houseicons:            boolean;
              housesCoOwned:         number;
              housesOwned:           number;
              hunger:                number;
              hungerRate:            number;
              hungerWildChance:      number;
              innocent:              boolean;
              isAnimal:              boolean;
              isAwake:               boolean;
              isCasting:             boolean;
              isCounselor:           boolean;
              isDisguised:           boolean;
              isFlying:              boolean;
              isGM:                  boolean;
              isGMPageable:          boolean;
              isHuman:               boolean;
              isIncognito:           boolean;
              isJailed:              boolean;
              isMeditating:          boolean;
              isonhorse:             boolean;
              isPolymorphed:         boolean;
              isRunning:             boolean;
              isShop:                boolean;
              isUsingPotion:         boolean;
              karma:                 number;
              karmaLock:             boolean;
              lastOn:                string;
              lastOnSecs:            number;
              lightlevel:            number;
              loyalty:               number;
              loyaltyRate:           number;
              magicReflect:          boolean;
              mana:                  number;
              maxLoyalty:            number;
              maxmana:               number;
              maxstamina:            number;
              mounted:               boolean;
              murdercount:           number;
              murderer:              boolean;
              neutral:               boolean;
              nextAct:               number;
              noNeedMana:            boolean;
              noNeedReags:           boolean;
              noSkillTitles:         boolean;
              npc:                   boolean;
              npcFlag:               number;
              npcGuild:              number;
              oldWandertype:         number;
              online:                boolean;
              orgID:                 number;
              orgSkin:               number;
              origName:              string;
              orneriness:            number;
              ownedItemsCount:       number;
              ownerCount:            number;
              pack:                  ItemOrNull;
              party:                 PartyOrNull;
              partyLootable:         boolean;
              pathTargX:             number;
              pathTargY:             number;
              permanentMagicReflect: boolean;
              petCount:              number;
              playTime:              number;
              poisonStrength:        number;
              priv:                  number;
              raceGate:              number;
              reAttackAt:            number;
              sayColour:             number;
              setPeace:              number;
              singClickSer:          boolean;
              skillCaps:             SkillsCap;
              skillLock:             SkillsLock;
              skills:                Skills;
              skillsused:            SkillsUsed;
              skillToPeace:          number;
              skillToProv:           number;
              skillToTame:           number;
              socket:                SocketOrNull;
              spattack:              number;
              spdelay:               number;
              spellCast:             number;
              split:                 number;
              splitchance:           number;
              squelch:               number;
              stabled:               boolean;
              stamina:               number;
              stealth:               number;
              tamed:                 boolean;
              tamedHungerRate:       number;
              tamedThirstRate:       number;
              target:                CharOrNull;
              tempdex:               number;
              tempint:               number;
              tempstr:               number;
              thirst:                number;
              thirstRate:            number;
              thirstWildChance:      number;
              town:                  TownRegionOrNull;
              townPriv:              number;
              trainer:               boolean;
              vulnerable:            boolean;
              wandertype:            number;
              willhunger:            boolean;
              willthirst:            boolean;
  }
  type UOXChar_class = Character;
  type CharOrItem = Character | Item;
  type CharOrNull = Character | null | undefined;
  type CharOrSocket = Character | Socket;
  enum ClientFeature {
    CF_BIT_CHAT = 0,        // 0x01
    CF_BIT_UOR,             // 0x02
    CF_BIT_TD,              // 0x04
    CF_BIT_LBR,             // 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
    CF_BIT_AOS,             // 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works for 4.0+
    CF_BIT_SIXCHARS,        // 0x20 - Enable sixth character slot
    CF_BIT_SE,              // 0x40
    CF_BIT_ML,              // 0x80 - Elven race, new spells, skills + housing tiles
    CF_BIT_EIGHTAGE,        // 0x100 - Splash screen for 8th age
    CF_BIT_NINTHAGE,        // 0x200 - Splash screen for 9th age
    CF_BIT_TENTHAGE,        // 0x400 - Splash screen for 10th age - crystal/shadow house tiles
    CF_BIT_UNKNOWN1,        // 0x800 - Increased housing/bank storage (6.0.3.0 or earlier)
    CF_BIT_SEVENCHARS,      // 0x1000 - Enable seventh character slot
    // CF_BIT_KRFACES,      // 0x2000 - KR release (6.0.0.0)
    // CF_BIT_TRIAL,        // 0x4000 - Trial account
    CF_BIT_EXPANSION = 15,  // 0x8000 - Live account
    CF_BIT_SA,              // 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles - clients 6.0.14.2+
    CF_BIT_HS,              // 0x20000 - Enable HS features: boats, new movementtype? ++
    CF_BIT_GOTHHOUSE,       // 0x40000
    CF_BIT_RUSTHOUSE,       // 0x80000
    CF_BIT_JUNGLEHOUSE,     // 0x100000 - Enable Jungle housing tiles
    CF_BIT_SHADOWHOUSE,     // 0x200000 - Enable Shadowguard housing tiles
    CF_BIT_TOLHOUSE,        // 0x400000 - Enable Time of Legends features
    CF_BIT_ENDLESSHOUSE,    // 0x800000 - Enable Endless Journey account
    CF_BIT_COUNT,          
  }
  enum CommandLevels {
    CL_PLAYER = 0,
    CL_CNS,      
    CL_GM,       
    CL_ADMIN,    
  }
  interface Console {
              logEcho: number;
              mode:    number;
    BeginShutdown(): void;
    ClearScreen(): void;
    Error( value: string ): void;
    Log( value: string, filename?: string ): void;
    MoveTo( x: number, y: number ): void;
    Print( value: string ): void;
    PrintBasedOnVal( value: boolean ): void;
    PrintDone(): void;
    PrintFailed(): void;
    PrintPassed(): void;
    PrintSectionBegin(): void;
    PrintSpecial( color: number, msg: string ): void;
    Reload( value: 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 ): void;
    TurnBlue(): void;
    TurnBrightWhite(): void;
    TurnGreen(): void;
    TurnNormal(): void;
    TurnRed(): void;
    TurnYellow(): void;
    Warning( value: string ): void;
  }
  type UOXConsole = Console;
  interface CreateEntries {
  }
  type UOXCreateEntries = CreateEntries;
  interface CreateEntry {
    readonly  addItem:     string;
    readonly  avgMaxSkill: number;
    readonly  avgMinSkill: number;
    readonly  colour:      number;
    readonly  delay:       number;
    readonly  id:          number;
    readonly  maxRank:     number;
    readonly  minRank:     number;
    readonly  name:        string;
    readonly  resources:   ResourceAmount[];
    readonly  skills:      ResourceSkill[];
    readonly  sound:       number;
    readonly  spell:       number;
  }
  type UOXCreateEntry = CreateEntry;
  interface Guild {
              abbreviation: string;
              charter:      string;
              master:       CharOrNull;
              name:         string;
              numMembers:   number;
              numRecruits:  number;
              stone:        ItemOrNull;
              type:         GuildTypes;
              webPage:      string;
    AcceptRecruit( recruit: Character ): void;
    IsAtPeace(): boolean;
  }
  type UOXGuild_class = Guild;
  type GuildOrNull = Guild | null | undefined;
  enum GuildTypes {
    GT_STANDARD = 0,
    GT_ORDER,      
    GT_CHAOS,      
    GT_UNKNOWN,    
  }
  interface Item extends BaseObject {
              ac:                number;
              ammoFX:            number;
              ammoFXHue:         number;
              ammoFXRender:      number;
              ammoHue:           number;
              ammoID:            number;
              amount:            number;
              artifactRarity:    number;
              att:               number;
              baseRange:         number;
              baseWeight:        number;
              buyvalue:          number;
    readonly  canBeLockedDown:   boolean;
              carveSection:      number;
              container:         BaseOrNull;
              corpse:            boolean;
              creator:           number;
              damageCold:        boolean;
              damageHeat:        boolean;
              damageLight:       boolean;
              damageLightning:   boolean;
              damagePoison:      boolean;
              damageRain:        boolean;
              damageSnow:        boolean;
              decayable:         boolean;
              decaytime:         number;
              def:               number;
              desc:              string;
              dir:               number;
              divinelock:        boolean;
              durabilityHpBonus: number;
              entryMadeFrom:     number;
              event:             string;
              good:              number;
              healthBonus:       number;
              healthLeech:       number;
    readonly  isContType:        boolean;
              isDamageable:      boolean;
              isDoorOpen:        boolean;
              isDyeable:         boolean;
    readonly  isFieldSpell:      boolean;
              isItemHeld:        boolean;
    readonly  isLeatherType:     boolean;
    readonly  isLockedDown:      boolean;
              isMarkedByMaker:   boolean;
    readonly  isMetalType:       boolean;
              isNewbie:          boolean;
              isPileable:        boolean;
    readonly  isShieldType:      boolean;
              isWipeable:        boolean;
              itemsinside:       number;
              layer:             number;
              lodamage:          number;
              lowerStateReq:     number;
              madeWith:          number;
              manaBonus:         number;
              manaLeech:         number;
              maxItems:          number;
              maxRange:          number;
              maxUses:           number;
              more:              number;
              more0:             number;
              more1:             number;
              more2:             number;
              morex:             number;
              morey:             number;
              morez:             number;
              movable:           number;
              name2:             string;
              rank:              number;
              resistCold:        number;
              resistHeat:        number;
              resistLight:       number;
              resistLightning:   number;
              resistPoison:      number;
              resistRain:        number;
              resistSnow:        number;
              restock:           number;
              sellvalue:         number;
              shouldSave:        boolean;
              speed:             number;
              staminaBonus:      number;
              staminaLeech:      number;
              stealable:         number;
              tempLastTraded:    number;
              tempTimer:         number;
              totalItemCount:    number;
              type:              number;
              usesLeft:          number;
              vendorPrice:       number;
              weightMax:         number;
              wipable:           number;
  }
  type UOXItem_class = Item;
  type ItemOrNull = Item | null | undefined;
  type ItemOrSerial = Item | number;
  interface MiningData {
              colour:    number;
              makemenu:  number;
              minSkill:  number;
              name:      string;
              oreChance: number;
              oreName:   string;
              scriptID:  number;
  }
  type MiningData_st = MiningData;
  interface Multi extends Item {
              bans:                number;
              banX:                number;
              banY:                number;
              buildTimestamp:      string;
              deed:                string;
              friends:             number;
              guests:              number;
              isPublic:            boolean;
    readonly  lockdowns:           number;
              maxBans:             number;
              maxFriends:          number;
              maxGuests:           number;
              maxLockdowns:        number;
              maxOwners:           number;
              maxSecureContainers: number;
              maxTrashContainers:  number;
              maxVendors:          number;
              moveType:            number;
              owners:              number;
              secureContainers:    number;
              tithing:             number;
              tradeTimestamp:      string;
              trashContainers:     number;
              vendors:             number;
  }
  type MultiOrNull = Multi | null | undefined;
  class Packet {
    Free(): void;
    ReserveSize( len: number ): void;
    WriteByte( position: number, value: number ): void;
    WriteLong( position: number, value: number ): void;
    WriteShort( position: number, value: number ): void;
    WriteString( position: number, value: string, len: number ): void;
    constructor();
  }
  interface Party {
              isNPC:       boolean;
              leader:      CharOrNull;
              memberCount: number;
    Add( toAdd: Character ): boolean;
    GetMember( index: number ): CharOrNull;
    Remove( toRemove: Character ): boolean;
  }
  type UOXParty_class = Party;
  type PartyOrNull = Party | null | undefined;
  interface Race {
              armourClass:      number;
              genderRestrict:   number;
    readonly  id:               number;
              isPlayerRace:     boolean;
              languageSkillMin: number;
              magicResistance:  number;
              name:             string;
              nightVision:      number;
              poisonResistance: number;
              requiresBeard:    boolean;
              requiresNoBeard:  boolean;
              visibleDistance:  number;
    CanWearArmour( item: Item ): boolean;
    IsValidBeardColour( colour: number ): boolean;
    IsValidHairColour( colour: number ): boolean;
    IsValidSkinColour( colour: number ): boolean;
  }
  type UOXRace_class = Race;
  type RaceOrNull = Race | null | undefined;
  type ResourceAmount = [number, number, number[]];
  type ResourceSkill = [number, number, number];
  interface SCRIPT {
  }
  type uox_class = SCRIPT;
  enum ServerFeature {
    SF_BIT_UNKNOWN1 = 0,    // 0x01
    SF_BIT_IGR,             // 0x02
    SF_BIT_ONECHAR,         // 0x04 - One char only, Siege-server style
    SF_BIT_CONTEXTMENUS,    // 0x08
    SF_BIT_LIMITCHAR,       // 0x10 - Limit amount of chars, combine with OneChar
    SF_BIT_AOS,             // 0x20 - Enable Tooltips, fight system book - but not monsters/map/skills/necro/pala classes
    SF_BIT_SIXCHARS,        // 0x40 - Use 6 character slots instead of 5 (4.0.3a)
    SF_BIT_SE,              // 0x80 - Samurai and Ninja classes, bushido, ninjitsu (4.0.5a)
    SF_BIT_ML,              // 0x100 - Elven race, spellweaving (4.0.11d)
    SF_BIT_UNKNOWN2,        // 0x200 - added with UO:KR launch (6.0.0.0)
    SF_BIT_SEND3DTYPE,      // 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
    SF_BIT_UNKNOWN4,        // 0x800 - added sometime between UO:KR and UO:SA
    SF_BIT_SEVENCHARS,      // 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
    SF_BIT_UNKNOWN5,        // 0x2000 - added with UO:SA launch, imbuing, mysticism, throwing? (7.0.0.0)
    SF_BIT_NEWMOVE,         // 0x4000 - new movement system (packets 0xF0, 0xF1, 0xF2))
    SF_BIT_FACTIONAREAS = 15, // 0x8000 - Unlock new Felucca faction-areas (map0x.mul?)
    SF_BIT_COUNT,           // 
  }
  interface Skill {
    readonly  dexterity:    number;
    readonly  intelligence: number;
    readonly  madeWord:     string;
    readonly  name:         string;
    readonly  scriptID:     number;
    readonly  skillDelay:   number;
    readonly  strength:     number;
  }
  type UOXGlobalSkill = Skill;
  interface Skills extends CertainSetOf {
  }
  type UOXSkills = Skills;
  interface SkillsCap extends CertainSetOf {
  }
  type UOXSkillsCap = SkillsCap;
  interface SkillsLock extends CertainSetOf {
  }
  type UOXSkillsLock = SkillsLock;
  interface SkillsUsed extends CertainSetOf {
  }
  type UOXSkillsUsed = SkillsUsed;
  interface Socket {
    readonly  account:          Account | undefined | null;
    readonly  bytesReceived:    number;
    readonly  bytesSent:        number;
              clickX:           number;
              clickY:           number;
              clickZ:           number;
              clientLetterVer:  number;
              clientMajorVer:   number;
              clientMinorVer:   number;
              clientSubVer:     number;
              clientType:       number;
              cryptClient:      boolean;
              currentChar:      Character | undefined | null;
              currentSpellType: number;
              firstPacket:      boolean;
              language:         number;
              logging:          boolean;
              newClient:        boolean;
    readonly  pickupSerial:     number;
              pickupSpot:       number;
              pickupX:          number;
              pickupY:          number;
              pickupZ:          number;
    readonly  target:           BaseObject | undefined | null;
              targetOK:         boolean;
              tempInt:          number;
              tempInt2:         number;
              tempObj:          BaseObject | undefined | null;
              tempObj2:         BaseObject | undefined | null;
              walkSequence:     number;
              wasIdleWarned:    boolean;
              xText:            string;
              xText2:           string;
    BuyFrom( npc: Character ): void;
    CanSee( target: Socket | Character | Item ): boolean;
    CanSee( x: number, y: number, z: number ): boolean;
    CloseGump( gumpId: number, buttonId: number ): void;
    CustomTarget( tNum: number, toSay?: string, cursorType?: number ): void;
    Disconnect(): void;
    DisplayDamage( toon: Character, damage: number ): void;
    FinishedTriggerWords(): boolean;
    FirstTriggerWord(): number;
    GetByte( offset: number ): number;
    GetDWord( offset: number ): number;
    GetSByte( offset: number ): number;
    GetSDWord( offset: number ): number;
    GetString( offset: number, length?: number ): string;
    GetSWord( offset: number ): number;
    GetTimer( timerID: SocketTimerID ): number;
    GetWord( offset: number ): number;
    MakeMenu( menuId: number, skillNum: number ): void;
    Music( songId: number ): void;
    NextTriggerWord(): number;
    OpenContainer( container: ItemOrNull ): void;
    OpenGump( menuNumber: number ): void;
    OpenURL( url: string ): void;
    Page( pageType: number ): void;
    PopUpTarget( tNum: number, toSay?: string ): void;
    ReadBytes( length: number ): void;
    SellTo( npc: Character ): void;
    Send( packet: Packet ): void;
    SendAddMenu( menuId: number ): void;
    SetByte( offset: number, value: number ): void;
    SetDWord( offset: number, value: number ): void;
    SetString( offset: number, value: string ): void;
    SetTimer( timerID: SocketTimerID, value: number ): void;
    SetWord( offset: number, value: number ): void;
    SoundEffect( soundId: number, allHear: boolean, tmpMonsterSound?: number ): void;
    SysMessage( color: number, msg: string, parm1?: string, parm2?: string, parm3?: string, parm4?: string, parm5?: string, parm6?: string, parm7?: string, parm8?: string ): void;
    SysMessage( msg: string, parm1?: string, parm2?: string, parm3?: string, parm4?: string, parm5?: string, parm6?: string, parm7?: string, parm8?: string, parm9?: string ): void;
    WhoList( forceRefresh?: boolean ): void;
  }
  type SocketOrNull = Socket | null | undefined;
  enum SocketTimerID {
  }
  interface SpawnItem extends Item {
              maxinterval:  number;
              mininterval:  number;
              sectionalist: number;
              spawnsection: string;
  }
  interface SpawnRegion {
  }
  type UOXSpawnRegion = SpawnRegion;
  interface Spell {
    readonly  action:          number;
    readonly  aggressiveSpell: boolean;
    readonly  ash:             number;
    readonly  baseDmg:         number;
    readonly  batwing:         number;
    readonly  circle:          number;
    readonly  daemonBlood:     number;
    readonly  damageDelay:     number;
    readonly  delay:           number;
    readonly  drake:           number;
    readonly  enabled:         boolean;
    readonly  fieldSpell:      boolean;
    readonly  garlic:          number;
    readonly  ginseng:         number;
    readonly  graveDust:       number;
    readonly  health:          number;
    readonly  highSkill:       number;
    readonly  id:              number;
    readonly  lowSkill:        number;
    readonly  mana:            number;
    readonly  mantra:          string;
    readonly  moss:            number;
    readonly  name:            string;
    readonly  noxCrystal:      number;
    readonly  pearl:           number;
    readonly  pigIron:         number;
    readonly  recoveryDelay:   number;
    readonly  reflectable:     boolean;
    readonly  requireChar:     boolean;
    readonly  requireItem:     boolean;
    readonly  requireLocation: boolean;
    readonly  requireTarget:   boolean;
    readonly  resistable:      boolean;
    readonly  scrollHigh:      number;
    readonly  scrollLow:       number;
    readonly  shade:           number;
    readonly  silk:            number;
    readonly  soundEffect:     number;
    readonly  stamina:         number;
    readonly  strToSay:        string;
    readonly  tithing:         number;
    readonly  travelSpell:     boolean;
  }
  type UOXSpell = Spell;
  enum TileFlags {
    TF_FLOORLEVEL = 0,  // Background
    TF_HOLDABLE,        // Weapon
    TF_TRANSPARENT,     // SignGuildBanner
    TF_TRANSLUCENT,     // WebDirtBlood
    TF_WALL,            // WallVertTile
    TF_DAMAGING,       
    TF_BLOCKING,        // Impassable
    TF_WET,             // LiquidWet
    TF_UNKNOWN1,        // Ignored
    TF_SURFACE,         // Standable
    TF_CLIMBABLE,       // Bridge
    TF_STACKABLE,       // Generic
    TF_WINDOW,          // WindowArchDoor
    TF_NOSHOOT,         // CannotShootThru
    TF_DISPLAYA,        // Prefix A
    TF_DISPLAYAN,       // Prefix An
    TF_DESCRIPTION,     // Internal
    TF_FOLIAGE,         // FadeWithTrans
    TF_PARTIALHUE,     
    TF_UNKNOWN2,       
    TF_MAP,            
    TF_CONTAINER,      
    TF_WEARABLE,        // Equipable
    TF_LIGHT,           // LightSource
    TF_ANIMATED,       
    TF_NODIAGONAL,      // 'HoverOver' in SA clients and later, to determine if tiles can be moved on by flying gargoyles
    TF_UNKNOWN3,        // 'NoDiagonal' in SA clients and later?
    TF_ARMOR,           // WholeBodyItem
    TF_ROOF,            // WallRoofWeap
    TF_DOOR,           
    TF_STAIRBACK,       // ClimbableBit1
    TF_STAIRRIGHT,      // ClimbableBit2
    TF_ALPHABLEND,      // Blend Alphas, tile blending
    TF_USENEWART,       // Uses new art style?
    TF_ARTUSED,         // Is art being used?
    TF_BIT36,           // Unknown/Unused
    TF_NOSHADOW,        // Disallow shadow on this tile, lightsource? lava?
    TF_PIXELBLEED,      // Let pixels bleed in to other tiles? Is this Disabling Texture Clamp?
    TF_PLAYANIMONCE,    // Play tile animation once.
    TF_BIT40,           // Unknown/Unused
    TF_MULTIMOVABLE,    // Movable multi? Cool ships and vehicles etc?
    TF_COUNT,          
  }
  interface Timer {
  }
  type UOXTimer = Timer;
  interface TownRegion {
              appearance:        number;
              canCastAggressive: boolean;
              canGate:           boolean;
              canMark:           boolean;
              canPlaceHouse:     boolean;
              canRecall:         boolean;
              canTeleport:       boolean;
              chanceBigOre:      number;
              health:            number;
              id:                number;
              instanceID:        number;
              isDisabled:        boolean;
              isDungeon:         boolean;
              isGuarded:         boolean;
              isSafeZone:        boolean;
              mayor:             CharOrNull;
    readonly  members:           string;
              music:             number;
              name:              string;
              numGuards:         number;
    readonly  numOrePrefs:       number;
              owner:             string;
    readonly  population:        number;
              race:              number;
              reserves:          number;
              scriptTrigger:     number;
              scriptTriggers:    number[];
              tax:               number;
              taxes:             number;
              taxResource:       number;
              weather:           number;
              worldNumber:       number;
    AddScriptTrigger( scriptId: number ): void;
    GetOreChance(): void;
    GetOrePref( oreId: number ): MiningData;
    RemoveScriptTrigger( scriptId: number ): void;
  }
  type UOXRegion = TownRegion;
  type TownRegionOrNull = TownRegion | null | undefined;
  enum UnicodeTypes {
    ZERO = 0,          
    UT_ARA,            
    UT_ARI,            
    UT_ARE,            
    UT_ARL,            
    UT_ARG,            
    UT_ARM,            
    UT_ART,            
    UT_ARO,            
    UT_ARY,            
    UT_ARS,            
    UT_ARJ,            
    UT_ARB,            
    UT_ARK,            
    UT_ARU,            
    UT_ARH,            
    UT_ARQ,            
    UT_BGR,            
    UT_CAT,            
    UT_CHT,            
    UT_CHS,            
    UT_ZHH,            
    UT_ZHI,            
    UT_ZHM,            
    UT_CSY,            
    UT_DAN,            
    UT_DEU,            
    UT_DES,            
    UT_DEA,            
    UT_DEL,            
    UT_DEC,            
    UT_ELL,            
    UT_ENU,            
    UT_ENG,            
    UT_ENA,            
    UT_ENC,            
    UT_ENZ,            
    UT_ENI,            
    UT_ENS,            
    UT_ENJ,            
    UT_ENB,            
    UT_ENL,            
    UT_ENT,            
    UT_ENW,            
    UT_ENP,            
    UT_ESP,            
    UT_ESM,            
    UT_ESN,            
    UT_ESG,            
    UT_ESC,            
    UT_ESA,            
    UT_ESD,            
    UT_ESV,            
    UT_ESO,            
    UT_ESR,            
    UT_ESS,            
    UT_ESF,            
    UT_ESL,            
    UT_ESY,            
    UT_ESZ,            
    UT_ESB,            
    UT_ESE,            
    UT_ESH,            
    UT_ESI,            
    UT_ESU,            
    UT_FIN,            
    UT_FRA,            
    UT_FRB,            
    UT_FRC,            
    UT_FRS,            
    UT_FRL,            
    UT_FRM,            
    UT_HEB,            
    UT_HUN,            
    UT_ISL,            
    UT_ITA,            
    UT_ITS,            
    UT_JPN,            
    UT_KOR,            
    UT_NLD,            
    UT_NLB,            
    UT_NOR,            
    UT_NON,            
    UT_PLK,            
    UT_PTB,            
    UT_PTG,            
    UT_ROM,            
    UT_RUS,            
    UT_HRV,            
    UT_SRL,            
    UT_SRB,            
    UT_SKY,            
    UT_SQI,            
    UT_SVE,            
    UT_SVF,            
    UT_THA,            
    UT_TRK,            
    UT_URP,            
    UT_IND,            
    UT_UKR,            
    UT_BEL,            
    UT_SLV,            
    UT_ETI,            
    UT_LVI,            
    UT_LTH,            
    UT_LTC,            
    UT_FAR,            
    UT_VIT,            
    UT_HYE,            
    UT_AZE,            
    UT_EUQ,            
    UT_MKI,            
    UT_AFK,            
    UT_KAT,            
    UT_FOS,            
    UT_HIN,            
    UT_MSL,            
    UT_MSB,            
    UT_KAZ,            
    UT_SWK,            
    UT_UZB,            
    UT_TAT,            
    UT_BEN,            
    UT_PAN,            
    UT_GUJ,            
    UT_ORI,            
    UT_TAM,            
    UT_TEL,            
    UT_KAN,            
    UT_MAL,            
    UT_ASM,            
    UT_MAR,            
    UT_SAN,            
    UT_KOK,            
    TOTAL_LANGUAGES,   
  }
  interface UOXResource {
              fishAmount: number;
              fishTime:   number;
              logAmount:  number;
              logTime:    number;
              oreAmount:  number;
              oreTime:    number;
  }
  type UOXResource_class = UOXResource;
  enum VisibleTypes {
    VT_VISIBLE = 0,    // Visible to All
    VT_TEMPHIDDEN,     // Hidden (With Skill) / Visible To Item Owner
    VT_INVISIBLE,      // Magically Hidden
    VT_PERMHIDDEN,     // Permanent Hidden (GM Hidden)
    VT_GHOSTHIDDEN,    // Hidden because the player is a ghost
  }
  enum WeatherType {
    NONE = 0,    
    PHYSICAL,    
    LIGHT,       
    RAIN,        
    COLD,        
    HEAT,        
    LIGHTNING,   
    POISON,      
    SNOW,        
    STORM,       
    STORMBREW,   
    WEATHNUM,    
  }

// Global Functions

  function ApplyDamageBonuses( damageType: WeatherType, attacker: Character, defender: Character, fightSkill: number, hitLoc: number, baseDamage: number ): number;
  function ApplyDefenseModifiers( damageType: WeatherType, attacker: Character, defender: Character, fightSkill: number, hitLoc: number, baseDamage: number, doArmorDamage: boolean ): number;
  function AreaCharacterFunction( inFunction: string, source: Character, range: number, sock?: Socket ): number;
  function AreaItemFunction( inFunction: string, source: Item, range: number, sock?: Socket ): number;
  function BASEITEMSERIAL(): number;
  function BroadcastMessage( message: string ): void;
  function CalcCharFromSer( serial: number ): Character;
  function CalcCharFromSer( byte1: number, byte2: number, byte3: number, byte4: number ): Character;
  function CalcItemFromSer( serial: number ): Item;
  function CalcItemFromSer( byte1: number, byte2: number, byte3: number, byte4: number ): Item;
  function CalcMultiFromSer( serial: number ): Multi;
  function CalcMultiFromSer( byte1: number, byte2: number, byte3: number, byte4: number ): Multi;
  function CalcTargetedChar( src: Socket ): CharOrNull;
  function CalcTargetedItem( src: Socket ): ItemOrNull;
  function CheckDynamicFlag( x: number, y: number, z: number, world: number, instanceId: number, toCheck: TileFlags ): boolean;
  function CheckInstaLog( x: number, y: number, world: number, instanceId: number ): boolean;
  function CheckStaticFlag( x: number, y: number, z: number, world: number, toCheck: TileFlags ): boolean;
  function CheckTileFlag( tileId: number, toCheck: TileFlags ): boolean;
  function CheckTimeSinceLastCombat( toon: Character, timespanSecs: number ): boolean;
  function CommandExists( command: string ): boolean;
  function CommandLevelReq( command: string ): number;
  function CompareGuildByGuild( guild1: number, guild2: number ): number;
  function CreateBaseMulti( multiId: number, x: number, y: number, z: number, worldNum?: number, instanceId?: number, color?: number, checkLocation?: boolean ): Multi;
  function CreateBlankItem( sock: SocketOrNull, toon: CharOrNull, itemAmount: number, name: string, itemId: number, color: number, objType: string, inPack: boolean ): Item;
  function CreateDFNItem( sock: SocketOrNull, toon: CharOrNull, sectNumber: string, itemAmount?: number, itemType?: string, inPack?: boolean, color?: number, worldNum?: number, instanceId?: number ): Item;
  function CreateHouse( houseId: number, x: number, y: number, z: number, worldNum?: number, instanceId?: number, color?: number, checkLocation?: boolean ): Multi;
  function CreateParty( source: CharOrSocket ): Party;
  function DeleteFile( file: string, folder?: string, scriptDataDir?: boolean ): boolean;
  function DisableCommand( command: string ): void;
  function DisableConsoleFunc( funcToDisable: string ): void;
  function DisableKey( key: number ): void;
  function DisableSpell( spellNumber: number ): void;
  function DistanceBetween( src: CharOrItem, trg: CharOrItem, checkZ?: boolean ): number;
  function DistanceBetween( srcX: number, srcY: number, trgX: number, trgY: number ): number;
  function DistanceBetween( srcX: number, srcY: number, srcZ: number, trgX: number, trgY: number, trgZ: number ): number;
  function DoesCharacterBlock( x: number, y: number, z: number, world: number, instanceId: number ): boolean;
  function DoesDynamicBlock( x: number, y: number, z: number, world: number, instanceId: number, checkWater: boolean, waterWalk: boolean, checkOnlyMultis: boolean, checkOnlyNonMultis: boolean ): boolean;
  function DoesEventExist( scriptId: number, eventToFire: string ): boolean;
  function DoesMapBlock( x: number, y: number, z: number, world: number, checkWater: boolean, waterWalk: boolean, checkMultiPlacement: boolean, checkForRoad: boolean ): boolean;
  function DoesStaticBlock( x: number, y: number, z: number, world: number, checkWater: boolean ): boolean;
  function DoMovingEffect( src: CharOrItem, trg: CharOrItem, effectId: number, speed: number, loop: number, explode: boolean, hue?: number, renderMode?: number ): void;
  function DoMovingEffect( src: CharOrItem, trgX: number, trgY: number, trgZ: number, effectId: number, speed: number, loop: number, explode: boolean, hue?: number, renderMode?: number ): void;
  function DoMovingEffect( srcX: number, srcY: number, srcZ: number, trgX: number, trgY: number, trgZ: number, effectId: number, speed: number, loop: number, explode: boolean, hue?: number, renderMode?: number ): void;
  function DoStaticEffect( x: number, y: number, z: number, effectId: number, speed: number, loop: number, explode: boolean ): void;
  function DoTempEffect( iType: number, src: CharOrNull, dst: CharOrItem, targNum: number, more1: number, more2: number, more3: number, item?: Item ): void;
  function EnableCommand( command: string ): void;
  function EnableConsoleFunc( funcToEnable: string ): void;
  function EnableKey( key: number ): void;
  function EnableSpell( spellNumber: number ): void;
  function EraStringToNum( era: string ): number;
  function FindItem( x: number, y: number, z: number, worldNum: number, id: number, instanceId?: number ): ItemOrNull;
  function FindMulti( src: CharOrItem ): void;
  function FindMulti( x: number, y: number, z: number, worldNum: number, instanceId?: number ): void;
  function FindRootContainer( src: ItemOrSerial, type: number ): Item;
  function FinishedCommandList(): boolean;
  function FirstCommand(): string;
  function GetAccountCount(): number;
  function GetCharacterCount(): number;
  function GetClientFeature( feature: ClientFeature ): boolean;
  function GetCurrentClock(): number;
  function GetDay(): number;
  function GetDictionaryEntry( entry: number, lang?: UnicodeTypes ): string;
  function GetHour(): number;
  function GetItem( x: number, y: number, z: number, worldNum: number, instanceId?: number ): ItemOrNull;
  function GetItemCount(): number;
  function GetMapElevation( x: number, y: number, world: number ): number;
  function GetMinute(): number;
  function GetMoongateFacetStatus( moongateFacet: number ): number;
  function GetMultiCount(): number;
  function GetMurderThreshold(): number;
  function GetPackOwner( src: ItemOrSerial, type: number ): Character;
  function GetPlayerCount(): number;
  function GetRaceCount(): number;
  function GetRaceSkillAdjustment( raceID: number, skillID: number ): number;
  function GetRandomSOSArea( worldNum: number, instanceId: number ): number[];
  function GetServerFeature( feature: ServerFeature ): boolean;
  function GetServerSetting( setting: string ): boolean | string | number;
  function GetServerVersionString(): string;
  function GetSocketFromIndex( index: number ): Socket;
  function GetSpawnRegion( xOrIndex: number, y: number, world: number, instance: number ): SpawnRegion;
  function GetSpawnRegionCount(): number;
  function GetSpawnRegionFacetStatus( spawnRegionFacet: number ): number;
  function GetStartTime(): number;
  function GetTileHeight( tileID: number ): number;
  function GetTileIDAtMapCoord( x: number, y: number, worldNum: number ): number;
  function GetTownRegion( id: number ): TownRegion;
  function GetTownRegionFromXY( x: number, y: number, world: number, instance: number ): TownRegion;
  function INVALIDCOLOUR(): number;
  function INVALIDID(): number;
  function INVALIDSERIAL(): number;
  function IsInBuilding( x: number, y: number, z: number, world: number, instanceId: number, checkHeight: boolean ): boolean;
  function IsRaceWeakToWeather( raceID: number, weathType: number ): boolean;
  function IterateOver( objType: string ): number;
  function IterateOverSpawnRegions(): number;
  function MakeItem( mSock: Socket, player: Character, itemMenu: number, resourceColour?: number ): void;
  function Moon( slot: number, newVal?: number ): number;
  function NextCommand(): string;
  function NumToHexString( value: number ): string;
  function NumToString( value: number ): string;
  function PossessTown( town: number, sTown: number ): void;
  function RaceCompareByRace( raceA: number, raceB: number ): number;
  function RandomNumber( low: number, high: number ): number;
  function RegisterCommand( command: string, execLevel: number, isEnabled: boolean ): void;
  function RegisterConsoleFunc( funcToRegister: string, cmdName: string ): void;
  function RegisterKey( key: string, toRegister: string ): void;
  function RegisterKey( key: number, cmdName: string ): void;
  function RegisterPacket( packet: number, subCmd: number ): void;
  function RegisterSkill( skillNumber: number, isEnabled: boolean ): void;
  function RegisterSpell( spellNumber: number, isEnabled: boolean ): void;
  function Reload( toReload: number ): void;
  function ReloadJSFile( scriptId: number ): void;
  function ResourceAmount( resType: string, newVal?: number ): number;
  function ResourceArea( resType: string ): number;
  function ResourceRegion( x: number, y: number, world: number ): UOXResource;
  function ResourceTime( resType: string, newVal?: number ): number;
  function RollDice( dice: number, face: number, add: number ): number;
  function SecondsPerUOMinute( newVal?: number ): number;
  function SendStaticStats( target: Socket ): void;
  function SetMoongateFacetStatus( moongateFacet: number, moongateFacetVal?: boolean ): void;
  function SetSpawnRegionFacetStatus( spawnRegionFacet: number, spawnRegionFacetVal?: boolean ): void;
  function SpawnNPC( npcNum: string, x: number, y: number, z: number, worldNum: number, instanceId?: number, useNpcList?: boolean ): Character;
  function StaticAt( x: number, y: number, worldNum: number, tileId?: number ): boolean;
  function StaticInRange( x: number, y: number, worldNum: number, radius: number, tileId: number ): boolean;
  function StringToNum( value: string ): number;
  function TriggerEvent( scriptId: number, eventToFire: string, ...extra: any[] ): void;
  function TriggerTrap( src: CharOrSocket, item: Item ): void;
  function UseItem( src: CharOrSocket, item: Item ): void;
  function ValidateObject( obj: CharOrItem ): boolean;
  function WillResultInCriminal( source: Character, target: Character ): boolean;
  function WorldBrightLevel( newLevel?: number ): number;
  function WorldDarkLevel( newLevel?: number ): number;
  function WorldDungeonLevel( newLevel?: number ): number;
  function Yell( source: Socket, value: string, commandLevel: CommandLevels ): void;

// Event Functions

  function inRange( srcObj: BaseObject, objInRange: BaseObject ): void;
  function onAICombatTarget( attacker: Character, target: Character ): void;
  function onAISliver( pSliver: Character ): void;
  function onAttack( attacker: Character, defender: Character, hitStatus: boolean, hitLoc: number, damageDealt: number ): void;
  function onBoatTurn( iMulti: Boat, oldDir: number, newDir: number, iTiller: Item ): void;
  function onBoughtFromVendor( targSock: Socket, objVendor: Character, objItemBought: BaseObject, numItemsBought: number ): void;
  function onBuy( targSock: Socket, objVendor: Character ): void;
  function onBuyFromVendor( targSock: Socket, objVendor: Character, objItemBought: BaseObject, numItemsBuying: number ): void;
  function onCallback( tSock: Socket, targeted: number, callNum: number ): void;
  function onCarveCorpse( player: Character, corpse: Item ): void;
  function onCharDoubleClick( currChar: Character, targChar: Character ): void;
  function onClick( sockPlayer: Socket, objClicked: BaseObject ): void;
  function onCollide( targSock: Socket, objColliding: Character, objCollideWith: BaseObject ): void;
  function onCombatDamageCalc( attacker: Character, defender: Character, getFightSkill: number, hitLoc: number ): void;
  function onCombatEnd( attacker: Character, defender: Character ): void;
  function onCombatStart( attacker: Character, defender: Character ): void;
  function onCommand( mSock: Socket, command: string ): void;
  function onContextMenuRequest( tSock: Socket, baseObj: BaseObject ): void;
  function onContextMenuSelect( tSock: Socket, baseObj: BaseObject, popupEntry: number ): void;
  function onContRemoveItem( contItem: Item, item: Item, pickerUpper: Character ): void;
  function onCreateDFN( thingCreated: BaseObject, dfnCreated: boolean, isPlayer: boolean ): void;
  function onCreatePlayer( thingCreated: BaseObject, dfnCreated: boolean, isPlayer: boolean ): void;
  function onCreateTile( thingCreated: BaseObject, dfnCreated: boolean, isPlayer: boolean ): void;
  function onDamage( damaged: Character, attacker: Character, damageValue: number, damageType: WeatherType ): void;
  function onDamageDeal( damaged: Character, attacker: Character, damageValue: number, damageType: WeatherType ): void;
  function onDeath( pDead: Character, iCorpse: Item ): void;
  function onDeathBlow( mKilled: Character, mKiller: Character ): void;
  function onDecay( decaying: Item ): void;
  function onDefense( attacker: Character, defender: Character, hitStatus: boolean, hitLoc: number, damageReceived: number ): void;
  function onDelete( thingDestroyed: BaseObject ): void;
  function onDispel( dispelled: BaseObject ): void;
  function onDrop( item: Item, dropper: Character ): void;
  function onDropItemOnItem( item: Item, dropper: Character, dest: Item ): void;
  function onDropItemOnNpc( srcChar: Character, targChar: Character, i: Item ): void;
  function onDyeTarget( player: Character, dyeTub: Item, target: Item ): void;
  function onEnterEvadeState( npc: Character, enemy: Character ): void;
  function onEnterRegion( entering: Character, region: number ): void;
  function onEntrance( left: Multi, leaving: BaseObject ): void;
  function onEquip( equipper: Character, equipping: Item ): void;
  function onEquipAttempt( equipper: Character, equipping: Item ): void;
  function onFacetChange( mChar: Character, oldFacet: number, newFacet: number ): void;
  function onFall( pFall: Character ): void;
  function onFlagChange( pChanging: Character, newStatus: number, oldStatus: number ): void;
  function onHelpButton( mChar: Character ): void;
  function onHouseCommand( targSock: Socket, multiObj: Multi, targId: number ): void;
  function onHungerChange( pChanging: Character, newStatus: number ): void;
  function onIterate( a: BaseObject, b: number, mSock: Socket ): void;
  function onIterateSpawnRegions( a: SpawnRegion, b: number ): void;
  function onLeaveRegion( entering: Character, region: number ): void;
  function onLeaving( left: Multi, leaving: BaseObject ): void;
  function onLightChange( tObject: BaseObject, lightLevel: number ): void;
  function onLogin( sockPlayer: Socket, pPlayer: Character ): void;
  function onLogout( sockPlayer: Socket, pPlayer: Character ): void;
  function onLoyaltyChange( pChanging: Character, newStatus: number ): void;
  function onMakeItem( mSock: Socket, objChar: Character, objItem: Item, createEntryId: number ): void;
  function onMoveDetect( sourceObj: BaseObject, CharInRange: Character, rangeToChar: number, oldCharX: number, oldCharY: number ): void;
  function onMultiLogout( iMulti: Multi, cPlayer: Character ): void;
  function onNameRequest( myObj: BaseObject, nameRequester: Character, requestSource: number ): void;
  function onPacketReceive( mSock: Socket, packetNum: number ): void;
  function onPathfindEnd( npc: Character, pathfindResult: number ): void;
  function onPickup( item: Item, pickerUpper: Character, objCont: BaseObject ): void;
  function onQuestGump( mChar: Character ): void;
  function onResurrect( pAlive: Character ): void;
  function onScrollCast( tChar: Character, SpellId: number ): void;
  function onScrollingGumpPress( tSock: Socket, gumpId: number, buttonId: number ): void;
  function onSell( targSock: Socket, objVendor: Character ): void;
  function onSellToVendor( targSock: Socket, objVendor: Character, objItemSold: BaseObject, numItemsSelling: number ): void;
  function onSkill( skillUse: BaseObject, skillUsed: number ): void;
  function onSkillChange( player: Character, skill: number, skillAmtChanged: number ): void;
  function onSkillCheck( myChar: Character, skill: number, lowSkill: number, highSkill: number, isCraftSkill: boolean ): void;
  function onSkillGain( player: Character, skill: number, skillAmtGained: number ): void;
  function onSkillGump( mChar: Character ): void;
  function onSkillLoss( player: Character, skill: number, skillAmtLost: number ): void;
  function onSnoopAttempt( snooped: Character, pack: Item, snooper: Character ): void;
  function onSnooped( snooped: Character, snooper: Character, success: boolean ): void;
  function onSoldToVendor( targSock: Socket, objVendor: Character, objItemSold: BaseObject, numItemsSold: number ): void;
  function onSpecialMove( mChar: Character, abilityId: number ): void;
  function onSpeech( speech: string, personTalking: Character, talkingTo: BaseObject ): void;
  function onSpeechInput( myChar: Character, myItem: Item, mySpeech: string ): void;
  function onSpellCast( tChar: Character, SpellId: number ): void;
  function onSpellGain( book: Item, spellNum: number ): void;
  function onSpellLoss( book: Item, spellNum: number ): void;
  function onSpellSuccess( tChar: Character, SpellId: number ): void;
  function onSpellTarget( target: BaseObject, caster: Character, spellNum: number ): void;
  function onSpellTargetSelect( caster: Character, target: BaseObject, spellNum: number ): void;
  function onStart(): void;
  function onStat(): void;
  function onStatChange( player: Character, stat: number, statChangeAmount: number ): void;
  function onStatGain( player: Character, stat: number, skill: number, statGainAmount: number ): void;
  function onStatGained( player: Character, stat: number, skill: number, statGainedAmount: number ): void;
  function onStatLoss( player: Character, stat: number, statLossAmount: number ): void;
  function onSteal( thief: Character, theft: Item, victim: Character ): void;
  function onStolenFrom( stealing: Character, stolenFrom: Character, stolen: Item ): void;
  function onStop(): void;
  function onSwing( swinging: Item, swinger: Character, swingTarg: Character ): void;
  function onSystemSlice(): void;
  function onTalk( myChar: Character, mySpeech: string ): void;
  function onTempChange( tObject: BaseObject, temp: number ): void;
  function onThirstChange( pChanging: Character, newStatus: number ): void;
  function onTimer( tObject: BaseObject, timerId: number ): void;
  function onTooltip( myObj: BaseObject, pSocket: Socket ): void;
  function onUnequip( equipper: Character, equipping: Item ): void;
  function onUnequipAttempt( equipper: Character, equipping: Item ): void;
  function onUseBandageMacro( mSock: Socket, targChar: Character, bandageItem: Item ): void;
  function onUseChecked( user: Character, iUsing: Item ): void;
  function onUseUnChecked( user: Character, iUsing: Item ): void;
  function onVirtueGumpPress( mChar: Character, tChar: Character, buttonId: number ): void;
  function onWarModeToggle( mChar: Character ): void;
  function onWeatherChange( tObject: BaseObject, element: WeatherType ): void;
  function outOfRange( srcObj: BaseObject, objVanish: BaseObject ): void;

// Global Objects

  const Spells: Spell[];
  const Skills: Skill[];
  const Accounts: Accounts;
  const Console: UOXConsole;
  const CreateEntries: UOXCreateEntries;
  const Timer: UOXTimer;
  const SCRIPT: uox_class;
}

// Ensure the file is treated as a module
export {};
