#include "scion.h"

#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\draw.h"
//#include "game/input.h"
#include "../objects/pickup.h"
#include "..\\SPECIFIC\\inv.h"
#include "..\\SPECIFIC\\items.h"
//#include "game/overlay.h"
#include "..\\SPECIFIC\\overlay.h"
#include "..\\SPECIFIC\\winmain.h"
//#include "game/sound.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"

PHD_VECTOR g_PickUpScionPosition = {0, 640, -310};
PHD_VECTOR g_PickUpScion4Position = {0, 280, -512 + 105};

int16_t g_PickUpScionBounds[12] = {
	-256,
	+256,
	+640 - 100,
	+640 + 100,
	-350,
	-200,
	-10 * PHD_DEGREE,
	+10 * PHD_DEGREE,
	0,
	0,
	0,
	0,
};

int16_t g_PickUpScion4Bounds[12] = {
	-256,
	+256,
	+256 - 50,
	+256 + 50,
	-512 - 350,
	-200,
	-10 * PHD_DEGREE,
	+10 * PHD_DEGREE,
	0,
	0,
	0,
	0,
};

void SetupScion1(OBJECT_INFO *obj)
{
	g_Objects[O_SCION_ITEM].draw_routine = DrawPickupItem;
	g_Objects[O_SCION_ITEM].collision = PickUpScionCollision;
}

void SetupScion2(OBJECT_INFO *obj)
{
	g_Objects[O_SCION_ITEM2].draw_routine = DrawPickupItem;
	g_Objects[O_SCION_ITEM2].collision = PickUpCollision;
	g_Objects[O_SCION_ITEM2].save_flags = 1;
}

void SetupScion3(OBJECT_INFO *obj)
{
	g_Objects[O_SCION_ITEM3].control = Scion3Control;
	g_Objects[O_SCION_ITEM3].hit_points = 5;
	g_Objects[O_SCION_ITEM3].save_flags = 1;
}

void SetupScion4(OBJECT_INFO *obj)
{
	g_Objects[O_SCION_ITEM4].control = ScionControl;
	g_Objects[O_SCION_ITEM4].collision = PickUpScion4Collision;
	g_Objects[O_SCION_ITEM4].save_flags = 1;
}

void SetupScionHolder(OBJECT_INFO *obj)
{
	g_Objects[O_SCION_HOLDER].control = ScionControl;
	g_Objects[O_SCION_HOLDER].collision = ObjectCollision;
	g_Objects[O_SCION_HOLDER].save_anim = 1;
	g_Objects[O_SCION_HOLDER].save_flags = 1;
}

void ScionControl(int16_t item_num) { AnimateItem(&g_Items[item_num]); }

void Scion3Control(int16_t item_num)
{
	static int32_t counter = 0;
	ITEM_INFO *item = &g_Items[item_num];

	if (item->hit_points > 0)
	{
		counter = 0;
		AnimateItem(item);
		return;
	}

	if (counter == 0)
	{
		item->status = IS_INVISIBLE;
		item->hit_points = DONT_TARGET;
		int16_t room_num = item->room_number;
		FLOOR_INFO *floor =
			GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_num);
		GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
		TestTriggers(g_TriggerIndex, 1);
		RemoveDrawnItem(item_num);
	}

	if (counter % 10 == 0)
	{
		int16_t fx_num = CreateEffect(item->room_number);
		if (fx_num != NO_ITEM)
		{
			FX_INFO *fx = &g_Effects[fx_num];
			fx->pos.x = item->pos.x + (Random_GetControl() - 0x4000) / 32;
			fx->pos.y =
				item->pos.y + (Random_GetControl() - 0x4000) / 256 - 500;
			fx->pos.z = item->pos.z + (Random_GetControl() - 0x4000) / 32;
			fx->speed = 0;
			fx->frame_number = 0;
			fx->object_number = O_EXPLOSION1;
			fx->counter = 0;
			Sound_Effect(SFX_ATLANTEAN_EXPLODE, &fx->pos, SPM_NORMAL);
			g_Camera.bounce = -200;
		}
	}

	counter++;
	if (counter >= FRAMES_PER_SECOND * 3)
	{
		KillItem(item_num);
	}
}

void PickUpScionCollision(int16_t item_num, ITEM_INFO *lara_item,
						  COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];
	item->pos.y_rot = lara_item->pos.y_rot;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;

	if (!TestLaraPosition(g_PickUpScionBounds, item, lara_item))
	{
		return;
	}

	if (lara_item->current_anim_state == AS_PICKUP)
	{
		if (lara_item->frame_number ==
			g_Anims[lara_item->anim_number].frame_base + AF_PICKUPSCION)
		{
			Overlay_AddPickup(item->object_number);
			Inv_AddItem(item->object_number);
			item->status = IS_INVISIBLE;
			RemoveDrawnItem(item_num);
			g_SaveGame.pickups++;
		}
	}
	else if (g_Input.action && g_Lara.gun_status == LGS_ARMLESS &&
			 !lara_item->gravity_status &&
			 lara_item->current_anim_state == AS_STOP)
	{
		AlignLaraPosition(&g_PickUpScionPosition, item, lara_item);
		lara_item->current_anim_state = AS_PICKUP;
		lara_item->goal_anim_state = AS_PICKUP;
		lara_item->anim_number = g_Objects[O_LARA_EXTRA].anim_index;
		lara_item->frame_number = g_Anims[lara_item->anim_number].frame_base;
		g_Lara.gun_status = LGS_HANDSBUSY;
		g_Camera.type = CAM_CINEMATIC;
		g_CineFrame = 0;
		g_CinePosition = lara_item->pos;
	}
}

void PickUpScion4Collision(int16_t item_num, ITEM_INFO *lara_item,
						   COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];
	item->pos.y_rot = lara_item->pos.y_rot;
	item->pos.x_rot = 0;
	item->pos.z_rot = 0;

	if (!TestLaraPosition(g_PickUpScion4Bounds, item, lara_item))
	{
		return;
	}

	if (g_Input.action && g_Lara.gun_status == LGS_ARMLESS &&
		!lara_item->gravity_status && lara_item->current_anim_state == AS_STOP)
	{
		AlignLaraPosition(&g_PickUpScion4Position, item, lara_item);
		lara_item->current_anim_state = AS_PICKUP;
		lara_item->goal_anim_state = AS_PICKUP;
		lara_item->anim_number = g_Objects[O_LARA_EXTRA].anim_index;
		lara_item->frame_number = g_Anims[lara_item->anim_number].frame_base;
		g_Lara.gun_status = LGS_HANDSBUSY;
		g_Camera.type = CAM_CINEMATIC;
		g_CineFrame = 0;
		g_CinePosition = lara_item->pos;
		g_CinePosition.y_rot -= PHD_90;
	}
}
