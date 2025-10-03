#pragma once

#include "types.h"

int LevelStats(int32_t level_num);
int Game_Loop(int demo_mode);
void Initialise_Camera();
int Control_Phase(int32_t nframes, int32_t demo_mode);
int Get_Key_State(int key);
void Input_Update();
INPUT_STATE Input_GetDebounced(INPUT_STATE input);
int Draw_Phase_Game();
void S_InitialisePolyList();
void DrawRooms(int16_t current_room);
void S_OutputPolyList();
int32_t S_DumpScreen();
void GetRoomBounds(int16_t room_num);
int32_t SetRoomBounds(int16_t *objptr, int16_t room_num, ROOM_INFO *parent);
void SetupAboveWater(int underwater);
void SetupBelowWater(int underwater);
void PrintRooms(int16_t room_number);
void DrawRoom(int16_t *obj_ptr);
int32_t CalcFogShade(int32_t depth);
int32_t S_SaveGame(SAVEGAME_INFO *save, int32_t slot);
int32_t S_LoadGame(SAVEGAME_INFO *save, int32_t slot);
void GetSavedGamesList(REQUEST_INFO *req);
int32_t S_FrontEndCheck();
