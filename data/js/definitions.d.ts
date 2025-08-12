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
    readonly  isChar:             boolean;
              isDispellable:      boolean;
              isGuarded:          boolean;
    readonly  isItem:             boolean;
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
    AddScriptTrigger( scriptId: number ): void;
    ApplySection( section: string ): void;
    Delete(): void;
    DistanceTo( target: BaseObject ): number;
    FinishedItems(): void;
    FirstItem(): void;
    GetJSTimer( timerId: number, scriptId: number ): number;
    GetNumTags(): void;
    GetSerial( part: 0 | 1 | 2 | 3 | 4 ): number;
    GetTag( tag: string ): string | boolean | number;
    GetTagMap(): void;
    GetTempEffect( effectID: number ): number;
    GetTempTag( tag: string ): string | boolean | number;
    GetTempTagMap(): void;
    HasScriptTrigger( scriptId: number ): boolean;
    InRange( other: BaseObject, distance: number ): boolean;
    KillJSTimer( timerId: number, scriptId: number ): number;
    KillTimers( triggerId?: number ): void;
    NextItem(): void;
    PauseJSTimer( timerId: number, scriptId: number ): number;
    PauseTempEffect( effectID: number ): number;
    Refresh(): void;
    RemoveScriptTrigger( scriptId: number ): void;
    Resist( resistType: WeatherType ): number | boolean;
    Resist( resistType: WeatherType, newValue: number ): number | boolean;
    ResourceCount( realId: number, itemColour?: number, moreVal?: number, sectionId?: string ): void;
    ResumeJSTimer( timerId: number, scriptId: number ): number;
    ResumeTempEffect( effectID: number ): number;
    ReverseTempEffect( effectID: number ): number;
    SetJSTimer( timerId: number, millis: number, scriptId: number ): number;
    SetLocation( targetOrGoPlace: BaseObject | number ): void;
    SetLocation( x: number, y: number, z?: number, world?: number, instance?: number ): void;
    SetRandomColor( colorList: string ): boolean;
    SetRandomName( nameList: string ): boolean;
    SetTag( tag: string, value?: string | number | boolean | null ): void;
    SetTempTag( tag: string, value?: string | number | boolean | null ): void;
    StartTimer( expireTime: number, triggerNum: number, more2?: number | boolean ): void;
    StaticEffect( effectId: number, speed: number, loop: number ): void;
    Teleport( targetOrGoPlace: BaseObject | number ): void;
    Teleport( x: number, y: number, z?: number, world?: number, instance?: number ): void;
    TextMessage( message: string, individual?: boolean | undefined | null, hue?: number, target?: SpeechTarget, targetSerial?: number, font?: FontType, speechType?: SpeechType ): void;
    UpdateStats( statType: number ): void;
    UseResource( amount: number, realId: number, color?: number, moreVal?: number, sectionId?: string ): void;
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
    readonly  isChar:                true;
              isCounselor:           boolean;
              isDisguised:           boolean;
              isFlying:              boolean;
              isGM:                  boolean;
              isGMPageable:          boolean;
              isHuman:               boolean;
              isIncognito:           boolean;
    readonly  isItem:                false;
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
    AddAggressorFlag( target: Character ): void;
    AddFollower( target: Character ): boolean;
    AddFriend( target: Character ): boolean;
    AddPermaGreyFlag( target: Character ): void;
    AddSkill( skillId: number, skillAmt: number, triggerEvent: boolean ): void;
    AddSpell( spellNum: number ): void;
    BoltEffect(): void;
    BreakConcentration(): void;
    BuyFrom( npc: Character ): void;
    CalculateControlChance( target: Character ): number;
    CanSee( target: Socket | Character | Item ): boolean;
    CanSee( x: number, y: number, z: number ): boolean;
    CastSpell( spellNum: number, immediate?: boolean ): boolean | void;
    CheckAggressorFlag( target: Character ): boolean;
    CheckPermaGreyFlag( target: Character ): boolean;
    CheckSkill( skillId: number, minSkill: number, maxSkill: number, isCraftSkill?: boolean, forceResult?: number ): boolean;
    ClearAggressorFlags(): void;
    ClearFriendList(): void;
    ClearPermaGreyFlags(): void;
    CustomTarget( tNum: number, toSay?: string, cursor?: number ): void;
    Damage( damage: number, element?: WeatherType, attacker?: Character | null | undefined, doRepSys?: boolean ): void;
    Defense( hitLoc: number, resistType: boolean, doArmorDamage: WeatherType, ignoreMedable?: boolean, includeShield?: boolean ): number;
    DirectionTo( target: BaseObject ): number;
    DirectionTo( x: number, y: number ): number;
    Dismount(): void;
    DoAction( action: number, subAction?: number, frameCount?: number, frameDelay?: number, playBackwards?: boolean ): void;
    Dupe(): void;
    EmoteMessage( msg: string, allSay?: boolean, hue?: number, target?: SpeechTarget | null, serial?: number ): void;
    ExecuteCommand( msg: string ): void;
    ExplodeItem( target: Item, damage: number, dmgType: number, explodeNearby?: boolean ): void;
    FindItemLayer( layerId: number ): Item;
    FindItemSection( sectionId: string ): ItemOrNull;
    FindItemType( typeId: ItemTypes ): ItemOrNull;
    Follow( target: CharOrNull ): void;
    Gate( target: Item ): void;
    Gate( place: number ): void;
    Gate( x: number, y: number, z: number, world: number, instance?: number ): void;
    GetFollowerList(): Character[];
    GetFriendList(): Character[];
    GetPetList(): Character[];
    GetTimer( timerID: SocketTimerID ): number;
    HasBeenOwner( toCheck: Character ): boolean;
    HasSpell( spellId: number ): boolean;
    Heal( amount: number, healer?: CharOrNull ): void;
    InitiateCombat( target: Character ): boolean;
    InitWanderArea(): void;
    InvalidateAttacker(): void;
    IsAggressor( playerOnly?: boolean ): boolean;
    IsPermaGrey( playerOnly?: boolean ): boolean;
    Jail( numSecs?: number ): void;
    Kill(): void;
    MagicEffect( spellId: number ): void;
    MakeMenu( menu: number, skillNum: number ): void;
    Mark( toMark: Item ): void;
    OpenBank( target?: Socket ): void;
    OpenLayer( target: Socket, layer: number ): void;
    PopUpTarget( tNum: number, toSay?: string ): void;
    ReactOnDamage( dmgType: WeatherType, attacker?: Character ): void;
    Recall( tablet: Item ): void;
    Release(): void;
    RemoveAggressorFlag( target: Character ): void;
    RemoveFollower( target: Character ): void;
    RemoveFriend( target: Character ): void;
    RemovePermaGreyFlag( target: Character ): void;
    RemoveSpell( spellId: number ): void;
    Resurrect(): void;
    RunTo( target: Item, maxSteps: number ): void;
    RunTo( x: number, y: number, maxSteps: number, allowPartial?: boolean, ignoreDoors?: boolean ): void;
    SellTo( target: Character ): boolean;
    SetInvisible( value: VisibleTypes, timeout?: number ): void;
    SetLocation(): void;
    SetPoisoned( poisonLevel: number, wearOff?: number, poisoner?: Character ): void;
    SetSkillByName( skill: string, value: number ): void;
    SetTimer( timerID: SocketTimerID, value: number ): void;
    SoundEffect( soundId: number, allHear: boolean, monsterId?: number ): void;
    SpeechInput( speechId: number, talking?: Item ): void;
    SpellFail(): void;
    SpellMoveEffect( spell: Spell, target: Character ): void;
    SpellStaticEffect( spell: Spell ): void;
    SysMessage( value1: string, value2?: string, value3?: string, value4?: string, value5?: string, value6?: string, value7?: string, value8?: string, value9?: string, value10?: string ): void;
    SysMessage( msgColor: number, value1: string, value2?: string, value3?: string, value4?: string, value5?: string, value6?: string, value7?: string, value8?: string, value9?: string ): void;
    Teleport(): void;
    TurnToward( target: BaseObject ): void;
    TurnToward( x: number, y: number ): void;
    UpdateAggressorFlagTimestamp( target: Character ): void;
    UpdatePermaGreyFlagTimestamp( target: Character ): void;
    WalkTo( target: Item, maxSteps: number ): void;
    WalkTo( x: number, y: number, maxSteps: number, allowPartial?: boolean, ignoreDoors?: boolean ): void;
    Wander( x1: number, y1: number, x2: number, y2?: number ): void;
    WhisperMessage( msg: string ): void;
    YellMessage( msg: string ): void;
  }
  type UOXChar_class = Character;
  type CharOrItem = Character | Item;
  type CharOrNull = Character | null | undefined;
  type CharOrSocket = Character | Socket;
  enum ClientFeature {
    CF_BIT_CHAT = 0,        // 0x01
    CF_BIT_UOR = 1,         // 0x02
    CF_BIT_TD = 2,          // 0x04
    CF_BIT_LBR = 3,         // 0x08 - Enables LBR features: mp3s instead of midi, show new LBR monsters
    CF_BIT_AOS = 4,         // 0x10 - Enable AoS monsters/map, AoS skills, Necro/Pala/Fight book stuff - works for 4.0+
    CF_BIT_SIXCHARS = 5,    // 0x20 - Enable sixth character slot
    CF_BIT_SE = 6,          // 0x40
    CF_BIT_ML = 7,          // 0x80 - Elven race, new spells, skills + housing tiles
    CF_BIT_EIGHTAGE = 8,    // 0x100 - Splash screen for 8th age
    CF_BIT_NINTHAGE = 9,    // 0x200 - Splash screen for 9th age
    CF_BIT_TENTHAGE = 10,   // 0x400 - Splash screen for 10th age - crystal/shadow house tiles
    CF_BIT_UNKNOWN1 = 11,   // 0x800 - Increased housing/bank storage (6.0.3.0 or earlier)
    CF_BIT_SEVENCHARS = 12, // 0x1000 - Enable seventh character slot
    // CF_BIT_KRFACES = 13, // 0x2000 - KR release (6.0.0.0)
    // CF_BIT_TRIAL = 14,   // 0x4000 - Trial account
    CF_BIT_EXPANSION = 15,  // 0x8000 - Live account
    CF_BIT_SA = 16,         // 0x10000 - Enable SA features: gargoyle race, spells, skills, housing tiles - clients 6.0.14.2+
    CF_BIT_HS = 17,         // 0x20000 - Enable HS features: boats, new movementtype? ++
    CF_BIT_GOTHHOUSE = 18,  // 0x40000
    CF_BIT_RUSTHOUSE = 19,  // 0x80000
    CF_BIT_JUNGLEHOUSE = 20, // 0x100000 - Enable Jungle housing tiles
    CF_BIT_SHADOWHOUSE = 21, // 0x200000 - Enable Shadowguard housing tiles
    CF_BIT_TOLHOUSE = 22,   // 0x400000 - Enable Time of Legends features
    CF_BIT_ENDLESSHOUSE = 23, // 0x800000 - Enable Endless Journey account
    CF_BIT_COUNT,          
  }
  enum CommandLevels {
    CL_PLAYER = 0,
    CL_CNS = 1,  
    CL_GM = 2,   
    CL_ADMIN = 3,
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
  enum FontType {
    FNT_NULL = -1,          
    FNT_BOLD = 0,           
    FNT_TEXT_WITH_SHADOW = 1,
    FNT_BOLD_PLUS_SHADOW = 2,
    FNT_NORMAL = 3,         
    FNT_GOTHIC = 4,         
    FNT_ITALIC = 5,         
    FNT_SMALL_DARK = 6,     
    FNT_COLOURFUL = 7,      
    FNT_RUNIC = 8,           // Only use CAPS!
    FNT_SMALL_LIGHT = 9,    
    FNT_TEN = 10,            // Unicode only
    FNT_ELEVEN = 11,         // Unicode only
    FNT_TWELVE = 12,         // Unicode only
    FNT_UNKNOWN = 13,       
  }
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
    GT_ORDER = 1,  
    GT_CHAOS = 2,  
    GT_UNKNOWN = 3,
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
    readonly  isChar:            false;
    readonly  isContType:        boolean;
              isDamageable:      boolean;
              isDoorOpen:        boolean;
              isDyeable:         boolean;
    readonly  isFieldSpell:      boolean;
    readonly  isItem:            true;
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
    ApplyRank( rank: number, maxRank: number ): void;
    Carve( target: Socket ): boolean;
    Dupe( target: SocketOrNull ): Item;
    GetMoreVar( name: string, part: number ): number;
    GetTileName(): string;
    Glow( target: Socket ): void;
    HasSpell( spellId: number ): boolean;
    IsBoat(): boolean;
    IsMulti(): boolean;
    IsOnFoodList( foodList: string ): boolean;
    LockDown(): void;
    OpenPlank(): void;
    PlaceInPack( autoStack: boolean ): void;
    RemoveSpell( spellId: number ): void;
    SetCont( newCont: BaseObject ): boolean;
    SetMoreVar( name: string, part: number, value: number ): void;
    SoundEffect( soundId: number, allHear: boolean, monsterId?: number ): void;
    UnGlow( target: Socket ): void;
  }
  type UOXItem_class = Item;
  type ItemOrNull = Item | null | undefined;
  type ItemOrSerial = Item | number;
  enum ItemTypes {
    IT_NOTYPE = 0,            
    IT_CONTAINER = 1,         
    IT_CASTLEGATEOPENER = 2,  
    IT_CASTLEGATE = 3,        
    IT_TELEPORTITEM = 6,      
    IT_KEY = 7,               
    IT_LOCKEDCONTAINER = 8,   
    IT_SPELLBOOK = 9,         
    IT_MAP = 10,              
    IT_BOOK = 11,             
    IT_DOOR = 12,             
    IT_LOCKEDDOOR = 13,       
    IT_FOOD = 14,             
    IT_MAGICWAND = 15,        
    IT_RESURRECTOBJECT = 16,  
    IT_CRYSTALBALL = 18,      
    IT_POTION = 19,           
    IT_TRADEWINDOW = 20,      
    IT_TOWNSTONE = 35,        
    IT_RUNEBOOK = 49,         
    IT_RECALLRUNE = 50,       
    IT_GATE = 51,             
    IT_OBJTELEPORTER = 60,    
    IT_ITEMSPAWNER = 61,      
    IT_NPCSPAWNER = 62,       
    IT_SPAWNCONT = 63,        
    IT_LOCKEDSPAWNCONT = 64,  
    IT_UNLOCKABLESPAWNCONT = 65,
    IT_AREASPAWNER = 69,      
    IT_ADVANCEGATE = 80,      
    IT_MULTIADVANCEGATE = 81, 
    IT_MONSTERGATE = 82,      
    IT_RACEGATE = 83,         
    IT_DAMAGEOBJECT = 85,     
    IT_TRASHCONT = 87,        
    IT_SOUNDOBJECT = 88,      
    IT_MAPCHANGEOBJECT = 89,  
    IT_WORLDCHANGEGATE = 90,  
    IT_MORPHOBJECT = 101,     
    IT_UNMORPHOBJECT = 102,   
    IT_DRINK = 105,           
    IT_STANDINGHARP = 106,    
    IT_SHIELD = 107,          
    IT_ZEROKILLSGATE = 111,   
    IT_PLANK = 117,           
    IT_FIREWORKSWAND = 118,   
    IT_SPELLCHANNELING = 119, 
    IT_ESCORTNPCSPAWNER = 125,
    IT_RENAMEDEED = 186,      
    IT_LEATHERREPAIRTOOL = 190,
    IT_BOWREPAIRTOOL = 191,   
    IT_TILLER = 200,          
    IT_HOUSEADDON = 201,      
    IT_GUILDSTONE = 202,      
    IT_HOUSESIGN = 203,       
    IT_TINKERTOOL = 204,      
    IT_METALREPAIRTOOL = 205, 
    IT_FORGE = 207,           
    IT_DYE = 208,             
    IT_DYEVAT = 209,          
    IT_MODELMULTI = 210,      
    IT_ARCHERYBUTTE = 211,    
    IT_DRUM = 212,            
    IT_TAMBOURINE = 213,      
    IT_HARP = 214,            
    IT_LUTE = 215,            
    IT_AXE = 216,             
    IT_PLAYERVENDORDEED = 217,
    IT_SMITHYTOOL = 218,      
    IT_CARPENTRYTOOL = 219,   
    IT_MININGTOOL = 220,      
    IT_EMPTYVIAL = 221,       
    IT_UNSPUNFABRIC = 222,    
    IT_UNCOOKEDFISH = 223,    
    IT_UNCOOKEDMEAT = 224,    
    IT_SPUNFABRIC = 225,      
    IT_FLETCHINGTOOL = 226,   
    IT_CANNONBALL = 227,      
    IT_WATERPITCHER = 228,    
    IT_UNCOOKEDDOUGH = 229,   
    IT_SEWINGKIT = 230,       
    IT_ORE = 231,             
    IT_MESSAGEBOARD = 232,    
    IT_SWORD = 233,           
    IT_CAMPING = 234,         
    IT_MAGICSTATUE = 235,     
    IT_GUILLOTINE = 236,      
    IT_FLOURSACK = 238,       
    IT_OPENFLOURSACK = 239,   
    IT_FISHINGPOLE = 240,     
    IT_CLOCK = 241,           
    IT_MORTAR = 242,          
    IT_SCISSORS = 243,        
    IT_BANDAGE = 244,         
    IT_SEXTANT = 245,         
    IT_HAIRDYE = 246,         
    IT_LOCKPICK = 247,        
    IT_COTTONPLANT = 248,     
    IT_TINKERAXLE = 249,      
    IT_TINKERAWG = 250,       
    IT_TINKERCLOCK = 251,     
    IT_TINKERSEXTANT = 252,   
    IT_TRAININGDUMMY = 253,   
    IT_PETTRANSFERDEED = 254, 
    IT_COUNT = 255,           
  }
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
    AddToBanList( newBan: Character ): void;
    AddToFriendList( newFriend: Character ): void;
    AddToGuestList( newGuest: Character ): void;
    AddToOwnerList( newOwner: Character ): void;
    AddTrashCont( trash: Item ): void;
    ClearBanList(): void;
    ClearFriendList(): void;
    ClearGuestList(): void;
    ClearOwnerList(): void;
    FinishedChars( listType?: string ): boolean;
    FirstChar( listType?: string ): CharOrNull;
    GetMultiCorner( corner: number ): string;
    GetTiller(): ItemOrNull;
    IsInMulti( toCheck: BaseObject ): boolean;
    IsOnBanList( toCheck: Character ): boolean;
    IsOnFriendList( toCheck: Character ): boolean;
    IsOnGuestList( toCheck: Character ): boolean;
    IsOnOwnerList( toCheck: Character ): boolean;
    IsOwner( toCheck: Character ): boolean;
    IsSecureContainer( toChcek: Item ): boolean;
    KillKeys( specific?: Character ): void;
    LockDownItem( toLock: Item ): boolean;
    NextChar( listType?: string ): CharOrNull;
    ReleaseItem( toRelease: Item ): boolean;
    RemoveFromBanList( toRemove: Character ): boolean;
    RemoveFromFriendList( toRemove: Character ): boolean;
    RemoveFromGuestList( toRemove: Character ): boolean;
    RemoveFromOwnerList( toRemove: Character ): boolean;
    RemoveTrashCont( toRemove: Item ): boolean;
    SecureContainer( toSecure: Item ): boolean;
    TurnBoat( direction: number ): void;
    UnsecureContainer( toUnsecure: Item ): boolean;
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
    SF_BIT_IGR = 1,         // 0x02
    SF_BIT_ONECHAR = 2,     // 0x04 - One char only, Siege-server style
    SF_BIT_CONTEXTMENUS = 3, // 0x08
    SF_BIT_LIMITCHAR = 4,   // 0x10 - Limit amount of chars, combine with OneChar
    SF_BIT_AOS = 5,         // 0x20 - Enable Tooltips, fight system book - but not monsters/map/skills/necro/pala classes
    SF_BIT_SIXCHARS = 6,    // 0x40 - Use 6 character slots instead of 5 (4.0.3a)
    SF_BIT_SE = 7,          // 0x80 - Samurai and Ninja classes, bushido, ninjitsu (4.0.5a)
    SF_BIT_ML = 8,          // 0x100 - Elven race, spellweaving (4.0.11d)
    SF_BIT_UNKNOWN2 = 9,    // 0x200 - added with UO:KR launch (6.0.0.0)
    SF_BIT_SEND3DTYPE = 10, // 0x400 - Send UO3D client type? KR and SA clients will send 0xE1)
    SF_BIT_UNKNOWN4 = 11,   // 0x800 - added sometime between UO:KR and UO:SA
    SF_BIT_SEVENCHARS = 12, // 0x1000 - Use 7 character slots instead of 5?6?, only 2D client?
    SF_BIT_UNKNOWN5 = 13,   // 0x2000 - added with UO:SA launch, imbuing, mysticism, throwing? (7.0.0.0)
    SF_BIT_NEWMOVE = 14,    // 0x4000 - new movement system (packets 0xF0, 0xF1, 0xF2))
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
    tCHAR_TIMEOUT = 0,          // Global Character Timer
    tCHAR_INVIS = 1,            // Global Character Timer
    tCHAR_HUNGER = 2,           // Global Character Timer
    tCHAR_THIRST = 3,           // Global Character Timer
    tCHAR_POISONTIME = 4,       // Global Character Timer
    tCHAR_POISONTEXT = 5,       // Global Character Timer
    tCHAR_POISONWEAROFF = 6,    // Global Character Timer
    tCHAR_SPELLTIME = 7,        // Global Character Timer
    tCHAR_SPELLRECOVERYTIME = 8, // Global Character Timer
    tCHAR_ANTISPAM = 9,         // Global Character Timer
    tCHAR_CRIMFLAG = 10,        // Global Character Timer
    tCHAR_STEALFLAG = 11,       // Global Character Timer
    tCHAR_MURDERRATE = 12,      // Global Character Timer
    tCHAR_PEACETIMER = 13,      // Global Character Timer
    tCHAR_FLYINGTOGGLE = 14,    // Global Character Timer
    tCHAR_FIREFIELDTICK = 15,   // Global Character Timer
    tCHAR_POISONFIELDTICK = 16, // Global Character Timer
    tCHAR_PARAFIELDTICK = 17,   // Global Character Timer
    tCHAR_YOUNGHEAL = 18,       // Global Character Timer
    tCHAR_YOUNGMESSAGE = 19,    // Global Character Timer
    tNPC_MOVETIME = 20,         // NPC Timer
    tNPC_SPATIMER = 21,         // NPC Timer
    tNPC_SUMMONTIME = 22,       // NPC Timer
    tNPC_EVADETIME = 23,        // NPC Timer
    tNPC_LOYALTYTIME = 24,      // NPC Timer
    tNPC_IDLEANIMTIME = 25,     // NPC Timer
    tNPC_PATHFINDDELAY = 26,    // NPC Timer
    tNPC_FLEECOOLDOWN = 27,     // NPC Timer
    tPC_LOGOUT = 28,            // PC Timer
    tCHAR_COUNT = 29,          
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
  enum SpeechTarget {
    SPTRG_NULL = -1,      
    SPTRG_INDIVIDUAL = 0,  // aimed at individualperson
    SPTRG_PCS = 1,         // all PCs in range
    SPTRG_PCNPC = 2,       // all NPCs and PCs in range
    SPTRG_BROADCASTPC = 3, // ALL PCs everywhere + NPCs in range
    SPTRG_BROADCASTALL = 4,
    SPTRG_ONLYRECEIVER = 5, // only the receiver
  }
  enum SpeechType {
    UNKNOWN = -1,    
    TALK = 0,         // normal system message
    PROMPT = 1,       // Display as system prompt
    EMOTE = 2,        // : text
    SAY = 3,          // character speaking
    OBJ = 4,          // at object
    NOTHING = 5,      // does not display
    SYSTEM = 6,       // text labelling an item
    NOSCROLL = 7,     // status msg, does not scroll
    WHISPER = 8,      // only those close can here
    YELL = 9,         // can be heard 2 screens away
    ASCIITALK = 192,  // ASCII version of TALK, all ASCII stuff is | 0xC0'd
    ASCIIPROMPT = 193,
    ASCIIEMOTE = 194,
    ASCIISAY = 195,  
    ASCIIOBJ = 196,  
    ASCIINOTHING = 197,
    ASCIISYSTEM = 198,
    ASCIINOSCROLL = 199,
    ASCIIWHISPER = 200,
    ASCIIYELL = 201, 
    BROADCAST = 255, 
  }
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
    TF_HOLDABLE = 1,    // Weapon
    TF_TRANSPARENT = 2, // SignGuildBanner
    TF_TRANSLUCENT = 3, // WebDirtBlood
    TF_WALL = 4,        // WallVertTile
    TF_DAMAGING = 5,   
    TF_BLOCKING = 6,    // Impassable
    TF_WET = 7,         // LiquidWet
    TF_UNKNOWN1 = 8,    // Ignored
    TF_SURFACE = 9,     // Standable
    TF_CLIMBABLE = 10,  // Bridge
    TF_STACKABLE = 11,  // Generic
    TF_WINDOW = 12,     // WindowArchDoor
    TF_NOSHOOT = 13,    // CannotShootThru
    TF_DISPLAYA = 14,   // Prefix A
    TF_DISPLAYAN = 15,  // Prefix An
    TF_DESCRIPTION = 16, // Internal
    TF_FOLIAGE = 17,    // FadeWithTrans
    TF_PARTIALHUE = 18,
    TF_UNKNOWN2 = 19,  
    TF_MAP = 20,       
    TF_CONTAINER = 21, 
    TF_WEARABLE = 22,   // Equipable
    TF_LIGHT = 23,      // LightSource
    TF_ANIMATED = 24,  
    TF_NODIAGONAL = 25, // 'HoverOver' in SA clients and later, to determine if tiles can be moved on by flying gargoyles
    TF_UNKNOWN3 = 26,   // 'NoDiagonal' in SA clients and later?
    TF_ARMOR = 27,      // WholeBodyItem
    TF_ROOF = 28,       // WallRoofWeap
    TF_DOOR = 29,      
    TF_STAIRBACK = 30,  // ClimbableBit1
    TF_STAIRRIGHT = 31, // ClimbableBit2
    TF_ALPHABLEND = 32, // Blend Alphas, tile blending
    TF_USENEWART = 33,  // Uses new art style?
    TF_ARTUSED = 34,    // Is art being used?
    TF_BIT36 = 35,      // Unknown/Unused
    TF_NOSHADOW = 36,   // Disallow shadow on this tile, lightsource? lava?
    TF_PIXELBLEED = 37, // Let pixels bleed in to other tiles? Is this Disabling Texture Clamp?
    TF_PLAYANIMONCE = 38, // Play tile animation once.
    TF_BIT40 = 39,      // Unknown/Unused
    TF_MULTIMOVABLE = 40, // Movable multi? Cool ships and vehicles etc?
    TF_COUNT = 41,     
  }
  enum Timer {
    TIMEOUT = 0,             // Character timer
    INVIS = 1,               // Character timer
    HUNGER = 2,              // Character timer
    THIRST = 3,              // Character timer
    POISONTIME = 4,          // Character timer
    POISONTEXT = 5,          // Character timer
    POISONWEAROFF = 6,       // Character timer
    SPELLTIME = 7,           // Character timer
    SPELLRECOVERYTIME = 8,   // Character timer
    CRIMFLAG = 9,            // Character timer
    ANTISPAM = 10,           // Character timer
    MURDERRATE = 11,         // Character timer
    PEACETIMER = 12,         // Character timer
    FLYINGTOGGLE = 13,       // Character timer
    FIREFIELDTICK = 14,      // Character timer
    POISONFIELDTICK = 15,    // Character timer
    PARAFIELDTICK = 16,      // Character timer
    YOUNGHEAL = 17,          // Character timer
    YOUNGMESSAGE = 18,       // Character timer
    MOVETIME = 19,           // Character timer
    SPATIMER = 20,           // Character timer
    SUMMONTIME = 21,         // Character timer
    EVADETIME = 22,          // Character timer
    LOYALTYTIME = 23,        // Character timer
    IDLEANIMTIME = 24,       // Character timer
    LOGOUT = 25,             // Character timer
    SOCK_SKILLDELAY = 26,    // Socket timer
    SOCK_OBJDELAY = 27,      // Socket timer
    SOCK_SPIRITSPEAK = 28,   // Socket timer
    SOCK_TRACKING = 29,      // Socket timer
    SOCK_FISHING = 30,       // Socket timer
    SOCK_MUTETIME = 31,      // Socket timer
    SOCK_TRACKINGDISPLAY = 32, // Socket timer
    SOCK_TRAFFICWARDEN = 33, // Socket timer
    COUNT = 34,              // Socket timer
  }
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
    UT_ARA = 1,        
    UT_ARI = 2,        
    UT_ARE = 3,        
    UT_ARL = 4,        
    UT_ARG = 5,        
    UT_ARM = 6,        
    UT_ART = 7,        
    UT_ARO = 8,        
    UT_ARY = 9,        
    UT_ARS = 10,       
    UT_ARJ = 11,       
    UT_ARB = 12,       
    UT_ARK = 13,       
    UT_ARU = 14,       
    UT_ARH = 15,       
    UT_ARQ = 16,       
    UT_BGR = 17,       
    UT_CAT = 18,       
    UT_CHT = 19,       
    UT_CHS = 20,       
    UT_ZHH = 21,       
    UT_ZHI = 22,       
    UT_ZHM = 23,       
    UT_CSY = 24,       
    UT_DAN = 25,       
    UT_DEU = 26,       
    UT_DES = 27,       
    UT_DEA = 28,       
    UT_DEL = 29,       
    UT_DEC = 30,       
    UT_ELL = 31,       
    UT_ENU = 32,       
    UT_ENG = 33,       
    UT_ENA = 34,       
    UT_ENC = 35,       
    UT_ENZ = 36,       
    UT_ENI = 37,       
    UT_ENS = 38,       
    UT_ENJ = 39,       
    UT_ENB = 40,       
    UT_ENL = 41,       
    UT_ENT = 42,       
    UT_ENW = 43,       
    UT_ENP = 44,       
    UT_ESP = 45,       
    UT_ESM = 46,       
    UT_ESN = 47,       
    UT_ESG = 48,       
    UT_ESC = 49,       
    UT_ESA = 50,       
    UT_ESD = 51,       
    UT_ESV = 52,       
    UT_ESO = 53,       
    UT_ESR = 54,       
    UT_ESS = 55,       
    UT_ESF = 56,       
    UT_ESL = 57,       
    UT_ESY = 58,       
    UT_ESZ = 59,       
    UT_ESB = 60,       
    UT_ESE = 61,       
    UT_ESH = 62,       
    UT_ESI = 63,       
    UT_ESU = 64,       
    UT_FIN = 65,       
    UT_FRA = 66,       
    UT_FRB = 67,       
    UT_FRC = 68,       
    UT_FRS = 69,       
    UT_FRL = 70,       
    UT_FRM = 71,       
    UT_HEB = 72,       
    UT_HUN = 73,       
    UT_ISL = 74,       
    UT_ITA = 75,       
    UT_ITS = 76,       
    UT_JPN = 77,       
    UT_KOR = 78,       
    UT_NLD = 79,       
    UT_NLB = 80,       
    UT_NOR = 81,       
    UT_NON = 82,       
    UT_PLK = 83,       
    UT_PTB = 84,       
    UT_PTG = 85,       
    UT_ROM = 86,       
    UT_RUS = 87,       
    UT_HRV = 88,       
    UT_SRL = 89,       
    UT_SRB = 90,       
    UT_SKY = 91,       
    UT_SQI = 92,       
    UT_SVE = 93,       
    UT_SVF = 94,       
    UT_THA = 95,       
    UT_TRK = 96,       
    UT_URP = 97,       
    UT_IND = 98,       
    UT_UKR = 99,       
    UT_BEL = 100,      
    UT_SLV = 101,      
    UT_ETI = 102,      
    UT_LVI = 103,      
    UT_LTH = 104,      
    UT_LTC = 105,      
    UT_FAR = 106,      
    UT_VIT = 107,      
    UT_HYE = 108,      
    UT_AZE = 109,      
    UT_EUQ = 110,      
    UT_MKI = 111,      
    UT_AFK = 112,      
    UT_KAT = 113,      
    UT_FOS = 114,      
    UT_HIN = 115,      
    UT_MSL = 116,      
    UT_MSB = 117,      
    UT_KAZ = 118,      
    UT_SWK = 119,      
    UT_UZB = 120,      
    UT_TAT = 121,      
    UT_BEN = 122,      
    UT_PAN = 123,      
    UT_GUJ = 124,      
    UT_ORI = 125,      
    UT_TAM = 126,      
    UT_TEL = 127,      
    UT_KAN = 128,      
    UT_MAL = 129,      
    UT_ASM = 130,      
    UT_MAR = 131,      
    UT_SAN = 132,      
    UT_KOK = 133,      
    TOTAL_LANGUAGES = 134,
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
    VT_TEMPHIDDEN = 1, // Hidden (With Skill) / Visible To Item Owner
    VT_INVISIBLE = 2,  // Magically Hidden
    VT_PERMHIDDEN = 3, // Permanent Hidden (GM Hidden)
    VT_GHOSTHIDDEN = 4, // Hidden because the player is a ghost
  }
  enum WeatherType {
    NONE = 0,    
    PHYSICAL = 1,
    LIGHT = 2,   
    RAIN = 3,    
    COLD = 4,    
    HEAT = 5,    
    LIGHTNING = 6,
    POISON = 7,  
    SNOW = 8,    
    STORM = 9,   
    STORMBREW = 10,
    WEATHNUM = 11,
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
  function TriggerEvent( scriptId: number, eventToFire: string, ...extra: any[] ): boolean;
  function TriggerTrap( src: CharOrSocket, item: Item ): void;
  function UseItem( src: CharOrSocket, item: Item ): void;
  function ValidateObject( obj: Character | Item | null | undefined ): obj is Character | Item;
  function WillResultInCriminal( source: Character, target: Character ): boolean;
  function WorldBrightLevel( newLevel?: number ): number;
  function WorldDarkLevel( newLevel?: number ): number;
  function WorldDungeonLevel( newLevel?: number ): number;
  function Yell( source: Socket, value: string, commandLevel: CommandLevels ): void;

// Event Functions

  function inRange( srcObj: BaseObject, objInRange: BaseObject ): void;
  function onAICombatTarget( attacker: Character, target: Character ): boolean;
  function onAISliver( pSliver: Character ): boolean;
  function onAttack( attacker: Character, defender: Character, hitStatus: boolean, hitLoc: number, damageDealt: number ): void;
  function onBoatTurn( iMulti: Boat, oldDir: number, newDir: number, iTiller: Item ): boolean;
  function onBoughtFromVendor( targSock: Socket, objVendor: Character, objItemBought: BaseObject, numItemsBought: number ): boolean;
  function onBuy( targSock: Socket, objVendor: Character ): boolean;
  function onBuyFromVendor( targSock: Socket, objVendor: Character, objItemBought: BaseObject, numItemsBuying: number ): boolean;
  function onCallback( tSock: Socket, target: Character | Item | null ): void;
  function onCarveCorpse( player: Character, corpse: Item ): boolean;
  function onCharDoubleClick( currChar: Character, targChar: Character ): boolean;
  function onClick( sockPlayer: Socket, objClicked: BaseObject ): boolean;
  function onCollide( targSock: Socket, objColliding: Character, objCollideWith: BaseObject ): boolean;
  function onCombatDamageCalc( attacker: Character, defender: Character, getFightSkill: number, hitLoc: number ): number;
  function onCombatEnd( attacker: Character, defender: Character ): boolean;
  function onCombatStart( attacker: Character, defender: Character ): boolean;
  function onCommand( mSock: Socket, command: string ): boolean;
  function onContextMenuRequest( tSock: Socket, baseObj: BaseObject ): boolean;
  function onContextMenuSelect( tSock: Socket, baseObj: BaseObject, popupEntry: number ): boolean;
  function onContRemoveItem( contItem: Item, item: Item, pickerUpper: Character ): void;
  function onCreateDFN( thingCreated: BaseObject, thingType: 0 | 1 ): void;
  function onCreatePlayer( thingCreated: BaseObject, thingType: 0 | 1 ): void;
  function onCreateTile( thingCreated: BaseObject, thingType: 0 | 1 ): void;
  function onDamage( damaged: Character, attacker: Character, damageValue: number, damageType: WeatherType ): boolean;
  function onDamageDeal( attacker: Character, damaged: Character, damageValue: number, damageType: WeatherType ): boolean;
  function onDeath( pDead: Character, iCorpse: Item ): boolean;
  function onDeathBlow( mKilled: Character, mKiller: Character ): boolean;
  function onDecay( decaying: Item ): boolean;
  function onDefense( attacker: Character, defender: Character, hitStatus: boolean, hitLoc: number, damageReceived: number ): void;
  function onDelete( thingDestroyed: BaseObject ): void;
  function onDispel( dispelled: BaseObject ): boolean;
  function onDrop( item: Item, dropper: Character ): number;
  function onDropItemOnItem( item: Item, dropper: Character, dest: Item ): number;
  function onDropItemOnNpc( srcChar: Character, targChar: Character, i: Item ): number;
  function onDyeTarget( player: Character, dyeTub: Item, target: Item ): boolean;
  function onEnterEvadeState( npc: Character, enemy: Character ): boolean;
  function onEnterRegion( entering: Character, region: number ): void;
  function onEntrance( left: Multi, leaving: BaseObject ): boolean;
  function onEquip( equipper: Character, equipping: Item ): boolean;
  function onEquipAttempt( equipper: Character, equipping: Item ): boolean;
  function onFacetChange( mChar: Character, oldFacet: number, newFacet: number ): boolean;
  function onFall( pFall: Character ): void;
  function onFlagChange( pChanging: Character, newStatus: number, oldStatus: number ): boolean;
  function onHelpButton( mChar: Character ): boolean;
  function onHouseCommand( targSock: Socket, multiObj: Multi, targId: number ): boolean;
  function onHungerChange( pChanging: Character, newStatus: number ): boolean;
  function onIterate( obj: Character | Item, mSock: Socket ): boolean;
  function onIterateSpawnRegions( a: SpawnRegion, b: number ): boolean;
  function onLeaveRegion( entering: Character, region: number ): void;
  function onLeaving( left: Multi, leaving: BaseObject ): boolean;
  function onLightChange( tObject: BaseObject, lightLevel: number ): boolean;
  function onLogin( sockPlayer: Socket, pPlayer: Character ): boolean;
  function onLogout( sockPlayer: Socket, pPlayer: Character ): boolean;
  function onLoyaltyChange( pChanging: Character, newStatus: number ): boolean;
  function onMakeItem( mSock: Socket, objChar: Character, objItem: Item, createEntryId: number ): void;
  function onMoveDetect( sourceObj: BaseObject, CharInRange: Character, rangeToChar: number, oldCharX: number, oldCharY: number ): boolean;
  function onMultiLogout( iMulti: Multi, cPlayer: Character ): boolean;
  function onNameRequest( myObj: BaseObject, nameRequester: Character, requestSource: number ): string;
  function onPacketReceive( mSock: Socket, packetNum: number ): boolean;
  function onPathfindEnd( npc: Character, pathfindResult: number ): boolean;
  function onPickup( item: Item, pickerUpper: Character, objCont: BaseObject ): void;
  function onQuestGump( mChar: Character ): void;
  function onResurrect( pAlive: Character ): void;
  function onScrollCast( tChar: Character, SpellId: number ): void;
  function onScrollingGumpPress( tSock: Socket, gumpId: number, buttonId: number ): void;
  function onSell( targSock: Socket, objVendor: Character ): void;
  function onSellToVendor( targSock: Socket, objVendor: Character, objItemSold: BaseObject, numItemsSelling: number ): void;
  function onSkill( skillUse: BaseObject, skillUsed: number, objType: 0 | 1 ): void;
  function onSkillChange( player: Character, skill: number, skillAmtChanged: number ): void;
  function onSkillCheck( myChar: Character, skill: number, lowSkill: number, highSkill: number, isCraftSkill: boolean ): void;
  function onSkillGain( player: Character, skill: number, skillAmtGained: number ): void;
  function onSkillGump( mChar: Character ): void;
  function onSkillLoss( player: Character, skill: number, skillAmtLost: number ): void;
  function onSnoopAttempt( snooped: Character, pack: Item, snooper: Character ): void;
  function onSnooped( snooped: Character, snooper: Character, success: boolean ): void;
  function onSoldToVendor( targSock: Socket, objVendor: Character, objItemSold: BaseObject, numItemsSold: number ): void;
  function onSpecialMove( mChar: Character, abilityId: number ): void;
  function onSpeech( speech: string, personTalking: Character, talkingTo: BaseObject ): null | undefined | number | boolean;
  function onSpeechInput( myChar: Character, myItem: Item, mySpeech: string ): void;
  function onSpellCast( tChar: Character, SpellId: number ): void;
  function onSpellGain( book: Item, spellNum: number ): void;
  function onSpellLoss( book: Item, spellNum: number ): void;
  function onSpellSuccess( tChar: Character, SpellId: number ): void;
  function onSpellTarget( target: BaseObject, caster: Character, spellNum: number ): void;
  function onSpellTargetSelect( caster: Character, target: BaseObject, spellNum: number ): number;
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
  function onUseChecked( user: Character, iUsing: Item ): boolean;
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
