#ifndef __SAVE_SETTINGS_H__
#define __SAVE_SETTINGS_H__

#include "gba/types.h"

struct GameSettings //6 bytes for now
{
    u8  gamemode;
    u8  starterSetting;
    u8  nationalDexFromStart;
    u8  padding[3];
} __attribute__((packed));

#endif