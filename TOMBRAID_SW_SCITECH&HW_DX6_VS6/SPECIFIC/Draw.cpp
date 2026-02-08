#include <windows.h>

#include "..\\OBJECTS\\keyhole.h"
#include "..\\OBJECTS\\puzzle_hole.h"
#include "..\\OBJECTS\\switch.h"
#include "..\\traps\\lava.h"
#include "..\\traps\\movable_block.h"
#include "3d_gen.h"
#include "box.h"
#include "collide.h"
#include "const.h"
#include "control_util.h"
#include "draw.h"
#include "game.h"
#include "items.h"
#include "lara.h"
#include "lot.h"
#include "matrix.h"
#include "phd_math.h"
#include "screen.h"
#include "setup.h"
#include "sound.h"
#include "sphere.h"
#include "util.h"
#include "vars.h"
#include "winmain.h"

#include "drawprimitive.h"

void DrawAnimatingItem(ITEM_INFO *item)
{
	static int16_t null_rotation[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	int16_t *frmptr[2];
	int32_t rate;
	int32_t frac = GetFrames(item, frmptr, &rate);
	OBJECT_INFO *object = &g_Objects[item->object_number];

	if (object->shadow_size)
	{
		Output_DrawShadow(object->shadow_size, frmptr[0], item);
	}

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	int32_t clip = S_GetObjectBounds(frmptr[0]);

	if (!clip)
	{
		phd_PopMatrix();
		return;
	}

	CalculateObjectLighting(item, frmptr[0]);
	int16_t *extra_rotation =
		(int16_t *)(item->data ? item->data : &null_rotation);

	int32_t bit = 1;
	int16_t **meshpp = &g_Meshes[object->mesh_index];
	int32_t *bone = &g_AnimBones[object->bone_index];

	if (!frac)
	{
		phd_TranslateRel(frmptr[0][FRAME_POS_X], frmptr[0][FRAME_POS_Y],
						 frmptr[0][FRAME_POS_Z]);

		int32_t *packed_rotation = (int32_t *)(frmptr[0] + FRAME_ROT);
		phd_RotYXZpack(*packed_rotation++);

		if (item->mesh_bits & bit)
		{
			Output_DrawPolygons(*meshpp++, clip);
		}

		for (int i = 1; i < object->nmeshes; i++)
		{
			int32_t bone_extra_flags = *bone;
			if (bone_extra_flags & BEB_POP)
			{
				phd_PopMatrix();
			}

			if (bone_extra_flags & BEB_PUSH)
			{
				phd_PushMatrix();
			}

			phd_TranslateRel(bone[1], bone[2], bone[3]);
			phd_RotYXZpack(*packed_rotation++);

			if (bone_extra_flags & BEB_ROT_Y)
			{
				phd_RotY(*extra_rotation++);
			}
			if (bone_extra_flags & BEB_ROT_X)
			{
				phd_RotX(*extra_rotation++);
			}
			if (bone_extra_flags & BEB_ROT_Z)
			{
				phd_RotZ(*extra_rotation++);
			}

			bit <<= 1;
			if (item->mesh_bits & bit)
			{
				Output_DrawPolygons(*meshpp, clip);
			}

			bone += 4;
			meshpp++;
		}
	}
	else
	{
		InitInterpolate(frac, rate);
		phd_TranslateRel_ID(frmptr[0][FRAME_POS_X], frmptr[0][FRAME_POS_Y],
							frmptr[0][FRAME_POS_Z], frmptr[1][FRAME_POS_X],
							frmptr[1][FRAME_POS_Y], frmptr[1][FRAME_POS_Z]);
		int32_t *packed_rotation1 = (int32_t *)(frmptr[0] + FRAME_ROT);
		int32_t *packed_rotation2 = (int32_t *)(frmptr[1] + FRAME_ROT);
		phd_RotYXZpack_I(*packed_rotation1++, *packed_rotation2++);

		if (item->mesh_bits & bit)
		{
			Output_DrawPolygons_I(*meshpp++, clip);
		}

		for (int i = 1; i < object->nmeshes; i++)
		{
			int32_t bone_extra_flags = *bone;
			if (bone_extra_flags & BEB_POP)
			{
				phd_PopMatrix_I();
			}

			if (bone_extra_flags & BEB_PUSH)
			{
				phd_PushMatrix_I();
			}

			phd_TranslateRel_I(bone[1], bone[2], bone[3]);
			phd_RotYXZpack_I(*packed_rotation1++, *packed_rotation2++);

			if (bone_extra_flags & BEB_ROT_Y)
			{
				phd_RotY_I(*extra_rotation++);
			}
			if (bone_extra_flags & BEB_ROT_X)
			{
				phd_RotX_I(*extra_rotation++);
			}
			if (bone_extra_flags & BEB_ROT_Z)
			{
				phd_RotZ_I(*extra_rotation++);
			}

			bit <<= 1;
			if (item->mesh_bits & bit)
			{
				Output_DrawPolygons_I(*meshpp, clip);
			}

			bone += 4;
			meshpp++;
		}
	}

	phd_PopMatrix();
}

int32_t GetFrames(ITEM_INFO *item, int16_t *frmptr[], int32_t *rate)
{
	ANIM_STRUCT *anim = &g_Anims[item->anim_number];
	frmptr[0] = anim->frame_ptr;
	frmptr[1] = anim->frame_ptr;

	*rate = anim->interpolation;

	int32_t frm = item->frame_number - anim->frame_base;
	int32_t first = frm / anim->interpolation;
	int32_t frame_size = g_Objects[item->object_number].nmeshes * 2 + 10;

	frmptr[0] += first * frame_size;
	frmptr[1] = frmptr[0] + frame_size;

	int32_t interp = frm % anim->interpolation;
	if (!interp)
	{
		return 0;
	}

	int32_t second = anim->interpolation * (first + 1);
	if (second > anim->frame_end)
	{
		*rate = anim->frame_end + anim->interpolation - second;
	}

	return interp;
}

void Output_DrawShadow(int16_t size, int16_t *bptr, ITEM_INFO *item)
{
	int i;

	g_ShadowInfo.vertex_count = 8;

	int32_t x0 = bptr[FRAME_BOUND_MIN_X];
	int32_t x1 = bptr[FRAME_BOUND_MAX_X];
	int32_t z0 = bptr[FRAME_BOUND_MIN_Z];
	int32_t z1 = bptr[FRAME_BOUND_MAX_Z];

	int32_t x_mid = (x0 + x1) / 2;
	int32_t z_mid = (z0 + z1) / 2;

	int32_t x_add = (x1 - x0) * size / 1024;
	int32_t z_add = (z1 - z0) * size / 1024;

	for (i = 0; i < g_ShadowInfo.vertex_count; i++)
	{
		int32_t angle = (PHD_180 + i * PHD_360) / g_ShadowInfo.vertex_count;

		g_ShadowInfo.vertex[i].x = x_mid + (x_add * 2) * phd_sin(angle) / PHD_90;
		g_ShadowInfo.vertex[i].z = z_mid + (z_add * 2) * phd_cos(angle) / PHD_90;
		g_ShadowInfo.vertex[i].y = 0;
	}

	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->floor, item->pos.z);
	phd_RotY(item->pos.y_rot);

	if (Output_CalcObjectVertices(&g_ShadowInfo.poly_count))
	{
		int16_t clip_and = 1;
		int16_t clip_positive = 1;
		int16_t clip_or = 0;
		for (i = 0; i < g_ShadowInfo.vertex_count; i++)
		{
			clip_and &= m_VBuf[i].clip;
			clip_positive &= m_VBuf[i].clip >= 0;
			clip_or |= m_VBuf[i].clip;
		}
		PHD_VBUF *vn1 = &m_VBuf[0];
		PHD_VBUF *vn2 = &m_VBuf[1];
		PHD_VBUF *vn3 = &m_VBuf[2];

		bool visible = ((int32_t)(((vn3->xs - vn2->xs) * (vn1->ys - vn2->ys)) -
					   ((vn1->xs - vn2->xs) * (vn3->ys - vn2->ys))) >= 0);

		if (!clip_and && clip_positive && visible)
		{
			S_Output_DrawShadow(&m_VBuf[0], clip_or ? 1 : 0,
								g_ShadowInfo.vertex_count);
		}
	}

	phd_PopMatrix();
}

void S_Output_DrawShadow_SW(PHD_VBUF* vbufs, int clip, int vertex_count)
{
	VBUF2 vertices[32];

	for (int i = 0; i < vertex_count; i++)
	{
		VBUF2 *vertex = &vertices[i];
		PHD_VBUF *vbuf = &vbufs[i];
		vertex->x = (float)vbuf->xs;
		vertex->y = (float)vbuf->ys;
		vertex->g = 24.0f;
	}

	int vert_count = 8;

	if (clip)
	{
		vert_count = ClipVertices2(vert_count, &vertices[0]);

		if (!vertex_count)
		{
			return;
		}
	}

	if (vert_count)
	{
		int depth = (int)((vbufs[0].zv + vbufs[1].zv + vbufs[2].zv + vbufs[3].zv +
					 vbufs[4].zv + vbufs[5].zv + vbufs[6].zv + vbufs[7].zv) /
					8);

		int32_t *sort = sort3dptr;
		int16_t *info = info3dptr;

		sort[0] = (int32_t)info;
		sort[1] = depth;

		sort3dptr += 2;

		info[0] = 7;  // draw type
		info[1] = 24; // color / tex page
		info[2] = vert_count;

		info += 3;

		int32_t indx = 0;

		if (vert_count > 0)
		{
			do
			{
				info[0] = (short int)vertices[indx].x;
				info[1] = (short int)vertices[indx].y;
				info += 2;
				indx++;

			} while (indx < vert_count);

			info3dptr = info;

			surfacenum++;

		} // if(vert_count > 0)
		
	}
}

int S_GetObjectBounds(int16_t *bptr)
{
	if (g_PhdMatrixPtr->_23 >= DRAW_DISTANCE_MAX)
	{
		return 0;
	}

	int32_t x_min = bptr[0];
	int32_t x_max = bptr[1];
	int32_t y_min = bptr[2];
	int32_t y_max = bptr[3];
	int32_t z_min = bptr[4];
	int32_t z_max = bptr[5];

	PHD_VECTOR vtx[8];
	vtx[0].x = x_min;
	vtx[0].y = y_min;
	vtx[0].z = z_min;
	vtx[1].x = x_max;
	vtx[1].y = y_min;
	vtx[1].z = z_min;
	vtx[2].x = x_max;
	vtx[2].y = y_max;
	vtx[2].z = z_min;
	vtx[3].x = x_min;
	vtx[3].y = y_max;
	vtx[3].z = z_min;
	vtx[4].x = x_min;
	vtx[4].y = y_min;
	vtx[4].z = z_max;
	vtx[5].x = x_max;
	vtx[5].y = y_min;
	vtx[5].z = z_max;
	vtx[6].x = x_max;
	vtx[6].y = y_max;
	vtx[6].z = z_max;
	vtx[7].x = x_min;
	vtx[7].y = y_max;
	vtx[7].z = z_max;

	int num_z = 0;
	x_min = 0x3FFFFFFF;
	y_min = 0x3FFFFFFF;
	x_max = -0x3FFFFFFF;
	y_max = -0x3FFFFFFF;

	for (int i = 0; i < 8; i++)
	{
		int32_t zv = g_PhdMatrixPtr->_20 * vtx[i].x +
					 g_PhdMatrixPtr->_21 * vtx[i].y +
					 g_PhdMatrixPtr->_22 * vtx[i].z + g_PhdMatrixPtr->_23;

		if (zv > DRAW_DISTANCE_MIN && zv < DRAW_DISTANCE_MAX)
		{
			++num_z;
			int32_t zp = zv / g_PhdPersp;
			int32_t xv =
				(g_PhdMatrixPtr->_00 * vtx[i].x +
				 g_PhdMatrixPtr->_01 * vtx[i].y +
				 g_PhdMatrixPtr->_02 * vtx[i].z + g_PhdMatrixPtr->_03) /
				zp;
			int32_t yv =
				(g_PhdMatrixPtr->_10 * vtx[i].x +
				 g_PhdMatrixPtr->_11 * vtx[i].y +
				 g_PhdMatrixPtr->_12 * vtx[i].z + g_PhdMatrixPtr->_13) /
				zp;

			if (x_min > xv)
			{
				x_min = xv;
			}
			else if (x_max < xv)
			{
				x_max = xv;
			}

			if (y_min > yv)
			{
				y_min = yv;
			}
			else if (y_max < yv)
			{
				y_max = yv;
			}
		}
	}

	x_min += ViewPort_GetCenterX();
	x_max += ViewPort_GetCenterX();
	y_min += ViewPort_GetCenterY();
	y_max += ViewPort_GetCenterY();

	if (!num_z || x_min > g_PhdRight || y_min > g_PhdBottom ||
		x_max < g_PhdLeft || y_max < g_PhdTop)
	{
		return 0; // out of screen
	}

	if (num_z < 8 || x_min < 0 || y_min < 0 || x_max > g_PhdRight ||
		y_max > g_PhdBottom)
	{
		return -1; // clipped
	}

	return 1; // fully on screen
}

