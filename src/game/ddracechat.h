/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */

// This file can be included several times.

#ifndef CHAT_COMMAND
#define CHAT_COMMAND(name, params, flags, callback, userdata, help)
#endif

CHAT_COMMAND("credits", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConCredits, this, "Shows the credits of the DDNet mod")
CHAT_COMMAND("rules", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConRules, this, "Shows the server rules")
CHAT_COMMAND("emote", "?s[emote name] i[duration in seconds]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConEyeEmote, this, "Sets your tee's eye emote")
CHAT_COMMAND("eyeemote", "?s['on'|'off'|'toggle']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConSetEyeEmote, this, "Toggles use of standard eye-emotes on/off, eyeemote s, where s = on for on, off for off, toggle for toggle and nothing to show current status")
CHAT_COMMAND("settings", "?s[configname]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConSettings, this, "Shows gameplay information for this server")
CHAT_COMMAND("help", "?r[command]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConHelp, this, "Shows help to command r, general help if left blank")
CHAT_COMMAND("info", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConInfo, this, "Shows info about this server")
CHAT_COMMAND("list", "?s[filter]", CFGFLAG_CHAT, ConList, this, "List connected players with optional case-insensitive substring matching filter")
CHAT_COMMAND("me", "r[message]", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConMe, this, "Like the famous irc command '/me says hi' will display '<yourname> says hi'")
CHAT_COMMAND("w", "s[player name] r[message]", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConWhisper, this, "Whisper something to someone (private message)")
CHAT_COMMAND("whisper", "s[player name] r[message]", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConWhisper, this, "Whisper something to someone (private message)")
CHAT_COMMAND("c", "r[message]", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConConverse, this, "Converse with the last person you whispered to (private message)")
CHAT_COMMAND("converse", "r[message]", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConConverse, this, "Converse with the last person you whispered to (private message)")
CHAT_COMMAND("pause", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTogglePause, this, "Toggles pause")
CHAT_COMMAND("spec", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConToggleSpec, this, "Toggles spec (if not available behaves as /pause)")
CHAT_COMMAND("pausevoted", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTogglePauseVoted, this, "Toggles pause on the currently voted player")
CHAT_COMMAND("specvoted", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConToggleSpecVoted, this, "Toggles spec on the currently voted player")
CHAT_COMMAND("dnd", "", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConDND, this, "Toggle Do Not Disturb (no chat and server messages)")
CHAT_COMMAND("mapinfo", "?r[map]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConMapInfo, this, "Show info about the map with name r gives (current map by default)")
CHAT_COMMAND("timeout", "?s[code]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTimeout, this, "Set timeout protection code s")
CHAT_COMMAND("practice", "?i['0'|'1']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConPractice, this, "Enable cheats (currently only /rescue) for your current team's run, but you can't earn a rank")
CHAT_COMMAND("swap", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConSwap, this, "Request to swap your tee with another team member")
CHAT_COMMAND("save", "?r[code]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConSave, this, "Save team with code r.")
CHAT_COMMAND("load", "?r[code]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConLoad, this, "Load with code r. /load to check your existing saves")
CHAT_COMMAND("map", "?r[map]", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConMap, this, "Vote a map by name")

CHAT_COMMAND("rankteam", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTeamRank, this, "Shows the team rank of player with name r (your team rank by default)")
CHAT_COMMAND("teamrank", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTeamRank, this, "Shows the team rank of player with name r (your team rank by default)")

CHAT_COMMAND("rank", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConRank, this, "Shows the rank of player with name r (your rank by default)")
CHAT_COMMAND("top5team", "?s[player name] ?i[rank to start with]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTeamTop5, this, "Shows five team ranks of the ladder or of a player beginning with rank i (1 by default, -1 for worst)")
CHAT_COMMAND("teamtop5", "?s[player name] ?i[rank to start with]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTeamTop5, this, "Shows five team ranks of the ladder or of a player beginning with rank i (1 by default, -1 for worst)")
CHAT_COMMAND("top", "?i[rank to start with]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTop, this, "Shows the top ranks of the global and regional ladder beginning with rank i (1 by default, -1 for worst)")
CHAT_COMMAND("top5", "?i[rank to start with]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTop, this, "Shows the top ranks of the global and regional ladder beginning with rank i (1 by default, -1 for worst)")
CHAT_COMMAND("times", "?s[player name] ?i[number of times to skip]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTimes, this, "/times ?s?i shows last 5 times of the server or of a player beginning with name s starting with time i (i = 1 by default, -1 for first)")
CHAT_COMMAND("points", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConPoints, this, "Shows the global points of a player beginning with name r (your rank by default)")
CHAT_COMMAND("top5points", "?i[number]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTopPoints, this, "Shows five points of the global point ladder beginning with rank i (1 by default)")
CHAT_COMMAND("timecp", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTimeCP, this, "Set your checkpoints based on another player")

CHAT_COMMAND("team", "?i[id]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConJoinTeam, this, "Lets you join team i (shows your team if left blank)")
CHAT_COMMAND("lock", "?i['0'|'1']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConLockTeam, this, "Toggle team lock so no one else can join and so the team restarts when a player dies. /lock 0 to unlock, /lock 1 to lock.")
CHAT_COMMAND("unlock", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConUnlockTeam, this, "Unlock a team")
CHAT_COMMAND("invite", "r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConInviteTeam, this, "Invite a person to a locked team")

CHAT_COMMAND("showothers", "?i['0'|'1'|'2']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConShowOthers, this, "Whether to show players from other teams or not (off by default), optional i = 0 for off, i = 1 for on, i = 2 for own team only")
CHAT_COMMAND("showall", "?i['0'|'1']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConShowAll, this, "Whether to show players at any distance (off by default), optional i = 0 for off else for on")
CHAT_COMMAND("specteam", "?i['0'|'1']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConSpecTeam, this, "Whether to show players from other teams when spectating (on by default), optional i = 0 for off else for on")
CHAT_COMMAND("ninjajetpack", "?i['0'|'1']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConNinjaJetpack, this, "Whether to use ninja jetpack or not. Makes jetpack look more awesome")
CHAT_COMMAND("saytime", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConSayTime, this, "Privately messages someone's current time in this current running race (your time by default)")
CHAT_COMMAND("saytimeall", "", CFGFLAG_CHAT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, ConSayTimeAll, this, "Publicly messages everyone your current time in this current running race")
CHAT_COMMAND("time", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTime, this, "Privately shows you your current time in this current running race in the broadcast message")
CHAT_COMMAND("timer", "?s['gametimer'|'broadcast'|'both'|'none'|'cycle']", CFGFLAG_CHAT | CFGFLAG_SERVER, ConSetTimerType, this, "Personal Setting of showing time in either broadcast or game/round timer, timer s, where s = broadcast for broadcast, gametimer for game/round timer, cycle for cycle, both for both, none for no timer and nothing to show current status")
CHAT_COMMAND("r", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConRescue, this, "Teleport yourself out of freeze (use sv_rescue 1 to enable this feature)")
CHAT_COMMAND("rescue", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConRescue, this, "Teleport yourself out of freeze (use sv_rescue 1 to enable this feature)")
CHAT_COMMAND("tp", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTele, this, "Teleport yourself to player or to where you are spectating if no player name is given")
CHAT_COMMAND("teleport", "?r[player name]", CFGFLAG_CHAT | CFGFLAG_SERVER, ConTele, this, "Teleport yourself to player or to where you are spectating if no player name is given")

CHAT_COMMAND("kill", "", CFGFLAG_CHAT | CFGFLAG_SERVER, ConProtectedKill, this, "Kill yourself when kill-protected during a long game (use f1, kill for regular kill)")

#undef CHAT_COMMAND
