#pragma once

//#include <stdint.h>
#include "types.h"

int32_t  Play_Cinematic(int32_t level_num);
void InitCinematicRooms();
int32_t StartCinematic(int32_t level_num);
int32_t StopCinematic(int32_t level_num);
int32_t CinematicLoop();
int32_t DoCinematic(int32_t nframes);
void CalculateCinematicCamera();
void ControlCinematicPlayer(int16_t item_num);
void ControlCinematicPlayer4(int16_t item_num);
void InitialisePlayer1(int16_t item_num);
void InitialiseGenPlayer(int16_t item_num);
void InGameCinematicCamera();
