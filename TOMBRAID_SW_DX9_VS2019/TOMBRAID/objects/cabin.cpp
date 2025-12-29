#include "cabin.h"
#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\vars.h"

void CabinControl(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];

	if ((item->flags & IF_CODE_BITS) == IF_CODE_BITS)
	{
		switch (item->current_anim_state)
		{
		case CABIN_START:
			item->goal_anim_state = CABIN_DROP1;
			break;
		case CABIN_DROP1:
			item->goal_anim_state = CABIN_DROP2;
			break;
		case CABIN_DROP2:
			item->goal_anim_state = CABIN_DROP3;
			break;
		}
		item->flags = 0;
	}

	if (item->current_anim_state == CABIN_FINISH)
	{
		g_FlipMapTable[3] = IF_CODE_BITS;
		FlipMap();
		KillItem(item_num);
	}

	AnimateItem(item);
}
