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
    LuaHook_Shutdown(L);

    lua_close(L);

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

    trap_R_Add2dPolys(verts, 4, cgs.media.whiteShader);

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

    trap_R_Add2dPolys(verts, 4, cgs.media.whiteShader );

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

    trap_R_Add2dPolys(verts, 3, cgs.media.whiteShader);

    return 0;
}

#define lua_registerenum(L,n,v) (lua_pushnumber(L, v), lua_setglobal(L, n))

void lua_RegisterUtil(lua_State *L)
{
    lua_register(L, "printf", lua_Printf);
    lua_register(L, "_exit", lua_Exit);

    lua_register(L, "cos", lua_cos);
    lua_register(L, "sin", lua_sin);

    lua_register(L, "NK_DrawCircleFilled", lua_DrawCircleFilled);
    lua_register(L, "NK_DrawRectMultiColor", lua_DrawRectMultiColor);
    lua_register(L, "NK_DrawLine", lua_DrawLine);
    lua_register(L, "NK_DrawTriangleFilled", lua_DrawTriangleFilled);

    lua_registerenum(L, "whiteShader", cgs.media.whiteShader);

    // horizontal alignment
    lua_registerenum(L, "UI_LEFT", UI_LEFT);
    lua_registerenum(L, "UI_CENTER", UI_CENTER);
    lua_registerenum(L, "UI_RIGHT", UI_RIGHT);
    lua_registerenum(L, "UI_FORMATMASK", UI_FORMATMASK);

    // vertical alignment
    lua_registerenum(L, "UI_VA_TOP", UI_VA_TOP);
    lua_registerenum(L, "UI_VA_CENTER", UI_VA_CENTER);
    lua_registerenum(L, "UI_VA_BOTTOM", UI_VA_BOTTOM);
    lua_registerenum(L, "UI_VA_FORMATMASK", UI_VA_FORMATMASK);

    // font selection
    lua_registerenum(L, "UI_SMALLFONT", UI_SMALLFONT);
    lua_registerenum(L, "UI_BIGFONT", UI_BIGFONT);
    lua_registerenum(L, "UI_GIANTFONT", UI_GIANTFONT);
    lua_registerenum(L, "UI_TINYFONT", UI_TINYFONT);
    lua_registerenum(L, "UI_NUMBERFONT", UI_NUMBERFONT);
    lua_registerenum(L, "UI_CONSOLEFONT", UI_CONSOLEFONT);
    lua_registerenum(L, "UI_FONTMASK", UI_FONTMASK);

    // other flags
    lua_registerenum(L, "UI_DROPSHADOW", UI_DROPSHADOW);
    lua_registerenum(L, "UI_BLINK", UI_BLINK);
    lua_registerenum(L, "UI_INVERSE", UI_INVERSE);
    lua_registerenum(L, "UI_PULSE", UI_PULSE);
    lua_registerenum(L, "UI_FORCECOLOR", UI_FORCECOLOR);
    lua_registerenum(L, "UI_GRADIENT", UI_GRADIENT);
    lua_registerenum(L, "UI_NOSCALE", UI_NOSCALE);
    lua_registerenum(L, "UI_INMOTION", UI_INMOTION);
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
    float width = Text_Width(text, &uis.textFont, uis.textFont.pointSize / 48.0f, 0);
    lua_pushnumber(L, width);
    return 1;
}

void lua_RegisterCgame(lua_State *L)
{
    lua_register(L, "CG_DrawRect", lua_CG_DrawRect);
    lua_register(L, "CG_FillRect", lua_CG_FillRect);
    lua_register(L, "CG_DrawString", lua_CG_DrawString);
    lua_register(L, "CG_SetClipRegion", lua_CG_SetClipRegion);
    lua_register(L, "Text_Width", lua_TextWidth);
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
