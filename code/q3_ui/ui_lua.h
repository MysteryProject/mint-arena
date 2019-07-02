#ifndef _H_LUA_
#define _H_LUA_
#include "../moonnuklear/moonnuklear.h"

void lua_RegisterUtil(lua_State *L);
void lua_RegisterCgame(lua_State *L);
void lua_RegisterSyscalls(lua_State *L);

#endif