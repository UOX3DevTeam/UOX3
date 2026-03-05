/// <reference path="../../definitions.d.ts" />
// @ts-check

// =============================================================================
// Quest Dialogue Trees
// - nodeId: string
// - text: string (HTML allowed)
// - choices: array of { text, next, action?, cond? }
//
// cond can be:
// - tagEquals: { tag:"SomeTag", value:1 }
// - tagMin:    { tag:"SomeTag", value:10 }
// - hasQuest:  questID
// - completedQuest: questID
//
// action can be:
// - startQuest: questID
// - turnInQuest: questID
// - setTags: { "TagA": 1, "TagB": null }   (null removes)
// - sysMsg: "some text"
// =============================================================================

function GetDialogueTree( treeID )
{
	// Return a single tree by id
	var trees = {
		"blacksmith_tutorial": {
			title: "Garrick the Smith",
			startNode: "start",
			nodes: {
				"start": {
					text:
						"<b>Garrick:</b> Hmph. If you want coin, you work. " +
						"Do you know your way around a forge?",
					choices: [
						{ text: "I want work.", next: "offer_work" },
						{ text: "Tell me about this town.", next: "town" },
						{ text: "Goodbye.", next: null }
					]
				},

				"town": {
					text:
						"<b>Garrick:</b> Cold nights. Hard people. Good steel. " +
						"Keep your head down and your blade sharp.",
					choices: [
						{ text: "Back.", next: "start" },
						{ text: "Goodbye.", next: null }
					]
				},

				"offer_work": {
					text:
						"<b>Garrick:</b> Bring me ten bucklers. Then we talk pay.",
					choices: [
						{
							text: "I will do it.",
							next: "accepted",
							action: { startQuest: 5 } // Example: quest id 5 "Battered Bucklers"
						},
						{
							text: "Not right now.",
							next: "start"
						}
					]
				},

				"accepted": {
					text:
						"<b>Garrick:</b> Good. Do not waste my time. " +
						"Come back when the work is done.",
					choices: [
						{
							text: "I have completed it.",
							next: "turnin_check",
							cond: { hasQuest: 5 }
						},
						{
							text: "Back.",
							next: "start"
						}
					]
				},

				"turnin_check": {
					text:
						"<b>Garrick:</b> Let me see...",
					choices: [
						{
							text: "Turn in the bucklers.",
							next: "turnin_done",
							action: { turnInQuest: 5 }
						},
						{ text: "Back.", next: "accepted" }
					]
				},

				"turnin_done": {
					text:
						"<b>Garrick:</b> Finally. Here. And listen... you did fine.",
					choices: [
						{
							text: "Thanks.",
							next: "after_turnin",
							action: { setTags: { "SmithTutorialDone": 1 } }
						},
						{ text: "Goodbye.", next: null }
					]
				},

				"after_turnin": {
					text:
						"<b>Garrick:</b> If you want more, ask again tomorrow.",
					choices: [
						{ text: "Goodbye.", next: null }
					]
				}
			}
		}
	};

	return trees[treeID] || null;
}

// Expose via TriggerEvent usage pattern if you want.
// Example: attach script id and call TriggerEvent(SCRIPTID, "GetDialogueTree", treeID)
function GetDialogueTree_Trigger( treeID )
{
	return GetDialogueTree( treeID );
}