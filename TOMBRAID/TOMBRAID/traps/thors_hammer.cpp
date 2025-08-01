#include "thors_hammer.h"

#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\vars.h"
#include "movable_block.h"

void SetupThorsHandle(OBJECT_INFO *obj)
{
	obj->initialise = InitialiseThorsHandle;
	obj->control = ThorsHandleControl;
	obj->draw_routine = DrawUnclippedItem;
	obj->collision = ThorsHandleCollision;
	obj->save_flags = 1;
	obj->save_anim = 1;
}

void SetupThorsHead(OBJECT_INFO *obj)
{
	obj->collision = ThorsHeadCollision;
	obj->draw_routine = DrawUnclippedItem;
	obj->save_flags = 1;
	obj->save_anim = 1;
}

void InitialiseThorsHandle(int16_t item_num)
{
	ITEM_INFO *hand_item = &g_Items[item_num];
	int16_t head_item_num = CreateItem();
	ITEM_INFO *head_item = &g_Items[head_item_num];
	head_item->object_number = O_THORS_HEAD;
	head_item->room_number = hand_item->room_number;
	head_item->pos = hand_item->pos;
	head_item->shade = hand_item->shade;
	InitialiseItem(head_item_num);
	hand_item->data = head_item;
	g_LevelItemCount++;
}

void ThorsHandleControl(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];

	switch (item->current_anim_state)
	{
	case THS_SET:
		if (TriggerActive(item))
		{
			item->goal_anim_state = THS_TEASE;
		}
		else
		{
			RemoveActiveItem(item_num);
			item->status = IS_NOT_ACTIVE;
		}
		break;

	case THS_TEASE:
		if (TriggerActive(item))
		{
			item->goal_anim_state = THS_ACTIVE;
		}
		else
		{
			item->goal_anim_state = THS_SET;
		}
		break;

	case THS_ACTIVE:
	{
		int32_t frm =
			item->frame_number - g_Anims[item->anim_number].frame_base;
		if (frm > 30)
		{
			int32_t x = item->pos.x;
			int32_t z = item->pos.z;

			switch (item->pos.y_rot)
			{
			case 0:
				z += WALL_L * 3;
				break;
			case PHD_90:
				x += WALL_L * 3;
				break;
			case -PHD_90:
				x -= WALL_L * 3;
				break;
			case -PHD_180:
				z -= WALL_L * 3;
				break;
			}

			if (g_LaraItem->hit_points >= 0 && g_LaraItem->pos.x > x - 520 &&
				g_LaraItem->pos.x < x + 520 && g_LaraItem->pos.z > z - 520 &&
				g_LaraItem->pos.z < z + 520)
			{
				g_LaraItem->hit_points = -1;
				g_LaraItem->pos.y = item->pos.y;
				g_LaraItem->gravity_status = 0;
				g_LaraItem->current_anim_state = AS_SPECIAL;
				g_LaraItem->goal_anim_state = AS_SPECIAL;
				g_LaraItem->anim_number = AA_RBALL_DEATH;
				g_LaraItem->frame_number = AF_RBALL_DEATH;
			}
		}
		break;
	}

	case THS_DONE:
	{
		int32_t x = item->pos.x;
		int32_t z = item->pos.z;
		int32_t old_x = x;
		int32_t old_z = z;

		int16_t room_num = item->room_number;
		FLOOR_INFO *floor = GetFloor(x, item->pos.y, z, &room_num);
		GetHeight(floor, x, item->pos.y, z);
		TestTriggers(g_TriggerIndex, 1);

		switch (item->pos.y_rot)
		{
		case 0:
			z += WALL_L * 3;
			break;
		case PHD_90:
			x += WALL_L * 3;
			break;
		case -PHD_90:
			x -= WALL_L * 3;
			break;
		case -PHD_180:
			z -= WALL_L * 3;
			break;
		}

		item->pos.x = x;
		item->pos.z = z;
		if (g_LaraItem->hit_points >= 0)
		{
			AlterFloorHeight(item, -WALL_L * 2);
		}
		item->pos.x = old_x;
		item->pos.z = old_z;

		RemoveActiveItem(item_num);
		item->status = IS_DEACTIVATED;
		break;
	}
	}
	AnimateItem(item);

	ITEM_INFO *head_item = (ITEM_INFO *)item->data;
	int32_t anim = item->anim_number - g_Objects[O_THORS_HANDLE].anim_index;
	int32_t frm = item->frame_number - g_Anims[item->anim_number].frame_base;
	head_item->anim_number = g_Objects[O_THORS_HEAD].anim_index + anim;
	head_item->frame_number = g_Anims[head_item->anim_number].frame_base + frm;
	head_item->current_anim_state = item->current_anim_state;
}

void ThorsHandleCollision(int16_t item_num, ITEM_INFO *lara_item,
						  COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];
	if (!TestBoundsCollide(item, lara_item, coll->radius))
	{
		return;
	}
	if (coll->enable_baddie_push)
	{
		ItemPushLara(item, lara_item, coll, 0, 1);
	}
}

void ThorsHeadCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];
	if (!TestBoundsCollide(item, lara_item, coll->radius))
	{
		return;
	}
	if (coll->enable_baddie_push && item->current_anim_state != THS_ACTIVE)
	{
		ItemPushLara(item, lara_item, coll, 0, 1);
	}
}
