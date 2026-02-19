#include "types.h"

extern int16_t* (*S_DrawObjectGT4)(int16_t* obj_ptr, int32_t number);
extern int16_t* (*S_DrawObjectGT3)(int16_t* obj_ptr, int32_t number);
extern int16_t* (*S_DrawObjectG4)(int16_t* obj_ptr, int32_t number);
extern int16_t* (*S_DrawObjectG3)(int16_t* obj_ptr, int32_t number);

extern void (*S_Output_DrawShadow)(PHD_VBUF* vbufs, int clip, int vertex_count);
extern void (*S_Output_DrawSprite)(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int z, int sprnum, int shade);
extern void (*S_Output_DrawScreenFBox)(int32_t sx, int32_t sy, int32_t w, int32_t h);
extern void (*S_Output_DrawScreenFlatQuad)(int32_t sx, int32_t sy, int32_t w, int32_t h, RGB888 color, int depth);
extern void (*S_Output_DrawTriangle)(VBUF2* vertices, int vert_count, int depth);
extern void (*S_Output_DrawLine)(VBUF2* vertices, int depth);
extern void (*S_InitialisePolyList)();
extern void (*S_OutputPolyList)();

