#ifndef _TWINKLE_
#define _TWINKLE_

#include "..\\SPECIFIC\\types.h"

void SetupTwinkle(OBJECT_INFO *obj);
void Twinkle(GAME_VECTOR *pos);
void ItemSparkle(ITEM_INFO *item, int meshmask);
void ControlTwinkle(int16_t fx_num);

#endif