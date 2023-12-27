#include "main_menu_settings.h"
#include "global.h"
#include "gflib.h"
#include "gpu_regs.h"
#include "main.h"
#include "save.h"
#include "text.h"
#include "task.h"
#include "window.h"
#include "menu.h"
#include "strings.h"
#include "string_util.h"
#include "bg.h"
#include "text_window_graphics.h"
#include "text_window.h"
#include "scanline_effect.h"
#include "palette.h"
#include <string.h>
#include "main_menu.h"

#define array_size(arr) sizeof(arr)/sizeof(arr[0])

void AddTextPrinterToWindow_MainMenuSettings(const u8 *str);
void MainCB_MainMenuSettings_ReturnToTitleScreen(void);
void PrintMainMenuSettingsTopMenu(void);
void CB2_InitMainMenuSettings(void);
void CB2_RunMainMenuSettings(void);
static void Task_MainMenuSettingsMenu(u8 taskId);
void MainMenuSettings_DrawTextBorder(u8 windowId);
static void InitMainMenuSettingsMenuBg(void);
static void Task_MainMenuSettingsInit(u8 taskId);

enum
{
    MAINMENU_SETTINGS_GAMEMODE = 0,
    MAINMENU_SETTINGS_OK,
    MAINMENU_SETTINGS_COUNT
};

// Window Ids
enum
{
    MAINMENUOPTIONWIN_TEXT_OPTION,
    MAINMENUOPTIONWIN_OPTIONS
};

struct MainMenuSettingsMenu
{
    /*0x00*/ u16 option[MAINMENU_SETTINGS_COUNT];
    /*0x0E*/ u16 cursorPos;
    /*0x10*/ u8 loadState;
    /*0x11*/ u8 state;
    /*0x12*/ u8 loadPaletteState;
};

static struct MainMenuSettingsMenu *sMainMenuSettingsMenuPtr = NULL;


// Data Definitions
static const struct WindowTemplate sMainMenuSettingsMenuWinTemplates[] =
{
    {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 3,
        .width = 26,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 2
    },
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 7,
        .width = 26,
        .height = 12,
        .paletteNum = 1,
        .baseBlock = 0x36
    },
    {
        .bg = 2,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x16e
    },
    DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sMainMenuSettingsMenuBgTemplates[] =
{
   {
       .bg = 1,
       .charBaseIndex = 1,
       .mapBaseIndex = 30,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 0,
       .baseTile = 0
   },
   {
       .bg = 0,
       .charBaseIndex = 1,
       .mapBaseIndex = 31,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 1,
       .baseTile = 0
   },
   {
       .bg = 2,
       .charBaseIndex = 1,
       .mapBaseIndex = 29,
       .screenSize = 0,
       .paletteMode = 0,
       .priority = 2,
       .baseTile = 0
   },
};

enum {
    Gamemode_Vanilla_Option = 0,
    Gamemode_Random_Option,
    Gamemode_Nuzlocke_Option, //Not implemented yet
    Gamemode_Solo_Option,
    Gamemode_Count,
};

static const u8 *const sTextGamemodeOptions[Gamemode_Count] = {
    [Gamemode_Vanilla_Option] = gText_Vanilla,
    [Gamemode_Random_Option] = gText_Random,
    [Gamemode_Nuzlocke_Option] = gText_Nuzlocke,
    [Gamemode_Solo_Option] = gText_Solo
};

static const u16 sMainMenuSettingsMenuPalette[] = INCBIN_U16("graphics/misc/option_menu.gbapal");
static const u16 sMainMenuSettingsMenuItemCounts[MAINMENU_SETTINGS_COUNT] = {array_size(sTextGamemodeOptions), 1};

static const u8 *const sMainMenuSettingsMenuItemsNames[MAINMENU_SETTINGS_COUNT] =
{
    [MAINMENU_SETTINGS_GAMEMODE] = gText_Gamemode,
    // [MAINMENUOPTIONITEM_TEXTSPEED]   = gText_TextSpeed,
    // [MAINMENUOPTIONITEM_BATTLESCENE] = gText_BattleScene,
    // [MAINMENUOPTIONITEM_BATTLESTYLE] = gText_BattleStyle,
    // [MAINMENUOPTIONITEM_SOUND]       = gText_Sound,
    // [MAINMENUOPTIONITEM_BUTTONMODE]  = gText_ButtonMode,
    // [MAINMENUOPTIONITEM_FRAMETYPE]   = gText_Frame,
    [MAINMENU_SETTINGS_OK]      = gText_Ok,
};

