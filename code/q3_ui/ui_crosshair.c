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

#define ART_BACK0 "menu/art/back_0"
#define ART_BACK1 "menu/art/back_1"
#define ART_SELECTED "menu/art/maps_selected"
#define ART_SELECT "menu/art/maps_select"
#define ART_ARROWS "menu/art/gs_arrows_0"
#define ART_ARROWL "menu/art/gs_arrows_l"
#define ART_ARROWR "menu/art/gs_arrows_r"

#define CROSSHAIR_ROWS 4
#define CROSSHAIR_COLUMNS 4
#define CROSSHAIRS_PER_PAGE (CROSSHAIR_ROWS * CROSSHAIR_COLUMNS)
#define NUM_CROSSHAIRS 45
#define NUM_PAGES (NUM_CROSSHAIRS / CROSSHAIRS_PER_PAGE) + ((NUM_CROSSHAIRS % CROSSHAIRS_PER_PAGE) ? 1 : 0)

#define ID_NEXT (CROSSHAIRS_PER_PAGE + 11)
#define ID_PREVIOUS (CROSSHAIRS_PER_PAGE + 12)
#define ID_BACK (CROSSHAIRS_PER_PAGE + 13)

#define CROSSHAIR_START_X 0
#define CROSSHAIR_START_Y 75
#define CROSSHAIR_SPACING_X 150
#define CROSSHAIR_SPACING_Y 110

typedef struct
{
    menuframework_s menu;

    menubitmap_s arrows;
    menubitmap_s next;
    menubitmap_s prev;
    menubitmap_s playerfront;
    menubitmap_s back;

    menubitmap_s activeCrosshair;
    menubitmap_s crosshairDisplay[CROSSHAIRS_PER_PAGE];
    menubitmap_s crosshairButton[CROSSHAIRS_PER_PAGE];
    qhandle_t crosshairIcon[NUM_CROSSHAIRS];

    int curX;
    int curY;
    int page;
    int selection;
    int crosshairNum;
    int localPlayerNum;
} crosshairMenu_t;

static crosshairMenu_t m_crosshair;

/*
=================
Menu Exit
=================
*/
static void CH_MenuUpdateGrid(void)
{
    int i, j;

    j = m_crosshair.page * CROSSHAIRS_PER_PAGE;

    for(i = 0; i < CROSSHAIRS_PER_PAGE; i++, j++)
    {
        if (j < NUM_CROSSHAIRS)
        {
            m_crosshair.crosshairDisplay[i].generic.name = va("crosshair%d", i);
            m_crosshair.crosshairButton[i].generic.flags &= ~QMF_INACTIVE;
        }
        else
        {
            m_crosshair.crosshairDisplay[i].generic.name = NULL;
            m_crosshair.crosshairButton[i].generic.flags |= ~QMF_INACTIVE;
        }

        m_crosshair.crosshairDisplay[i].generic.flags &= ~QMF_HIGHLIGHT;
        m_crosshair.crosshairDisplay[i].shader = 0;
        m_crosshair.crosshairButton[i].generic.flags |= QMF_PULSEIFFOCUS;
    }

    if (m_crosshair.selection / CROSSHAIRS_PER_PAGE == m_crosshair.page)
    {
        i = m_crosshair.selection % CROSSHAIRS_PER_PAGE;
        m_crosshair.crosshairDisplay[i].generic.flags |= QMF_HIGHLIGHT;
        m_crosshair.crosshairButton[i].generic.flags &= ~QMF_PULSEIFFOCUS;
    }

    if (NUM_PAGES > 1)
    {
        if (m_crosshair.page > 0)
            m_crosshair.prev.generic.flags &= ~QMF_INACTIVE;
        else
            m_crosshair.prev.generic.flags |= QMF_INACTIVE;

        if (m_crosshair.page < NUM_PAGES - 1)
            m_crosshair.next.generic.flags &= ~QMF_INACTIVE;
        else
            m_crosshair.next.generic.flags |= QMF_INACTIVE;
    }
    else
    {
        m_crosshair.prev.generic.flags |= QMF_INACTIVE;
        m_crosshair.next.generic.flags |= QMF_INACTIVE;
    }
}

/*
=================
Menu Exit
=================
*/
static void CH_MenuExit(void)
{
    trap_Cvar_SetValue(Com_LocalPlayerCvarName(m_crosshair.localPlayerNum, "cg_drawcrosshair"), m_crosshair.crosshairNum);
}

