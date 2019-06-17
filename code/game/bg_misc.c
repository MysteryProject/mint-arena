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
// bg_misc.c -- both games misc functions, all completely stateless

#include "../qcommon/q_shared.h"
#include "bg_public.h"

/*QUAKED item_***** ( 0 0 0 ) (-16 -16 -16) (16 16 16) suspended
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up.  If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait"	override the default wait before respawning.  -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
*/

// entityState_t fields
#define	NETF(x) (size_t)&((entityState_t*)0)->x, 1
#define	NETA(x) (size_t)&((entityState_t*)0)->x, ARRAY_LEN( ((entityState_t*)0)->x )

vmNetField_t	bg_entityStateFields[] = 
{
{ NETF(pos.trTime), 32 },
{ NETF(pos.trBase[0]), 0 },
{ NETF(pos.trBase[1]), 0 },
{ NETF(pos.trDelta[0]), 0 },
{ NETF(pos.trDelta[1]), 0 },
{ NETF(pos.trBase[2]), 0 },
{ NETF(apos.trBase[1]), 0 },
{ NETF(pos.trDelta[2]), 0 },
{ NETF(apos.trBase[0]), 0 },
{ NETF(event), 10 },
{ NETF(angles2[1]), 0 },
{ NETF(eType), 8 },
{ NETF(torsoAnim), 8 },
{ NETF(eventParm), 8 },
{ NETF(legsAnim), 8 },
{ NETF(groundEntityNum), GENTITYNUM_BITS },
{ NETF(pos.trType), 8 },
{ NETF(eFlags), 32 },
{ NETF(otherEntityNum), GENTITYNUM_BITS },
{ NETF(weapon), MAX( 8, WEAPONNUM_BITS ) }, // because 'weapon' is used for things besides weaponnum it must be minimum of 8 bits
{ NETF(playerNum), 8 },
{ NETF(angles[1]), 0 },
{ NETF(pos.trDuration), 32 },
{ NETF(apos.trType), 8 },
{ NETF(origin[0]), 0 },
{ NETF(origin[1]), 0 },
{ NETF(origin[2]), 0 },
{ NETF(contents), 32 },
{ NETF(collisionType), 16 },
{ NETF(mins[0]), 0 },
{ NETF(mins[1]), 0 },
{ NETF(mins[2]), 0 },
{ NETF(maxs[0]), 0 },
{ NETF(maxs[1]), 0 },
{ NETF(maxs[2]), 0 },
{ NETF(powerups), MAX_POWERUPS },
{ NETF(modelindex), MODELINDEX_BITS },
{ NETF(otherEntityNum2), GENTITYNUM_BITS },
{ NETF(loopSound), 8 },
{ NETF(tokens), 8 },
{ NETF(team), 8 },
{ NETF(origin2[2]), 0 },
{ NETF(origin2[0]), 0 },
{ NETF(origin2[1]), 0 },
{ NETF(modelindex2), MODELINDEX_BITS },
{ NETF(angles[0]), 0 },
{ NETF(time), 32 },
{ NETF(apos.trTime), 32 },
{ NETF(apos.trDuration), 32 },
{ NETF(apos.trBase[2]), 0 },
{ NETF(apos.trDelta[0]), 0 },
{ NETF(apos.trDelta[1]), 0 },
{ NETF(apos.trDelta[2]), 0 },
{ NETF(time2), 32 },
{ NETF(angles[2]), 0 },
{ NETF(angles2[0]), 0 },
{ NETF(angles2[2]), 0 },
{ NETF(constantLight), 32 },
{ NETF(dl_intensity), 32 },
{ NETF(density), 10},
{ NETF(frame), 16 },
{ NETF(damage), 16 }
};

int bg_numEntityStateFields = ARRAY_LEN(bg_entityStateFields);

// playerState_t fields
#define	PSF(x) (size_t)&((playerState_t*)0)->x, 1
#define	PSA(x) (size_t)&((playerState_t*)0)->x, ARRAY_LEN( ((playerState_t*)0)->x )

vmNetField_t	bg_playerStateFields[] = 
{
{ PSF(commandTime), 32 },				
{ PSF(origin[0]), 0 },
{ PSF(origin[1]), 0 },
{ PSF(bobCycle), 8 },
{ PSF(velocity[0]), 0 },
{ PSF(velocity[1]), 0 },
{ PSF(viewangles[1]), 0 },
{ PSF(viewangles[0]), 0 },
{ PSF(weaponTime), -16 },
{ PSF(origin[2]), 0 },
{ PSF(velocity[2]), 0 },
{ PSF(legsTimer), 8 },
{ PSF(pm_time), -16 },
{ PSF(eventSequence), 16 },
{ PSF(torsoAnim), 8 },
{ PSF(movementDir), 4 },
{ PSF(events[0]), 8 },
{ PSF(legsAnim), 8 },
{ PSF(events[1]), 8 },
{ PSF(pm_flags), 16 },
{ PSF(groundEntityNum), GENTITYNUM_BITS },
{ PSF(weaponstate), 4 },
{ PSF(eFlags), 32 },
{ PSA(stats), -16 },
{ PSA(persistant), -16 },
{ PSA(ammo), -16 },
{ PSA(powerups), 32 },
{ PSF(contents), 32 },
{ PSF(collisionType), 16 },
{ PSF(linked), 1 },
{ PSF(externalEvent), 10 },
{ PSF(gravity), 16 },
{ PSF(speed), 16 },
{ PSF(delta_angles[1]), 16 },
{ PSF(externalEventParm), 8 },
{ PSF(viewheight), -8 },
{ PSF(damageEvent), 8 },
{ PSF(damageYaw), 8 },
{ PSF(damagePitch), 8 },
{ PSF(damageCount), 8 },
{ PSF(tokens), 8 },
{ PSF(pm_type), 8 },					
{ PSF(delta_angles[0]), 16 },
{ PSF(delta_angles[2]), 16 },
{ PSF(torsoTimer), 12 },
{ PSF(eventParms[0]), 8 },
{ PSF(eventParms[1]), 8 },
{ PSF(playerNum), 8 },
{ PSF(weapon), WEAPONNUM_BITS },
{ PSF(viewangles[2]), 0 },
{ PSF(grapplePoint[0]), 0 },
{ PSF(grapplePoint[1]), 0 },
{ PSF(grapplePoint[2]), 0 },
{ PSF(jumppad_ent), GENTITYNUM_BITS },
{ PSF(loopSound), 16 },
{ PSF(mins[0]), 0 },
{ PSF(mins[1]), 0 },
{ PSF(mins[2]), 0 },
{ PSF(maxs[0]), 0 },
{ PSF(maxs[1]), 0 },
{ PSF(maxs[2]), 0 },
{ PSF(jumpTime), -16 },
{ PSF(crouchTime), -16 },
{ PSF(walljumpTime), -16 },
{ PSF(walljumpCount), -16 }
};

int bg_numPlayerStateFields = ARRAY_LEN(bg_playerStateFields);

// may not contain spaces, dpmaster will reject the server
const char *bg_netGametypeNames[GT_MAX_GAME_TYPE] = {
	"FFA",
	"GunGame",
	"Tournament",
	"SP",
	"TeamDM",
	"CTF",
#ifdef MISSIONPACK
	"1FCTF",
	"Overload",
	"Harvester"
#endif
};

const char *bg_displayGametypeNames[GT_MAX_GAME_TYPE] = {
	"Free For All",
	"Gun Game",
	"Tournament",
	"Single Player",
	"Team Deathmatch",
	"Capture the Flag",
#ifdef MISSIONPACK
	"One Flag CTF",
	"Overload",
	"Harvester"
#endif
};

