#pragma once

#include "..\\SPECIFIC\\types.h"

#define LAVA_GLOB_DAMAGE 10
#define LAVA_WEDGE_SPEED 25

void LavaBurn(ITEM_INFO *item);
void LavaEmitterControl(int16_t item_num);
void LavaControl(int16_t fx_num);
void LavaWedgeControl(int16_t item_num);