/*
=================
Menu Event
=================
*/
static void CH_MenuEvent(void *ptr, int event)
{
    int id = -1;

    if (event != QM_ACTIVATED)
        return;

    switch (id = ((menucommon_s *)ptr)->id)
    {
    case ID_NEXT:
        if (m_crosshair.page < NUM_PAGES - 1)
        {
            m_crosshair.page++;
            CH_MenuUpdateGrid();
        }
        break;
    case ID_PREVIOUS:
        if (m_crosshair.page > 0)
        {
            m_crosshair.page--;
            CH_MenuUpdateGrid();
        }
        break;
    case ID_BACK:
        CH_MenuExit();
        UI_PopMenu();
        break;
    default:
        m_crosshair.selection = (id - 10);
        m_crosshair.crosshairNum = m_crosshair.selection + (m_crosshair.page * CROSSHAIRS_PER_PAGE);
        break;
    }
}

/*
=================
Menu Key
=================
*/
static sfxHandle_t CH_MenuKey(int key)
{
    if (key == K_MOUSE2 || key == K_ESCAPE)
        CH_MenuExit();

    return Menu_DefaultKey(&m_crosshair.menu, key);
}

/*
=================
NewText
=================
*/
static int chmenuy = 320;

static void CH_NewText(menutext_s *mtext, char *string, int flags, int style, int id)
{
    chmenuy += 34;
    mtext->generic.type = MTYPE_PTEXT;
    mtext->generic.flags = flags | QMF_CENTER_JUSTIFY | QMF_PULSEIFFOCUS;
    mtext->generic.x = 320;
    mtext->generic.y = chmenuy;
    mtext->generic.id = id;
    mtext->generic.callback = CH_MenuEvent;
    mtext->string = string;
    mtext->color = text_big_color;
    mtext->style = style | UI_DROPSHADOW | UI_CENTER;
}

/*
=================
Menu Crosshair Draw
=================
*/
static void CH_MenuCrosshairDraw(void *self)
{
    menubitmap_s *s;
    float *color;
    int x, y;
    int style;
    qboolean focus;
    vec4_t crosshairColor;

    s = (menubitmap_s *)self;
    x = s->generic.x;
    y = s->generic.y;

    style = UI_SMALLFONT;
    
    if (s->generic.flags & QMF_GRAYED)
        color = text_color_disabled;
    else if (focus)
    {
        color = text_color_highlight;
        style |= UI_PULSE;
    }
    else if (s->generic.flags & QMF_BLINK)
    {
        color = text_color_highlight;
        style |= UI_BLINK;
    }
    else
        color = text_color_normal;

    if (focus)
    {
        // draw cursor
    //    CG_FillRect(s->generic.left, s->generic.top, s->generic.right - s->generic.left + 1, s->generic.bottom - s->generic.top + 1, listbar_color);
    //    UI_DrawChar(x, y, GLYPH_ARROW, UI_CENTER | UI_BLINK | UI_SMALLFONT, color);
    }

    UI_DrawString(x - SMALLCHAR_WIDTH, y, s->generic.name, style | UI_RIGHT, color);

    VectorSet(crosshairColor, 1, 0, 0);

    crosshairColor[3] = 1;

    CG_DrawPicColor(x + SMALLCHAR_WIDTH, y - 4, 24, 24, m_crosshair.crosshairIcon[m_crosshair.crosshairNum], crosshairColor);
}

/*
=================
Menu Cache
=================
*/
void CH_MenuCache(void)
{
    int i = 0;

    for (i = 0; i < NUM_CROSSHAIRS; i++)
        m_crosshair.crosshairIcon[i] = trap_R_RegisterShader(va("crosshairs/crosshair%d", i + 1));
}

#define VERTICAL_SPACING 24
#define VARIANT_TEXT_WIDTH 140
#define MAX_NAMELENGTH 16

