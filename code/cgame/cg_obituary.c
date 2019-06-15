#include "cg_obituary.h"

static obituary_t obitStack[OBIT_MAX_VISABLE];
static int numObits;

static qboolean obitInit = qfalse;

/*
===================
CG_ClearObit
===================
*/
static void CG_ClearObit(int obit)
{
    if (obit >= 0 && obit < OBIT_MAX_VISABLE)
        memset(&obitStack[obit], 0, sizeof(obituary_t));
}

/*
===================
CG_MoveObitInStack
===================
*/
static void CG_MoveObitInStack(int obit, int move)
{
    if (obit + move < OBIT_MAX_VISABLE)
        memcpy(&obitStack[obit + move], &obitStack[obit], sizeof(obituary_t));

    CG_ClearObit(obit);
}

/*
===================
CG_CountObits
===================
*/
static void CG_CountObits(void)
{
    int i, count = 0;

    for (i = 0; i < OBIT_MAX_VISABLE; i++)
    {
        if (obitStack[i].target[0] != '\0')
            count++;
    }

    numObits = count;
}

/*
===================
CG_ShiftObituaryStack
===================
*/
static void CG_ShiftObituaryStack(void)
{
    int i;

    for (i = OBIT_MAX_VISABLE - 1; i >= 0; i--)
    {
        if (obitStack[i].target[0] != '\0')
        {
            CG_MoveObitInStack(i, 1);
        }
    }

    CG_ClearObit(0);
}

char *weaponStrings[] =
{
    "??",
    "Gauntlet",
    "Machinegun",
    "Shotgun",
    "Grenade Launcher",
    "Rocket Launcher",
    "Lightning Gun",
    "Railgun",
    "Plasmagun",
    "BFG10K",
    "GPHook",
    "Mini-Railgun",
    "Auto-Shotty",
    "Tap-Rifle",
    "Impact Cannon"
};

// these are just for logging, the client prints its own messages
char *modNames2[] = {
    "MOD_UNKNOWN",
    "MOD_SHOTGUN",
    "MOD_AUTOSHOTTY",
    "MOD_GAUNTLET",
    "MOD_MACHINEGUN",
    "MOD_TAPRIFLE",
    "MOD_GRENADE",
    "MOD_GRENADE_SPLASH",
    "MOD_ROCKET",
    "MOD_ROCKET_SPLASH",
    "MOD_PLASMA",
    "MOD_PLASMA_SPLASH",
    "MOD_IMPACTCANNON",
    "MOD_IMPACTCANNON_SPLASH",
    "MOD_RAILGUN",
    "MOD_MINIRAIL",
    "MOD_LIGHTNING",
    "MOD_BFG",
    "MOD_BFG_SPLASH",
    "MOD_WATER",
    "MOD_SLIME",
    "MOD_LAVA",
    "MOD_CRUSH",
    "MOD_TELEFRAG",
    "MOD_FALLING",
    "MOD_SUICIDE",
    "MOD_TARGET_LASER",
    "MOD_TRIGGER_HURT",
#ifdef MISSIONPACK
    "MOD_NAIL",
    "MOD_CHAINGUN",
    "MOD_PROXIMITY_MINE",
    "MOD_KAMIKAZE",
    "MOD_JUICED",
#endif
    "MOD_KNOCKOUT",
    "MOD_GRAPPLE",
    "MOD_SUICIDE_TEAM_CHANGE"
};

