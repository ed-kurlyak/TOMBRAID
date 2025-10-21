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
#include "control_util.h"
#include "sound.h"
#include "backbuffer.h"
#include "..\objects\pierre.h"

int Initialise_Level_Flags()
{
	int result = 0;

	g_SaveGame.secrets = 0;
	g_SaveGame.pickups = 0;
	g_SaveGame.timer = 0;
	g_SaveGame.kills = 0;

	return result;
}

//������� ��� ���������� ������� ���������� ����� � �������
int Compose_Colour(int inputR, int inputG, int inputB)
{
	int closestIndex = 0; //���������� ������� ��������� ������ �������
	int minDistance =
		INT_MAX; //����������� ��������� (���������� �������������)

	for (int i = 0; i < 256; ++i)
	{
		//������� ����� �������� ������� � ������� �� �������
		float redDiff = (float)(inputR - GameNormalPalette[i].r);
		float greenDiff = (float)(inputG - GameNormalPalette[i].g);
		float blueDiff = (float)(inputB - GameNormalPalette[i].b);

		//���������� ������������� ����������
		int distance =
			(int)(pow(redDiff, 2) + pow(greenDiff, 2) + pow(blueDiff, 2));

		//�������� �� ����������� ����������
		if (distance < minDistance)
		{
			minDistance = distance;
			closestIndex = i;
		}
	}

	return closestIndex; //���������� ������ ���������� ����� � �������
}

void Init_Colours()
{
	//����� ������ ���� � ������� ���� ������� ��������� ��.���������
	//����� ������ ������� The Great Pyramid
	ColorLighting1 = Compose_Colour(0, 0, 255);
	//����� ����� �� ������ ���� � ������� ���� ������� ��������� ��.���������
	//����� ����� �� ������ ������� The Great Pyramid
	ColorLighting2 = Compose_Colour(255, 255, 255);
}

int Initialise_Level(int LevelNum)
{
	// 21d = 0x15 ���������� saved level
	if (g_LevelNumTR == 21)
	{
		LevelNum = g_SaveGame.current_level;
	}

	g_CurrentLevel = LevelNum;

	Initialise_Game_Flags();

	g_Lara.item_number = NO_ITEM;

	if (!Load_Level(g_CurrentLevel))
	{
		return 0;
	}

	// new for tr1
	//������� ������� ����� �������� ������
	//�.�. ������ � ������� �� ����� ������
	if (!Hardware)
		Create_Normal_Palette();

	if (g_Lara.item_number != NO_ITEM)
	{
		InitialiseLara();
	}

	g_Effects =
		(FX_INFO *)Game_Alloc(NUM_EFFECTS * sizeof(FX_INFO), GBUF_EFFECTS);

	InitialiseFXArray();
	InitialiseLOTArray();
	Init_Colours();
	Text_Init();
	Overlay_Init();

	g_HealthBarTimer = 100;

	/*
	Sound_ResetEffects();
	*/

	if (g_LevelNumTR == 21)
	{
		ExtractSaveGameInfo();
	}

	phd_AlterFOV(80 * PHD_DEGREE);

	/*
	if (g_GameFlow.levels[g_CurrentLevel].music)
	{
		Music_PlayLooped(g_GameFlow.levels[g_CurrentLevel].music);
	}
	*/
	g_Lara.request_gun_type = LGT_UNARMED;

	g_Camera.underwater = 0;

	return (1);
}

void Initialise_Game_Flags()
{
	for (int i = 0; i < MAX_FLIP_MAPS; i++)
	{
		g_FlipMapTable[i] = 0;
	}

	for (int i = 0; i < MAX_CD_TRACKS; i++)
	{
		g_MusicTrackFlags[i] = 0;
	}

	for (int i = 0; i < O_NUMBER_OF; i++)
	{
		g_Objects[i].loaded = 0;
	}

	g_FlipStatus = 0;
	g_LevelComplete = false;
	g_FlipEffect = -1;
	g_PierreItemNum = NO_ITEM;
}

int Load_Level(int32_t LevelNum)
{
	char *szLevelName;

	if (GameType == VER_TR1)
	{
		szLevelName = LevelNamesTR1[LevelNum];
	}
	else if (GameType == VER_TR1_GOLD)
	{
		szLevelName = LevelNamesGold[LevelNum];
	}
	else
		return false;

	bool result = S_LoadLevel(szLevelName);

	g_GameFlow.levels[LevelNum].secrets = GetSecretCount();

	return result;
}