/*
==============
BG_CheckSpawnEntity
==============
*/
qboolean BG_CheckSpawnEntity( const bgEntitySpawnInfo_t *info ) {
	int			i, gametype;
	char		*s, *value, *gametypeName;
	static char *gametypeNames[GT_MAX_GAME_TYPE] = {"ffa", "tournament", "single", "team", "ctf"
#ifdef MISSIONPACK
		, "oneflag", "obelisk", "harvester"
#endif
		};

	gametype = info->gametype;

	// check for "notsingle" flag
	if ( gametype == GT_SINGLE_PLAYER ) {
		info->spawnInt( "notsingle", "0", &i );
		if ( i ) {
			return qfalse;
		}
	}

	// check for "notteam" flag (GT_FFA, GT_TOURNAMENT, GT_SINGLE_PLAYER)
	if ( gametype >= GT_TEAM ) {
		info->spawnInt( "notteam", "0", &i );
		if ( i ) {
			return qfalse;
		}
	} else {
		info->spawnInt( "notfree", "0", &i );
		if ( i ) {
			return qfalse;
		}
	}

#ifdef MISSIONPACK
	info->spawnInt( "notta", "0", &i );
	if ( i ) {
			return qfalse;
	}
#else
	info->spawnInt( "notq3a", "0", &i );
	if ( i ) {
			return qfalse;
	}
#endif

	if( info->spawnString( "!gametype", NULL, &value ) ) {
		if( gametype >= 0 && gametype < GT_MAX_GAME_TYPE ) {
			gametypeName = gametypeNames[gametype];

			s = strstr( value, gametypeName );
			if( s ) {
				return qfalse;
			}
		}
	}

	if( info->spawnString( "gametype", NULL, &value ) ) {
		if( gametype >= 0 && gametype < GT_MAX_GAME_TYPE ) {
			gametypeName = gametypeNames[gametype];

			s = strstr( value, gametypeName );
			if( !s ) {
				return qfalse;
			}
		}
	}

	return qtrue;
}

/*
==============
BG_FindItemForPowerup
==============
*/
gitem_t	*BG_FindItemForPowerup( powerup_t pw ) {
	int		i;

	for ( i = 0 ; i < bg_numItems ; i++ ) {
		if ( (bg_itemlist[i].type == IT_POWERUP || 
				bg_itemlist[i].type == IT_TEAM ||
				bg_itemlist[i].type == IT_PERSISTANT_POWERUP) && 
				bg_itemlist[i].localIndex == pw ) {
			return &bg_itemlist[i];
		}
	}

	return NULL;
}


/*
==============
BG_FindItemForHoldable
==============
*/
gitem_t	*BG_FindItemForHoldable( holdable_t pw ) {
	int		i;

	for ( i = 0 ; i < bg_numItems ; i++ ) {
		if ( bg_itemlist[i].type == IT_HOLDABLE && bg_itemlist[i].localIndex == pw ) {
			return &bg_itemlist[i];
		}
	}

	Com_Error( ERR_DROP, "HoldableItem not found" );

	return NULL;
}


/*
===============
BG_FindItemForWeapon

===============
*/
gitem_t	*BG_FindItemForWeapon( weapon_t weapon ) {
	gitem_t	*it;
	
	for ( it = bg_itemlist + 1 ; it->classname ; it++) {
		if ( it->type == IT_WEAPON && it->localIndex == weapon ) {
			return it;
		}
	}

	Com_Error( ERR_DROP, "Couldn't find item for weapon %i", weapon);
	return NULL;
}

/*
===============
BG_FindItemForAmmo

===============
*/
gitem_t	*BG_FindItemForAmmo( weapon_t weapon ) {
	gitem_t	*it;

	for ( it = bg_itemlist + 1 ; it->classname ; it++) {
		if ( it->type == IT_WEAPON && it->localIndex == weapon ) {
			return &bg_itemlist[it->ammoId];
		}
	}

	return NULL;
}

/*
===============
BG_FindItem

===============
*/
gitem_t	*BG_FindItem( const char *pickupName ) {
	gitem_t	*it;

	for ( it = bg_itemlist + 1 ; it->classname ; it++ ) {
		if ( !Q_stricmp( it->displayName, pickupName ) )
			return it;
	}

	return NULL;
}

/*
===============
BG_FindItemForAmmoName

===============
*/
gitem_t	*BG_FindItemForAmmoName( const char *ammoName ) {
	gitem_t	*it;
	const char *compare = va("ammo_%s", ammoName);

	for ( it = bg_itemlist + 1 ; it->classname ; it++ ) {
		if ( !Q_stricmp( it->classname, compare) )
			return it;
	}

	return NULL;
}

/*
===============
BG_FindItem

===============
*/
gitem_t *BG_FindItemByClassname(const char *classname)
{
	gitem_t *it;

	for (it = bg_itemlist + 1; it->classname; it++)
	{
		if (!Q_stricmp(it->classname, classname))
			return it;
	}

	return NULL;
}

/*
============
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make
grabbing them easier
============
*/
qboolean	BG_PlayerTouchesItem( playerState_t *ps, entityState_t *item, int atTime ) {
	vec3_t		origin;

	BG_EvaluateTrajectory( &item->pos, atTime, origin );

	// we are ignoring ducked differences here
	if ( ps->origin[0] - origin[0] > 44
		|| ps->origin[0] - origin[0] < -50
		|| ps->origin[1] - origin[1] > 36
		|| ps->origin[1] - origin[1] < -36
		|| ps->origin[2] - origin[2] > 36
		|| ps->origin[2] - origin[2] < -36 ) {
		return qfalse;
	}

	return qtrue;
}



/*
================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up.
This needs to be the same for client side prediction and server use.
================
*/
qboolean BG_CanItemBeGrabbed( int gametype, int knockout, const entityState_t *ent, const playerState_t *ps ) {
	gitem_t	*item;
	int		upperBound;
	qboolean redArmor = qfalse;

	if ( ent->modelindex < 1 || ent->modelindex >= BG_NumItems() ) {
		Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: index out of range" );
	}

	item = BG_ItemForItemNum( ent->modelindex );

	switch( item->type ) {
	case IT_WEAPON:
		return qtrue;	// weapons are always picked up

	case IT_AMMO:
		if (ps->ammo[item->localIndex] >= 200)
		{
			return qfalse;		// can't hold any more
		}
		return qtrue;

	case IT_ARMOR:
#ifdef MISSIONPACK
		if( BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_SCOUT ) {
			return qfalse;
		}

		if( BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_GUARD ) {
			upperBound = ps->stats[STAT_MAX_HEALTH];
		}
		else
#endif
		if (item->amount == 100)
			return (ps->stats[STAT_ARMOR] >= ARMOR_LVL2_MAX) ? qfalse : qtrue;
		else if (item->amount == 50)
		{
			if (ps->stats[STAT_ARMOR_LEVEL] <= ARMOR_LVL1)
				return (ps->stats[STAT_ARMOR] >= ARMOR_LVL1_MAX) ? qfalse : qtrue;

			return (ps->stats[STAT_ARMOR] > ARMOR_LVL2_BREAKPOINT) ? qfalse : qtrue;
		}

		return qtrue;

	case IT_HEALTH:
		// small and mega healths will go over the max, otherwise
		// don't pick up if already at max
		if (knockout)
		{
			if (ps->stats[STAT_DAMAGE] <= 0)
				return qfalse;
				
			return qtrue;
		} 
		else
		{
#ifdef MISSIONPACK
			if( BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_GUARD ) {
			}
			else
#endif
			if ( item->amount == 5 || item->amount == 100 ) {
				if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] * 2 ) {
					return qfalse;
				}
				return qtrue;
			}

			if ( ps->stats[STAT_HEALTH] >= ps->stats[STAT_MAX_HEALTH] ) {
				return qfalse;
			}
			return qtrue;
		}
	case IT_POWERUP:
#ifdef MISSIONPACK
		// scout overrides haste
		if (item->giTag == PW_HASTE && BG_ItemForItemNum( ps->stats[STAT_PERSISTANT_POWERUP] )->giTag == PW_SCOUT ) {
			return qfalse;
		}
#endif
		return qtrue;

#ifdef MISSIONPACK
	case IT_PERSISTANT_POWERUP:
		// can only hold one item at a time
		if ( ps->stats[STAT_PERSISTANT_POWERUP] ) {
			return qfalse;
		}

		// check team only
		if( ent->team != 255 && ( ps->persistant[PERS_TEAM] != ent->team ) ) {
			return qfalse;
		}

		return qtrue;
#endif

	case IT_TEAM: // team items, such as flags
#ifdef MISSIONPACK		
		if( gametype == GT_1FCTF ) {
			// neutral flag can always be picked up
			if( item->giTag == PW_NEUTRALFLAG ) {
				return qtrue;
			}
			if (ps->persistant[PERS_TEAM] == TEAM_RED) {
				if (item->giTag == PW_BLUEFLAG  && ps->powerups[PW_NEUTRALFLAG] ) {
					return qtrue;
				}
			} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
				if (item->giTag == PW_REDFLAG  && ps->powerups[PW_NEUTRALFLAG] ) {
					return qtrue;
				}
			}
		}
#endif
		if( gametype == GT_CTF ) {
			// ent->modelindex2 is non-zero on items if they are dropped
			// we need to know this because we can pick up our dropped flag (and return it)
			// but we can't pick up our flag at base
			if (ps->persistant[PERS_TEAM] == TEAM_RED) {
				if (item->localIndex == PW_BLUEFLAG ||
					(item->localIndex == PW_REDFLAG && ent->modelindex2) ||
					(item->localIndex == PW_REDFLAG && ps->powerups[PW_BLUEFLAG]) )
					return qtrue;
			} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
				if (item->localIndex == PW_REDFLAG ||
					(item->localIndex == PW_BLUEFLAG && ent->modelindex2) ||
					(item->localIndex == PW_BLUEFLAG && ps->powerups[PW_REDFLAG]) )
					return qtrue;
			}
		}

#ifdef MISSIONPACK
		if( gametype == GT_HARVESTER ) {
			return qtrue;
		}
#endif
		return qfalse;

	case IT_HOLDABLE:
		// can only hold one item at a time
		if ( ps->stats[STAT_HOLDABLE_ITEM] ) {
			return qfalse;
		}
		return qtrue;

	case IT_BAD:
		Com_Error( ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD" );

	default:
		Com_Printf( "BG_CanItemBeGrabbed: unknown enum %d\n", item->type );
	}

	return qfalse;
}

//======================================================================

/*
================
BG_EvaluateTrajectory

================
*/
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result ) {
	float		deltaTime;
	float		phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorCopy( tr->trBase, result );
		break;
	case TR_LINEAR:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = sin( deltaTime * M_PI * 2 );
		VectorMA( tr->trBase, phase, tr->trDelta, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			atTime = tr->trTime + tr->trDuration;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		if ( deltaTime < 0 ) {
			deltaTime = 0;
		}
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorMA( tr->trBase, deltaTime, tr->trDelta, result );
		result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime;		// FIXME: local gravity...
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trType );
		break;
	}
}

