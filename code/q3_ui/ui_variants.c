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
#define ART_SELECTED "menu/art/maps_selected"
#define ART_SELECT "menu/art/maps_select"
#define ART_ARROWS "menu/art/gs_arrows_0"
#define ART_ARROWL "menu/art/gs_arrows_l"
#define ART_ARROWR "menu/art/gs_arrows_r"

#define ART_CAMO_DEFAULT "menu/art/unknownmap"
#define ART_CAMO_CHERRY "camos/cherry_blossom"
#define ART_CAMO_BACON "camos/bacon"
#define ART_CAMO_BENJAMINS "camos/benjamins"

#define MAX_CAMOS 9

#define ID_NEXT (MAX_CAMOS + 11)
#define ID_PREVIOUS (MAX_CAMOS + 12)
#define ID_BACK (MAX_CAMOS + 13)

#define CAMO_START_X 0
#define CAMO_START_Y 75
#define CAMO_SPACING_X 150
#define CAMO_SPACING_Y 110

typedef struct
{
    menuframework_s menu;
    menutext_s weaponName;
    menubitmap_s arrows;
    menubitmap_s next;
    menubitmap_s prev;
    menubitmap_s playerfront;
    menubitmap_s back;

    menubitmap_s camoDisplay[MAX_CAMOS];
    menubitmap_s camoButton[MAX_CAMOS];
    int camoCount;
    int camosX;
    int camosY;
    int selection;

    uiPlayerInfo_t playerinfo;
    weapon_t weapon;
    int localPlayerNum;
    int variantNum;

    char playerHead[MAX_QPATH];
    char playerModel[MAX_QPATH];
} variants_t;

static variants_t s_variants;

static void Variants_UpdateModel(void)
{
    vec3_t viewangles;
    vec3_t moveangles;
    gitem_t *item;
    qboolean oneHanded;
    int anim, i;

    for (i = 0; i < MAX_CAMOS; i++)
    {
        s_variants.camoDisplay[i].generic.flags &= ~QMF_HIGHLIGHT;

        if (i == s_variants.variantNum)
            s_variants.camoDisplay[i].generic.flags |= QMF_HIGHLIGHT;
    }

    memset(&s_variants.playerinfo, 0, sizeof(uiPlayerInfo_t));

    viewangles[YAW] = 180 + 60;
    viewangles[PITCH] = 0;
    viewangles[ROLL] = 0;
    VectorClear(moveangles);

    item = BG_FindItemForWeapon(s_variants.weapon);
    oneHanded = item->oneHanded;
    anim = oneHanded ? TORSO_STAND2: TORSO_STAND;
   
    UI_PlayerInfo_SetModel(&s_variants.playerinfo, s_variants.playerModel, s_variants.playerHead, NULL);
    UI_PlayerInfo_SetInfo(&s_variants.playerinfo, s_variants.localPlayerNum, LEGS_IDLE, anim, viewangles, moveangles, s_variants.weapon, qfalse);

    s_variants.playerinfo.variant = s_variants.variantNum;

    if (item)
        s_variants.weaponName.string = item->displayName;
}

static void VariantsMenu_Save(void)
{
    gitem_t *item = BG_FindItemForWeapon(s_variants.weapon);
    trap_Cvar_SetValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, va("variant_%s", item->classname)), s_variants.variantNum);
}

static void Variants_ReadVariant(void)
{
    gitem_t *item = BG_FindItemForWeapon(s_variants.weapon);
    s_variants.variantNum = trap_Cvar_VariableValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, va("variant_%s", item->classname)));
}

