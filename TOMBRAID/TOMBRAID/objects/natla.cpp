#include "natla.h"

#include "../effects/missile.h"
#include "..\\SPECIFIC\\box.h"
#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\lot.h"
#include "..\\SPECIFIC\\phd_math.h"
//#include "game/music.h"
#include "..\\SPECIFIC\\people.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"
#include "..\\SPECIFIC\\winmain.h"

BITE_INFO g_NatlaGun = {5, 220, 7, 4};

void SetupNatla(OBJECT_INFO *obj)
{
	if (!obj->loaded)
	{
		return;
	}
	obj->collision = CreatureCollision;
	obj->initialise = InitialiseCreature;
	obj->control = NatlaControl;
	obj->shadow_size = UNIT_SHADOW / 2;
	obj->hit_points = NATLA_HITPOINTS;
	obj->radius = NATLA_RADIUS;
	obj->smartness = NATLA_SMARTNESS;
	obj->intelligent = 1;
	obj->save_position = 1;
	obj->save_hitpoints = 1;
	obj->save_anim = 1;
	obj->save_flags = 1;
	g_AnimBones[obj->bone_index + 8] |= BEB_ROT_Z | BEB_ROT_X;
}

void SetupNatlaGun(OBJECT_INFO *obj) { obj->control = ControlNatlaGun; }

