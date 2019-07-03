/*
===========================================================================
Copyright (C) 2019 Marcus Collins.

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

#include "ui_local.h"
#include "ui_lua.h"

#define Option 10

typedef struct
{
    menuframework_s menu;

    menutext_s option;
} debugMenu_t;

static debugMenu_t m_debug;

/*
=================
Menu Event
=================
*/
static void MenuEvent(void *ptr, int event)
{
    if (event != QM_ACTIVATED)
        return;

    switch (((menucommon_s *)ptr)->id)
    {
    case Option:
        //CH_MenuMain();
        break;
    default:
        break;
    }
}

static int menuy = 320;

static void NewText(menutext_s *mtext, char *string, int flags, int style, int id)
{
    menuy += 34;
    mtext->generic.type = MTYPE_PTEXT;
    mtext->generic.flags = flags | QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    mtext->generic.x = 320;
    mtext->generic.y = menuy;
    mtext->generic.id = id;
    mtext->generic.callback = MenuEvent;
    mtext->string = string;
    mtext->color = text_big_color;
    mtext->style = style | UI_DROPSHADOW | UI_CENTER;
}


void HandleCrosshairChange(int num)
{
}

lua_State *L;

int luaopen_moonnuklear(lua_State *L);

/*
=================
Menu Init
=================
*/

static int report (lua_State *L, int status) {
  const char *msg;
  if (status) {
    msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error with no message)";
    //fprintf(stderr, "status=%d, %s\n", status, msg);
    Com_Printf("status=%d, %s\n", status, msg);
    lua_pop(L, 1);
  }
  return status;
}

char basedir[MAX_STRING_CHARS];

void LuaHook_Init(void)
{
    /* push functions and arguments */
    lua_getglobal(L, "main"); // main ui table
    lua_getfield(L, -1, "init"); // ui:init

    lua_pushvalue(L, -2); // push 'self' as first arg
    
    /* do the call (1 arguments, 1 result) */
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        Com_Error(ERR_DROP, "error running function `main:init': %s", lua_tostring(L, -1));
        return;
    }
}

void LuaHook_Draw(void)
{
    /* push functions and arguments */
    lua_getglobal(L, "main"); // main ui table
    lua_getfield(L, -1, "draw"); // ui:init

    lua_pushvalue(L, -2); // push 'self' as first arg
    
    /* do the call (1 arguments, 1 result) */
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        Com_Error(ERR_DROP, "error running function `main:draw': %s", lua_tostring(L, -1));
        return;
    }
}

static void DebugDraw(void)
{
    LuaHook_Draw();
}

static sfxHandle_t DebugKey(int key, qboolean down)
{
    lua_getglobal(L, "main");
    lua_getfield(L, -1, "key_event");
    lua_pushvalue(L, -2);
    lua_pushnumber(L, key);
    lua_pushboolean(L, down);

    if (lua_pcall(L, 3, 0, 0) != 0)
    {
        Com_Error(ERR_DROP, "error running function `main:key_event`: %s", lua_tostring(L, -1));
    }

    return menu_null_sound;
}

void MenuInit(void)
{
    memset(&m_debug, 0, sizeof(m_debug));

    m_debug.menu.wrapAround = qtrue;
    m_debug.menu.fullscreen = qtrue;

    NewText(&m_debug.option, "Option", 0, 0, Option);

    m_debug.menu.draw = DebugDraw;
    m_debug.menu.key2 = DebugKey;

    //Menu_AddItem(&m_debug.menu, &m_debug.option);

    // lua
    trap_Cvar_VariableStringBuffer( "fs_basepath", basedir, sizeof( basedir ) );

    //if (L != NULL)
    //    lua_close(L);

    if (L == NULL)
    {
        L = luaL_newstate();
        luaL_openlibs(L);

        luaopen_moonnuklear(L);

        lua_RegisterUtil(L);
        lua_RegisterCgame(L);
        lua_RegisterSyscalls(L);

        lua_pushstring(L, va("%s\\baseq3\\lua\\", basedir));
        lua_setglobal(L, "BASE_DIR");

        if (report(L, luaL_loadfile(L, va("%s\\baseq3\\lua\\main.lua", basedir)) || lua_pcall(L, 0,0,0)))
        {
            Com_Printf("Lua Error!\n");
        }
    }
    LuaHook_Init();
}

/*
=================
Menu Main
=================
*/
void MenuMain(void)
{
    MenuInit();
    UI_PushMenu(&m_debug.menu);
}
