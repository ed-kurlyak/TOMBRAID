#pragma once

#include "..\\SPECIFIC\\types.h"

//#include <stdint.h>

typedef enum
{
	MBS_STILL = 1,
	MBS_PUSH = 2,
	MBS_PULL = 3,
} MOVABLE_BLOCK_STATE;

extern int16_t g_MovingBlockBounds[12];

// void SetupMovableBlock(OBJECT_INFO *obj);
void InitialiseMovableBlock(int16_t item_num);
void MovableBlockControl(int16_t item_num);
void MovableBlockCollision(int16_t item_num, ITEM_INFO *lara_item,
						   COLL_INFO *coll);
void DrawMovableBlock(ITEM_INFO *item);

int32_t TestBlockMovable(ITEM_INFO *item, int32_t blockhite);
int32_t TestBlockPush(ITEM_INFO *item, int32_t blokhite, uint16_t quadrant);
int32_t TestBlockPull(ITEM_INFO *item, int32_t blokhite, uint16_t quadrant);

void AlterFloorHeight(ITEM_INFO *item, int32_t height);
