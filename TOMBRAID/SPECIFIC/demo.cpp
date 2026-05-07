#include "demo.h"

//#include "config.h"
//#include "control.h"
#include "game.h"
#include "gameflow.h"
#include "input.h"
#include "items.h"
//#include "output.h"
//#include "random.h"
#include "setup.h"
#include "text.h"
#include "const.h"
#include "types.h"
#include "vars.h"
#include "ctr_util.h"

//#include <stdbool.h>

static int32_t m_DemoLevel = -1;
static uint32_t *m_DemoPtr = NULL;

int32_t StartDemo()
{
    TEXTSTRING *txt;
    START_INFO start, *s;

    bool any_demos = false;
    for (int32_t i = g_GameFlow.first_level_num; i < g_GameFlow.last_level_num; i++)
        {
        if (g_GameFlow.levels[i].demo)
                {
            any_demos = true;
        }
    }
    if (!any_demos)
        {
        return GF_EXIT_TO_TITLE;
    }

    int16_t level_num = m_DemoLevel;
    do {
        level_num++;
        if (level_num > g_GameFlow.last_level_num)
                {
            level_num = g_GameFlow.first_level_num;
        }
    } while (!g_GameFlow.levels[level_num].demo);

    m_DemoLevel = level_num;

    s = &g_SaveGame.start[m_DemoLevel];
    start = *s;
    s->available = 1;
    s->got_pistols = 1;
    s->pistol_ammo = 1000;
    s->gun_status = LGS_ARMLESS;
    s->gun_type = LGT_PISTOLS;

	//g_LevelNumTR == 2;

    Random_SeedDraw(0xD371F947);
    Random_SeedControl(0xD371F947);

    // changing the controls affects negatively the original game demo data,
    // so temporarily turn off all the T1M enhancements
    //int8_t old_enhanced_look = g_Config.enable_enhanced_look;
    //g_Config.enable_enhanced_look = 0;

    //if (Initialise_Level(m_DemoLevel, GFL_DEMO))
        if (Initialise_Level(m_DemoLevel))
        {
        LoadLaraDemoPos();

        Random_SeedDraw(0xD371F947);
        Random_SeedControl(0xD371F947);

        txt = Text_Create(0, -16, g_GameFlow.strings[GS_MISC_DEMO_MODE]);
        Text_Flash(txt, 1, 20);
        Text_AlignBottom(txt, 1);
        Text_CentreH(txt, 1);

        Game_Loop(1);

        Text_Remove(txt);

        *s = start;
        //Output_FadeToBlack();
    }

    //g_Config.enable_enhanced_look = old_enhanced_look;

    return GF_EXIT_TO_TITLE;
}

void LoadLaraDemoPos()
{
    m_DemoPtr = g_DemoData;
    ITEM_INFO *item = g_LaraItem;
    item->pos.x = *m_DemoPtr++;
    item->pos.y = *m_DemoPtr++;
    item->pos.z = *m_DemoPtr++;
    item->pos.x_rot = *m_DemoPtr++;
    item->pos.y_rot = *m_DemoPtr++;
    item->pos.z_rot = *m_DemoPtr++;
    int16_t room_num = *m_DemoPtr++;

    if (item->room_number != room_num)
        {
        ItemNewRoom(g_Lara.item_number, room_num);
    }

    FLOOR_INFO *floor = GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_num);
    item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
}

bool ProcessDemoInput()
{
    if (m_DemoPtr >= &g_DemoData[DEMO_COUNT_MAX] || (int32_t)*m_DemoPtr == -1)
        {
        return false;
    }

    // Translate demo inputs (that use TombATI key values) to T1M inputs.
        

	g_Input = 0;

	g_Input = *m_DemoPtr;

	/*

	if((*m_DemoPtr & IN_FORWARD) != 0)
		g_Input |= IN_FORWARD;

	if((*m_DemoPtr & IN_BACK) != 0)
		g_Input |= IN_BACK;

	if((*m_DemoPtr & IN_LEFT) != 0)
		g_Input |= IN_LEFT;

	if((*m_DemoPtr & IN_RIGHT) != 0)
		g_Input |= IN_RIGHT;

	if((*m_DemoPtr & IN_JUMP) != 0)
		g_Input |= IN_JUMP;

	if((*m_DemoPtr & IN_DRAW) != 0)
		g_Input |= IN_DRAW;

	if((*m_DemoPtr & IN_ACTION) != 0)
		g_Input |= IN_ACTION;
	
	if ((*m_DemoPtr & IN_ROLL) != 0)
		g_Input |= IN_ROLL;
	*/
    
    m_DemoPtr++;
    return true;
}
