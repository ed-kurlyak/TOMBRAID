#pragma once

#include "..\\SPECIFIC\\types.h"

void TrapDoorControl(int16_t item_num);
void TrapDoorFloor(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
				   int16_t *height);
void TrapDoorCeiling(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					 int16_t *height);
int32_t OnTrapDoor(ITEM_INFO *item, int32_t x, int32_t z);
