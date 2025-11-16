#ifndef _FILE_
#define _FILE_

#include "types.h"
//#include "draw.h"
#include "init.h"
#include "types.h"
#include "vars.h"
#include <stdio.h>
#include <windows.h>

bool S_LoadLevel(char *FileName);
bool Load_Texture_Pages(FILE *fp);
bool Load_Rooms(FILE *fp);
bool Load_Objects(FILE *fp);
void Initialise_Objects();
void BaddyObjects();
void TrapObjects();
void ObjectObjects();
bool LoadSprites(FILE *fp);
bool LoadCameras(FILE *fp);
bool LoadSoundEffects(FILE *fp);
bool LoadBoxes(FILE *fp);
bool LoadAnimatedTextures(FILE *fp);
bool LoadItems(FILE *fp);
// void InitialiseItemArray(int32_t NumItems);
// void InitialiseItem(int16_t ItemNum);
bool LoadDepthQ(FILE *fp);
bool LoadPalette(FILE *fp);
bool LoadCinematic(FILE *fp);
bool LoadDemo(FILE *fp);
bool LoadSoundSamples(FILE *fp);
size_t File_Size(FILE *fp);
// void DirectSound_Init();

void Create_DX_Textures();

#endif