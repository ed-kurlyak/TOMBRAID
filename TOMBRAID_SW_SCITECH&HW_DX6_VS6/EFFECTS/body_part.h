#ifndef _BODY_PART_
#define _BODY_PART_

#include "..\\SPECIFIC\\types.h"

//#include <stdint.h>

void SetupBodyPart(OBJECT_INFO *obj);
int32_t ExplodingDeath(int16_t item_num, int32_t mesh_bits, int16_t damage);
void ControlBodyPart(int16_t fx_num);

#endif