#define USE_SOS_LIBRARY 1
#define DOS32 1
#define WATCOM 1

#include <dos.h> //set get interrupt vector func _dos_getvect _dos_setvect

#include <stdio.h>
#include <conio.h> //inp outp

//#include "setup_hmi.h"

/*
extern "C"
{


#include ".\hmi\sos.h"
#include ".\hmi\sosm.h"
#include ".\hmi\sosez.h"


        // local data for digital driver
        extern _SOS_DIGI_DRIVER sDIGIDriver;               // digital driver structure
        extern HANDLE hDIGIDriver;                              // handle to digital driver
        extern HANDLE hDIGITimer;                               // handle to digital mixer

        //HANDLE hDIGITimer_My = -1;                               // handle to digital mixer

        extern W32 wDIGIDeviceID;
        extern W32 wMIDIDeviceID;
        extern W32 hDIGIDriver;
        //W32 hMIDIDriver;

        extern HANDLE G_timerHandle;

        //extern HANDLE G_timerHandle;
        */
        /*

        #include ".\hmi\sos.h"
        #include ".\hmi\sosm.h"
        #include ".\hmi\sosez.h"

                        extern W32 wDIGIDeviceID;
                        extern W32 wMIDIDeviceID;
                        extern W32 hDIGIDriver;
                        extern W32 hMIDIDriver;

                        */
/*
}

*/




#include "winmain.h"
#include "vars.h"
#include "timer.h"
#include "backbuffer.h"
#include "draw.h"
#include "gameflow.h"
#include "init.h"
#include "inv.h"
#include "setup.h"
#include "text.h"
#include "inv.h"
#include "game.h"
#include "screen.h"
#include "input.h"
#include "sound.h"
#include "savegame.h"
#include "cinema.h"
#include "demo.h"
#include <malloc.h>

//--------------------------------

/*
#include ".\hmi\sos.h"
#include ".\hmi\sosm.h"
#include ".\hmi\sosez.h"
*/





//***************************
// SETUP START
//***************************

// SETUP PART #1 FULLSCREEN/NO
int Fullscreen = 0;

//---------------------------
// SETUP PART #2 WIDESCREEN/NO
// учет аспекта в matrix.cpp phd_GenerateW2V()
int Widescreen = 1;

//---------------------------
// SETUP PART #3 GAME TYPE - TR1/GOLD
int GameType = VER_TR1;
//int GameType = VER_TR1_GOLD;

//---------------------------
// SETUP PART #5 SCREEN RESOLUTION

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 480;

//---------------------------
// SETUP PART #4 LARA DIST
// false is original TR1 Lara dist
//влияет на g_PhdPersp
//int lara_dist = false;
int lara_dist = true;

// SETUP PART #5 CHEATS MODE
// full ammo/medi
int CheatsMode = 1;

//***************************
// SETUP END
//***************************


/*

TOMB ORIGINAL LEVELS LIST
Level 0 - GYM - GYM.PHD
Level 1 - Caves - LEVEL1.PHD
Level 2 - City of Vilcabamba - LEVEL2.PHD
Level 3 - Lost Valley - LEVEL3A.PHD
Level 4 - Tomb of Qualopec - LEVEL3B.PHD
Level 5 - St Francis' Folly - LEVEL4.PHD
Level 6 - Colosseum - LEVEL5.PHD
Level 7 - Palace Midas - LEVEL6.PHD
Level 8 - The Cistern - LEVEL7A.PHD
Level 9 - Tomb of Tihocan - LEVEL7B.PHD
Level 10 - City of Khamoon - LEVEL8A.PHD
Level 11 - Obelisk of Khamoon - LEVEL8B.PHD
Level 12 - Sanctuary of Scion - LEVEL8C.PHD
Level 13 - Natla's Mines - LEVEL10A.PHD
Level 14 - Atlantis - LEVEL10B.PHD
Level 15 - The Great Pyramid - LEVEL10C.PHD

TOMB GOLD LEVELS LIST
Level 0 - GYM - GYM.PHD
Level 1 - Return to Egypt - EGYPT.PHD
Level 2 - Temple of the Cat - CAT.PHD
Level 3 - Atlantean Stronghold - END.PHD
Level 4 - Hive - END2.PHD

*/


//int g_bWindowClosed = false;
//int g_bFocus = true;

int32_t g_RandControl = 0xD371F947;
int32_t g_RandDraw = 0xD371F947;