void CalculateObjectLighting(ITEM_INFO *item, int16_t *frame)
{
	if (item->shade >= 0)
	{
		Output_CalculateStaticLight(item->shade);
		return;
	}

	phd_PushUnitMatrix();
	g_PhdMatrixPtr->_23 = 0;
	g_PhdMatrixPtr->_13 = 0;
	g_PhdMatrixPtr->_03 = 0;

	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
	phd_TranslateRel((frame[FRAME_BOUND_MIN_X] + frame[FRAME_BOUND_MAX_X]) / 2,
					 (frame[FRAME_BOUND_MIN_Y] + frame[FRAME_BOUND_MAX_Y]) / 2,
					 (frame[FRAME_BOUND_MIN_Z] + frame[FRAME_BOUND_MAX_Z]) / 2);

	int32_t x = (g_PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
	int32_t y = (g_PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
	int32_t z = (g_PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;

	phd_PopMatrix();

	Output_CalculateLight(x, y, z, item->room_number);
}

void Output_DrawPolygons(int16_t *obj_ptr, int clip)
{
	obj_ptr += 4;
	obj_ptr = Output_CalcObjectVertices(obj_ptr);
	if (obj_ptr)
	{
		obj_ptr = Output_CalcVerticeLight(obj_ptr);
		obj_ptr = S_DrawObjectGT4(obj_ptr + 1, *obj_ptr);
		obj_ptr = S_DrawObjectGT3(obj_ptr + 1, *obj_ptr);
		obj_ptr = S_DrawObjectG4(obj_ptr + 1, *obj_ptr);
		obj_ptr = S_DrawObjectG3(obj_ptr + 1, *obj_ptr);
	}
}

void Output_DrawPolygons_I(int16_t *obj_ptr, int32_t clip)
{
	phd_PushMatrix();
	InterpolateMatrix();
	Output_DrawPolygons(obj_ptr, clip);
	phd_PopMatrix();
}

int16_t *Output_CalcObjectVertices(int16_t *obj_ptr)
{
	int16_t total_clip = -1;

	obj_ptr++;
	int vertex_count = *obj_ptr++;
	for (int i = 0; i < vertex_count; i++)
	{
		int32_t xv = g_PhdMatrixPtr->_00 * obj_ptr[0] +
					 g_PhdMatrixPtr->_01 * obj_ptr[1] +
					 g_PhdMatrixPtr->_02 * obj_ptr[2] + g_PhdMatrixPtr->_03;
		int32_t yv = g_PhdMatrixPtr->_10 * obj_ptr[0] +
					 g_PhdMatrixPtr->_11 * obj_ptr[1] +
					 g_PhdMatrixPtr->_12 * obj_ptr[2] + g_PhdMatrixPtr->_13;
		int32_t zv = g_PhdMatrixPtr->_20 * obj_ptr[0] +
					 g_PhdMatrixPtr->_21 * obj_ptr[1] +
					 g_PhdMatrixPtr->_22 * obj_ptr[2] + g_PhdMatrixPtr->_23;
		m_VBuf[i].xv = (float)xv;
		m_VBuf[i].yv = (float)yv;
		m_VBuf[i].zv = (float)zv;

		float one = (256.0f * 8.0f * 16384.0f);

		float z = ((float)(g_PhdPersp)) / zv;
		//m_VBuf[i].ooz = z * (one / g_PhdPersp);

		m_VBuf[i].ooz = ((float)(one)) / zv;


		int32_t clip_flags;
		if (zv < DRAW_DISTANCE_MIN)
		{
			clip_flags = -32768;
		}
		else
		{
			clip_flags = 0;

			int32_t xs = ViewPort_GetCenterX() + xv / (zv / g_PhdPersp);
			int32_t ys = ViewPort_GetCenterY() + yv / (zv / g_PhdPersp);

			if (xs < g_PhdLeft)
			{
				if (xs < -32760)
				{
					xs = -32760;
				}
				clip_flags |= 1;
			}
			else if (xs > g_PhdRight)
			{
				if (xs > 32760)
				{
					xs = 32760;
				}
				clip_flags |= 2;
			}

			if (ys < g_PhdTop)
			{
				if (ys < -32760)
				{
					ys = -32760;
				}
				clip_flags |= 4;
			}
			else if (ys > g_PhdBottom)
			{
				if (ys > 32760)
				{
					ys = 32760;
				}
				clip_flags |= 8;
			}

			m_VBuf[i].xs = (float)xs;
			m_VBuf[i].ys = (float)ys;
		}

		m_VBuf[i].clip = clip_flags;
		total_clip &= clip_flags;
		obj_ptr += 3;
	}

	return total_clip == 0 ? obj_ptr : NULL;
}

int16_t *Output_CalcVerticeLight(int16_t *obj_ptr)
{
	int32_t vertex_count = *obj_ptr++;
	if (vertex_count > 0)
	{
		if (g_LsDivider)
		{
			int32_t xv = (g_PhdMatrixPtr->_00 * g_LsVectorView.x +
						  g_PhdMatrixPtr->_10 * g_LsVectorView.y +
						  g_PhdMatrixPtr->_20 * g_LsVectorView.z) /
						 g_LsDivider;
			int32_t yv = (g_PhdMatrixPtr->_01 * g_LsVectorView.x +
						  g_PhdMatrixPtr->_11 * g_LsVectorView.y +
						  g_PhdMatrixPtr->_21 * g_LsVectorView.z) /
						 g_LsDivider;
			int32_t zv = (g_PhdMatrixPtr->_02 * g_LsVectorView.x +
						  g_PhdMatrixPtr->_12 * g_LsVectorView.y +
						  g_PhdMatrixPtr->_22 * g_LsVectorView.z) /
						 g_LsDivider;
			for (int i = 0; i < vertex_count; i++)
			{
				int16_t shade =
					g_LsAdder +
					((obj_ptr[0] * xv + obj_ptr[1] * yv + obj_ptr[2] * zv) >>
					 16);
				CLAMP(shade, 0, 0x1FFF);
				m_VBuf[i].g = shade;
				obj_ptr += 3;
			}
			return obj_ptr;
		}
		else
		{
			int16_t shade = g_LsAdder;
			CLAMP(shade, 0, 0x1FFF);
			for (int i = 0; i < vertex_count; i++)
			{
				m_VBuf[i].g = shade;
			}
			obj_ptr += 3 * vertex_count;
		}
	}
	else
	{
		for (int i = 0; i < -vertex_count; i++)
		{
			int16_t shade = g_LsAdder + *obj_ptr++;
			CLAMP(shade, 0, 0x1FFF);
			m_VBuf[i].g = shade;
		}
	}

	return obj_ptr;
}

int16_t* S_DrawObjectGT4_SW(int16_t* obj_ptr, int32_t number)
{
	PHD_VBUF *vns[4];
	VBUF vertices[8];
	POINT_INFO points[4];
	PHD_TEXTURE *tex;
	int32_t vert_count = 4;

	int num_TexturedQuad = number;

	if (number > 0)
	{
		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];
			vns[3] = &m_VBuf[*obj_ptr++];

			tex = &g_PhdTextureInfo[*obj_ptr++];

			if (!(vns[0]->clip & vns[1]->clip & vns[2]->clip & vns[3]->clip))
			{
				if (vns[0]->clip >= 0 && vns[1]->clip >= 0 &&
					vns[2]->clip >= 0 && vns[3]->clip >= 0)
				{
					//псевдоскалярное косое умножение векторов
					//выясняем куда смотрит полигон - от зрителя или к зрителю
					if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs) -
							(vns[2]->ys - vns[1]->ys) *
								(vns[0]->xs - vns[1]->xs) >
						0)
					{
						//полигон смотрит к зрителю
						vertices[0].x = (float)vns[0]->xs;
						vertices[0].y = (float)vns[0]->ys;
						vertices[0].z = 8589934592.0f / vns[0]->zv;
						vertices[0].u = vertices[0].z * tex->uv[0].u1;
						vertices[0].v = vertices[0].z * tex->uv[0].v1;
						vertices[0].g = vns[0]->g;

						vertices[1].x = (float)vns[1]->xs;
						vertices[1].y = (float)vns[1]->ys;
						vertices[1].z = 8589934592.0f / vns[1]->zv;
						vertices[1].u = vertices[1].z * tex->uv[1].u1;
						vertices[1].v = vertices[1].z * tex->uv[1].v1;
						vertices[1].g = vns[1]->g;

						vertices[2].x = (float)vns[2]->xs;
						vertices[2].y = (float)vns[2]->ys;
						vertices[2].z = 8589934592.0f / vns[2]->zv;
						vertices[2].u = vertices[2].z * tex->uv[2].u1;
						vertices[2].v = vertices[2].z * tex->uv[2].v1;
						vertices[2].g = vns[2]->g;

						vertices[3].x = (float)vns[3]->xs;
						vertices[3].y = (float)vns[3]->ys;
						vertices[3].z = 8589934592.0f / vns[3]->zv;
						vertices[3].u = vertices[3].z * tex->uv[3].u1;
						vertices[3].v = vertices[3].z * tex->uv[3].v1;
						vertices[3].g = vns[3]->g;

						vert_count = 4;

						//если необходимо отсечение по границе экрана
						if (vns[0]->clip || vns[1]->clip || vns[2]->clip ||
							vns[3]->clip)
						{

						ClipVerts: //если необходимо отсечение по границе экрана
							vert_count = ClipVertices(vert_count, &vertices[0]);
						}

						// Skip_ClipVerts:

						if (vert_count)
						{
							int depth = (int)vns[0]->zv;

							if (depth < vns[1]->zv)
							{
								depth = (int)vns[1]->zv;
							}

							if (depth < vns[2]->zv)
							{
								depth = (int)vns[2]->zv;
							}

							if (depth < vns[3]->zv)
							{
								depth = (int)vns[3]->zv;
							}

							int32_t *sort = sort3dptr;
							int16_t *info = info3dptr;

							sort[0] = (int32_t)info;
							sort[1] = depth;

							sort3dptr += 2;

							// info3dptr 2 bytes - 16 бит
							// 0 draw routine
							// 1 texture page
							// 2 num coords ie vert count
							// 3 vert1
							// 4 vert2
							// 5 vert3

							info[0] = tex->drawtype + 2;
							info[1] = tex->tpage;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if (vert_count > 0)
							{

								do
								{
									info[0] = (short int)vertices[indx].x;
									info[1] = (short int)vertices[indx].y;
									info[2] = (short int)vertices[indx].g;

									*(float *)&info[3] = vertices[indx].z;
									*(float *)&info[5] = vertices[indx].u;
									*(float *)&info[7] = vertices[indx].v;

									info += 9;
									indx++;

								} while (indx < vert_count);

								info3dptr = info;

							} // if(vert_count > 0)

							surfacenum++;

						} // if(vert_count)

						// goto LABEL_32;

					} // if kosoe > 0
				}
				else // if && clip
				{
					//отбрасывание задних поверхностей
					//если поверхность обращена от зрителя - пропускаем ее
					if (phd_VisibleZClip(vns[0], vns[1], vns[2]))
					{

						points[0].xs = (float)vns[0]->xs;
						points[0].ys = (float)vns[0]->ys;
						points[0].xv = (float)vns[0]->xv;
						points[0].yv = (float)vns[0]->yv;
						points[0].zv = (float)vns[0]->zv;
						points[0].u = tex->uv[0].u1;
						points[0].v = tex->uv[0].v1;
						points[0].g = vns[0]->g;

						points[1].xs = (float)vns[1]->xs;
						points[1].ys = (float)vns[1]->ys;
						points[1].xv = (float)vns[1]->xv;
						points[1].yv = (float)vns[1]->yv;
						points[1].zv = (float)vns[1]->zv;
						points[1].u = tex->uv[1].u1;
						points[1].v = tex->uv[1].v1;
						points[1].g = vns[1]->g;

						points[2].xs = (float)vns[2]->xs;
						points[2].ys = (float)vns[2]->ys;
						points[2].xv = (float)vns[2]->xv;
						points[2].yv = (float)vns[2]->yv;
						points[2].zv = (float)vns[2]->zv;
						points[2].u = tex->uv[2].u1;
						points[2].v = tex->uv[2].v1;
						points[2].g = vns[2]->g;

						points[3].xs = (float)vns[3]->xs;
						points[3].ys = (float)vns[3]->ys;
						points[3].xv = (float)vns[3]->xv;
						points[3].yv = (float)vns[3]->yv;
						points[3].zv = (float)vns[3]->zv;
						points[3].u = tex->uv[3].u1;
						points[3].v = tex->uv[3].v1;
						points[3].g = vns[3]->g;

						vert_count = 4;

						vert_count = ZedClipper(vert_count, &points[0], &vertices[0]);

						if (vert_count)
						{
							goto ClipVerts;
						}

						// goto Skip_ClipVerts;

					} // if ( phd_VisibleZClip

				} // else if any clip
			}
			// LABEL_32:

			num_TexturedQuad--;
			// if not all clip

		} while (num_TexturedQuad > 0);

	} // if (number > )

	return obj_ptr;
}

int16_t *S_DrawObjectGT3_SW(int16_t *obj_ptr, int32_t number)
{
	PHD_VBUF *vns[3];
	VBUF vertices[8];
	POINT_INFO points[3];
	PHD_TEXTURE *tex;
	int32_t vert_count = 3;

	int num_TexturedTri = number;

	if (number > 0)
	{

		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];

			tex = &g_PhdTextureInfo[*obj_ptr++];

			if (!(vns[0]->clip & vns[1]->clip & vns[2]->clip))
			{
				if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0)
				{
					//псевдоскалярное косое умножение векторов
					//выясняем куда смотрит полигон - от зрителя или к зрителю
					if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs) -
							(vns[2]->ys - vns[1]->ys) *
								(vns[0]->xs - vns[1]->xs) >
						0)
					{
						//полигон смотрит к зрителю

						vertices[0].x = (float)vns[0]->xs;
						vertices[0].y = (float)vns[0]->ys;
						vertices[0].z = 8589934592.0f / vns[0]->zv;
						vertices[0].u = vertices[0].z * tex->uv[0].u1;
						vertices[0].v = vertices[0].z * tex->uv[0].v1;
						vertices[0].g = vns[0]->g;

						vertices[1].x = (float)vns[1]->xs;
						vertices[1].y = (float)vns[1]->ys;
						vertices[1].z = 8589934592.0f / vns[1]->zv;
						vertices[1].u = vertices[1].z * tex->uv[1].u1;
						vertices[1].v = vertices[1].z * tex->uv[1].v1;
						vertices[1].g = vns[1]->g;

						vertices[2].x = (float)vns[2]->xs;
						vertices[2].y = (float)vns[2]->ys;
						vertices[2].z = 8589934592.0f / vns[2]->zv;
						vertices[2].u = vertices[2].z * tex->uv[2].u1;
						vertices[2].v = vertices[2].z * tex->uv[2].v1;
						vertices[2].g = vns[2]->g;

						vert_count = 3;

						//если необходимо отсечение по границе экрана
						if (vns[0]->clip || vns[1]->clip || vns[2]->clip)
						{

						ClipVerts: //если необходимо отсечение по границе экрана
							vert_count = ClipVertices(vert_count, &vertices[0]);
						}

						// Skip_ClipVerts:

						if (vert_count)
						{
							int depth = (int)vns[0]->zv;

							if (depth < vns[1]->zv)
							{
								depth = (int)vns[1]->zv;
							}

							if (depth < vns[2]->zv)
							{
								depth = (int)vns[2]->zv;
							}

							int32_t *sort = sort3dptr;
							int16_t *info = info3dptr;

							sort[0] = (int32_t)info;
							sort[1] = depth;

							sort3dptr += 2;

							// info3dptr 2 bytes - 16 бит
							// 0 draw routine
							// 1 texture page
							// 2 num coords ie vert count
							// 3 vert1
							// 4 vert2
							// 5 vert3

							info[0] = tex->drawtype + 2;
							info[1] = tex->tpage;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if (vert_count > 0)
							{

								do
								{
									info[0] =
										(short int)vertices[indx].x; // edx
									info[1] = (short int)vertices[indx]
													.y; // edx + 4
									info[2] = (short int)vertices[indx].g;

									*(float *)&info[3] = vertices[indx].z;
									*(float *)&info[5] = vertices[indx].u;
									*(float *)&info[7] = vertices[indx].v;

									info += 9;
									indx++;

								} while (indx < vert_count);

								info3dptr = info;

							} // if(vert_count > 0)

							surfacenum++;

						} // if(vert_count)

						// goto LABEL_32;

					} // if kosoe > 0
				}
				else // if && clip
				{
					//отбрасывание задних поверхностей
					//если поверхность обращена от зрителя - пропускаем ее
					if (phd_VisibleZClip(vns[0], vns[1], vns[2]))
					{

						points[0].xs = (float)vns[0]->xs;
						points[0].ys = (float)vns[0]->ys;
						points[0].xv = (float)vns[0]->xv;
						points[0].yv = (float)vns[0]->yv;
						points[0].zv = (float)vns[0]->zv;
						points[0].u = tex->uv[0].u1;
						points[0].v = tex->uv[0].v1;
						points[0].g = vns[0]->g;

						points[1].xs = (float)vns[1]->xs;
						points[1].ys = (float)vns[1]->ys;
						points[1].xv = (float)vns[1]->xv;
						points[1].yv = (float)vns[1]->yv;
						points[1].zv = (float)vns[1]->zv;
						points[1].u = tex->uv[1].u1;
						points[1].v = tex->uv[1].v1;
						points[1].g = vns[1]->g;

						points[2].xs = (float)vns[2]->xs;
						points[2].ys = (float)vns[2]->ys;

						points[2].xv = (float)vns[2]->xv;
						points[2].yv = (float)vns[2]->yv;
						points[2].zv = (float)vns[2]->zv;
						points[2].u = tex->uv[2].u1;
						points[2].v = tex->uv[2].v1;
						points[2].g = vns[2]->g;

						vert_count = 3;

						vert_count = ZedClipper(vert_count, &points[0], &vertices[0]);

						if (vert_count)
						{
							goto ClipVerts;
						}

						// goto Skip_ClipVerts;

					} // if ( phd_VisibleZClip

				} // else if any clip
			}
			// LABEL_32:

			num_TexturedTri--;
			// if not all clip

		} while (num_TexturedTri > 0);

	} // if (number > )

	return obj_ptr;
}

int16_t *S_DrawObjectG4_SW(int16_t *obj_ptr, int32_t number)
{
	PHD_VBUF *vns[4];
	VBUF2 vertices[8];
	int color;
	int32_t vert_count = 4;

	int num_ColoredQuad = number;

	if (number > 0)
	{

		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];
			vns[3] = &m_VBuf[*obj_ptr++];

			color = *obj_ptr++;

			if (!(vns[0]->clip & vns[1]->clip & vns[2]->clip & vns[3]->clip))
			{
				if (vns[0]->clip >= 0 && vns[1]->clip >= 0 &&
					vns[2]->clip >= 0 && vns[3]->clip >= 0)
				{
					//псевдоскалярное косое умножение векторов
					//выясняем куда смотрит полигон - от зрителя или к зрителю
					if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs) -
							(vns[2]->ys - vns[1]->ys) *
								(vns[0]->xs - vns[1]->xs) >
						0)
					{
						//полигон смотрит к зрителю
						vertices[0].x = (float)vns[0]->xs;
						vertices[0].y = (float)vns[0]->ys;
						vertices[0].g = (float)vns[0]->g;

						vertices[1].x = (float)vns[1]->xs;
						vertices[1].y = (float)vns[1]->ys;
						vertices[1].g = (float)vns[1]->g;

						vertices[2].x = (float)vns[2]->xs;
						vertices[2].y = (float)vns[2]->ys;
						vertices[2].g = (float)vns[2]->g;

						vertices[3].x = (float)vns[3]->xs;
						vertices[3].y = (float)vns[3]->ys;
						vertices[3].g = (float)vns[3]->g;

						vert_count = 4;

						//если необходимо отсечение по границе экрана
						if (vns[0]->clip || vns[1]->clip || vns[2]->clip ||
							vns[3]->clip)
						{

							//		ClipVerts:		//если необходимо отсечение
							//по границе экрана
							vert_count =
								ClipVertices2(vert_count, &vertices[0]);
						}

						//		Skip_ClipVerts:

						if (vert_count)
						{
							int depth = (int)((vns[0]->zv + vns[1]->zv + vns[2]->zv +
										 vns[3]->zv) / 4);

							int32_t *sort = sort3dptr;
							int16_t *info = info3dptr;

							sort[0] = (int32_t)info;
							sort[1] = depth;

							sort3dptr += 2;

							// info3dptr 2 bytes - 16 бит
							// 0 draw routine
							// 1 texture page
							// 2 num coords ie vert count
							// 3 vert1
							// 4 vert2
							// 5 vert3

							info[0] = 6;
							info[1] = color;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if (vert_count > 0)
							{


								do
								{

									info[0] = (short int)vertices[indx].x;
									info[1] = (short int)vertices[indx].y;
									info[2] = (short int)vertices[indx].g;

									info += 3;
									indx++;

								} while (indx < vert_count);

								info3dptr = info;

							} // if(vert_count > 0)

								surfacenum++;

						} // if(vert_count)

						goto LABEL_32;

					} // if kosoe > 0
				}
				
			}
		LABEL_32:

			num_ColoredQuad--;
			// if not all clip

		} while (num_ColoredQuad > 0);

	} // if (number > )

	return obj_ptr;
}

//-------------------------------------------

int16_t *S_DrawObjectG3_SW(int16_t *obj_ptr, int32_t number)
{
	PHD_VBUF *vns[4];
	VBUF2 vertices[8];
	int color;
	int32_t vert_count = 3;

	int num_ColoredQuad = number;

	if (number > 0)
	{

		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];

			color = *obj_ptr++;

			if (!(vns[0]->clip & vns[1]->clip & vns[2]->clip))
			{
				if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0)
				{
					//псевдоскалярное косое умножение векторов
					//выясняем куда смотрит полигон - от зрителя или к зрителю
					if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs) -
							(vns[2]->ys - vns[1]->ys) *
								(vns[0]->xs - vns[1]->xs) >
						0)
					{
						//полигон смотрит к зрителю
						vertices[0].x = (float)vns[0]->xs;
						vertices[0].y = (float)vns[0]->ys;
						vertices[0].g = (float)vns[0]->g;

						vertices[1].x = (float)vns[1]->xs;
						vertices[1].y = (float)vns[1]->ys;
						vertices[1].g = (float)vns[1]->g;

						vertices[2].x = (float)vns[2]->xs;
						vertices[2].y = (float)vns[2]->ys;
						vertices[2].g = (float)vns[2]->g;

						vert_count = 3;

						//если необходимо отсечение по границе экрана
						if (vns[0]->clip || vns[1]->clip || vns[2]->clip)
						{

							//		ClipVerts:		//если необходимо отсечение
							//по границе экрана
							vert_count =
								ClipVertices2(vert_count, &vertices[0]);
						}

						//		Skip_ClipVerts:

						if (vert_count)
						{
							int depth = (int)((vns[0]->zv + vns[1]->zv + vns[2]->zv) / 3);

							int32_t *sort = sort3dptr;
							int16_t *info = info3dptr;

							sort[0] = (int32_t)info;
							sort[1] = depth;

							sort3dptr += 2;

							// info3dptr 2 bytes - 16 бит
							// 0 draw routine
							// 1 texture page
							// 2 num coords ie vert count
							// 3 vert1
							// 4 vert2
							// 5 vert3

							info[0] = 6;
							info[1] = color;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if (vert_count > 0)
							{

								do
								{
									info[0] = (short int)vertices[indx].x;
									info[1] = (short int)vertices[indx].y;
									info[2] = (short int)vertices[indx].g;

									info += 3;
									indx++;

								} while (indx < vert_count);

								info3dptr = info;

							} // if(vert_count > 0)

							surfacenum++;

						} // if(vert_count)

						goto LABEL_32;

					} // if kosoe > 0
				}
				
			}
		LABEL_32:

			num_ColoredQuad--;
			// if not all clip

		} while (num_ColoredQuad > 0);

	} // if (number > )

	return obj_ptr;

}