/*
===================
CG_AddObituary
===================
*/
static void CG_AddObituary(char *attackerName, char *targetName, team_t attackerTeam, team_t targetTeam, int attacker, int target, int mod)
{
    char *weapString = "Unk";

    if (numObits >= OBIT_MAX_VISABLE)
    {
        CG_ShiftObituaryStack();
        CG_CountObits();
    }

    if (!targetName)
    {
        Com_Printf("CG_AddObituary: called with NULL targetName!\n");
        return;
    }

    CG_ShiftObituaryStack();
    CG_CountObits();

    switch (mod)
    {
        case MOD_GAUNTLET:
            weapString = weaponStrings[WP_GAUNTLET];
            break;

        case MOD_MACHINEGUN:
            weapString = weaponStrings[WP_MACHINEGUN];
            break;

        case MOD_TAPRIFLE:
            weapString = weaponStrings[WP_TAPRIFLE];
            break;

        case MOD_SHOTGUN:
            weapString = weaponStrings[WP_SHOTGUN];
            break;

        case MOD_AUTOSHOTTY:
            weapString = weaponStrings[WP_AUTOSHOTTY];
            break;

        case MOD_GRENADE:
        case MOD_GRENADE_SPLASH:
            weapString = weaponStrings[WP_GRENADE_LAUNCHER];
            break;

        case MOD_ROCKET:
        case MOD_ROCKET_SPLASH:
            weapString = weaponStrings[WP_ROCKET_LAUNCHER];
            break;

        case MOD_LIGHTNING:
            weapString = weaponStrings[WP_LIGHTNING];
            break;

        case MOD_PLASMA:
        case MOD_PLASMA_SPLASH:
            weapString = weaponStrings[WP_PLASMAGUN];
            break;

        case MOD_IMPACTCANNON:
        case MOD_IMPACTCANNON_SPLASH:
            weapString = weaponStrings[WP_IMPACT_CANNON];
            break;

        case MOD_RAILGUN:
            weapString = weaponStrings[WP_RAILGUN];
            break;

        case MOD_MINIRAIL:
            weapString = weaponStrings[WP_MINIRAIL];
            break;

        case MOD_BFG:
        case MOD_BFG_SPLASH:
            weapString = weaponStrings[WP_BFG];
            break;

        //case MI_HEADSHOT:
        //    weapString = "HS";
        //    break;

        case MI_SAMETEAM:
            weapString = "Teamkill";
            break;

        case MOD_TELEFRAG:
            weapString = "Telefrag";
            break;

        case MOD_CRUSH:
            weapString = "Crushed";
            break;

        case MOD_LAVA:
            weapString = "Lava";
            break;

        case MOD_WATER:
            weapString = "Drowned";
            break;

        case MOD_TRIGGER_HURT:
        case MOD_FALLING:
            weapString = "Fell";
            break;
            
        case MOD_SUICIDE:
            weapString = "Suicide";
            break;

        case MOD_KNOCKOUT:
            weapString = "Knocked Out";
            break;

        case MOD_EXPLODED:
            weapString = "SpEeD";
            break;

        case MI_NONE:
        case MOD_UNKNOWN:
        default:
            weapString = modNames2[mod];
            break;
    }

    Q_strncpyz(obitStack[0].weapon, weapString, sizeof(obitStack[0].weapon));

    if (attackerName)
        Q_strncpyz(obitStack[0].attacker, attackerName, sizeof(obitStack[0].attacker));

    Q_strncpyz(obitStack[0].target, targetName, sizeof(obitStack[0].target));

    obitStack[0].attackerTeam = attackerTeam;
    obitStack[0].targetTeam = targetTeam;
    obitStack[0].attackerPlayerNum = attacker;
    obitStack[0].targetPlayerNum = attacker;

    obitStack[0].time = cg.time;
}

/*
===================
CG_InitObituary
===================
*/
static void CG_InitObituary(void)
{
    /*
    qhandle_t handles[2];
    vec3_t origin = {70, 0, 0};
    vec3_t angles = {0, 180, 0};
    vec3_t spacing = {-10, -10, 0};

    handles[0] = cgs.media.gibSkull;
    handles[1] = 0;

    CG_RegisterModelInfo(MI_NONE, handles, spacing, origin, angles);
    */
    
    obitInit = qtrue;
}

