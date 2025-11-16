#ifndef _SETUP_
#define _SETUP_

#include "lot.h"
#include "types.h"

int Initialise_Level_Flags();
// int Initialise_Level(int LevelNum);
int Initialise_Level(int LevelNum);
void Initialise_Game_Flags();
int Compose_Colour(int inputR, int inputG, int inputB);
int Load_Level(int32_t LevelNum);
// void InitialiseLara();
// void InitialiseLaraInventory(int32_t level_num);
// int32_t InitialiseLOT(LOT_INFO *LOT);
// void LaraInitialiseMeshes(int32_t level_num);

#endif