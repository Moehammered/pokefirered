#include "main_menu_option.h"
#include "global.h"
#include "gflib.h"
#include "gpu_regs.h"
#include "main.h"
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

void AddTextPrinterToWindow_MainMenuOption(const u8 *str);
void MainCB_MainMenuOption_ReturnToTitleScreen(void);
void PrintMainMenuOptionTopMenu(void);
void CB2_InitMainMenuOption(void);
void CB2_RunMainMenuOption(void);
static void Task_MainMenuOptionMenu(u8 taskId);
void MainMenuOption_DrawTextBorder(u8 windowId);
static void InitMainMenuOptionMenuBg(void);
static void Task_MainMenuOptionInit(u8 taskId);

enum
{
    MAINMENUOPTIONITEM_TEXTSPEED = 0,
    MAINMENUOPTIONITEM_BATTLESCENE,
    MAINMENUOPTIONITEM_BATTLESTYLE,
    MAINMENUOPTIONITEM_SOUND,
    MAINMENUOPTIONITEM_BUTTONMODE,
    MAINMENUOPTIONITEM_FRAMETYPE,
    MAINMENUOPTIONITEM_CANCEL,
    MAINMENUOPTIONITEM_COUNT
};

// Window Ids
enum
{
    MAINMENUOPTIONWIN_TEXT_OPTION,
    MAINMENUOPTIONWIN_OPTIONS
};

struct MainMenuOptionMenu
{
    /*0x00*/ u16 option[MAINMENUOPTIONITEM_COUNT];
    /*0x0E*/ u16 cursorPos;
    /*0x10*/ u8 loadState;
    /*0x11*/ u8 state;
    /*0x12*/ u8 loadPaletteState;
};

static struct MainMenuOptionMenu *sMainMenuOptionMenuPtr = NULL;


// Data Definitions
static const struct WindowTemplate sMainMenuOptionMenuWinTemplates[] =
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

static const struct BgTemplate sMainMenuOptionMenuBgTemplates[] =
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

static const u16 sMainMenuOptionMenuPalette[] = INCBIN_U16("graphics/misc/option_menu.gbapal");
static const u16 sMainMenuOptionMenuItemCounts[MAINMENUOPTIONITEM_COUNT] = {3, 2, 2, 2, 3, 10, 0};

static const u8 *const sMainMenuOptionMenuItemsNames[MAINMENUOPTIONITEM_COUNT] =
{
    [MAINMENUOPTIONITEM_TEXTSPEED]   = gText_TextSpeed,
    [MAINMENUOPTIONITEM_BATTLESCENE] = gText_BattleScene,
    [MAINMENUOPTIONITEM_BATTLESTYLE] = gText_BattleStyle,
    [MAINMENUOPTIONITEM_SOUND]       = gText_Sound,
    [MAINMENUOPTIONITEM_BUTTONMODE]  = gText_ButtonMode,
    [MAINMENUOPTIONITEM_FRAMETYPE]   = gText_Frame,
    [MAINMENUOPTIONITEM_CANCEL]      = gText_OptionMenuCancel,
};

static const u8 *const sTextSpeedOptions[] =
{
    gText_TextSpeedSlow, 
    gText_TextSpeedMid, 
    gText_TextSpeedFast
};

static const u8 *const sBattleSceneOptions[] =
{
    gText_BattleSceneOn, 
    gText_BattleSceneOff
};

static const u8 *const sBattleStyleOptions[] =
{
    gText_BattleStyleShift,
    gText_BattleStyleSet
};

static const u8 *const sSoundOptions[] =
{
    gText_SoundMono, 
    gText_SoundStereo
};

static const u8 *const sButtonTypeOptions[] =
{
	gText_ButtonTypeLR,
	gText_ButtonTypeLEqualsA
};

