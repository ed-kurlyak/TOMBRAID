#pragma once

#include "..\\SPECIFIC\\types.h"

#define POD_EXPLODE_DIST (WALL_L * 4) // = 4096

typedef enum
{
	POD_SET = 0,
	POD_EXPLODE = 1,
} POD_ANIM;

void InitialisePod(int16_t item_num);
void PodControl(int16_t item_num);
