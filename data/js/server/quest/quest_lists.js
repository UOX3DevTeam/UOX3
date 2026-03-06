/**
 * =============================================================================
 *                              QUEST LIST MODULE
 * =============================================================================
 *
 *  File: QuestList.js
 *  Purpose:
 *    Centralized registry of all quest definitions for the game.
 *    Provides a consistent structure for quest data, enabling retrieval
 *    by ID or iteration over all available quests.
 *
 *  Usage:
 *    - QuestList() -> Returns an array of all quest definitions.
 *    - QuestList(questID) -> Returns the quest with the given ID, or null if not found.
 *
 * =============================================================================
 *  QUEST OBJECT STRUCTURE (DL/UOX3)
 * =============================================================================
 *
 *  Core Details:
 *  -------------
 *   title           - Display name of the quest.
 *   description     - Narrative/backstory shown in quest conversation/log.
 *   uncomplete      - NPC text when quest is not yet complete.
 *   complete        - NPC text when quest is completed.
 *   oncomplete      - Optional completion message (commonly used by skillgain).
 *   refuse          - NPC text when player declines the quest.
 *   npcPhrases      - Optional NPC idle chatter.
 *
 *  Quest Classification:
 *  ----------------------
 *   type            - Quest type:
 *                     collect, timecollect, kill, timekills, multi, delivery, skillgain
 *                     (reward types are defined under "rewards", not in "type")
 *   category        - Quest grouping (e.g., "Side Quests", "Daily", "Main Story").
 *
 *  Flags & Control:
 *  ----------------
 *   oneTimeQuest        - 1/true = quest can only be completed once.
 *   dailyQuest          - 1/true = quest can be repeated after reset window.
 *   resetDailyTime      - Hours until daily quest becomes available again (default 24).
 *   timeLimit           - Timed quest limit in seconds (converted to ms internally).
 *   questTurnIn         - 1 = Requires NPC turn-in, 0 = Auto-complete when objectives met.
 *   bankgold            - (reward-level flag typically) 1 = gold to bank, 0 = to pack.
 *   requiresQuestID     - Prerequisite questID that must be archived before accepting.
 *
 *  Objectives:
 *  -----------
 *   targetItems     - Items to collect:
 *                    [{ sectionID, name, amount }]
 *   targetKills     - Creatures to defeat:
 *                    [{ npcID, name, amount }]
 *
 *  Delivery Quests:
 *  ----------------
 *   deliveryItem    - Item details:
 *                    { sectionID, name, amount }
 *   targetDelivery  - NPC recipient target:
 *                    { npcID?, sectionID?, name?, location?: { x, y, z, world } }
 *
 *  Skillgain Quests:
 *  -----------------
 *   targetSkill     - Skill ID to train (numeric).
 *   targetRegion    - Region ID required for accelerated gain (numeric).
 *   regionName      - Friendly region label for UI.
 *   maxSkillPoints  - Goal in tenths (e.g. 50.0 = 500 tenths if you store in tenths).
 *   minPoint        - Min accelerated gain per proc (tenths).
 *   maxPoint        - Max accelerated gain per proc (tenths).
 *
 * =============================================================================
 *  REWARDS
 * =============================================================================
 *
 *   rewards         - Array of reward objects. Supported reward types:
 *                    - { type:"gold", amount, bankgold?: 1|0 }
 *                    - { type:"item", sectionID, amount, name?, color?|hue? }
 *                    - { type:"karma", amount }
 *                    - { type:"fame", amount }
 *                    - { type:"skill", skill:<string|number>, amount:<number> }   // respects skillCaps
 *                    - { type:"skillpoints", amount:<number> }                   // pooled points
 *                    - { type:"virtue", virtueIndex:<0..7>, amount:<number>, name? }
 *
 *   virtueIndex mapping:
 *     0 Humility, 1 Sacrifice, 2 Compassion, 3 Spirituality,
 *     4 Valor, 5 Honor, 6 Justice, 7 Honesty
 *
 *   Pooled skill points:
 *     - Stored under tag "UnspentSkillPoints" in tenths.
 *     - Spend via:
 *         SpendSkillPoints( player, socket, "Magery", 3.0 );  // +3.0 Magery (respects caps)
 *         SpendSkillPoints( player, socket, 27, 2.5 );        // +2.5 skill ID 27 (e.g., Tactics)
 *
 * =============================================================================
 *  QUEST CHAINS (LINEAR + BRANCHING)
 * =============================================================================
 *
 *   Linear chain:
 *     nextQuestID      - Next questID in chain (legacy/simple).
 *
 *   Branching chain:
 *     nextQuest        - Array of candidate next quests resolved in order:
 *       [
 *         { questID: 20011, cond: { completedQuest: 20010 } },
 *         { questID: 20012, cond: { tagMin: { tag:"dl_rep", value: 10 } } },
 *         { questID: 20013, cond: { always: true } }
 *       ]
 *
 *     Supported cond keys:
 *       - tagEquals:      { tag:"X", value: 1 }
 *       - tagMin:         { tag:"X", value: 10 }
 *       - completedQuest: <questID>
 *       - notCompletedQuest: <questID>
 *       - hasQuest:       <questID>      // active
 *       - notHasQuest:    <questID>
 *       - worldStateEquals: { key:"ws_key", value: 1 }
 *       - worldStateMin:    { key:"ws_key", value: 10 }
 *       - always: true
 *
 * =============================================================================
 *  QUEST COMPLETION EFFECTS (TAGS + WORLD STATE)
 * =============================================================================
 *
 *  Persistent Tag Updates:
 *  -----------------------
 *   setTags         - Apply/remove persistent character tags on completion.
 *                    Example: setTags: { "starterTier": 2, "needsTutorial": null }
 *
 *   setTagDeltas    - Add/subtract integer deltas to persistent tags.
 *                    Missing/non-numeric treated as 0.
 *                    Example: setTagDeltas: { "FF_GuardStanding": 2, "WinterEventPoints": 5 }
 *
 *   deltaRules      - Optional per-tag clamping for setTagDeltas.
 *                    Example:
 *                      deltaRules: {
 *                        "FF_GuardStanding": { min: -100, max: 100 },
 *                        "WinterEventPoints": { min: 0, max: 999 }
 *                      }
 *
 *   deltaRulesDefault
 *                  - Optional default clamp rule for setTagDeltas keys without an entry.
 *                    Example: deltaRulesDefault: { min: 0, max: 100 }
 *
 *  Temporary Tag Updates:
 *  ----------------------
 *   setTempTags       - Apply/remove temp tags on completion.
 *   setTempTagDeltas  - Add/subtract integer deltas to temp tags.
 *   tempDeltaRules    - Optional per-key clamping for setTempTagDeltas.
 *   tempDeltaRulesDefault
 *                    - Optional default clamp for setTempTagDeltas.
 *
 *  World State (file-backed, per-character-keyed):
 *  -----------------------------------------------
 *   setWorldState       - Set/remove integer world-state keys on completion.
 *                        Example: setWorldState: { "dl_ws_joinedthieves": 1, "dl_ws_oldkey": null }
 *
 *   worldStateDeltas    - Add/subtract integer deltas to existing world-state keys.
 *                        Example: worldStateDeltas: { "dl_ws_ironreachrep": 5 }
 *
 *   worldStateDeltaRules / worldStateDeltaRulesDefault
 *                        - Optional clamp rules for worldStateDeltas (same min/max schema).
 *
 * =============================================================================
 *  FUNCTION SIGNATURE
 * =============================================================================
 *   QuestList(questID?)
 *     questID - Optional quest ID. If omitted, returns all quests.
 *     Returns - Quest object, array of quests, or null if ID not found.
 *
 * =============================================================================
 */

function QuestList( questID )
{
	var questList = { };

	// Return the full quest list if no questID is provided
	if( !questID )
	{
		return questList;
	}

	// Return a specific quest if questID is provided
	return questList[questID] || null;
}