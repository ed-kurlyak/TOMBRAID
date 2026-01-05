#pragma once

#include "..\\SPECIFIC\\types.h"

#define VOLE_DIE_ANIM 2
#define VOLE_SWIM_TURN (PHD_DEGREE * 3) // = 546
#define VOLE_ATTACK_RANGE SQUARE(300)   // = 90000

void VoleControl(int16_t item_num);
