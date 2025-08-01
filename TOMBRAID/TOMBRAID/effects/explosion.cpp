#include "explosion.h"

#include "..\\SPECIFIC\\items.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"

void SetupExplosion(OBJECT_INFO *obj) { obj->control = ControlExplosion1; }

void ControlExplosion1(int16_t fx_num)
{
	FX_INFO *fx = &g_Effects[fx_num];
	fx->counter++;
	if (fx->counter == 2)
	{
		fx->counter = 0;
		fx->frame_number--;
		if (fx->frame_number <= g_Objects[fx->object_number].nmeshes)
		{
			KillEffect(fx_num);
		}
	}
}

void Explosion(ITEM_INFO *item)
{

	Sound_Effect(SFX_EXPLOSION_FX, NULL, SPM_NORMAL);
	g_Camera.bounce = -75;
	g_FlipEffect = -1;
}
