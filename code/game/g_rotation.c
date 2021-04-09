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

// g_rotation.c

#include "g_local.h"

qboolean G_MapExist( const char *map ) {
	fileHandle_t fh;
	int len;

	if ( !map || !*map ) {
		return qfalse;
    }

	len = trap_FS_FOpenFile( va( "maps/%s.bsp", map ), &fh, FS_READ );

	if ( len < 0 ) {
		return qfalse;
    }

	trap_FS_FCloseFile( fh );

	return ( len >= 144 ) ? qtrue : qfalse ;
}

void G_LoadMap( const char *map )  {
	char cmd[ MAX_CVAR_VALUE_STRING ];
	char ver[ 16 ];
	int version;

	trap_Cvar_VariableStringBuffer( "version", ver, sizeof( ver ) );
	if ( !Q_strncmp( ver, "Q3 1.32 ", 8 ) || !Q_strncmp( ver, "Q3 1.32b ", 9 ) ||
		!Q_strncmp( ver, "Q3 1.32c ", 9 ) ) 
		version = 0; // buggy vanilla binaries
	else
		version = 1;

	if ( !map || !*map || !G_MapExist( map ) || !Q_stricmp( map, g_mapname.string ) ) {
		if ( level.time > 12*60*60*1000 || version == 0 || level.denyMapRestart )
			Com_sprintf( cmd, sizeof(cmd), "map \"%s\"\n", g_mapname.string );
		else
			strcpy( cmd, "map_restart 0\n" );
	} else {
		if ( !G_MapExist( map ) ) // required map doesn't exists, reload existing
			Com_sprintf( cmd, sizeof(cmd), "map \"%s\"\n", g_mapname.string );
		else
			Com_sprintf( cmd, sizeof(cmd), "map \"%s\"\n", map );
	}

	trap_Cmd_ExecuteText( EXEC_APPEND, cmd );
	level.restarted = qtrue;
}


qboolean ParseMapRotation( void ) {
	char buf[ 4096 ];
	char cvar[ 256 ];
	char map[ 256 ];
	char *s;
	fileHandle_t fh;
	int	len;
	char *tk;
	int reqIndex; 
	int curIndex = 0;
	int scopeLevel = 0;

	if ( g_gametype.integer == GT_SINGLE_PLAYER || !g_rotation.string[0] )
		return qfalse;

	len = trap_FS_FOpenFile( g_rotation.string, &fh, FS_READ );
	if ( !fh ) {
		Com_Printf( S_COLOR_YELLOW "%s: map rotation file doesn't exists.\n", g_rotation.string );
		return qfalse;
	}
	if ( len >= sizeof( buf ) ) {
		Com_Printf( S_COLOR_YELLOW "%s: map rotation file is too big.\n", g_rotation.string );
		len = sizeof( buf ) - 1;
	}
	trap_FS_Read( buf, len, fh );
	buf[ len ] = '\0';
	trap_FS_FCloseFile( fh );
	
	Com_InitSeparators(); // needed for COM_ParseSep()

	reqIndex = trap_Cvar_VariableIntegerValue( SV_ROTATION );
	if ( reqIndex == 0 )
		reqIndex = 1;

__rescan:

	COM_BeginParseSession( g_rotation.string );

	s = buf; // initialize token parsing
	map[0] = '\0';

	while ( 1 ) {
		tk = COM_ParseSep( &s, qtrue );
		if ( tk[0] == '\0' ) 
			break;

		if ( tk[0] == '$' ) {
			 // save cvar name
			strcpy( cvar, tk+1 );
			tk = COM_ParseSep( &s, qfalse );
			// expect '='
			if ( tk[0] == '=' && tk[1] == '\0' ) {
				tk = COM_ParseSep( &s, qtrue );
				if ( !scopeLevel || curIndex == reqIndex ) {
					trap_Cvar_Set( cvar, tk );
				}
				SkipTillSeparators( &s ); 
				continue;
			} else {
				COM_ParseWarning( S_COLOR_YELLOW "missing '=' after '%s'", cvar );
				SkipRestOfLine( &s );
				continue;
			}

		} else if ( tk[0] == '{' && tk[1] == '\0' ) {
			if ( scopeLevel == 0 && curIndex ) {
				scopeLevel++;
				continue;
			} else {
				COM_ParseWarning( S_COLOR_YELLOW "unexpected '{'" );
				continue;
			}
		} else if ( tk[0] == '}' && tk[1] == '\0' ) {
			if ( scopeLevel == 1 ) {
				scopeLevel--;
				continue;
			} else {
				COM_ParseWarning( S_COLOR_YELLOW "unexpected '}'" );
			}
		} else if ( G_MapExist( tk ) ) {
			curIndex++;
			if ( curIndex == reqIndex ) {
				Q_strncpyz( map, tk, sizeof( map ) );
			}
		} else {
			COM_ParseWarning( S_COLOR_YELLOW "map '%s' doesn't exists", tk );
			SkipRestOfLine( &s );
			continue;
		}
	}

	if ( curIndex == 0 ) {
		Com_Printf( S_COLOR_YELLOW "%s: no maps in rotation file.\n", g_rotation.string );
		trap_Cvar_Set( SV_ROTATION, "1" );
		return qfalse;
	}

	if ( !map[0] ) {
		if ( reqIndex > 1 ) {
			Com_Printf( S_COLOR_CYAN "%s: map at index %i not found, rescan\n", g_rotation.integer, reqIndex );
			reqIndex = 1;
			goto __rescan;
		}
		trap_Cvar_Set( SV_ROTATION, "1" );
		return qfalse;
	}

	reqIndex++;
	if ( reqIndex > curIndex )
		reqIndex = 1;

	trap_Cvar_Set( SV_ROTATION, va( "%i", reqIndex ) );
	trap_Cvar_Set( "g_restarted", "1" );
	G_LoadMap( map );

	return qtrue;
}
