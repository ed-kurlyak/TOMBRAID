#pragma once

#include "types.h"

//#include <stdint.h>

void InitialiseLOTArray();
void DisableBaddieAI(int16_t item_num);
int32_t EnableBaddieAI(int16_t item_num, int32_t always);
void InitialiseSlot(int16_t item_num, int32_t slot);
void CreateZone(ITEM_INFO *item);
int32_t InitialiseLOT(LOT_INFO *LOT);
void ClearLOT(LOT_INFO *LOT);
