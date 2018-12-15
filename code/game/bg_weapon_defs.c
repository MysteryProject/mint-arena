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

bgweapon_defs_t bg_weapons[] = 
{
    {
        WP_GAUNTLET,
        WP_GAUNTLET,
        400,
        qtrue,
        qtrue
    },
    {
        WP_MACHINEGUN,
        WP_MACHINEGUN,
        100,
        qtrue,
        qfalse
    },
    {
        WP_SHOTGUN,
        WP_SHOTGUN,
        1000,
        qtrue,
        qfalse
    },
    {
        WP_GRENADE_LAUNCHER,
        WP_GRENADE_LAUNCHER,
        800,
        qtrue,
        qfalse
    },    
    {
        WP_ROCKET_LAUNCHER,
        WP_ROCKET_LAUNCHER,
        800,
        qtrue,
        qfalse,
    },
    {
        WP_LIGHTNING,
        WP_LIGHTNING,
        50,
        qtrue,
        qfalse
    },
    {
        WP_PLASMAGUN,
        WP_PLASMAGUN,
        100,
        qtrue,
        qfalse
    },
    {
        WP_RAILGUN,
        WP_RAILGUN,
        1500,
        qtrue,
        qfalse
    },
    {
        WP_BFG,
        WP_BFG,
        200,
        qtrue,
        qfalse
    },
    // new weapons
    {
        WP_TAPRIFLE,
        WP_MACHINEGUN,
        1,
        qfalse,
        qfalse
    },
    {
        WP_AUTOSHOTTY,
        WP_SHOTGUN,
        280,
        qtrue,
        qfalse
    },
    {
        WP_MINIRAIL,
        WP_RAILGUN,
        750,
        qtrue,
        qfalse
    },
    {
        WP_IMPACT_CANNON,
        WP_PLASMAGUN,
        700,
        qtrue,
        qfalse
    }
};

int bg_numWeapons = ARRAY_LEN(bg_weapons);

bgweapon_defs_t *BG_GetWeaponDefinition(weapon_t weapon)
{
    int i;

    for (i = 0; i < bg_numWeapons; i++)
        if (bg_weapons[i].weapon == weapon)
            return &bg_weapons[i];

    return NULL;
}
