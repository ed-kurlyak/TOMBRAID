#pragma once

#include "types.h"

//#include <stdbool.h>
//#include <stdint.h>

// bool Screen_SetResIdx(int32_t idx);
// bool Screen_SetPrevRes();
// bool Screen_SetNextRes();

// int32_t Screen_GetResIdx();
int32_t Screen_GetResWidth();
int32_t Screen_GetResHeight();
int32_t Screen_GetResWidthDownscaled();
int32_t Screen_GetResHeightDownscaled();

// int32_t Screen_GetPendingResIdx();
// int32_t Screen_GetPendingResWidth();
// int32_t Screen_GetPendingResHeight();

int32_t Screen_GetRenderScale(int32_t unit);
// int32_t Screen_GetRenderScaleGLRage(int32_t unit);

// void Screen_ApplyResolution();

int32_t ViewPort_GetCenterX();
int32_t ViewPort_GetCenterY();