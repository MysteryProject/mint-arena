#define NK_IMPLEMENTATION
#include "ui_local.h"

void UI_assert(qboolean expression)
{
    if (!expression)
        Com_Printf("assert failed");
}