void NatlaControl(int16_t item_num)
{
	static int16_t facing = 0;
	ITEM_INFO *item = &g_Items[item_num];

	if (item->status == IS_INVISIBLE)
	{
		if (!EnableBaddieAI(item_num, 0))
		{
			return;
		}
		item->status = IS_ACTIVE;
	}

	CREATURE_INFO *natla = (CREATURE_INFO *)item->data;
	int16_t head = 0;
	int16_t angle = 0;
	int16_t tilt = 0;
	int16_t gun = natla->head_rotation * 7 / 8;
	int16_t timer = natla->flags & NATLA_TIMER;

	if (item->hit_points <= 0 && item->hit_points > DONT_TARGET)
	{
		item->goal_anim_state = NATLA_DEATH;
	}
	else if (item->hit_points <= NATLA_NEAR_DEATH)
	{
		natla->LOT.step = STEP_L;
		natla->LOT.drop = -STEP_L;
		natla->LOT.fly = 0;

		AI_INFO info;
		CreatureAIInfo(item, &info);

		if (info.ahead)
		{
			head = info.angle;
		}

		CreatureMood(item, &info, 1);

		angle = CreatureTurn(item, NATLA_RUN_TURN);

		int8_t shoot = info.angle > -NATLA_FIRE_ARC &&
					   info.angle < NATLA_FIRE_ARC && Targetable(item, &info);

		if (facing)
		{
			item->pos.y_rot += facing;
			facing = 0;
		}

		switch (item->current_anim_state)
		{
		case NATLA_FALL:
			if (item->pos.y < item->floor)
			{
				item->gravity_status = 1;
				item->speed = 0;
			}
			else
			{
				item->gravity_status = 0;
				item->goal_anim_state = NATLA_SEMIDEATH;
				item->pos.y = item->floor;
				timer = 0;
			}
			break;

		case NATLA_STAND:
			if (!shoot)
			{
				item->goal_anim_state = NATLA_RUN;
			}
			if (timer >= 20)
			{

				int16_t fx_num = CreatureEffect(item, &g_NatlaGun, ShardGun);
				if (fx_num != NO_ITEM)
				{
					FX_INFO *fx = &g_Effects[fx_num];
					gun = fx->pos.x_rot;
					Sound_Effect(SFX_ATLANTEAN_NEEDLE, &fx->pos, SPM_NORMAL);
				}

				timer = 0;
			}
			break;

		case NATLA_RUN:
			tilt = angle;
			if (timer >= 20)
			{

				int16_t fx_num = CreatureEffect(item, &g_NatlaGun, ShardGun);
				if (fx_num != NO_ITEM)
				{
					FX_INFO *fx = &g_Effects[fx_num];
					gun = fx->pos.x_rot;
					Sound_Effect(SFX_ATLANTEAN_NEEDLE, &fx->pos, SPM_NORMAL);
				}

				timer = 0;
			}
			if (shoot)
			{
				item->goal_anim_state = NATLA_STAND;
			}
			break;

		case NATLA_SEMIDEATH:
			if (timer == NATLA_DIE_TIME)
			{
				item->goal_anim_state = NATLA_STAND;
				natla->flags = 0;
				timer = 0;
				item->hit_points = NATLA_NEAR_DEATH;
				// Music_Play(54);
			}
			else
			{
				item->hit_points = DONT_TARGET;
			}
			break;

		case NATLA_FLY:
			item->goal_anim_state = NATLA_FALL;
			timer = 0;
			break;

		case NATLA_STOP:
		case NATLA_AIM:
		case NATLA_SHOOT:
			item->goal_anim_state = NATLA_SEMIDEATH;
			item->flags = 0;
			timer = 0;
			break;
		}
	}
	else
	{
		natla->LOT.step = STEP_L;
		natla->LOT.drop = -STEP_L;
		natla->LOT.fly = 0;

		AI_INFO info;
		CreatureAIInfo(item, &info);

		int8_t shoot = info.angle > -NATLA_FIRE_ARC &&
					   info.angle < NATLA_FIRE_ARC && Targetable(item, &info);
		if (item->current_anim_state == NATLA_FLY &&
			(natla->flags & NATLA_FLY_MODE))
		{
			if (shoot && Random_GetControl() < NATLA_LAND_CHANCE)
			{
				natla->flags &= ~NATLA_FLY_MODE;
			}
			if (!(natla->flags & NATLA_FLY_MODE))
			{
				CreatureMood(item, &info, 1);
			}
			natla->LOT.step = WALL_L * 20;
			natla->LOT.drop = -WALL_L * 20;
			natla->LOT.fly = STEP_L / 8;
			CreatureAIInfo(item, &info);
		}
		else if (!shoot)
		{
			natla->flags |= NATLA_FLY_MODE;
		}

		if (info.ahead)
		{
			head = info.angle;
		}

		if (item->current_anim_state != NATLA_FLY ||
			(natla->flags & NATLA_FLY_MODE))
		{
			CreatureMood(item, &info, 0);
		}

		item->pos.y_rot -= facing;
		angle = CreatureTurn(item, NATLA_FLY_TURN);

		if (item->current_anim_state == NATLA_FLY)
		{
			if (info.angle > NATLA_FLY_TURN)
			{
				facing += NATLA_FLY_TURN;
			}
			else if (info.angle < -NATLA_FLY_TURN)
			{
				facing -= NATLA_FLY_TURN;
			}
			else
			{
				facing += info.angle;
			}
			item->pos.y_rot += facing;
		}
		else
		{
			item->pos.y_rot += facing - angle;
			facing = 0;
		}

		switch (item->current_anim_state)
		{
		case NATLA_STOP:
			timer = 0;
			if (natla->flags & NATLA_FLY_MODE)
			{
				item->goal_anim_state = NATLA_FLY;
			}
			else
			{
				item->goal_anim_state = NATLA_AIM;
			}
			break;

		case NATLA_FLY:
			if (!(natla->flags & NATLA_FLY_MODE) && item->pos.y == item->floor)
			{
				item->goal_anim_state = NATLA_STOP;
			}
			if (timer >= 30)
			{

				int16_t fx_num = CreatureEffect(item, &g_NatlaGun, RocketGun);
				if (fx_num != NO_ITEM)
				{
					FX_INFO *fx = &g_Effects[fx_num];
					gun = fx->pos.x_rot;
					Sound_Effect(SFX_ATLANTEAN_NEEDLE, &fx->pos, SPM_NORMAL);
				}

				timer = 0;
			}
			break;

		case NATLA_AIM:
			if (item->required_anim_state)
			{
				item->goal_anim_state = item->required_anim_state;
			}
			else if (shoot)
			{
				item->goal_anim_state = NATLA_SHOOT;
			}
			else
			{
				item->goal_anim_state = NATLA_STOP;
			}
			break;

		case NATLA_SHOOT:
			if (!item->required_anim_state)
			{
				int16_t fx_num = CreatureEffect(item, &g_NatlaGun, RocketGun);
				if (fx_num != NO_ITEM)
				{
					FX_INFO *fx = &g_Effects[fx_num];
					gun = fx->pos.x_rot;
				}
				fx_num = CreatureEffect(item, &g_NatlaGun, RocketGun);
				if (fx_num != NO_ITEM)
				{
					FX_INFO *fx = &g_Effects[fx_num];
					fx->pos.y_rot += (Random_GetControl() - 0x4000) / 4;
				}
				fx_num = CreatureEffect(item, &g_NatlaGun, RocketGun);
				if (fx_num != NO_ITEM)
				{
					FX_INFO *fx = &g_Effects[fx_num];
					fx->pos.y_rot += (Random_GetControl() - 0x4000) / 4;
				}

				item->required_anim_state = NATLA_STOP;
			}
			break;
		}
	}

	CreatureTilt(item, tilt);

	natla->neck_rotation = -head;
	if (gun)
	{
		natla->head_rotation = gun;
	}

	timer++;
	natla->flags &= ~NATLA_TIMER;
	natla->flags |= timer & NATLA_TIMER;

	item->pos.y_rot -= facing;
	CreatureAnimation(item_num, angle, 0);
	item->pos.y_rot += facing;
}

void ControlNatlaGun(int16_t fx_num)
{
	FX_INFO *fx = &g_Effects[fx_num];
	OBJECT_INFO *object = &g_Objects[fx->object_number];

	fx->frame_number--;
	if (fx->frame_number <= object->nmeshes)
	{
		KillEffect(fx_num);
	}

	if (fx->frame_number == -1)
	{
		return;
	}

	int32_t z = fx->pos.z + ((fx->speed * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT);
	int32_t x = fx->pos.x + ((fx->speed * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT);
	int32_t y = fx->pos.y;
	int16_t room_num = fx->room_number;
	FLOOR_INFO *floor = GetFloor(x, y, z, &room_num);

	if (y >= GetHeight(floor, x, y, z) || y <= GetCeiling(floor, x, y, z))
	{
		return;
	}

	fx_num = CreateEffect(room_num);
	if (fx_num != NO_ITEM)
	{
		FX_INFO *newfx = &g_Effects[fx_num];
		newfx->pos.x = x;
		newfx->pos.y = y;
		newfx->pos.z = z;
		newfx->pos.y_rot = fx->pos.y_rot;
		newfx->room_number = room_num;
		newfx->speed = fx->speed;
		newfx->frame_number = 0;
		newfx->object_number = O_MISSILE1;
	}
}
