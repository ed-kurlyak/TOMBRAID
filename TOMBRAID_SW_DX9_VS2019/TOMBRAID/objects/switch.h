#pragma once

#include "..\\SPECIFIC\\types.h"

extern PHD_VECTOR g_Switch2Position;
extern int16_t g_Switch1Bounds[12];
extern int16_t g_Switch2Bounds[12];

void SwitchCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll);
void SwitchCollision2(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll);
void SwitchControl(int16_t item_num);
int32_t SwitchTrigger(int16_t item_num, int16_t timer);