int32_t ZedClipper(int32_t vertex_count, POINT_INFO *pts, VBUF *vertices)
{

	float clip;
	POINT_INFO *pts1;
	POINT_INFO *pts0;

	float multiplier = 0.0625f * BRIGHTNESS;

	float one = (256.0f * 8.0f * 16384.0f);

	int j = 0;

	float near_z = Z_NEAR;

	float persp_o_near_z = g_PhdPersp / near_z;

	if (vertex_count == 0)
		return 0;

	VBUF *v = &vertices[0];
	pts0 = &pts[vertex_count - 1];

	int i;

	for (i = 0; i < vertex_count; i++)
	{
		pts1 = pts0;
		pts0 = &pts[i];

		if (near_z > pts1->zv)
		{
			if (near_z > pts0->zv)
			{
				continue;
			}

			clip = (near_z - pts0->zv) / (pts1->zv - pts0->zv);

			v->x = ((pts1->xv - pts0->xv) * clip + pts0->xv) * persp_o_near_z +
				ViewPort_GetCenterX();

			v->y = ((pts1->yv - pts0->yv) * clip + pts0->yv) * persp_o_near_z +
				   ViewPort_GetCenterY();

			if(!Hardware)
			{
				v->z = 8589934592.0f / near_z;
				v->u = v->z * ((pts1->u - pts0->u) * clip + pts0->u);
				v->v = v->z * ((pts1->v - pts0->v) * clip + pts0->v);
				v->g = (pts1->g - pts0->g) * clip + pts0->g;

			}
			else
			{
				v->z = near_z;
				v->w = one / v->z;
				v->u = v->w * ((pts1->u - pts0->u) * clip + pts0->u);
				v->v = v->w * ((pts1->v - pts0->v) * clip + pts0->v);
				v->g = (8192.0f - ((pts1->g - pts0->g) * clip + pts0->g)) * multiplier;
			
			}

			v++;

		}

		if (near_z > pts0->zv)
		{
			clip = (near_z - pts0->zv) / (pts1->zv - pts0->zv);

			v->x = ((pts1->xv - pts0->xv) * clip + pts0->xv) * persp_o_near_z +
				   ViewPort_GetCenterX();

			v->y = ((pts1->yv - pts0->yv) * clip + pts0->yv) * persp_o_near_z +
				   ViewPort_GetCenterY();

			if(!Hardware)
			{
				v->z = 8589934592.0f / near_z;
				v->u = v->z * ((pts1->u - pts0->u) * clip + pts0->u);
				v->v = v->z * ((pts1->v - pts0->v) * clip + pts0->v);
				v->g = (pts1->g - pts0->g) * clip + pts0->g;
			}
			else
			{
				v->z = near_z;
				v->w = one / v->z;
				v->u = v->w * ((pts1->u - pts0->u) * clip + pts0->u);
				v->v = v->w * ((pts1->v - pts0->v) * clip + pts0->v);
				v->g = (8192.0f - ((pts1->g - pts0->g) * clip + pts0->g)) * multiplier;
			}

			v++;

		}
		else
		{
			v->x = pts0->xs;
			v->y = pts0->ys;

			if(!Hardware)
			{
				v->z = 8589934592.0f / pts0->zv;
				v->u = pts0->u * v->z;
				v->v = pts0->v * v->z;
				v->g = pts0->g;
			}
			else
			{
				v->z = pts0->zv;
				v->w = one / v->z;
				v->u = v->w * pts0->u;
				v->v = v->w * pts0->v;
				v->g = (8192.0f - pts0->g) * multiplier;
			}

			v++;

		}
	}

	int32_t count = (int32_t)(v - vertices);
	return count < 3 ? 0 : count;

	return 1;
}

#define CLIP_GUV(OUT, B, A) \
do { \
    OUT.w = B->w + (A->w - B->w) * clipper; \
    OUT.z = B->z + (A->z - B->z) * clipper; \
    OUT.u = B->u + (A->u - B->u) * clipper; \
    OUT.v = B->v + (A->v - B->v) * clipper; \
    OUT.g = B->g + (A->g - B->g) * clipper; \
} while(0)


VBUF XYGUV_output[8];

//из Томб Райдер 3
//int RoomXYGUVClipper(int number, VBUF* input)
int ClipVertices(int number, VBUF* input)
{
	/* Clips to X then to Y and returns new number of points: input must be able to hold extra points! */
	VBUF* A, * B;
	int 			i, j;
	float 			clipper;
	//double			fconv;

	/* Clip against X sides */
	B = &input[number - 1];
	for (i = j = 0; i < number; i++)
	{
		A = B;
		B = &input[i];

		/* Clip if point A is off screen */
		if (A->x < g_SurfaceMinX)
		{
			if (B->x < g_SurfaceMinX)
				continue;

			clipper = (g_SurfaceMinX - B->x) / (A->x - B->x);
			CLIP_GUV(XYGUV_output[j], B, A);
			XYGUV_output[j].x = g_SurfaceMinX;
			XYGUV_output[j++].y = B->y + (A->y - B->y) * clipper;
		}
		else if (A->x > g_SurfaceMaxX)
		{
			if (B->x > g_SurfaceMaxX)
				continue;

			clipper = (g_SurfaceMaxX - B->x) / (A->x - B->x);
			CLIP_GUV(XYGUV_output[j], B, A);
			XYGUV_output[j].x = g_SurfaceMaxX;
			XYGUV_output[j++].y = B->y + (A->y - B->y) * clipper;
		}

		/* Clip if point B is off screen, else add it's point unclipped */
		if (B->x < g_SurfaceMinX)
		{
			clipper = (g_SurfaceMinX - B->x) / (A->x - B->x);
			CLIP_GUV(XYGUV_output[j], B, A);
			XYGUV_output[j].x = g_SurfaceMinX;
			XYGUV_output[j++].y = B->y + (A->y - B->y) * clipper;
		}
		else if (B->x > g_SurfaceMaxX)
		{
			clipper = (g_SurfaceMaxX - B->x) / (A->x - B->x);
			CLIP_GUV(XYGUV_output[j], B, A);
			XYGUV_output[j].x = g_SurfaceMaxX;
			XYGUV_output[j++].y = B->y + (A->y - B->y) * clipper;
		}
		else
		{
			/* Not clipped */
			XYGUV_output[j].x = B->x;
			XYGUV_output[j].y = B->y;
			XYGUV_output[j].z = B->z;
			XYGUV_output[j].w = B->w;
			XYGUV_output[j].g = B->g;
			
			XYGUV_output[j].u = B->u;
			XYGUV_output[j++].v = B->v;
		}
	}

	/* New number of points in 'XYGUV_output': now clip these against Y */
	if (j < 3)
		return (0);
	number = j;

	/* Clip against Y sides */
	B = &XYGUV_output[number - 1];
	for (i = j = 0; i < number; i++)
	{
		A = B;
		B = &XYGUV_output[i];

		/* Clip if point A is off screen */
		if (A->y < g_SurfaceMinY)
		{
			if (B->y < g_SurfaceMinY)
				continue;

			clipper = (g_SurfaceMinY - B->y) / (A->y - B->y);
			CLIP_GUV(input[j], B, A);
			input[j].x = B->x + (A->x - B->x) * clipper;
			input[j++].y = g_SurfaceMinY;
		}
		else if (A->y > g_SurfaceMaxY)
		{
			if (B->y > g_SurfaceMaxY)
				continue;

			clipper = (g_SurfaceMaxY - B->y) / (A->y - B->y);
			CLIP_GUV(input[j], B, A);
			input[j].x = B->x + (A->x - B->x) * clipper;
			input[j++].y = g_SurfaceMaxY;
		}

		/* Clip if point B is off screen, else add its point unclipped */
		if (B->y < g_SurfaceMinY)
		{
			clipper = (g_SurfaceMinY - B->y) / (A->y - B->y);
			CLIP_GUV(input[j], B, A);
			input[j].x = B->x + (A->x - B->x) * clipper;
			input[j++].y = g_SurfaceMinY;
		}
		else if (B->y > g_SurfaceMaxY)
		{
			clipper = (g_SurfaceMaxY - B->y) / (A->y - B->y);
			CLIP_GUV(input[j], B, A);
			input[j].x = B->x + (A->x - B->x) * clipper;
			input[j++].y = g_SurfaceMaxY;
		}
		else
		{
			/* Unclipped point */
			input[j].x = B->x;
			input[j].y = B->y;
			input[j].z = B->z;
			input[j].w = B->w;
			input[j].u = B->u;
			input[j].g = B->g;
			input[j++].v = B->v;
		}
	}

	/* Throw away polygons that have been reduced to lines or less */
	if (j < 3)
		return (0);

	return (j);
}



//оригинал из Tomb 1 Main ver 2.0
/*
int32_t ClipVertices(int32_t num, VBUF *source)
{

	float scale;
	VBUF vertices[8];

	VBUF *l = &source[num - 1];
	int j = 0;

	for (int i = 0; i < num; i++)
	{
		VBUF *v1 = &vertices[j];
		VBUF *v2 = l;
		l = &source[i];

		if (v2->x < g_SurfaceMinX)
		{
			if (l->x < g_SurfaceMinX)
			{
				continue;
			}
			scale = (g_SurfaceMinX - l->x) / (v2->x - l->x);

			v1->x = g_SurfaceMinX;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
	
			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;

			v1 = &vertices[++j];
		}
		else if (v2->x > g_SurfaceMaxX)
		{
			if (l->x > g_SurfaceMaxX)
			{
				continue;
			}
			scale = (g_SurfaceMaxX - l->x) / (v2->x - l->x);

			v1->x = g_SurfaceMaxX;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
			
			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;

			v1 = &vertices[++j];
		}

		if (l->x < g_SurfaceMinX)
		{
			scale = (g_SurfaceMinX - l->x) / (v2->x - l->x);

			v1->x = g_SurfaceMinX;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
			
			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;

			v1 = &vertices[++j];
		}
		else if (l->x > g_SurfaceMaxX)
		{
			scale = (g_SurfaceMaxX - l->x) / (v2->x - l->x);

			v1->x = g_SurfaceMaxX;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
			
			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;

			v1 = &vertices[++j];
		}
		else
		{
			v1->x = l->x;
			v1->y = l->y;
			v1->z = l->z;
			v1->w = l->w;
			v1->g = l->g;
			v1->u = l->u;
			v1->v = l->v;

			v1 = &vertices[++j];
		}
	}

	if (j < 3)
	{
		return 0;
	}

	num = j;
	l = &vertices[j - 1];
	j = 0;

	for (int i = 0; i < num; i++)
	{
		VBUF *v1 = &source[j];
		VBUF *v2 = l;
		l = &vertices[i];

		if (v2->y < g_SurfaceMinY)
		{
			if (l->y < g_SurfaceMinY)
			{
				continue;
			}
			scale = (g_SurfaceMinY - l->y) / (v2->y - l->y);

			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMinY;
			v1->z = (v2->z - l->z) * scale + l->z;

			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;

			v1 = &source[++j];
		}
		else if (v2->y > g_SurfaceMaxY)
		{
			if (l->y > g_SurfaceMaxY)
			{
				continue;
			}
			scale = (g_SurfaceMaxY - l->y) / (v2->y - l->y);

			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMaxY;
			v1->z = (v2->z - l->z) * scale + l->z;

			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;

			v1 = &source[++j];
		}

		if (l->y < g_SurfaceMinY)
		{
			scale = (g_SurfaceMinY - l->y) / (v2->y - l->y);

			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMinY;
			v1->z = (v2->z - l->z) * scale + l->z;

			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;
	

			v1 = &source[++j];
		}
		else if (l->y > g_SurfaceMaxY)
		{
			scale = (g_SurfaceMaxY - l->y) / (v2->y - l->y);

			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMaxY;
			v1->z = (v2->z - l->z) * scale + l->z;

			
			v1->w = (v2->w - l->w) * scale + l->w;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->u = (v2->u - l->u) * scale + l->u;
			v1->v = (v2->v - l->v) * scale + l->v;
			

			v1 = &source[++j];
		}
		else
		{
			v1->x = l->x;
			v1->y = l->y;
			v1->z = l->z;
			v1->w = l->w;
			v1->g = l->g;
			v1->u = l->u;
			v1->v = l->v;

			v1 = &source[++j];
		}
	}

	if (j < 3)
	{
		return 0;
	}

	return j;

}
*/


int32_t ClipVertices2(int32_t num, VBUF2 *source)
{

	float scale;
	VBUF2 vertices[32];

	VBUF2 *l = &source[num - 1];
	int j = 0;

	for (int i = 0; i < num; i++)
	{
		VBUF2 *v1 = &vertices[j];
		VBUF2 *v2 = l;
		l = &source[i];

		if (v2->x < g_SurfaceMinX)
		{
			if (l->x < g_SurfaceMinX)
			{
				continue;
			}
			scale = (g_SurfaceMinX - l->x) / (v2->x - l->x);
			v1->x = g_SurfaceMinX;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &vertices[++j];
		}
		else if (v2->x > g_PhdWinxmax)
		{
			if (l->x > g_PhdWinxmax)
			{
				continue;
			}
			scale = (g_PhdWinxmax - l->x) / (v2->x - l->x);
			v1->x = g_PhdWinxmax;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &vertices[++j];
		}

		if (l->x < g_SurfaceMinX)
		{
			scale = (g_SurfaceMinX - l->x) / (v2->x - l->x);
			v1->x = g_SurfaceMinX;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &vertices[++j];
		}
		else if (l->x > g_PhdWinxmax)
		{
			scale = (g_PhdWinxmax - l->x) / (v2->x - l->x);
			v1->x = g_PhdWinxmax;
			v1->y = (v2->y - l->y) * scale + l->y;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &vertices[++j];
		}
		else
		{
			v1->x = l->x;
			v1->y = l->y;
			v1->z = l->z;
			v1->g = l->g;
			v1 = &vertices[++j];
		}
	}

	if (j < 3)
	{
		return 0;
	}

	num = j;
	l = &vertices[j - 1];
	j = 0;

	//for (int i = 0; i < num; i++)
	for (i = 0; i < num; i++)
	{
		VBUF2 *v1 = &source[j];
		VBUF2 *v2 = l;
		l = &vertices[i];

		if (v2->y < g_SurfaceMinY)
		{
			if (l->y < g_SurfaceMinY)
			{
				continue;
			}
			scale = (g_SurfaceMinY - l->y) / (v2->y - l->y);
			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMinY;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &source[++j];
		}
		else if (v2->y > g_SurfaceMaxY)
		{
			if (l->y > g_SurfaceMaxY)
			{
				continue;
			}
			scale = (g_SurfaceMaxY - l->y) / (v2->y - l->y);
			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMaxY;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &source[++j];
		}

		if (l->y < g_SurfaceMinY)
		{
			scale = (g_SurfaceMinY - l->y) / (v2->y - l->y);
			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMinY;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &source[++j];
		}
		else if (l->y > g_SurfaceMaxY)
		{
			scale = (g_SurfaceMaxY - l->y) / (v2->y - l->y);
			v1->x = (v2->x - l->x) * scale + l->x;
			v1->y = g_SurfaceMaxY;
			v1->z = (v2->z - l->z) * scale + l->z;
			v1->g = (v2->g - l->g) * scale + l->g;
			v1->w = (v2->w - l->w) * scale + l->w;
			v1 = &source[++j];
		}
		else
		{
			v1->x = l->x;
			v1->y = l->y;
			v1->z = l->z;
			v1->g = l->g;
			v1 = &source[++j];
		}
	}

	if (j < 3)
	{
		return 0;
	}

	return j;
}

void Output_CalculateStaticLight(int16_t adder)
{
	g_LsAdder = adder - 16 * 256;
	int32_t distance = g_PhdMatrixPtr->_23 >> W2V_SHIFT;
	g_LsAdder += CalcFogShade(distance);
	CLAMPG(g_LsAdder, 0x1FFF);
}

void Output_CalculateLight(int32_t x, int32_t y, int32_t z, int16_t room_num)
{

	ROOM_INFO *r = &g_RoomInfo[room_num];

	if (r->num_lights == 0)
	{
		g_LsAdder = r->ambient;
		g_LsDivider = 0;
	}
	else
	{
		int32_t ambient = 0x1FFF - r->ambient;
		int32_t brightest = 0;

		PHD_VECTOR ls = {0, 0, 0};
		for (int i = 0; i < r->num_lights; i++)
		{
			LIGHT_INFO *light = &r->light[i];
			PHD_VECTOR lc;
			lc.x = x - light->x;
			lc.y = y - light->y;
			lc.z = z - light->z;

			int32_t distance =
				(SQUARE(lc.x) + SQUARE(lc.y) + SQUARE(lc.z)) >> 12;
			int32_t falloff = SQUARE(light->falloff) >> 12;
			int32_t shade =
				ambient + (light->intensity * falloff) / (distance + falloff);

			if (shade > brightest)
			{
				brightest = shade;
				ls = lc;
			}
		}

		g_LsAdder = (ambient + brightest) / 2;
		if (brightest == ambient)
		{
			g_LsDivider = 0;
		}
		else
		{
			g_LsDivider = (1 << (W2V_SHIFT + 12)) / (brightest - g_LsAdder);
		}
		g_LsAdder = 0x1FFF - g_LsAdder;

		PHD_ANGLE angles[2];
		phd_GetVectorAngles(ls.x, ls.y, ls.z, angles);
		phd_RotateLight(angles[1], angles[0]);
	}

	int32_t distance = g_PhdMatrixPtr->_23 >> W2V_SHIFT;
	g_LsAdder += CalcFogShade(distance);
	CLAMPG(g_LsAdder, 0x1FFF);
}

void DrawDummyItem(ITEM_INFO *item)
{
}

void InitialiseEvilLara(int16_t item_num)
{
	g_Objects[O_EVIL_LARA].frame_base = g_Objects[O_LARA].frame_base;
	g_Items[item_num].data = NULL;
}

void ControlEvilLara(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];

	if (item->hit_points < LARA_HITPOINTS)
	{
		g_LaraItem->hit_points -= (LARA_HITPOINTS - item->hit_points) * 10;
		item->hit_points = LARA_HITPOINTS;
	}

	if (!item->data)
	{
		int32_t x = 2 * 36 * WALL_L - g_LaraItem->pos.x;
		int32_t y = g_LaraItem->pos.y;
		int32_t z = 2 * 60 * WALL_L - g_LaraItem->pos.z;

		int16_t room_num = item->room_number;
		FLOOR_INFO *floor = GetFloor(x, y, z, &room_num);
		int32_t h = GetHeight(floor, x, y, z);
		item->floor = h;

		room_num = g_LaraItem->room_number;
		floor = GetFloor(g_LaraItem->pos.x, g_LaraItem->pos.y,
						 g_LaraItem->pos.z, &room_num);
		int32_t lh = GetHeight(floor, g_LaraItem->pos.x, g_LaraItem->pos.y,
							   g_LaraItem->pos.z);

		item->anim_number = g_LaraItem->anim_number;
		item->frame_number = g_LaraItem->frame_number;
		item->pos.x = x;
		item->pos.y = y;
		item->pos.z = z;
		item->pos.x_rot = g_LaraItem->pos.x_rot;
		item->pos.y_rot = g_LaraItem->pos.y_rot - PHD_180;
		item->pos.z_rot = g_LaraItem->pos.z_rot;
		ItemNewRoom(item_num, g_LaraItem->room_number);

		if (h >= lh + WALL_L && !g_LaraItem->gravity_status)
		{
			item->current_anim_state = AS_FASTFALL;
			item->goal_anim_state = AS_FASTFALL;
			item->anim_number = AA_FASTFALL;
			item->frame_number = AF_FASTFALL;
			item->speed = 0;
			item->fall_speed = 0;
			item->gravity_status = 1;
			item->data = (void *)-1;
			item->pos.y += 50;
		}
	}

	if (item->data)
	{
		AnimateItem(item);

		int32_t x = item->pos.x;
		int32_t y = item->pos.y;
		int32_t z = item->pos.z;

		int16_t room_num = item->room_number;
		FLOOR_INFO *floor = GetFloor(x, y, z, &room_num);
		int32_t h = GetHeight(floor, x, y, z);
		item->floor = h;

		TestTriggers(g_TriggerIndex, 1);
		if (item->pos.y >= h)
		{
			item->floor = h;
			item->pos.y = h;
			floor = GetFloor(x, h, z, &room_num);
			GetHeight(floor, x, h, z);
			TestTriggers(g_TriggerIndex, 1);
			item->gravity_status = 0;
			item->fall_speed = 0;
			item->goal_anim_state = AS_DEATH;
			item->required_anim_state = AS_DEATH;
		}
	}
}

