#ifndef __CG_OBITUARY_H__
#define __CG_OBITUARY_H__

#include "cg_local.h"

#define OBIT_MAX_VISABLE 8

#define fontType UI_TINYFONT
#define fontFlags (UI_PULSE | fontType)

#define MI_NONE -1
#define MI_HEADSHOT -2
#define MI_SAMETEAM -3

typedef struct
{
    char attacker[MAX_NAME_LENGTH];
    char target[MAX_NAME_LENGTH];
    char weapon[MAX_NAME_LENGTH];
    team_t attackerTeam;
    team_t targetTeam;
    int time;
} obituary_t;

#endif
