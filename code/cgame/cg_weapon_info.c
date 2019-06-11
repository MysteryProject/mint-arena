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

void CG_ParseFireInfoJSON(void)
{
    int i, j, k, idx, r;
    jsmn_parser parser;
    jsmntok_t tokens[MAX_TOKENS];
    char json[8192];

    BG_LoadFileContents(json, "fire_info.json");

    jsmn_init(&parser);
    r = jsmn_parse(&parser, json, strlen(json), tokens, sizeof(tokens) / sizeof(tokens[0]));

    //CG_Printf("Parsing json: %d\n", strlen(json), json);

    if (r < 0)
    {
        trap_Print( va( S_COLOR_RED "CG_ParseFireInfoJSON: Failed to parse JSON: %d\n", r ) );
		return;
    }

    if (tokens[0].type != JSMN_ARRAY)
    {
        trap_Print(S_COLOR_RED "CG_ParseFireInfoJSON: JSON top object is not an array\n");
        return;
    }

    for (i = 1; i < r; i++)
    {
        if (cg_numFireInfo >= MAX_FIREINFO-1)
        {
            CG_Printf("CG_ParseFireInfoJSON: Reached max fireinfos %d\n", MAX_FIREINFO);
            break;
        }

        //CG_Printf("Token: %.*s\n", tokens[i].end - tokens[i].start, json + tokens[i].start);

        if (tokens[i].type == JSMN_OBJECT)
        {
            for (j = 1; i + j < r; j += 2)
            {
                jsmntok_t keyToken = tokens[i + j];
                jsmntok_t valueToken;
                int keyLen, valueLen;
                char key[128];
                char value[MAX_QPATH * 10];

                //CG_Printf("Tokenkey: %.*s\n", keyToken.end - keyToken.start, json + keyToken.start);

                if (keyToken.type == JSMN_OBJECT)
                {
                    i += j - 1;
                    break; // we hit the next item
                }

                valueToken = tokens[i + j + 1];

                keyLen = keyToken.end - keyToken.start;
                valueLen = valueToken.end - valueToken.start;

                if (keyLen <= 0)
                {
                    CG_Printf("CG_ParseFireInfoJSON: key length <= 0\n");
                    break;
                }

                if (valueLen <= 0)
                {
                    CG_Printf("CG_ParseFireInfoJSON: value length <= 0\n");
                    break;
                }

                Q_strncpyz(key, json + keyToken.start, sizeof(key));
                Q_strncpyz(value, json + valueToken.start, sizeof(value));

                key[keyLen] = '\0';
                value[valueLen] = '\0';

                //CG_Printf("Parsing token %s: %s\n", key, value);

                if (Q_stricmp(key, "name") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].identifier = trap_HeapMalloc(strlen(value) + 1);
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].identifier, value, strlen(value) + 1);
                    //CG_Printf("indentifier: %s\n", cg_fireInfo[cg_numFireInfo].identifier);
                } 
                else if (Q_stricmp(key, "idleSound") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].idleSound = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].idleSound, value, strlen(value) + 1);
                   //CG_Printf("idleSound: %s\n", cg_fireInfo[cg_numFireInfo].idleSound);
                }
                else if (Q_stricmp(key, "fireSound") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].fireSound = trap_HeapMalloc(strlen(value));
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].fireSound, value, strlen(value) + 1);
                    //CG_Printf("fireSound: %s\n", cg_fireInfo[cg_numFireInfo].fireSound);
                }
                else if (Q_stricmp(key, "flashColor") == 0)
                {
                    char colorValue[8];
                    jsmntok_t *colorTok = &tokens[i + j + 2];

                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start + 1);
                    cg_fireInfo[cg_numFireInfo].flashColor[0] = atof(colorValue);

                    colorTok = &tokens[i + j + 3];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start + 1);
                    cg_fireInfo[cg_numFireInfo].flashColor[1] = atof(colorValue);

                    colorTok = &tokens[i + j + 4];
                    Q_strncpyz(colorValue, json + colorTok->start, colorTok->end - colorTok->start + 1);
                    cg_fireInfo[cg_numFireInfo].flashColor[2] = atof(colorValue);
                    j += 3;
                    //CG_Printf("flashColor (%f, %f, %f)\n", cg_fireInfo[cg_numFireInfo].flashColor[0], 
                    //                                        cg_fireInfo[cg_numFireInfo].flashColor[1], 
                    //                                        cg_fireInfo[cg_numFireInfo].flashColor[2]);
                }
                else if (Q_stricmp(key, "flashSound") == 0)
                {
                    char path[MAX_QPATH];
                    jsmntok_t tok;
                    int tokLen;

                    for(k = 0; k < valueToken.size && k < 4; k++)
                    {
                        tok = tokens[i + j + 2 + k];
                        tokLen = tok.end - tok.start;
                        Q_strncpyz(path, json + tok.start, tokLen + 1);
                        cg_fireInfo[cg_numFireInfo].flashSound[k] = trap_HeapMalloc(strlen(path) + 1);
                        Q_strncpyz(cg_fireInfo[cg_numFireInfo].flashSound[k], path, strlen(path) + 1);
                        //CG_Printf("flashSound %d %s\n", k, cg_fireInfo[cg_numFireInfo].flashSound[k]); 
                    }

                    j += valueToken.size;
                    //CG_Printf("soundSize: %d\n", valueToken.size);
                }
                else if (Q_stricmp(key, "missileModel") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileModel = trap_HeapMalloc(strlen(value) + 1);
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].missileModel, value, strlen(value) + 1);
                    //CG_Printf("missileModel: %s\n", cg_fireInfo[cg_numFireInfo].missileModel);
                }
                else if (Q_stricmp(key, "missileSound") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileSound = trap_HeapMalloc(strlen(value) + 1);
                    Q_strncpyz(cg_fireInfo[cg_numFireInfo].missileSound, value, strlen(value) + 1);
                    //CG_Printf("missileSound: %s\n", cg_fireInfo[cg_numFireInfo].missileSound);
                }
                else if (Q_stricmp(key, "missileTrailTime") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileTrailTime = atoi(value);
                    //CG_Printf("missileTrailTime: %d\n", cg_fireInfo[cg_numFireInfo].missileTrailTime);
                }
                else if (Q_stricmp(key, "missileTrailRadius") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileTrailRadius = atoi(value);
                    //CG_Printf("missileTrailRadius: %d\n", cg_fireInfo[cg_numFireInfo].missileTrailRadius);
                }
                else if (Q_stricmp(key, "missileDlight") == 0)
                {
                    cg_fireInfo[cg_numFireInfo].missileDlight = atoi(value);
                    //CG_Printf("missileDlight: %d\n", cg_fireInfo[cg_numFireInfo].missileDlight);
                }
                else if (Q_stricmp(key, "missileDlightColor") == 0)
                {
                    char colorValue[8];
                    jsmntok_t colorTok = tokens[i + j + 2];

                    Q_strncpyz(colorValue, json + colorTok.start, colorTok.end - colorTok.start + 1);
                    cg_fireInfo[cg_numFireInfo].missileDlightColor[0] = atof(colorValue);

                    colorTok = tokens[i + j + 3];
                    Q_strncpyz(colorValue, json + colorTok.start, colorTok.end - colorTok.start + 1);
                    cg_fireInfo[cg_numFireInfo].missileDlightColor[1] = atof(colorValue);

                    colorTok = tokens[i + j + 4];
                    Q_strncpyz(colorValue, json + colorTok.start, colorTok.end - colorTok.start + 1);
                    cg_fireInfo[cg_numFireInfo].missileDlightColor[2] = atof(colorValue);

                    j += 3;
                }
                else if (Q_stricmp(key, "brassType") == 0)
                {
                    if (Q_stricmp(value, "none") == 0)
                        cg_fireInfo[cg_numFireInfo].brassType = BF_NONE;
                    else if (Q_stricmp(value, "bullet") == 0)
                        cg_fireInfo[cg_numFireInfo].brassType = BF_BULLET;
                    else if (Q_stricmp(value, "shell") == 0)
                        cg_fireInfo[cg_numFireInfo].brassType = BF_SHELL;

                    //CG_Printf("brassType: %d\n", cg_fireInfo[cg_numFireInfo].brassType);
                }
                else if (Q_stricmp(key, "customShading") == 0)
                {
                    if (Q_stricmp(value, "true") == 0)
                        cg_fireInfo[cg_numFireInfo].customShading = qtrue;
                    else 
                        cg_fireInfo[cg_numFireInfo].customShading = qfalse;

                    //CG_Printf("customShading: %d\n", cg_fireInfo[cg_numFireInfo].customShading);
                }
                else
                {
                    CG_Printf("CG_ParseFireInfoJSON: Could not parse token %s: %s\n", key, value);
                }
            }
            //CG_Printf("Parsed info for %s\n", cg_fireInfo[cg_numFireInfo].identifier);
            cg_numFireInfo++;
        }
    }
}

cg_fireInfo_t *CG_FireInfo(const char *name)
{
    int i;

    for(i = 0; i < cg_numFireInfo; i++)
    {
        //CG_Printf("CG_FireInfo: checking %s == %s\n", name, cg_fireInfo[i].identifier);
        if (!Q_stricmp(name, cg_fireInfo[i].identifier))
            return &cg_fireInfo[i];
    }

    CG_Printf("CG_FireInfo: could not find fire info %s\n", name);
    return NULL;
}