void DrawEvilLara(ITEM_INFO *item)
{
	int16_t *old_mesh_ptrs[LM_NUMBER_OF];

	for (int i = 0; i < LM_NUMBER_OF; i++)
	{
		old_mesh_ptrs[i] = g_Lara.mesh_ptrs[i];
		g_Lara.mesh_ptrs[i] = g_Meshes[g_Objects[O_EVIL_LARA].mesh_index + i];
	}

	DrawLara(item);

	//for (int i = 0; i < LM_NUMBER_OF; i++)
	for (i = 0; i < LM_NUMBER_OF; i++)
	{
		g_Lara.mesh_ptrs[i] = old_mesh_ptrs[i];
	}
}

void AnimateItem(ITEM_INFO *item)
{
	item->touch_bits = 0;
	item->hit_status = 0;

	ANIM_STRUCT *anim = &g_Anims[item->anim_number];

	item->frame_number++;

	if (anim->number_changes > 0)
	{
		if (GetChange(item, anim))
		{
			anim = &g_Anims[item->anim_number];
			item->current_anim_state = anim->current_anim_state;

			if (item->required_anim_state == item->current_anim_state)
			{
				item->required_anim_state = 0;
			}
		}
	}

	if (item->frame_number > anim->frame_end)
	{
		if (anim->number_commands > 0)
		{
			int16_t *command = &g_AnimCommands[anim->command_index];

			for (int i = 0; i < anim->number_commands; i++)
			{
				switch (*command++)
				{
				case AC_MOVE_ORIGIN:
					TranslateItem(item, command[0], command[1], command[2]);
					command += 3;
					break;

				case AC_JUMP_VELOCITY:
					item->fall_speed = command[0];
					item->speed = command[1];
					item->gravity_status = 1;
					command += 2;
					break;

				case AC_DEACTIVATE:
					item->status = IS_DEACTIVATED;
					break;

				case AC_SOUND_FX:
				case AC_EFFECT:
					command += 2;
					break;
				}
			}
		}

		item->anim_number = anim->jump_anim_num;
		item->frame_number = anim->jump_frame_num;

		anim = &g_Anims[item->anim_number];
		item->current_anim_state = anim->current_anim_state;
		item->goal_anim_state = item->current_anim_state;

		if (item->required_anim_state == item->current_anim_state)
		{
			item->required_anim_state = 0;
		}
	}

	if (anim->number_commands > 0)
	{
		int16_t *command = &g_AnimCommands[anim->command_index];

		for (int i = 0; i < anim->number_commands; i++)
		{
			switch (*command++)
			{
			case AC_MOVE_ORIGIN:
				command += 3;
				break;

			case AC_JUMP_VELOCITY:
				command += 2;
				break;

			case AC_SOUND_FX:
				if (item->frame_number == command[0])
				{
					Sound_Effect(command[1], &item->pos,
								 g_RoomInfo[item->room_number].flags);
				}
				command += 2;
				break;

			case AC_EFFECT:
				if (item->frame_number == command[0])
				{
					g_EffectRoutines[command[1]](item);
				}
				command += 2;
				break;
			}
		}
	}

	if (!item->gravity_status)
	{
		int32_t speed = anim->velocity;
		if (anim->acceleration)
		{
			speed +=
				anim->acceleration * (item->frame_number - anim->frame_base);
		}
		item->speed = speed >> 16;
	}
	else
	{
		item->fall_speed += (item->fall_speed < FASTFALL_SPEED) ? GRAVITY : 1;
		item->pos.y += item->fall_speed;
	}

	item->pos.x += (phd_sin(item->pos.y_rot) * item->speed) >> W2V_SHIFT;
	item->pos.z += (phd_cos(item->pos.y_rot) * item->speed) >> W2V_SHIFT;
}

void TestTriggers(int16_t *data, int32_t heavy)
{
	if (!data)
	{
		return;
	}

	if ((*data & DATA_TYPE) == FT_LAVA)
	{
		if (!heavy && g_LaraItem->pos.y == g_LaraItem->floor)
		{
			LavaBurn(g_LaraItem);
		}

		if (*data & END_BIT)
		{
			return;
		}

		data++;
	}

	int16_t type = (*data++ >> 8) & 0x3F;
	int32_t switch_off = 0;
	int32_t flip = 0;
	int32_t new_effect = -1;
	int16_t flags = *data++;
	int16_t timer = flags & 0xFF;

	if (g_Camera.type != CAM_HEAVY)
	{
		RefreshCamera(type, data);
	}

	if (heavy)
	{
		if (type != TT_HEAVY)
		{
			return;
		}
	}
	else
	{
		switch (type)
		{
		case TT_SWITCH:
		{
			int16_t value = *data++ & VALUE_BITS;
			if (!SwitchTrigger(value, timer))
			{
				return;
			}
			switch_off = g_Items[value].current_anim_state == AS_RUN;
			break;
		}

		case TT_PAD:
		case TT_ANTIPAD:
			if (g_LaraItem->pos.y != g_LaraItem->floor)
			{
				return;
			}
			break;

		case TT_KEY:
		{
			int16_t value = *data++ & VALUE_BITS;

			if (!KeyTrigger(value))
			{
				return;
			}
			break;
		}

		case TT_PICKUP:
		{
			int16_t value = *data++ & VALUE_BITS;

			if (!PickupTrigger(value))
			{
				return;
			}
			break;
		}

		case TT_HEAVY:
		case TT_DUMMY:
			return;

		case TT_COMBAT:
			if (g_Lara.gun_status != LGS_READY)
			{
				return;
			}
			break;
		}
	}

	ITEM_INFO *camera_item = NULL;
	int16_t trigger;
	do
	{
		trigger = *data++;
		int16_t value = trigger & VALUE_BITS;

		switch (TRIG_BITS(trigger))
		{
		case TO_OBJECT:
		{
			ITEM_INFO *item = &g_Items[value];

			if (item->flags & IF_ONESHOT)
			{
				break;
			}

			item->timer = timer;

			if (timer != 1)
			{
				item->timer *= FRAMES_PER_SECOND;
			}

			if (type == TT_SWITCH)
			{
				item->flags ^= flags & IF_CODE_BITS;
			}
			else if (type == TT_ANTIPAD)
			{
				item->flags &= -1 - (flags & IF_CODE_BITS);
			}
			else if (flags & IF_CODE_BITS)
			{
				item->flags |= flags & IF_CODE_BITS;
			}

			if ((item->flags & IF_CODE_BITS) != IF_CODE_BITS)
			{
				break;
			}

			if (flags & IF_ONESHOT)
			{
				item->flags |= IF_ONESHOT;
			}

			if (!item->active)
			{
				if (g_Objects[item->object_number].intelligent)
				{
					if (item->status == IS_NOT_ACTIVE)
					{
						item->touch_bits = 0;
						item->status = IS_ACTIVE;
						AddActiveItem(value);
						EnableBaddieAI(value, 1);
					}
					else if (item->status == IS_INVISIBLE)
					{
						item->touch_bits = 0;
						if (EnableBaddieAI(value, 0))
						{
							item->status = IS_ACTIVE;
						}
						else
						{
							item->status = IS_INVISIBLE;
						}

						AddActiveItem(value);
					}
				}
				else
				{
					item->touch_bits = 0;
					item->status = IS_ACTIVE;
					AddActiveItem(value);
				}
			}
			break;
		}

		case TO_CAMERA:
		{
			trigger = *data++;
			int16_t camera_flags = trigger;
			int16_t camera_timer = trigger & 0xFF;

			if (g_Camera.fixed[value].flags & IF_ONESHOT)
			{
				break;
			}

			g_Camera.number = value;

			if (g_Camera.type == CAM_LOOK || g_Camera.type == CAM_COMBAT)
			{
				break;
			}

			if (type == TT_COMBAT)
			{
				break;
			}

			if (type == TT_SWITCH && timer && switch_off)
			{
				break;
			}

			if (g_Camera.number == g_Camera.last && type != TT_SWITCH)
			{
				break;
			}

			g_Camera.timer = camera_timer;
			if (g_Camera.timer != 1)
			{
				g_Camera.timer *= FRAMES_PER_SECOND;
			}

			if (camera_flags & IF_ONESHOT)
			{
				g_Camera.fixed[g_Camera.number].flags |= IF_ONESHOT;
			}

			g_Camera.speed = ((camera_flags & IF_CODE_BITS) >> 6) + 1;
			g_Camera.type = heavy ? CAM_HEAVY : CAM_FIXED;
			break;
		}

		case TO_TARGET:
			camera_item = &g_Items[value];
			break;

		case TO_SINK:
		{
			OBJECT_VECTOR *obvector = &g_Camera.fixed[value];

			if (g_Lara.LOT.required_box != obvector->flags)
			{
				g_Lara.LOT.target.x = obvector->x;
				g_Lara.LOT.target.y = obvector->y;
				g_Lara.LOT.target.z = obvector->z;
				g_Lara.LOT.required_box = obvector->flags;
			}

			g_Lara.current_active = obvector->data * 6;
			break;
		}

		case TO_FLIPMAP:
			if (g_FlipMapTable[value] & IF_ONESHOT)
			{
				break;
			}

			if (type == TT_SWITCH)
			{
				g_FlipMapTable[value] ^= flags & IF_CODE_BITS;
			}
			else if (flags & IF_CODE_BITS)
			{
				g_FlipMapTable[value] |= flags & IF_CODE_BITS;
			}

			if ((g_FlipMapTable[value] & IF_CODE_BITS) == IF_CODE_BITS)
			{
				if (flags & IF_ONESHOT)
				{
					g_FlipMapTable[value] |= IF_ONESHOT;
				}

				if (!g_FlipStatus)
				{
					flip = 1;
				}
			}
			else if (g_FlipStatus)
			{
				flip = 1;
			}
			break;

		case TO_FLIPON:
			if ((g_FlipMapTable[value] & IF_CODE_BITS) == IF_CODE_BITS &&
				!g_FlipStatus)
			{
				flip = 1;
			}
			break;

		case TO_FLIPOFF:
			if ((g_FlipMapTable[value] & IF_CODE_BITS) == IF_CODE_BITS &&
				g_FlipStatus)
			{
				flip = 1;
			}
			break;

		case TO_FLIPEFFECT:
			new_effect = value;
			break;

		case TO_FINISH:
			g_LevelComplete = true;
			break;

		case TO_CD:
			// Play CD Music Track
			break;

		case TO_SECRET:
			if ((g_SaveGame.secrets & (1 << value)))
			{
				break;
			}
			g_SaveGame.secrets |= 1 << value;
			Sound_Effect(SFX_SECRET, NULL, SPM_ALWAYS);
			break;
		}
	} while (!(trigger & END_BIT));

	if (camera_item &&
		(g_Camera.type == CAM_FIXED || g_Camera.type == CAM_HEAVY))
	{
		g_Camera.item = camera_item;
	}

	if (flip)
	{
		FlipMap();

		if (new_effect != -1)
		{
			g_FlipEffect = new_effect;
			g_FlipTimer = 0;
		}
	}
}

