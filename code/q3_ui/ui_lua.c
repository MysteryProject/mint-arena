#include "ui_local.h"
#include "ui_lua.h"

void LuaHook_Shutdown(lua_State *L)
{
    /* push functions and arguments */
    lua_getglobal(L, "main"); // main ui table
    lua_getfield(L, -1, "shutdown"); // ui:init

    lua_pushvalue(L, -2); // push 'self' as first arg
    
    /* do the call (1 arguments, 1 result) */
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        Com_Error(ERR_DROP, "error running function `main:shutdown': %s", lua_tostring(L, -1));
        return;
    }
}

float *moonnuklear_checkfloatlist(lua_State *L, int arg, int *count, int *err);

static int lua_Printf(lua_State *L)
{
    int n = lua_gettop(L);
    int i;

    for (i = 1; i <= n; i++)
        CG_Printf("%s", lua_tostring(L, i));

    CG_Printf("\n");

    return 0;
}

static int lua_Exit(lua_State *L)
{
//    LuaHook_Shutdown(L);

    UI_PopMenu();

    return 0;
}

static int lua_cos(lua_State *L)
{
    float arg = lua_tonumber(L, 1);
    lua_pushnumber(L, cos(arg));
    return 1;
}

static int lua_sin(lua_State *L)
{
    float arg = lua_tonumber(L, 1);
    lua_pushnumber(L, sin(arg));
    return 1;
}

#define CIRCLE_VERTS	21

static int lua_DrawCircleFilled(lua_State *L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float w = lua_tonumber(L, 3);
    float h = lua_tonumber(L, 4);
    int count, err;
    float *color = moonnuklear_checkfloatlist(L, 5, &count, &err);
    //
	polyVert_t verts[CIRCLE_VERTS];
	int i;
	float horizontalRadius, verticialRadius;

	CG_AdjustFrom640( &x, &y, &w, &h );

	horizontalRadius = w * 0.5f;
	verticialRadius = h * 0.5f;

	// move to center of circle
	x += horizontalRadius;
	y += verticialRadius;

	// full bright point at center
	i = 0;

    Vector2Set(verts[i].xyz, x, y);
    Vector2Set(verts[i].st, 0.5f, 0.5f);
    Vector4Copy(color, verts[i].modulate);

    for ( i = 1; i < CIRCLE_VERTS; i++ ) {
		float theta = 2.0f * M_PI * (float)(i-1) / (float)(CIRCLE_VERTS-2);

		verts[i].xyz[0] = x + cos( theta ) * horizontalRadius;
		verts[i].xyz[1] = y + sin( theta ) * verticialRadius;
		verts[i].xyz[2] = 0;

		// these are wrong.. but drawing blank image anyway
		verts[i].st[0] = 0.5f + ( verts[i].xyz[0] - x ) / horizontalRadius;
		verts[i].st[1] = 0.5f + ( verts[i].xyz[1] - y ) / verticialRadius;

        Vector4Copy(color, verts[i].modulate);
	}

	trap_R_Add2dPolys( verts, CIRCLE_VERTS, cgs.media.whiteShader );
    return 0;
}

static int lua_DrawRectMultiColor(lua_State *L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float w = lua_tonumber(L, 3);
    float h = lua_tonumber(L, 4);
    int count, err;
    float *left = moonnuklear_checkfloatlist(L, 5, &count, &err);
    float *top = moonnuklear_checkfloatlist(L, 6, &count, &err);
    float *right = moonnuklear_checkfloatlist(L, 7, &count, &err);
    float *bottom = moonnuklear_checkfloatlist(L, 8, &count, &err);
    polyVert_t verts[4];

    CG_AdjustFrom640(&x, &y, &w, &h);

    Vector2Set(verts[0].xyz, x, y);
    Vector4Copy(left, verts[0].modulate);
    Vector2Set(verts[0].st, 0, 0);

    Vector2Set(verts[1].xyz, x + w, y);
    Vector4Copy(top, verts[1].modulate);
    Vector2Set(verts[1].st, 0, 1);

    Vector2Set(verts[2].xyz, x + w, y + h);
    Vector4Copy(right, verts[2].modulate);
    Vector2Set(verts[2].st, 1, 1);

    Vector2Set(verts[3].xyz, x, y + h);
    Vector4Copy(bottom, verts[3].modulate);
    Vector2Set(verts[3].st, 1, 0);

    trap_R_Add2dPolys(verts, 4, uis.whiteShader);
    return 0;
}