int Settings_Write()
{
        char KeyBuff[13] = {0};
        char MusicVolume = g_Config.music_volume;
        char SoundVolume = g_Config.sound_volume;

        for (int i = 0; i < INPUT_KEY_NUMBER_OF; i++)
        {
                S_INPUT_KEYCODE Key = S_Input_GetAssignedKeyCode(INPUT_LAYOUT_USER, (INPUT_KEY)i);

                KeyBuff[i] = Key;
        }

        FILE *Fp;

        Fp = fopen("settings.dat", "wb");

        fwrite(KeyBuff, sizeof(char) * 13, 1, Fp);

        fwrite(&MusicVolume, sizeof(char), 1, Fp);
        fwrite(&SoundVolume, sizeof(char), 1, Fp);

        fclose(Fp);

        return 1;
}

int Settings_Read()
{
        FILE *Fp;

        Fp = fopen("settings.dat", "rb");

        char KeyBuff;
        char MusicVolume;
        char SoundVolume;

        for (int i = 0; i < INPUT_KEY_NUMBER_OF; i++)
        {
                fread(&KeyBuff, sizeof(char), 1, Fp);

                S_Input_AssignKeyCode(INPUT_LAYOUT_USER, (INPUT_KEY)i, KeyBuff);
        }

        fread(&MusicVolume, sizeof(char), 1, Fp);
        g_Config.music_volume = MusicVolume;

        fread(&SoundVolume, sizeof(char), 1, Fp);
        g_Config.sound_volume = SoundVolume;
        Sound_SetMasterVolume(g_Config.sound_volume);

        fclose(Fp);

        return 1;
}

void S_SeedRandom()
{
        time_t lt = time(0);
        struct tm *tptr = localtime(&lt);
        Random_SeedControl(tptr->tm_sec + 57 * tptr->tm_min + 3543 * tptr->tm_hour);
        Random_SeedDraw(tptr->tm_sec + 43 * tptr->tm_min + 3477 * tptr->tm_hour);
}

void Random_SeedDraw(int32_t seed)
{ 
        g_RandDraw = seed;
}

int32_t Random_GetDraw()
{
        g_RandDraw = 0x41C64E6D * g_RandDraw + 0x3039;
        return (g_RandDraw >> 10) & 0x7FFF;
}

void Random_SeedControl(int32_t seed)
{ 
        g_RandControl = seed;
}

int32_t Random_GetControl()
{
        g_RandControl = 0x41C64E6D * g_RandControl + 0x3039;
        return (g_RandControl >> 10) & 0x7FFF;
}

void phd_InitWindow(int x, int y, int width, int height, int nearz, int farz,
                                        int view_angle, int scrwidth, int scrheight,
                                        unsigned char *BackBuff)
{

        // float values screen width/height
        g_SurfaceMinX = 0;
        g_SurfaceMinY = 0;
        g_SurfaceMaxX = (float)Screen_GetResWidth() - 1;
        g_SurfaceMaxY = (float)Screen_GetResHeight() - 1;

        // int values screen width/height
        g_PhdWinxmin = 0;
        g_PhdWinymin = 0;
        g_PhdWinxmax = Screen_GetResWidth() - 1;  // Maximum Window X coord
        g_PhdWinymax = Screen_GetResHeight() - 1; // Maximum Window Y coord
        g_PhdScrwidth = Screen_GetResWidth();
        g_PhdWinwidth = Screen_GetResWidth();

        
        //int16_t c = phd_cos(fov / 2);
        //int16_t s = phd_sin(fov / 2);
        //g_PhdPersp = ((SCREEN_WIDTH / 2) * c) / s;
        
}