void DrawLara(ITEM_INFO *item)
{
	OBJECT_INFO *object;
	int16_t *frame;
	int16_t *frmptr[2];
	PHD_MATRIX saved_matrix;

	int32_t top = g_PhdTop;
	int32_t left = g_PhdLeft;
	int32_t bottom = g_PhdBottom;
	int32_t right = g_PhdRight;

	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;

	if (g_Lara.hit_direction < 0)
	{
		int32_t rate;
		int32_t frac = GetFrames(item, frmptr, &rate);
		if (frac)
		{
			DrawLaraInt(item, frmptr[0], frmptr[1], frac, rate);
			g_PhdLeft = left;
			g_PhdRight = right;
			g_PhdTop = top;
			g_PhdBottom = bottom;
			return;
		}
	}

	object = &g_Objects[item->object_number];

	if (g_Lara.hit_direction >= 0)
	{
		switch (g_Lara.hit_direction)
		{
		default:
		case DIR_NORTH:
			frame = g_Anims[AA_SPAZ_FORWARD].frame_ptr;
			break;
		case DIR_EAST:
			frame = g_Anims[AA_SPAZ_RIGHT].frame_ptr;
			break;
		case DIR_SOUTH:
			frame = g_Anims[AA_SPAZ_BACK].frame_ptr;
			break;
		case DIR_WEST:
			frame = g_Anims[AA_SPAZ_LEFT].frame_ptr;
			break;
		}

		frame += g_Lara.hit_frame * (object->nmeshes * 2 + FRAME_ROT);
	}
	else
	{
		frame = frmptr[0];
	}

	// save matrix for hair
	saved_matrix = *g_PhdMatrixPtr;

	Output_DrawShadow(object->shadow_size, frame, item);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	int32_t clip = S_GetObjectBounds(frame);
	if (!clip)
	{
		phd_PopMatrix();
		return;
	}

	phd_PushMatrix();

	CalculateObjectLighting(item, frame);

	int32_t *bone = &g_AnimBones[object->bone_index];
	int32_t *packed_rotation = (int32_t *)(frame + FRAME_ROT);

	phd_TranslateRel(frame[FRAME_POS_X], frame[FRAME_POS_Y],
					 frame[FRAME_POS_Z]);
	phd_RotYXZpack(packed_rotation[LM_HIPS]);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HIPS], clip);

	phd_PushMatrix();

	phd_TranslateRel(bone[1], bone[2], bone[3]);
	phd_RotYXZpack(packed_rotation[LM_THIGH_L]);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_THIGH_L], clip);

	phd_TranslateRel(bone[5], bone[6], bone[7]);
	phd_RotYXZpack(packed_rotation[LM_CALF_L]);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_CALF_L], clip);

	phd_TranslateRel(bone[9], bone[10], bone[11]);
	phd_RotYXZpack(packed_rotation[LM_FOOT_L]);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_FOOT_L], clip);

	phd_PopMatrix();

	phd_PushMatrix();

	phd_TranslateRel(bone[13], bone[14], bone[15]);
	phd_RotYXZpack(packed_rotation[LM_THIGH_R]);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_THIGH_R], clip);

	phd_TranslateRel(bone[17], bone[18], bone[19]);
	phd_RotYXZpack(packed_rotation[LM_CALF_R]);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_CALF_R], clip);

	phd_TranslateRel(bone[21], bone[22], bone[23]);
	phd_RotYXZpack(packed_rotation[LM_FOOT_R]);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_FOOT_R], clip);

	phd_PopMatrix();

	phd_TranslateRel(bone[25], bone[26], bone[27]);
	phd_RotYXZpack(packed_rotation[LM_TORSO]);
	phd_RotYXZ(g_Lara.torso_y_rot, g_Lara.torso_x_rot, g_Lara.torso_z_rot);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_TORSO], clip);

	phd_PushMatrix();

	phd_TranslateRel(bone[53], bone[54], bone[55]);
	phd_RotYXZpack(packed_rotation[LM_HEAD]);
	phd_RotYXZ(g_Lara.head_y_rot, g_Lara.head_x_rot, g_Lara.head_z_rot);
	Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HEAD], clip);

	*g_PhdMatrixPtr = saved_matrix;
	DrawHair();

	phd_PopMatrix();

	int32_t fire_arms = 0;

	if (g_Lara.gun_status == LGS_READY || g_Lara.gun_status == LGS_DRAW ||
		g_Lara.gun_status == LGS_UNDRAW)
	{
		fire_arms = g_Lara.gun_type;
	}

	switch (fire_arms)
	{
	case LGT_UNARMED:
		phd_PushMatrix();

		phd_TranslateRel(bone[29], bone[30], bone[31]);
		phd_RotYXZpack(packed_rotation[LM_UARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		phd_RotYXZpack(packed_rotation[LM_LARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		phd_RotYXZpack(packed_rotation[LM_HAND_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

		phd_PopMatrix();

		phd_PushMatrix();

		phd_TranslateRel(bone[41], bone[42], bone[43]);
		phd_RotYXZpack(packed_rotation[LM_UARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[11], clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		phd_RotYXZpack(packed_rotation[LM_LARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		phd_RotYXZpack(packed_rotation[LM_HAND_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

		phd_PopMatrix();
		break;

	case LGT_PISTOLS:
	case LGT_MAGNUMS:
	case LGT_UZIS:
		phd_PushMatrix();

		phd_TranslateRel(bone[29], bone[30], bone[31]);

		g_PhdMatrixPtr->_00 = g_PhdMatrixPtr[-2]._00;
		g_PhdMatrixPtr->_01 = g_PhdMatrixPtr[-2]._01;
		g_PhdMatrixPtr->_02 = g_PhdMatrixPtr[-2]._02;
		g_PhdMatrixPtr->_10 = g_PhdMatrixPtr[-2]._10;
		g_PhdMatrixPtr->_11 = g_PhdMatrixPtr[-2]._11;
		g_PhdMatrixPtr->_12 = g_PhdMatrixPtr[-2]._12;
		g_PhdMatrixPtr->_20 = g_PhdMatrixPtr[-2]._20;
		g_PhdMatrixPtr->_21 = g_PhdMatrixPtr[-2]._21;
		g_PhdMatrixPtr->_22 = g_PhdMatrixPtr[-2]._22;

		packed_rotation = (int32_t *)(g_Lara.right_arm.frame_base +
									  g_Lara.right_arm.frame_number *
										  (object->nmeshes * 2 + FRAME_ROT) +
									  10);
		phd_RotYXZ(g_Lara.right_arm.y_rot, g_Lara.right_arm.x_rot,
				   g_Lara.right_arm.z_rot);
		phd_RotYXZpack(packed_rotation[LM_UARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		phd_RotYXZpack(packed_rotation[LM_LARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		phd_RotYXZpack(packed_rotation[LM_HAND_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

		if (g_Lara.right_arm.flash_gun)
		{
			saved_matrix = *g_PhdMatrixPtr;
		}

		phd_PopMatrix();

		phd_PushMatrix();

		phd_TranslateRel(bone[41], bone[42], bone[43]);

		g_PhdMatrixPtr->_00 = g_PhdMatrixPtr[-2]._00;
		g_PhdMatrixPtr->_01 = g_PhdMatrixPtr[-2]._01;
		g_PhdMatrixPtr->_02 = g_PhdMatrixPtr[-2]._02;
		g_PhdMatrixPtr->_10 = g_PhdMatrixPtr[-2]._10;
		g_PhdMatrixPtr->_11 = g_PhdMatrixPtr[-2]._11;
		g_PhdMatrixPtr->_12 = g_PhdMatrixPtr[-2]._12;
		g_PhdMatrixPtr->_20 = g_PhdMatrixPtr[-2]._20;
		g_PhdMatrixPtr->_21 = g_PhdMatrixPtr[-2]._21;
		g_PhdMatrixPtr->_22 = g_PhdMatrixPtr[-2]._22;

		packed_rotation = (int32_t *)(g_Lara.left_arm.frame_base +
									  g_Lara.left_arm.frame_number *
										  (object->nmeshes * 2 + FRAME_ROT) +
									  10);
		phd_RotYXZ(g_Lara.left_arm.y_rot, g_Lara.left_arm.x_rot,
				   g_Lara.left_arm.z_rot);
		phd_RotYXZpack(packed_rotation[LM_UARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		phd_RotYXZpack(packed_rotation[LM_LARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		phd_RotYXZpack(packed_rotation[LM_HAND_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

		if (g_Lara.left_arm.flash_gun)
		{
			DrawGunFlash(fire_arms, clip);
		}
		if (g_Lara.right_arm.flash_gun)
		{
			*g_PhdMatrixPtr = saved_matrix;
			DrawGunFlash(fire_arms, clip);
		}

		phd_PopMatrix();
		break;

	case LGT_SHOTGUN:
		phd_PushMatrix();

		packed_rotation = (int32_t *)(g_Lara.right_arm.frame_base +
									  g_Lara.right_arm.frame_number *
										  (object->nmeshes * 2 + FRAME_ROT) +
									  10);
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		phd_RotYXZpack(packed_rotation[LM_UARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		phd_RotYXZpack(packed_rotation[LM_LARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		phd_RotYXZpack(packed_rotation[LM_HAND_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

		if (g_Lara.right_arm.flash_gun)
		{
			saved_matrix = *g_PhdMatrixPtr;
		}

		phd_PopMatrix();

		phd_PushMatrix();

		packed_rotation = (int32_t *)(g_Lara.left_arm.frame_base +
									  g_Lara.left_arm.frame_number *
										  (object->nmeshes * 2 + FRAME_ROT) +
									  10);
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		phd_RotYXZpack(packed_rotation[LM_UARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		phd_RotYXZpack(packed_rotation[LM_LARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		phd_RotYXZpack(packed_rotation[LM_HAND_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

		if (g_Lara.right_arm.flash_gun)
		{
			*g_PhdMatrixPtr = saved_matrix;
			DrawGunFlash(fire_arms, clip);
		}

		phd_PopMatrix();
		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
	g_PhdLeft = left;
	g_PhdRight = right;
	g_PhdTop = top;
	g_PhdBottom = bottom;
}

void RefreshCamera(int16_t type, int16_t *data)
{
	int16_t trigger;
	int16_t target_ok = 2;
	do
	{
		trigger = *data++;
		int16_t value = trigger & VALUE_BITS;

		switch (TRIG_BITS(trigger))
		{
		case TO_CAMERA:
			data++;

			if (value == g_Camera.last)
			{
				g_Camera.number = value;

				if (g_Camera.timer < 0 || g_Camera.type == CAM_LOOK ||
					g_Camera.type == CAM_COMBAT)
				{
					g_Camera.timer = -1;
					target_ok = 0;
				}
				else
				{
					g_Camera.type = CAM_FIXED;
					target_ok = 1;
				}
			}
			else
			{
				target_ok = 0;
			}
			break;

		case TO_TARGET:
			if (g_Camera.type != CAM_LOOK && g_Camera.type != CAM_COMBAT)
			{
				g_Camera.item = &g_Items[value];
			}
			break;
		}
	} while (!(trigger & END_BIT));

	if (g_Camera.item != NULL)
	{
		if (!target_ok || (target_ok == 2 && g_Camera.item->looked_at &&
						   g_Camera.item != g_Camera.last_item))
		{
			g_Camera.item = NULL;
		}
	}
}

void FlipMap()
{
	// StopAmbientSounds();

	for (int i = 0; i < g_RoomCount; i++)
	{
		ROOM_INFO *r = &g_RoomInfo[i];

		if (r->flipped_room < 0)
		{
			continue;
		}

		RemoveRoomFlipItems(r);

		ROOM_INFO *flipped = &g_RoomInfo[r->flipped_room];
		ROOM_INFO temp = *r;
		*r = *flipped;
		*flipped = temp;

		r->flipped_room = flipped->flipped_room;
		flipped->flipped_room = -1;

		// XXX: is this really necessary given the assignments above?
		r->item_number = flipped->item_number;
		r->fx_number = flipped->fx_number;

		AddRoomFlipItems(r);
	}

	g_FlipStatus = !g_FlipStatus;
}

void DrawHair()
{
	if (!g_Objects[O_HAIR].loaded)
	{
		return;
	}

	OBJECT_INFO *object = &g_Objects[O_HAIR];
	int16_t **mesh = &g_Meshes[object->mesh_index];

	for (int i = 0; i < HAIR_SEGMENTS; i++)
	{
		phd_PushMatrix();

		phd_TranslateAbs(m_Hair[i].x, m_Hair[i].y, m_Hair[i].z);
		phd_RotY(m_Hair[i].y_rot);
		phd_RotX(m_Hair[i].x_rot);
		Output_DrawPolygons(*mesh++, 1);

		phd_PopMatrix();
	}
}

void DrawGunFlash(int32_t weapon_type, int32_t clip)
{
	int32_t light;
	int32_t len;
	int32_t off;

	switch (weapon_type)
	{
	case LGT_MAGNUMS:
		light = 16 * 256;
		len = 155;
		off = 55;
		break;

	case LGT_UZIS:
		light = 10 * 256;
		len = 180;
		off = 55;
		break;

	case LGT_SHOTGUN:
		light = 10 * 256;
		len = 285;
		off = 0;
		break;

	default:
		light = 20 * 256;
		len = 155;
		off = 55;
		break;
	}

	phd_TranslateRel(0, len, off);
	phd_RotYXZ(0, -90 * PHD_DEGREE, (PHD_ANGLE)(Random_GetDraw() * 2));
	Output_CalculateStaticLight(light);
	Output_DrawPolygons(g_Meshes[g_Objects[O_GUN_FLASH].mesh_index], clip);
}

void TranslateItem(ITEM_INFO *item, int32_t x, int32_t y, int32_t z)
{
	int32_t c = phd_cos(item->pos.y_rot);
	int32_t s = phd_sin(item->pos.y_rot);

	item->pos.x += (c * x + s * z) >> W2V_SHIFT;
	item->pos.y += y;
	item->pos.z += (c * z - s * x) >> W2V_SHIFT;
}

void DrawLaraInt(ITEM_INFO *item, int16_t *frame1, int16_t *frame2,
				 int32_t frac, int32_t rate)
{
	PHD_MATRIX saved_matrix;

	OBJECT_INFO *object = &g_Objects[item->object_number];
	int16_t *bounds = GetBoundsAccurate(item);

	saved_matrix = *g_PhdMatrixPtr;

	Output_DrawShadow(object->shadow_size, bounds, item);
	phd_PushMatrix();
	phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
	phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

	int32_t clip = S_GetObjectBounds(frame1);
	if (!clip)
	{
		phd_PopMatrix();
		return;
	}

	phd_PushMatrix();

	CalculateObjectLighting(item, frame1);

	int32_t *bone = &g_AnimBones[object->bone_index];
	int32_t *packed_rotation1 = (int32_t *)(frame1 + FRAME_ROT);
	int32_t *packed_rotation2 = (int32_t *)(frame2 + FRAME_ROT);

	InitInterpolate(frac, rate);

	phd_TranslateRel_ID(frame1[FRAME_POS_X], frame1[FRAME_POS_Y],
						frame1[FRAME_POS_Z], frame2[FRAME_POS_X],
						frame2[FRAME_POS_Y], frame2[FRAME_POS_Z]);

	phd_RotYXZpack_I(packed_rotation1[LM_HIPS], packed_rotation2[LM_HIPS]);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_HIPS], clip);

	phd_PushMatrix_I();

	phd_TranslateRel_I(bone[1], bone[2], bone[3]);
	phd_RotYXZpack_I(packed_rotation1[LM_THIGH_L],
					 packed_rotation2[LM_THIGH_L]);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_THIGH_L], clip);

	phd_TranslateRel_I(bone[5], bone[6], bone[7]);
	phd_RotYXZpack_I(packed_rotation1[LM_CALF_L], packed_rotation2[LM_CALF_L]);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_CALF_L], clip);

	phd_TranslateRel_I(bone[9], bone[10], bone[11]);
	phd_RotYXZpack_I(packed_rotation1[LM_FOOT_L], packed_rotation2[LM_FOOT_L]);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_FOOT_L], clip);

	phd_PopMatrix_I();

	phd_PushMatrix_I();

	phd_TranslateRel_I(bone[13], bone[14], bone[15]);
	phd_RotYXZpack_I(packed_rotation1[LM_THIGH_R],
					 packed_rotation2[LM_THIGH_R]);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_THIGH_R], clip);

	phd_TranslateRel_I(bone[17], bone[18], bone[19]);
	phd_RotYXZpack_I(packed_rotation1[LM_CALF_R], packed_rotation2[LM_CALF_R]);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_CALF_R], clip);

	phd_TranslateRel_I(bone[21], bone[22], bone[23]);
	phd_RotYXZpack_I(packed_rotation1[LM_FOOT_R], packed_rotation2[LM_FOOT_R]);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_FOOT_R], clip);

	phd_PopMatrix_I();

	phd_TranslateRel_I(bone[25], bone[26], bone[27]);
	phd_RotYXZpack_I(packed_rotation1[LM_TORSO], packed_rotation2[LM_TORSO]);
	phd_RotYXZ_I(g_Lara.torso_y_rot, g_Lara.torso_x_rot, g_Lara.torso_z_rot);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_TORSO], clip);

	phd_PushMatrix_I();

	phd_TranslateRel_I(bone[53], bone[54], bone[55]);
	phd_RotYXZpack_I(packed_rotation1[LM_HEAD], packed_rotation2[LM_HEAD]);
	phd_RotYXZ_I(g_Lara.head_y_rot, g_Lara.head_x_rot, g_Lara.head_z_rot);
	Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_HEAD], clip);

	*g_PhdMatrixPtr = saved_matrix;
	DrawHair();

	phd_PopMatrix_I();

	int32_t fire_arms = 0;
	if (g_Lara.gun_status == LGS_READY || g_Lara.gun_status == LGS_DRAW ||
		g_Lara.gun_status == LGS_UNDRAW)
	{
		fire_arms = g_Lara.gun_type;
	}

	switch (fire_arms)
	{
	case LGT_UNARMED:
		phd_PushMatrix_I();

		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		phd_RotYXZpack_I(packed_rotation1[LM_UARM_R],
						 packed_rotation2[LM_UARM_R]);
		Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_UARM_R], clip);

		phd_TranslateRel_I(bone[33], bone[34], bone[35]);
		phd_RotYXZpack_I(packed_rotation1[LM_LARM_R],
						 packed_rotation2[LM_LARM_R]);
		Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_LARM_R], clip);

		phd_TranslateRel_I(bone[37], bone[38], bone[39]);
		phd_RotYXZpack_I(packed_rotation1[LM_HAND_R],
						 packed_rotation2[LM_HAND_R]);
		Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_HAND_R], clip);

		phd_PopMatrix_I();

		phd_PushMatrix_I();

		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		phd_RotYXZpack_I(packed_rotation1[LM_UARM_L],
						 packed_rotation2[LM_UARM_L]);
		Output_DrawPolygons_I(g_Lara.mesh_ptrs[11], clip);

		phd_TranslateRel_I(bone[45], bone[46], bone[47]);
		phd_RotYXZpack_I(packed_rotation1[LM_LARM_L],
						 packed_rotation2[LM_LARM_L]);
		Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_LARM_L], clip);

		phd_TranslateRel_I(bone[49], bone[50], bone[51]);
		phd_RotYXZpack_I(packed_rotation1[LM_HAND_L],
						 packed_rotation2[LM_HAND_L]);
		Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_HAND_L], clip);

		phd_PopMatrix_I();
		break;

	case LGT_PISTOLS:
	case LGT_MAGNUMS:
	case LGT_UZIS:
		phd_PushMatrix_I();

		phd_TranslateRel_I(bone[29], bone[30], bone[31]);
		InterpolateArmMatrix();

		packed_rotation1 = (int32_t *)(g_Lara.right_arm.frame_base +
									   g_Lara.right_arm.frame_number *
										   (object->nmeshes * 2 + FRAME_ROT) +
									   10);
		phd_RotYXZ(g_Lara.right_arm.y_rot, g_Lara.right_arm.x_rot,
				   g_Lara.right_arm.z_rot);
		phd_RotYXZpack(packed_rotation1[LM_UARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		phd_RotYXZpack(packed_rotation1[LM_LARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		phd_RotYXZpack(packed_rotation1[LM_HAND_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

		if (g_Lara.right_arm.flash_gun)
		{
			saved_matrix = *g_PhdMatrixPtr;
		}

		phd_PopMatrix_I();

		phd_PushMatrix_I();

		phd_TranslateRel_I(bone[41], bone[42], bone[43]);
		InterpolateArmMatrix();

		packed_rotation1 = (int32_t *)(g_Lara.left_arm.frame_base +
									   g_Lara.left_arm.frame_number *
										   (object->nmeshes * 2 + FRAME_ROT) +
									   10);
		phd_RotYXZ(g_Lara.left_arm.y_rot, g_Lara.left_arm.x_rot,
				   g_Lara.left_arm.z_rot);
		phd_RotYXZpack(packed_rotation1[LM_UARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		phd_RotYXZpack(packed_rotation1[LM_LARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		phd_RotYXZpack(packed_rotation1[LM_HAND_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

		if (g_Lara.left_arm.flash_gun)
		{
			DrawGunFlash(fire_arms, clip);
		}

		if (g_Lara.right_arm.flash_gun)
		{
			*g_PhdMatrixPtr = saved_matrix;
			DrawGunFlash(fire_arms, clip);
		}

		phd_PopMatrix_I();
		break;

	case LGT_SHOTGUN:
		phd_PushMatrix_I();
		InterpolateMatrix();

		packed_rotation1 = (int32_t *)(g_Lara.right_arm.frame_base +
									   g_Lara.right_arm.frame_number *
										   (object->nmeshes * 2 + FRAME_ROT) +
									   10);
		phd_TranslateRel(bone[29], bone[30], bone[31]);
		phd_RotYXZpack(packed_rotation1[LM_UARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

		phd_TranslateRel(bone[33], bone[34], bone[35]);
		phd_RotYXZpack(packed_rotation1[LM_LARM_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

		phd_TranslateRel(bone[37], bone[38], bone[39]);
		phd_RotYXZpack(packed_rotation1[LM_HAND_R]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

		if (g_Lara.right_arm.flash_gun)
		{
			saved_matrix = *g_PhdMatrixPtr;
		}

		phd_PopMatrix();

		phd_PushMatrix();

		packed_rotation1 = (int32_t *)(g_Lara.left_arm.frame_base +
									   g_Lara.left_arm.frame_number *
										   (object->nmeshes * 2 + FRAME_ROT) +
									   10);
		phd_TranslateRel(bone[41], bone[42], bone[43]);
		phd_RotYXZpack(packed_rotation1[LM_UARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

		phd_TranslateRel(bone[45], bone[46], bone[47]);
		phd_RotYXZpack(packed_rotation1[LM_LARM_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

		phd_TranslateRel(bone[49], bone[50], bone[51]);
		phd_RotYXZpack(packed_rotation1[LM_HAND_L]);
		Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

		if (g_Lara.right_arm.flash_gun)
		{
			*g_PhdMatrixPtr = saved_matrix;
			DrawGunFlash(fire_arms, clip);
		}

		phd_PopMatrix_I();
		break;
	}

	phd_PopMatrix();
	phd_PopMatrix();
}

void RemoveRoomFlipItems(ROOM_INFO *r)
{
	for (int16_t item_num = r->item_number; item_num != NO_ITEM;
		 item_num = g_Items[item_num].next_item)
	{
		ITEM_INFO *item = &g_Items[item_num];

		switch (item->object_number)
		{
		case O_MOVABLE_BLOCK:
		case O_MOVABLE_BLOCK2:
		case O_MOVABLE_BLOCK3:
		case O_MOVABLE_BLOCK4:
			AlterFloorHeight(item, WALL_L);
			break;

		case O_ROLLING_BLOCK:
			AlterFloorHeight(item, WALL_L * 2);
			break;
		}
	}
}

void AddRoomFlipItems(ROOM_INFO *r)
{
	for (int16_t item_num = r->item_number; item_num != NO_ITEM;
		 item_num = g_Items[item_num].next_item)
	{
		ITEM_INFO *item = &g_Items[item_num];

		switch (item->object_number)
		{
		case O_MOVABLE_BLOCK:
		case O_MOVABLE_BLOCK2:
		case O_MOVABLE_BLOCK3:
		case O_MOVABLE_BLOCK4:
			AlterFloorHeight(item, -WALL_L);
			break;

		case O_ROLLING_BLOCK:
			AlterFloorHeight(item, -WALL_L * 2);
			break;
		}
	}
}

int16_t *GetBoundsAccurate(ITEM_INFO *item)
{
	int32_t rate;
	int16_t *frmptr[2];

	int32_t frac = GetFrames(item, frmptr, &rate);
	if (!frac)
	{
		return frmptr[0];
	}

	for (int i = 0; i < 6; i++)
	{
		int16_t a = frmptr[0][i];
		int16_t b = frmptr[1][i];
		m_InterpolatedBounds[i] = a + (((b - a) * frac) / rate);
	}
	return m_InterpolatedBounds;
}

int16_t *GetBestFrame(ITEM_INFO *item)
{
	int16_t *frmptr[2];
	int32_t rate;
	int32_t frac = GetFrames(item, frmptr, &rate);

	if (frac <= rate / 2)
	{
		return frmptr[0];
	}
	else
	{
		return frmptr[1];
	}
}

int16_t *CalcRoomVertices(int16_t *obj_ptr)
{

	int32_t vertex_count = *obj_ptr++;

	for (int i = 0; i < vertex_count; i++)
	{
		int32_t xv = g_PhdMatrixPtr->_00 * obj_ptr[0] +
					 g_PhdMatrixPtr->_01 * obj_ptr[1] +
					 g_PhdMatrixPtr->_02 * obj_ptr[2] + g_PhdMatrixPtr->_03;

		int32_t yv = g_PhdMatrixPtr->_10 * obj_ptr[0] +
					 g_PhdMatrixPtr->_11 * obj_ptr[1] +
					 g_PhdMatrixPtr->_12 * obj_ptr[2] + g_PhdMatrixPtr->_13;

		int32_t zv = g_PhdMatrixPtr->_20 * obj_ptr[0] +
					 g_PhdMatrixPtr->_21 * obj_ptr[1] +
					 g_PhdMatrixPtr->_22 * obj_ptr[2] + g_PhdMatrixPtr->_23;

		m_VBuf[i].xv = (float)xv;
		m_VBuf[i].yv = (float)yv;
		m_VBuf[i].zv = (float)zv;
		m_VBuf[i].g = obj_ptr[3];



		float one = (256.0f * 8.0f * 16384.0f);

		float z = ((float)(g_PhdPersp)) / zv;
		//m_VBuf[i].ooz = z * (one / g_PhdPersp);

		m_VBuf[i].ooz = ((float)(one)) / zv;

		if (zv < Z_NEAR)
		// if (zv < ZNear)
		{
			m_VBuf[i].clip = (int16_t)0x8000;
		}
		else
		{
			int16_t clip_flags = 0;
			int32_t depth = zv >> W2V_SHIFT;

			if (depth > DRAW_DIST_MAX)
			{
				m_VBuf[i].g = 0x1FFF;
				clip_flags |= 16;
			}
			else if (depth)
			{
				m_VBuf[i].g += CalcFogShade(depth);

				if (!g_IsWaterEffect)
				{
					CLAMPG(m_VBuf[i].g, 0x1FFF);
				}
			}

			int32_t xs = ViewPort_GetCenterX() + xv / (zv / g_PhdPersp);
			int32_t ys = ViewPort_GetCenterY() + yv / (zv / g_PhdPersp);

			if (g_IsWibbleEffect)
			{
				xs += g_WibbleTable[(ys + g_WibbleOffset) & 0x1F];
				ys += g_WibbleTable[(xs + g_WibbleOffset) & 0x1F];
			}

			if (xs < g_PhdLeft)
			{
				if (xs < -32760)
				{
					xs = -32760;
				}
				clip_flags |= 1;
			}
			else if (xs > g_PhdRight)
			{
				if (xs > 32760)
				{
					xs = 32760;
				}
				clip_flags |= 2;
			}

			if (ys < g_PhdTop)
			{
				if (ys < -32760)
				{
					ys = -32760;
				}
				clip_flags |= 4;
			}
			else if (ys > g_PhdBottom)
			{
				if (ys > 32760)
				{
					ys = 32760;
				}
				clip_flags |= 8;
			}

			if (g_IsWaterEffect)
			{
				m_VBuf[i].g += g_ShadeTable[(
					((uint8_t)g_WibbleOffset +
					 (uint8_t)g_RandTable[(vertex_count - i) % WIBBLE_SIZE]) %
					WIBBLE_SIZE)];

				CLAMP(m_VBuf[i].g, 0, 0x1FFF);
			}

			m_VBuf[i].xs = (float)xs;
			m_VBuf[i].ys = (float)ys;
			m_VBuf[i].clip = clip_flags;
		}
		obj_ptr += 4;
	}

	return obj_ptr;
}

int32_t TriggerActive(ITEM_INFO *item)
{
	int32_t ok = (item->flags & IF_REVERSE) ? 0 : 1;

	if ((item->flags & IF_CODE_BITS) != IF_CODE_BITS)
	{
		return !ok;
	}

	if (!item->timer)
	{
		return ok;
	}

	if (item->timer == -1)
	{
		return !ok;
	}

	item->timer--;

	if (!item->timer)
	{
		item->timer = -1;
	}

	return ok;
}

void DrawUnclippedItem(ITEM_INFO *item)
{
	int32_t left = g_PhdLeft;
	int32_t top = g_PhdTop;
	int32_t right = g_PhdRight;
	int32_t bottom = g_PhdBottom;

	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;

	DrawAnimatingItem(item);

	g_PhdLeft = left;
	g_PhdTop = top;
	g_PhdRight = right;
	g_PhdBottom = bottom;
}

void DrawPickupItem(ITEM_INFO *item) { DrawSpriteItem(item); }

void DrawSpriteItem(ITEM_INFO *item)
{
	Output_DrawSprite(item->pos.x, item->pos.y, item->pos.z,
					  g_Objects[item->object_number].mesh_index -
						  item->frame_number,
					  item->shade);
}

void Output_DrawScreenSprite(int32_t sx, int32_t sy, int32_t z, int32_t scale_h,
							 int32_t scale_v, int32_t sprnum, int16_t shade,
							 uint16_t flags)
{
	PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];
	int32_t x1 = sx + (scale_h * (sprite->x1 >> 3) / PHD_ONE);
	int32_t x2 = sx + (scale_h * (sprite->x2 >> 3) / PHD_ONE);
	int32_t y1 = sy + (scale_v * (sprite->y1 >> 3) / PHD_ONE);
	int32_t y2 = sy + (scale_v * (sprite->y2 >> 3) / PHD_ONE);

	if (x2 >= 0 && y2 >= 0 && x1 < Screen_GetResWidth() &&
		y1 < Screen_GetResHeight())
	{
		S_Output_DrawSprite(x1, y1, x2, y2, 8 * z, sprnum, 0);
	}
}

//функция рисует текст на экране
void Output_DrawScreenSprite2D(int32_t sx, int32_t sy, int32_t z,
							   int32_t scale_h, int32_t scale_v, int32_t sprnum,
							   int16_t shade, uint16_t flags, int32_t page)
{
	PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];

	int32_t x1 = sx + (scale_h * sprite->x1 / PHD_ONE);
	int32_t x2 = sx + (scale_h * sprite->x2 / PHD_ONE);
	int32_t y1 = sy + (scale_v * sprite->y1 / PHD_ONE);
	int32_t y2 = sy + (scale_v * sprite->y2 / PHD_ONE);

	if (x2 >= 0 && y2 >= 0 && x1 < Screen_GetResWidth() &&
		y1 < Screen_GetResHeight())
	{
		if(Hardware)
		{
			//30 << W2V_SHIFT значит немного дальше чем
			//ближняя плоскость отсечения значение Z_NEAR 20
			S_Output_DrawSprite(x1, y1, x2, y2, 30 << W2V_SHIFT, sprnum, 0);
		}
		else
		{
			S_Output_DrawSprite(x1, y1, x2, y2, z, sprnum, 0);
		}
		
	}
}

void Output_DrawSprite(int32_t x, int32_t y, int32_t z, int16_t sprnum,
					   int16_t shade)
{

	x -= g_W2VMatrix._03;
	y -= g_W2VMatrix._13;
	z -= g_W2VMatrix._23;

	if (x < -DRAW_DIST_MAX || x > DRAW_DIST_MAX)
	{
		return;
	}

	if (y < -DRAW_DIST_MAX || y > DRAW_DIST_MAX)
	{
		return;
	}

	if (z < -DRAW_DIST_MAX || z > DRAW_DIST_MAX)
	{
		return;
	}

	int32_t zv = g_W2VMatrix._20 * x + g_W2VMatrix._21 * y + g_W2VMatrix._22 * z;

	if (zv < Z_NEAR || zv > (DRAW_DIST_MAX << W2V_SHIFT))
	{
		return;
	}

	int32_t xv = g_W2VMatrix._00 * x + g_W2VMatrix._01 * y + g_W2VMatrix._02 * z;
	int32_t yv = g_W2VMatrix._10 * x + g_W2VMatrix._11 * y + g_W2VMatrix._12 * z;
	int32_t zp = zv / g_PhdPersp;

	PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];

	/*

	int32_t xa1;
	int32_t ya1;
	int32_t xa2;
	int32_t ya2;

	//сдвиг (над полом) экранных координат маленькой аптечки
	//что бы аптечка не тонула в полу (данные взяты из ТР2)
	if (sprnum == 7)
	{
		xa1 = -88;
		ya1 = -96;
		xa2 = 92;
		ya2 = 4;
	}
	//сдвиг (над полом) экранных координат большой аптечки
	//что бы аптечка не тонула в полу (для большой не обязательно)
	//(данные взяты из ТР2)
	else if (sprnum == 8)
	{
		xa1 = -93;
		ya1 = -126;
		xa2 = 97;
		ya2 = 9;
	}
	else
	{
		xa1 = sprite->x1;
		ya1 = sprite->y1;
		xa2 = sprite->x2;
		ya2 = sprite->y2;
	}

	int32_t x1 = ViewPort_GetCenterX() + (xv + (xa1 << W2V_SHIFT)) / zp;
	int32_t y1 = ViewPort_GetCenterY() + (yv + (ya1 << W2V_SHIFT)) / zp;
	int32_t x2 = ViewPort_GetCenterX() + (xv + (xa2 << W2V_SHIFT)) / zp;
	int32_t y2 = ViewPort_GetCenterY() + (yv + (ya2 << W2V_SHIFT)) / zp;
	*/

	
	int32_t x1 = ViewPort_GetCenterX() + (xv + (sprite->x1 << W2V_SHIFT)) / zp;
	int32_t y1 = ViewPort_GetCenterY() + (yv + (sprite->y1 << W2V_SHIFT)) / zp;
	int32_t x2 = ViewPort_GetCenterX() + (xv + (sprite->x2 << W2V_SHIFT)) / zp;
	int32_t y2 = ViewPort_GetCenterY() + (yv + (sprite->y2 << W2V_SHIFT)) / zp;

	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;

	if (x2 >= g_PhdLeft && y2 >= g_PhdTop && x1 <= g_PhdRight &&
		y1 <= g_PhdBottom)
	{
		int32_t depth = zv >> W2V_SHIFT;
		shade += CalcFogShade(depth);
		CLAMPG(shade, 0x1FFF);

		S_Output_DrawSprite(x1, y1, x2, y2, zv, sprnum, shade);
	}
}

void S_Output_DrawSprite_SW(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int z,
	int sprnum, int shade)
{

	int32_t *sort = sort3dptr;
	int16_t *info = info3dptr;

	sort[0] = (int32_t)info;
	sort[1] = z; // depth

	sort3dptr += 2;

	info[0] = 8; // scaled sprite draw func
	info[1] = x1;
	info[2] = y1;
	info[3] = x2;
	info[4] = y2;
	info[5] = sprnum;
	info[6] = shade;

	info += 7;

	info3dptr = info;

	surfacenum++;
}

//рисует спрайт когда в игре пользователь поднимает предмет
//предмет отображается в виде спрайта справа внизу экрана
void Output_DrawUISprite(int32_t x, int32_t y, int32_t scale, int16_t sprnum,
						 int16_t shade)
{
	PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];

	int32_t x1 = x + (scale * sprite->x1 >> 16);
	int32_t x2 = x + (scale * sprite->x2 >> 16);
	int32_t y1 = y + (scale * sprite->y1 >> 16);
	int32_t y2 = y + (scale * sprite->y2 >> 16);

	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;

	if (x2 >= g_PhdLeft && y2 >= g_PhdTop && x1 <= g_PhdRight &&
		y1 <= g_PhdBottom)
	{
		if(Hardware)
		{
			//30 << W2V_SHIFT значит немного дальше чем
			//ближняя плоскость отсечения значение Z_NEAR 20
			S_Output_DrawSprite(x1, y1, x2, y2, 30 << W2V_SHIFT, sprnum, shade);
		}
		else
		{
			S_Output_DrawSprite(x1, y1, x2, y2, 200, sprnum, shade);
		}
	}
}

void S_Output_DrawScreenBox(int32_t sx, int32_t sy, int32_t w, int32_t h)
{
	int rgb_border_light = 15;
	int rgb_border_dark = 31;
	Output_DrawScreenLine(sx - 1, sy - 1, w + 3, 0, rgb_border_light);
	Output_DrawScreenLine(sx, sy, w + 1, 0, rgb_border_dark);
	Output_DrawScreenLine(w + sx + 1, sy, 0, h + 1, rgb_border_light);
	Output_DrawScreenLine(w + sx + 2, sy - 1, 0, h + 3, rgb_border_dark);
	Output_DrawScreenLine(w + sx + 1, h + sy + 1, -w - 1, 0, rgb_border_light);
	Output_DrawScreenLine(w + sx + 2, h + sy + 2, -w - 3, 0, rgb_border_dark);
	Output_DrawScreenLine(sx - 1, h + sy + 2, 0, -3 - h, rgb_border_light);
	Output_DrawScreenLine(sx, h + sy + 1, 0, -1 - h, rgb_border_dark);
}

void S_Output_DrawScreenFBox_SW(int32_t sx, int32_t sy, int32_t w, int32_t h)
{
	int32_t *sort = sort3dptr;
	int16_t *info = info3dptr;

	sort[0] = (int32_t)info;
	sort[1] = 1200; // depth исправить как в ТР1

	sort3dptr += 2;

	info[0] = 7; // draw routine flat transparent shaded poly

	// info[1] = Compose_Colour(30, 30, 30); //color исправить как в ТР1
	info[1] = 24;
	info[2] = 4; // num coords

	info[3] = sx;
	info[4] = sy;

	info[5] = sx + w;
	info[6] = sy;

	info[7] = sx + w;
	info[8] = sy + h;

	info[9] = sx;
	info[10] = sy + h;

	info += 11;

	info3dptr = info;

	surfacenum++;
}

void S_Output_DrawScreenFlatQuad_SW(int32_t sx, int32_t sy, int32_t w, int32_t h, RGB888 color, int depth)
{
	int32_t *sort = sort3dptr;
	int16_t *info = info3dptr;

	sort[0] = (int32_t)info;
	sort[1] = depth; // depth

	sort3dptr += 2;

	info[0] = 5; // draw routine flat shaded poly

	info[1] = Compose_Colour(color.r, color.g, color.b); // color
	info[2] = 4;										 // num coords

	info[3] = sx;
	info[4] = sy;

	info[5] = sx + w;
	info[6] = sy;

	info[7] = sx + w;
	info[8] = sy + h;

	info[9] = sx;
	info[10] = sy + h;

	info += 11;

	info3dptr = info;

	surfacenum++;
}

void S_AnimateTextures(int32_t ticks)
{
	g_WibbleOffset = (g_WibbleOffset + ticks) % WIBBLE_SIZE;

	static int32_t tick_comp = 0;
	tick_comp += ticks;

	while (tick_comp > TICKS_PER_FRAME * 5)
	{
		int16_t *ptr = g_AnimTextureRanges;
		int16_t i = *ptr++;
		while (i > 0)
		{
			int16_t j = *ptr++;
			PHD_TEXTURE temp = g_PhdTextureInfo[*ptr];
			while (j > 0)
			{
				g_PhdTextureInfo[ptr[0]] = g_PhdTextureInfo[ptr[1]];
				j--;
				ptr++;
			}
			g_PhdTextureInfo[*ptr] = temp;
			i--;
			ptr++;
		}
		tick_comp -= TICKS_PER_FRAME * 5;
	}
}

void Output_CalcWibbleTable()
{
	for (int i = 0; i < WIBBLE_SIZE; i++)
	{
		PHD_ANGLE angle = (i * PHD_360) / WIBBLE_SIZE;
		g_WibbleTable[i] = phd_sin(angle) * MAX_WIBBLE >> W2V_SHIFT;
		g_ShadeTable[i] = phd_sin(angle) * MAX_SHADE >> W2V_SHIFT;
		g_RandTable[i] = (Random_GetDraw() >> 5) - 0x01FF;
	}
}

void DrawEffect(int16_t fxnum)
{
	FX_INFO *fx = &g_Effects[fxnum];

	OBJECT_INFO *object = &g_Objects[fx->object_number];

	if (!object->loaded)
	{
		return;
	}

	if (object->nmeshes < 0)
	{
		Output_DrawSprite(fx->pos.x, fx->pos.y, fx->pos.z,
						  object->mesh_index - fx->frame_number, 4096);
	}
	else
	{
		phd_PushMatrix();
		phd_TranslateAbs(fx->pos.x, fx->pos.y, fx->pos.z);
		if (g_PhdMatrixPtr->_23 > Z_NEAR
			&& g_PhdMatrixPtr->_23 < DRAW_DIST_MAX << W2V_SHIFT)
		{
			phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
			if (object->nmeshes)
			{
				Output_CalculateStaticLight(fx->shade);
				Output_DrawPolygons(g_Meshes[object->mesh_index], -1);
			}
			else
			{
				Output_CalculateLight(fx->pos.x, fx->pos.y, fx->pos.z,
									  fx->room_number);
				Output_DrawPolygons(g_Meshes[fx->frame_number], -1);
			}
		}
		phd_PopMatrix();
	}
}

void Output_DrawLightningSegment(int32_t x1, int32_t y1, int32_t z1, int32_t x2,
								 int32_t y2, int32_t z2, int32_t width)
{

	if (z1 >= Z_NEAR && z1 <= DRAW_DISTANCE_MAX && z2 >= Z_NEAR &&
		z2 <= DRAW_DISTANCE_MAX)
	{
		x1 = ViewPort_GetCenterX() + x1 / (z1 / g_PhdPersp);
		y1 = ViewPort_GetCenterY() + y1 / (z1 / g_PhdPersp);
		x2 = ViewPort_GetCenterX() + x2 / (z2 / g_PhdPersp);
		y2 = ViewPort_GetCenterY() + y2 / (z2 / g_PhdPersp);

		int32_t thickness1 = (width << W2V_SHIFT) / (z1 / g_PhdPersp);
		int32_t thickness2 = (width << W2V_SHIFT) / (z2 / g_PhdPersp);

		S_Output_DrawLightningSegment(x1, y1, z1, thickness1, x2, y2, z2,
									  thickness2);
	}
}

void S_Output_DrawLightningSegment(int x1, int y1, int z1, int thickness1,
								   int x2, int y2, int z2, int thickness2)
{
	//рисуем саму синюю молнию от Тора
	VBUF2 vertices[8];

	int depth = z1;
	if (z2 < z1)
		depth = z2;

	vertices[0].x = (float)x1;
	vertices[0].y = (float)y1;
	// vertices[0].z = (float)8589934592.0f / z1;
	vertices[0].z = (float)depth;
	vertices[0].g = 0.0f;

	vertices[1].x = (float)(thickness1 + x1);
	vertices[1].y = (float)y1;
	// vertices[1].z = (float)8589934592.0f / z1;
	vertices[1].z = (float)depth;
	vertices[1].g = 0.0f;

	vertices[2].x = (float)(thickness2 + x2);
	vertices[2].y = (float)y2;
	// vertices[2].z = 8589934592.0f / z2;
	vertices[2].z = (float)depth;
	vertices[2].g = 0.0f;

	vertices[3].x = (float)x2;
	vertices[3].y = (float)y2;
	// vertices[3].z = 8589934592.0f / z2;
	vertices[3].z = (float)depth;
	vertices[3].g = 0.0f;

	int num = ClipVertices2(4, vertices);

	if (num)
	{
		S_Output_DrawTriangle(vertices, num, depth);
	}

	//рисуем белую линию посредине молнии Тора
	vertices[0].x = (float)(thickness1 / 2 + x1);
	vertices[0].y = (float)y1;
	// vertices[0].z = depth;
	vertices[0].g = (float)ColorLighting2; // Compose_Colour(255, 255, 255);

	vertices[1].x = (float)(thickness2 / 2 + x2);
	vertices[1].y = (float)y2;
	// vertices[1].z = depth;
	vertices[1].g = (float)ColorLighting2; // Compose_Colour(255, 255, 255);

	S_Output_DrawLine(vertices, depth);
}

void Output_DrawScreenLine(int32_t sx, int32_t sy, int32_t w, int32_t h,
						   int color)
{

	VBUF2 vertices[8];

	vertices[0].x = (float)sx;
	vertices[0].y = (float)sy;
	// vertices[0].z = depth;
	vertices[0].g = (float)color; // Compose_Colour(255, 255, 255);

	vertices[1].x = (float)sx + w;
	vertices[1].y = (float)sy + h;
	// vertices[1].z = depth;
	vertices[1].g = (float)color; // Compose_Colour(255, 255, 255);

	if (Hardware)
	{
		//35 << W2V_SHIFT значит немного дальше чем
		//ближняя плоскость отсечения значение Z_NEAR 20
		S_Output_DrawLine(vertices, 35 << W2V_SHIFT);
	}
	else
	{
		S_Output_DrawLine(vertices, 0);
	}
}

void S_Output_DrawTriangle_SW(VBUF2* vertices, int vert_count, int depth)
{
	int32_t *sort = sort3dptr;
	int16_t *info = info3dptr;

	sort[0] = (int32_t)info;
	sort[1] = depth;

	sort3dptr += 2;

	info[0] = 6;			  // tex->drawtype;
	info[1] = ColorLighting1; // color tex->tpage;
	info[2] = vert_count;

	info += 3;

	int32_t indx = 0;

	do
	{
		info[0] = (short int)vertices[indx].x;
		info[1] = (short int)vertices[indx].y;
		info[2] = (short int)vertices[indx].g;

		info += 3;
		indx++;

	} while (indx < vert_count);

	info3dptr = info;

	surfacenum++;
}

void S_Output_DrawLine_SW(VBUF2* vertices, int depth)
{
	int32_t *sort = sort3dptr;
	int16_t *info = info3dptr;

	sort[0] = (int32_t)info;
	sort[1] = depth - 2;

	sort3dptr += 2;

	info[0] = 4; // tex->drawtype;
	info[1] = (short int)vertices[0].x;
	info[2] = (short int)vertices[0].y;
	info[3] = (short int)vertices[1].x;
	info[4] = (short int)vertices[1].y;
	// info[5] = ColorLighting2; //color
	info[5] = (short int)vertices[0].g; // color

	info += 6;

	info3dptr = info;

	surfacenum++;
}

void Output_DrawSpriteRel(int32_t x, int32_t y, int32_t z, int16_t sprnum,
						  int16_t shade)
{
	int32_t zv = g_PhdMatrixPtr->_20 * x + g_PhdMatrixPtr->_21 * y +
				 g_PhdMatrixPtr->_22 * z + g_PhdMatrixPtr->_23;
	if (zv < Z_NEAR || zv > (DRAW_DIST_MAX << W2V_SHIFT))
	{
		return;
	}

	int32_t xv = g_PhdMatrixPtr->_00 * x + g_PhdMatrixPtr->_01 * y +
				 g_PhdMatrixPtr->_02 * z + g_PhdMatrixPtr->_03;
	int32_t yv = g_PhdMatrixPtr->_10 * x + g_PhdMatrixPtr->_11 * y +
				 g_PhdMatrixPtr->_12 * z + g_PhdMatrixPtr->_13;
	int32_t zp = zv / g_PhdPersp;

	PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];

	int32_t x1 = ViewPort_GetCenterX() + (xv + (sprite->x1 << W2V_SHIFT)) / zp;
	int32_t y1 = ViewPort_GetCenterY() + (yv + (sprite->y1 << W2V_SHIFT)) / zp;
	int32_t x2 = ViewPort_GetCenterX() + (xv + (sprite->y1 << W2V_SHIFT)) / zp;
	int32_t y2 = ViewPort_GetCenterY() + (yv + (sprite->y2 << W2V_SHIFT)) / zp;

	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;

	if (x2 >= g_PhdLeft && y2 >= g_PhdTop && x1 <= g_PhdRight &&
		y1 <= g_PhdBottom)
	{
		int32_t depth = zv >> W2V_SHIFT;
		shade += CalcFogShade(depth);
		CLAMPG(shade, 0x1FFF);
		S_Output_DrawSprite(x1, y1, x2, y2, zv, sprnum, shade);
	}
}

int16_t* DrawRoomSprites(int16_t* obj_ptr, int32_t number)
{
	for (int i = 0; i < number; i++)
	{
		int16_t vbuf_num = obj_ptr[0];
		int16_t sprnum = obj_ptr[1];
		obj_ptr += 2;


		PHD_VBUF* vbuf = &m_VBuf[vbuf_num];

		if (vbuf->clip < 0)
		{
			continue;
		}

		int32_t zv = (int)vbuf->zv;

		PHD_SPRITE* sprite = &g_PhdSpriteInfo[sprnum];

		int32_t zp = (zv / g_PhdPersp);

		int32_t x1 = (int)(ViewPort_GetCenterX() + (vbuf->xv + (sprite->x1 << W2V_SHIFT)) / zp);
		int32_t y1 = (int)(ViewPort_GetCenterY() + (vbuf->yv + (sprite->y1 << W2V_SHIFT)) / zp);
		int32_t x2 = (int)(ViewPort_GetCenterX() + (vbuf->xv + (sprite->x2 << W2V_SHIFT)) / zp);
		int32_t y2 = (int)(ViewPort_GetCenterY() + (vbuf->yv + (sprite->y2 << W2V_SHIFT)) / zp);

		if (x2 >= g_PhdLeft && y2 >= g_PhdTop && x1 < g_PhdRight
			&& y1 < g_PhdBottom)
		{
			S_Output_DrawSprite(x1, y1, x2, y2, zv, sprnum, vbuf->g);
		}

	}

	return obj_ptr;

}

//-----------------------------------------
//hardware funcs section
//-----------------------------------------

float TransformZ(float ooz);


void TEX_VERT_BUFF_TO_DX_BUFFER(TEXTUREBUCKET_OPAQUE &OUT_BUCKET, VBUF IN_VERTICES, DWORD IN_COLOR)
{                                                               
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sx = IN_VERTICES.x;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sy = IN_VERTICES.y;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sz = TransformZ(IN_VERTICES.w);
    OUT_BUCKET.Vertex[OUT_BUCKET.count].rhw = IN_VERTICES.w;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].color = IN_COLOR;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].tu = IN_VERTICES.u / IN_VERTICES.w;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].tv = IN_VERTICES.v / IN_VERTICES.w;
    OUT_BUCKET.count++;
}


void COLOR_VERT_BUFF_TO_DX_BUFFER(COLOREDBUCKET &OUT_BUCKET, VBUF2 IN_VERTICES, DWORD IN_COLOR)  \
{
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sx = IN_VERTICES.x;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sy = IN_VERTICES.y;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sz = TransformZ(IN_VERTICES.w);
    OUT_BUCKET.Vertex[OUT_BUCKET.count].rhw = 1.0f;
    OUT_BUCKET.Vertex[OUT_BUCKET.count].color = IN_COLOR;
    OUT_BUCKET.count++;
}

#define COLOR_VERT_BUFF_TO_DX_BUFFER2(OUT_BUCKET, IN_VERTICES, DEPTH, IN_COLOR)  \
do {                                                                \
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sx = IN_VERTICES.x;       \
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sy = IN_VERTICES.y;       \
    OUT_BUCKET.Vertex[OUT_BUCKET.count].sz = TransformZ(DEPTH);       \
    OUT_BUCKET.Vertex[OUT_BUCKET.count].rhw = 1.0f;       \
    OUT_BUCKET.Vertex[OUT_BUCKET.count].color = IN_COLOR;         \
    OUT_BUCKET.count++;                                             \
} while(0)

float TransformZ(float ooz)
{
	float f_znear = 20 << W2V_SHIFT;
	float f_zfar = 20480 << W2V_SHIFT;

	float one = (256.0f * 8.0f * 16384.0f);

	float zv = one / ooz;
	float res = (zv - f_znear) / (f_zfar - f_znear);
	return res;

	/*
	// минимальные и максимальные значения Z для нормализации
	const float MINZR = 0.005f;
	const float MAXZR = 0.995f;
	const float ZRANGE = MAXZR - MINZR;

	// расчёт стандартных коэффициентов движка
	float f_b = (ZRANGE * f_znear * f_zfar) / (f_znear - f_zfar);
	float f_a = MINZR - (f_b / f_znear);
	f_b = -f_b;

	// масштабирование с учётом инверсии ooz
	float f_boo = f_b / one;
	float p_sz = f_a - f_boo * ooz;

	return p_sz;
	*/


}



void Draw_Textured_Triangle(PHD_VBUF * v1, PHD_VBUF *v2, PHD_VBUF *v3, PHD_UV* tex1, PHD_UV* tex2, PHD_UV* tex3, int draw_type, int tpage)
{
	PHD_VBUF* vns[4];
	VBUF vertices[8];
	POINT_INFO points[4];
	PHD_TEXTURE* tex;
	int32_t vert_count = 0;

	int i = 0;

	vns[0] = v1;
	vns[1] = v2;
	vns[2] = v3;

	tex = new PHD_TEXTURE[1];

	tex->uv[0].u1 = tex1->u1;
	tex->uv[0].v1 = tex1->v1;
	tex->uv[1].u1 = tex2->u1;
	tex->uv[1].v1 = tex2->v1;
	tex->uv[2].u1 = tex3->u1;
	tex->uv[2].v1 = tex3->v1;

	float multiplier = 0.0625f * BRIGHTNESS;
	float one = (256.0f * 8.0f * 16384.0f); // как в TR2: 33554432.0f
				

	if (!(vns[0]->clip & vns[1]->clip & vns[2]->clip))
	{
		if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0)
		{
			//псевдоскалярное косое умножение векторов
			//выясняем куда смотрит полигон - от зрителя или к зрителю
			if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs) -
				(vns[2]->ys - vns[1]->ys) *
				(vns[0]->xs - vns[1]->xs) >
				0)
			{
				//полигон смотрит к зрителю
				//float multiplier = 0.0625f * BRIGHTNESS;
				//float one = (256.0f * 8.0f * 16384.0f); // как в TR2: 33554432.0f
				
				//for (int i = 0; i < 3; i++)
				for (i = 0; i < 3; i++)
				{

					vertices[i].x = (float)vns[i]->xs;
					vertices[i].y = (float)vns[i]->ys;
					//vertices[i].z = ((float)vns[i]->zv - Z_NEAR) / ((0x9000 << W2V_SHIFT) - Z_NEAR);
					vertices[i].z = (float)vns[i]->zv;
					//vertices[i].w = 1.0f / vns[i]->zv;
					vertices[i].w = vns[i]->ooz;

					//vertices[i].w = ooz;

					vertices[i].u = ((tex->uv[i].u1 & 0xFF00) + 127) / 65536.0f * vertices[i].w;
					vertices[i].v = ((tex->uv[i].v1 & 0xFF00) + 127) / 65536.0f * vertices[i].w;
					vertices[i].g = ((8192.0f - vns[i]->g) * multiplier);

				}

				vert_count = 3;

				//если необходимо отсечение по границе экрана
				if (vns[0]->clip || vns[1]->clip || vns[2]->clip)
				{

				ClipVerts: //если необходимо отсечение по границе экрана

					//if (!DX9Clipping)
					{

						vert_count = ClipVertices(vert_count, &vertices[0]);
					}
				}

				if (vert_count)
				{

					//int draw_type = tex->drawtype + 2;

					//не прозрачный перспект текст цвет
					if (draw_type == 2)
					{
						if (vert_count > 0)
						{


							//int buket = FindBucket(tex->tpage);
							int buket = FindBucket(tpage);

							for (int i = 1; i < vert_count - 1; i++)
							{
								DWORD color1;
								DWORD color2;
								DWORD color3;

								BYTE diffuse;

								if (g_IsShadeEffect)
								{
									BYTE r;
									BYTE g;
									BYTE b;

									r = (BYTE)CLAMP255(vertices[0].g);
									g = (BYTE)CLAMP255(vertices[0].g);
									b = (BYTE)CLAMP255(vertices[0].g);

									r = r * 2 / 3;
									g = g * 2 / 3;
									b = b;

									color1 = (0xFF << 24) | (r << 16) | (g << 8) | b;

									r = (BYTE)CLAMP255(vertices[i].g);
									g = (BYTE)CLAMP255(vertices[i].g);
									b = (BYTE)CLAMP255(vertices[i].g);

									r = r * 2 / 3;
									g = g * 2 / 3;
									b = b;

									color2 = (0xFF << 24) | (r << 16) | (g << 8) | b;

									r = (BYTE)CLAMP255(vertices[i + 1].g);
									g = (BYTE)CLAMP255(vertices[i + 1].g);
									b = (BYTE)CLAMP255(vertices[i + 1].g);

									r = r * 2 / 3;
									g = g * 2 / 3;
									b = b;


									color3 = (0xFF << 24) | (r << 16) | (g << 8) | b;

								}
								else
								{
									diffuse = (BYTE)CLAMP255(vertices[0].g);
									color1 = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;

									diffuse = (BYTE)CLAMP255(vertices[i].g);
									color2 = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;

									diffuse = (BYTE)CLAMP255(vertices[i + 1].g);
									color3 = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
								}
								
								TEX_VERT_BUFF_TO_DX_BUFFER(Bucket_Tex_Color_Opaque[buket], vertices[0], color1);
								TEX_VERT_BUFF_TO_DX_BUFFER(Bucket_Tex_Color_Opaque[buket], vertices[i], color2);
								TEX_VERT_BUFF_TO_DX_BUFFER(Bucket_Tex_Color_Opaque[buket], vertices[i + 1], color3);

							}
						} // if(vert_count > 0)
					}
					else if (draw_type == 3)
					{
						int depth = (int)vns[0]->zv;

						if (depth < vns[1]->zv)
						{
							depth = (int)vns[1]->zv;
						}

						if (depth < vns[2]->zv)
						{
							depth = (int)vns[2]->zv;
						}

						int32_t* sort = sort3dptr;
						int16_t* info = info3dptr;

						sort[0] = (int32_t)info;
						sort[1] = depth;

						sort3dptr += 2;

						//info[0] = tex->drawtype + 2;
						info[0] = draw_type;
						//info[1] = tex->tpage;
						info[1] = tpage;
						info[2] = vert_count;

						info += 3;

						int32_t indx = 0;

						if (vert_count > 0)
						{

							do
							{
								*(float*)& info[0] = vertices[indx].x;
								*(float*)& info[2] = vertices[indx].y;
								*(float*)& info[4] = vertices[indx].g;

								//*(float*)& info[6] = vertices[indx].z;
								*(float*)& info[6] = TransformZ(vertices[indx].w);
								*(float*)& info[8] = vertices[indx].w;
								*(float*)& info[10] = vertices[indx].u / vertices[indx].w;
								*(float*)& info[12] = vertices[indx].v / vertices[indx].w;

								info += 14;
								indx++;

							} while (indx < vert_count);

							info3dptr = info;

						} // if(vert_count > 0)

						surfacenum++;

					}

				} // if(vert_count)

				// goto LABEL_32;

			} // if kosoe > 0
		}
		else // if && clip
		{
			//отбрасывание задних поверхностей
			//если поверхность обращена от зрителя - пропускаем ее
			if (phd_VisibleZClip(vns[0], vns[1], vns[2]))
			{
				//for ( int i = 0; i < 3; i++ )
				for ( int i = 0; i < 3; i++ )
				{
					points[i].xs = (float)vns[i]->xs;
					points[i].ys = (float)vns[i]->ys;
					points[i].xv = (float)vns[i]->xv;
					points[i].yv = (float)vns[i]->yv;
					points[i].zv = (float)vns[i]->zv;
					//points[i].w = (float)vns[i]->ooz;
					points[i].u = ((tex->uv[i].u1 & 0xFF00) + 127) / 65536.0f;
					points[i].v = ((tex->uv[i].v1 & 0xFF00) + 127) / 65536.0f;
					points[i].g = vns[i]->g;
				}

				vert_count = ZedClipper(3, &points[0], &vertices[0]);

				if (vert_count)
				{
					goto ClipVerts;
				}
			} // if ( phd_VisibleZClip
		} // else if any clip
	}
	// LABEL_32:
	delete[] tex;
}

int16_t* S_DrawObjectGT4_HW(int16_t* obj_ptr, int32_t number)
{

	PHD_VBUF* vns[4];
	//VBUF vertices[8];
	//POINT_INFO points[4];
	PHD_TEXTURE* tex;
	int32_t vert_count = 0;

	int num_TexturedQuad = number;

	if (number > 0)
	{
		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];
			vns[3] = &m_VBuf[*obj_ptr++];

			tex = &g_PhdTextureInfo[*obj_ptr++];

			Draw_Textured_Triangle(vns[0], vns[1], vns[2], &tex->uv[0], &tex->uv[1], &tex->uv[2], tex->drawtype + 2, tex->tpage);
			Draw_Textured_Triangle(vns[0], vns[2], vns[3], &tex->uv[0], &tex->uv[2], &tex->uv[3], tex->drawtype + 2, tex->tpage);

			num_TexturedQuad--;
			// if not all clip

		} while (num_TexturedQuad > 0);

	} // if (number > )

	return obj_ptr;
}

int16_t* S_DrawObjectGT3_HW(int16_t* obj_ptr, int32_t number)
{
	PHD_VBUF* vns[3];
	//VBUF vertices[8];
	//POINT_INFO points[3];
	PHD_TEXTURE* tex;
	int32_t vert_count = 0;

	int num_TexturedTri = number;

	if (number > 0)
	{
		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];

			tex = &g_PhdTextureInfo[*obj_ptr++];

			Draw_Textured_Triangle(vns[0], vns[1], vns[2], &tex->uv[0], &tex->uv[1], &tex->uv[2], tex->drawtype + 2, tex->tpage);
			num_TexturedTri--;
			// if not all clip

		} while (num_TexturedTri > 0);

	} // if (number > )

	return obj_ptr;
}

void Draw_Colored_Triangle(PHD_VBUF* v1, PHD_VBUF* v2, PHD_VBUF* v3, int color)
{
	PHD_VBUF* vns[4];
	VBUF2 vertices[8];
	//int color;
	int32_t vert_count = 3;

	vns[0] = v1;
	vns[1] = v2;
	vns[2] = v3;

	if (!(vns[0]->clip & vns[1]->clip & vns[2]->clip))
	{
		if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0)
		{
			//псевдоскалярное косое умножение векторов
			//выясняем куда смотрит полигон - от зрителя или к зрителю
			if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs) -
				(vns[2]->ys - vns[1]->ys) *
				(vns[0]->xs - vns[1]->xs) >
				0)
			{
				//полигон смотрит к зрителю

				for ( int i = 0; i < 3; i++ )
				{
					vertices[i].x = (float)vns[i]->xs;
					vertices[i].y = (float)vns[i]->ys;
					vertices[i].z = (float)vns[i]->zv;
					vertices[i].g = (float)vns[i]->g;

					vertices[i].w = (float)vns[i]->ooz;
				}
				vert_count = 3;

				//if (!DX9Clipping)
				{
					//если необходимо отсечение по границе экрана
					if (vns[0]->clip || vns[1]->clip || vns[2]->clip)
					{

						//		ClipVerts:		//если необходимо отсечение
						//по границе экрана
						vert_count =
							ClipVertices2(vert_count, &vertices[0]);
					}

				}

				//		Skip_ClipVerts:

				if (vert_count)
				{
					BYTE r;
					BYTE g;
					BYTE b;

					if (g_IsShadeEffect)
					{
						r = GamePalette[color].r * 2 / 3;
						g = GamePalette[color].g * 2 / 3;
						b = GamePalette[color].b;
					}
					else
					{
						r = GamePalette[color].r;
						g = GamePalette[color].g;
						b = GamePalette[color].b;
					}

					float divisor = (1.0f / BRIGHTNESS) * 1024.0f;

					for (int i = 1; i < vert_count - 1; i++)
					{

						float light = (8192.0f - vertices[0].g) / divisor;

						BYTE r1 = CLAMP255(r * light);
						BYTE g1 = CLAMP255(g * light);
						BYTE b1 = CLAMP255(b * light);


						DWORD color = (0xFF << 24) | (r1 << 16) | (g1 << 8) | b1;

						COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[0], color);



						light = (8192.0f - vertices[i].g) / divisor;

						r1 = CLAMP255(r * light);
						g1 = CLAMP255(g * light);
						b1 = CLAMP255(b * light);

						color = (0xFF << 24) | (r1 << 16) | (g1 << 8) | b1;

						COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[i], color);



						light = (8192.0f - vertices[i + 1].g) / divisor;

						r1 = CLAMP255(r * light);
						g1 = CLAMP255(g * light);
						b1 = CLAMP255(b * light);

						color = (0xFF << 24) | (r1 << 16) | (g1 << 8) | b1;

						COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[i + 1], color);
					}

				} // if(vert_count)

				goto LABEL_32;

			} // if kosoe > 0
		}
	}
LABEL_32:
	;
}

int16_t* S_DrawObjectG4_HW(int16_t* obj_ptr, int32_t number)
{

	PHD_VBUF* vns[4];
	//VBUF2 vertices[8];
	int color;
	int32_t vert_count = 4;

	int num_ColoredQuad = number;

	if (number > 0)
	{

		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];
			vns[3] = &m_VBuf[*obj_ptr++];

			color = *obj_ptr++;

			Draw_Colored_Triangle(vns[0], vns[1], vns[2], color);
			Draw_Colored_Triangle(vns[0], vns[2], vns[3], color);


			
			num_ColoredQuad--;
			// if not all clip

		} while (num_ColoredQuad > 0);

	} // if (number > )

	return obj_ptr;
}

int16_t* S_DrawObjectG3_HW(int16_t* obj_ptr, int32_t number)
{

	PHD_VBUF* vns[4];
	//VBUF2 vertices[8];
	int color;
	int32_t vert_count = 3;

	int num_ColoredTri = number;

	if (number > 0)
	{

		do
		{
			vns[0] = &m_VBuf[*obj_ptr++];
			vns[1] = &m_VBuf[*obj_ptr++];
			vns[2] = &m_VBuf[*obj_ptr++];

			color = *obj_ptr++;

			Draw_Colored_Triangle(vns[0], vns[1], vns[2], color);
			
			num_ColoredTri--;
			// if not all clip

		} while (num_ColoredTri > 0);

	} // if (number > )

	return obj_ptr;
}

#define SHADOW_BIAS		131072

void S_Output_DrawShadow_HW(PHD_VBUF* vbufs, int clip, int vertex_count)
{
	VBUF2 vertices[32] = { 0 };

	float   one = (256.0f * 8.0f * 16384.0f);

	for (int i = 0; i < vertex_count; i++)
	{
		VBUF2* vertex = &vertices[i];
		PHD_VBUF* vbuf = &vbufs[i];
		vertex->x = (float)vbuf->xs;
		vertex->y = (float)vbuf->ys;
		vertex->z = (float)vbuf->zv;
		vertex->w = one / float(vertex->z - SHADOW_BIAS);
 		//vertex->g = 24.0f;
	}

	if (clip)
	{
		vertex_count = ClipVertices2(vertex_count, &vertices[0]);

		if (!vertex_count)
		{
			return;
		}
	}

	


	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 0;
	BYTE a = 128;

	DWORD color = (a << 24) | (r << 16) | (g << 8) | b;

	//for (int i = 1; i < vertex_count - 1; i++)
	for (i = 1; i < vertex_count - 1; i++)
	{
		//vert 1

		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sx = vertices[0].x;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sy = vertices[0].y;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sz = TransformZ(vertices[0].w);
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].rhw = vertices[0].w;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].color = color;
		Bucket_TransQuad.count++;

		//vert 2

		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sx = vertices[i].x;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sy = vertices[i].y;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sz = TransformZ(vertices[i].w);
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].rhw = vertices[i].w;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].color = color;
		Bucket_TransQuad.count++;

		//vert 3

		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sx = vertices[i + 1].x;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sy = vertices[i + 1].y;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sz = TransformZ(vertices[i + 1].w);
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].rhw = vertices[i + 1].w;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].color = color;
		Bucket_TransQuad.count++;

	}


}

