#define NK_IMPLEMENTATION
#include "ui_local.h"

void UI_assert(qboolean expression)
{
    if (!expression)
        Com_Printf("assert failed");
}



void NK_RenderCommandList(struct nk_context *ctx)
{
    const struct nk_command *cmd;
    vec4_t color;

    nk_foreach(cmd, ctx)
    {
		switch (cmd->type) {
        case NK_COMMAND_IMAGE: {
            const struct nk_command_image *image = (const struct nk_command_image*)cmd;
			NKTOQ3COLOR(image->col, color);
            NK_DrawImage(image->x, image->y, image->w, image->h, image->img.handle.id, color);
        } break;
        default:
            CG_Printf("No handler for nuklear type %d\n", cmd->type);
            break;
        }
    }
    nk_clear(ctx);
}
