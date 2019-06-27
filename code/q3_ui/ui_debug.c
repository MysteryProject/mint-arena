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

#define MAX_CROSSHAIRS 45

struct nk_image testImage;
struct nk_image crosshairImage;

qhandle_t crosshairs[MAX_CROSSHAIRS];
int crosshairNum = 0;

struct nk_style_button button_style;

void NK_RenderCommandList(struct nk_context *ctx);

void HandleCrosshairChange(int num)
{
    crosshairNum += num;

    if (crosshairNum < 0)
        crosshairNum = MAX_CROSSHAIRS;
    else if (crosshairNum >= MAX_CROSSHAIRS)
        crosshairNum = 0;

    crosshairImage = nk_image_id(crosshairs[crosshairNum]);    
}

struct nk_colorf color;

static void DebugDraw(void)
{
    struct nk_vec2 padding, spacing;

    nk_input_motion(&uis.nContext, uis.cursorx, uis.cursory);
    nk_input_end(&uis.nContext);

    padding = uis.nContext.style.window.padding;
    spacing = uis.nContext.style.window.spacing;

    uis.nContext.style.window.padding.x = 0;
    uis.nContext.style.window.padding.y = 0;
    uis.nContext.style.window.spacing.x = 0;
    uis.nContext.style.window.spacing.y = 0;

    if (nk_begin(&uis.nContext, "Settings", nk_rect(160, 0, 320, 480), 0))
    {
        nk_layout_row_dynamic(&uis.nContext, 320, 1);
        nk_image_color(&uis.nContext, crosshairImage, nk_rgba_cf(color));

        nk_layout_row_dynamic(&uis.nContext, 25, 1);
        nk_property_float(&uis.nContext, "#R", 0, &color.r, 1.0f, 0.01f, 0.005f);
        nk_property_float(&uis.nContext, "#G", 0, &color.g, 1.0f, 0.01f, 0.005f);
        nk_property_float(&uis.nContext, "#B", 0, &color.b, 1.0f, 0.01f, 0.005f);
        nk_property_float(&uis.nContext, "#A", 0, &color.a, 1.0f, 0.01f, 0.005f);
    }
    nk_end(&uis.nContext);

    uis.nContext.style.window.padding = padding;
    uis.nContext.style.window.spacing = spacing;

    NK_RenderCommandList(&uis.nContext);

    // reset before the next frame
    nk_input_begin(&uis.nContext);
}

static sfxHandle_t DebugKey( int key, qboolean down ) {
    int nKey = -1;

    //CG_Printf("Key event %d\n", key);

    switch(key)
    {
        case K_MOUSE1:
            nk_input_button(&uis.nContext, NK_BUTTON_LEFT, uis.cursorx, uis.cursory, down);
            break;
        case K_ENTER:
            nKey = NK_KEY_ENTER;
            break;
        default:
            break;
    }

    if (nKey != -1)
    {
        nk_input_key(&uis.nContext, nKey, down);
        //nk_input_key(&uis.nContext, nKey, qfalse); // FIXME: hack around
    }
    else
        nk_input_char(&uis.nContext, key);

    return Menu_DefaultKey( NULL, key );
}

/*
=================
Menu Init
=================
*/
void MenuInit(void)
{
    memset(&m_debug, 0, sizeof(m_debug));

    m_debug.menu.wrapAround = qtrue;
    m_debug.menu.fullscreen = qtrue;

    NewText(&m_debug.option, "Option", 0, 0, Option);

    testImage = nk_image_id(cgs.media.consoleShader);
    crosshairImage = nk_image_id(crosshairs[0]);

    m_debug.menu.draw = DebugDraw;
    m_debug.menu.key2 = DebugKey;

    Menu_AddItem(&m_debug.menu, &m_debug.option);
}

/*
=================
Menu Main
=================
*/
void MenuMain(void)
{
    int i = 0;

    for (i = 0; i < NUM_CROSSHAIRS; i++)
        crosshairs[i] = trap_R_RegisterShader(va("crosshairs/crosshair%d", i + 1));

    MenuInit();
    UI_PushMenu(&m_debug.menu);
}
