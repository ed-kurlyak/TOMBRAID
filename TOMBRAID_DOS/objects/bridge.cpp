#include "bridge.h"
#include "cog.h"
#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\vars.h"

int32_t OnDrawBridge(ITEM_INFO *item, int32_t x, int32_t y)
{
	int32_t ix = item->pos.z >> WALL_SHIFT;
	int32_t iy = item->pos.x >> WALL_SHIFT;

	x >>= WALL_SHIFT;
	y >>= WALL_SHIFT;

	if (item->pos.y_rot == 0 && y == iy && (x == ix - 1 || x == ix - 2))
	{
		return 1;
	}
	if (item->pos.y_rot == -PHD_180 && y == iy && (x == ix + 1 || x == ix + 2))
	{
		return 1;
	}
	if (item->pos.y_rot == PHD_90 && x == ix && (y == iy - 1 || y == iy - 2))
	{
		return 1;
	}
	if (item->pos.y_rot == -PHD_90 && x == ix && (y == iy + 1 || y == iy + 2))
	{
		return 1;
	}

	return 0;
}

void DrawBridgeFloor(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					 int16_t *height)
{
	if (item->current_anim_state != DOOR_OPEN)
	{
		return;
	}
	if (!OnDrawBridge(item, z, x))
	{
		return;
	}

	if (y <= item->pos.y)
	{
		*height = item->pos.y;
	}
}

void DrawBridgeCeiling(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					   int16_t *height)
{
	if (item->current_anim_state != DOOR_OPEN)
	{
		return;
	}
	if (!OnDrawBridge(item, z, x))
	{
		return;
	}

	if (y > item->pos.y)
	{
		*height = item->pos.y + STEP_L;
	}
}

void DrawBridgeCollision(int16_t item_num, ITEM_INFO *lara_item,
						 COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];
	if (item->current_anim_state == DOOR_CLOSED)
	{
		DoorCollision(item_num, lara_item, coll);
	}
}

void BridgeFlatFloor(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					 int16_t *height)
{
	if (y <= item->pos.y)
	{
		*height = item->pos.y;
	}
}

void BridgeFlatCeiling(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					   int16_t *height)
{
	if (y > item->pos.y)
	{
		*height = item->pos.y + STEP_L;
	}
}

int32_t GetOffset(ITEM_INFO *item, int32_t x, int32_t z)
{
	if (item->pos.y_rot == 0)
	{
		return (WALL_L - x) & (WALL_L - 1);
	}
	else if (item->pos.y_rot == -PHD_180)
	{
		return x & (WALL_L - 1);
	}
	else if (item->pos.y_rot == PHD_90)
	{
		return z & (WALL_L - 1);
	}
	else
	{
		return (WALL_L - z) & (WALL_L - 1);
	}
}

void BridgeTilt1Floor(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					  int16_t *height)
{
	int32_t level = item->pos.y + (GetOffset(item, x, z) >> 2);
	if (y <= level)
	{
		*height = level;
	}
}

void BridgeTilt1Ceiling(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
						int16_t *height)
{
	int32_t level = item->pos.y + (GetOffset(item, x, z) >> 2);
	if (y > level)
	{
		*height = level + STEP_L;
	}
}

void BridgeTilt2Floor(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					  int16_t *height)
{
	int32_t level = item->pos.y + (GetOffset(item, x, z) >> 1);
	if (y <= level)
	{
		*height = level;
	}
}

void BridgeTilt2Ceiling(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
						int16_t *height)
{
	int32_t level = item->pos.y + (GetOffset(item, x, z) >> 1);
	if (y > level)
	{
		*height = level + STEP_L;
	}
}
