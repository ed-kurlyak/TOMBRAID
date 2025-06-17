#pragma once

#include "types.h"

int LevelStats(int32_t level_num);
int Game_Loop(int demo_mode);
void Initialise_Camera();
//void CalculateCamera();
int Control_Phase(int32_t nframes, int32_t demo_mode);
int Get_Key_State(int key);
void Input_Update();
INPUT_STATE Input_GetDebounced(INPUT_STATE input);
//void LaraControl(int16_t item_num);
int Draw_Phase_Game();
void S_InitialisePolyList();
//void ChaseCamera(ITEM_INFO *item);
//void MoveCamera(GAME_VECTOR *ideal, int32_t speed);
void DrawRooms(int16_t current_room);
void S_OutputPolyList();
int32_t S_DumpScreen();
/*
void SmartShift(GAME_VECTOR *ideal,
    void (*shift)(
        int32_t *x, int32_t *y, int32_t target_x, int32_t target_y,
        int32_t left, int32_t top, int32_t right, int32_t bottom));
void ShiftCamera(int32_t *x, int32_t *y, int32_t target_x, int32_t target_y, int32_t left,
    int32_t top, int32_t right, int32_t bottom);
*/
/*
int32_t zLOS(GAME_VECTOR *start, GAME_VECTOR *target);
int32_t xLOS(GAME_VECTOR *start, GAME_VECTOR *target);
int32_t LOS(GAME_VECTOR *start, GAME_VECTOR *target);
*/
void GetRoomBounds(int16_t room_num);
int32_t SetRoomBounds(int16_t *objptr, int16_t room_num, ROOM_INFO *parent);
void SetupAboveWater(int underwater);
void SetupBelowWater(int underwater);
void PrintRooms(int16_t room_number);
//int32_t BadPosition(int32_t x, int32_t y, int32_t z, int16_t room_num);
//int32_t ClipTarget(GAME_VECTOR *start, GAME_VECTOR *target, FLOOR_INFO *floor);
void DrawRoom(int16_t *obj_ptr);
//void LaraBaddieCollision(ITEM_INFO *lara_item, COLL_INFO *coll);
//void ShiftItem(ITEM_INFO *item, COLL_INFO *coll);
int32_t CalcFogShade(int32_t depth);
//int32_t CalculateTarget(PHD_VECTOR *target, ITEM_INFO *item, LOT_INFO *LOT);
//int32_t UpdateLOT(LOT_INFO *LOT, int32_t expansion);
//int32_t SearchLOT(LOT_INFO *LOT, int32_t expansion);

int32_t S_SaveGame(SAVEGAME_INFO *save, int32_t slot);
int32_t S_LoadGame(SAVEGAME_INFO *save, int32_t slot);
void GetSavedGamesList(REQUEST_INFO *req);
int32_t S_FrontEndCheck();