void S_Output_DrawSprite_HW(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int z, int sprnum, int shade)
{

	float t1;
	float t2;
	float t3;
	float t4;
	float t5;
	float vz;
	float vshade;
	PHD_SPRITE* sprite;
	VBUF vertices[10];
	
	float multiplier = 0.0625f * BRIGHTNESS;

	sprite = &g_PhdSpriteInfo[sprnum];
	vshade = (8192.0f - shade) * multiplier;
	if (vshade > 255.0f)
	{
		vshade = 255.0f;
	}
	else if (vshade < 0.0f)
	{
		vshade = 0.0f;
	}



	

	t1 = ((int)sprite->offset & 0xFF) + 0.5f;
	t2 = ((int)sprite->offset >> 8) + 0.5f;
	t3 = ((int)sprite->width >> 8) + t1;
	t4 = ((int)sprite->height >> 8) + t2;
	vz = (float)z;

	float one = (256.0f * 8.0f * 16384.0f);
	//t5 = (float) (1.0f / z);
	t5 = (float)(one / z);

	vertices[0].x = x1;
	vertices[0].y = y1;
	//vertices[0].z = vz;
	vertices[0].z = TransformZ(t5);
	//vertices[0].u = t1 * t5 * 0.00390625f;
	//vertices[0].v = t2 * t5 * 0.00390625f;
	vertices[0].u = t1 * 0.00390625f;
	vertices[0].v = t2 * 0.00390625f;
	vertices[0].w = t5;
	vertices[0].g = vshade;

	vertices[1].x = x2;
	vertices[1].y = y1;
	vertices[1].z = TransformZ(t5);
	//vertices[1].z = vz;
	//vertices[1].u = t3 * t5 * 0.00390625f;
	//vertices[1].v = t2 * t5 * 0.00390625f;
	vertices[1].u = t3 * 0.00390625f;
	vertices[1].v = t2 * 0.00390625f;
	vertices[1].w = t5;
	vertices[1].g = vshade;
	
	vertices[2].x = x2;
	vertices[2].y = y2;
	vertices[2].z = TransformZ(t5);
	//vertices[2].z = vz;
	//vertices[2].u = t3 * t5 * 0.00390625f;
	//vertices[2].v = t4 * t5 * 0.00390625f;
	vertices[2].u = t3 * 0.00390625f;
	vertices[2].v = t4 * 0.00390625f;
	vertices[2].w = t5;
	vertices[2].g = vshade;

	vertices[3].x = x1;
	vertices[3].y = y2;
	vertices[3].z = TransformZ(t5);
	//vertices[3].z = vz;
	//vertices[3].u = t1 * t5 * 0.00390625f;
	//vertices[3].v = t4 * t5 * 0.00390625f;
	vertices[3].u = t1 * 0.00390625f;
	vertices[3].v = t4 * 0.00390625f;
	vertices[3].w = t5;
	vertices[3].g = vshade;

	int vert_count = 4;

	int32_t* sort = sort3dptr;
	int16_t* info = info3dptr;

	sort[0] = (int32_t)info;
	sort[1] = z;

	sort3dptr += 2;

	info[0] = 8; //draw type sprite

	info[1] = sprite->tpage;
	info[2] = vert_count;

	info += 3;

	int32_t indx = 0;

	if (vert_count > 0)
	{

		do
		{
			*(float*)& info[0] = vertices[indx].x;
			*(float*)& info[2] = vertices[indx].y;
			*(float*)& info[4] = vertices[indx].g;

			*(float*)& info[6] = vertices[indx].z;
			*(float*)& info[8] = vertices[indx].w;
			*(float*)& info[10] = vertices[indx].u;
			*(float*)& info[12] = vertices[indx].v;

			info += 14;
			indx++;

		} while (indx < vert_count);

		info3dptr = info;

	} // if(vert_count > 0)

	surfacenum++;
	
	/*
	int buket = FindBucket(sprite->tpage);

	float ZNear = Z_NEAR;
	float ZFar = 0x9000 << W2V_SHIFT;

	float Q = ZFar / (ZFar - ZNear);

	for (int i = 1; i < vert_count - 1; i++)
	{
		DWORD color1;
		DWORD color2;
		DWORD color3;

		DWORD diffuse;
		
		diffuse = (BYTE)CLAMP255(vertices[0].g);
		color1 = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;

		diffuse = (BYTE)CLAMP255(vertices[i].g);
		color2 = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;

		diffuse = (BYTE)CLAMP255(vertices[i + 1].g);
		color3 = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;

		TEX_VERT_BUFF_TO_DX_BUFFER(Bucket_Tex_Color_Opaque[buket], vertices[0], color1);
		TEX_VERT_BUFF_TO_DX_BUFFER(Bucket_Tex_Color_Opaque[buket], vertices[i], color2);
		TEX_VERT_BUFF_TO_DX_BUFFER(Bucket_Tex_Color_Opaque[buket], vertices[i + 1], color3);
	}
	*/
}

