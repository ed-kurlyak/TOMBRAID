#pragma once

#include "..\\SPECIFIC\\types.h"

//#include <stdint.h>

#define FALLING_CEILING_DAMAGE 300

void SetupFallingCeilling(OBJECT_INFO *obj);
void FallingCeilingControl(int16_t item_num);
