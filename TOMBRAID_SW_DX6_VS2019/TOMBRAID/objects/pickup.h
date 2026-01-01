#pragma once

#include "..\\SPECIFIC\\types.h"

extern PHD_VECTOR g_PickUpPosition;
extern PHD_VECTOR g_PickUpPositionUW;
extern int16_t g_PickUpBounds[12];
extern int16_t g_PickUpBoundsUW[12];

void PickUpCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll);