void VariantsMenu_Event(void *ptr, int event)
{
    int id;
    if (event != QM_ACTIVATED)
    {
        return;
    }

    switch (id = ((menucommon_s *)ptr)->id)
    {
        case ID_NEXT:
            VariantsMenu_Save();

            s_variants.weapon++;

            if (s_variants.weapon >= WP_NUM_WEAPONS)
                s_variants.weapon = WP_NONE + 1;
            else if (s_variants.weapon == WP_GRAPPLING_HOOK)
                s_variants.weapon++;

            Variants_ReadVariant();
            Variants_UpdateModel();
            break;
        case ID_PREVIOUS:
            VariantsMenu_Save();

            s_variants.weapon--;

            if (s_variants.weapon <= WP_NONE)
                s_variants.weapon = WP_NUM_WEAPONS - 1;
            else if (s_variants.weapon == WP_GRAPPLING_HOOK)
                s_variants.weapon--;

            Variants_ReadVariant();
            Variants_UpdateModel();
            break;
        case ID_BACK:
            VariantsMenu_Save();
            UI_PopMenu();
            break;

            /*    case ID_VARIANT1:
        s_variants.variantNum = 0;
        //trap_Cvar_SetValue(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "variant_weapon_machinegun"), 0);
        Variants_UpdateModel();
        break;*/
    default:
        {
            s_variants.variantNum = id - 10;

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
    trap_Cvar_VariableStringBuffer(Com_LocalPlayerCvarName(s_variants.localPlayerNum, "model"), model, sizeof(model));
    UI_PlayerInfo_SetModel(&s_variants.playerinfo, model, headmodel, NULL);
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
    trap_R_RegisterShaderNoMip(ART_CAMO_DEFAULT);
    trap_R_RegisterShaderNoMip(ART_CAMO_CHERRY);
    trap_R_RegisterShaderNoMip(ART_SELECTED);
    trap_R_RegisterShaderNoMip(ART_SELECT);
    trap_R_RegisterShaderNoMip(ART_ARROWL);
    trap_R_RegisterShaderNoMip(ART_ARROWR);
    trap_R_RegisterShaderNoMip(ART_ARROWS);
}

#define VERTICAL_SPACING 24
#define VARIANT_TEXT_WIDTH 140
#define MAX_NAMELENGTH 16

static void Variants_LevelshotDraw(void *self)
{
    menubitmap_s *b;
    int x;
    int y;
    int w;
    int h;
    const char *info;
    char mapname[MAX_NAMELENGTH];

    b = (menubitmap_s *)self;

    if (!b->generic.name)
    {
        return;
    }

    if (b->generic.name && !b->shader)
    {
        b->shader = trap_R_RegisterShaderNoMip(b->generic.name);
        if (!b->shader && b->errorpic)
        {
            b->shader = trap_R_RegisterShaderNoMip(b->errorpic);
        }
    }

    if (b->focuspic && !b->focusshader)
    {
        b->focusshader = trap_R_RegisterShaderNoMip(b->focuspic);
    }

    x = b->generic.x;
    y = b->generic.y;
    w = b->width;
    h = b->height;
    if (b->shader)
    {
        CG_DrawPic(x, y, w, h, b->shader);
    }

    x = b->generic.x;
    y = b->generic.y + b->height;
    CG_FillRect(x, y, b->width, 28, colorBlack);

    x += b->width / 2;
    y += 4;

    UI_DrawString(x, y, s_variants.camoButton[b->generic.id-10].generic.name, UI_CENTER | UI_SMALLFONT, color_orange);

    x = b->generic.x;
    y = b->generic.y;
    w = b->width;
    h = b->height + 28;
    if (b->generic.flags & QMF_HIGHLIGHT)
    {
        CG_DrawPic(x, y, w, h, b->focusshader);
    }
}

static void AddCamoMenuItem(char *camo, char *name)
{
    if (s_variants.camoCount != 0)
    {
        if (s_variants.camoCount % 3 == 0)
        {
            s_variants.camosX = CAMO_START_X;
            s_variants.camosY += CAMO_SPACING_Y;
        }
        else
            s_variants.camosX += CAMO_SPACING_X;
    }

    s_variants.camoDisplay[s_variants.camoCount].generic.type = MTYPE_BITMAP;
    s_variants.camoDisplay[s_variants.camoCount].generic.name = camo;
    s_variants.camoDisplay[s_variants.camoCount].generic.flags = QMF_LEFT_JUSTIFY | QMF_INACTIVE;
    s_variants.camoDisplay[s_variants.camoCount].generic.x = s_variants.camosX;
    s_variants.camoDisplay[s_variants.camoCount].generic.y = s_variants.camosY;
    s_variants.camoDisplay[s_variants.camoCount].generic.id = s_variants.camoCount + 10;
    s_variants.camoDisplay[s_variants.camoCount].generic.ownerdraw = Variants_LevelshotDraw;
    s_variants.camoDisplay[s_variants.camoCount].focuspic = ART_SELECTED;
    s_variants.camoDisplay[s_variants.camoCount].width = 128;
    s_variants.camoDisplay[s_variants.camoCount].height = 75;

    s_variants.camoButton[s_variants.camoCount].generic.type = MTYPE_BITMAP;
    s_variants.camoButton[s_variants.camoCount].generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
    s_variants.camoButton[s_variants.camoCount].generic.id = s_variants.camoCount + 10;
    s_variants.camoButton[s_variants.camoCount].generic.callback = VariantsMenu_Event;
    s_variants.camoButton[s_variants.camoCount].generic.x = s_variants.camosX - 30;
    s_variants.camoButton[s_variants.camoCount].generic.y = s_variants.camosY - 26;
    s_variants.camoButton[s_variants.camoCount].width = 254;
    s_variants.camoButton[s_variants.camoCount].height = 200;
    s_variants.camoButton[s_variants.camoCount].generic.left = s_variants.camosX;
    s_variants.camoButton[s_variants.camoCount].generic.top = s_variants.camosY;
    s_variants.camoButton[s_variants.camoCount].generic.right = s_variants.camosX + 128;
    s_variants.camoButton[s_variants.camoCount].generic.bottom = s_variants.camosY + 128;
    s_variants.camoButton[s_variants.camoCount].generic.name = name;
    s_variants.camoButton[s_variants.camoCount].focuspic = ART_SELECT;

    Menu_AddItem(&s_variants.menu, &s_variants.camoDisplay[s_variants.camoCount]);
    Menu_AddItem(&s_variants.menu, &s_variants.camoButton[s_variants.camoCount]);

    s_variants.camoCount++;
}

static void VariantsMenu_Init(int localPlayerNum)
{
    int y, i, x;
    gitem_t *item;

    memset(&s_variants, 0, sizeof(s_variants));

    s_variants.weapon = WP_MACHINEGUN;
    s_variants.localPlayerNum = localPlayerNum;

    Variants_ReadVariant();
    Variants_Cache();

    s_variants.menu.fullscreen = qtrue;
    s_variants.menu.wrapAround = qtrue;
    s_variants.camosX = CAMO_START_X;
    s_variants.camosY = CAMO_START_Y;

    y = 20;

    item = BG_FindItemForWeapon(s_variants.weapon);

    x = SCREEN_WIDTH / 2;

    s_variants.weaponName.generic.type = MTYPE_BTEXT;
    s_variants.weaponName.generic.flags = QMF_CENTER_JUSTIFY;
    s_variants.weaponName.generic.x = x;
    s_variants.weaponName.generic.y = y;
    s_variants.weaponName.generic.callback = VariantsMenu_Event;
    s_variants.weaponName.string = "";
    s_variants.weaponName.color = text_big_color;
    s_variants.weaponName.style = UI_CENTER;

    s_variants.arrows.generic.type = MTYPE_BITMAP;
    s_variants.arrows.generic.name = ART_ARROWS;
    s_variants.arrows.generic.flags = QMF_INACTIVE;
    s_variants.arrows.generic.x = 480;
    s_variants.arrows.generic.y = 400;
    s_variants.arrows.width = 128;
    s_variants.arrows.height = 32;

    s_variants.prev.generic.type = MTYPE_BITMAP;
    s_variants.prev.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    s_variants.prev.generic.callback = VariantsMenu_Event;
    s_variants.prev.generic.id = ID_PREVIOUS;
    s_variants.prev.generic.x = 480;
    s_variants.prev.generic.y = 400;
    s_variants.prev.width = 64;
    s_variants.prev.height = 32;
    s_variants.prev.focuspic = ART_ARROWL;

    s_variants.next.generic.type = MTYPE_BITMAP;
    s_variants.next.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    s_variants.next.generic.callback = VariantsMenu_Event;
    s_variants.next.generic.id = ID_NEXT;
    s_variants.next.generic.x = 541;
    s_variants.next.generic.y = 400;
    s_variants.next.width = 64;
    s_variants.next.height = 32;
    s_variants.next.focuspic = ART_ARROWR;

    s_variants.playerfront.generic.type = MTYPE_BITMAP;
    s_variants.playerfront.generic.flags = QMF_INACTIVE;
    s_variants.playerfront.generic.ownerdraw = Variants_DrawModel;
    s_variants.playerfront.generic.x = 400;
    s_variants.playerfront.generic.y = -40;
    s_variants.playerfront.width = 320;
    s_variants.playerfront.height = 560;

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

    Menu_AddItem(&s_variants.menu, &s_variants.weaponName);
    Menu_AddItem(&s_variants.menu, &s_variants.arrows);
    Menu_AddItem(&s_variants.menu, &s_variants.prev);
    Menu_AddItem(&s_variants.menu, &s_variants.next);
    Menu_AddItem(&s_variants.menu, &s_variants.playerfront);
    Menu_AddItem(&s_variants.menu, &s_variants.back);

    AddCamoMenuItem(ART_CAMO_DEFAULT, "Default");
    AddCamoMenuItem(ART_CAMO_CHERRY, "Cherry Blossom");
    AddCamoMenuItem(ART_CAMO_BACON, "Bacon");
    AddCamoMenuItem(ART_CAMO_BENJAMINS, "Benjamins");
    AddCamoMenuItem("camos/jungle", "Jungle");
    AddCamoMenuItem("camos/yellow_flake", "Yellow Flake");
    AddCamoMenuItem("camos/uk", "UK");
    AddCamoMenuItem("camos/shattered_glass", "Shattered Glass");
    AddCamoMenuItem("camos/green_leaf", "Green Leaf");

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
