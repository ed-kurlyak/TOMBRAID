#include "misc.h"

#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\objects\cog.h"
#include "..\\objects\pickup.h"

void SetupCameraTarget(OBJECT_INFO *obj)
{
	obj->draw_routine = DrawDummyItem;
}

