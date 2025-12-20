/// <reference path="../definitions.d.ts" />
// @ts-check

// Character Script Trigger: 6005 (Wraith Form mana leech)

/** @type { ( attacker: Character, damaged: Character, damageValue: number, damageType: WeatherType ) => boolean } */
function onDamageDeal(attacker, damaged, damageValue, damageType)
{
    if (!ValidateObject(attacker) || !ValidateObject(damaged) || damageValue <= 0)
        return true;

    // Only if actually in wraith form (paranoia check)
    if (attacker.GetTag("necroForm") !== "wraith")
        return true;

    // Spirit Speak 0..1000 -> percent = SS / 50  (100.0 => 20%, 120.0 => 24%)
    var ss = attacker.skills.spiritspeak | 0;
    var leechPct = ss / 50.0;

    if (leechPct <= 0)
        return true;

    var drain = Math.floor(damageValue * (leechPct / 100.0));
    if (drain <= 0)
        return true;

    // Cap by target's available mana
    var actual = Math.min(drain, Math.max(0, damaged.mana | 0));
    if (actual <= 0)
        return true;

    // Drain from target, give to attacker
    damaged.mana = Math.max(0, (damaged.mana | 0) - actual);
    attacker.mana = (attacker.mana | 0) + actual;

    // Small cue (optional)
    attacker.SoundEffect(0x44D, true);

    return true;
}