// static const u8 *const sTextSpeedOptions[] =
// {
//     gText_TextSpeedSlow, 
//     gText_TextSpeedMid, 
//     gText_TextSpeedFast
// };

// static const u8 *const sBattleSceneOptions[] =
// {
//     gText_BattleSceneOn, 
//     gText_BattleSceneOff
// };

// static const u8 *const sBattleStyleOptions[] =
// {
//     gText_BattleStyleShift,
//     gText_BattleStyleSet
// };

// static const u8 *const sSoundOptions[] =
// {
//     gText_SoundMono, 
//     gText_SoundStereo
// };

// static const u8 *const sButtonTypeOptions[] =
// {
// 	gText_ButtonTypeLR,
// 	gText_ButtonTypeLEqualsA
// };

static const u8 sMainMenuOptionMenuPickSwitchCancelTextColor[] = {TEXT_DYNAMIC_COLOR_6, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
static const u8 sMainMenuOptionMenuTextColor[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED};


void CreateMainMenuOptionTask()
{
    u32 taskId = CreateTask(Task_MainMenuSettingsInit, 0);
    //Task.Data is s16  * 16 = s32 * 8 = 4 bytes * 8 = 32 bytes
    struct MainMenuSettingsMenu * data = (void *)gTasks[taskId].data;
    memset(data, 0, sizeof(struct MainMenuSettingsMenu));
    sMainMenuSettingsMenuPtr = data;
    sMainMenuSettingsMenuPtr->option[MAINMENU_SETTINGS_GAMEMODE] = min(max(gSaveBlock1Ptr->gamemode, 0), Gamemode_Count-1);
    // data->state = MG_STATE_TO_MAIN_MENU;
    // data->textState = 0;
    // data->unused4 = 0;
    // data->unused5 = 0;
    // data->isWonderNews = FALSE;
    // data->sourceIsFriend = FALSE;
    // data->var = 0;
    // data->unused1 = 0;
    // data->unused2 = 0;
    // data->unused3 = 0;
    // data->msgId = 0;
    // data->clientMsg = AllocZeroed(CLIENT_MAX_MSG_SIZE);
}

void AddTextPrinterToWindow_MainMenuSettings(const u8 *str)
{

}

void MainCB_MainMenuSettings_ReturnToTitleScreen(void)
{

}

void PrintMainMenuSettingsTopMenu(void)
{

}

void CB2_MainMenuOption_InitTaskSystem(void)
{
    RunTasks();
    RunTextPrinters();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void CB2_InitMainMenuSettings(void)
{
    CreateMainMenuOptionTask(); //kick off the main task to run here
    SetMainCallback2(CB2_MainMenuOption_InitTaskSystem); //set callback to setup task system
}

void CB2_RunMainMenuSettings(void)
{
    //init default menu or load existing
    u32 taskId = CreateTask(Task_MainMenuSettingsMenu, 0);
    struct MainMenuSettingsMenu * data = (void *)gTasks[taskId].data;
    memset(data, 0, sizeof(struct MainMenuSettingsMenu));
    sMainMenuSettingsMenuPtr = data;
    sMainMenuSettingsMenuPtr->option[MAINMENU_SETTINGS_GAMEMODE] = min(max(gSaveBlock1Ptr->gamemode, 0), Gamemode_Count-1);
    // CreateMainMenuOptionTask(); //kick off the main task to run here
    // SetMainCallback2(CB2_MainMenuOption_InitTaskSystem); //set callback to setup task system
}

void MainMenuSettings_DrawTextBorder(u8 windowId)
{

}

static void VBlankCB_MainMenuOptionMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static u8 MainMenuOptionMenu_ProcessInput(void)
{ 
    u16 current;
    u16 *curr;
    if (JOY_REPT(DPAD_RIGHT))
    {
        current = sMainMenuSettingsMenuPtr->option[(sMainMenuSettingsMenuPtr->cursorPos)];
        if (current == (sMainMenuSettingsMenuItemCounts[sMainMenuSettingsMenuPtr->cursorPos] - 1))
            sMainMenuSettingsMenuPtr->option[sMainMenuSettingsMenuPtr->cursorPos] = 0;
        else
            sMainMenuSettingsMenuPtr->option[sMainMenuSettingsMenuPtr->cursorPos] = current + 1;

        return 4;
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        curr = &sMainMenuSettingsMenuPtr->option[sMainMenuSettingsMenuPtr->cursorPos];
        if (*curr == 0)
            *curr = sMainMenuSettingsMenuItemCounts[sMainMenuSettingsMenuPtr->cursorPos] - 1;
        else
            --*curr;
        
        return 4;
    }
    else if (JOY_REPT(DPAD_UP))
    {
        if (sMainMenuSettingsMenuPtr->cursorPos == MAINMENU_SETTINGS_GAMEMODE)
            sMainMenuSettingsMenuPtr->cursorPos = MAINMENU_SETTINGS_OK;
        else
            sMainMenuSettingsMenuPtr->cursorPos = sMainMenuSettingsMenuPtr->cursorPos - 1;
        return 3;        
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (sMainMenuSettingsMenuPtr->cursorPos == MAINMENU_SETTINGS_OK)
            sMainMenuSettingsMenuPtr->cursorPos = MAINMENU_SETTINGS_GAMEMODE;
        else
            sMainMenuSettingsMenuPtr->cursorPos = sMainMenuSettingsMenuPtr->cursorPos + 1;
        return 3;
    }
    else if (JOY_NEW(B_BUTTON) || JOY_NEW(A_BUTTON))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void BufferMainMenuOptionMenuString(u8 selection)
{
    u8 str[20];
    u8 buf[12];
    u8 dst[3];
    u8 x, y;
    
    memcpy(dst, sMainMenuOptionMenuTextColor, 3);
    x = 0x82;
    y = ((GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT) - 1) * selection) + 2;
    FillWindowPixelRect(1, 1, x, y, 0x46, GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT));

    switch (selection)
    {
    case MAINMENU_SETTINGS_GAMEMODE:
        AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sTextGamemodeOptions[sMainMenuSettingsMenuPtr->option[selection]]);
        break;
    // case MAINMENUOPTIONITEM_TEXTSPEED:
    //     AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sTextSpeedOptions[sMainMenuSettingsMenuPtr->option[selection]]);
    //     break;
    // case MAINMENUOPTIONITEM_BATTLESCENE:
    //     AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sBattleSceneOptions[sMainMenuSettingsMenuPtr->option[selection]]);
    //     break;
    // case MAINMENUOPTIONITEM_BATTLESTYLE:
    //     AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sBattleStyleOptions[sMainMenuSettingsMenuPtr->option[selection]]);
    //     break;
    // case MAINMENUOPTIONITEM_SOUND:
    //     AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sSoundOptions[sMainMenuSettingsMenuPtr->option[selection]]);
    //     break;
    // case MAINMENUOPTIONITEM_BUTTONMODE:
    //     AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sButtonTypeOptions[sMainMenuSettingsMenuPtr->option[selection]]);
    //     break;
    // case MAINMENUOPTIONITEM_FRAMETYPE:
    //     StringCopy(str, gText_FrameType);
    //     ConvertIntToDecimalStringN(buf, sMainMenuSettingsMenuPtr->option[selection] + 1, 1, 2);
    //     StringAppendN(str, buf, 3);
    //     AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, str);
    //     break;
    default:
        break;
    }
    PutWindowTilemap(1);
    CopyWindowToVram(1, COPYWIN_FULL);
}

