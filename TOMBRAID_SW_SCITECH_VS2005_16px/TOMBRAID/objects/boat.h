#pragma once

#include "..\\SPECIFIC\\types.h"

typedef enum
{
	BOAT_EMPTY = 0,
	BOAT_SET = 1,
	BOAT_MOVE = 2,
	BOAT_STOP = 3,
} BOAT_ANIM;

void BoatControl(int16_t item_num);
