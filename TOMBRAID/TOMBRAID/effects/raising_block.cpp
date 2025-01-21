#include "raising_block.h"

#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"

void RaisingBlock(ITEM_INFO *item)
{
	
    Sound_Effect(SFX_RAISINGBLOCK_FX, NULL, SPM_NORMAL);
    g_FlipEffect = -1;
	
}
