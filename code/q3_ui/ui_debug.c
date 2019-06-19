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

#define NKTOQ3COLOR(nk, c) (c[0] = nk.r / 255.0f, c[1] = nk.g/ 255.0f, c[2] = nk.b/ 255.0f, c[3] = nk.a/ 255.0f)
#define MAKEPOLYVERT(vert, vec, color) (vert.xyz[0] = vec.x, \
                                        vert.xyz[1] = vec.y, \
                                        vert.modulate[0] = color[0], \
                                        vert.modulate[1] = color[1], \
                                        vert.modulate[2] = color[2], \
                                        vert.modulate[3] = color[3])

struct nk_image testImage;

static void DebugDraw(void)
{
    const struct nk_command *cmd;
	vec4_t color;
    int property;


    nk_input_motion(&uis.nContext, uis.cursorx, uis.cursory);
    nk_input_end(&uis.nContext);

    if (nk_begin(&uis.nContext, "Debug Menu", nk_rect(0, 0, 640, 480), 0))
    {
        int active = 0;

        nk_layout_row_static(&uis.nContext, 30, 80, 1);
        // label
        nk_label(&uis.nContext, "Text Widget", NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);
        // button
        if (nk_button_label(&uis.nContext, m_debug.option.string))
            CG_Printf("Pressed button\n");
        // image button
        if (nk_button_image(&uis.nContext, testImage))
            CG_Printf("Pressed image\n");

        //
        if (nk_option_label(&uis.nContext, "easy", 1))
        {}//    CG_Printf("Pressed easy\n");
        if (nk_option_label(&uis.nContext, "hard", 0))
        {}//    CG_Printf("Pressed hard\n");
        if (nk_radio_label(&uis.nContext, "Instagib", &active))
        {}//    CG_Printf("Pressed hard\n");

        //
        nk_property_int(&uis.nContext, "Property Thing:", 0, &property, 100, 10, 1);

        // slider
        {
            static int sliderValue = 0;
            
            sliderValue = nk_slide_int(&uis.nContext, 0, sliderValue, 100, 1);

            nk_label(&uis.nContext, va("Slider: %d", sliderValue), 0);
        }

        {
            static nk_size proVal = 0;
            
            proVal = nk_prog(&uis.nContext, proVal, 100, 1);

            nk_label(&uis.nContext, va("Progress: %d", proVal), 0);
        }

        {
            static struct nk_colorf pColor;

            if (nk_color_pick(&uis.nContext, &pColor, NK_RGBA))
            {
                vec4_t color;
                NKTOQ3COLOR(pColor, color);
                CG_Printf("Current color: %f %f %f %f\n", color[0], color[1], color[2], color[3]);
            }
        }
    }
    nk_end(&uis.nContext);

    // reset before the next frame
    nk_input_begin(&uis.nContext);


    nk_foreach(cmd, &uis.nContext)
    {
		switch (cmd->type) {
        case NK_COMMAND_RECT_MULTI_COLOR: {
            const struct nk_command_rect_multi_color *rmc = (const struct nk_command_rect_multi_color*)cmd;
        } break;
        case NK_COMMAND_LINE: {
            const struct nk_command_line *line = (const struct nk_command_line*)cmd;
        } break;
        case NK_COMMAND_IMAGE: {
            const struct nk_command_image *image = (const struct nk_command_image*)cmd;
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const struct nk_command_circle_filled *cf = (const struct nk_command_circle_filled*)cmd;
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled *trf = (const struct nk_command_triangle_filled*)cmd;
            polyVert_t verts[3];

			NKTOQ3COLOR(trf->color, color);
            MAKEPOLYVERT(verts[0], trf->a, color);
            verts[0].st[0] = 0;
        	verts[0].st[1] = 0;
            MAKEPOLYVERT(verts[1], trf->b, color);
            verts[1].st[0] = 0;
        	verts[1].st[1] = 1;
            MAKEPOLYVERT(verts[2], trf->c, color);
            verts[2].st[0] = 1;
        	verts[2].st[1] = 0;
            trap_R_Add2dPolys(verts, 3, cgs.media.whiteShader );
        } break;
        case NK_COMMAND_SCISSOR: {
            const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
            CG_SetClipRegion(s->x, s->y, s->w, s->h);
        } break;
        case NK_COMMAND_RECT_FILLED: {
            const struct nk_command_rect_filled *fr = (const struct nk_command_rect_filled *)cmd;
			NKTOQ3COLOR(fr->color, color);
			//CG_Printf("Drawing filled box %d %d %d %d\n", r->x, r->y, r->w, r->h);
            CG_FillRect(fr->x, fr->y, fr->w, fr->h, color);
        } break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
			NKTOQ3COLOR(r->color, color);
			//CG_Printf("Drawing box %d %d %d %d\n", r->x, r->y, r->w, r->h);
            CG_DrawRect(r->x, r->y, r->w, r->h, 2, color);
        } break;
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *t = (const struct nk_command_text*)cmd;
			NKTOQ3COLOR(t->foreground, color);
			//CG_Printf("Drawing text: %s %f %f %f %f\n", t->string, color[0], color[1], color[2], color[3]);
			CG_DrawStringCommon(t->x, t->y, t->string, UI_CENTER, &uis.textFont, color, 0, 0, 0, 0, -1, -1, 0);
        } break;
        default:
            CG_Printf("No handler for nuklear type %d\n", cmd->type);
            break;
        }
    }
    nk_clear(&uis.nContext);
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

    m_debug.menu.draw = DebugDraw;
    m_debug.menu.key2 = DebugKey;

    testImage = nk_image_id(cgs.media.botSkillShaders[4]);

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
