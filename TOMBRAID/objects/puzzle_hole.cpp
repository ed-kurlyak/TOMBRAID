#include "puzzle_hole.h"

#include "..\\SPECIFIC\\collide.h"
//#include "game/input.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\inv.h"
#include "..\\SPECIFIC\\lara.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"
#include "keyhole.h"

PHD_VECTOR g_PuzzleHolePosition = {0, 0, WALL_L / 2 - LARA_RAD - 85};

int16_t g_PuzzleHoleBounds[12] = {
	-200,
	+200,
	0,
	0,
	WALL_L / 2 - 200,
	WALL_L / 2,
	-10 * PHD_DEGREE,
	+10 * PHD_DEGREE,
	-30 * PHD_DEGREE,
	+30 * PHD_DEGREE,
	-10 * PHD_DEGREE,
	+10 * PHD_DEGREE,
};

void SetupPuzzleHole(OBJECT_INFO *obj)
{
	obj->collision = PuzzleHoleCollision;
	obj->save_flags = 1;
}

void SetupPuzzleDone(OBJECT_INFO *obj) { obj->save_flags = 1; }

void PuzzleHoleCollision(int16_t item_num, ITEM_INFO *lara_item,
						 COLL_INFO *coll)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (lara_item->current_anim_state == AS_USEPUZZLE)
	{
		if (!TestLaraPosition(g_PuzzleHoleBounds, item, lara_item))
		{
			return;
		}

		if (lara_item->frame_number == AF_USEPUZZLE)
		{
			switch (item->object_number)
			{
			case O_PUZZLE_HOLE1:
				item->object_number = O_PUZZLE_DONE1;
				break;

			case O_PUZZLE_HOLE2:
				item->object_number = O_PUZZLE_DONE2;
				break;

			case O_PUZZLE_HOLE3:
				item->object_number = O_PUZZLE_DONE3;
				break;

			case O_PUZZLE_HOLE4:
				item->object_number = O_PUZZLE_DONE4;
				break;
			}
		}

		return;
	}
	else if (lara_item->current_anim_state != AS_STOP)
	{
		return;
	}

	if ((g_InvChosen == -1 && !g_Input.action) ||
		g_Lara.gun_status != LGS_ARMLESS || lara_item->gravity_status)
	{
		return;
	}

	if (!TestLaraPosition(g_PuzzleHoleBounds, item, lara_item))
	{
		return;
	}

	if (item->status != IS_NOT_ACTIVE)
	{
		if (lara_item->pos.x != g_PickUpX || lara_item->pos.y != g_PickUpY ||
			lara_item->pos.z != g_PickUpZ)
		{
			g_PickUpX = lara_item->pos.x;
			g_PickUpY = lara_item->pos.y;
			g_PickUpZ = lara_item->pos.z;
			Sound_Effect(SFX_LARA_NO, &lara_item->pos, SPM_NORMAL);
		}
		return;
	}

	if (g_InvChosen == -1)
	{
		Display_Inventory(INV_KEYS_MODE);
	}
	else
	{
		g_PickUpY = lara_item->pos.y - 1;
	}

	if (g_InvChosen == -1 && g_InvKeysObjects)
	{
		return;
	}

	if (g_InvChosen != -1)
	{
		g_PickUpY = lara_item->pos.y - 1;
	}

	int32_t correct = 0;
	switch (item->object_number)
	{
	case O_PUZZLE_HOLE1:
		if (g_InvChosen == O_PUZZLE_OPTION1)
		{
			Inv_RemoveItem(O_PUZZLE_OPTION1);
			correct = 1;
		}
		break;

	case O_PUZZLE_HOLE2:
		if (g_InvChosen == O_PUZZLE_OPTION2)
		{
			Inv_RemoveItem(O_PUZZLE_OPTION2);
			correct = 1;
		}
		break;

	case O_PUZZLE_HOLE3:
		if (g_InvChosen == O_PUZZLE_OPTION3)
		{
			Inv_RemoveItem(O_PUZZLE_OPTION3);
			correct = 1;
		}
		break;

	case O_PUZZLE_HOLE4:
		if (g_InvChosen == O_PUZZLE_OPTION4)
		{
			Inv_RemoveItem(O_PUZZLE_OPTION4);
			correct = 1;
		}
		break;
	}

	g_InvChosen = -1;
	if (correct)
	{
		AlignLaraPosition(&g_PuzzleHolePosition, item, lara_item);
		AnimateLaraUntil(lara_item, AS_USEPUZZLE);
		lara_item->goal_anim_state = AS_STOP;
		g_Lara.gun_status = LGS_HANDSBUSY;
		item->status = IS_ACTIVE;
		g_PickUpX = lara_item->pos.x;
		g_PickUpY = lara_item->pos.y;
		g_PickUpZ = lara_item->pos.z;
	}
	else if (lara_item->pos.x != g_PickUpX || lara_item->pos.y != g_PickUpY ||
			 lara_item->pos.z != g_PickUpZ)
	{
		Sound_Effect(SFX_LARA_NO, &lara_item->pos, SPM_NORMAL);
		g_PickUpX = lara_item->pos.x;
		g_PickUpY = lara_item->pos.y;
		g_PickUpZ = lara_item->pos.z;
	}
}

int32_t PickupTrigger(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (item->status != IS_INVISIBLE)
	{
		return 0;
	}

	item->status = IS_DEACTIVATED;

	return 1;
}
