#include "pickup.h"
#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\inv.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\lara.h"
#include "..\\SPECIFIC\\overlay.h"
#include "..\\SPECIFIC\\vars.h"

PHD_VECTOR g_PickUpPosition = {0, 0, -100};
PHD_VECTOR g_PickUpPositionUW = {0, -200, -350};

int16_t g_PickUpBounds[12] = {
	-256, +256, -100, +100, -256, +256, -10 * PHD_DEGREE, +10 * PHD_DEGREE,
	0,	0,	0,	0,
};

int16_t g_PickUpBoundsUW[12] = {
	-512,
	+512,
	-512,
	+512,
	-512,
	+512,
	-45 * PHD_DEGREE,
	+45 * PHD_DEGREE,
	-45 * PHD_DEGREE,
	+45 * PHD_DEGREE,
	-45 * PHD_DEGREE,
	+45 * PHD_DEGREE,
};

void PickUpCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];
	item->pos.y_rot = lara_item->pos.y_rot;
	item->pos.z_rot = 0;

	if (g_Lara.water_status == LWS_ABOVEWATER)
	{
		item->pos.x_rot = 0;

		if (!TestLaraPosition(g_PickUpBounds, item, lara_item))
		{
			return;
		}

		if (lara_item->current_anim_state == AS_PICKUP)
		{
			// AF_PICKUP момент в анимации когда Лара склонилась
			//и протянула руку к предмету перед тем как встать
			if (lara_item->frame_number != AF_PICKUP)
			{
				return;
			}

			if (item->object_number == O_SHOTGUN_ITEM)
			{
				g_Lara.mesh_ptrs[LM_TORSO] =
					g_Meshes[g_Objects[O_SHOTGUN].mesh_index + LM_TORSO];
			}

			Overlay_AddPickup(item->object_number);
			Inv_AddItem(item->object_number);
			item->status = IS_INVISIBLE;
			RemoveDrawnItem(item_num);
			g_SaveGame.pickups++;
			return;
		}

		if (g_Input.action && g_Lara.gun_status == LGS_ARMLESS &&
			!lara_item->gravity_status &&
			lara_item->current_anim_state == AS_STOP)
		{
			AlignLaraPosition(&g_PickUpPosition, item, lara_item);
			AnimateLaraUntil(lara_item, AS_PICKUP);
			lara_item->goal_anim_state = AS_STOP;
			g_Lara.gun_status = LGS_HANDSBUSY;
			return;
		}
	}
	else if (g_Lara.water_status == LWS_UNDERWATER)
	{
		item->pos.x_rot = -25 * PHD_DEGREE;
		if (!TestLaraPosition(g_PickUpBoundsUW, item, lara_item))
		{
			return;
		}

		if (lara_item->current_anim_state == AS_PICKUP)
		{
			if (lara_item->frame_number != AF_PICKUP_UW)
			{
				return;
			}
			Overlay_AddPickup(item->object_number);
			Inv_AddItem(item->object_number);
			item->status = IS_INVISIBLE;
			RemoveDrawnItem(item_num);
			g_SaveGame.pickups++;
			return;
		}

		if (g_Input.action && lara_item->current_anim_state == AS_TREAD)
		{
			if (!MoveLaraPosition(&g_PickUpPositionUW, item, lara_item))
			{
				return;
			}
			AnimateLaraUntil(lara_item, AS_PICKUP);
			lara_item->goal_anim_state = AS_TREAD;
		}
	}
}
