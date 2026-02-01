#include "chain_block.h"
#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"

void ChainBlock(ITEM_INFO *item)
{
	if (g_FlipTimer == 0)
	{
		Sound_Effect(SFX_SECRET, NULL, SPM_NORMAL);
	}

	g_FlipTimer++;
	if (g_FlipTimer == 55)
	{
		Sound_Effect(SFX_LARA_SPLASH, NULL, SPM_NORMAL);
		g_FlipEffect = -1;
	}
}
