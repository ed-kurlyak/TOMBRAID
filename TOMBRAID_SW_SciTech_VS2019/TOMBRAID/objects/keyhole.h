#pragma once

#include "..\\SPECIFIC\\types.h"

extern PHD_VECTOR g_KeyHolePosition;
extern int16_t g_KeyHoleBounds[12];

extern int32_t g_PickUpX;
extern int32_t g_PickUpY;
extern int32_t g_PickUpZ;

void KeyHoleCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll);
int32_t KeyTrigger(int16_t item_num);
