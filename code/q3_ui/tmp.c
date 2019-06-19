struct nk_context ctx;

struct q3Font {
	fontInfo_t *handle;
	struct nk_user_font nk;
};

struct q3Font myFont;

/*
=================
UI_Shutdown
=================
*/
void UI_Shutdown( void ) {
	nk_free(&ctx);
}


static float nk_q3font_get_text_width(nk_handle handle, float height, const char *text, int len)
{
	return Text_Width(text, (fontInfo_t *)handle.ptr, 1.0, 0);
}

/*
=================
UI_Init
=================
*/

struct nk_allocator alloc;

static void*
nkmalloc(nk_handle unused, void *old,nk_size size)
{
    NK_UNUSED(unused);
    NK_UNUSED(old);
    return trap_HeapMalloc(size);
}

static void
nkmfree(nk_handle unused, void *ptr)
{
    NK_UNUSED(unused);
    trap_HeapFree(ptr);
}

void UI_Init( qboolean inGameLoad, int maxSplitView ) {
	uis.maxSplitView = Com_Clamp(1, MAX_SPLITVIEW, maxSplitView);

	Key_SetCatcher( KEYCATCH_UI );

	if (!inGameLoad)
	{
		CG_RegisterCamos();
	}

	UI_RegisterCvars();

	UI_InitGameinfo();

	// initialize the menu system
	Menu_Cache();

	uis.activemenu = NULL;
	uis.menusp     = 0;

    alloc.userdata.ptr = 0;
    alloc.alloc = nkmalloc;
    alloc.free = nkmfree;

	myFont.handle = &uis.textFont;
	myFont.nk.userdata = nk_handle_ptr(&myFont);
	myFont.nk.height = uis.textFont.pointSize + 2;
	myFont.nk.width = nk_q3font_get_text_width;
	nk_init(&ctx, &alloc, &myFont.nk);
}

#define NKTOQ3COLOR(nk, c) (c[0] = nk.r / 255.0f, c[1] = nk.g/ 255.0f, c[2] = nk.b/ 255.0f, c[3] = nk.a/ 255.0f)

void UI_NuklearRender(void)
{
	const struct nk_command *cmd;
	vec4_t color;

    nk_foreach(cmd, &ctx)
    {
		switch (cmd->type) {
        case NK_COMMAND_NOP: break;
        case NK_COMMAND_SCISSOR: break;
        case NK_COMMAND_LINE: break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
			NKTOQ3COLOR(r->color, color);
			CG_Printf("Drawing box %d %d %d %d\n", r->x, r->y, r->w, r->h);
            CG_DrawRect(r->x, r->y, r->w, r->h, 2, color);
        } break;
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *t = (const struct nk_command_text*)cmd;
			NKTOQ3COLOR(t->foreground, color);
			CG_Printf("Drawing text: %s %f %f %f %f\n", t->string, color[0], color[1], color[2], color[3]);
			CG_DrawStringCommon(t->x, t->y, t->string, UI_CENTER, &uis.textFont, color, 0, 0, 0, 0, -1, -1, 0);
		} break;
        case NK_COMMAND_RECT_MULTI_COLOR:
        case NK_COMMAND_IMAGE:
        case NK_COMMAND_ARC_FILLED:
        default: break;
        }
    }
    nk_clear(&ctx);
}

/*
=================
UI_Refresh
=================
*/
void UI_Refresh( int realtime )
{
	uis.frametime = realtime - uis.realtime;
	uis.realtime  = realtime;

	if ( !( Key_GetCatcher() & KEYCATCH_UI ) ) {
		CG_Printf("WHYYY\n");
		return;
	}

	UI_UpdateCvars();

	CG_ClearViewport();
	CG_DrawPic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, uis.menuBackShader );

	// nuklear
	{
		if (nk_begin(&ctx, "Test", nk_rect(50, 50, 200, 200), 0))
		{
			enum {EASY, HARD};
			static int op = EASY;
			static int property = 20;

			nk_layout_row_static(&ctx, 30, 80, 1);
			if (nk_button_label(&ctx, "button"))
				Com_Printf("button pressed\n");
			nk_layout_row_dynamic(&ctx, 30, 2);
			if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
			if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;
			nk_layout_row_dynamic(&ctx, 22, 1);
			nk_property_int(&ctx, "Compression:", 0, &property, 100, 10, 1);
		}
		nk_end(&ctx);

		UI_NuklearRender();
	}

	// draw cursor
	trap_R_SetColor( NULL );
	CG_DrawPic( uis.cursorx-16, uis.cursory-16, 32, 32, uis.cursor);

	// delay playing the enter sound until after the
	// menu has been drawn, to avoid delay while
	// caching images
	if (m_entersound)
	{
		trap_S_StartLocalSound( menu_in_sound, CHAN_LOCAL_SOUND );
		m_entersound = qfalse;
	}
}