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
        CH_MenuMain();
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

    Menu_AddItem(&m_debug.menu, &m_debug.option);
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
