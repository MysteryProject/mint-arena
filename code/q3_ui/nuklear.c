#define NK_IMPLEMENTATION
#include "ui_local.h"

void UI_assert(qboolean expression)
{
    if (!expression)
        Com_Printf("assert failed");
}

void NK_DrawRectMultiColor(float x, float y, float w, float h, vec4_t *colors)
{
    polyVert_t verts[4];

    if (ARRAY_LEN(*colors) < 4)
    {
        CG_Printf("NK_DrawRectMultiColor: must have 4 colors\n");
        return;
    }

    CG_AdjustFrom640(&x, &y, &w, &h);

    Vector2Set(verts[0].xyz, x, y);
    Vector4Copy(colors[0], verts[0].modulate);
    Vector2Set(verts[0].st, 0, 0);

    Vector2Set(verts[1].xyz, x + w, y);
    Vector4Copy(colors[1], verts[1].modulate);
    Vector2Set(verts[1].st, 0, 1);

    Vector2Set(verts[2].xyz, x + w, y + h);
    Vector4Copy(colors[2], verts[2].modulate);
    Vector2Set(verts[2].st, 1, 1);

    Vector2Set(verts[3].xyz, x, y + h);
    Vector4Copy(colors[3], verts[3].modulate);
    Vector2Set(verts[3].st, 1, 0);

    trap_R_Add2dPolys(verts, 4, cgs.media.whiteShader );
}

void NK_DrawLine(vec2_t start, vec2_t end, unsigned short thickness, vec4_t color)
{
#if 1
    float length, w = thickness / 2;
    vec2_t v, p, n;
    polyVert_t verts[4];

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
#else
    float slope = (end[1] - start[1]) / (end[0] - start[0]);
    float yintersect = start[1] + (slope * start[0]);
    float xNudge = thickness / 2.0f;
    polyVert_t verts[4];

    CG_AdjustFrom640(&start[0], &start[1], NULL, NULL);
    CG_AdjustFrom640(&end[0], &end[1], NULL, NULL);

    Vector2Set(verts[0].xyz, end[0] - xNudge, -(slope * (end[0] - xNudge) + yintersect));
    Vector4Copy(color, verts[0].modulate);
    Vector2Set(verts[0].st, 0, 0);

    Vector2Set(verts[1].xyz, end[0] + xNudge, -(slope * (end[0] + xNudge) + yintersect));
    Vector4Copy(color, verts[1].modulate);
    Vector2Set(verts[1].st, 0, 1);

    Vector2Set(verts[2].xyz, start[0] + xNudge, (slope * (start[0] + xNudge) + yintersect));
    Vector4Copy(color, verts[2].modulate);
    Vector2Set(verts[2].st, 1, 1);

    Vector2Set(verts[3].xyz, start[0] - xNudge, (slope * (start[0] - xNudge) + yintersect));
    Vector4Copy(color, verts[3].modulate);
    Vector2Set(verts[3].st, 1, 0);

    trap_R_Add2dPolys(verts, 4, cgs.media.whiteShader );
#endif
}

void NK_DrawImage(float x, float y, float w, float h, qhandle_t handle, vec4_t color)
{
    trap_R_SetColor(color);
    CG_DrawPic(x, y, w, h, handle);
    trap_R_SetColor(NULL);
}

#define CIRCLE_VERTS	21

void NK_DrawCircleFilled(float x, float y, float w, float h, vec4_t color)
{
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
        case NK_COMMAND_TEXT: {
            const struct nk_command_text *t = (const struct nk_command_text*)cmd;
			NKTOQ3COLOR(t->foreground, color);
            CG_DrawStringCommon(t->x, t->y, t->string, 0, &uis.textFont, color, 0, 0, 0, 0, -1, -1, 0);
        } break;
        default:
            CG_Printf("No handler for nuklear type %d\n", cmd->type);
            break;
        }
    }
    nk_clear(ctx);
}