static int lua_DrawLine(lua_State *L)
{
    int count, err;
    float *startf = moonnuklear_checkfloatlist(L, 1, &count, &err);
    float *endf = moonnuklear_checkfloatlist(L, 2, &count, &err);
    unsigned short thickness = lua_tonumber(L, 3);
    float *color = moonnuklear_checkfloatlist(L, 4, &count, &err);
    //
    float length, w = thickness / 2;
    vec2_t start;
    vec2_t end;
    vec2_t v, p, n;
    polyVert_t verts[4];

    Vector2Set(start, startf[0], startf[1]);
    Vector2Set(end, endf[0], endf[1]);

    CG_AdjustFrom640(&start[0], &start[1], NULL, NULL);
    CG_AdjustFrom640(&end[0], &end[1], NULL, NULL);

    Vector2Set(v, end[0] - start[0], end[1] - start[1]);
    Vector2Set(p, v[1], -v[0]);

    length = Vector2Dot(p, p);

    Vector2Set(n, p[0] / length, p[1] / length);

    Vector2Set(verts[0].xyz, start[0] + n[0] * w, start[1] + n[1] * w);
    Vector4Copy(color, verts[0].modulate);
    Vector2Set(verts[0].st, 0, 0);

    Vector2Set(verts[1].xyz, start[0] - n[0] * w, start[1] - n[1] * w);
    Vector4Copy(color, verts[1].modulate);
    Vector2Set(verts[1].st, 0, 1);

    Vector2Set(verts[2].xyz, end[0] + n[0] * w, end[1] + n[1] * w);
    Vector4Copy(color, verts[2].modulate);
    Vector2Set(verts[2].st, 1, 1);

    Vector2Set(verts[3].xyz, end[0] - n[0] * w, end[1] - n[1] * w);
    Vector4Copy(color, verts[3].modulate);
    Vector2Set(verts[3].st, 1, 0);

    trap_R_Add2dPolys(verts, 4, uis.whiteShader);
    return 0;
}

static int lua_DrawTriangleFilled(lua_State *L)
{
    int count, err;
    float *a = moonnuklear_checkfloatlist(L, 1, &count, &err);
    float *b = moonnuklear_checkfloatlist(L, 2, &count, &err);
    float *c = moonnuklear_checkfloatlist(L, 3, &count, &err);
    float *color = moonnuklear_checkfloatlist(L, 4, &count, &err);
    //
    polyVert_t verts[3];
    
    CG_AdjustFrom640(&a[0], &a[1], NULL, NULL);
    CG_AdjustFrom640(&b[0], &b[1], NULL, NULL);
    CG_AdjustFrom640(&c[0], &c[1], NULL, NULL);

    Vector2Copy(a, verts[0].xyz);
    Vector4Copy(color, verts[0].modulate);
    Vector2Set(verts[0].st, 0, 0);

    Vector2Copy(b, verts[1].xyz);
    Vector4Copy(color, verts[1].modulate);
    Vector2Set(verts[1].st, 0, 1);

    Vector2Copy(c, verts[2].xyz);
    Vector4Copy(color, verts[2].modulate);
    Vector2Set(verts[2].st, 1, 1);

    trap_R_Add2dPolys(verts, 3, uis.whiteShader);
    return 0;
}

static int lua_GetMousePosition(lua_State *L)
{
    lua_pushnumber(L, uis.cursorx);
    lua_pushnumber(L, uis.cursory);
    return 2;
}

static int lua_strlen(lua_State *L)
{
    const char *str = lua_tostring(L, 1);
    lua_pushnumber(L, strlen(str));
    return 1;
}

