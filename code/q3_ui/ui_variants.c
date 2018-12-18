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
/*
=======================================================================

CONTROLS MENU

=======================================================================
*/

#include "ui_local.h"

#define ART_BACK0 "menu/art/back_0"
#define ART_BACK1 "menu/art/back_1"

#define ID_WEAPON 10
#define ID_WEAPON_END (ID_WEAPON + WP_NUM_WEAPONS)

#define ID_VARIANT1 (ID_WEAPON_END + 1)
#define ID_VARIANT2 (ID_VARIANT1 + 1)
#define ID_VARIANT3 (ID_VARIANT2 + 1)
#define ID_VARIANT4 (ID_VARIANT3 + 1)
#define ID_BACK 100

typedef struct
{
    menuframework_s menu;
    menutext_s weaponNames[WP_NUM_WEAPONS];
    menutext_s variant[4];
    menubitmap_s playerfront;
    menubitmap_s playerback;
    menubitmap_s back;

    uiPlayerInfo_t playerinfo;
    uiPlayerInfo_t playerinfo2;
    weapon_t weapon;
    int localPlayerNum;
    int variantNum;

    char playerHead[MAX_QPATH];
    char playerModel[MAX_QPATH];
} variants_t;

static variants_t s_variants;

static char *variantNames[WP_NUM_WEAPONS][4] = {
    {NULL, NULL, NULL, NULL}, // wp_none
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {"Classic", "Arcade", "NONE", "NONE"},
    {NULL, NULL, NULL, NULL}, // wp_grapple
    {"Variant 1", "NONE", "NONE", "NONE"},
    {"Variant 1", "NONE", "NONE", "NONE"},
    {"Variant 1", "NONE", "NONE", "NONE"},
    {"Variant 1", "NONE", "NONE", "NONE"}
};

static void Variants_UpdateModel(void)
{
    vec3_t viewangles;
    vec3_t viewangles2;
    vec3_t moveangles;
    vec3_t moveangles2;
    qboolean oneHanded = BG_GetWeaponDefinition(s_variants.weapon)->oneHanded;
    int anim = TORSO_STAND;
    int i;
    gitem_t *item;

    memset(&s_variants.playerinfo, 0, sizeof(uiPlayerInfo_t));
    memset(&s_variants.playerinfo2, 0, sizeof(uiPlayerInfo_t));

    viewangles[YAW] = 180 + 60;
    viewangles[PITCH] = 0;
    viewangles[ROLL] = 0;
    VectorClear(moveangles);

    viewangles2[YAW] = 180 - 60;
    viewangles2[PITCH] = 0;
    viewangles2[ROLL] = 0;
    VectorClear(moveangles2);

    UI_PlayerInfo_SetModel(&s_variants.playerinfo, s_variants.playerModel, s_variants.playerHead, NULL);
    UI_PlayerInfo_SetModel(&s_variants.playerinfo2, s_variants.playerModel, s_variants.playerHead, NULL);

    s_variants.playerinfo.variant = s_variants.variantNum;
    s_variants.playerinfo2.variant = s_variants.variantNum;

    if (oneHanded)
        anim = TORSO_STAND2;

    UI_PlayerInfo_SetInfo(&s_variants.playerinfo, LEGS_IDLE, anim, viewangles, moveangles, s_variants.weapon, qfalse);
    UI_PlayerInfo_SetInfo(&s_variants.playerinfo2, LEGS_IDLE, anim, viewangles2, moveangles2, s_variants.weapon, qfalse);

    item = BG_FindItemForWeapon(s_variants.weapon);
    for (i = 0; i < 4; i++)
    {
        s_variants.variant[i].string = variantNames[s_variants.weapon][i];
        s_variants.variant[i].generic.flags |= (QMF_GRAYED | QMF_INACTIVE);

        if (item->world_model[i] != NULL)
            s_variants.variant[i].generic.flags &= ~(QMF_GRAYED | QMF_INACTIVE);
    }
}

static void VariantsMenu_Save(void)
{
    gitem_t *item;

    item = BG_FindItemForWeapon(s_variants.weapon);

    trap_Cvar_SetValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, va("variant_%s", item->classname)), s_variants.variantNum);
}

