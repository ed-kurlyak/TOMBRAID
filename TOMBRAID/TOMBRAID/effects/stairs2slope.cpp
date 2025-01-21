#include <windows.h>
#include "stairs2slope.h"


#include "..\\SPECIFIC\\sound.h"
#include "..\\SPECIFIC\\vars.h"

void Stairs2Slope(ITEM_INFO *item)
{
	
    if (g_FlipTimer == 5) {
        Sound_Effect(SFX_STAIRS2SLOPE_FX, NULL, SPM_NORMAL);
        g_FlipEffect = -1;
    }
    g_FlipTimer++;
	
}
