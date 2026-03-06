/**
 * =============================================================================
 *                              QUEST LIST MODULE
 * =============================================================================
 *
 *  File: QuestList.js
 *
 *  Purpose:
 *    Centralized registry of all quest definitions used by the quest system.
 *    Every quest in the shard is defined here in a consistent structure so
 *    scripts can easily retrieve quest data by ID.
 *
 *  This file does NOT contain quest logic. It only defines quest data.
 *
 * =============================================================================
 *  HOW THE SYSTEM USES THIS FILE
 * =============================================================================
 *
 *  QuestList()           -> Returns an array of all quest definitions.
 *
 *  QuestList(questID)    -> Returns the quest with the given ID.
 *                           Returns null if the ID is not found.
 *
 *  Example:
 *
 *      var quest = QuestList(20010);
 *
 * =============================================================================
 *  HOW TO ADD A NEW QUEST
 * =============================================================================
 *
 *  Each quest is a single object inside the quest array.
 *
 *  Basic steps to create a quest:
 *
 *      1. Choose a UNIQUE questID
 *      2. Fill in the core text fields
 *      3. Choose the quest type
 *      4. Add the correct objective fields for that type
 *      5. Define the rewards
 *      6. (Optional) Add prerequisites, quest chains, or tag updates
 *
 *  IMPORTANT:
 *      Do NOT reuse questIDs.
 *      Each quest must have a unique ID.
 *
 * =============================================================================
 *  MINIMAL QUEST EXAMPLE
 * =============================================================================
 *
 *  {
 *      questID: 1,
 *      title: "A Simple Task",
 *      description: "Bring me 10 bandages.",
 *      type: "collect",
 *      category: "Side Quests",
 *
 *      uncomplete: "You have not brought me the bandages yet.",
 *      complete: "Well done. This is exactly what I needed.",
 *      refuse: "Very well. Maybe another time.",
 *
 *      oneTimeQuest: true,
 *      dailyQuest: false,
 *      questTurnIn: 1,
 *
 *      targetItems: [
 *          { sectionID: "bandages", name: "bandages", amount: 10 }
 *      ],
 *
 *      rewards: [
 *          { type: "gold", amount: 250 }
 *      ]
 *  }
 *
 * =============================================================================
 *  QUEST OBJECT STRUCTURE (DL/UOX3)
 * =============================================================================
 *
 * -----------------------------------------------------------------------------
 *  CORE DETAILS
 * -----------------------------------------------------------------------------
 *
 *   questID        - Unique quest identifier.
 *   title          - Display name of the quest.
 *   description    - Story text shown in the quest log.
 *
 *   uncomplete     - NPC dialogue when quest is still in progress.
 *   complete       - NPC dialogue when quest is finished.
 *   oncomplete     - Optional completion message (often used by skillgain).
 *   refuse         - NPC dialogue if player declines the quest.
 *
 *   npcPhrases     - Optional idle NPC chatter lines.
 *
 * -----------------------------------------------------------------------------
 *  QUEST CLASSIFICATION
 * -----------------------------------------------------------------------------
 *
 *   type           - Quest objective type:
 *
 *        collect       Player gathers items
 *        timecollect   Collect items within a time limit
 *        kill          Kill specific creatures
 *        timekills     Kill creatures within a time limit
 *        multi         Combination of kill and collect
 *        delivery      Deliver item to another NPC
 *        skillgain     Gain skill points while training
 *
 *   category       - UI grouping such as:
 *
 *        "Main Story"
 *        "Side Quests"
 *        "Daily"
 *        "Guild Tasks"
 *
 * -----------------------------------------------------------------------------
 *  QUEST CONTROL FLAGS
 * -----------------------------------------------------------------------------
 *
 *   oneTimeQuest       - true/1 = Can only be completed once
 *
 *   dailyQuest         - true/1 = Quest repeats after reset window
 *
 *   resetDailyTime     - Hours before daily quest resets (default 24)
 *
 *   questTurnIn        - 1 = Player must return to NPC
 *                        0 = Auto-completes when objectives finish
 *
 *   requiresQuestID    - Quest prerequisite ID that must already be completed
 *
 *   timeLimit          - Time limit in seconds for timed quests
 *
 * -----------------------------------------------------------------------------
 *  OBJECTIVES
 * -----------------------------------------------------------------------------
 *
 *  Collect Quests:
 *
 *      targetItems
 *
 *      Example:
 *
 *      targetItems: [
 *          { sectionID: "rat_tail", name: "rat tail", amount: 5 }
 *      ]
 *
 *
 *  Kill Quests:
 *
 *      targetKills
 *
 *      Example:
 *
 *      targetKills: [
 *          { npcID: "wolf", name: "wolf", amount: 8 }
 *      ]
 *
 *
 *  Multi Quests:
 *
 *      Use BOTH targetItems and targetKills.
 *
 *
 * -----------------------------------------------------------------------------
 *  DELIVERY QUESTS
 * -----------------------------------------------------------------------------
 *
 *   deliveryItem
 *
 *      Item player receives from quest giver.
 *
 *   targetDelivery
 *
 *      NPC that receives the item.
 *
 *   Example:
 *
 *      deliveryItem:
 *      {
 *          sectionID: "sealed_note",
 *          name: "sealed note",
 *          amount: 1
 *      }
 *
 *      targetDelivery:
 *      {
 *          sectionID: "npc_blacksmith",
 *          name: "the blacksmith",
 *          location: { x:1450, y:1620, z:20, world:0 }
 *      }
 *
 * -----------------------------------------------------------------------------
 *  SKILLGAIN QUESTS
 * -----------------------------------------------------------------------------
 *
 *   targetSkill      - Skill ID to train
 *   targetRegion     - Region required for accelerated gain
 *   regionName       - Friendly region name for UI
 *
 *   maxSkillPoints   - Total points allowed during quest
 *
 *   minPoint         - Minimum skill gain per proc
 *   maxPoint         - Maximum skill gain per proc
 *
 * -----------------------------------------------------------------------------
 *  REWARDS
 * -----------------------------------------------------------------------------
 *
 *   rewards - Array of reward objects.
 *
 *   Supported reward types:
 *
 *      gold
 *      item
 *      fame
 *      karma
 *      skill
 *      skillpoints
 *      virtue
 *
 * -----------------------------------------------------------------------------
 *  GOLD REWARD
 * -----------------------------------------------------------------------------
 *
 *      { type:"gold", amount:500 }
 *
 *      Optional:
 *
 *      bankgold:1
 *
 *      Sends gold directly to bank instead of backpack.
 *
 * -----------------------------------------------------------------------------
 *  ITEM REWARD
 * -----------------------------------------------------------------------------
 *
 *      { type:"item", sectionID:"reward_ring", amount:1 }
 *
 *      Optional fields:
 *
 *      name
 *      hue
 *      color
 *
 * -----------------------------------------------------------------------------
 *  SKILL REWARD
 * -----------------------------------------------------------------------------
 *
 *      { type:"skill", skill:"Magery", amount:3.0 }
 *
 *      Respects skill caps.
 *
 * -----------------------------------------------------------------------------
 *  POOLED SKILL POINT REWARD
 * -----------------------------------------------------------------------------
 *
 *      { type:"skillpoints", amount:20 }
 *
 *      Stored under tag:
 *
 *      "UnspentSkillPoints"
 *
 * -----------------------------------------------------------------------------
 *  VIRTUE REWARD
 * -----------------------------------------------------------------------------
 *
 *      { type:"virtue", virtueIndex:4, amount:10 }
 *
 *  Virtue Index Mapping:
 *
 *      0 Humility
 *      1 Sacrifice
 *      2 Compassion
 *      3 Spirituality
 *      4 Valor
 *      5 Honor
 *      6 Justice
 *      7 Honesty
 *
 * -----------------------------------------------------------------------------
 *  QUEST CHAINS
 * -----------------------------------------------------------------------------
 *
 *  Simple Linear Chain:
 *
 *      nextQuestID: 20011
 *
 *
 *  Branching Chains:
 *
 *      nextQuest: [
 *          { questID:20011, cond:{ completedQuest:20010 } },
 *          { questID:20012, cond:{ tagMin:{ tag:"rep", value:10 } } },
 *          { questID:20013, cond:{ always:true } }
 *      ]
 *
 *
 *  Supported Conditions:
 *
 *      completedQuest
 *      notCompletedQuest
 *      hasQuest
 *      notHasQuest
 *      tagEquals
 *      tagMin
 *      worldStateEquals
 *      worldStateMin
 *      always
 *
 * -----------------------------------------------------------------------------
 *  QUEST COMPLETION EFFECTS
 * -----------------------------------------------------------------------------
 *
 *  Persistent Character Tags
 *
 *      setTags:
 *      {
 *          "starterTier":2
 *      }
 *
 *
 *  Tag Deltas
 *
 *      setTagDeltas:
 *      {
 *          "GuardStanding":2
 *      }
 *
 *
 *  Temp Tags
 *
 *      setTempTags
 *      setTempTagDeltas
 *
 *
 *  World State Updates
 *
 *      setWorldState
 *      worldStateDeltas
 *
 * -----------------------------------------------------------------------------
 *  NOTES
 * -----------------------------------------------------------------------------
 *
 *  Only include fields that apply to the quest type.
 *
 *  Example:
 *
 *      Kill quests do NOT need deliveryItem
 *      Delivery quests do NOT need targetKills
 *
 *  Keeping quest objects minimal makes them easier to maintain.
 *
 * =============================================================================
 *  FUNCTION SIGNATURE
 * =============================================================================
 *
 *      QuestList(questID?)
 *
 *      questID   Optional quest ID
 *
 *      Returns:
 *
 *          quest object
 *          array of quests
 *          null if ID not found
 *
 * =============================================================================
 */

Example of a quest npc that will call quest 1
[mugg]
{
GET=basehuman
NAME=Mugg the Miner
TITLE=Quest Giver
ID=0x0190
PRIV=4
SKIN=0x840A
EQUIPITEM=0x2047//afro
EQUIPITEM=pickaxe
EQUIPITEM=boots
COLOR=0x901
EQUIPITEM=shortpants
COLOR=0x3B2
EQUIPITEM=shirt
COLOR=0x22B
EQUIPITEM=skullcap
COLOR=0x177
EQUIPITEM=halfapron
COLOR=0x5F1
EQUIPITEM=0x1414//plategloves
STR=100 100
DEX=100 100
INT=25 25
SCRIPT=5802//questconversation.js
CUSTOMINTTAG=QuestID 1
}