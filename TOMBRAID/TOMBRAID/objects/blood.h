#pragma once

#include "..\\SPECIFIC\\types.h"

//#include <stdint.h>

// void SetupBlood(OBJECT_INFO *obj);
int16_t DoBloodSplat(int32_t x, int32_t y, int32_t z, int16_t speed,
					 int16_t direction, int16_t room_num);
void ControlBlood1(int16_t fx_num);
