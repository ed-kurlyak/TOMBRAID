#include "types.h"

int32_t GetChange(ITEM_INFO *item, ANIM_STRUCT *anim);
FLOOR_INFO *GetFloor(int32_t x, int32_t y, int32_t z, int16_t *room_num);
int16_t GetWaterHeight(int32_t x, int32_t y, int32_t z, int16_t room_num);
int16_t GetHeight(FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z);
int16_t GetCeiling(FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z);
int16_t GetDoor(FLOOR_INFO *floor);
int32_t LOS(GAME_VECTOR *start, GAME_VECTOR *target);
int32_t zLOS(GAME_VECTOR *start, GAME_VECTOR *target);
int32_t xLOS(GAME_VECTOR *start, GAME_VECTOR *target);
int32_t ClipTarget(GAME_VECTOR *start, GAME_VECTOR *target, FLOOR_INFO *floor);
int GetSecretCount();