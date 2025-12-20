/// <reference path="../definitions.d.ts" />
// @ts-check

// Character Script Trigger: 6006 (Vampiric Embrace HP leech)

/** @type { ( attacker: Character, damaged: Character, damageValue: number, damageType: WeatherType ) => boolean } */
function onDamageDeal(attacker, damaged, damageValue, damageType)
{
	if (!ValidateObject(attacker) || !ValidateObject(damaged) || damageValue <= 0)
		return true;

	// Only if actually in Vampiric Embrace
	if (attacker.GetTag("necroForm") !== "vampire")
		return true;

	// Optional: melee-only leech (enable by setting Vamp_LeechMeleeOnly=1 on the char)
	if ((attacker.GetTag("Vamp_LeechMeleeOnly")|0) === 1 && !attacker.InRange(damaged, 1))
		return true;

	// OSI behavior is a flat 20% life leech from damage dealt
	var pct = parseFloat(attacker.GetTag("Vamp_LeechPct")) || 20.0; // allow tuning via tag; default 20%
	if (pct <= 0) return true;

	var leeched = Math.floor(damageValue * (pct / 100.0));
	if (leeched <= 0) return true;

	// Heal attacker (engine typically caps at max HP)
	attacker.health = (attacker.health|0) + leeched;

	// (optional) tiny cue
	attacker.SoundEffect(0x44D, true);

	return true;
}
