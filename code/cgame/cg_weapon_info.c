#include "cg_local.h"

#define VEC3(x, y, z) {x, y, z}

#define FLASH1(x) {x, NULL, NULL, NULL}
#define FLASH2(x, y) {x, y, NULL, NULL}
#define FLASH3(x, y, z) {x, y, z, NULL}
#define FLASH4(x, y, z, w) {x, y, z, w}

#define NO_MISSILE NULL, NULL, 0, 0, 0, VEC3(0, 0, 0)

#define MAX_FIREINFO 32
#define FIREINFO_FILE "fire_info.json"
#define MAX_TOKENS 1024

cg_fireInfo_t cg_fireInfo[MAX_FIREINFO];
int cg_numFireInfo = 0;

static qboolean jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      Q_strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return qtrue;
  }
  return qfalse;
}

void CG_ParseFireInfoJSON(char *json)
{
    int i, j, k, r;
    jsmn_parser parser;
    jsmntok_t tokens[MAX_TOKENS];

    jsmn_init(&parser);
    r = jsmn_parse(&parser, json, strlen(json), tokens, sizeof(tokens) / sizeof(tokens[0]));

    CG_Printf("Parsing json: %d\n", strlen(json), json);

    if (r < 0)
    {
        trap_Print( va( S_COLOR_RED "Failed to parse JSON: %d\n", r ) );
		return;
    }

    if (tokens[0].type != JSMN_ARRAY)
    {
        trap_Print(S_COLOR_RED "JSON top object is not an array\n");
        return;
    }

    for (i = 1; i < r; i++)
    {
        if (tokens[i].type == JSMN_ARRAY)
        {
            for (j = 1; i + j < r; j += 2)
            {
                jsmntok_t *keyToken = &tokens[i + j];
                jsmntok_t *valueToken;
                char key[128];
                char value[MAX_QPATH];

                if (keyToken->type == JSMN_ARRAY)
                {
                    i += j - 1;
                    break; // we hit the next item
                }

                valueToken = &tokens[i + j + 1];

                Q_strncpyz(key, json + keyToken->start, keyToken->end - keyToken->start);
                Q_strncpyz(value, json + valueToken->start, valueToken->end - valueToken->start);

                CG_Printf("Parsing token %s: %s\n", key, value);

                if (Q_stricmp(key, "name") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].identifier = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].identifier, value, strlen(value));
                } 
                else if (Q_stricmp(key, "idleSound") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].idleSound = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].idleSound, value, strlen(value));
                }
                else if (Q_stricmp(key, "fireSound") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].fireSound = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].fireSound, value, strlen(value));
                }
                else if (Q_stricmp(key, "flashColor") == 0)
                {
                    char colorValue[8];
                    jsmntok_t *colorTok = &tokens[i + j + 2];

                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start);
                    cg_fireInfo[cg_numFireInfo].flashColor[0] = atof(colorValue);

                    colorTok = &tokens[i + j + 3];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start);
                    cg_fireInfo[cg_numFireInfo].flashColor[1] = atof(colorValue);

                    colorTok = &tokens[i + j + 4];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start);
                    cg_fireInfo[cg_numFireInfo].flashColor[2] = atof(colorValue);
                    j += 3;
                }
                else if (Q_stricmp(key, "flashSound") == 0)
                {
                    char path[MAX_QPATH];
                    jsmntok_t *tok;

                    for(k = 0; k < keyToken->size && k < 4; k++)
                    {
                        tok = &tokens[i + j + 2 + k];
                        Q_strncpyz(path, json + tok->start, tok->end - tok->start);
                        cg_fireInfo[cg_numFireInfo].flashSound[k] = trap_HeapMalloc(strlen(path));
                        Q_strncpyz(cg_fireInfo[cg_numFireInfo].flashSound[k], path, strlen(path));
                    }

                    j += keyToken->size;
                }
                else if (Q_stricmp(key, "missileModel") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileModel = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].missileModel, value, strlen(value));
                }
                else if (Q_stricmp(key, "missileSound") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileSound = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].missileSound, value, strlen(value));
                }
                else if (Q_stricmp(key, "missileTrailTime") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileTrailTime = atoi(value);
                }
                else if (Q_stricmp(key, "missileTrailRadius") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileTrailRadius = atoi(value);
                }
                else if (Q_stricmp(key, "missileDlight") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileDlight = atoi(value);
                }
                else if (Q_stricmp(key, "missileDlightColor") == 0)
                {
                    char colorValue[8];
                    jsmntok_t *colorTok = &tokens[i + j + 2];

                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start);
                    cg_fireInfo[cg_numFireInfo].missileDlightColor[0] = atof(colorValue);

                    colorTok = &tokens[i + j + 3];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start);
                    cg_fireInfo[cg_numFireInfo].missileDlightColor[1] = atof(colorValue);

                    colorTok = &tokens[i + j + 4];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start);
                    cg_fireInfo[cg_numFireInfo].missileDlightColor[2] = atof(colorValue);

                    j += 2;
                }
                else if (Q_stricmp(key, "brassType") == 0)
                {
                    if (Q_stricmp(value, "none") == 0)
                        cg_fireInfo[cg_numFireInfo].brassType = BF_NONE;
                    else if (Q_stricmp(value, "bullet") == 0)
                        cg_fireInfo[cg_numFireInfo].brassType = BF_BULLET;
                    else if (Q_stricmp(value, "shell") == 0)
                        cg_fireInfo[cg_numFireInfo].brassType = BF_SHELL;
                }
                else if (Q_stricmp(key, "customShading") == 0)
                {
                    if (Q_stricmp(value, "true") == 0)
                        cg_fireInfo[cg_numFireInfo].customShading = qtrue;
                    else 
                        cg_fireInfo[cg_numFireInfo].customShading = qfalse;
                }
            }
            CG_Printf("Parsed info for %s\n", cg_fireInfo[cg_numFireInfo].identifier);
            cg_numFireInfo++;
        }
    }
}

void CG_FireInfoLoad(void)
{
    int				len;
	fileHandle_t	f;
	char			buf[8192];

	len = trap_FS_FOpenFile( FIREINFO_FILE, &f, FS_READ );
	
    if ( !f ) {
		trap_Print( va( S_COLOR_RED "file not found: %s\n", FIREINFO_FILE ) );
		return;
	}

	if ( len >= 8192 ) {
		trap_Print( va( S_COLOR_RED "file too large: %s is %i, max allowed is %i\n", FIREINFO_FILE, len, MAX_BOTS_TEXT ) );
		trap_FS_FCloseFile( f );
		return;
	}

	trap_FS_Read( buf, len, f );
	buf[len] = 0;
	trap_FS_FCloseFile( f );

    CG_ParseFireInfoJSON(buf);
}

cg_fireInfo_t *CG_FireInfo(const char *name)
{
    int i;

    for(i = 0; i < cg_numFireInfo; i++)
        if (!Q_stricmp(name, cg_fireInfo[i].identifier))
            return &cg_fireInfo[i];

    return NULL;
}