void VariantsMenu_Event(void *ptr, int event)
{
    if (event != QM_ACTIVATED)
    {
        return;
    }

    switch (((menucommon_s *)ptr)->id)
    {
    case ID_VARIANT1:
        s_variants.variantNum = 0;
        //trap_Cvar_SetValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "variant_weapon_machinegun"), 0);
        Variants_UpdateModel();
        break;

    case ID_VARIANT2:
        s_variants.variantNum = 1;
        //trap_Cvar_SetValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "variant_weapon_machinegun"), 1);
        Variants_UpdateModel();
        break;
    case ID_VARIANT3:
        s_variants.variantNum = 2;
        //trap_Cvar_SetValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "variant_weapon_machinegun"), 1);
        Variants_UpdateModel();
        break;
    case ID_VARIANT4:
        s_variants.variantNum = 3;
        //trap_Cvar_SetValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "variant_weapon_machinegun"), 1);
        Variants_UpdateModel();
        break;
    case ID_BACK:
        VariantsMenu_Save();
        UI_PopMenu();
        break;
    default:
    {
        weapon_t weaponNum = ((menucommon_s *)ptr)->id - ID_WEAPON;
        gitem_t *item;

        VariantsMenu_Save(); // save the old one first

        if (weaponNum <= WP_NONE || weaponNum >= WP_NUM_WEAPONS)
            break;

        item = BG_FindItemForWeapon(weaponNum);

        s_variants.weapon = weaponNum;
        s_variants.variantNum = trap_Cvar_VariableValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, va("variant_%s", item->classname)));
        Variants_UpdateModel();
        }
        break;
    }
}

static sfxHandle_t VariantsMenu_Key(int key)
{
    if (key == K_MOUSE2 || key == K_ESCAPE)
    {
        VariantsMenu_Save();
    }
    return Menu_DefaultKey(&s_variants.menu, key);
}

static void Variants_InitModel(void)
{
    char model[MAX_QPATH], headmodel[MAX_QPATH];

    memset(&s_variants.playerinfo, 0, sizeof(uiPlayerInfo_t));
    memset(&s_variants.playerinfo2, 0, sizeof(uiPlayerInfo_t));

    trap_Cvar_VariableStringBuffer(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "model"), model, sizeof(model));
    trap_Cvar_VariableStringBuffer(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "headmodel"), headmodel, sizeof(headmodel));

    UI_PlayerInfo_SetModel(&s_variants.playerinfo, model, headmodel, NULL);
    UI_PlayerInfo_SetModel(&s_variants.playerinfo2, model, headmodel, NULL);

    Variants_UpdateModel();
}

static void Variants_DrawModel(void *self)
{
    menubitmap_s *b = (menubitmap_s *)self;
    char model[MAX_QPATH], headmodel[MAX_QPATH];

    trap_Cvar_VariableStringBuffer(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "model"), model, sizeof(model));
    trap_Cvar_VariableStringBuffer(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "headmodel"), headmodel, sizeof(headmodel));
    if (strcmp(model, s_variants.playerModel) != 0 || strcmp(headmodel, s_variants.playerHead) != 0)
    {
        UI_PlayerInfo_SetModel(&s_variants.playerinfo, model, headmodel, NULL);
        strcpy(s_variants.playerModel, model);
        strcpy(s_variants.playerHead, headmodel);
        Variants_UpdateModel();
    }

    UI_DrawPlayer(b->generic.x, b->generic.y, b->width, b->height, &s_variants.playerinfo, uis.realtime / 2);
}

static void Variants_DrawModel2(void *self)
{
    menubitmap_s *b = (menubitmap_s *)self;
    char model[MAX_QPATH], headmodel[MAX_QPATH];

    trap_Cvar_VariableStringBuffer(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "model"), model, sizeof(model));
    trap_Cvar_VariableStringBuffer(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "headmodel"), headmodel, sizeof(headmodel));
    if (strcmp(model, s_variants.playerModel) != 0 || strcmp(headmodel, s_variants.playerHead) != 0)
    {
        UI_PlayerInfo_SetModel(&s_variants.playerinfo2, model, headmodel, NULL);
        strcpy(s_variants.playerModel, model);
        strcpy(s_variants.playerHead, headmodel);
        Variants_UpdateModel();
    }

    UI_DrawPlayer(b->generic.x, b->generic.y, b->width, b->height, &s_variants.playerinfo2, uis.realtime / 2);
}

