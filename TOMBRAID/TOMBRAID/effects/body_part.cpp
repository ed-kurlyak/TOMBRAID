#include "body_part.h"

#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\matrix.h"
#include "..\\SPECIFIC\\phd_math.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"
#include "..\\SPECIFIC\\winmain.h"

void SetupBodyPart(OBJECT_INFO *obj)
{
	obj->control = ControlBodyPart;
	obj->nmeshes = 0;
	obj->loaded = 1;
}

int32_t ExplodingDeath(int16_t item_num, int32_t mesh_bits, int16_t damage)
{
	ITEM_INFO *item = &g_Items[item_num];
	OBJECT_INFO *object = &g_Objects[item->object_number];
	int32_t abortion = item->object_number == O_ABORTION;

	int16_t *frame = GetBestFrame(item);

	phd_PushUnitMatrix();
	g_PhdMatrixPtr->_03 = 0;
	g_PhdMatrixPtr->_13 = 0;
	g_PhdMatrixPtr->_23 = 0;

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel(frame[FRAME_POS_X], frame[FRAME_POS_Y],
					 frame[FRAME_POS_Z]);

	int32_t *packed_rotation = (int32_t *)(frame + FRAME_ROT);
	phd_RotYXZpack(*packed_rotation++);

	int32_t *bone = &g_AnimBones[object->bone_index];
#if 0
    // XXX: present in OG, removed by GLrage on the grounds that it sometimes
    // crashes.
    int16_t *extra_rotation = (int16_t*)item->data;
#endif

	int32_t bit = 1;
	if ((bit & mesh_bits) && (bit & item->mesh_bits))
	{
		int16_t fx_num = CreateEffect(item->room_number);
		if (fx_num != NO_ITEM)
		{
			FX_INFO *fx = &g_Effects[fx_num];
			fx->room_number = item->room_number;
			fx->pos.x = (g_PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
			fx->pos.y = (g_PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
			fx->pos.z = (g_PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
			fx->pos.y_rot = (Random_GetControl() - 0x4000) * 2;
			if (abortion)
			{
				fx->speed = Random_GetControl() >> 7;
				fx->fall_speed = -Random_GetControl() >> 7;
			}
			else
			{
				fx->speed = Random_GetControl() >> 8;
				fx->fall_speed = -Random_GetControl() >> 8;
			}
			fx->counter = damage;
			fx->frame_number = object->mesh_index;
			fx->object_number = O_BODY_PART;
		}
		item->mesh_bits -= bit;
	}

	for (int i = 1; i < object->nmeshes; i++)
	{
		int32_t bone_extra_flags = *bone++;
		if (bone_extra_flags & BEB_POP)
		{
			phd_PopMatrix();
		}
		if (bone_extra_flags & BEB_PUSH)
		{
			phd_PushMatrix();
		}

		phd_TranslateRel(bone[0], bone[1], bone[2]);
		phd_RotYXZpack(*packed_rotation++);

#if 0
    if (extra_rotation) {
        if (bone_extra_flags & (BEB_ROT_X | BEB_ROT_Y | BEB_ROT_Z)) {
            if (bone_extra_flags & BEB_ROT_Y) {
                phd_RotY(*extra_rotation++);
            }
            if (bone_extra_flags & BEB_ROT_X) {
                phd_RotX(*extra_rotation++);
            }
            if (bone_extra_flags & BEB_ROT_Z) {
                phd_RotZ(*extra_rotation++);
            }
        }
    }
#endif

		bit <<= 1;
		if ((bit & mesh_bits) && (bit & item->mesh_bits))
		{
			int16_t fx_num = CreateEffect(item->room_number);
			if (fx_num != NO_ITEM)
			{
				FX_INFO *fx = &g_Effects[fx_num];
				fx->room_number = item->room_number;
				fx->pos.x = (g_PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
				fx->pos.y = (g_PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
				fx->pos.z = (g_PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
				fx->pos.y_rot = (Random_GetControl() - 0x4000) * 2;
				if (abortion)
				{
					fx->speed = Random_GetControl() >> 7;
					fx->fall_speed = -Random_GetControl() >> 7;
				}
				else
				{
					fx->speed = Random_GetControl() >> 8;
					fx->fall_speed = -Random_GetControl() >> 8;
				}
				fx->counter = damage;
				fx->object_number = O_BODY_PART;
				fx->frame_number = object->mesh_index + i;
			}
			item->mesh_bits -= bit;
		}

		bone += 3;
	}

	phd_PopMatrix();

	return !(item->mesh_bits & (0x7FFFFFFF >> (31 - object->nmeshes)));
}

void ControlBodyPart(int16_t fx_num)
{
	FX_INFO *fx = &g_Effects[fx_num];
	fx->pos.x_rot += 5 * PHD_DEGREE;
	fx->pos.z_rot += 10 * PHD_DEGREE;
	fx->pos.z += (fx->speed * phd_cos(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->pos.x += (fx->speed * phd_sin(fx->pos.y_rot)) >> W2V_SHIFT;
	fx->fall_speed += GRAVITY;
	fx->pos.y += fx->fall_speed;

	int16_t room_num = fx->room_number;
	FLOOR_INFO *floor = GetFloor(fx->pos.x, fx->pos.y, fx->pos.z, &room_num);

	int32_t ceiling = GetCeiling(floor, fx->pos.x, fx->pos.y, fx->pos.z);
	if (fx->pos.y < ceiling)
	{
		fx->fall_speed = -fx->fall_speed;
		fx->pos.y = ceiling;
	}

	int32_t height = GetHeight(floor, fx->pos.x, fx->pos.y, fx->pos.z);
	if (fx->pos.y >= height)
	{
		if (fx->counter)
		{
			fx->speed = 0;
			fx->frame_number = 0;
			fx->counter = 0;
			fx->object_number = O_EXPLOSION1;
			Sound_Effect(SFX_ATLANTEAN_EXPLODE, &fx->pos, SPM_NORMAL);
		}
		else
		{
			KillEffect(fx_num);
		}
		return;
	}

	if (ItemNearLara(&fx->pos, fx->counter * 2))
	{
		g_LaraItem->hit_points -= fx->counter;
		g_LaraItem->hit_status = 1;

		if (fx->counter)
		{
			fx->speed = 0;
			fx->frame_number = 0;
			fx->counter = 0;
			fx->object_number = O_EXPLOSION1;
			Sound_Effect(SFX_ATLANTEAN_EXPLODE, &fx->pos, SPM_NORMAL);

			g_Lara.spaz_effect_count = 5;
			g_Lara.spaz_effect = fx;
		}
		else
		{
			KillEffect(fx_num);
		}
	}

	if (room_num != fx->room_number)
	{
		EffectNewRoom(fx_num, room_num);
	}
}
