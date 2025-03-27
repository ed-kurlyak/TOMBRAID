#include <math.h>
#include "setup.h"
#include "vars.h"
#include "3d_gen.h"
#include "file.h"
#include "draw.h"
#include "lara.h"
#include "items.h"
#include "winmain.h"
#include "overlay.h"
#include "text.h"
#include "savegame.h"

int Initialise_Level_Flags()
{
	int result = 0;

	g_SaveGame.secrets = 0;
    g_SaveGame.pickups = 0;
	g_SaveGame.timer = 0;
    g_SaveGame.kills = 0;

	return result;
}

// Функция для вычисления индекса ближайшего цвета
int Compose_Colour(int inputR, int inputG, int inputB)
{
    int closestIndex = 0; // Изначально считаем ближайшим первый элемент
    int minDistance = INT_MAX; // Минимальная дистанция (изначально бесконечность)

    for (int i = 0; i < 256; ++i)
    {
        // Разница между входными цветами и цветами из палитры
        float redDiff = (float) (inputR - GamePalette[i].r);
        float greenDiff = (float)(inputG - GamePalette[i].g);
        float blueDiff = (float) (inputB - GamePalette[i].b);

        // Вычисление квадратичного расстояния
        int distance = (int)(pow(redDiff, 2) + pow(greenDiff, 2) + pow(blueDiff, 2));

        // Проверка на минимальное расстояние
        if (distance < minDistance)
        {
            minDistance = distance;
            closestIndex = i;
        }
    }

    return closestIndex; // Возвращаем индекс ближайшего цвета
}


void Init_Colours()
{

	//синяя молния Тора в комнате Тора уровень Монастырь Св.Франциска
	color_tor_lighting = Compose_Colour(0, 0, 255);
	//белая линия на молнии Тора в комнате Тора уровень Монастырь Св.Франциска
	color_tor_lighting2 = Compose_Colour(255, 255, 255);

	//цвет воды TR1 original - pal = 43
	//int val1 = Compose_Colour(153, 178, 255);

	//цвет воды TR1 original - pal = 43
	//int val2 = Compose_Colour(114, 255, 255);

	int debug = 0;

	
}

int Initialise_Level(int LevelNum)
{
	//if(LevelNum == 0x15) //21d
    
    if (g_level_num_TR1 == 21)
	{
		LevelNum = g_SaveGame.current_level;
	}

	g_CurrentLevel = LevelNum;

	Text_RemoveAll();
	Initialise_Game_Flags();

	g_Lara.item_number = NO_ITEM;
	
	if (!Load_Level(g_CurrentLevel))
	{
        return 0;
    }

	if (g_Lara.item_number != NO_ITEM)
	{
        InitialiseLara();
    }



	g_Effects = (FX_INFO *)Game_Alloc(NUM_EFFECTS * sizeof(FX_INFO), GBUF_EFFECTS);
    InitialiseFXArray();
    InitialiseLOTArray();

	Overlay_Init();

	Init_Colours();

    g_HealthBarTimer = 100;
    //Sound_ResetEffects();

    
    if (g_level_num_TR1 == 21)
    {
        ExtractSaveGameInfo();
    }
	
	phd_AlterFOV(80 * PHD_DEGREE);

	/*
	
	if (g_GameFlow.levels[g_CurrentLevel].music) {
        Music_PlayLooped(g_GameFlow.levels[g_CurrentLevel].music);
    }

	*/
	g_Camera.underwater = 0;


	return (1);
}

void Initialise_Game_Flags()
{
	for (int i = 0; i < MAX_FLIP_MAPS; i++) {
        g_FlipMapTable[i] = 0;
    }

    for (int i = 0; i < MAX_CD_TRACKS; i++) {
        g_MusicTrackFlags[i] = 0;
    }

    for (int i = 0; i < O_NUMBER_OF; i++) {
        g_Objects[i].loaded = 0;
    }

	g_FlipStatus = 0;
    g_LevelComplete = false;
    g_FlipEffect = -1;
    //g_PierreItemNum = NO_ITEM;
}

