#include "splash.h"

#include "..\\SPECIFIC\\control_util.h"
#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\phd_math.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"
#include "..\\SPECIFIC\\winmain.h"

void SetupSplash(OBJECT_INFO *obj) { obj->control = ControlSplash1; }

void Splash(ITEM_INFO *item)
{
	int16_t wh = GetWaterHeight(item->pos.x, item->pos.y, item->pos.z,
								item->room_number);
	int16_t room_num = item->room_number;
	GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_num);

	Sound_Effect(SFX_LARA_SPLASH, &item->pos, SPM_NORMAL);

	for (int i = 0; i < 10; i++)
	{
		int16_t fx_num = CreateEffect(room_num);
		if (fx_num != NO_ITEM)
		{
			FX_INFO *fx = &g_Effects[fx_num];
			fx->pos.x = item->pos.x;
			fx->pos.y = wh;
			fx->pos.z = item->pos.z;
			fx->pos.y_rot = PHD_180 + 2 * Random_GetDraw();
			fx->object_number = O_SPLASH1;
			fx->frame_number = 0;
			fx->speed = Random_GetDraw() / 256;
		}
	}
}

void ControlSplash1(int16_t fx_num)
{
	FX_INFO *fx = &g_Effects[fx_num];
	fx->frame_number--;
	if (fx->frame_number <= g_Objects[fx->object_number].nmeshes)
	{
		KillEffect(fx_num);
		return;
	}

	fx->pos.z += (phd_cos(fx->pos.y_rot) * fx->speed) >> W2V_SHIFT;
	fx->pos.x += (phd_sin(fx->pos.y_rot) * fx->speed) >> W2V_SHIFT;
}
