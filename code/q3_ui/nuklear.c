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

void NK_DrawTriangleFilled(vec2_t a, vec2_t b, vec2_t c, vec4_t color)
{
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
}

void NK_RenderCommandList(struct nk_context *ctx)
{
    const struct nk_command *cmd;
    vec4_t color;

    nk_foreach(cmd, ctx)
    {
		switch (cmd->type) {
        case NK_COMMAND_RECT_MULTI_COLOR: {
            const struct nk_command_rect_multi_color *rmc = (const struct nk_command_rect_multi_color*)cmd;
            vec4_t colors[4];
			NKTOQ3COLOR2(rmc->left, colors[0]);
			NKTOQ3COLOR2(rmc->top, colors[1]);
            NKTOQ3COLOR2(rmc->right, colors[2]);
			NKTOQ3COLOR2(rmc->bottom, colors[3]);
            NK_DrawRectMultiColor(rmc->x, rmc->y, rmc->w, rmc->h, colors);
        } break;
        case NK_COMMAND_LINE: {
            const struct nk_command_line *line = (const struct nk_command_line*)cmd;
            vec2_t a, b;
            Vector2Set(a, line->begin.x, line->begin.y);
            Vector2Set(b, line->end.x, line->end.y);
			NKTOQ3COLOR2(line->color, color);
            NK_DrawLine(a, b, line->line_thickness, color);
        }
        break;
        case NK_COMMAND_IMAGE: {
            const struct nk_command_image *image = (const struct nk_command_image*)cmd;
			NKTOQ3COLOR(image->col, color);
            NK_DrawImage(image->x, image->y, image->w, image->h, image->img.handle.id, color);
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const struct nk_command_circle_filled *cf = (const struct nk_command_circle_filled*)cmd;
			NKTOQ3COLOR(cf->color, color);
            NK_DrawCircleFilled(cf->x, cf->y, cf->w, cf->h, color);
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled *trf = (const struct nk_command_triangle_filled*)cmd;
            vec2_t a, b, c;
            Vector2Set(a, trf->a.x, trf->a.y);
            Vector2Set(b, trf->b.x, trf->b.y);
            Vector2Set(c, trf->c.x, trf->c.y);
			NKTOQ3COLOR2(trf->color, color);
            NK_DrawTriangleFilled(a, b, c, color);
        } break;
        case NK_COMMAND_SCISSOR: {
            const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
            CG_SetClipRegion(s->x, s->y, s->w, s->h);
        } break;
        case NK_COMMAND_RECT_FILLED: {
            const struct nk_command_rect_filled *fr = (const struct nk_command_rect_filled *)cmd;
			NKTOQ3COLOR(fr->color, color);
            CG_FillRect(fr->x, fr->y, fr->w, fr->h, color);
        } break;
        case NK_COMMAND_RECT: {
            const struct nk_command_rect *r = (const struct nk_command_rect *)cmd;
			NKTOQ3COLOR(r->color, color);
            CG_DrawRect(r->x, r->y, r->w, r->h, 2, color);
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
