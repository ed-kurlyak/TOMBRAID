#pragma once

#include "..\\SPECIFIC\\types.h"

//#include <stdint.h>

typedef enum
{
	DART_EMITTER_IDLE = 0,
	DART_EMITTER_FIRE = 1,
} DART_EMITTER_STATE;

// void SetupDartEmitter(OBJECT_INFO *obj);
void DartEmitterControl(int16_t item_num);