/*
=================
Menu Init
=================
*/
static void CH_AddCrosshair(int crosshairCount, int x, int y)
{
    m_crosshair.crosshairDisplay[crosshairCount].generic.type = MTYPE_BITMAP;
    //m_crosshair.crosshairDisplay[crosshairCount].generic.name = "YES";
    m_crosshair.crosshairDisplay[crosshairCount].generic.flags = QMF_LEFT_JUSTIFY | QMF_INACTIVE;
    m_crosshair.crosshairDisplay[crosshairCount].generic.x = x;
    m_crosshair.crosshairDisplay[crosshairCount].generic.y = y;
    m_crosshair.crosshairDisplay[crosshairCount].generic.id = crosshairCount + 10;
    m_crosshair.crosshairDisplay[crosshairCount].generic.ownerdraw = CH_MenuCrosshairDraw;
    m_crosshair.crosshairDisplay[crosshairCount].focuspic = ART_SELECTED;
    m_crosshair.crosshairDisplay[crosshairCount].focuscolor = colorRed;
    m_crosshair.crosshairDisplay[crosshairCount].width = 64;
    m_crosshair.crosshairDisplay[crosshairCount].height = 64;

    m_crosshair.crosshairButton[crosshairCount].generic.type = MTYPE_BITMAP;
    m_crosshair.crosshairButton[crosshairCount].generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS | QMF_NODEFAULTINIT;
    m_crosshair.crosshairButton[crosshairCount].generic.id = crosshairCount + 10;
    m_crosshair.crosshairButton[crosshairCount].generic.callback = CH_MenuEvent;
    m_crosshair.crosshairButton[crosshairCount].generic.x = x - 16;
    m_crosshair.crosshairButton[crosshairCount].generic.y = y - 16;
    m_crosshair.crosshairButton[crosshairCount].width = 128;
    m_crosshair.crosshairButton[crosshairCount].height = 128;
    m_crosshair.crosshairButton[crosshairCount].generic.left = x;
    m_crosshair.crosshairButton[crosshairCount].generic.top = y;
    m_crosshair.crosshairButton[crosshairCount].generic.right = x + 64;
    m_crosshair.crosshairButton[crosshairCount].generic.bottom = y + 64;
    //m_crosshair.crosshairButton[crosshairCount].generic.name = "YES";
    m_crosshair.crosshairButton[crosshairCount].focuspic = ART_SELECT;
    m_crosshair.crosshairButton[crosshairCount].focuscolor = colorRed;

    Menu_AddItem(&m_crosshair.menu, &m_crosshair.crosshairDisplay[crosshairCount]);
    Menu_AddItem(&m_crosshair.menu, &m_crosshair.crosshairButton[crosshairCount]);
}

/*
=================
Menu Init
=================
*/
void CH_MenuInit(void)
{
    int i, j, k, x, y;

    memset(&m_crosshair, 0, sizeof(m_crosshair));

    m_crosshair.crosshairNum = 1;
    m_crosshair.selection = 1;

    m_crosshair.menu.wrapAround = qtrue;
    m_crosshair.menu.fullscreen = qtrue;

    m_crosshair.arrows.generic.type = MTYPE_BITMAP;
    m_crosshair.arrows.generic.name = ART_ARROWS;
    m_crosshair.arrows.generic.flags = QMF_INACTIVE;
    m_crosshair.arrows.generic.x = 480;
    m_crosshair.arrows.generic.y = 400;
    m_crosshair.arrows.width = 128;
    m_crosshair.arrows.height = 32;

    m_crosshair.prev.generic.type = MTYPE_BITMAP;
    m_crosshair.prev.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    m_crosshair.prev.generic.callback = CH_MenuEvent;
    m_crosshair.prev.generic.id = ID_PREVIOUS;
    m_crosshair.prev.generic.x = 480;
    m_crosshair.prev.generic.y = 400;
    m_crosshair.prev.width = 64;
    m_crosshair.prev.height = 32;
    m_crosshair.prev.focuspic = ART_ARROWL;

    m_crosshair.next.generic.type = MTYPE_BITMAP;
    m_crosshair.next.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    m_crosshair.next.generic.callback = CH_MenuEvent;
    m_crosshair.next.generic.id = ID_NEXT;
    m_crosshair.next.generic.x = 541;
    m_crosshair.next.generic.y = 400;
    m_crosshair.next.width = 64;
    m_crosshair.next.height = 32;
    m_crosshair.next.focuspic = ART_ARROWR;

    m_crosshair.back.generic.type = MTYPE_BITMAP;
    m_crosshair.back.generic.name = ART_BACK0;
    m_crosshair.back.generic.flags = QMF_LEFT_JUSTIFY | QMF_PULSEIFFOCUS;
    m_crosshair.back.generic.x = 0;
    m_crosshair.back.generic.y = 480 - 64;
    m_crosshair.back.generic.id = ID_BACK;
    m_crosshair.back.generic.callback = CH_MenuEvent;
    m_crosshair.back.width = 128;
    m_crosshair.back.height = 64;
    m_crosshair.back.focuspic = ART_BACK1;

    y = 59;
    for (i = 0; i < CROSSHAIR_ROWS; i++)
    {
        x = 50;
        for (j = 0; j < CROSSHAIR_COLUMNS; j++, k++)
        {
            CH_AddCrosshair(k, x, y);
            x += 64 + 6;
        }
        y += 64 + 6;
    }

    Menu_AddItem(&m_crosshair.menu, &m_crosshair.arrows);
    Menu_AddItem(&m_crosshair.menu, &m_crosshair.prev);
    Menu_AddItem(&m_crosshair.menu, &m_crosshair.next);
    Menu_AddItem(&m_crosshair.menu, &m_crosshair.back);

    CH_MenuUpdateGrid();
}

/*
=================
Menu Main
=================
*/
void CH_MenuMain(void)
{
    CH_MenuInit();
    UI_PushMenu(&m_crosshair.menu);
}