static void UpdateMainMenuOptionSettingSelectionDisplay(u16 selection)
{
    u16 maxLetterHeight, y;
    
    maxLetterHeight = GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT);
    y = selection * (maxLetterHeight - 1) + 0x3A;
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(y, y + maxLetterHeight));
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0x10, 0xE0));
}

static void CloseAndSaveOptionMenu(u8 taskId)
{
    // SetMainCallback2(gMain.savedCallback);
    FreeAllWindowBuffers();
    gSaveBlock1Ptr->gamemode = sMainMenuSettingsMenuPtr->option[MAINMENU_SETTINGS_GAMEMODE];
    TrySavingData(SAVE_LINK);
    // gSaveBlock2Ptr->optionsTextSpeed = sOptionMenuPtr->option[MENUITEM_TEXTSPEED];
    // gSaveBlock2Ptr->optionsBattleSceneOff = sOptionMenuPtr->option[MENUITEM_BATTLESCENE];
    // gSaveBlock2Ptr->optionsBattleStyle = sOptionMenuPtr->option[MENUITEM_BATTLESTYLE];
    // gSaveBlock2Ptr->optionsSound = sOptionMenuPtr->option[MENUITEM_SOUND];
    // gSaveBlock2Ptr->optionsButtonMode = sOptionMenuPtr->option[MENUITEM_BUTTONMODE];
    // gSaveBlock2Ptr->optionsWindowFrameType = sOptionMenuPtr->option[MENUITEM_FRAMETYPE];
    // SetPokemonCryStereo(gSaveBlock2Ptr->optionsSound);
    // FREE_AND_SET_NULL(sMainMenuSettingsMenuPtr);
    DestroyTask(taskId);
    SetMainCallback2(CB2_InitMainMenu);
}

