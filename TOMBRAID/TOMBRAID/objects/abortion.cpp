#include "abortion.h"

#include "..\\SPECIFIC\\box.h"
#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\lot.h"
#include "..\\SPECIFIC\\phd_math.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"
#include "..\\SPECIFIC\\winmain.h"
#include "..\\effects\\body_part.h"

void SetupAbortion(OBJECT_INFO *obj)
{
	if (!obj->loaded)
	{
		return;
	}
	obj->initialise = InitialiseCreature;
	obj->control = AbortionControl;
	obj->collision = CreatureCollision;
	obj->shadow_size = UNIT_SHADOW / 3;
	obj->hit_points = ABORTION_HITPOINTS;
	obj->radius = ABORTION_RADIUS;
	obj->smartness = ABORTION_SMARTNESS;
	obj->intelligent = 1;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_anim = 1;
	obj->save_flags = 1;
	g_AnimBones[obj->bone_index + 4] |= BEB_ROT_Y;
}

void AbortionControl(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (item->status == IS_INVISIBLE)
	{
		if (!EnableBaddieAI(item_num, 0))
		{
			return;
		}
		item->status = IS_ACTIVE;
	}

	CREATURE_INFO *abortion = (CREATURE_INFO *)item->data;
	int16_t head = 0;
	int16_t angle = 0;

	if (item->hit_points <= 0)
	{
		if (item->current_anim_state != ABORTION_DEATH)
		{
			item->current_anim_state = ABORTION_DEATH;
			item->anim_number =
				g_Objects[O_ABORTION].anim_index + ABORTION_DIE_ANIM;
			item->frame_number = g_Anims[item->anim_number].frame_base;
		}
	}
	else
	{
		AI_INFO info;
		CreatureAIInfo(item, &info);

		if (info.ahead)
		{
			head = info.angle;
		}

		CreatureMood(item, &info, 1);

		angle = phd_atan(abortion->target.z - item->pos.z,
						 abortion->target.x - item->pos.x) -
				item->pos.y_rot;

		if (item->touch_bits)
		{
			g_LaraItem->hit_points -= ABORTION_TOUCH_DAMAGE;
			g_LaraItem->hit_status = 1;
		}

		switch (item->current_anim_state)
		{
		case ABORTION_SET:
			item->goal_anim_state = ABORTION_FALL;
			item->gravity_status = 1;
			break;

		case ABORTION_STOP:
			if (g_LaraItem->hit_points <= 0)
			{
				break;
			}

			abortion->flags = 0;
			if (angle > ABORTION_NEED_TURN)
			{
				item->goal_anim_state = ABORTION_TURN_R;
			}
			else if (angle < -ABORTION_NEED_TURN)
			{
				item->goal_anim_state = ABORTION_TURN_L;
			}
			else if (info.distance >= ABORTION_ATTACK_RANGE)
			{
				item->goal_anim_state = ABORTION_FORWARD;
			}
			else if (g_LaraItem->hit_points > ABORTION_ATTACK_DAMAGE)
			{
				if (Random_GetControl() < 0x4000)
				{
					item->goal_anim_state = ABORTION_ATTACK1;
				}
				else
				{
					item->goal_anim_state = ABORTION_ATTACK2;
				}
			}
			else if (info.distance < ABORTION_CLOSE_RANGE)
			{
				item->goal_anim_state = ABORTION_ATTACK3;
			}
			else
			{
				item->goal_anim_state = ABORTION_FORWARD;
			}
			break;

		case ABORTION_FORWARD:
			if (angle < -ABORTION_TURN)
			{
				item->goal_anim_state -= ABORTION_TURN;
			}
			else if (angle > ABORTION_TURN)
			{
				item->goal_anim_state += ABORTION_TURN;
			}
			else
			{
				item->goal_anim_state += angle;
			}

			if (angle > ABORTION_NEED_TURN || angle < -ABORTION_NEED_TURN)
			{
				item->goal_anim_state = ABORTION_STOP;
			}
			else if (info.distance < ABORTION_ATTACK_RANGE)
			{
				item->goal_anim_state = ABORTION_STOP;
			}
			break;

		case ABORTION_TURN_L:
			if (!abortion->flags)
			{
				abortion->flags = item->frame_number;
			}
			else if (item->frame_number - abortion->flags > 13 &&
					 item->frame_number - abortion->flags < 23)
			{
				item->pos.y_rot -= PHD_DEGREE * 9;
			}

			if (angle > -ABORTION_NEED_TURN)
			{
				item->goal_anim_state = ABORTION_STOP;
			}
			break;

		case ABORTION_TURN_R:
			if (!abortion->flags)
			{
				abortion->flags = item->frame_number;
			}
			else if (item->frame_number - abortion->flags > 16 &&
					 item->frame_number - abortion->flags < 23)
			{
				item->pos.y_rot += PHD_DEGREE * 14;
			}

			if (angle < ABORTION_NEED_TURN)
			{
				item->goal_anim_state = ABORTION_STOP;
			}
			break;

		case ABORTION_ATTACK1:
			if (!abortion->flags && (item->touch_bits & ABORTION_TRIGHT))
			{
				g_LaraItem->hit_points -= ABORTION_ATTACK_DAMAGE;
				g_LaraItem->hit_status = 1;
				abortion->flags = 1;
			}
			break;

		case ABORTION_ATTACK2:
			if (!abortion->flags && (item->touch_bits & ABORTION_TOUCH))
			{
				g_LaraItem->hit_points -= ABORTION_ATTACK_DAMAGE;
				g_LaraItem->hit_status = 1;
				abortion->flags = 1;
			}
			break;

		case ABORTION_ATTACK3:
			if ((item->touch_bits & ABORTION_TRIGHT) ||
				g_LaraItem->hit_points <= 0)
			{
				item->goal_anim_state = ABORTION_KILL;

				g_LaraItem->anim_number = g_Objects[O_LARA_EXTRA].anim_index;
				g_LaraItem->frame_number =
					g_Anims[g_LaraItem->anim_number].frame_base;
				g_LaraItem->current_anim_state = AS_SPECIAL;
				g_LaraItem->goal_anim_state = AS_SPECIAL;
				g_LaraItem->room_number = item->room_number;
				g_LaraItem->pos.x = item->pos.x;
				g_LaraItem->pos.y = item->pos.y;
				g_LaraItem->pos.z = item->pos.z;
				g_LaraItem->pos.x_rot = 0;
				g_LaraItem->pos.y_rot = item->pos.y_rot;
				g_LaraItem->pos.z_rot = 0;
				g_LaraItem->gravity_status = 0;
				g_LaraItem->hit_points = -1;
				g_Lara.air = -1;
				g_Lara.gun_status = LGS_HANDSBUSY;
				g_Lara.gun_type = LGT_UNARMED;

				g_Camera.target_distance = WALL_L * 2;
				g_Camera.flags = FOLLOW_CENTRE;
			}
			break;

		case ABORTION_KILL:
			g_Camera.target_distance = WALL_L * 2;
			g_Camera.flags = FOLLOW_CENTRE;
			break;
		}
	}

	CreatureHead(item, head);

	if (item->current_anim_state == ABORTION_FALL)
	{
		AnimateItem(item);

		if (item->pos.y > item->floor)
		{
			item->goal_anim_state = ABORTION_STOP;
			item->gravity_status = 0;
			item->pos.y = item->floor;
			g_Camera.bounce = 500;
		}
	}
	else
	{
		CreatureAnimation(item_num, 0, 0);
	}

	if (item->status == IS_DEACTIVATED)
	{
		Sound_Effect(SFX_ATLANTEAN_DEATH, &item->pos, SPM_NORMAL);
		ExplodingDeath(item_num, -1, ABORTION_PART_DAMAGE);
		FLOOR_INFO *floor =
			GetFloor(item->pos.x, item->pos.y, item->pos.z, &item->room_number);
		GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);
		TestTriggers(g_TriggerIndex, 1);

		KillItem(item_num);
		item->status = IS_DEACTIVATED;
	}
}
