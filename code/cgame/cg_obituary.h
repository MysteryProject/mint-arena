#ifndef __CG_OBITUARY_H__
#define __CG_OBITUARY_H__

#include "cg_local.h"

#define OBIT_MAX_VISABLE 5

#define OBIT_MAX_NAME_LENGTH 28

#define OBIT_FADE_TIME 5000

#define OBIT_POS_X (640 - 200)
#define OBIT_POS_Y (480 - 120)

#define OBIT_ICON_HEIGHT 32
#define OBIT_ICON_WIDTH 64

#define OBIT_SPACING 25
#define OBIT_GAP_WIDTH 10

#define fontType UI_TINYFONT
#define fontFlags (UI_PULSE | fontType)

#define MAX_MODEL_INFO 32

#define MI_NONE -1
#define MI_HEADSHOT -2
#define MI_SAMETEAM -3

typedef struct
{
    char attacker[OBIT_MAX_NAME_LENGTH];
    char target[OBIT_MAX_NAME_LENGTH];
    char weapon[OBIT_MAX_NAME_LENGTH];
    team_t attackerTeam;
    team_t targetTeam;
    int time;
} obituary_t;

#endif
