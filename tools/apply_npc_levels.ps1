param(
	[switch]$Apply,
	[switch]$Report
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
$npcRoot = Join-Path $repoRoot "data/dfndata/npc"
$utf8NoBom = [System.Text.UTF8Encoding]::new($false)
$strictUtf8 = [System.Text.UTF8Encoding]::new($false, $true)
$windows1252 = [System.Text.Encoding]::GetEncoding(1252)

function Read-DfnText([string]$path)
{
	$bytes = [System.IO.File]::ReadAllBytes($path)
	try
	{
		return [pscustomobject]@{ Text = $strictUtf8.GetString($bytes); Encoding = $utf8NoBom }
	}
	catch [System.Text.DecoderFallbackException]
	{
		# A few legacy DFNs contain Windows-1252 author-name characters.
		return [pscustomobject]@{ Text = $windows1252.GetString($bytes); Encoding = $windows1252 }
	}
}

# Curated 1-50 combat tiers. Names are normalized across expansion suffixes,
# gender variants and summon variants before this table is consulted.
$levelOverrides = @{}
function Set-NpcLevel([int]$level, [string[]]$sections)
{
	foreach ($section in $sections)
	{
		$levelOverrides[$section.ToLowerInvariant()] = $level
	}
}

Set-NpcLevel 1 @(
	"bird", "canary", "cat", "chicken", "cow", "crow", "goat", "hind", "jackrabbit",
	"pig", "rabbit", "rat", "raven", "sheep", "squirrel", "tropicalbird", "walrus"
)
Set-NpcLevel 2 @(
	"boar", "bullfrog", "dog", "dolphin", "falcon", "hart", "hawk", "horse", "llama",
	"mongbat", "mountaingoat", "packhorse", "packllama", "seahorse", "sewerrat", "slime"
)
Set-NpcLevel 3 @(
	"boneaxeman", "crane", "frostooze", "giantrat", "headless", "skeleton", "snake"
)
Set-NpcLevel 4 @(
	"blackbear", "brownbear", "bull", "eagle", "greatermongbat", "greywolf", "graywolf",
	"panther", "shade", "shadowwisp", "snowleopard", "timberwolf", "whitewolf", "zombie"
)
Set-NpcLevel 5 @(
	"cougar", "ferret", "gazerlarva", "giantspider", "gorilla", "hellcat"
)
Set-NpcLevel 6 @(
	"desertostard", "forestostard", "frenziedostard", "gaman", "gianttoad", "grizbear",
	"lizardman", "orc", "ratman"
)
Set-NpcLevel 7 @(
	"direwolf", "frostspider", "orccaptain", "orcscout", "polarbear", "ratmanarcher",
	"scorpion", "silverwolf"
)
Set-NpcLevel 8 @(
	"chaosdaemon", "fandancer", "harpy", "imp", "kappa", "largehellcat", "orcbomber",
	"orcchopper", "orcmage", "ratmanmage", "shadowfiend", "teradrone"
)
Set-NpcLevel 10 @(
	"corpser", "evilmage", "gargoyle", "giantblackwidow", "hellhound", "ogre", "ettin",
	"orclord", "spectre", "wraith"
)
Set-NpcLevel 12 @(
	"bonemage", "gazer", "giantserpent", "icefiend", "iceserpent", "lavalizard", "mummy",
	"reaper", "skeletalmage", "troll"
)
Set-NpcLevel 14 @(
	"airele", "bronzeele", "copperele", "dullcopperele", "earthele", "fireele", "goldele",
	"shadowironele", "stonegargoyle", "waterele", "winterreaper"
)
Set-NpcLevel 15 @(
	"boneknight", "chaosdragoon", "craftedgolem", "golem", "lavaserpent", "meercaptain",
	"meerwarrior", "skeletalknight", "stoneharpy", "swampdragon"
)
Set-NpcLevel 16 @(
	"crystalele", "frosttroll", "gargoyleenforcer", "iceele", "ophapprentice", "ophshaman",
	"ophwarrior", "black_solen_worker", "red_solen_worker"
)
Set-NpcLevel 18 @(
	"centaur", "cyclops", "deepseaserpent", "energyvortex", "enslavedgargoyle", "firegargoyle",
	"killerpumpkin", "seaserpent", "snowele", "spectralarmor", "terawarrior", "wisp"
)
Set-NpcLevel 20 @(
	"agapiteele", "black_solen_warrior", "red_solen_warrior", "dreadspider", "ethwarrior",
	"giantbeetle", "ophenforcer", "pixie", "whippingvine", "wyvern"
)
Set-NpcLevel 22 @(
	"bogthing", "firebeetle", "gargoyledestroyer", "ophknight", "quagmire", "swamptentacle",
	"veriteele", "valoriteele"
)
Set-NpcLevel 24 @(
	"black_queen", "red_queen", "black_solen_infilrator_warrior", "red_solen_infilrator_warrior",
	"jukawarrior", "plaguebeast", "rottingcorpse", "yomotsuwarrior"
)
Set-NpcLevel 26 @(
	"acidele", "arcanedaemon", "black_infilrator_queen", "red_infilrator_queen", "krampus_minion",
	"moloch", "ophavenger", "ophjusticar", "ophzealot", "yomotsupriest"
)
Set-NpcLevel 28 @(
	"bakekitsune", "bogling", "chaosdragoonelite", "eldergazer", "efreet", "evilmagelord",
	"meermage", "raiju", "yomotsuelder"
)
Set-NpcLevel 30 @(
	"daemon", "exodusoverseer", "gargoylealchemist", "gargoylemason", "kirin", "nightmare",
	"oni", "ophmatriarch", "phoenix", "skeletalmount", "titan", "unicorn"
)
Set-NpcLevel 32 @(
	"ambitious_black_solen_queen", "ambitious_red_solen_queen", "black_solen_matriarch",
	"red_solen_matriarch", "bloodele", "poisonele"
)
Set-NpcLevel 34 @(
	"arcticogrelord", "blackthornjuggernaut", "graydrake", "reddrake", "drake", "firesteed",
	"ogrelord"
)
Set-NpcLevel 36 @(
	"balron", "betrayer", "hiryu", "jukalord", "jukamage", "serpentinedragon"
)
Set-NpcLevel 38 @(
	"graydragon", "reddragon", "dragon", "exodusminion", "meereternal", "teraavenger"
)
Set-NpcLevel 40 @(
	"ancientlich", "armeddaemon", "exodus", "exodusminionlord", "lichlord", "phoenix"
)
Set-NpcLevel 42 @(
	"doomtree", "deathwillow", "reptalon", "succubus", "whitewyrm"
)
Set-NpcLevel 44 @(
	"chargerofthefallen", "cusidhe", "shadowwyrm", "skeletaldragon", "tentacles_of_the_harrower"
)
Set-NpcLevel 46 @("ancientwyrm")
Set-NpcLevel 48 @("theharrower", "trueharrower")
Set-NpcLevel 50 @(
	"abyssmalhorror", "barracoon", "bonedaemon", "darkfather", "darknightcreeper", "dragonturtle",
	"dreadhorn", "fleshrenderer", "impaler", "interredgrizzle", "krampus", "ladymelisande",
	"lordoaks", "mephitis", "neira", "paroxysmus", "rikktor", "semidar", "serado",
	"shadowknight", "shimmeringeffusion", "silvani"
)

# Hand-tuned exceptions where raw fame, hit points or inherited expansion data
# do not represent the creature's practical combat tier well.
Set-NpcLevel 2 @("ferret")
Set-NpcLevel 5 @("alligator")
Set-NpcLevel 8 @("bogling")
Set-NpcLevel 10 @("cursed", "ghoul", "shade")
Set-NpcLevel 12 @("gargoylealchemist", "gargoylemason")
Set-NpcLevel 14 @("treefellow", "silverserpent")
Set-NpcLevel 16 @("orcbrute")
Set-NpcLevel 22 @("graydrake", "reddrake", "drake")
Set-NpcLevel 24 @("lysander_gathenwale", "grimmoch_drummel", "morg_bergen", "tavara_sewel")
Set-NpcLevel 26 @("icefiend")
Set-NpcLevel 28 @("fandancer", "lesserhiryu", "lich")
Set-NpcLevel 30 @("energyvortex", "kraken", "teramatriarch")
Set-NpcLevel 38 @("lichlord")
Set-NpcLevel 42 @("yamandon")
Set-NpcLevel 45 @("travesty")
Set-NpcLevel 46 @("ancientlich")
Set-NpcLevel 50 @("theharrower", "trueharrower")
Set-NpcLevel 24 @("zealot_knight_male", "zealot_knight_female")
Set-NpcLevel 28 @("zealot_summoner_male", "zealot_summoner_female")

$humanCombatLevels = @{
	"guard" = 50; "murderer-archmage" = 18; "pirate" = 8; "fighter" = 8; "paladin" = 10;
	"warrior" = 8; "brigand" = 10; "thief" = 8; "ranger" = 8; "golemcontroller" = 18;
	"executioner" = 22; "chaosdragoon" = 22; "chaosdragoonelite" = 30;
	"lordbritish" = 50; "lordblackthorn" = 50; "dupre" = 45; "garriot" = 50;
	"savageshaman" = 14; "savagewarrior" = 12; "savagerider" = 16;
	"zealot_knight" = 24; "zealot_summoner" = 28
}

$hiddenHumanRoles = @(
	"male", "female", "banker", "whealer", "sailor", "beggar", "noble", "thief_guildmaster",
	"townthief", "peasant", "gypsy", "seeker", "jailer", "towncrier", "artist", "gambler",
	"judge", "mayor", "monk", "prisoner", "sculptor", "gypsybanker", "shepherd"
)

function Get-NormalizedSection([string]$section)
{
	$name = $section.ToLowerInvariant()
	$name = $name -replace "_(aos|lbr|se|t2a|tol)$", ""
	$name = $name -replace "-summon$", ""
	$name = $name -replace "^(m_|f_)", ""
	$name = $name -replace "^(dark|mane|pure)(nightmare)$", '$2'
	$name = $name -replace "^ethereal", ""
	$name = $name -replace "^armored(swampdragon)$", '$1'
	$name = $name -replace "^lesser_?hiryu$", "lesserhiryu"
	return $name
}

function Get-MaxNumber([string]$value)
{
	if ([string]::IsNullOrWhiteSpace($value)) { return 0 }
	$numbers = [regex]::Matches($value, "-?\d+") | ForEach-Object { [int64]$_.Value }
	if (-not $numbers) { return 0 }
	return [int64](($numbers | Measure-Object -Maximum).Maximum)
}

$files = Get-ChildItem $npcRoot -Recurse -File -Filter "*.dfn" | Where-Object {
	$_.Name -ne "namelists.dfn" -and $_.FullName -notmatch "[\\/]npclists[\\/]"
}

$entries = [System.Collections.Generic.List[object]]::new()
$entryIndex = @{}
foreach ($file in $files)
{
	$fileText = Read-DfnText $file.FullName
	$text = $fileText.Text
	$matches = [regex]::Matches($text, "(?ms)^\s*\[(?<section>[^\]]+)\]\s*\r?\n(?<body>.*?)(?=^\s*\[|\z)")
	foreach ($match in $matches)
	{
		$properties = @{}
		foreach ($property in [regex]::Matches($match.Groups["body"].Value, "(?mi)^\s*([A-Z][A-Z0-9_]*)\s*=\s*([^/\r\n]*)"))
		{
			$key = $property.Groups[1].Value.ToUpperInvariant()
			if (-not $properties.ContainsKey($key)) { $properties[$key] = $property.Groups[2].Value.Trim() }
		}
		$entry = [pscustomobject]@{
			File = $file
			Section = $match.Groups["section"].Value.Trim()
			Body = $match.Groups["body"].Value
			Properties = $properties
		}
		$entries.Add($entry)
		$entryIndex[$entry.Section.ToLowerInvariant()] = $entry
	}
}

$metricCache = @{}
function Get-EntryMetrics($entry, [System.Collections.Generic.HashSet[string]]$seen)
{
	$key = ($entry.File.FullName + "|" + $entry.Section).ToLowerInvariant()
	if ($metricCache.ContainsKey($key)) { return $metricCache[$key] }
	if (-not $seen.Add($key)) { return @{ Fame = 0L; Health = 0L; Damage = 0L } }

	$fame = Get-MaxNumber $entry.Properties["FAME"]
	$health = [math]::Max((Get-MaxNumber $entry.Properties["HPMAX"]), (Get-MaxNumber $entry.Properties["HP"]))
	$health = [math]::Max($health, (Get-MaxNumber $entry.Properties["STR"]))
	$damage = Get-MaxNumber $entry.Properties["DAMAGE"]

	foreach ($getKey in @("GET", "GETUO", "GETT2A", "GETUOR", "GETTD", "GETLBR", "GETAOS", "GETSE", "GETML", "GETSA", "GETHS", "GETTOL"))
	{
		$reference = $entry.Properties[$getKey]
		if ($reference -and $entryIndex.ContainsKey($reference.ToLowerInvariant()))
		{
			$parentMetrics = Get-EntryMetrics $entryIndex[$reference.ToLowerInvariant()] $seen
			$fame = [math]::Max($fame, $parentMetrics.Fame)
			$health = [math]::Max($health, $parentMetrics.Health)
			$damage = [math]::Max($damage, $parentMetrics.Damage)
		}
	}

	$seen.Remove($key) | Out-Null
	$result = @{ Fame = $fame; Health = $health; Damage = $damage }
	$metricCache[$key] = $result
	return $result
}

function Get-AssignedLevel($entry)
{
	$normalized = Get-NormalizedSection $entry.Section
	$leafName = $entry.File.Name.ToLowerInvariant()

	if ($leafName -in @("femalevendors.dfn", "malevendors.dfn", "vendors.dfn", "quest_givers.dfn"))
	{
		return 0
	}
	if ($humanCombatLevels.ContainsKey($normalized))
	{
		return [int]$humanCombatLevels[$normalized]
	}
	if ($normalized -in $hiddenHumanRoles)
	{
		return 0
	}
	if ($levelOverrides.ContainsKey($normalized))
	{
		return [int]$levelOverrides[$normalized]
	}

	$metrics = Get-EntryMetrics $entry ([System.Collections.Generic.HashSet[string]]::new())
	$fameLevel = [math]::Ceiling($metrics.Fame / 450.0)
	$healthLevel = [math]::Ceiling($metrics.Health / 40.0)
	$damageLevel = [math]::Ceiling($metrics.Damage / 2.0)
	return [int][math]::Clamp([math]::Max(1, [math]::Max($fameLevel, [math]::Max($healthLevel, $damageLevel))), 1, 50)
}

$changedFiles = 0
$taggedSections = 0
$levelCounts = @{}
$assignments = [System.Collections.Generic.List[object]]::new()
foreach ($file in $files)
{
	$fileText = Read-DfnText $file.FullName
	$text = $fileText.Text
	$updated = [regex]::Replace($text, "(?ms)^(?<header>\s*\[(?<section>[^\]]+)\]\s*\r?\n)(?<body>.*?)(?=^\s*\[|\z)", {
		param($match)
		$section = $match.Groups["section"].Value.Trim()
		if ($section -match "^base") { return $match.Value }

		$entryKey = $section.ToLowerInvariant()
		$entry = $entryIndex[$entryKey]
		if ($null -eq $entry) { return $match.Value }

		$body = $match.Groups["body"].Value
		if ($body -notmatch "(?m)^\s*\{") { return $match.Value }
		$level = Get-AssignedLevel $entry
		$script:taggedSections++
		$script:assignments.Add([pscustomobject]@{
			File = $file.FullName.Substring($repoRoot.Length + 1)
			Section = $section
			Level = $level
		})
		if (-not $script:levelCounts.ContainsKey($level)) { $script:levelCounts[$level] = 0 }
		$script:levelCounts[$level]++

		$tagLine = "CUSTOMINTTAG=npcLevel $level"
		if ($body -match "(?mi)^\s*CUSTOMINTTAG\s*=\s*npcLevel\s+[^\r\n]*")
		{
			$body = [regex]::Replace($body, "(?mi)^\s*CUSTOMINTTAG\s*=\s*npcLevel\s+[^\r\n]*", $tagLine, 1)
		}
		else
		{
			$newline = if ($text.Contains("`r`n")) { "`r`n" } else { "`n" }
			$closingMatch = [regex]::Match($body, "(?m)^[ \t]*}[ \t]*(?://[^\r\n]*)?(?:\r?\n|$)")
			if (-not $closingMatch.Success) { return $match.Value }
			$closingBrace = $closingMatch.Index
			$body = $body.Insert($closingBrace, $tagLine + $newline)
		}
		return $match.Groups["header"].Value + $body
	})

	if ($updated -ne $text)
	{
		$changedFiles++
		if ($Apply)
		{
			[System.IO.File]::WriteAllText($file.FullName, $updated, $fileText.Encoding)
		}
	}
}

$mode = if ($Apply) { "Updated" } else { "Would update" }
Write-Output "$mode $changedFiles files and assigned $taggedSections NPC sections."
Write-Output "Level distribution:"
$levelCounts.GetEnumerator() | Sort-Object Name | ForEach-Object { Write-Output ("  {0,2}: {1}" -f $_.Name, $_.Value) }
if ($Report)
{
	$assignments | Sort-Object File, Section | Format-Table -AutoSize
}