int LoadTitle()
{
        
        
        Text_Init();
                
        // Output_DisplayPicture(g_GameFlow.main_menu_background_path);
        g_LevelNumTR = g_GameFlow.title_level_num;
        Initialise_Level(g_GameFlow.title_level_num); // title.phd level #20
        //палитру создаем только после загрузки уровня
        Create_Normal_Palette();

        int ret = Display_Inventory(INV_TITLE_MODE);
                
        if (ret == GF_START_DEMO)
                return GF_START_DEMO;

        if (ret == GF_EXIT_GAME)
                return GF_EXIT_GAME;

        switch (g_InvChosen)
        {
        case O_PHOTO_OPTION:
                // g_InvExtraData[1] = 0;
                return GF_START_GAME | g_GameFlow.gym_level_num;

        case O_PASSPORT_OPTION:

                // g_InvExtraData[0] - это номер страницы паспорта
                // g_InvExtraData[1] - это номер уровня который будет загружаться

                // страница 0: load game
                if (g_InvExtraData[0] == 0)
                {
                        S_LoadGame(&g_SaveGame, g_InvExtraData[1]);
                        g_LevelNumTR = 21; // 21 значит saved level
                        return GF_START_GAME | g_InvExtraData[1];
                }
                else if (g_InvExtraData[0] == 1)
                {
                        // страница 1: new game
                        InitialiseStartInfo();
                        g_LevelNumTR = g_GameFlow.first_level_num;
                        return GF_START_GAME | g_GameFlow.first_level_num;
                }
                else
                {
                        // страница 3: exit game
                        return GF_EXIT_GAME;
                }
                break;

        default:
                break;
        }

        return 0;
}








//0x000–0x0FF > состояние клавиш (256 сканкодов)
//0x100–0x13F > буфер нажатий (64 байта)
//0x140       > head (позиция записи)
//0x142       > размер буфера
//0x143       > состояние extended-префикса
//0x144       > последний сканкод
//0x145       > количество зажатых клавиш

//cseg01:0004A84C     Keyboard_Proc   proc near Tomb Raider 1

void (_interrupt _far* Old_Key_Isr)(); // holds old keyboard interrupt handler

void _interrupt _far New_Key_Int(void);


void  _interrupt _far New_Key_Int()
{
                static int ch = 0;
        int key_scan_code;
        
        key_scan_code = inp(0x60);

        int key_stat = Keys_States_Array[0x143];
           
        if (key_stat != 0)
        {
                goto extended_mode;
        }

        key_stat++;

        if (key_scan_code == 0xE0)
        {
                //key_stat для 0xE0 равно 1
                goto set_extended;
        }

        key_stat++;

        if (key_scan_code == 0xE1)
        {
                //key_stat для 0xE0 равно 2
                goto set_extended;
        }

        if (key_scan_code & 0x80)
        {
                goto key_release;
        }


                key_stat = Keys_States_Array[key_scan_code];


        if (key_stat == 0)
        {
                Keys_States_Array[0x145]++; // counter
                Keys_States_Array[key_scan_code] = 1;
                                


        }

                key_stat = 0;

        goto finish;

key_release:

        key_scan_code &= 0x7F;

                key_stat = Keys_States_Array[key_scan_code];

        if (key_stat != 0)
        {
                Keys_States_Array[0x145]--;
                Keys_States_Array[key_scan_code] = 0;
        }

                key_stat = 0;

        goto finish;


extended_mode:

        key_stat--;
                
        if (key_stat == 0)
        {
                if (key_scan_code & 0x80 == 0)
                {
                        key_scan_code |= 0x80;

                                                key_stat |= Keys_States_Array[key_scan_code];

                                if (key_stat == 0)
                                {
                                                                                key_stat++;
                                        Keys_States_Array[0x145]++;
                                        Keys_States_Array[key_scan_code] = key_stat;

                                }
                }
                else
                {

                                        key_stat |= Keys_States_Array[key_scan_code];

                                if (key_stat == 0)
                                {
                                                                        goto set_extended;
                                }

                        Keys_States_Array[0x145]--;
                        Keys_States_Array[key_scan_code] = 0;
                }

                goto zero_state;
        }
                

        key_stat += 2;

        if (key_stat == 3)
        {
                goto set_extended;
        }

        ch++;

        if (key_scan_code != 0x45)
        {
                goto zero_state;
        }

                key_stat = Keys_States_Array[0x0C5];
                
                if (key_stat == 0)
                {
                                                key_stat++;
                        Keys_States_Array[0x145]++;
                        Keys_States_Array[0x0C5] = key_stat;
                }
                else
                {
                        Keys_States_Array[0x145]--;
                        Keys_States_Array[0x0C5] = 0;
                }
                                
zero_state:

                                key_stat = 0;

set_extended:

                Keys_States_Array[0x143] = key_stat;


finish:
                                /*
        if (ch | key_state == 0 && (key_scan_code & 0x80) == 0 && key_scan_code != 0xAA)
        {
                uint8_t idx = Keys_States_Array[0x142];

                if (idx != 0x40)
                {
                        idx++;

                                                key_state = key_scan_code;

                        uint8_t head = Keys_States_Array[0x140];

                        Keys_States_Array[0x144] = key_state;
                        Keys_States_Array[0x142] = idx;

                        Keys_States_Array[head + 0x100] = key_state;

                        head++;
                        head &= 0x3F;

                        Keys_States_Array[0x140] = head;
                }
        }
                */
                
        // EOI (end of interrupt)
                outp(0x20, 0x20);

}