void Setup_MenuText(menutext_s *mtext, char *string, int x, int y, int flags, int style, int id)
{
    mtext->generic.type = MTYPE_PTEXT;
    mtext->generic.flags = flags | QMF_PULSEIFFOCUS;
    mtext->generic.x = x;
    mtext->generic.y = y;
    mtext->generic.id = id;
    mtext->generic.callback = VariantsMenu_Event;
    mtext->string = string;
    mtext->color = text_big_color;
    mtext->style = style | UI_DROPSHADOW | UI_SMALLFONT;
}

void Variants_Cache(void)
{
    trap_R_RegisterShaderNoMip(ART_BACK0);
    trap_R_RegisterShaderNoMip(ART_BACK1);
}

#define VERTICAL_SPACING 24
#define VARIANT_TEXT_WIDTH 140

static void VariantsMenu_Init(int localPlayerNum)
{
    int y, i, x;
    gitem_t *item;
    char *variantStrings[4] = {"Variant 1", "Variant 2", "Variant 3", "Variant 4"};

    memset(&s_variants, 0, sizeof(s_variants));

    s_variants.weapon = WP_MACHINEGUN;
    s_variants.variantNum = trap_Cvar_VariableValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "variant_weapon_machinegun"));
    s_variants.localPlayerNum = localPlayerNum;

    Variants_Cache();

    s_variants.menu.fullscreen = qtrue;
    s_variants.menu.wrapAround = qtrue;

    y = SCREEN_HEIGHT/2 - ((VERTICAL_SPACING * (WP_NUM_WEAPONS - 3)) / 2);

    for (i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++)
    {
        if (i == WP_GRAPPLING_HOOK)
            continue;

        item = BG_FindItemForWeapon(i);

        if (item == NULL)
            continue;

        Setup_MenuText(&s_variants.weaponNames[i], item->pickup_name, -60, y, QMF_LEFT_JUSTIFY, UI_LEFT, ID_WEAPON + i);
        y += VERTICAL_SPACING;
    }

    item = BG_FindItemForWeapon(s_variants.weapon);

    x = 460 - (VARIANT_TEXT_WIDTH * 3) / 2;

    for (i = 0; i < 4; i++)
    {
        Setup_MenuText(&s_variants.variant[i], variantNames[s_variants.weapon][i], x + (VARIANT_TEXT_WIDTH * i), SCREEN_HEIGHT - 50, QMF_CENTER_JUSTIFY, UI_CENTER, ID_VARIANT1 + i);
    }

    s_variants.playerfront.generic.type = MTYPE_BITMAP;
    s_variants.playerfront.generic.flags = QMF_INACTIVE;
    s_variants.playerfront.generic.ownerdraw = Variants_DrawModel;
    s_variants.playerfront.generic.x = 400;
    s_variants.playerfront.generic.y = -40;
    s_variants.playerfront.width = 320;
    s_variants.playerfront.height = 560;

    s_variants.playerback.generic.type = MTYPE_BITMAP;
    s_variants.playerback.generic.flags = QMF_INACTIVE;
    s_variants.playerback.generic.ownerdraw = Variants_DrawModel2;
    s_variants.playerback.generic.x = 200;
    s_variants.playerback.generic.y = -40;
    s_variants.playerback.width = 320;
    s_variants.playerback.height = 560;

    s_variants.back.generic.type = MTYPE_BITMAP;
    s_variants.back.generic.name = ART_BACK0;
    s_variants.back.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    s_variants.back.generic.x = 0;
    s_variants.back.generic.y = 480 - 64;
    s_variants.back.generic.id = ID_BACK;
    s_variants.back.generic.callback = VariantsMenu_Event;
    s_variants.back.width = 128;
    s_variants.back.height = 64;
    s_variants.back.focuspic = ART_BACK1;

    for (i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++)
    {
        if (i == WP_GRAPPLING_HOOK)
            continue;

        Menu_AddItem(&s_variants.menu, &s_variants.weaponNames[i]);
    }

    for (i = 0; i < 4; i++)
    {
        Menu_AddItem(&s_variants.menu, &s_variants.variant[i]);
    }

    Menu_AddItem(&s_variants.menu, &s_variants.playerfront);
    Menu_AddItem(&s_variants.menu, &s_variants.playerback);
    Menu_AddItem(&s_variants.menu, &s_variants.back);

    Variants_UpdateModel();
}

/*
=================
UI_VariantsMenu
=================
*/
void UI_VariantsMenu(int localPlayerNum)
{
    VariantsMenu_Init(localPlayerNum);
    UI_PushMenu(&s_variants.menu);
}