void S_Output_DrawScreenFBox_HW(int32_t sx, int32_t sy, int32_t w, int32_t h)
{

	VBUF vertices[4];

	vertices[0].x = (float)sx;
	vertices[0].y = (float)sy;

	vertices[1].x = (float) (sx + w);
	vertices[1].y = (float)sy;

	vertices[2].x = (float)(sx + w);
	vertices[2].y = (float)(sy + h);

	vertices[3].x = (float)sx;
	vertices[3].y = (float)(sy + h);
	
	//40 << W2V_SHIFT значит немного дальше чем
	//ближняя плоскость отсечения значение Z_NEAR 20
	int depth = Z_NEAR + (20 << W2V_SHIFT);

	float   one = (256.0f * 8.0f * 16384.0f);

	float fooz = one / float(depth);

	int vert_count = 4;

	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 0;
	BYTE a = 128;

	DWORD color = (a << 24) | (r << 16) | (g << 8) | b;

	for ( int i = 1; i < vert_count - 1; i++ )
	{
		//vert 1

		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sx = vertices[0].x;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sy = vertices[0].y;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sz = TransformZ(fooz);
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].rhw = fooz;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].color = color;
		Bucket_TransQuad.count++;

		//vert 2

		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sx = vertices[i].x;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sy = vertices[i].y;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sz = TransformZ(fooz);
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].rhw = fooz;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].color = color;
		Bucket_TransQuad.count++;

		//vert 3

		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sx = vertices[i + 1].x;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sy = vertices[i + 1].y;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].sz = TransformZ(fooz);
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].rhw = fooz;
		Bucket_TransQuad.Vertex[Bucket_TransQuad.count].color = color;
		Bucket_TransQuad.count++;
	
	}


}

