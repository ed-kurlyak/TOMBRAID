#include "spikes.h"

#include "..\\OBJECTS\\blood.h"
#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\sphere.h"
#include "..\\SPECIFIC\\vars.h"
#include "..\\SPECIFIC\\winmain.h"

void SetupSpikes(OBJECT_INFO *obj) { obj->collision = SpikeCollision; }

void SpikeCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];
	if (lara_item->hit_points < 0)
	{
		return;
	}

	if (!TestBoundsCollide(item, lara_item, coll->radius))
	{
		return;
	}
	if (!TestCollision(item, lara_item))
	{
		return;
	}

	int32_t num = Random_GetControl() / 24576;
	if (lara_item->gravity_status)
	{
		if (lara_item->fall_speed > 0)
		{
			lara_item->hit_points = -1;
			num = 20;
		}
	}
	else if (lara_item->speed < 30)
	{
		return;
	}

	lara_item->hit_points -= SPIKE_DAMAGE;
	while (num > 0)
	{
		int32_t x = lara_item->pos.x + (Random_GetControl() - 0x4000) / 256;
		int32_t z = lara_item->pos.z + (Random_GetControl() - 0x4000) / 256;
		int32_t y = lara_item->pos.y - Random_GetControl() / 64;
		DoBloodSplat(x, y, z, 20, Random_GetControl(), item->room_number);
		num--;
	}

	if (lara_item->hit_points <= 0)
	{
		lara_item->current_anim_state = AS_DEATH;
		lara_item->goal_anim_state = AS_DEATH;
		lara_item->anim_number = AA_SPIKE_DEATH;
		lara_item->frame_number = AF_SPIKE_DEATH;
		lara_item->pos.y = item->pos.y;
		lara_item->gravity_status = 0;
	}
}
