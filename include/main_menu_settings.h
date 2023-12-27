#ifndef __MAIN_MENU_SETTINGS_H__
#define __MAIN_MENU_SETTINGS_H__

#include "global.h"

// bool32 PrintMysteryGiftMenuMessage(u8 * cmdPtr, const u8 * src);
void AddTextPrinterToWindow_MainMenuSettings(const u8 *str);
void MainCB_MainMenuSettings_ReturnToTitleScreen(void);
void PrintMainMenuSettingsTopMenu(void);
void CB2_InitMainMenuSettings(void);
void MainMenuSettings_DrawTextBorder(u8 windowId);
// void CB2_MysteryGiftEReader(void);
// s8 DoMysteryGiftYesNo(u8 * textState, u16 * windowId, bool8 yesNoBoxPlacement, const u8 * str);
// u16 GetMysteryGiftBaseBlock(void);

#endif