void S_Output_DrawScreenFlatQuad_HW(int32_t sx, int32_t sy, int32_t w, int32_t h, RGB888 color, int depth)
{
	VBUF2 vertices[4];

	depth = depth << W2V_SHIFT;

	float one = (256.0f * 8.0f * 16384.0f);
	float d = one / depth;

	vertices[0].x = (float)sx;
	vertices[0].y = (float)sy;
	vertices[0].z = (float)depth;
	vertices[0].w = d;
	//vertices[0].z = TransformZ(d);

	vertices[1].x = (float)(sx + w);
	vertices[1].y = (float)sy;
	vertices[1].z = (float)depth;
	vertices[1].w = d;
	//vertices[1].z = TransformZ(d);

	vertices[2].x = (float)(sx + w);
	vertices[2].y = (float)(sy + h);
	vertices[2].z = (float)depth;
	vertices[2].w = d;
	//vertices[2].z = TransformZ(d);

	vertices[3].x = (float)sx;
	vertices[3].y = (float)(sy + h);
	vertices[3].z = (float)depth;
	vertices[3].w = d;
	//vertices[3].z = TransformZ(d);

	int vert_count = 4;

	//depth = depth << W2V_SHIFT;

	BYTE r = color.r;
	BYTE g = color.g;
	BYTE b = color.b;

	DWORD color2 = (0xFF << 24) | (r << 16) | (g << 8) | b;

	for (int i = 1; i < vert_count - 1; i++)
	{
		COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[0], color2);
		COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[i], color2);
		COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[i + 1], color2);
		//vert 1
		/*
		Bucket_Colored.Vertex[Bucket_Colored.count].sx = vertices[0].x;
		Bucket_Colored.Vertex[Bucket_Colored.count].sy = vertices[0].y;
		Bucket_Colored.Vertex[Bucket_Colored.count].sz = (float)depth;
		Bucket_Colored.Vertex[Bucket_Colored.count].rhw = 1.0f;
		Bucket_Colored.Vertex[Bucket_Colored.count].color = color2;

		Bucket_Colored.count++;

		//vert 2

		Bucket_Colored.Vertex[Bucket_Colored.count].x = vertices[i].x;
		Bucket_Colored.Vertex[Bucket_Colored.count].y = vertices[i].y;
		Bucket_Colored.Vertex[Bucket_Colored.count].z = (float) depth;
		Bucket_Colored.Vertex[Bucket_Colored.count].w = 1.0f;
		Bucket_Colored.Vertex[Bucket_Colored.count].diffuse = color2;

		Bucket_Colored.count++;

		//vert 3

		Bucket_Colored.Vertex[Bucket_Colored.count].x = vertices[i + 1].x;
		Bucket_Colored.Vertex[Bucket_Colored.count].y = vertices[i + 1].y;
		Bucket_Colored.Vertex[Bucket_Colored.count].z = (float) depth;
		Bucket_Colored.Vertex[Bucket_Colored.count].w = 1.0f;
		Bucket_Colored.Vertex[Bucket_Colored.count].diffuse = color2;

		Bucket_Colored.count++;

		*/
	}
}

void S_Output_DrawTriangle_HW(VBUF2* vertices, int vert_count, int depth)
{

	//голубой цвет молнии
	BYTE r = 0;
	BYTE g = 0;
	BYTE b = 255;

	float one = (256.0f * 8.0f * 16384.0f);

	DWORD color2 = (0xFF << 24) | (r << 16) | (g << 8) | b;

	for (int i = 1; i < vert_count - 1; i++)
	{

		//vert 1
		COLOR_VERT_BUFF_TO_DX_BUFFER2(Bucket_Colored, vertices[0], (float)one / depth, color2);

		//vert 2
		COLOR_VERT_BUFF_TO_DX_BUFFER2(Bucket_Colored, vertices[i], (float)one / depth, color2);

		//vert 3
		COLOR_VERT_BUFF_TO_DX_BUFFER2(Bucket_Colored, vertices[i + 1], (float)one / depth, color2);

		/*
		COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[0], color2);
		COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[i], color2);
		COLOR_VERT_BUFF_TO_DX_BUFFER(Bucket_Colored, vertices[i + 1], color2);
		*/

		/*
		//vert 1
		Bucket_Colored.Vertex[Bucket_Colored.count].sx = vertices[0].x;
		Bucket_Colored.Vertex[Bucket_Colored.count].sy = vertices[0].y;
		Bucket_Colored.Vertex[Bucket_Colored.count].sz = (float) depth;
		Bucket_Colored.Vertex[Bucket_Colored.count].rhw = 1.0f;
		Bucket_Colored.Vertex[Bucket_Colored.count].color = color2;

		Bucket_Colored.count++;

		//vert 2

		Bucket_Colored.Vertex[Bucket_Colored.count].sx = vertices[i].x;
		Bucket_Colored.Vertex[Bucket_Colored.count].sy = vertices[i].y;
		Bucket_Colored.Vertex[Bucket_Colored.count].sz = (float)depth;
		Bucket_Colored.Vertex[Bucket_Colored.count].rhw = 1.0f;
		Bucket_Colored.Vertex[Bucket_Colored.count].color = color2;

		Bucket_Colored.count++;

		//vert 3

		Bucket_Colored.Vertex[Bucket_Colored.count].sx = vertices[i + 1].x;
		Bucket_Colored.Vertex[Bucket_Colored.count].sy = vertices[i + 1].y;
		Bucket_Colored.Vertex[Bucket_Colored.count].sz = (float) depth;
		Bucket_Colored.Vertex[Bucket_Colored.count].rhw = 1.0f;
		Bucket_Colored.Vertex[Bucket_Colored.count].color = color2;

		Bucket_Colored.count++;
		*/
	}
}

void S_Output_DrawLine_HW(VBUF2* vertices, int depth)
{
	float one = (256.0f * 8.0f * 16384.0f);

	Bucket_Lines.Vertex[Bucket_Lines.count].x = vertices[0].x;
	Bucket_Lines.Vertex[Bucket_Lines.count].y = vertices[0].y;
	//Bucket_Lines.Vertex[Bucket_Lines.count].z = (float) depth;
	//Bucket_Lines.Vertex[Bucket_Lines.count].z = ((float)depth - Z_NEAR) / (0x9000 << W2V_SHIFT - Z_NEAR);
	Bucket_Lines.Vertex[Bucket_Lines.count].z = TransformZ(one / depth);
	Bucket_Lines.Vertex[Bucket_Lines.count].w = 1.0f / depth;

	BYTE diffuse = (BYTE)CLAMP255(vertices[0].g);
	BYTE r = GameNormalPalette[diffuse].r;
	BYTE g = GameNormalPalette[diffuse].g;
	BYTE b = GameNormalPalette[diffuse].b;
	DWORD color = (0xFF << 24) | (r << 16) | (g << 8) | b;

	Bucket_Lines.Vertex[Bucket_Lines.count].diffuse = color;

	Bucket_Lines.count++;

	Bucket_Lines.Vertex[Bucket_Lines.count].x = vertices[1].x;
	Bucket_Lines.Vertex[Bucket_Lines.count].y = vertices[1].y;
	//Bucket_Lines.Vertex[Bucket_Lines.count].z = ((float)depth - Z_NEAR) / (0x9000 << W2V_SHIFT - Z_NEAR);
	Bucket_Lines.Vertex[Bucket_Lines.count].z = TransformZ(one / depth);
	Bucket_Lines.Vertex[Bucket_Lines.count].w = 1.0f / depth;

	diffuse = (BYTE)CLAMP255(vertices[1].g);
	r = GameNormalPalette[diffuse].r;
	g = GameNormalPalette[diffuse].g;
	b = GameNormalPalette[diffuse].b;
	color = (0xFF << 24) | (r << 16) | (g << 8) | b;


	Bucket_Lines.Vertex[Bucket_Lines.count].diffuse = color;

	Bucket_Lines.count++;


}

int FindBucket(DWORD tpage)
{
	// Find Bucket_Tex_Color_Opaque Already In Use
	for (int n = 0; n < MAXBUCKETS; n++)
	{
		if (Bucket_Tex_Color_Opaque[n].tpage == tpage && Bucket_Tex_Color_Opaque[n].count < (VERTSPERBUCKET - 32))
			return n;
	}

	// No Bucket_Tex_Color_Opaque In Use/Or Full Find Next Available
	//for (int n = 0; n < MAXBUCKETS; n++)
	for (n = 0; n < MAXBUCKETS; n++)
	{
		if (Bucket_Tex_Color_Opaque[n].tpage == -1)
			//if(Bucket_Tex_Color_Opaque[n].tpage == (LPDIRECT3DTEXTURE9) 0xFFFFFFFF)		
		{
			Bucket_Tex_Color_Opaque[n].tpage = tpage;

			//полигоны без текстуры
			if (tpage == 0xFFFFFFFF)
			{
				Bucket_Tex_Color_Opaque[n].lp_tpage = NULL;
			}
			//полигоны с текстурой
			else
			{
				Bucket_Tex_Color_Opaque[n].lp_tpage = m_pLevelTile[tpage];
			}
			return n;
		}

	}

	return -1;
}

void InitBuckets()
{
	for (int n = 0; n < MAXBUCKETS; n++)
	{
		Bucket_Tex_Color_Opaque[n].tpage = -1;
		//Bucket_Tex_Color_Opaque[n].tpage = 0xFFFFFFFF;
		Bucket_Tex_Color_Opaque[n].lp_tpage = NULL;
		Bucket_Tex_Color_Opaque[n].count = 0;
	}

	Bucket_Colored.count = 0;
	//Bucket_Colored.VertBuff = NULL;

	Bucket_Lines.count = 0;
	//Bucket_Lines.VertBuff = NULL;

	Bucket_TransQuad.count = 0;
	//Bucket_TransQuad.VertBuff = NULL;
}






	



