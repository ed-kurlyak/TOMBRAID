#include "blood.h"

#include "..\\SPECIFIC\\phd_math.h"
//#include "game/items.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\vars.h"

/*
void SetupBlood(OBJECT_INFO *obj)
{
	obj->control = ControlBlood1;
}
*/

int16_t DoBloodSplat(int32_t x, int32_t y, int32_t z, int16_t speed,
					 int16_t direction, int16_t room_num)
{
	int16_t fx_num = CreateEffect(room_num);

	if (fx_num != NO_ITEM)
	{
		FX_INFO *fx = &g_Effects[fx_num];
		fx->pos.x = x;
		fx->pos.y = y;
		fx->pos.z = z;
		fx->pos.y_rot = direction;
		fx->object_number = O_BLOOD1;
		fx->frame_number = 0;
		fx->counter = 0;
		fx->speed = speed;
	}
	return fx_num;
}

void ControlBlood1(int16_t fx_num)
{
	FX_INFO *fx = &g_Effects[fx_num];

	fx->pos.x += (phd_sin(fx->pos.y_rot) * fx->speed) >> W2V_SHIFT;
	fx->pos.z += (phd_cos(fx->pos.y_rot) * fx->speed) >> W2V_SHIFT;

	fx->counter++;

	if (fx->counter == 4)
	{
		fx->counter = 0;
		fx->frame_number--;

		if (fx->frame_number <= g_Objects[fx->object_number].nmeshes)
		{
			KillEffect(fx_num);
		}
	}
}
