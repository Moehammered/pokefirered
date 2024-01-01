#include "extras/extras.h"
#include "global.h"
#include "gba/types.h"

u8 GetStarterMode()
{
    return gSaveBlock1Ptr->settings.starterSetting;
}

u8 GetGamemode()
{
    return gSaveBlock1Ptr->settings.gamemode;
}

u8 GetNationalFromStart()
{
    return gSaveBlock1Ptr->settings.nationalDexFromStart;
}