static const u8 sMainMenuOptionMenuPickSwitchCancelTextColor[] = {TEXT_DYNAMIC_COLOR_6, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
static const u8 sMainMenuOptionMenuTextColor[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED};


void CreateMainMenuOptionTask()
{
    u8 taskId = CreateTask(Task_MainMenuOptionInit, 0);
    struct MainMenuOptionMenu * data = (void *)gTasks[taskId].data;
    memset(data, 0, sizeof(struct MainMenuOptionMenu));
    sMainMenuOptionMenuPtr = data;
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

void AddTextPrinterToWindow_MainMenuOption(const u8 *str)
{

}

void MainCB_MainMenuOption_ReturnToTitleScreen(void)
{

}

void PrintMainMenuOptionTopMenu(void)
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

void CB2_InitMainMenuOption(void)
{
    CreateMainMenuOptionTask(); //kick off the main task to run here
    SetMainCallback2(CB2_MainMenuOption_InitTaskSystem); //set callback to setup task system
}

void CB2_RunMainMenuOption(void)
{
    CreateTask(Task_MainMenuOptionMenu, 0);
    // CreateMainMenuOptionTask(); //kick off the main task to run here
    // SetMainCallback2(CB2_MainMenuOption_InitTaskSystem); //set callback to setup task system
}

void MainMenuOption_DrawTextBorder(u8 windowId)
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
        current = sMainMenuOptionMenuPtr->option[(sMainMenuOptionMenuPtr->cursorPos)];
        if (current == (sMainMenuOptionMenuItemCounts[sMainMenuOptionMenuPtr->cursorPos] - 1))
            sMainMenuOptionMenuPtr->option[sMainMenuOptionMenuPtr->cursorPos] = 0;
        else
            sMainMenuOptionMenuPtr->option[sMainMenuOptionMenuPtr->cursorPos] = current + 1;
        if (sMainMenuOptionMenuPtr->cursorPos == MAINMENUOPTIONITEM_FRAMETYPE)
            return 2;
        else
            return 4;
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        curr = &sMainMenuOptionMenuPtr->option[sMainMenuOptionMenuPtr->cursorPos];
        if (*curr == 0)
            *curr = sMainMenuOptionMenuItemCounts[sMainMenuOptionMenuPtr->cursorPos] - 1;
        else
            --*curr;
        
        if (sMainMenuOptionMenuPtr->cursorPos == MAINMENUOPTIONITEM_FRAMETYPE)
            return 2;
        else
            return 4;
    }
    else if (JOY_REPT(DPAD_UP))
    {
        if (sMainMenuOptionMenuPtr->cursorPos == MAINMENUOPTIONITEM_TEXTSPEED)
            sMainMenuOptionMenuPtr->cursorPos = MAINMENUOPTIONITEM_CANCEL;
        else
            sMainMenuOptionMenuPtr->cursorPos = sMainMenuOptionMenuPtr->cursorPos - 1;
        return 3;        
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (sMainMenuOptionMenuPtr->cursorPos == MAINMENUOPTIONITEM_CANCEL)
            sMainMenuOptionMenuPtr->cursorPos = MAINMENUOPTIONITEM_TEXTSPEED;
        else
            sMainMenuOptionMenuPtr->cursorPos = sMainMenuOptionMenuPtr->cursorPos + 1;
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
    case MAINMENUOPTIONITEM_TEXTSPEED:
        AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sTextSpeedOptions[sMainMenuOptionMenuPtr->option[selection]]);
        break;
    case MAINMENUOPTIONITEM_BATTLESCENE:
        AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sBattleSceneOptions[sMainMenuOptionMenuPtr->option[selection]]);
        break;
    case MAINMENUOPTIONITEM_BATTLESTYLE:
        AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sBattleStyleOptions[sMainMenuOptionMenuPtr->option[selection]]);
        break;
    case MAINMENUOPTIONITEM_SOUND:
        AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sSoundOptions[sMainMenuOptionMenuPtr->option[selection]]);
        break;
    case MAINMENUOPTIONITEM_BUTTONMODE:
        AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, sButtonTypeOptions[sMainMenuOptionMenuPtr->option[selection]]);
        break;
    case MAINMENUOPTIONITEM_FRAMETYPE:
        StringCopy(str, gText_FrameType);
        ConvertIntToDecimalStringN(buf, sMainMenuOptionMenuPtr->option[selection] + 1, 1, 2);
        StringAppendN(str, buf, 3);
        AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, str);
        break;
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
    // gSaveBlock2Ptr->optionsTextSpeed = sOptionMenuPtr->option[MENUITEM_TEXTSPEED];
    // gSaveBlock2Ptr->optionsBattleSceneOff = sOptionMenuPtr->option[MENUITEM_BATTLESCENE];
    // gSaveBlock2Ptr->optionsBattleStyle = sOptionMenuPtr->option[MENUITEM_BATTLESTYLE];
    // gSaveBlock2Ptr->optionsSound = sOptionMenuPtr->option[MENUITEM_SOUND];
    // gSaveBlock2Ptr->optionsButtonMode = sOptionMenuPtr->option[MENUITEM_BUTTONMODE];
    // gSaveBlock2Ptr->optionsWindowFrameType = sOptionMenuPtr->option[MENUITEM_FRAMETYPE];
    // SetPokemonCryStereo(gSaveBlock2Ptr->optionsSound);
    // FREE_AND_SET_NULL(sMainMenuOptionMenuPtr);
    DestroyTask(taskId);
    SetMainCallback2(CB2_InitMainMenu);
}

