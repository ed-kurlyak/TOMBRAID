#include "misc.h"

#include "..\\SPECIFIC\\collide.h"
#include "..\\SPECIFIC\\draw.h"
#include "..\\objects\cog.h"
#include "..\\objects\pickup.h"

void SetupCameraTarget(OBJECT_INFO *obj)
{
    obj->draw_routine = DrawDummyItem;
}

void SetupMovingBar(OBJECT_INFO *obj)
{
    obj->control = CogControl;
    obj->collision = ObjectCollision;
    obj->save_flags = 1;
    obj->save_anim = 1;
    obj->save_position = 1;
}

void SetupLeadBar(OBJECT_INFO *obj)
{
    obj->draw_routine = DrawPickupItem;
    obj->collision = PickUpCollision;
    obj->save_flags = 1;
}
