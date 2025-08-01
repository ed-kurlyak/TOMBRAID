#include "switch.h"

#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
//#include "game/input.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\lara.h"
#include "..\\SPECIFIC\\vars.h"

PHD_VECTOR g_Switch2Position = {0, 0, 108};

int16_t g_Switch1Bounds[12] = {
	-200,
	+200,
	+0,
	+0,
	+WALL_L / 2 - 200,
	+WALL_L / 2,
	-10 * PHD_DEGREE,
	+10 * PHD_DEGREE,
	-30 * PHD_DEGREE,
	+30 * PHD_DEGREE,
	-10 * PHD_DEGREE,
	+10 * PHD_DEGREE,
};

int16_t g_Switch2Bounds[12] = {
	-WALL_L,		  +WALL_L,			-WALL_L,		  +WALL_L,
	-WALL_L,		  +WALL_L / 2,		-80 * PHD_DEGREE, +80 * PHD_DEGREE,
	-80 * PHD_DEGREE, +80 * PHD_DEGREE, -80 * PHD_DEGREE, +80 * PHD_DEGREE,
};

/*
void SetupSwitch1(OBJECT_INFO *obj)
{
	obj->control = SwitchControl;
	obj->collision = SwitchCollision;
	obj->save_anim = 1;
	obj->save_flags = 1;
}

void SetupSwitch2(OBJECT_INFO *obj)
{
	obj->control = SwitchControl;
	obj->collision = SwitchCollision2;
	obj->save_anim = 1;
	obj->save_flags = 1;
}
*/
void SwitchControl(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];
	item->flags |= IF_CODE_BITS;

	if (!TriggerActive(item))
	{
		item->goal_anim_state = SWITCH_STATE_ON;
		item->timer = 0;
	}

	AnimateItem(item);
}

int32_t SwitchTrigger(int16_t item_num, int16_t timer)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (item->status != IS_DEACTIVATED)
	{
		return 0;
	}

	if (item->current_anim_state == SWITCH_STATE_OFF && timer > 0)
	{
		item->timer = timer;

		if (timer != 1)
		{
			item->timer *= FRAMES_PER_SECOND;
		}
		item->status = IS_ACTIVE;
	}
	else
	{
		RemoveActiveItem(item_num);
		item->status = IS_NOT_ACTIVE;
	}
	return 1;
}

void SwitchCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (!g_Input.action || item->status != IS_NOT_ACTIVE ||
		g_Lara.gun_status != LGS_ARMLESS || lara_item->gravity_status)
	{
		return;
	}

	if (lara_item->current_anim_state != AS_STOP)
	{
		return;
	}

	if (!TestLaraPosition(g_Switch1Bounds, item, lara_item))
	{
		return;
	}

	lara_item->pos.y_rot = item->pos.y_rot;

	if (item->current_anim_state == SWITCH_STATE_ON)
	{
		AnimateLaraUntil(lara_item, AS_SWITCHON);
		lara_item->goal_anim_state = AS_STOP;
		g_Lara.gun_status = LGS_HANDSBUSY;
		item->status = IS_ACTIVE;
		item->goal_anim_state = SWITCH_STATE_OFF;
		AddActiveItem(item_num);
		AnimateItem(item);
	}
	else if (item->current_anim_state == SWITCH_STATE_OFF)
	{
		AnimateLaraUntil(lara_item, AS_SWITCHOFF);
		lara_item->goal_anim_state = AS_STOP;
		g_Lara.gun_status = LGS_HANDSBUSY;
		item->status = IS_ACTIVE;
		item->goal_anim_state = SWITCH_STATE_ON;
		AddActiveItem(item_num);
		AnimateItem(item);
	}
}

void SwitchCollision2(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (!g_Input.action || item->status != IS_NOT_ACTIVE ||
		g_Lara.water_status != LWS_UNDERWATER)
	{
		return;
	}

	if (lara_item->current_anim_state != AS_TREAD)
	{
		return;
	}

	if (!TestLaraPosition(g_Switch2Bounds, item, lara_item))
	{
		return;
	}

	if (item->current_anim_state == SWITCH_STATE_ON ||
		item->current_anim_state == SWITCH_STATE_OFF)
	{
		if (!MoveLaraPosition(&g_Switch2Position, item, lara_item))
		{
			return;
		}

		lara_item->fall_speed = 0;
		AnimateLaraUntil(lara_item, AS_SWITCHON);
		lara_item->goal_anim_state = AS_TREAD;
		g_Lara.gun_status = LGS_HANDSBUSY;
		item->status = IS_ACTIVE;
		if (item->current_anim_state == SWITCH_STATE_ON)
		{
			item->goal_anim_state = SWITCH_STATE_OFF;
		}
		else
		{
			item->goal_anim_state = SWITCH_STATE_ON;
		}
		AddActiveItem(item_num);
		AnimateItem(item);
	}
}
