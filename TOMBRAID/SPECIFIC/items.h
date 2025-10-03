#pragma once

#include "types.h"

//#include <stdint.h>

void InitialiseItemArray(int32_t num_items);
void KillItem(int16_t item_num);
int16_t CreateItem();
void InitialiseItem(int16_t item_num);
void RemoveActiveItem(int16_t item_num);
void RemoveDrawnItem(int16_t item_num);
void AddActiveItem(int16_t item_num);
void ItemNewRoom(int16_t item_num, int16_t room_num);
int16_t SpawnItem(ITEM_INFO *item, int16_t object_num);
int32_t GlobalItemReplace(int32_t src_object_num, int32_t dst_object_num);
void InitialiseFXArray();
int16_t CreateEffect(int16_t room_num);
void KillEffect(int16_t fx_num);
void EffectNewRoom(int16_t fx_num, int16_t room_num);