static void Task_MainMenuSettingsMenu(u8 taskId)
{
    switch (sMainMenuSettingsMenuPtr->loadState)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_MainMenuOptionMenu);
        sMainMenuSettingsMenuPtr->loadState++;
        break;
    case 1:
        if (gPaletteFade.active)
            return;
        sMainMenuSettingsMenuPtr->loadState++;
        break;
    case 2:
        switch (MainMenuOptionMenu_ProcessInput())
        {
        case 0:
            break;
        case 1:
            sMainMenuSettingsMenuPtr->loadState++;
            break;
        case 2:
            // LoadBgTiles(1, GetUserWindowGraphics(sMainMenuSettingsMenuPtr->option[MAINMENUOPTIONITEM_FRAMETYPE])->tiles, 0x120, 0x1AA);
            // LoadPalette(GetUserWindowGraphics(sMainMenuSettingsMenuPtr->option[MAINMENUOPTIONITEM_FRAMETYPE])->palette, BG_PLTT_ID(2), PLTT_SIZE_4BPP);
            // BufferMainMenuOptionMenuString(sMainMenuSettingsMenuPtr->cursorPos);
            break;
        case 3:
            UpdateMainMenuOptionSettingSelectionDisplay(sMainMenuSettingsMenuPtr->cursorPos);
            break;
        case 4:
            BufferMainMenuOptionMenuString(sMainMenuSettingsMenuPtr->cursorPos);
            break;
        }
        break;
    case 3:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
        sMainMenuSettingsMenuPtr->loadState++;
        break;
    case 4:
        if (gPaletteFade.active)
            return;
        sMainMenuSettingsMenuPtr->loadState++;
        break;
    case 5:
        CloseAndSaveOptionMenu(taskId);
        break;
    }
}

static void InitMainMenuSettingsMenuBg(void)
{
    void *dest = (void *)VRAM;
    DmaClearLarge16(3, dest, VRAM_SIZE, 0x1000);    
    DmaClear32(3, (void *)OAM, OAM_SIZE);
    DmaClear16(3, (void *)PLTT, PLTT_SIZE);    
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sMainMenuSettingsMenuBgTemplates, NELEMS(sMainMenuSettingsMenuBgTemplates));
    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    ChangeBgX(1, 0, 0);
    ChangeBgY(1, 0, 0);
    ChangeBgX(2, 0, 0);
    ChangeBgY(2, 0, 0);
    ChangeBgX(3, 0, 0);
    ChangeBgY(3, 0, 0);
    InitWindows(sMainMenuSettingsMenuWinTemplates);
    DeactivateAllTextPrinters();
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_EFFECT_LIGHTEN);
    SetGpuReg(REG_OFFSET_BLDY, BLDCNT_TGT1_BG1);
    SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
    SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_CLR);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
};

static void MainMenuOptionMenu_ResetSpriteData(void)
{
    ResetSpriteData();
    ResetPaletteFade();
    FreeAllSpritePalettes();
    // ResetTasks();
    ScanlineEffect_Stop();
}

