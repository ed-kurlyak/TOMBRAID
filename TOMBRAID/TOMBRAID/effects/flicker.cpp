#include "flicker.h"


#include "..\\SPECIFIC\\control_util.h"
//#include "game/sound.h"
#include "..\\SPECIFIC\\vars.h"
#include "..\\SPECIFIC\\draw.h"

void Flicker(ITEM_INFO *item)
{
	
    if (g_FlipTimer > 125) {
        FlipMap();
        g_FlipEffect = -1;
    } else if (
        g_FlipTimer == 90 || g_FlipTimer == 92 || g_FlipTimer == 105
        || g_FlipTimer == 107) {
        FlipMap();
    }
    g_FlipTimer++;
}
