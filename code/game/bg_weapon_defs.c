/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/
//
// g_weapon_defs.c -- weapon definitions for server only
#include "../qcommon/q_shared.h"
#include "bg_public.h"

#define MAX_WEAPONDEFS WP_NUM_WEAPONS
#define MAX_TOKENS 1024

const char *weaponEnumStrings[] =
{
    "WP_NONE",
    "WP_GAUNTLET",
    "WP_MACHINEGUN",
    "WP_SHOTGUN",
    "WP_GRENADE_LAUNCHER",
    "WP_ROCKET_LAUNCHER",
    "WP_LIGHTNING",
    "WP_RAILGUN",
    "WP_PLASMAGUN",
    "WP_BFG",
    "WP_GRAPPLING_HOOK",
    "WP_MINIRAIL",
    "WP_AUTOSHOTTY",
    "WP_TAPRIFLE",
    "WP_IMPACT_CANNON",
    "WP_NUM_WEAPONS"
};

bgweapon_defs_t bg_weapons[MAX_WEAPONDEFS];
int bg_numWeapons = 0;

void BG_ParseWeaponDefsJSON(void)
{
    int i, j, k, r;
    jsmn_parser parser;
    jsmntok_t tokens[MAX_TOKENS];
    char json[8192];

    BG_LoadFileContents(json, "weapon_info.json");

    jsmn_init(&parser);
    r = jsmn_parse(&parser, json, strlen(json), tokens, sizeof(tokens) / sizeof(tokens[0]));

    //CG_Printf("Parsing json: %d\n", strlen(json), json);

    if (r < 0)
    {
        trap_Print( va( S_COLOR_RED "BG_ParseWeaponDefsJSON: Failed to parse JSON: %d\n", r ) );
		return;
    }

    if (tokens[0].type != JSMN_ARRAY)
    {
        trap_Print(S_COLOR_RED "BG_ParseWeaponDefsJSON: JSON top object is not an array\n");
        return;
    }

    for (i = 1; i < r; i++)
    {
        if (bg_numWeapons >= MAX_WEAPONDEFS)
        {
            Com_Printf("BG_ParseWeaponDefsJSON: Reached max weapondefs %d\n", MAX_WEAPONDEFS);
            break;
        }

        //CG_Printf("Token: %.*s\n", tokens[i].end - tokens[i].start, json + tokens[i].start);

        if (tokens[i].type == JSMN_OBJECT)
        {
            for (j = 1; i + j < r; j += 2)
            {
                jsmntok_t keyToken = tokens[i + j];
                jsmntok_t valueToken;
                int keyLen, valueLen;
                char key[128];
                char value[MAX_QPATH * 10];

                //Com_Printf("Tokenkey: %.*s\n", keyToken.end - keyToken.start, json + keyToken.start);

                if (keyToken.type == JSMN_OBJECT)
                {
                    i += j - 1;
                    break; // we hit the next item
                }

                valueToken = tokens[i + j + 1];

                keyLen = keyToken.end - keyToken.start;
                valueLen = valueToken.end - valueToken.start;

                if (keyLen <= 0)
                {
                    Com_Printf("BG_ParseWeaponDefsJSON: key length <= 0\n");
                    break;
                }

                if (valueLen <= 0)
                {
                    Com_Printf("BG_ParseWeaponDefsJSON: value length <= 0\n");
                    break;
                }

                Q_strncpyz(key, json + keyToken.start, sizeof(key));
                Q_strncpyz(value, json + valueToken.start, sizeof(value));

                key[keyLen] = '\0';
                value[valueLen] = '\0';

                //Com_Printf("Parsing token %s: %s\n", key, value);

                if (Q_stricmp(key, "weapon") == 0)
                {
                    int a;
                    for(a = WP_NONE + 1; a < WP_NUM_WEAPONS; a++)
                    {
                        if (!Q_stricmp(value, weaponEnumStrings[a]))
                            bg_weapons[bg_numWeapons].weapon = a;
                    }
                } 
                else if (Q_stricmp(key, "ammoType") == 0)
                {
                    int a;
                    for(a = WP_NONE + 1; a < WP_NUM_WEAPONS; a++)
                    {
                        if (!Q_stricmp(value, weaponEnumStrings[a]))
                            bg_weapons[bg_numWeapons].ammoType = a;
                    }
                } 
                else if (Q_stricmp(key, "attackDelay") == 0)
                {
                    bg_weapons[bg_numWeapons].attackDelay = atoi(value);
                }
                else if (Q_stricmp(key, "autoAttack") == 0)
                {
                    bg_weapons[bg_numWeapons].autoAttack = !Q_stricmp(value, "true");
                }
                else if (Q_stricmp(key, "oneHanded") == 0)
                {
                    bg_weapons[bg_numWeapons].oneHanded = !Q_stricmp(value, "true");
                }
                else if (Q_stricmp(key, "fireInfo") == 0)
                {
                    bg_weapons[bg_numWeapons].fireInfo = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(bg_weapons[bg_numWeapons].fireInfo, value, strlen(value) + 1);
                }
                else
                {
                    Com_Printf("BG_ParseWeaponDefsJSON: Could not parse token %s: %s\n", key, value);
                }
            }
            Com_Printf("BG_ParseWeaponDefsJSON: Parsed info for %s\n", weaponEnumStrings[bg_weapons[bg_numWeapons].weapon]);
            bg_numWeapons++;
        }
    }
}

bgweapon_defs_t *BG_GetWeaponDefinition(weapon_t weapon)
{
    int i;

    for (i = 0; i < bg_numWeapons; i++)
        if (bg_weapons[i].weapon == weapon)
            return &bg_weapons[i];

    return NULL;
}