static bool8 LoadMainMenuOptionMenuPalette(void)
{
    switch (sMainMenuSettingsMenuPtr->loadPaletteState)
    {
    case 0:
        LoadBgTiles(1, GetUserWindowGraphics(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1AA);
        break;
    case 1:
        LoadPalette(GetUserWindowGraphics(gSaveBlock2Ptr->optionsWindowFrameType)->palette, BG_PLTT_ID(2), PLTT_SIZE_4BPP);
        break;
    case 2:
        LoadPalette(sMainMenuSettingsMenuPalette, BG_PLTT_ID(1), sizeof(sMainMenuSettingsMenuPalette));
        LoadPalette(GetTextWindowPalette(2), BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        break;
    case 3:
        LoadStdWindowGfxOnBg(1, 0x1B3, BG_PLTT_ID(3));
        break;
    default:
        return TRUE;
    }
    sMainMenuSettingsMenuPtr->loadPaletteState++;
    return FALSE;
}

static void PrintMainMenuOptionMenuHeader(void)
{
    FillWindowPixelBuffer(0, PIXEL_FILL(1));
    AddTextPrinterParameterized(MAINMENUOPTIONWIN_TEXT_OPTION, FONT_NORMAL, gText_Option, 8, 1, TEXT_SKIP_DRAW, NULL);
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}


static void DrawMainMenuOptionMenuBg(void)
{
    u8 h;
    h = 2;
    
    FillBgTilemapBufferRect(1, 0x1B3, 1, 2, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1B4, 2, 2, 0x1B, 1, 3);
    FillBgTilemapBufferRect(1, 0x1B5, 0x1C, 2, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1B6, 1, 3, 1, h, 3);
    FillBgTilemapBufferRect(1, 0x1B8, 0x1C, 3, 1, h, 3);
    FillBgTilemapBufferRect(1, 0x1B9, 1, 5, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1BA, 2, 5, 0x1B, 1, 3);
    FillBgTilemapBufferRect(1, 0x1BB, 0x1C, 5, 1, 1, 3);
    FillBgTilemapBufferRect(1, 0x1AA, 1, 6, 1, 1, h);
    FillBgTilemapBufferRect(1, 0x1AB, 2, 6, 0x1A, 1, h);
    FillBgTilemapBufferRect(1, 0x1AC, 0x1C, 6, 1, 1, h);
    FillBgTilemapBufferRect(1, 0x1AD, 1, 7, 1, 0x10, h);
    FillBgTilemapBufferRect(1, 0x1AF, 0x1C, 7, 1, 0x10, h);
    FillBgTilemapBufferRect(1, 0x1B0, 1, 0x13, 1, 1, h);
    FillBgTilemapBufferRect(1, 0x1B1, 2, 0x13, 0x1A, 1, h);
    FillBgTilemapBufferRect(1, 0x1B2, 0x1C, 0x13, 1, 1, h);
    CopyBgTilemapBufferToVram(1);
}

static void LoadMainMenuOptionMenuItemNames(void)
{
    u8 i;
    
    FillWindowPixelBuffer(1, PIXEL_FILL(1));
    for (i = 0; i < MAINMENU_SETTINGS_COUNT; i++)
    {
        AddTextPrinterParameterized(MAINMENUOPTIONWIN_OPTIONS, FONT_NORMAL, sMainMenuSettingsMenuItemsNames[i], 8, (u8)((i * (GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT))) + 2) - i, TEXT_SKIP_DRAW, NULL);    
    }
}

static void MainMenuOptionMenu_PickSwitchCancel(void)
{
    s32 x;
    x = 0xE4 - GetStringWidth(FONT_SMALL, gText_PickSwitchCancel, 0);
    FillWindowPixelBuffer(2, PIXEL_FILL(15)); 
    AddTextPrinterParameterized3(2, FONT_SMALL, x, 0, sMainMenuOptionMenuPickSwitchCancelTextColor, 0, gText_PickSwitchCancel);
    PutWindowTilemap(2);
    CopyWindowToVram(2, COPYWIN_FULL);
}


static void Task_MainMenuSettingsInit(u8 taskId)
{
    u8 i, state;
    state = sMainMenuSettingsMenuPtr->state;
    switch (state)
    {
    case 0: //init screen blank callbacks
        SetVBlankCallback(NULL);
        SetHBlankCallback(NULL);
        break;
    case 1:
        InitMainMenuSettingsMenuBg();
        break;
    case 2:
        MainMenuOptionMenu_ResetSpriteData();
        break;
    case 3:
        if (LoadMainMenuOptionMenuPalette() != TRUE)
            return;
        break;
    case 4:
        PrintMainMenuOptionMenuHeader();
        break;
    case 5:
        DrawMainMenuOptionMenuBg();
        break;
    case 6:
        LoadMainMenuOptionMenuItemNames();
        break;
    case 7:
        for (i = 0; i < MAINMENU_SETTINGS_COUNT; i++)
            BufferMainMenuOptionMenuString(i);
        break;
    case 8:
        UpdateMainMenuOptionSettingSelectionDisplay(sMainMenuSettingsMenuPtr->cursorPos);
        break;
    case 9:
        MainMenuOptionMenu_PickSwitchCancel();
        break;
    default:
        DestroyTask(taskId);
        CB2_RunMainMenuSettings();
		break;
    }
    sMainMenuSettingsMenuPtr->state++;
}
