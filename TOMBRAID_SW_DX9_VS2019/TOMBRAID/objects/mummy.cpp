#include "mummy.h"
#include "..\\SPECIFIC\\box.h"
#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\init.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\phd_math.h"
#include "..\\SPECIFIC\\util.h"
#include "..\\SPECIFIC\\vars.h"

void InitialiseMummy(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];
	item->touch_bits = 0;
	item->mesh_bits = 0xFFFF87FF;
	item->data = Game_Alloc(sizeof(int16_t), GBUF_CREATURE_DATA);
	*(int16_t *)item->data = 0;
}

void MummyControl(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];
	int16_t head = 0;

	if (item->current_anim_state == MUMMY_STOP)
	{
		head = phd_atan(g_LaraItem->pos.z - item->pos.z,
						g_LaraItem->pos.x - item->pos.x) -
			   item->pos.y_rot;
		CLAMP(head, -FRONT_ARC, FRONT_ARC);

		if (item->hit_points <= 0 || item->touch_bits)
		{
			item->goal_anim_state = MUMMY_DEATH;
		}
	}

	CreatureHead(item, head);
	AnimateItem(item);

	if (item->status == IS_DEACTIVATED)
	{
		RemoveActiveItem(item_num);
		item->hit_points = DONT_TARGET;
	}
}