/*
================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time
================
*/
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result ) {
	float	deltaTime;
	float	phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear( result );
		break;
	case TR_LINEAR:
		VectorCopy( tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = cos( deltaTime * M_PI * 2 );	// derivative of sin = cos
		phase *= 0.5;
		VectorScale( tr->trDelta, phase, result );
		break;
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		VectorCopy( tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
		break;
	default:
		Com_Error( ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trType );
		break;
	}
}

char *eventnames[] = {
	"EV_NONE",

	"EV_FOOTSTEP",
	"EV_FOOTSTEP_METAL",
	"EV_FOOTSPLASH",
	"EV_FOOTWADE",
	"EV_SWIM",

	"EV_STEP_4",
	"EV_STEP_8",
	"EV_STEP_12",
	"EV_STEP_16",

	"EV_FALL_SHORT",
	"EV_FALL_MEDIUM",
	"EV_FALL_FAR",

	"EV_JUMP_PAD",			// boing sound at origin", jump sound on player

	"EV_JUMP",
	"EV_WATER_TOUCH",	// foot touches
	"EV_WATER_LEAVE",	// foot leaves
	"EV_WATER_UNDER",	// head touches
	"EV_WATER_CLEAR",	// head leaves

	"EV_ITEM_PICKUP",			// normal item pickups are predictable
	"EV_GLOBAL_ITEM_PICKUP",	// powerup / team sounds are broadcast to everyone

	"EV_NOAMMO",
	"EV_CHANGE_WEAPON",
	"EV_FIRE_WEAPON",

	"EV_USE_ITEM0",
	"EV_USE_ITEM1",
	"EV_USE_ITEM2",
	"EV_USE_ITEM3",
	"EV_USE_ITEM4",
	"EV_USE_ITEM5",
	"EV_USE_ITEM6",
	"EV_USE_ITEM7",
	"EV_USE_ITEM8",
	"EV_USE_ITEM9",
	"EV_USE_ITEM10",
	"EV_USE_ITEM11",
	"EV_USE_ITEM12",
	"EV_USE_ITEM13",
	"EV_USE_ITEM14",
	"EV_USE_ITEM15",

	"EV_ITEM_RESPAWN",
	"EV_ITEM_POP",
	"EV_PLAYER_TELEPORT_IN",
	"EV_PLAYER_TELEPORT_OUT",

	"EV_GRENADE_BOUNCE",		// eventParm will be the soundindex

	"EV_GENERAL_SOUND",
	"EV_GLOBAL_SOUND",		// no attenuation
	"EV_GLOBAL_TEAM_SOUND",

	"EV_BULLET_HIT_FLESH",
	"EV_BULLET_HIT_WALL",

	"EV_MISSILE_HIT",
	"EV_MISSILE_MISS",
	"EV_MISSILE_MISS_METAL",
	"EV_RAILTRAIL",
	"EV_MINIRAILTRAIL",
	"EV_SHOTGUN",
	"EV_AUTOSHOTTY",

	"EV_PAIN",
	"EV_DEATH1",
	"EV_DEATH2",
	"EV_DEATH3",
	"EV_OBITUARY",

	"EV_POWERUP_QUAD",
	"EV_POWERUP_BATTLESUIT",
	"EV_POWERUP_REGEN",

	"EV_SCOREPLUM",			// score plum

//#ifdef MISSIONPACK
	"EV_PROXIMITY_MINE_STICK",
	"EV_PROXIMITY_MINE_TRIGGER",
	"EV_KAMIKAZE",			// kamikaze explodes
	"EV_OBELISKEXPLODE",		// obelisk explodes
	"EV_OBELISKPAIN",		// obelisk pain
	"EV_INVUL_IMPACT",		// invulnerability sphere impact
	"EV_JUICED",				// invulnerability juiced effect
	"EV_LIGHTNINGBOLT",		// lightning bolt bounced of invulnerability sphere
//#endif

	"EV_DEBUG_LINE",
	"EV_STOPLOOPINGSOUND",
	"EV_TAUNT",
	"EV_TAUNT_YES",
	"EV_TAUNT_NO",
	"EV_TAUNT_FOLLOWME",
	"EV_TAUNT_GETFLAG",
	"EV_TAUNT_GUARDBASE",
	"EV_TAUNT_PATROL",

	"EV_GUNGAMESWAP"
};

/*
===============
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers
===============
*/
void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps ) {

#ifdef _DEBUG
	{
		char buf[256];
		trap_Cvar_VariableStringBuffer("showevents", buf, sizeof(buf));
		if ( atof(buf) != 0 ) {
#ifdef GAME
			Com_Printf(" game event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#else
			Com_Printf("Cgame event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#endif
		}
	}
#endif
	ps->events[ps->eventSequence & (MAX_PS_EVENTS-1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS-1)] = eventParm;
	ps->eventSequence++;
}

/*
========================
BG_TouchJumpPad
========================
*/
void BG_TouchJumpPad( playerState_t *ps, entityState_t *jumppad ) {
	vec3_t	angles;
	float p;
	int effectNum;

	// spectators don't use jump pads
	if ( ps->pm_type != PM_NORMAL ) {
		return;
	}

	// flying characters don't hit bounce pads
	if ( ps->powerups[PW_FLIGHT] ) {
		return;
	}

	// if we didn't hit this same jumppad the previous frame
	// then don't play the event sound again if we are in a fat trigger
	if ( ps->jumppad_ent != jumppad->number ) {

		vectoangles( jumppad->origin2, angles);
		p = fabs( AngleNormalize180( angles[PITCH] ) );
		if( p < 45 ) {
			effectNum = 0;
		} else {
			effectNum = 1;
		}
		BG_AddPredictableEventToPlayerstate( EV_JUMP_PAD, effectNum, ps );
	}
	// remember hitting this jumppad this frame
	ps->jumppad_ent = jumppad->number;
	ps->jumppad_frame = ps->pmove_framecount;
	// give the player the velocity from the jumppad
	VectorCopy( jumppad->origin2, ps->velocity );
}

/*
========================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityState( playerState_t *ps, entityState_t *s, qboolean snap ) {
	int		i;

	if ( !ps->linked ) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->playerNum;

	s->pos.trType = TR_INTERPOLATE;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}
	// set the trDelta for flag direction
	VectorCopy( ps->velocity, s->pos.trDelta );

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->playerNum = ps->playerNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	s->damage = ps->stats[STAT_DAMAGE];

	s->contents = ps->contents;
	s->loopSound = ps->loopSound;
	s->tokens = ps->tokens;
	s->team = ps->persistant[PERS_TEAM];

	s->collisionType = ps->collisionType;

	VectorCopy( ps->mins, s->mins );
	VectorCopy( ps->maxs, s->maxs );
	if ( snap ) {
		SnapVector( s->mins );
		SnapVector( s->maxs );
	}
}

/*
========================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server,
and after local prediction on the client
========================
*/
void BG_PlayerStateToEntityStateExtraPolate( playerState_t *ps, entityState_t *s, int time, qboolean snap ) {
	int		i;

	if ( !ps->linked ) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->playerNum;

	s->pos.trType = TR_LINEAR_STOP;
	VectorCopy( ps->origin, s->pos.trBase );
	if ( snap ) {
		SnapVector( s->pos.trBase );
	}
	// set the trDelta for flag direction and linear prediction
	VectorCopy( ps->velocity, s->pos.trDelta );
	// set the time for linear prediction
	s->pos.trTime = time;
	// set maximum extra polation time
	s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)

	s->apos.trType = TR_INTERPOLATE;
	VectorCopy( ps->viewangles, s->apos.trBase );
	if ( snap ) {
		SnapVector( s->apos.trBase );
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->playerNum = ps->playerNum;		// ET_PLAYER looks here instead of at number
										// so corpses can also reference the proper config
	s->eFlags = ps->eFlags;
	if ( ps->stats[STAT_HEALTH] <= 0 ) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if ( ps->externalEvent ) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if ( ps->entityEventSequence < ps->eventSequence ) {
		int		seq;

		if ( ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}
		seq = ps->entityEventSequence & (MAX_PS_EVENTS-1);
		s->event = ps->events[ seq ] | ( ( ps->entityEventSequence & 3 ) << 8 );
		s->eventParm = ps->eventParms[ seq ];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;

	s->powerups = 0;
	for ( i = 0 ; i < MAX_POWERUPS ; i++ ) {
		if ( ps->powerups[ i ] ) {
			s->powerups |= 1 << i;
		}
	}

	s->damage = ps->stats[STAT_DAMAGE];

	s->contents = ps->contents;
	s->loopSound = ps->loopSound;
	s->tokens = ps->tokens;
	s->team = ps->persistant[PERS_TEAM];

	s->collisionType = ps->collisionType;

	VectorCopy( ps->mins, s->mins );
	VectorCopy( ps->maxs, s->maxs );
	if ( snap ) {
		SnapVector( s->mins );
		SnapVector( s->maxs );
	}
}

/*
========================
BG_ComposeBits
========================
*/
void BG_ComposeBits( int *msg, int *bitsUsed, int value, int bits ) {
	*msg |= ( value & ( ( 1 << bits ) - 1 ) ) << *bitsUsed;
	*bitsUsed += bits;

	if ( *bitsUsed > 32 ) {
		Com_Error( ERR_DROP, "BG_ComposeBits exceeded 32 bits" );
	}
}

/*
========================
BG_DecomposeBits
========================
*/
void BG_DecomposeBits( int msg, int *bitsUsed, int *value, int bits ) {
	if ( value ) {
		*value = ( msg >> *bitsUsed ) & ( ( 1 << bits ) - 1 );
	}
	*bitsUsed += bits;

	if ( *bitsUsed > 32 ) {
		Com_Error( ERR_DROP, "BG_DecomposeBits exceeded 32 bits" );
	}
}

/*
========================
BG_ComposeUserCmdValue
========================
*/
int BG_ComposeUserCmdValue( int weapon ) {
	int value = 0;
	int bitsUsed = 0;

	BG_ComposeBits( &value, &bitsUsed, weapon, WEAPONNUM_BITS );

	return value;
}

/*
========================
BG_DecomposeUserCmdValue
========================
*/
void BG_DecomposeUserCmdValue( int value, int *weapon ) {
	int		bitsUsed = 0;

	BG_DecomposeBits( value, &bitsUsed, weapon, WEAPONNUM_BITS );
}

/*
=============
BG_AddStringToList
=============
*/
void BG_AddStringToList( char *list, size_t listSize, int *listLength, char *name ) {
	size_t namelen;
	int val;
	char *listptr;

	namelen = strlen( name );

	if ( *listLength + namelen + 1 >= listSize ) {
		return;
	}

	for ( listptr = list; *listptr; listptr += strlen( listptr ) + 1 ) {
		val = Q_stricmp( name, listptr );
		if ( val == 0 ) {
			return;
		}
		// insert into list
		else if ( val < 0 ) {
			int moveBytes = *listLength - (int)( listptr - list ) + 1;

			memmove( listptr + namelen + 1, listptr, moveBytes );
			strncpy( listptr, name, namelen + 1 );
			*listLength += namelen + 1;
			return;
		}
	}

	strncpy( listptr, name, namelen + 1 );
	*listLength += namelen + 1;
}

/*
======================
SnapVectorTowards

Round a vector to integers for more efficient network
transmission, but make sure that it rounds towards a given point
rather than blindly truncating.  This prevents it from truncating
into a wall.
======================
*/
void SnapVectorTowards( vec3_t v, vec3_t to ) {
	int		i;

	for ( i = 0 ; i < 3 ; i++ ) {
		if ( to[i] <= v[i] ) {
			v[i] = floor(v[i]);
		} else {
			v[i] = ceil(v[i]);
		}
	}
}

/*
=============
cmdcmp
=============
*/
int cmdcmp( const void *a, const void *b ) {
	return Q_stricmp( (const char *)a, ((dummyCmd_t *)b)->name );
}

/*
=================
PC_SourceWarning
=================
*/
void PC_SourceWarning(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_YELLOW "WARNING: %s, line %d: %s\n", filename, line, string);
}

/*
=================
PC_SourceError
=================
*/
void PC_SourceError(int handle, char *format, ...) {
	int line;
	char filename[128];
	va_list argptr;
	static char string[4096];

	va_start (argptr, format);
	Q_vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	filename[0] = '\0';
	line = 0;
	trap_PC_SourceFileAndLine(handle, filename, &line);

	Com_Printf(S_COLOR_RED "ERROR: %s, line %d: %s\n", filename, line, string);
}

/*
=================
PC_CheckTokenString
=================
*/
int PC_CheckTokenString(int handle, char *string) {
	pc_token_t tok;

	if (!trap_PC_ReadToken(handle, &tok)) return qfalse;
	//if the token is available
	if (!strcmp(tok.string, string)) return qtrue;
	//
	trap_PC_UnreadToken(handle);
	return qfalse;
}

/*
=================
PC_ExpectTokenString
=================
*/
int PC_ExpectTokenString(int handle, char *string) {
	pc_token_t token;

	if (!trap_PC_ReadToken(handle, &token))
	{
		PC_SourceError(handle, "couldn't find expected %s", string);
		return qfalse;
	} //end if

	if (strcmp(token.string, string))
	{
		PC_SourceError(handle, "expected %s, found %s", string, token.string);
		return qfalse;
	} //end if
	return qtrue;
}

/*
=================
PC_ExpectTokenType
=================
*/
int PC_ExpectTokenType(int handle, int type, int subtype, pc_token_t *token) {
	char str[MAX_TOKENLENGTH];

	if (!trap_PC_ReadToken(handle, token))
	{
		PC_SourceError(handle, "couldn't read expected token");
		return qfalse;
	}

	if (token->type != type)
	{
		strcpy(str, "");
		if (type == TT_STRING) strcpy(str, "string");
		if (type == TT_LITERAL) strcpy(str, "literal");
		if (type == TT_NUMBER) strcpy(str, "number");
		if (type == TT_NAME) strcpy(str, "name");
		if (type == TT_PUNCTUATION) strcpy(str, "punctuation");
		PC_SourceError(handle, "expected a %s, found %s", str, token->string);
		return qfalse;
	}
	if (token->type == TT_NUMBER)
	{
		if ((token->subtype & subtype) != subtype)
		{
			if (subtype & TT_DECIMAL) strcpy(str, "decimal");
			else if (subtype & TT_HEX) strcpy(str, "hex");
			else if (subtype & TT_OCTAL) strcpy(str, "octal");
			else if (subtype & TT_BINARY) strcpy(str, "binary");
			else strcpy(str, "unknown");

			if (subtype & TT_LONG) strcat(str, " long");
			if (subtype & TT_UNSIGNED) strcat(str, " unsigned");
			if (subtype & TT_FLOAT) strcat(str, " float");
			if (subtype & TT_INTEGER) strcat(str, " integer");

			PC_SourceError(handle, "expected %s, found %s", str, token->string);
			return qfalse;
		}
	}
	else if (token->type == TT_PUNCTUATION)
	{
		if (token->subtype != subtype)
		{
			PC_SourceError(handle, "found %s", token->string);
			return qfalse;
		}
	}
	return qtrue;
}

/*
=================
PC_ExpectAnyToken
=================
*/
int PC_ExpectAnyToken(int handle, pc_token_t *token) {
	if (!trap_PC_ReadToken(handle, token)) {
		PC_SourceError(handle, "couldn't read expected token");
		return qfalse;
	} else {
		return qtrue;
	}
}

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
fielddef_t *FindField(fielddef_t *defs, char *name)
{
	int i;

	for (i = 0; defs[i].name; i++)
	{
		if (!strcmp(defs[i].name, name)) return &defs[i];
	} //end for
	return NULL;
} //end of the function FindField
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
qboolean ReadNumber(int source, fielddef_t *fd, void *p)
{
	pc_token_t token;
	int negative = qfalse;
	long int intval, intmin = 0, intmax = 0;
	double floatval;

	if (!PC_ExpectAnyToken(source, &token)) return 0;

	//check for minus sign
	if (token.type == TT_PUNCTUATION)
	{
		if (fd->type & FT_UNSIGNED)
		{
			PC_SourceError(source, "expected unsigned value, found %s", token.string);
			return 0;
		} //end if
		//if not a minus sign
		if (strcmp(token.string, "-"))
		{
			PC_SourceError(source, "unexpected punctuation %s", token.string);
			return 0;
		} //end if
		negative = qtrue;
		//read the number
		if (!PC_ExpectAnyToken(source, &token)) return 0;
	} //end if
	//check if it is a number
	if (token.type != TT_NUMBER)
	{
		PC_SourceError(source, "expected number, found %s", token.string);
		return 0;
	} //end if
	//check for a float value
	if (token.subtype & TT_FLOAT)
	{
		if ((fd->type & FT_TYPE) != FT_FLOAT)
		{
			PC_SourceError(source, "unexpected float");
			return 0;
		} //end if
		floatval = token.floatvalue;
		if (negative) floatval = -floatval;
		if (fd->type & FT_BOUNDED)
		{
			if (floatval < fd->floatmin || floatval > fd->floatmax)
			{
				PC_SourceError(source, "float out of range [%f, %f]", fd->floatmin, fd->floatmax);
				return 0;
			} //end if
		} //end if
		*(float *) p = (float) floatval;
		return 1;
	} //end if
	//
	intval = token.intvalue;
	if (negative) intval = -intval;
	//check bounds
	if ((fd->type & FT_TYPE) == FT_CHAR)
	{
		if (fd->type & FT_UNSIGNED) {intmin = 0; intmax = 255;}
		else {intmin = -128; intmax = 127;}
	} //end if
	if ((fd->type & FT_TYPE) == FT_INT)
	{
		if (fd->type & FT_UNSIGNED) {intmin = 0; intmax = 65535;}
		else {intmin = -32768; intmax = 32767;}
	} //end else if
	if ((fd->type & FT_TYPE) == FT_CHAR || (fd->type & FT_TYPE) == FT_INT)
	{
		if (fd->type & FT_BOUNDED)
		{
			intmin = MAX(intmin, fd->floatmin);
			intmax = MIN(intmax, fd->floatmax);
		} //end if
		if (intval < intmin || intval > intmax)
		{
			PC_SourceError(source, "value %ld out of range [%ld, %ld]", intval, intmin, intmax);
			return 0;
		} //end if
	} //end if
	else if ((fd->type & FT_TYPE) == FT_FLOAT)
	{
		if (fd->type & FT_BOUNDED)
		{
			if (intval < fd->floatmin || intval > fd->floatmax)
			{
				PC_SourceError(source, "value %ld out of range [%f, %f]", intval, fd->floatmin, fd->floatmax);
				return 0;
			} //end if
		} //end if
	} //end else if
	//store the value
	if ((fd->type & FT_TYPE) == FT_CHAR)
	{
		if (fd->type & FT_UNSIGNED) *(unsigned char *) p = (unsigned char) intval;
		else *(char *) p = (char) intval;
	} //end if
	else if ((fd->type & FT_TYPE) == FT_INT)
	{
		if (fd->type & FT_UNSIGNED) *(unsigned int *) p = (unsigned int) intval;
		else *(int *) p = (int) intval;
	} //end else
	else if ((fd->type & FT_TYPE) == FT_FLOAT)
	{
		*(float *) p = (float) intval;
	} //end else
	return 1;
} //end of the function ReadNumber
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
qboolean ReadChar(int source, fielddef_t *fd, void *p)
{
	pc_token_t token;

	if (!PC_ExpectAnyToken(source, &token)) return 0;

	//take literals into account
	if (token.type == TT_LITERAL)
	{
		*(char *) p = token.string[0];
	} //end if
	else
	{
		trap_PC_UnreadToken(source);
		if (!ReadNumber(source, fd, p)) return 0;
	} //end if
	return 1;
} //end of the function ReadChar
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int ReadString(int source, fielddef_t *fd, void *p)
{
	pc_token_t token;

	if (!PC_ExpectTokenType(source, TT_STRING, 0, &token)) return 0;
	//copy the string
	strncpy((char *) p, token.string, MAX_STRINGFIELD);
	//make sure the string is closed with a zero
	((char *)p)[MAX_STRINGFIELD-1] = '\0';
	//
	return 1;
} //end of the function ReadString

/*
=================
PC_ReadStructure
=================
*/
qboolean PC_ReadStructure(int source, structdef_t *def, void *structure)
{
	pc_token_t token;
	fielddef_t *fd;
	void *p;
	int num;

	if (!PC_ExpectTokenString(source, "{")) return 0;
	while(1)
	{
		if (!PC_ExpectAnyToken(source, &token)) return qfalse;
		//if end of structure
		if (!strcmp(token.string, "}")) break;
		//find the field with the name
		fd = FindField(def->fields, token.string);
		if (!fd)
		{
			PC_SourceError(source, "unknown structure field %s", token.string);
			return qfalse;
		} //end if
		if (fd->type & FT_ARRAY)
		{
			num = fd->maxarray;
			if (!PC_ExpectTokenString(source, "{")) return qfalse;
		} //end if
		else
		{
			num = 1;
		} //end else
		p = (void *)((byte*)structure + fd->offset);
		while (num-- > 0)
		{
			if (fd->type & FT_ARRAY)
			{
				if (PC_CheckTokenString(source, "}")) break;
			} //end if
			switch(fd->type & FT_TYPE)
			{
				case FT_CHAR:
				{
					if (!ReadChar(source, fd, p)) return qfalse;
					p = (char *) p + sizeof(char);
					break;
				} //end case
				case FT_INT:
				{
					if (!ReadNumber(source, fd, p)) return qfalse;
					p = (char *) p + sizeof(int);
					break;
				} //end case
				case FT_FLOAT:
				{
					if (!ReadNumber(source, fd, p)) return qfalse;
					p = (char *) p + sizeof(float);
					break;
				} //end case
				case FT_STRING:
				{
					if (!ReadString(source, fd, p)) return qfalse;
					p = (char *) p + MAX_STRINGFIELD;
					break;
				} //end case
				case FT_STRUCT:
				{
					if (!fd->substruct)
					{
						PC_SourceError(source, "BUG: no sub structure defined");
						return qfalse;
					} //end if
					PC_ReadStructure(source, fd->substruct, (char *) p);
					p = (char *) p + fd->substruct->size;
					break;
				} //end case
			} //end switch
			if (fd->type & FT_ARRAY)
			{
				if (!PC_ExpectAnyToken(source, &token)) return qfalse;
				if (!strcmp(token.string, "}")) break;
				if (strcmp(token.string, ","))
				{
					PC_SourceError(source, "expected a comma, found %s", token.string);
					return qfalse;
				} //end if
			} //end if
		} //end while
	} //end while
	return qtrue;
} //end of the function ReadStructure

/*
==============
RemoveColorEscapeSequences
==============
*/
void RemoveColorEscapeSequences(char *text)
{
	int i, l;

	l = 0;
	for (i = 0; text[i]; i++)
	{
		if (Q_IsColorString(&text[i]))
		{
			i++;
			continue;
		}
		if (text[i] > 0x7E)
			continue;
		text[l++] = text[i];
	}
	text[l] = '\0';
}

char **str_split(char *str, char delim, int *numSplits)
{
	char **ret;
	int retLen;
	char *c;

	if ((str == NULL) ||
		(delim == '\0'))
	{
		/* Either of those will cause problems */
		ret = NULL;
		retLen = -1;
	}
	else
	{
		retLen = 0;
		c = str;

		/* Pre-calculate number of elements */
		do
		{
			if (*c == delim)
			{
				retLen++;
			}

			c++;
		} while (*c != '\0');

		ret = trap_HeapMalloc((retLen + 1) * sizeof(*ret));
		ret[retLen] = NULL;

		c = str;
		retLen = 1;
		ret[0] = str;

		do
		{
			if (*c == delim)
			{
				ret[retLen++] = &c[1];
				*c = '\0';
			}

			c++;
		} while (*c != '\0');
	}

	if (numSplits != NULL)
	{
		*numSplits = retLen;
	}

	return ret;
}

gunGameInfo_t bg_gunGameInfo;

gunGameInfo_t bg_classicGunGameInfo = {
	9,
	{
		WP_MACHINEGUN,
		WP_SHOTGUN,
		WP_GRENADE_LAUNCHER,
		WP_ROCKET_LAUNCHER,
		WP_LIGHTNING,
		WP_RAILGUN,
		WP_PLASMAGUN,
		WP_BFG,
		WP_GAUNTLET
	}
};

gunGameInfo_t bg_newGunGameInfo = {
	13,
	{
		WP_MACHINEGUN,
		WP_AUTOSHOTTY,
		WP_SHOTGUN,
		WP_GRENADE_LAUNCHER,
		WP_IMPACT_CANNON,
		WP_ROCKET_LAUNCHER,
		WP_LIGHTNING,
		WP_TAPRIFLE,
		WP_RAILGUN,
		WP_MINIRAIL,
		WP_PLASMAGUN,
		WP_BFG,
		WP_GAUNTLET
	}
};

void BG_GunGameInfoFromString(const char *info)
{
	char *strCpy;
	char **split;
	int num;
	int i;

	if (Q_stricmp(info, "classic") == 0)
	{
		bg_gunGameInfo = bg_classicGunGameInfo;
	}
	else if (Q_stricmp(info, "new") == 0)
	{
		bg_gunGameInfo = bg_newGunGameInfo;
	}
	else
	{
		strCpy = trap_HeapMalloc(strlen(info) * sizeof(*strCpy));
		strcpy(strCpy, info);

		split = str_split(strCpy, '/', &num);

		if (num > MAX_GUNGAME_LEVELS)
			num = MAX_GUNGAME_LEVELS; // limit this for now

		bg_gunGameInfo.numLevels = num;

		if (split == NULL)
			trap_Print("str_split returned NULL");
		else
		{
			for (i = 0; i < num; i++)
			{
				if (Q_stricmp(split[i], "mg") == 0)
					bg_gunGameInfo.levels[i] = WP_MACHINEGUN;
				else if (Q_stricmp(split[i], "sg") == 0)
					bg_gunGameInfo.levels[i] = WP_SHOTGUN;
				else if (Q_stricmp(split[i], "gl") == 0)
					bg_gunGameInfo.levels[i] = WP_GRENADE_LAUNCHER;
				else if (Q_stricmp(split[i], "rl") == 0)
					bg_gunGameInfo.levels[i] = WP_ROCKET_LAUNCHER;
				else if (Q_stricmp(split[i], "lg") == 0)
					bg_gunGameInfo.levels[i] = WP_LIGHTNING;
				else if (Q_stricmp(split[i], "rg") == 0)
					bg_gunGameInfo.levels[i] = WP_RAILGUN;
				else if (Q_stricmp(split[i], "pg") == 0)
					bg_gunGameInfo.levels[i] = WP_PLASMAGUN;
				else if (Q_stricmp(split[i], "bfg") == 0)
					bg_gunGameInfo.levels[i] = WP_BFG;
				else if (Q_stricmp(split[i], "mr") == 0)
					bg_gunGameInfo.levels[i] = WP_MINIRAIL;
				else if (Q_stricmp(split[i], "as") == 0)
					bg_gunGameInfo.levels[i] = WP_AUTOSHOTTY;
				else if (Q_stricmp(split[i], "tr") == 0)
					bg_gunGameInfo.levels[i] = WP_TAPRIFLE;
				else if (Q_stricmp(split[i], "ic") == 0)
					bg_gunGameInfo.levels[i] = WP_IMPACT_CANNON;
				else if (Q_stricmp(split[i], "g") == 0)
					bg_gunGameInfo.levels[i] = WP_GAUNTLET;
				else
					bg_gunGameInfo.levels[i] = WP_MACHINEGUN;
			}
		}

		trap_HeapFree(split);
		trap_HeapFree(strCpy);
	}
}

#define MAX_JSON_SIZE 10000

void BG_LoadFileContents(char *buf, const char *filename)
{
    int				len;
	fileHandle_t	f;

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	
    if ( !f ) {
		trap_Print( va( S_COLOR_RED "BG_LoadFileContents: file not found: %s\n", filename ) );
		return;
	}

	if ( len >= MAX_JSON_SIZE ) {
		trap_Print( va( S_COLOR_RED "BG_LoadFileContents: file too large: %s is %i, max allowed is %i\n", filename, len, MAX_JSON_SIZE ) );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

    Com_Printf("Loaded %s.\n", filename);
}

#define BG_CopyJSONString(dest, src) \
		Q_strncpyz(dest, src, ITEM_MAX_TEXT);

const char *itemTypeStrings[] = {
	"bad_",
	"weapon_",
	"ammo_",
	"item_armor_",
	"item_health_",
	"item_", // powerup
	"holdable_",
	"peristantpowerup", // not used
	"" // team
};

gitem_t bg_itemlist[MAX_ITEMS];
int bg_numItems = 1;
int bg_numItemsForType[8];

void BG_LoadJSONFile(const char *filename, itemType_t type)
{
    int i, j, k, r;
    jsmn_parser parser;
    jsmntok_t tokens[1024];
    char json[MAX_JSON_SIZE];

    BG_LoadFileContents(json, filename);

    jsmn_init(&parser);
    r = jsmn_parse(&parser, json, strlen(json), tokens, sizeof(tokens) / sizeof(tokens[0]));

    if (r < 0)
    {
        Com_Printf(S_COLOR_RED "BG_LoadJSONFile: Failed to parse JSON: %d (%s)\n", r, filename);
		return;
    }

    if (tokens[0].type != JSMN_ARRAY)
    {
        Com_Printf(S_COLOR_RED "BG_LoadJSONFile: JSON top object is not an array (%s)\n", filename);
        return;
    }

    for (i = 1; i < r; i++)
    {
        if (bg_numItems >= MAX_ITEMS)
        {
            Com_Printf("BG_LoadJSONFile: Reached max itemdefs %d (%s)\n", MAX_ITEMS, filename);
            break;
        }

        if (tokens[i].type == JSMN_OBJECT)
        {
			if (type == IT_WEAPON)
			{
				bg_itemlist[bg_numItems].flashImpulse = qtrue;
				bg_itemlist[bg_numItems].autoAttack = qtrue;
				bg_itemlist[bg_numItems].oneHanded = qfalse;
			}

            for (j = 1; i + j < r; j += 2)
            {
                jsmntok_t keyToken = tokens[i + j];
                jsmntok_t valueToken;
                int keyLen, valueLen;
                char key[128];
                char value[MAX_QPATH * 10];

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
                    Com_Printf("BG_LoadJSONFile: key length <= 0\n");
                    break;
                }

                if (valueLen <= 0)
                {
                    Com_Printf("BG_LoadJSONFile: value length <= 0\n");
                    //break;
                }

                Q_strncpyz(key, json + keyToken.start, sizeof(key));
                Q_strncpyz(value, json + valueToken.start, sizeof(value));

                key[keyLen] = '\0';
                value[valueLen] = '\0';

                //Com_Printf("Parsing token %s: %s\n", key, value);

                if (Q_stricmp(key, "id") == 0)
                {
					if (valueLen > 0)
					{
						BG_CopyJSONString(bg_itemlist[bg_numItems].classname, va("%s%s", itemTypeStrings[type], value));
					}
					else
					{
						int len = strlen(itemTypeStrings[type]) - 1;
						BG_CopyJSONString(bg_itemlist[bg_numItems].classname, va("%.*s", len, itemTypeStrings[type]));
					}
                }
				else if (!Q_stricmp(key, "type"))
				{
				}
				else if (Q_stricmp(key, "displayName") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].displayName, value);
                }
				else if (Q_stricmp(key, "shortName") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].shortName, value);
                }
				else if (Q_stricmp(key, "ammo") == 0)
                {
					bg_itemlist[bg_numItems].ammoId = BG_ItemNumForItem(BG_FindItemForAmmoName(value));
				}
				else if (Q_stricmp(key, "displayModel") == 0)
                {
                    char path[MAX_QPATH];
                    jsmntok_t tok;
                    int tokLen;

                    for(k = 0; k < valueToken.size && k < 4; k++)
                    {
                        tok = tokens[i + j + 2 + k];
                        tokLen = tok.end - tok.start;
                        Q_strncpyz(path, json + tok.start, tokLen + 1);
						BG_CopyJSONString(bg_itemlist[bg_numItems].displayModel[k], path);
		            }

                    j += valueToken.size;
                }
				else if (Q_stricmp(key, "icon") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].icon, value);
                }
				else if (Q_stricmp(key, "ammoId") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems + WP_NUM_WEAPONS - 1].classname, va("%s%s", itemTypeStrings[IT_AMMO], value));
					bg_itemlist[bg_numItems].ammoId = BG_ItemNumForItem(BG_FindItemForAmmoName(value));
                }
				else if (Q_stricmp(key, "ammoName") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems + WP_NUM_WEAPONS - 1].displayName, value);
                }
				else if (Q_stricmp(key, "ammoModel") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems + WP_NUM_WEAPONS - 1].displayModel[0], value);
                }
				else if (Q_stricmp(key, "ammoIcon") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems + WP_NUM_WEAPONS - 1].icon, value);
                }
				else if (Q_stricmp(key, "pickupSound") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].pickupSound, value);
                }
				else if (Q_stricmp(key, "sounds") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].sounds, value);
                }
				else if (Q_stricmp(key, "idleSound") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].idleSound, value);
                }
				else if (Q_stricmp(key, "fireSound") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].fireSound, value);
                }
				else if (Q_stricmp(key, "flashColor") == 0)
                {
                    char colorValue[8];
                    jsmntok_t *colorTok = &tokens[i + j + 2];

                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start + 1);
                    bg_itemlist[bg_numItems].flashColor[0] = atof(colorValue);

                    colorTok = &tokens[i + j + 3];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start + 1);
                    bg_itemlist[bg_numItems].flashColor[1] = atof(colorValue);

                    colorTok = &tokens[i + j + 4];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start + 1);
                    bg_itemlist[bg_numItems].flashColor[2] = atof(colorValue);
                    j += 3;
                }
				else if (Q_stricmp(key, "flashSound") == 0)
                {
                    char path[MAX_QPATH];
                    jsmntok_t tok;
                    int tokLen;

                    for(k = 0; k < valueToken.size && k < MAX_FLASH_SOUNDS; k++)
                    {
                        tok = tokens[i + j + 2 + k];
                        tokLen = tok.end - tok.start;
                        Q_strncpyz(path, json + tok.start, tokLen + 1);
						BG_CopyJSONString(bg_itemlist[bg_numItems].flashSound[k], path);
					}

                    j += valueToken.size;
                }
				else if (Q_stricmp(key, "flashImpulse") == 0)
                {
					bg_itemlist[bg_numItems].flashImpulse = !Q_stricmp(value, "true");
				}
				else if (Q_stricmp(key, "customShading") == 0)
                {
					bg_itemlist[bg_numItems].customShading = !Q_stricmp(value, "true");
				}
				else if (Q_stricmp(key, "missileModel") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].missileModel, value);
                }
				else if (Q_stricmp(key, "missileSound") == 0)
                {
					BG_CopyJSONString(bg_itemlist[bg_numItems].missileSound, value);
                }
				else if (Q_stricmp(key, "missileTrailType") == 0)
                {
                    if (Q_stricmp(value, "rocket") == 0)
                        bg_itemlist[bg_numItems].missileTrailType = MTT_ROCKET;
                    else if (Q_stricmp(value, "grenade") == 0)
                        bg_itemlist[bg_numItems].missileTrailType = MTT_GRENADE;
                    else if (Q_stricmp(value, "plasma") == 0)
                        bg_itemlist[bg_numItems].missileTrailType = MTT_PLASMA;
					else
						bg_itemlist[bg_numItems].missileTrailType = MTT_NONE;
                }
				else if (Q_stricmp(key, "missileTrailTime") == 0)
                {
                    bg_itemlist[bg_numItems].missileTrailTime = atoi(value);
                }
				else if (Q_stricmp(key, "missileTrailRadius") == 0)
                {
                    bg_itemlist[bg_numItems].missileTrailRadius = atoi(value);
                }
				else if (Q_stricmp(key, "missileDlight") == 0)
                {
                    bg_itemlist[bg_numItems].missileDlight = atoi(value);
                }
				else if (Q_stricmp(key, "missileDlightColor") == 0)
                {
                    char colorValue[8];
                    jsmntok_t colorTok = tokens[i + j + 2];

                    Q_strncpyz(colorValue, json + colorTok.start, colorTok.end - colorTok.start + 1);
                    bg_itemlist[bg_numItems].missileDlightColor[0] = atof(colorValue);

                    colorTok = tokens[i + j + 3];
                    Q_strncpyz(colorValue, json + colorTok.start, colorTok.end - colorTok.start + 1);
                    bg_itemlist[bg_numItems].missileDlightColor[1] = atof(colorValue);

                    colorTok = tokens[i + j + 4];
                    Q_strncpyz(colorValue, json + colorTok.start, colorTok.end - colorTok.start + 1);
                    bg_itemlist[bg_numItems].missileDlightColor[2] = atof(colorValue);

                    j += 3;
                }
				else if (Q_stricmp(key, "brassType") == 0)
                {
                    if (Q_stricmp(value, "bullet") == 0)
                        bg_itemlist[bg_numItems].brassType = BF_BULLET;
                    else if (Q_stricmp(value, "shell") == 0)
                        bg_itemlist[bg_numItems].brassType = BF_SHELL;
					else
						bg_itemlist[bg_numItems].brassType = BF_NONE;
                }
				else if (Q_stricmp(key, "autoAttack") == 0)
                {
					bg_itemlist[bg_numItems].autoAttack = !Q_stricmp(value, "true");
				}
				else if (Q_stricmp(key, "oneHanded") == 0)
                {
					bg_itemlist[bg_numItems].oneHanded = !Q_stricmp(value, "true");
				}
                else if (Q_stricmp(key, "attackDelay") == 0)
                {
                    bg_itemlist[bg_numItems].attackDelay = atoi(value);
                }
                else if (Q_stricmp(key, "amount") == 0)
                {
                    bg_itemlist[bg_numItems].amount = atoi(value);
                }
				else if (Q_stricmp(key, "ammoAmount") == 0)
                {
                    bg_itemlist[bg_numItems + WP_NUM_WEAPONS - 1].amount = atoi(value);
                }
                else
                {
                    Com_Printf("BG_LoadJSONFile: Could not parse token %s: %s\n", key, value);
                }
            }
            bg_itemlist[bg_numItems].type = type;
            bg_itemlist[bg_numItems].localIndex = bg_numItemsForType[type];
            //Com_Printf("BG_LoadJSONFile: Parsed info for %s %d %d\n", bg_itemlist[bg_numItems].classname, bg_itemlist[bg_numItems].localIndex, bg_numItems);

			if (type == IT_WEAPON)
			{
				//if (!bg_itemlist[bg_numItems].noAmmoDef)
				{
					bg_itemlist[bg_numItems + WP_NUM_WEAPONS - 1].type = IT_AMMO;
            		bg_itemlist[bg_numItems + WP_NUM_WEAPONS - 1].localIndex = bg_itemlist[bg_numItems].localIndex;
					bg_numItemsForType[IT_AMMO]++;
					bg_itemlist[bg_numItems].ammoId = bg_numItems + WP_NUM_WEAPONS - 1;
				}
			}

            bg_numItems++;
			bg_numItemsForType[type]++;
		}
    }

	if (type == IT_WEAPON)
		bg_numItems += WP_NUM_WEAPONS; // skip over the slots being used for the ammo items
}

qboolean initialised = qfalse;

void BG_LoadItemJSON(void)
{
	if (initialised)
		return;

	bg_numItemsForType[IT_WEAPON]++;
	bg_numItemsForType[IT_AMMO]++;
	bg_numItemsForType[IT_HOLDABLE]++;
	bg_numItemsForType[IT_POWERUP]++;

	BG_LoadJSONFile("weapons.json", IT_WEAPON);
	BG_LoadJSONFile("armor.json", IT_ARMOR);
	BG_LoadJSONFile("health.json", IT_HEALTH);
	BG_LoadJSONFile("holdables.json", IT_HOLDABLE);
	BG_LoadJSONFile("powerups.json", IT_POWERUP);
	BG_LoadJSONFile("team.json", IT_TEAM);

	BG_FindItemByClassname("team_CTF_redflag")->localIndex = PW_REDFLAG;
	BG_FindItemByClassname("team_CTF_blueflag")->localIndex = PW_BLUEFLAG;

	//bg_itemlist[bg_numItems].classname = NULL;
	initialised = qtrue;
}
