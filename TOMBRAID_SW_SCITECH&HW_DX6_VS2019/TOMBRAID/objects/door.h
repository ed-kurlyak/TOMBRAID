#ifndef _DOOR_
#define _DOOR_


#include "..\\SPECIFIC\\types.h"

void InitialiseDoor(int16_t item_num);
void DoorControl(int16_t item_num);
void OpenNearestDoors(ITEM_INFO *lara_item);

#endif
