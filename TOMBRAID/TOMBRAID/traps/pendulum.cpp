#include "pendulum.h"

//#include "game/collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\objects\\blood.h"
//#include "game/random.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\vars.h"
#include "..\\SPECIFIC\\winmain.h"

/*
void SetupPendulum(OBJECT_INFO *obj)
{
	obj->control = PendulumControl;
	obj->collision = TrapCollision;
	obj->shadow_size = UNIT_SHADOW / 2;
	obj->save_flags = 1;
	obj->save_anim = 1;
}
*/

void PendulumControl(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (TriggerActive(item))
	{
		if (item->current_anim_state == TRAP_SET)
		{
			item->goal_anim_state = TRAP_WORKING;
		}
	}
	else
	{
		if (item->current_anim_state == TRAP_WORKING)
		{
			item->goal_anim_state = TRAP_SET;
		}
	}

	if (item->current_anim_state == TRAP_WORKING && item->touch_bits)
	{
		g_LaraItem->hit_points -= PENDULUM_DAMAGE;
		g_LaraItem->hit_status = 1;
		int32_t x = g_LaraItem->pos.x + (Random_GetControl() - 0x4000) / 256;
		int32_t z = g_LaraItem->pos.z + (Random_GetControl() - 0x4000) / 256;
		int32_t y = g_LaraItem->pos.y - Random_GetControl() / 44;
		int32_t d = g_LaraItem->pos.y_rot + (Random_GetControl() - 0x4000) / 8;
		DoBloodSplat(x, y, z, g_LaraItem->speed, d, g_LaraItem->room_number);
	}

	FLOOR_INFO *floor =
		GetFloor(item->pos.x, item->pos.y, item->pos.z, &item->room_number);
	item->floor = GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);

	AnimateItem(item);
}
