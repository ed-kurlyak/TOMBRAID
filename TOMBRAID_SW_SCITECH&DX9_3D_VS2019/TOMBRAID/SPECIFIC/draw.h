#pragma once

#include "types.h"
#include "vars.h"

void DrawAnimatingItem(ITEM_INFO *item);
int32_t GetFrames(ITEM_INFO *item, int16_t *frmptr[], int32_t *rate);
void Output_DrawShadow(int16_t size, int16_t *bptr, ITEM_INFO *item);
int S_GetObjectBounds(int16_t *bptr);
void CalculateObjectLighting(ITEM_INFO *item, int16_t *frame);
void Output_DrawPolygons(int16_t *obj_ptr, int clip);
void Output_DrawPolygons_I(int16_t *obj_ptr, int32_t clip);
int16_t *Output_CalcObjectVertices(int16_t *obj_ptr);
int16_t *Output_CalcVerticeLight(int16_t *obj_ptr);
int32_t ZedClipper(int32_t vertex_count, POINT_INFO *pts, VBUF *vertices);
int32_t ClipVertices(int32_t num, VBUF *source);
int32_t ClipVertices2(int32_t num, VBUF2 *source);
void Output_CalculateStaticLight(int16_t adder);
void Output_CalculateLight(int32_t x, int32_t y, int32_t z, int16_t room_num);
void DrawDummyItem(ITEM_INFO *item);
void DrawEvilLara(ITEM_INFO *item);
void ControlEvilLara(int16_t item_num);
void InitialiseEvilLara(int16_t item_num);
void AnimateItem(ITEM_INFO *item);
int16_t GetHeight(FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z);
void TestTriggers(int16_t *data, int32_t heavy);
void DrawLara(ITEM_INFO *item);
void RefreshCamera(int16_t type, int16_t *data);
void FlipMap();
void DrawHair();
void DrawGunFlash(int32_t weapon_type, int32_t clip);
void TranslateItem(ITEM_INFO *item, int32_t x, int32_t y, int32_t z);
void DrawLaraInt(ITEM_INFO *item, int16_t *frame1, int16_t *frame2,
				 int32_t frac, int32_t rate);
void RemoveRoomFlipItems(ROOM_INFO *r);
void AddRoomFlipItems(ROOM_INFO *r);
int16_t *GetBoundsAccurate(ITEM_INFO *item);
int16_t *GetBestFrame(ITEM_INFO *item);
int16_t *CalcRoomVertices(int16_t *obj_ptr);
int32_t TriggerActive(ITEM_INFO *item);
void DrawUnclippedItem(ITEM_INFO *item);
void DrawPickupItem(ITEM_INFO *item);
void DrawSpriteItem(ITEM_INFO *item);
void Output_DrawScreenSprite(int32_t sx, int32_t sy, int32_t z, int32_t scale_h,
							 int32_t scale_v, int32_t sprnum, int16_t shade,
							 uint16_t flags);
void Output_DrawScreenSprite2D(int32_t sx, int32_t sy, int32_t z,
							   int32_t scale_h, int32_t scale_v, int32_t sprnum,
							   int16_t shade, uint16_t flags, int32_t page);
void Output_DrawSprite(int32_t x, int32_t y, int32_t z, int16_t sprnum,
					   int16_t shade);
void Output_DrawUISprite(int32_t x, int32_t y, int32_t scale, int16_t sprnum,
						 int16_t shade);
void S_Output_DrawScreenBox(int32_t sx, int32_t sy, int32_t w, int32_t h);
void S_AnimateTextures(int32_t ticks);
void Output_CalcWibbleTable();
void DrawEffect(int16_t fxnum);
void Output_DrawLightningSegment(int32_t x1, int32_t y1, int32_t z1, int32_t x2,
								 int32_t y2, int32_t z2, int32_t width);
void S_Output_DrawLightningSegment(int x1, int y1, int z1, int thickness1,
								   int x2, int y2, int z2, int thickness2);
void Output_DrawScreenLine(int32_t sx, int32_t sy, int32_t w, int32_t h,
						   int color);
void Output_DrawSpriteRel(int32_t x, int32_t y, int32_t z, int16_t sprnum,
						  int16_t shade);
int16_t* DrawRoomSprites(int16_t* obj_ptr, int32_t number);

//---------------------------------------------
//	software section funcs
//---------------------------------------------

int16_t* S_DrawObjectGT4_SW(int16_t* obj_ptr, int32_t number);
int16_t* S_DrawObjectGT3_SW(int16_t* obj_ptr, int32_t number);
int16_t* S_DrawObjectG4_SW(int16_t* obj_ptr, int32_t number);
int16_t* S_DrawObjectG3_SW(int16_t* obj_ptr, int32_t number);
void S_Output_DrawShadow_SW(PHD_VBUF* vbufs, int clip, int vertex_count);
void S_Output_DrawSprite_SW(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int z,
	int sprnum, int shade);
void S_Output_DrawScreenFBox_SW(int32_t sx, int32_t sy, int32_t w, int32_t h);
void S_Output_DrawScreenFlatQuad_SW(int32_t sx, int32_t sy, int32_t w, int32_t h,
	RGB888 color, int depth);
void S_Output_DrawTriangle_SW(VBUF2* vertices, int vert_count, int depth);
void S_Output_DrawLine_SW(VBUF2* vertices, int depth);

//---------------------------------------------
//	hardware section funcs
//---------------------------------------------

int16_t* S_DrawObjectGT4_HW(int16_t* obj_ptr, int32_t number);
int16_t* S_DrawObjectGT3_HW(int16_t* obj_ptr, int32_t number);
int16_t* S_DrawObjectG4_HW(int16_t* obj_ptr, int32_t number);
int16_t* S_DrawObjectG3_HW(int16_t* obj_ptr, int32_t number);
void S_Output_DrawShadow_HW(PHD_VBUF* vbufs, int clip, int vertex_count);
void S_Output_DrawSprite_HW(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int z, int sprnum, int shade);
void S_Output_DrawScreenFBox_HW(int32_t sx, int32_t sy, int32_t w, int32_t h);
void S_Output_DrawScreenFlatQuad_HW(int32_t sx, int32_t sy, int32_t w, int32_t h, RGB888 color, int depth);
void S_Output_DrawTriangle_HW(VBUF2* vertices, int vert_count, int depth);
void S_Output_DrawLine_HW(VBUF2* vertices, int depth);

void InitBuckets();
int FindBucket(DWORD tpage);