void CG_ParseObituary(entityState_t *ent)
{
    int mod;
    int target, attacker;
    playerInfo_t *targetpi, *attackerpi;
    const char *targetInfo;
    const char *attackerInfo;
    char targetName[MAX_NAME_LENGTH];
    char attackerName[MAX_NAME_LENGTH];
    int i;

    if (!obitInit)
        CG_InitObituary();

    target = ent->otherEntityNum;
    attacker = ent->otherEntityNum2;
    targetpi = &cgs.playerinfo[target];
    attackerpi = &cgs.playerinfo[attacker];
    mod = ent->eventParm;

    if (target < 0 || target >= MAX_CLIENTS)
    {
        Com_Printf("CG_ParseObituary: target out of range!\n");
        return;
    }

    if (attacker < 0 || attacker >= MAX_CLIENTS)
    {
        attacker = ENTITYNUM_WORLD;
        attackerInfo = NULL;
    }
    else
    {
        attackerInfo = CG_ConfigString(CS_PLAYERS + attacker);
    }

    targetInfo = CG_ConfigString(CS_PLAYERS + target);

    if (!targetInfo)
    {
        Com_Printf("CG_ParseObituary: could not get target info!\n");
        return;
    }

    Q_strncpyz(targetName, Info_ValueForKey(targetInfo, "n"), sizeof(targetName));
    strcat(targetName, S_COLOR_WHITE);

    if ((attacker == ENTITYNUM_WORLD) || (attacker == target))
    {
        CG_AddObituary(NULL, targetName, -1, targetpi->team, attacker, target, (attacker == target) ? MOD_SUICIDE : mod);
        return;
    }

    if (CG_LocalPlayerState(attacker))
    {
        char *s;
        playerState_t *ps;

        for (i = 0; i < CG_MaxSplitView(); i++)
        {
            if (attacker != cg.snap->pss[i].playerNum)
                continue;

            ps = &cg.snap->pss[i];

            if (cgs.gametype == GT_GUNGAME)
            {
                s = va(S_COLOR_GREEN "%s" S_COLOR_WHITE " complete\nYou are %s place",
                       BG_FindItemForWeapon(bg_gunGameInfo.levels[ps->persistant[PERS_GUNGAME_LEVEL] - 1])->displayName,
                       CG_PlaceString(ps->persistant[PERS_RANK] + 1, qfalse));
            }
            else if (cgs.gametype < GT_TEAM)
            {
                s = va("You killed %s\n%s place with %i", targetName,
                       CG_PlaceString(ps->persistant[PERS_RANK] + 1, qfalse),
                       ps->persistant[PERS_SCORE]);
            }
            else
            {
                s = va("You killed %s", targetName);
            }

            CG_CenterPrint(i, s, SCREEN_HEIGHT * 0.3, 0.35);
        }
    }

    if (!attackerInfo)
    {
        attacker = ENTITYNUM_WORLD;
        strcpy(attackerName, "noname");
    }
    else
    {
        Q_strncpyz(attackerName, Info_ValueForKey(attackerInfo, "n"), sizeof(attackerName));
        strcat(attackerName, S_COLOR_WHITE);

        for (i = 0; i < CG_MaxSplitView(); i++)
        {
            if (target == cg.snap->pss[i].playerNum)
                Q_strncpyz(cg.localPlayers[i].killerName, attackerName, sizeof(cg.localPlayers[i].killerName));
        }
    }

    if (attacker != ENTITYNUM_WORLD)
    {
        if (cgs.gametype >= GT_TEAM && targetpi->team == attackerpi->team)
            mod = MI_SAMETEAM;
        //else if (ent->eFlags & EF_HEADSHOT)
        //    mod = MI_HEADSHOT;

        CG_AddObituary(attackerName, targetName, attackerpi->team, targetpi->team, attacker, target, mod);
        return;
    }

    CG_AddObituary(NULL, targetName, -1, targetpi->team, attacker, target, MI_NONE);
}

static float obitRedColor[4] = {1.0f, 0.1f, 0.1f, 1.0f};
static float obitBlueColor[4] = {0.1f, 0.1f, 1.0f, 1.0f};
static float obitEnemyColor[4] = {0.1f, 1.0f, 1.0f, 1.0f};
static float obitNormalColor[4] = {1.0f, 0.5f, 0.1f, 1.0f};
static float obitMyColor[4] = {1.0f, 0.9f, 0.2f, 1.0f};