static void Task_MainMenuOptionMenu(u8 taskId)
{
    switch (sMainMenuOptionMenuPtr->loadState)
    {
    case 0:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_MainMenuOptionMenu);
        sMainMenuOptionMenuPtr->loadState++;
        break;
    case 1:
        if (gPaletteFade.active)
            return;
        sMainMenuOptionMenuPtr->loadState++;
        break;
    case 2:
        switch (MainMenuOptionMenu_ProcessInput())
        {
        case 0:
            break;
        case 1:
            sMainMenuOptionMenuPtr->loadState++;
            break;
        case 2:
            LoadBgTiles(1, GetUserWindowGraphics(sMainMenuOptionMenuPtr->option[MAINMENUOPTIONITEM_FRAMETYPE])->tiles, 0x120, 0x1AA);
            LoadPalette(GetUserWindowGraphics(sMainMenuOptionMenuPtr->option[MAINMENUOPTIONITEM_FRAMETYPE])->palette, BG_PLTT_ID(2), PLTT_SIZE_4BPP);
            BufferMainMenuOptionMenuString(sMainMenuOptionMenuPtr->cursorPos);
            break;
        case 3:
            UpdateMainMenuOptionSettingSelectionDisplay(sMainMenuOptionMenuPtr->cursorPos);
            break;
        case 4:
            BufferMainMenuOptionMenuString(sMainMenuOptionMenuPtr->cursorPos);
            break;
        }
        break;
    case 3:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
        sMainMenuOptionMenuPtr->loadState++;
        break;
    case 4:
        if (gPaletteFade.active)
            return;
        sMainMenuOptionMenuPtr->loadState++;
        break;
    case 5:
        CloseAndSaveOptionMenu(taskId);
        break;
    }
}

static void InitMainMenuOptionMenuBg(void)
{
    void *dest = (void *)VRAM;
    DmaClearLarge16(3, dest, VRAM_SIZE, 0x1000);    
    DmaClear32(3, (void *)OAM, OAM_SIZE);
    DmaClear16(3, (void *)PLTT, PLTT_SIZE);    
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sMainMenuOptionMenuBgTemplates, NELEMS(sMainMenuOptionMenuBgTemplates));
    ChangeBgX(0, 0, 0);
    ChangeBgY(0, 0, 0);
    ChangeBgX(1, 0, 0);
    ChangeBgY(1, 0, 0);
    ChangeBgX(2, 0, 0);
    ChangeBgY(2, 0, 0);
    ChangeBgX(3, 0, 0);
    ChangeBgY(3, 0, 0);
    InitWindows(sMainMenuOptionMenuWinTemplates);
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
    switch (sMainMenuOptionMenuPtr->loadPaletteState)
    {
    case 0:
        LoadBgTiles(1, GetUserWindowGraphics(sMainMenuOptionMenuPtr->option[MAINMENUOPTIONITEM_FRAMETYPE])->tiles, 0x120, 0x1AA);
        break;
    case 1:
        LoadPalette(GetUserWindowGraphics(sMainMenuOptionMenuPtr->option[MAINMENUOPTIONITEM_FRAMETYPE])->palette, BG_PLTT_ID(2), PLTT_SIZE_4BPP);
        break;
    case 2:
        LoadPalette(sMainMenuOptionMenuPalette, BG_PLTT_ID(1), sizeof(sMainMenuOptionMenuPalette));
        LoadPalette(GetTextWindowPalette(2), BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        break;
    case 3:
        LoadStdWindowGfxOnBg(1, 0x1B3, BG_PLTT_ID(3));
        break;
    default:
        return TRUE;
    }
    sMainMenuOptionMenuPtr->loadPaletteState++;
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
    for (i = 0; i < MAINMENUOPTIONITEM_COUNT; i++)
    {
        AddTextPrinterParameterized(MAINMENUOPTIONWIN_OPTIONS, FONT_NORMAL, sMainMenuOptionMenuItemsNames[i], 8, (u8)((i * (GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT))) + 2) - i, TEXT_SKIP_DRAW, NULL);    
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


static void Task_MainMenuOptionInit(u8 taskId)
{
    u8 i, state;
    state = sMainMenuOptionMenuPtr->state;
    switch (state)
    {
    case 0: //init screen blank callbacks
        SetVBlankCallback(NULL);
        SetHBlankCallback(NULL);
        break;
    case 1:
        InitMainMenuOptionMenuBg();
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
        for (i = 0; i < MAINMENUOPTIONITEM_COUNT; i++)
            BufferMainMenuOptionMenuString(i);
        break;
    case 8:
        UpdateMainMenuOptionSettingSelectionDisplay(sMainMenuOptionMenuPtr->cursorPos);
        break;
    case 9:
        MainMenuOptionMenu_PickSwitchCancel();
        break;
    default:
        DestroyTask(taskId);
        CB2_RunMainMenuOption();
		break;
    }
    sMainMenuOptionMenuPtr->state++;
}
