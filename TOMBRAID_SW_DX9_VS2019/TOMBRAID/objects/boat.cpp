#include "boat.h"

#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\vars.h"
/*
void SetupBoat(OBJECT_INFO *obj)
{
	obj->control = BoatControl;
	obj->save_flags = 1;
	obj->save_anim = 1;
	obj->save_position = 1;
}
*/

void BoatControl(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];

	switch (item->current_anim_state)
	{
	case BOAT_SET:
		item->goal_anim_state = BOAT_MOVE;
		break;
	case BOAT_MOVE:
		item->goal_anim_state = BOAT_STOP;
		break;
	case BOAT_STOP:
		KillItem(item_num);
		break;
	}

	AnimateItem(item);
}
