#include "cg_local.h"

#define VEC3(x, y, z) {x, y, z}

#define FLASH1(x) {x, NULL, NULL, NULL}
#define FLASH2(x, y) {x, y, NULL, NULL}
#define FLASH3(x, y, z) {x, y, z, NULL}
#define FLASH4(x, y, z, w) {x, y, z, w}

#define NO_MISSILE NULL, NULL, 0, 0, 0, VEC3(0, 0, 0)

cg_fireInfo_t cg_fireInfo[] = {
    {
        "rail_fire",
        "sound/weapons/railgun/rg_hum.wav",
        NULL,
        VEC3(1, 0.5f, 0),
        FLASH1("sound/weapons/railgun/railgf1a.wav"),
        NO_MISSILE,
        BF_NONE,
        qtrue
    },
    {
        "minirail_fire",
        "sound/weapons/minirail/rg_hum.wav",
        NULL,
        VEC3(1, 0.5f, 0),
        FLASH1("sound/weapons/minirail/railgf1a.wav"),
        NO_MISSILE,
        BF_NONE,
        qtrue
    },
    {
        "gauntlet_fire",
        NULL,
        "sound/weapons/melee/fstrun.wav",
        VEC3(0.6f, 0.6f, 1),
        FLASH1("sound/weapons/melee/fstatck.wav"),
        NO_MISSILE,
        BF_NONE,
        qfalse
    },
    {
        "lightning_fire",
        "sound/weapons/melee/fsthum.wav",
        "sound/weapons/lightning/lg_hum.wav",
        VEC3(0.6f, 0.6f, 1.0f),
        FLASH1("sound/weapons/lightning/lg_fire.wav"),
        NO_MISSILE,
        BF_NONE,
        qfalse
    },
    {
        "mg_fire",
        NULL,
        NULL,
        VEC3(1, 1, 0),
        FLASH4("sound/weapons/machinegun/machgf1b.wav",
                "sound/weapons/machinegun/machgf2b.wav",
                "sound/weapons/machinegun/machgf3b.wav",
                "sound/weapons/machinegun/machgf4b.wav"),
        NO_MISSILE,
        BF_BULLET,
        qfalse
    },
    {
        "sg_fire",
        NULL,
        NULL,
        VEC3(1, 1, 0),
        FLASH1("sound/weapons/shotgun/sshotf1b.wav"),
        NO_MISSILE,
        BF_SHELL,
        qfalse
    },
    {
        "rocket_fire",
        NULL,
        NULL,
        VEC3(1, 0.75f, 0),
        FLASH1("sound/weapons/rocket/rocklf1a.wav"),
        "models/ammo/rocket/rocket.md3",
        "sound/weapons/rocket/rockfly.wav",
        2000,
        64,
        200,
        VEC3(1, 0.75f, 0),
        BF_NONE,
        qfalse
    },
    {
        "grenade_fire",
        NULL,
        NULL,
        VEC3(1, 0.7f, 0),
        FLASH1("sound/weapons/grenade/grenlf1a.wav"),
        "models/ammo/grenade1.md3",
        NULL,
        700,
        32,
        0,
        VEC3(0, 0, 0),
        BF_NONE,
        qfalse
    },
    {
        "impact_fire",
        NULL,
        NULL,
        VEC3(1, 0.6f, 0),
        FLASH1("sound/weapons/impactcannon/rocklf1a.wav"),
        NULL,
        "sound/weapons/impactcannon/rockfly.wav",
        0,
        0,
        0,
        VEC3(0, 0, 0),
        BF_NONE,
        qfalse
    },
    {
        "plasma_fire",
        NULL,
        NULL,
        VEC3(0.6f, 0.6f, 1),
        FLASH1("sound/weapons/plasma/hyprbf1a.wav"),
        NULL,
        "sound/weapons/plasma/lasfly.wav",
        0,
        0,
        0,
        VEC3(0, 0, 0),
        BF_NONE,
        qfalse
    },
    {
        "bfg_fire",
        "sound/weapons/bfg/bfg_hum.wav",
        NULL,
        VEC3(1, 0.7f, 1),
        FLASH1("sound/weapons/bfg/bfg_fire.wav"),
        "models/weaphits/bfg.md3",
        "sound/weapons/rocket/rockfly.wav",
        0,
        0,
        0,
        VEC3(0, 0, 0),
        BF_NONE,
        qfalse
    },
};

int cg_numFireInfo = ARRAY_LEN(cg_fireInfo);

cg_fireInfo_t *CG_FireInfo(const char *name)
{
    int i;

    for(i = 0; i < cg_numFireInfo; i++)
        if (!Q_stricmp(name, cg_fireInfo[i].identifier))
            return &cg_fireInfo[i];

    return NULL;
}