int main(void)
{
        Sound_Init();

        Old_Key_Isr = _dos_getvect(KEYBOARD_INT);

        _dos_setvect(KEYBOARD_INT, New_Key_Int);

        //новое для ТР1
        Init_GameFlow();

        // GS_CONTROL_DEFAULT_KEYS
        g_Config.input.layout = INPUT_LAYOUT_DEFAULT;
        g_GameFlow.gym_level_num = 0;
        g_GameFlow.first_level_num = 1;
  
        if (GameType == VER_TR1)
        {
                g_GameFlow.title_level_num = 20;
                g_GameFlow.level_count = 22;
                g_GameFlow.last_level_num = 15;
        }
        else if (GameType == VER_TR1_GOLD)
        {
                g_GameFlow.title_level_num = 5;
                g_GameFlow.level_count = 4;
                g_GameFlow.last_level_num = 4;
        }

        g_Config.render_flags.fps_counter = 1;
        g_Config.enable_enemy_healthbar = 1;
        g_Config.enemy_healthbar_location = T1M_BL_BOTTOM_LEFT;
        g_Config.enemy_healthbar_color = 2;
        g_Config.ui.bar_scale = 1.0f;
        g_Config.ui.text_scale = 1.0f;
        g_Config.healthbar_location = T1M_BL_TOP_LEFT;
        g_Config.healthbar_color = 0;
        g_Config.airbar_location = T1M_BL_TOP_RIGHT;
        g_Config.airbar_color = 1;

        S_SeedRandom();

        Output_CalcWibbleTable();


        //параметры ниже widht, height умножаются на screen_sizer
        // x, y, width, height нужны для уменьшения увеличения окна игры
        // scrwidth, scrheight - дисплей размеры
        //параметры phd_InitWindow следующие x, y, width, height,
        // nearz, farz, view_angle, scrwidth, scrheight, backbuffer_memory
        // 20480 = 0x5000
        unsigned char *pBackBuff = 0;
        phd_InitWindow(0, 0, Screen_GetResWidth(), Screen_GetResHeight(), 10, 20480,
                                   80, Screen_GetResWidth(), Screen_GetResHeight(), pBackBuff);

        if (g_GameMemory)
                free(g_GameMemory);

        g_GameMemory = (int8_t *)malloc(g_Malloc_Size);

        if (!g_GameMemory)
        {
                printf("ERROR: Could not allocate enough memory");
        }

        Init_Game_Malloc();
        S_FrontEndCheck();
        Settings_Read();
        Create_BackBuffer();
        InitialiseStartInfo();

        int intro_played = false;
        int loop_continue = true;

                int32_t gf_option = LoadTitle();

        //int32_t gf_option = 3;
                
        while (loop_continue)
        {
                int32_t gf_direction = gf_option & ~((1 << 6) - 1);
                int32_t gf_param = gf_option & ((1 << 6) - 1);

                switch (gf_direction)
                {

                case GF_EXIT_TO_TITLE:
                        gf_option = LoadTitle();
                        break;

                case GF_START_GAME:
                        gf_option = Start_New_Game(gf_param);
                        break;

                case GF_START_CINE:
                        // gf_option = GameFlow_InterpretSequence(gf_param, GFL_CUTSCENE);

                        gf_option = Play_Cinematic(gf_param);

                        break;

                case GF_START_DEMO:
                        gf_option = StartDemo();
                        break;

                case GF_LEVEL_COMPLETE:
                        
                        //gf_option = Print_Final_Stats(g_CurrentLevel);
                        gf_option = Print_Level_Stats(gf_param);
                        g_LevelNumTR = gf_param;

                        break;

                case GF_EXIT_GAME:
                        loop_continue = false;
                        break;

                default:
                        break;
                }
        }

                _dos_setvect(KEYBOARD_INT, Old_Key_Isr);

        free(g_GameMemory);
        Delete_BackBuffer();

                Sound_UnInit();

        return 0;
}