#define lua_registerenum(L,n,v) (lua_pushnumber(L, v), lua_setglobal(L, n))
#define LUA_ENUM(L, name) \
  lua_pushnumber(L, name); \
  lua_setglobal(L, #name);

void lua_RegisterUtil(lua_State *L)
{
    lua_register(L, "printf", lua_Printf);
    lua_register(L, "_exit", lua_Exit);

    lua_register(L, "GetMousePosition", lua_GetMousePosition);

    lua_register(L, "cos", lua_cos);
    lua_register(L, "sin", lua_sin);
    lua_register(L, "strlen", lua_strlen);

    lua_register(L, "NK_DrawCircleFilled", lua_DrawCircleFilled);
    lua_register(L, "NK_DrawRectMultiColor", lua_DrawRectMultiColor);
    lua_register(L, "NK_DrawLine", lua_DrawLine);
    lua_register(L, "NK_DrawTriangleFilled", lua_DrawTriangleFilled);

    LUA_ENUM(L, K_CHAR_FLAG);

    // horizontal alignment
    LUA_ENUM(L, UI_LEFT);
    LUA_ENUM(L, UI_CENTER);
    LUA_ENUM(L, UI_RIGHT);
    LUA_ENUM(L, UI_FORMATMASK);

    // vertical alignment
    LUA_ENUM(L, UI_VA_TOP);
    LUA_ENUM(L, UI_VA_CENTER);
    LUA_ENUM(L, UI_VA_BOTTOM);
    LUA_ENUM(L, UI_VA_FORMATMASK);

    // font selection
    LUA_ENUM(L, UI_SMALLFONT);
    LUA_ENUM(L, UI_BIGFONT);
    LUA_ENUM(L, UI_GIANTFONT);
    LUA_ENUM(L, UI_TINYFONT);
    LUA_ENUM(L, UI_NUMBERFONT);
    LUA_ENUM(L, UI_CONSOLEFONT);
    LUA_ENUM(L, UI_FONTMASK);

    // other flags
    LUA_ENUM(L, UI_DROPSHADOW);
    LUA_ENUM(L, UI_BLINK);
    LUA_ENUM(L, UI_INVERSE);
    LUA_ENUM(L, UI_PULSE);
    LUA_ENUM(L, UI_FORCECOLOR);
    LUA_ENUM(L, UI_GRADIENT);
    LUA_ENUM(L, UI_NOSCALE);
    LUA_ENUM(L, UI_INMOTION);

    LUA_ENUM(L, K_TAB);
    LUA_ENUM(L, K_ENTER);
    LUA_ENUM(L, K_ESCAPE);
    LUA_ENUM(L, K_SPACE);
    LUA_ENUM(L, K_BACKSPACE);
    LUA_ENUM(L, K_CAPSLOCK);
    LUA_ENUM(L, K_UPARROW);
    LUA_ENUM(L, K_DOWNARROW);
    LUA_ENUM(L, K_LEFTARROW);
    LUA_ENUM(L, K_RIGHTARROW);
    LUA_ENUM(L, K_MOUSE1);
    LUA_ENUM(L, K_MOUSE2);
    LUA_ENUM(L, K_MOUSE3);
    LUA_ENUM(L, K_MOUSE4);
    LUA_ENUM(L, K_MOUSE5);
}

static int lua_CG_DrawRect(lua_State *L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float w = lua_tonumber(L, 3);
    float h = lua_tonumber(L, 4);
    float size = lua_tonumber(L, 5);
    int count, err;
    float *color = moonnuklear_checkfloatlist(L, 6, &count, &err);
    CG_DrawRect(x, y, w, h, size, color);
    return 0;
}

static int lua_CG_FillRect(lua_State *L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float w = lua_tonumber(L, 3);
    float h = lua_tonumber(L, 4);
    int count, err;
    float *color = moonnuklear_checkfloatlist(L, 5, &count, &err);
    CG_FillRect(x, y, w, h, color);
    return 0;
}

static int lua_CG_DrawString(lua_State *L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    const char *str = lua_tostring(L, 3);
    int style = lua_tonumber(L, 4);
    int count, err;
    float *color = moonnuklear_checkfloatlist(L, 5, &count, &err);
    UI_DrawString(x, y, str, style, color);
    return 0;
}

static int lua_CG_SetClipRegion(lua_State *L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float w = lua_tonumber(L, 3);
    float h = lua_tonumber(L, 4);
    CG_SetClipRegion(x, y, w, h);
    return 0;
}

static int lua_TextWidth(lua_State *L)
{
    const char *text = lua_tostring(L, 1);
    float width = CG_DrawStrlenCommon(text, 0, &uis.textFont, 0);
    lua_pushnumber(L, width);
    return 1;
}

static int lua_DrawImage(lua_State *L)
{
    float x = lua_tonumber(L, 1);
    float y = lua_tonumber(L, 2);
    float w = lua_tonumber(L, 3);
    float h = lua_tonumber(L, 4);
    qhandle_t handle = lua_tonumber(L, 5);
    int count, err;
    float *color = moonnuklear_checkfloatlist(L, 5, &count, &err);

    trap_R_SetColor(color);
    CG_DrawPic(x, y, w, h, handle);
    trap_R_SetColor(NULL);
    return 0;
}

void lua_RegisterCgame(lua_State *L)
{
    lua_register(L, "CG_DrawRect", lua_CG_DrawRect);
    lua_register(L, "CG_FillRect", lua_CG_FillRect);
    lua_register(L, "CG_DrawString", lua_CG_DrawString);
    lua_register(L, "CG_SetClipRegion", lua_CG_SetClipRegion);
    lua_register(L, "CG_DrawImage", lua_DrawImage);
    lua_register(L, "CG_DrawStrlen", lua_TextWidth);
}

static int lua_RegisterShader(lua_State *L)
{
    lua_pushnumber(L, trap_R_RegisterShader(lua_tostring(L, 1)));
    return 1;
}

void lua_RegisterSyscalls(lua_State *L)
{
    lua_register(L, "trap_R_RegisterShader", lua_RegisterShader);
}
