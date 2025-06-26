#pragma once

//#include <stdint.h>

#include "types.h"

extern TEXTSTRING* m_AmmoText;
extern TEXTSTRING* m_FPSText;

void Overlay_Init();

int32_t Screen_GetRenderScaleGLRage(int32_t unit);

void Overlay_DrawHealthBar();
void Overlay_DrawAirBar();
//void Overlay_DrawEnemyBar();
//void Overlay_DrawAmmoInfo();
void Overlay_DrawPickups();
void Overlay_DrawFPSInfo();
void Overlay_DrawGameInfo();

void Overlay_AddPickup(int16_t object_num);

void Overlay_MakeAmmoString(char *string);