bool Load_Level(int32_t LevelNum)
{

	char *szLevelName;

	if(select_game == VER_TR1)
	{
		szLevelName = LevelNamesTR1[LevelNum];
	}
	else if(select_game == VER_TR_GOLD)
	{
		szLevelName = LevelNamesGold[LevelNum];
	}
	else
		return false;

	bool result = S_LoadLevel(szLevelName);


	return result;
}

/*
void InitialiseLara()
{
    g_LaraItem->collidable = 0;
    g_LaraItem->data = &g_Lara;
    g_LaraItem->hit_points = LARA_HITPOINTS;

	g_Lara.air = LARA_AIR;
    g_Lara.torso_y_rot = 0;
    g_Lara.torso_x_rot = 0;
    g_Lara.torso_z_rot = 0;
    g_Lara.head_y_rot = 0;
    g_Lara.head_x_rot = 0;
    g_Lara.head_z_rot = 0;
    g_Lara.calc_fall_speed = 0;
    g_Lara.mesh_effects = 0;
    g_Lara.hit_frame = 0;
    g_Lara.hit_direction = 0;
    g_Lara.death_count = 0;
    g_Lara.target = NULL;
    g_Lara.spaz_effect = NULL;
    g_Lara.spaz_effect_count = 0;
    g_Lara.turn_rate = 0;
    g_Lara.move_angle = 0;
    g_Lara.right_arm.flash_gun = 0;
    g_Lara.left_arm.flash_gun = 0;
    g_Lara.right_arm.lock = 0;
    g_Lara.left_arm.lock = 0;


	if (g_RoomInfo[g_LaraItem->room_number].flags & 1)
	{
        g_Lara.water_status = LWS_UNDERWATER;
        g_LaraItem->fall_speed = 0;
        g_LaraItem->goal_anim_state = AS_TREAD;
        g_LaraItem->current_anim_state = AS_TREAD;
        g_LaraItem->anim_number = AA_TREAD;
        g_LaraItem->frame_number = AF_TREAD;
    }
	else
	{
        g_Lara.water_status = LWS_ABOVEWATER;
        g_LaraItem->goal_anim_state = AS_STOP;
        g_LaraItem->current_anim_state = AS_STOP;
        g_LaraItem->anim_number = AA_STOP;
        g_LaraItem->frame_number = AF_STOP;
    }

	g_Lara.current_active = 0;

    InitialiseLOT(&g_Lara.LOT);

    g_Lara.LOT.step = WALL_L * 20;
    g_Lara.LOT.drop = -WALL_L * 20;
    g_Lara.LOT.fly = STEP_L;

    InitialiseLaraInventory(g_CurrentLevel);
}

*/
/*
void InitialiseLaraInventory(int32_t level_num)
{
*/
	/*
	Inv_RemoveAllItems();

	START_INFO *start = &g_SaveGame.start[level_num];

	g_Lara.pistols.ammo = 1000;
    
	if (start->got_pistols)
	{
        Inv_AddItem(O_GUN_ITEM);
    }
	*/


	//LaraInitialiseMeshes(level_num);

	//InitialiseNewWeapon();
/*
}

*/


/*
int32_t InitialiseLOT(LOT_INFO *LOT)
{
    LOT->node = (BOX_NODE *) Game_Alloc(sizeof(BOX_NODE) * g_NumberBoxes, GBUF_CREATURE_LOT);
    ClearLOT(LOT);
    return 1;
}
*/
/*
void LaraInitialiseMeshes(int32_t level_num)
{
	for (int i = 0; i < LM_NUMBER_OF; i++) {
        g_Lara.mesh_ptrs[i] = g_Meshes[g_Objects[O_LARA].mesh_index + i];
    }

	g_Lara.mesh_ptrs[LM_THIGH_L] =
            g_Meshes[g_Objects[O_PISTOLS].mesh_index + LM_THIGH_L];
        g_Lara.mesh_ptrs[LM_THIGH_R] =
            g_Meshes[g_Objects[O_PISTOLS].mesh_index + LM_THIGH_R];



}

*/