/*
===================
CG_DrawObituary
===================
*/
void CG_DrawObituary(void)
{
    int i;
    float *color;
    float drawColor[4];
    float *teamColor1;
    float *teamColor2;
    float x, y;
    char *weapString;
    float space = cg_obituarySpacing[cg.cur_localPlayerNum].integer;

    if (cg.numViewports != 1)
        space *= cg_splitviewTextScale.value;
    else
        space *= cg_hudTextScale.value;

    if (!obitInit)
        CG_InitObituary();

    CG_SetScreenPlacement(PLACE_RIGHT, PLACE_TOP);

    for (i = OBIT_MAX_VISABLE - 1; i >= 0; i--)
    {
        if (obitStack[i].target[0] == '\0')
            continue;

        color = CG_FadeColor(obitStack[i].time, 1000 * atof(cg_obituaryFadetime[cg.cur_localPlayerNum].string));

        if (!color)
        {
            CG_ClearObit(i);
            CG_CountObits();
            continue;
        }

        if (cgs.gametype >= GT_TEAM)
        {
            teamColor1 = (obitStack[i].attackerTeam == TEAM_RED) ? obitRedColor : obitBlueColor;
            teamColor2 = (obitStack[i].targetTeam == TEAM_RED) ? obitRedColor : obitBlueColor;
        }
        else
        {
            teamColor1 = teamColor2 = obitEnemyColor;
        }

        if (obitStack[i].attackerPlayerNum == cg.cur_lc->playerNum)
            teamColor1 = obitMyColor;
        else if (obitStack[i].targetPlayerNum == cg.cur_lc->playerNum)
            teamColor2 = obitMyColor;

        RemoveColorEscapeSequences(obitStack[i].attacker);
        RemoveColorEscapeSequences(obitStack[i].target);

        y = 480 - (cg_obituaryY[cg.cur_localPlayerNum].integer + ((OBIT_MAX_VISABLE - 1 - i) * space));
        x = 640 - cg_obituaryX[cg.cur_localPlayerNum].integer;

        weapString = va("[%s]", obitStack[i].weapon);

        if (obitStack[i].attacker[0] != '\0')
        {
            // center
            x -= ((CG_DrawStrlen(va("%s %s %s", obitStack[i].attacker, weapString, obitStack[i].target), fontFlags)) / 2) + space;

            drawColor[0] = color[0] * teamColor1[0];
            drawColor[1] = color[1] * teamColor1[1];
            drawColor[2] = color[2] * teamColor1[2];
            drawColor[3] = color[3] * teamColor1[3];

            // center all prints by first '['
            //x -= CG_DrawStrlen(obitStack[i].attacker, fontFlags);

            trap_R_SetColor(drawColor);
            CG_DrawString(x, y, obitStack[i].attacker, fontFlags, drawColor);
            x += CG_DrawStrlen(obitStack[i].attacker, fontFlags);
        }
        else
        {
            // center
            x -= ((CG_DrawStrlen(va("%s %s", weapString, obitStack[i].target), fontFlags)) / 2) + space;
        }

        if (cg.numViewports != 1)
            x += cg_obituaryGap[cg.cur_localPlayerNum].integer * cg_splitviewTextScale.value;
        else
            x += cg_obituaryGap[cg.cur_localPlayerNum].integer * cg_hudTextScale.value;

        drawColor[0] = color[0] * 255;
        drawColor[1] = color[1] * 255;
        drawColor[2] = color[2] * 255;
        drawColor[3] = color[3] * obitNormalColor[3];

        trap_R_SetColor(drawColor);
        CG_DrawString(x, y, weapString, fontFlags, drawColor);
        x += CG_DrawStrlen(weapString, fontFlags);

        if (cg.numViewports != 1)
            x += cg_obituaryGap[cg.cur_localPlayerNum].integer * cg_splitviewTextScale.value;
        else
            x += cg_obituaryGap[cg.cur_localPlayerNum].integer * cg_hudTextScale.value;

        drawColor[0] = color[0] * teamColor2[0];
        drawColor[1] = color[1] * teamColor2[1];
        drawColor[2] = color[2] * teamColor2[2];
        drawColor[3] = color[3] * teamColor2[3];

        trap_R_SetColor(drawColor);
        CG_DrawString(x, y, obitStack[i].target, fontFlags, drawColor);

        trap_R_SetColor(NULL);
    }
}
