#include <windows.h>

#include "draw.h"
#include "const.h"
#include "matrix.h"
#include "phd_math.h"
#include "3d_gen.h"
#include "winmain.h"
#include "game.h"
#include "vars.h"
#include "box.h"
#include "items.h"
#include "lot.h"
#include "lara.h"
#include "control_util.h"
#include "util.h"
#include "collide.h"
#include "sphere.h"
#include "sound.h"
#include "setup.h"
#include "screen.h"
#include "..\\OBJECTS\\switch.h"
#include "..\\OBJECTS\\keyhole.h"
#include "..\\OBJECTS\\puzzle_hole.h"
#include "..\\traps\\movable_block.h"
#include "..\\traps\\lava.h"

void DrawAnimatingItem(ITEM_INFO *item)
{
    static int16_t null_rotation[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    int16_t *frmptr[2];
    int32_t rate;
    int32_t frac = GetFrames(item, frmptr, &rate);
    OBJECT_INFO *object = &g_Objects[item->object_number];

    if (object->shadow_size) {
        Output_DrawShadow(object->shadow_size, frmptr[0], item);
    }

    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    int32_t clip = S_GetObjectBounds(frmptr[0]);
    
	if (!clip) {
        phd_PopMatrix();
        return;
    }
	
    CalculateObjectLighting(item, frmptr[0]);
    int16_t *extra_rotation = (int16_t *) (item->data ? item->data : &null_rotation);

    int32_t bit = 1;
    int16_t **meshpp = &g_Meshes[object->mesh_index];
    int32_t *bone = &g_AnimBones[object->bone_index];

    if (!frac)
	{
        phd_TranslateRel(
            frmptr[0][FRAME_POS_X], frmptr[0][FRAME_POS_Y],
            frmptr[0][FRAME_POS_Z]);

        int32_t *packed_rotation = (int32_t *)(frmptr[0] + FRAME_ROT);
        phd_RotYXZpack(*packed_rotation++);

        if (item->mesh_bits & bit)
		{
            Output_DrawPolygons(*meshpp++, clip);
        }

        for (int i = 1; i < object->nmeshes; i++) {
            int32_t bone_extra_flags = *bone;
            if (bone_extra_flags & BEB_POP) {
                phd_PopMatrix();
            }

            if (bone_extra_flags & BEB_PUSH) {
                phd_PushMatrix();
            }

            phd_TranslateRel(bone[1], bone[2], bone[3]);
            phd_RotYXZpack(*packed_rotation++);

            if (bone_extra_flags & BEB_ROT_Y) {
                phd_RotY(*extra_rotation++);
            }
            if (bone_extra_flags & BEB_ROT_X) {
                phd_RotX(*extra_rotation++);
            }
            if (bone_extra_flags & BEB_ROT_Z) {
                phd_RotZ(*extra_rotation++);
            }

            bit <<= 1;
            if (item->mesh_bits & bit) {
                Output_DrawPolygons(*meshpp, clip);
            }

            bone += 4;
            meshpp++;
        }
    } else {
        InitInterpolate(frac, rate);
        phd_TranslateRel_ID(
            frmptr[0][FRAME_POS_X], frmptr[0][FRAME_POS_Y],
            frmptr[0][FRAME_POS_Z], frmptr[1][FRAME_POS_X],
            frmptr[1][FRAME_POS_Y], frmptr[1][FRAME_POS_Z]);
        int32_t *packed_rotation1 = (int32_t *)(frmptr[0] + FRAME_ROT);
        int32_t *packed_rotation2 = (int32_t *)(frmptr[1] + FRAME_ROT);
        phd_RotYXZpack_I(*packed_rotation1++, *packed_rotation2++);

        if (item->mesh_bits & bit) {
            Output_DrawPolygons_I(*meshpp++, clip);
        }

        for (int i = 1; i < object->nmeshes; i++) {
            int32_t bone_extra_flags = *bone;
            if (bone_extra_flags & BEB_POP) {
                phd_PopMatrix_I();
            }

            if (bone_extra_flags & BEB_PUSH) {
                phd_PushMatrix_I();
            }

            phd_TranslateRel_I(bone[1], bone[2], bone[3]);
            phd_RotYXZpack_I(*packed_rotation1++, *packed_rotation2++);

            if (bone_extra_flags & BEB_ROT_Y) {
                phd_RotY_I(*extra_rotation++);
            }
            if (bone_extra_flags & BEB_ROT_X) {
                phd_RotX_I(*extra_rotation++);
            }
            if (bone_extra_flags & BEB_ROT_Z) {
                phd_RotZ_I(*extra_rotation++);
            }

            bit <<= 1;
            if (item->mesh_bits & bit) {
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
    if (!interp) {
        return 0;
    }

    int32_t second = anim->interpolation * (first + 1);
    if (second > anim->frame_end) {
        *rate = anim->frame_end + anim->interpolation - second;
    }

    return interp;
}

void Output_DrawShadow(int16_t size, int16_t *bptr, ITEM_INFO *item)
{
    int i;

    //g_ShadowInfo.vertex_count = g_Config.enable_round_shadow ? 32 : 8;
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

        bool visible = ((int32_t)(((vn3->xs - vn2->xs) * (vn1->ys - vn2->ys)) - ((vn1->xs - vn2->xs) * (vn3->ys - vn2->ys))) >= 0);

        if (!clip_and && clip_positive && visible)
        {
            S_Output_DrawShadow(&m_VBuf[0], clip_or ? 1 : 0, g_ShadowInfo.vertex_count);
        }
    }

    phd_PopMatrix();
}

void S_Output_DrawShadow(PHD_VBUF *vbufs, int clip, int vertex_count)
{

    VBUF2 vertices[32];

    for (int i = 0; i < vertex_count; i++)
    {
        VBUF2* vertex = &vertices[i];
        PHD_VBUF* vbuf = &vbufs[i];
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
        int depth = (vbufs[0].zv + vbufs[1].zv + vbufs[2].zv + vbufs[3].zv
            + vbufs[4].zv + vbufs[5].zv + vbufs[6].zv + vbufs[7].zv) / 8;

        int32_t* sort = sort3dptr;
        int16_t* info = info3dptr;

        sort[0] = (int32_t)info;
        sort[1] = depth;

        sort3dptr += 2;

        info[0] = 7;//draw type
        info[1] = 24; //color / tex page
        info[2] = vert_count;

        info += 3;

        int32_t indx = 0;

        if (vert_count > 0)
        {
            do
            {
                info[0] = (short int)vertices[indx].x;
                info[1] = (short int)vertices[indx].y;
                //info[2] = (short int)vertices[indx].g;

                //info += 3;
                info += 2;
                indx++;

            } while (indx < vert_count);

            info3dptr = info;

            surfacenum++;

        } // if(vert_count > 0)

        
    }

    


    // needs to be more than 8 cause clipping might return more polygons.
    //GFX_3D_Vertex vertices[vertex_count * CLIP_VERTCOUNT_SCALE];
	/*
	GFX_3D_Vertex* vertices = (GFX_3D_Vertex*)malloc(vertex_count * CLIP_VERTCOUNT_SCALE * sizeof(GFX_3D_Vertex));
    int i;
    int32_t tmp;

    for (i = 0; i < vertex_count; i++) {
        GFX_3D_Vertex *vertex = &vertices[i];
        PHD_VBUF *vbuf = &vbufs[i];
        vertex->x = vbuf->xs;
        vertex->y = vbuf->ys;
        vertex->z = vbuf->zv * 0.0001f - 16.0f;
        vertex->b = 0.0f;
        vertex->g = 0.0f;
        vertex->r = 0.0f;
        vertex->a = 128.0f;
    }

    if (clip) {
        int original = vertex_count;
        vertex_count = S_Output_ClipVertices(vertex_count, vertices);
        assert(vertex_count < original * CLIP_VERTCOUNT_SCALE);
    }

    if (!vertex_count) {
        return;
    }

    S_Output_DisableTextureMode();

    GFX_3D_Renderer_SetBlendingEnabled(m_Renderer3D, true);
    S_Output_DrawTriangleStrip(vertices, vertex_count);
    GFX_3D_Renderer_SetBlendingEnabled(m_Renderer3D, false);

	free(vertices);
	*/
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

    for (int i = 0; i < 8; i++) {
        int32_t zv = g_PhdMatrixPtr->_20 * vtx[i].x
            + g_PhdMatrixPtr->_21 * vtx[i].y + g_PhdMatrixPtr->_22 * vtx[i].z
            + g_PhdMatrixPtr->_23;

        if (zv > DRAW_DISTANCE_MIN && zv < DRAW_DISTANCE_MAX) {
            ++num_z;
            int32_t zp = zv / g_PhdPersp;
            int32_t xv =
                (g_PhdMatrixPtr->_00 * vtx[i].x + g_PhdMatrixPtr->_01 * vtx[i].y
                 + g_PhdMatrixPtr->_02 * vtx[i].z + g_PhdMatrixPtr->_03)
                / zp;
            int32_t yv =
                (g_PhdMatrixPtr->_10 * vtx[i].x + g_PhdMatrixPtr->_11 * vtx[i].y
                 + g_PhdMatrixPtr->_12 * vtx[i].z + g_PhdMatrixPtr->_13)
                / zp;

            if (x_min > xv) {
                x_min = xv;
            } else if (x_max < xv) {
                x_max = xv;
            }

            if (y_min > yv) {
                y_min = yv;
            } else if (y_max < yv) {
                y_max = yv;
            }
        }
    }

    x_min += ViewPort_GetCenterX();
    x_max += ViewPort_GetCenterX();
    y_min += ViewPort_GetCenterY();
    y_max += ViewPort_GetCenterY();

    if (!num_z || x_min > g_PhdRight || y_min > g_PhdBottom || x_max < g_PhdLeft
        || y_max < g_PhdTop) {
        return 0; // out of screen
    }
	
	if (num_z < 8 || x_min < 0 || y_min < 0 || x_max > g_PhdRight
        || y_max > g_PhdBottom)
	{
		return -1; // clipped
    }
    

    return 1; // fully on screen
}

void CalculateObjectLighting(ITEM_INFO *item, int16_t *frame)
{
    if (item->shade >= 0) {
        Output_CalculateStaticLight(item->shade);
        return;
    }

    phd_PushUnitMatrix();
    g_PhdMatrixPtr->_23 = 0;
    g_PhdMatrixPtr->_13 = 0;
    g_PhdMatrixPtr->_03 = 0;

    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    phd_TranslateRel(
        (frame[FRAME_BOUND_MIN_X] + frame[FRAME_BOUND_MAX_X]) / 2,
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
    if (obj_ptr) {
        obj_ptr = Output_CalcVerticeLight(obj_ptr);
        //obj_ptr = Output_DrawObjectGT4(obj_ptr + 1, *obj_ptr);
        //obj_ptr = Output_DrawObjectGT3(obj_ptr + 1, *obj_ptr);
		obj_ptr = DrawObjectGT4(obj_ptr + 1, *obj_ptr);
        obj_ptr = DrawObjectGT3(obj_ptr + 1, *obj_ptr);
        //obj_ptr = Output_DrawObjectG4(obj_ptr + 1, *obj_ptr);
		//obj_ptr = Output_DrawObjectG3(obj_ptr + 1, *obj_ptr);
		obj_ptr = DrawObjectG4(obj_ptr + 1, *obj_ptr);
        obj_ptr = DrawObjectG3(obj_ptr + 1, *obj_ptr);
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
    for (int i = 0; i < vertex_count; i++) {
        int32_t xv = g_PhdMatrixPtr->_00 * obj_ptr[0]
            + g_PhdMatrixPtr->_01 * obj_ptr[1]
            + g_PhdMatrixPtr->_02 * obj_ptr[2] + g_PhdMatrixPtr->_03;
        int32_t yv = g_PhdMatrixPtr->_10 * obj_ptr[0]
            + g_PhdMatrixPtr->_11 * obj_ptr[1]
            + g_PhdMatrixPtr->_12 * obj_ptr[2] + g_PhdMatrixPtr->_13;
        int32_t zv = g_PhdMatrixPtr->_20 * obj_ptr[0]
            + g_PhdMatrixPtr->_21 * obj_ptr[1]
            + g_PhdMatrixPtr->_22 * obj_ptr[2] + g_PhdMatrixPtr->_23;
        m_VBuf[i].xv = xv;
        m_VBuf[i].yv = yv;
        m_VBuf[i].zv = zv;

		//m_VBuf[i].g = 0;

        int32_t clip_flags;
        if (zv < DRAW_DISTANCE_MIN) {
            clip_flags = -32768;
        } else {
            clip_flags = 0;

            int32_t xs = ViewPort_GetCenterX() + xv / (zv / g_PhdPersp);
            int32_t ys = ViewPort_GetCenterY() + yv / (zv / g_PhdPersp);

            if (xs < g_PhdLeft) {
                if (xs < -32760) {
                    xs = -32760;
                }
                clip_flags |= 1;
            } else if (xs > g_PhdRight) {
                if (xs > 32760) {
                    xs = 32760;
                }
                clip_flags |= 2;
            }

            if (ys < g_PhdTop) {
                if (ys < -32760) {
                    ys = -32760;
                }
                clip_flags |= 4;
            } else if (ys > g_PhdBottom) {
                if (ys > 32760) {
                    ys = 32760;
                }
                clip_flags |= 8;
            }

            m_VBuf[i].xs = xs;
            m_VBuf[i].ys = ys;
        }

        m_VBuf[i].clip = clip_flags;
        total_clip &= clip_flags;
        obj_ptr += 3;
    }

    return total_clip == 0 ? obj_ptr : NULL;
}

int16_t *Output_CalcVerticeLight(int16_t *obj_ptr)
{
	/*
	int32_t vertex_count = *obj_ptr++;

	obj_ptr += 3 * vertex_count;

	for (int i = 0; i < vertex_count; i++)
	{
            
			//m_VBuf[i].g = 0;
		//m_VBuf[i].g = 0x1FFF;
		m_VBuf[i].g = 0x1FFF - 2700;
    }
	*/
	
    int32_t vertex_count = *obj_ptr++;
    if (vertex_count > 0)
	{
        if (g_LsDivider)
		{
            int32_t xv = (g_PhdMatrixPtr->_00 * g_LsVectorView.x
                          + g_PhdMatrixPtr->_10 * g_LsVectorView.y
                          + g_PhdMatrixPtr->_20 * g_LsVectorView.z)
                / g_LsDivider;
            int32_t yv = (g_PhdMatrixPtr->_01 * g_LsVectorView.x
                          + g_PhdMatrixPtr->_11 * g_LsVectorView.y
                          + g_PhdMatrixPtr->_21 * g_LsVectorView.z)
                / g_LsDivider;
            int32_t zv = (g_PhdMatrixPtr->_02 * g_LsVectorView.x
                          + g_PhdMatrixPtr->_12 * g_LsVectorView.y
                          + g_PhdMatrixPtr->_22 * g_LsVectorView.z)
                / g_LsDivider;
            for (int i = 0; i < vertex_count; i++) {
                int16_t shade = g_LsAdder
                    + ((obj_ptr[0] * xv + obj_ptr[1] * yv + obj_ptr[2] * zv)
                       >> 16);
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
        for (int i = 0; i < -vertex_count; i++) {
            int16_t shade = g_LsAdder + *obj_ptr++;
            CLAMP(shade, 0, 0x1FFF);
            m_VBuf[i].g = shade;
        }
    }
	
    return obj_ptr;

}

int16_t *DrawObjectGT4(int16_t *obj_ptr, int32_t number)
{
    PHD_VBUF *vns[4];
	VBUF vertices[8];
	POINT_INFO points[4];
    PHD_TEXTURE *tex;
	int32_t vert_count = 4;
	//int32_t vert_count = 0;

	int num_TexturedQuad = number;

	//перед вызовом DrawObjectGT4
	//obj_ptr + 0 количество GT4 = index 1 - 2 байта
	//obj_ptr + 1 vert #1 = index 2 - 2 байта
	//obj_ptr + 2 vert #2 = index 3 - 2 байта
	//obj_ptr + 3 vert #3 = index 4 - 2 байта
	//obj_ptr + 4 vert #4 = index 5 - 2 байта
	//obj_ptr + 5 = g_PhdTextureInfo = index 6 - 2 байта
	
	//внутри DrawObjectGT4 eax = obj_ptr = пропускается количество GT4
	//obj_ptr + 0 vert #1 in 2 bytes + 0 bytes
	//obj_ptr + 1 vert #2 in 2 bytes + 2 bytes 
	//obj_ptr + 2 vert #3 in 2 bytes + 4 bytes
	//obj_ptr + 3 vert #4 in 2 bytes + 6 bytes
	//obj_ptr + 4 g_PhdTextureInfo in 2 bytes

	//вычисление текстуры
	//start memory obj_ptr = byte4(1122) byte4(3344) texinfo byte4(5500)
	//вычисление текстуры eax = obj_ptr
	//mov eax, [eax + 6] => start memory obj = byte4(11 22) byte4(33 => eax = 00 00 55 44
	//sar eax >> 16 => eax = 55

	//sizeof(PHD_TEXTURE) = 20 bytes
	//sizeof(PHD_VBUF) = 32 bytes
	//sizeof(obj) = 12 bytes
	
	//------------------
	//vert buff VERT m_VBuf -> sizeof(PHD_VBUF) = 32
	// x	+0
	// y	+4
	// z	+8
	// xs	+12
	// yx	+16
	// dist	+20
	// clip	+22
	// g	+24
	// tu	+26
	// tv	+28

	if(number > 0)
	{

	do
	{
        vns[0] = &m_VBuf[*obj_ptr++];
        vns[1] = &m_VBuf[*obj_ptr++];
        vns[2] = &m_VBuf[*obj_ptr++];
        vns[3] = &m_VBuf[*obj_ptr++];

		tex = &g_PhdTextureInfo[*obj_ptr++];
        
		/*
		//результат if ноль инвертируем
		//все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//нету вобще отсечения
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		*/

		/*
		результат if 1 инвертируем

		//все вершины с одной стороны экрана - полигон не видим
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/


		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		//вариант 2 - нету вобще отсечения (в пределах экрана)
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)

		/*
		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//вариант 2 - нету вобще отсечения (в пределах экрана)
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/
		
		//если вариант 1 и 2
		if ( !(vns[0]->clip & vns[1]->clip & vns[2]->clip & vns[3]->clip) )
		{
			//если вариант 1 и 2
			if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0 && vns[3]->clip >= 0)
			{
				//псевдоскалярное косое умножение векторов
				//выясняем куда смотрит полигон - от зрителя или к зрителю
				if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs)
                - (vns[2]->ys - vns[1]->ys) * (vns[0]->xs - vns[1]->xs)	> 0)
				{
					//полигон смотрит к зрителю
					vertices[0].x = (float) vns[0]->xs;
					vertices[0].y = (float) vns[0]->ys;
					vertices[0].z = 8589934592.0f / vns[0]->zv;
					vertices[0].u = vertices[0].z * tex->uv[0].u1;
					vertices[0].v = vertices[0].z * tex->uv[0].v1;
					vertices[0].g = vns[0]->g;

					vertices[1].x = (float) vns[1]->xs;
					vertices[1].y = (float) vns[1]->ys;
					vertices[1].z = 8589934592.0f / vns[1]->zv;
					vertices[1].u = vertices[1].z * tex->uv[1].u1;
					vertices[1].v = vertices[1].z * tex->uv[1].v1;
					vertices[1].g = vns[1]->g;

					vertices[2].x = (float) vns[2]->xs;
					vertices[2].y = (float) vns[2]->ys;
					vertices[2].z = 8589934592.0f / vns[2]->zv;
					vertices[2].u = vertices[2].z * tex->uv[2].u1;
					vertices[2].v = vertices[2].z * tex->uv[2].v1;
					vertices[2].g = vns[2]->g;

					vertices[3].x = (float) vns[3]->xs;
					vertices[3].y = (float) vns[3]->ys;
					vertices[3].z = 8589934592.0f / vns[3]->zv;
					vertices[3].u = vertices[3].z * tex->uv[3].u1;
					vertices[3].v = vertices[3].z * tex->uv[3].v1;
					vertices[3].g = vns[3]->g;

					vert_count = 4;

					//если необходимо отсечение по границе экрана
					if (vns[0]->clip || vns[1]->clip || vns[2]->clip || vns[3]->clip) 
					{
						

		ClipVerts:		//если необходимо отсечение по границе экрана
						vert_count = ClipVertices(vert_count, &vertices[0]); 
					}

		//Skip_ClipVerts:

					if(vert_count)
					{
						int depth = vns[0]->zv;
						
						if(depth < vns[1]->zv)
						{
							depth = vns[1]->zv;
						}

						if(depth < vns[2]->zv)
						{
							depth = vns[2]->zv;
						}

						if(depth < vns[3]->zv)
						{
							depth = vns[3]->zv;
						}

						int32_t * sort = sort3dptr;
						int16_t * info = info3dptr;
		
						sort[0] = (int32_t)info;
						sort[1] = depth;

						sort3dptr += 2;

						//info3dptr 2 bytes - 16 бит
						//0 draw routine
						//1 texture page
						//2 num coords ie vert count
						//3 vert1
						//4 vert2
						//5 vert3

						//info3dptr[0] = tex->drawtype;
						//info3dptr[1] = tex->tpage;
						//info3dptr[2] = vert_count;

						/*
						if(depth > perspective_distance)
						{
							
							info[0] = tex->drawtype;
							//info[1] = tex->tpage;
							info[2] = vert_count;

							if(vert_count > 0)
							{
								int32_t indx = 0;

								info += 3; //пропускаем drawType, texNum, vertNumbrs

								do {

									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									
									//vertices[0].z ; //edx + 8
									
									info[2] = (short int) (vertices[indx].u / vertices[indx].z); //edx + 12 = 0Ch
									info[4] = (short int) (vertices[indx].v / vertices[indx].z); //edx + 10 = 0Ch

									info += 5;
									indx++;

								} while ( indx < vert_count );

								info3dptr = info;
								
							}

							
						}
						else // if(depth > perspective_distance)
							*/
						{
							info[0] = tex->drawtype + 2;
							info[1] = tex->tpage;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if(vert_count > 0)
							{

								do
								{
									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									info[2] = (short int) vertices[indx].g;

									*(float*)&info[3] = vertices[indx].z;
									*(float*)&info[5] = vertices[indx].u;
									*(float*)&info[7] = vertices[indx].v;

									info += 9;
									indx++;

								}while ( indx < vert_count );

								info3dptr = info;
								
							} // if(vert_count > 0)
							
							surfacenum++;

						} // if(depth > perspective_distance)

						//surfacenum++;

					} // if(vert_count)

					//goto LABEL_32;

				} //if kosoe > 0
			}
			else // if && clip
			{
				//отбрасывание задних поверхностей
				//если поверхность обращена от зрителя - пропускаем ее
				if ( phd_VisibleZClip(vns[0], vns[1], vns[2]) )
				{

					points[0].xs = (float) vns[0]->xs;
					points[0].ys = (float) vns[0]->ys;
					points[0].xv = (float) vns[0]->xv;
					points[0].yv = (float) vns[0]->yv;
					points[0].zv = (float) vns[0]->zv;
					points[0].u = tex->uv[0].u1;
					points[0].v = tex->uv[0].v1;
					points[0].g = vns[0]->g;

					points[1].xs = (float) vns[1]->xs;
					points[1].ys = (float) vns[1]->ys;
					points[1].xv = (float) vns[1]->xv;
					points[1].yv = (float) vns[1]->yv;
					points[1].zv = (float) vns[1]->zv;
					points[1].u = tex->uv[1].u1;
					points[1].v = tex->uv[1].v1;
					points[1].g = vns[1]->g;

					points[2].xs = (float) vns[2]->xs;
					points[2].ys = (float) vns[2]->ys;
					points[2].xv = (float) vns[2]->xv;
					points[2].yv = (float) vns[2]->yv;
					points[2].zv = (float) vns[2]->zv;
					points[2].u = tex->uv[2].u1;
					points[2].v = tex->uv[2].v1;
					points[2].g = vns[2]->g;

					points[3].xs = (float) vns[3]->xs;
					points[3].ys = (float) vns[3]->ys;
					points[3].xv = (float) vns[3]->xv;
					points[3].yv = (float) vns[3]->yv;
					points[3].zv = (float) vns[3]->zv;
					points[3].u = tex->uv[3].u1;
					points[3].v = tex->uv[3].v1;
					points[3].g = vns[3]->g;

					//vert_count = 4;

					vert_count = ZedClipper(4, &points[0], &vertices[0]);

					if ( vert_count )
					{
						//v27 = &vertices[0];
						goto ClipVerts;
				    }

					//goto Skip_ClipVerts;

				} // if ( phd_VisibleZClip
				
			}	//else if any clip

		}
//LABEL_32:

			num_TexturedQuad--;
		 // if not all clip

    } while (num_TexturedQuad > 0);

	} //if (number > )

    return obj_ptr;
}


int16_t *DrawObjectGT3(int16_t *obj_ptr, int32_t number)
{
    PHD_VBUF *vns[3];
	VBUF vertices[8];
	POINT_INFO points[3];
    PHD_TEXTURE *tex;
	//int32_t vert_count = 3;
	int32_t vert_count = 0;

	int num_TexturedTri = number;

	//перед вызовом DrawObjectGT4
	//obj_ptr + 0 количество GT4 = index 1 - 2 байта
	//obj_ptr + 1 vert #1 = index 2 - 2 байта
	//obj_ptr + 2 vert #2 = index 3 - 2 байта
	//obj_ptr + 3 vert #3 = index 4 - 2 байта
	//obj_ptr + 4 vert #4 = index 5 - 2 байта
	//obj_ptr + 5 = g_PhdTextureInfo = index 6 - 2 байта
	
	//внутри DrawObjectGT4 eax = obj_ptr = пропускается количество GT4
	//obj_ptr + 0 vert #1 in 2 bytes + 0 bytes
	//obj_ptr + 1 vert #2 in 2 bytes + 2 bytes 
	//obj_ptr + 2 vert #3 in 2 bytes + 4 bytes
	//obj_ptr + 3 vert #4 in 2 bytes + 6 bytes
	//obj_ptr + 4 g_PhdTextureInfo in 2 bytes

	//вычисление текстуры
	//start memory obj_ptr = byte4(1122) byte4(3344) texinfo byte4(5500)
	//вычисление текстуры eax = obj_ptr
	//mov eax, [eax + 6] => start memory obj = byte4(11 22) byte4(33 => eax = 00 00 55 44
	//sar eax >> 16 => eax = 55

	//sizeof(PHD_TEXTURE) = 20 bytes
	//sizeof(PHD_VBUF) = 32 bytes
	//sizeof(obj) = 12 bytes
	
	//------------------
	//vert buff VERT m_VBuf -> sizeof(PHD_VBUF) = 32
	// x	+0
	// y	+4
	// z	+8
	// xs	+12
	// yx	+16
	// dist	+20
	// clip	+22
	// g	+24
	// tu	+26
	// tv	+28

	if(number > 0)
	{

	do
	{
        vns[0] = &m_VBuf[*obj_ptr++];
        vns[1] = &m_VBuf[*obj_ptr++];
        vns[2] = &m_VBuf[*obj_ptr++];
        //vns[3] = &m_VBuf[*obj_ptr++];

		tex = &g_PhdTextureInfo[*obj_ptr++];
        
		/*
		//результат if ноль инвертируем
		//все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//нету вобще отсечения
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		*/

		/*
		результат if 1 инвертируем

		//все вершины с одной стороны экрана - полигон не видим
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/


		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		//вариант 2 - нету вобще отсечения (в пределах экрана)
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)

		/*
		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//вариант 2 - нету вобще отсечения (в пределах экрана)
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/
		
		//если вариант 1 и 2
		//if ( !(vns[0]->clip & vns[1]->clip & vns[2]->clip & vns[3]->clip) )
		if ( !(vns[0]->clip & vns[1]->clip & vns[2]->clip) )
		{
			//если вариант 1 и 2
			//if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0 && vns[3]->clip >= 0)
			if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0)
			{
				//псевдоскалярное косое умножение векторов
				//выясняем куда смотрит полигон - от зрителя или к зрителю
				if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs)
                - (vns[2]->ys - vns[1]->ys) * (vns[0]->xs - vns[1]->xs)	> 0)
				{
					//полигон смотрит к зрителю

					vertices[0].x = (float) vns[0]->xs;
					vertices[0].y = (float) vns[0]->ys;
					vertices[0].z = 8589934592.0f / vns[0]->zv;
					vertices[0].u = vertices[0].z * tex->uv[0].u1;
					vertices[0].v = vertices[0].z * tex->uv[0].v1;
					vertices[0].g = vns[0]->g;

					vertices[1].x = (float) vns[1]->xs;
					vertices[1].y = (float) vns[1]->ys;
					vertices[1].z = 8589934592.0f / vns[1]->zv;
					vertices[1].u = vertices[1].z * tex->uv[1].u1;
					vertices[1].v = vertices[1].z * tex->uv[1].v1;
					vertices[1].g = vns[1]->g;

					vertices[2].x = (float) vns[2]->xs;
					vertices[2].y = (float) vns[2]->ys;
					vertices[2].z = 8589934592.0f / vns[2]->zv;
					vertices[2].u = vertices[2].z * tex->uv[2].u1;
					vertices[2].v = vertices[2].z * tex->uv[2].v1;
					vertices[2].g = vns[2]->g;

					/*
					vertices[3].x = (float) vns[3]->xs;
					vertices[3].y = (float) vns[3]->ys;
					vertices[3].z = 8589934592.0f / vns[3]->zv;
					vertices[3].u = vertices[3].z * tex->uv[3].u1;
					vertices[3].v = vertices[3].z * tex->uv[3].v1;
					vertices[3].g = vns[3]->g;
					*/

					vert_count = 3;
					

					//если необходимо отсечение по границе экрана
					//if (vns[0]->clip || vns[1]->clip || vns[2]->clip || vns[3]->clip) 
					if (vns[0]->clip || vns[1]->clip || vns[2]->clip) 
					{

		ClipVerts:		//если необходимо отсечение по границе экрана
						vert_count = ClipVertices(vert_count, &vertices[0]); 
					}

		//Skip_ClipVerts:

					if(vert_count)
					{
						int depth = vns[0]->zv;
						
						if(depth < vns[1]->zv)
						{
							depth = vns[1]->zv;
						}

						if(depth < vns[2]->zv)
						{
							depth = vns[2]->zv;
						}

						/*
						if(depth < vns[3]->zv)
						{
							depth = vns[3]->zv;
						}
						*/

						int32_t * sort = sort3dptr;
						int16_t * info = info3dptr;
		
						sort[0] = (int32_t)info;
						sort[1] = depth;

						sort3dptr += 2;

						//info3dptr 2 bytes - 16 бит
						//0 draw routine
						//1 texture page
						//2 num coords ie vert count
						//3 vert1
						//4 vert2
						//5 vert3

						//info3dptr[0] = tex->drawtype;
						//info3dptr[1] = tex->tpage;
						//info3dptr[2] = vert_count;

/*
						if(depth > perspective_distance)
						{
							
							info[0] = tex->drawtype;
							//info[1] = tex->tpage;
							info[2] = vert_count;

							if(vert_count > 0)
							{
								int32_t indx = 0;

								info += 3; //пропускаем drawType, texNum, vertNumbrs

								do {

									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									
									//vertices[0].z ; //edx + 8
									
									info[2] = (short int) (vertices[indx].u / vertices[indx].z); //edx + 12 = 0Ch
									info[4] = (short int) (vertices[indx].v / vertices[indx].z); //edx + 10 = 0Ch

									info += 5;
									indx++;

								} while ( indx < vert_count );

								info3dptr = info;
							}

							
						}
						else // if(depth > perspective_distance)
						*/
						{
							info[0] = tex->drawtype + 2;
							info[1] = tex->tpage;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if(vert_count > 0)
							{

								do
								{
									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									info[2] = (short int) vertices[indx].g;

									*(float*)&info[3] = vertices[indx].z;
									*(float*)&info[5] = vertices[indx].u;
									*(float*)&info[7] = vertices[indx].v;

									info += 9;
									indx++;

								}while ( indx < vert_count );
								
								info3dptr = info;

							} // if(vert_count > 0)
							
							surfacenum++;

						} // if(depth > perspective_distance)

						//surfacenum++;

					} // if(vert_count)

					//goto LABEL_32;

				} //if kosoe > 0
			}
			else // if && clip
			{
				//отбрасывание задних поверхностей
				//если поверхность обращена от зрителя - пропускаем ее
				if ( phd_VisibleZClip(vns[0], vns[1], vns[2]) )
				{

					points[0].xs = (float) vns[0]->xs;
					points[0].ys = (float) vns[0]->ys;
					points[0].xv = (float) vns[0]->xv;
					points[0].yv = (float) vns[0]->yv;
					points[0].zv = (float) vns[0]->zv;
					points[0].u = tex->uv[0].u1;
					points[0].v = tex->uv[0].v1;
					points[0].g = vns[0]->g;




					points[1].xs = (float) vns[1]->xs;
					points[1].ys = (float) vns[1]->ys;
					points[1].xv = (float) vns[1]->xv;
					points[1].yv = (float) vns[1]->yv;
					points[1].zv = (float) vns[1]->zv;
					points[1].u = tex->uv[1].u1;
					points[1].v = tex->uv[1].v1;
					points[1].g = vns[1]->g;




					points[2].xs = (float) vns[2]->xs;
					points[2].ys = (float) vns[2]->ys;
					
					points[2].xv = (float) vns[2]->xv;
					points[2].yv = (float) vns[2]->yv;
					points[2].zv = (float) vns[2]->zv;
					points[2].u = tex->uv[2].u1;
					points[2].v = tex->uv[2].v1;
					points[2].g = vns[2]->g;

					vert_count = ZedClipper(3, &points[0], &vertices[0]);

					if ( vert_count )
					{
						//v27 = &vertices[0];
						goto ClipVerts;
				    }

					//goto Skip_ClipVerts;
					

				} // if ( phd_VisibleZClip
				
			}	//else if any clip

		}
//LABEL_32:

			num_TexturedTri--;
		 // if not all clip

    } while (num_TexturedTri > 0);

	} //if (number > )

    return obj_ptr;
}


int16_t *DrawObjectG4(int16_t *obj_ptr, int32_t number)
{
    PHD_VBUF *vns[4];
	VBUF2 vertices[8];
	//POINT_INFO points[4];
    //PHD_TEXTURE *tex;
	int color;
	//int32_t vert_count = 4;
	int32_t vert_count = 0;

	int num_TexturedQuad = number;

	//перед вызовом DrawObjectGT4
	//obj_ptr + 0 количество GT4 = index 1 - 2 байта
	//obj_ptr + 1 vert #1 = index 2 - 2 байта
	//obj_ptr + 2 vert #2 = index 3 - 2 байта
	//obj_ptr + 3 vert #3 = index 4 - 2 байта
	//obj_ptr + 4 vert #4 = index 5 - 2 байта
	//obj_ptr + 5 = g_PhdTextureInfo = index 6 - 2 байта
	
	//внутри DrawObjectGT4 eax = obj_ptr = пропускается количество GT4
	//obj_ptr + 0 vert #1 in 2 bytes + 0 bytes
	//obj_ptr + 1 vert #2 in 2 bytes + 2 bytes 
	//obj_ptr + 2 vert #3 in 2 bytes + 4 bytes
	//obj_ptr + 3 vert #4 in 2 bytes + 6 bytes
	//obj_ptr + 4 g_PhdTextureInfo in 2 bytes

	//вычисление текстуры
	//start memory obj_ptr = byte4(1122) byte4(3344) texinfo byte4(5500)
	//вычисление текстуры eax = obj_ptr
	//mov eax, [eax + 6] => start memory obj = byte4(11 22) byte4(33 => eax = 00 00 55 44
	//sar eax >> 16 => eax = 55

	//sizeof(PHD_TEXTURE) = 20 bytes
	//sizeof(PHD_VBUF) = 32 bytes
	//sizeof(obj) = 12 bytes
	
	//------------------
	//vert buff VERT m_VBuf -> sizeof(PHD_VBUF) = 32
	// x	+0
	// y	+4
	// z	+8
	// xs	+12
	// yx	+16
	// dist	+20
	// clip	+22
	// g	+24
	// tu	+26
	// tv	+28

	if(number > 0)
	{

	do
	{
        vns[0] = &m_VBuf[*obj_ptr++];
        vns[1] = &m_VBuf[*obj_ptr++];
        vns[2] = &m_VBuf[*obj_ptr++];
        vns[3] = &m_VBuf[*obj_ptr++];

		//tex = &g_PhdTextureInfo[*obj_ptr++];
		color = *obj_ptr++;
        
		/*
		//результат if ноль инвертируем
		//все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//нету вобще отсечения
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		*/

		/*
		результат if 1 инвертируем

		//все вершины с одной стороны экрана - полигон не видим
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/


		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		//вариант 2 - нету вобще отсечения (в пределах экрана)
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)

		/*
		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//вариант 2 - нету вобще отсечения (в пределах экрана)
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/
		
		//если вариант 1 и 2
		if ( !(vns[0]->clip & vns[1]->clip & vns[2]->clip & vns[3]->clip) )
		{
			//если вариант 1 и 2
			if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0 && vns[3]->clip >= 0)
			{
				//псевдоскалярное косое умножение векторов
				//выясняем куда смотрит полигон - от зрителя или к зрителю
				if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs)
                - (vns[2]->ys - vns[1]->ys) * (vns[0]->xs - vns[1]->xs)	> 0)
				{
					//полигон смотрит к зрителю
					vertices[0].x = (float) vns[0]->xs;
					vertices[0].y = (float) vns[0]->ys;
					vertices[0].g = (float) vns[0]->g;

					vertices[1].x = (float) vns[1]->xs;
					vertices[1].y = (float) vns[1]->ys;
					vertices[1].g = (float) vns[1]->g;

					vertices[2].x = (float) vns[2]->xs;
					vertices[2].y = (float) vns[2]->ys;
					vertices[2].g = (float) vns[2]->g;

					vertices[3].x = (float) vns[3]->xs;
					vertices[3].y = (float) vns[3]->ys;
					vertices[3].g = (float) vns[3]->g;

					vert_count = 4;

					//если необходимо отсечение по границе экрана
					if (vns[0]->clip || vns[1]->clip || vns[2]->clip || vns[3]->clip) 
					{
						

//		ClipVerts:		//если необходимо отсечение по границе экрана
						vert_count = ClipVertices2(vert_count, &vertices[0]); 
					}

//		Skip_ClipVerts:

					if(vert_count)
					{
						int depth = (vns[0]->zv + vns[1]->zv + vns[2]->zv + vns[3]->zv ) /4;
						
						int32_t * sort = sort3dptr;
						int16_t * info = info3dptr;
		
						sort[0] = (int32_t)info;
						sort[1] = depth;

						sort3dptr += 2;

						//info3dptr 2 bytes - 16 бит
						//0 draw routine
						//1 texture page
						//2 num coords ie vert count
						//3 vert1
						//4 vert2
						//5 vert3

						//info3dptr[0] = tex->drawtype;
						//info3dptr[1] = tex->tpage;
						//info3dptr[2] = vert_count;

						/*
						if(depth > perspective_distance)
						{
							
							info[0] = tex->drawtype;
							//info[1] = tex->tpage;
							info[2] = vert_count;

							if(vert_count > 0)
							{
								int32_t indx = 0;

								info += 3; //пропускаем drawType, texNum, vertNumbrs

								do {

									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									
									//vertices[0].z ; //edx + 8
									
									info[2] = (short int) (vertices[indx].u / vertices[indx].z); //edx + 12 = 0Ch
									info[4] = (short int) (vertices[indx].v / vertices[indx].z); //edx + 10 = 0Ch

									info += 5;
									indx++;

								} while ( indx < vert_count );

								info3dptr = info;
								
							}

							
						}
						else // if(depth > perspective_distance)
							*/
						{
							//info[0] = tex->drawtype;
							info[0] = 6;
							//info[1] = tex->tpage;
							info[1] = color;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if(vert_count > 0)
							{

								do
								{
									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									info[2] = (short int) vertices[indx].g;

									/*
									*(float*)&info[3] = vertices[indx].z;
									*(float*)&info[5] = vertices[indx].u;
									*(float*)&info[7] = vertices[indx].v;
									*/

									info += 3;
									indx++;

								}while ( indx < vert_count );

								info3dptr = info;
								
							} // if(vert_count > 0)
							
							surfacenum++;

						} // if(depth > perspective_distance)

						//surfacenum++;

					} // if(vert_count)

					goto LABEL_32;

				} //if kosoe > 0
			}
			/*
			else // if && clip
			{
				//отбрасывание задних поверхностей
				//если поверхность обращена от зрителя - пропускаем ее
				if ( phd_VisibleZClip(vns[0], vns[1], vns[2]) )
				{

					points[0].xs = (float) vns[0]->xs;
					points[0].ys = (float) vns[0]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;

					points[0].xv = (float) vns[0]->xv;
					points[0].yv = (float) vns[0]->yv;
					points[0].zv = (float) vns[0]->zv;
					points[0].u = tex->uv[0].u1;
					points[0].v = tex->uv[0].v1;
					points[0].g = vns[0]->g;




					points[1].xs = (float) vns[1]->xs;
					points[1].ys = (float) vns[1]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;
					
					points[1].xv = (float) vns[1]->xv;
					points[1].yv = (float) vns[1]->yv;
					points[1].zv = (float) vns[1]->zv;
					points[1].u = tex->uv[1].u1;
					points[1].v = tex->uv[1].v1;
					points[1].g = vns[1]->g;




					points[2].xs = (float) vns[2]->xs;
					points[2].ys = (float) vns[2]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;


					points[2].xv = (float) vns[2]->xv;
					points[2].yv = (float) vns[2]->yv;
					points[2].zv = (float) vns[2]->zv;
					points[2].u = tex->uv[2].u1;
					points[2].v = tex->uv[2].v1;
					points[2].g = vns[2]->g;



					points[3].xs = (float) vns[3]->xs;
					points[3].ys = (float) vns[3]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;

					points[3].xv = (float) vns[3]->xv;
					points[3].yv = (float) vns[3]->yv;
					points[3].zv = (float) vns[3]->zv;
					points[3].u = tex->uv[3].u1;
					points[3].v = tex->uv[3].v1;
					points[3].g = vns[3]->g;


					//vert_count = 4;

					vert_count = ZedClipper(4, &points[0], &vertices[0]);

					
					if ( vert_count )
					{
						//v27 = &vertices[0];
						goto ClipVerts;
				    }

					goto Skip_ClipVerts;

				} // if ( phd_VisibleZClip
				
				
			}	//else if any clip
			*/

		}
LABEL_32:

			num_TexturedQuad--;
		 // if not all clip

    } while (num_TexturedQuad > 0);

	} //if (number > )

    return obj_ptr;
}

//-------------------------------------------

int16_t *DrawObjectG3(int16_t *obj_ptr, int32_t number)
{
    PHD_VBUF *vns[4];
	VBUF2 vertices[8];
//	POINT_INFO points[4];
    //PHD_TEXTURE *tex;
	int color;
	//int32_t vert_count = 4;
	int32_t vert_count = 0;

	int num_TexturedQuad = number;

	//перед вызовом DrawObjectGT4
	//obj_ptr + 0 количество GT4 = index 1 - 2 байта
	//obj_ptr + 1 vert #1 = index 2 - 2 байта
	//obj_ptr + 2 vert #2 = index 3 - 2 байта
	//obj_ptr + 3 vert #3 = index 4 - 2 байта
	//obj_ptr + 4 vert #4 = index 5 - 2 байта
	//obj_ptr + 5 = g_PhdTextureInfo = index 6 - 2 байта
	
	//внутри DrawObjectGT4 eax = obj_ptr = пропускается количество GT4
	//obj_ptr + 0 vert #1 in 2 bytes + 0 bytes
	//obj_ptr + 1 vert #2 in 2 bytes + 2 bytes 
	//obj_ptr + 2 vert #3 in 2 bytes + 4 bytes
	//obj_ptr + 3 vert #4 in 2 bytes + 6 bytes
	//obj_ptr + 4 g_PhdTextureInfo in 2 bytes

	//вычисление текстуры
	//start memory obj_ptr = byte4(1122) byte4(3344) texinfo byte4(5500)
	//вычисление текстуры eax = obj_ptr
	//mov eax, [eax + 6] => start memory obj = byte4(11 22) byte4(33 => eax = 00 00 55 44
	//sar eax >> 16 => eax = 55

	//sizeof(PHD_TEXTURE) = 20 bytes
	//sizeof(PHD_VBUF) = 32 bytes
	//sizeof(obj) = 12 bytes
	
	//------------------
	//vert buff VERT m_VBuf -> sizeof(PHD_VBUF) = 32
	// x	+0
	// y	+4
	// z	+8
	// xs	+12
	// yx	+16
	// dist	+20
	// clip	+22
	// g	+24
	// tu	+26
	// tv	+28

	if(number > 0)
	{

	do
	{
        vns[0] = &m_VBuf[*obj_ptr++];
        vns[1] = &m_VBuf[*obj_ptr++];
        vns[2] = &m_VBuf[*obj_ptr++];
        //vns[3] = &m_VBuf[*obj_ptr++];

		//tex = &g_PhdTextureInfo[*obj_ptr++];
		color = *obj_ptr++;
        
		/*
		//результат if ноль инвертируем
		//все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//нету вобще отсечения
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		*/

		/*
		результат if 1 инвертируем

		//все вершины с одной стороны экрана - полигон не видим
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/


		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		//вариант 2 - нету вобще отсечения (в пределах экрана)
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)

		/*
		//вариант 1 - все вершины с разных сторон (зритель близко к полигону)
		int clip_flags1 = 1;
		int clip_flags2 = 2;
		int clip_flags3 = 4;
		int clip_flags4 = 8;

		//вариант 2 - нету вобще отсечения (в пределах экрана)
		int clip_flags1 = 0;
		int clip_flags2 = 0;
		int clip_flags3 = 0;
		int clip_flags4 = 0;
		
		//вариант 3 - все вершины с одной стороны экрана (полигон не видим)
		int clip_flags1 = 1;
		int clip_flags2 = 1;
		int clip_flags3 = 1;
		int clip_flags4 = 1;
		*/
		
		//если вариант 1 и 2
		if ( !(vns[0]->clip & vns[1]->clip & vns[2]->clip) )
		{
			//если вариант 1 и 2
			if (vns[0]->clip >= 0 && vns[1]->clip >= 0 && vns[2]->clip >= 0)
			{
				//псевдоскалярное косое умножение векторов
				//выясняем куда смотрит полигон - от зрителя или к зрителю
				if ((vns[0]->ys - vns[1]->ys) * (vns[2]->xs - vns[1]->xs)
                - (vns[2]->ys - vns[1]->ys) * (vns[0]->xs - vns[1]->xs)	> 0)
				{
					//полигон смотрит к зрителю
					vertices[0].x = (float) vns[0]->xs;
					vertices[0].y = (float) vns[0]->ys;
					vertices[0].g = (float) vns[0]->g;

					vertices[1].x = (float) vns[1]->xs;
					vertices[1].y = (float) vns[1]->ys;
					vertices[1].g = (float) vns[1]->g;

					vertices[2].x = (float) vns[2]->xs;
					vertices[2].y = (float) vns[2]->ys;
					vertices[2].g = (float) vns[2]->g;

					vert_count = 3;

					//если необходимо отсечение по границе экрана
					if (vns[0]->clip || vns[1]->clip || vns[2]->clip) 
					{
						

//		ClipVerts:		//если необходимо отсечение по границе экрана
						vert_count = ClipVertices2(vert_count, &vertices[0]); 
					}

//		Skip_ClipVerts:

					if(vert_count)
					{
						int depth = (vns[0]->zv + vns[1]->zv + vns[2]->zv) /3;
						
						int32_t * sort = sort3dptr;
						int16_t * info = info3dptr;
		
						sort[0] = (int32_t)info;
						sort[1] = depth;

						sort3dptr += 2;

						//info3dptr 2 bytes - 16 бит
						//0 draw routine
						//1 texture page
						//2 num coords ie vert count
						//3 vert1
						//4 vert2
						//5 vert3

						//info3dptr[0] = tex->drawtype;
						//info3dptr[1] = tex->tpage;
						//info3dptr[2] = vert_count;

						/*
						if(depth > perspective_distance)
						{
							
							info[0] = tex->drawtype;
							//info[1] = tex->tpage;
							info[2] = vert_count;

							if(vert_count > 0)
							{
								int32_t indx = 0;

								info += 3; //пропускаем drawType, texNum, vertNumbrs

								do {

									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									
									//vertices[0].z ; //edx + 8
									
									info[2] = (short int) (vertices[indx].u / vertices[indx].z); //edx + 12 = 0Ch
									info[4] = (short int) (vertices[indx].v / vertices[indx].z); //edx + 10 = 0Ch

									info += 5;
									indx++;

								} while ( indx < vert_count );

								info3dptr = info;
								
							}

							
						}
						else // if(depth > perspective_distance)
							*/
						{
							//info[0] = tex->drawtype;
							info[0] = 6;
							//info[1] = tex->tpage;
							info[1] = color;
							info[2] = vert_count;

							info += 3;

							int32_t indx = 0;

							if(vert_count > 0)
							{

								do
								{
									info[0] = (short int) vertices[indx].x; //edx
									info[1] = (short int) vertices[indx].y; //edx + 4
									info[2] = (short int) vertices[indx].g;

									/*
									*(float*)&info[3] = vertices[indx].z;
									*(float*)&info[5] = vertices[indx].u;
									*(float*)&info[7] = vertices[indx].v;
									*/

									info += 3;
									indx++;

								}while ( indx < vert_count );

								info3dptr = info;
								
							} // if(vert_count > 0)
							
							surfacenum++;

						} // if(depth > perspective_distance)

						//surfacenum++;

					} // if(vert_count)

					goto LABEL_32;

				} //if kosoe > 0
			}
			/*
			else // if && clip
			{
				//отбрасывание задних поверхностей
				//если поверхность обращена от зрителя - пропускаем ее
				if ( phd_VisibleZClip(vns[0], vns[1], vns[2]) )
				{

					points[0].xs = (float) vns[0]->xs;
					points[0].ys = (float) vns[0]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;

					points[0].xv = (float) vns[0]->xv;
					points[0].yv = (float) vns[0]->yv;
					points[0].zv = (float) vns[0]->zv;
					points[0].u = tex->uv[0].u1;
					points[0].v = tex->uv[0].v1;
					points[0].g = vns[0]->g;




					points[1].xs = (float) vns[1]->xs;
					points[1].ys = (float) vns[1]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;
					
					points[1].xv = (float) vns[1]->xv;
					points[1].yv = (float) vns[1]->yv;
					points[1].zv = (float) vns[1]->zv;
					points[1].u = tex->uv[1].u1;
					points[1].v = tex->uv[1].v1;
					points[1].g = vns[1]->g;




					points[2].xs = (float) vns[2]->xs;
					points[2].ys = (float) vns[2]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;


					points[2].xv = (float) vns[2]->xv;
					points[2].yv = (float) vns[2]->yv;
					points[2].zv = (float) vns[2]->zv;
					points[2].u = tex->uv[2].u1;
					points[2].v = tex->uv[2].v1;
					points[2].g = vns[2]->g;



					points[3].xs = (float) vns[3]->xs;
					points[3].ys = (float) vns[3]->ys;
					
					//points[0].z = 8589934592.0f / vns[0]->zv;
					//points[0].u = points[0].z * tex->uv[0].u1;
					//points[0].v = points[0].z * tex->uv[0].v1;

					points[3].xv = (float) vns[3]->xv;
					points[3].yv = (float) vns[3]->yv;
					points[3].zv = (float) vns[3]->zv;
					points[3].u = tex->uv[3].u1;
					points[3].v = tex->uv[3].v1;
					points[3].g = vns[3]->g;


					//vert_count = 4;

					vert_count = ZedClipper(4, &points[0], &vertices[0]);

					
					if ( vert_count )
					{
						//v27 = &vertices[0];
						goto ClipVerts;
				    }

					goto Skip_ClipVerts;

				} // if ( phd_VisibleZClip
				
				
			}	//else if any clip
			*/

		}
LABEL_32:

			num_TexturedQuad--;
		 // if not all clip

    } while (num_TexturedQuad > 0);

	} //if (number > )

    return obj_ptr;
}

/*
int32_t ZedClipper2(int32_t vertex_count, POINT_INFO *Source, VBUF *Vertices)
{
    float Scale;
    //vector3 Vertices[8];

	//последняя вершина
	POINT_INFO *l = &Source[Num - 1];
    int j = 0;

    for (int i = 0; i < Num; i++)
	{
		//сюда записываем результат
        vector3 *v1 = &Vertices[j];
        //последняя вершина v2
		vector3 *v2 = l;
        //нулевая (первая) вершина l
		l = &Source[i];

        if (v2->z < m_ZNear)
		{
            if (l->z < m_ZNear)
			{
                continue;
            }
            Scale = (m_ZNear - l->z) / (v2->z - l->z);
			v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }

		if (l->z < m_ZNear)
		{
            Scale = (m_ZNear - l->z) / (v2->z - l->z);
            v1->x = (v2->x - l->x) * Scale + l->x;
            v1->y = (v2->y - l->y) * Scale + l->y;
            v1->z = (v2->z - l->z) * Scale + l->z;
            v1->tu = (v2->tu - l->tu) * Scale + l->tu;
            v1->tv = (v2->tv - l->tv) * Scale + l->tv;
			v1 = &Vertices[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
            v1->z = l->z;
            v1->tu = l->tu;
            v1->tv = l->tv;
			v1 = &Vertices[++j];
        }
    }

    if (j < 3)
	{
        return 0;
    }

	for ( int i = 0; i < j; i++ )
	{
		Source[i] = Vertices[i];
	}

	return j;
}

*/

int32_t ZedClipper(int32_t vertex_count, POINT_INFO *pts, VBUF *vertices)
{

	float clip;
	POINT_INFO *pts1;
	POINT_INFO *pts0;
	
	int j = 0;

	//float near_z = ZNear;
	float near_z = Z_NEAR;

	float persp_o_near_z = g_PhdPersp / near_z;
	//float persp_o_near_z = near_z / g_PhdPersp;

	if(vertex_count == 0)
		return 0;

	VBUF * v = &vertices[0];
	pts0 = &pts[vertex_count - 1];

	int i;

	for ( i = 0; i < vertex_count; i++ )
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

			v->x = ((pts1->xv - pts0->xv) * clip + pts0->xv) * persp_o_near_z
					+ ViewPort_GetCenterX();
	
			v->y = ((pts1->yv - pts0->yv) * clip + pts0->yv) * persp_o_near_z
					+ ViewPort_GetCenterY();
            
			v->z = 8589934592.0f / near_z;

	        v->u = v->z * ((pts1->u - pts0->u) * clip + pts0->u);
			v->v = v->z * ((pts1->v - pts0->v) * clip + pts0->v);

			v->g = (pts1->g - pts0->g) * clip + pts0->g;

			v++;

			//j++;

		}

		if (near_z > pts0->zv)
		{
            clip = (near_z - pts0->zv) / (pts1->zv - pts0->zv);
            
			v->x = ((pts1->xv - pts0->xv) * clip + pts0->xv) * persp_o_near_z
                + ViewPort_GetCenterX();

            v->y = ((pts1->yv - pts0->yv) * clip + pts0->yv) * persp_o_near_z
                + ViewPort_GetCenterY();
            
			v->z = 8589934592.0f / near_z;

            v->u = v->z * ((pts1->u - pts0->u) * clip + pts0->u);
            v->v = v->z * ((pts1->v - pts0->v) * clip + pts0->v);

            v->g = (pts1->g - pts0->g) * clip + pts0->g;

            v++;
			
			//j++;
        }
		else
		{
            v->x = pts0->xs;
            v->y = pts0->ys;

            v->z = 8589934592.0f /  pts0->zv;
            v->u = pts0->u * v->z;
            v->v = pts0->v * v->z;

            v->g= pts0->g;

            v++;

			//j++;
        }
	}

	int32_t count = (int32_t) (v - vertices);
    return count < 3 ? 0 : count;

	//if( j < 3 )
	//	return 0;

	//return j;


	/*
    int32_t i;
    int32_t count;
    POINT_INFO *pts0;
    POINT_INFO *pts1;
    
	GFX_3D_Vertex *v;
    float clip;
    float persp_o_near_z;
    float multiplier;

    multiplier = 0.0625f * g_Config.brightness;
    float near_z = Output_GetNearZ();
    persp_o_near_z = g_PhdPersp / near_z;

    v = &vertices[0];
    pts0 = &pts[vertex_count - 1];
    for (i = 0; i < vertex_count; i++) {
        pts1 = pts0;
        pts0 = &pts[i];
        if (near_z > pts1->zv) {
            if (near_z > pts0->zv) {
                continue;
            }

            clip = (near_z - pts0->zv) / (pts1->zv - pts0->zv);
            v->x = ((pts1->xv - pts0->xv) * clip + pts0->xv) * persp_o_near_z
                + ViewPort_GetCenterX();
            v->y = ((pts1->yv - pts0->yv) * clip + pts0->yv) * persp_o_near_z
                + ViewPort_GetCenterY();
            v->z = near_z * 0.0001f;

            v->w = 65536.0f / near_z;
            v->s = v->w * ((pts1->u - pts0->u) * clip + pts0->u) * 0.00390625f;
            v->t = v->w * ((pts1->v - pts0->v) * clip + pts0->v) * 0.00390625f;

            v->r = v->g = v->b =
                (8192.0f - ((pts1->g - pts0->g) * clip + pts0->g)) * multiplier;
            Output_ApplyWaterEffect(&v->r, &v->g, &v->b);

            v++;
        }

        if (near_z > pts0->zv) {
            clip = (near_z - pts0->zv) / (pts1->zv - pts0->zv);
            v->x = ((pts1->xv - pts0->xv) * clip + pts0->xv) * persp_o_near_z
                + ViewPort_GetCenterX();
            v->y = ((pts1->yv - pts0->yv) * clip + pts0->yv) * persp_o_near_z
                + ViewPort_GetCenterY();
            v->z = near_z * 0.0001f;

            v->w = 65536.0f / near_z;
            v->s = v->w * ((pts1->u - pts0->u) * clip + pts0->u) * 0.00390625f;
            v->t = v->w * ((pts1->v - pts0->v) * clip + pts0->v) * 0.00390625f;

            v->r = v->g = v->b =
                (8192.0f - ((pts1->g - pts0->g) * clip + pts0->g)) * multiplier;
            Output_ApplyWaterEffect(&v->r, &v->g, &v->b);

            v++;
        } else {
            v->x = pts0->xs;
            v->y = pts0->ys;
            v->z = pts0->zv * 0.0001f;

            v->w = 65536.0f / pts0->zv;
            v->s = pts0->u * v->w * 0.00390625f;
            v->t = pts0->v * v->w * 0.00390625f;

            v->r = v->g = v->b = (8192.0f - pts0->g) * multiplier;
            Output_ApplyWaterEffect(&v->r, &v->g, &v->b);

            v++;
        }
    }

    count = v - vertices;
    return count < 3 ? 0 : count;

	
	
	//return count < 3 ? 0 : count;

	*/
	

	return 1;
}

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
            v1->g = (v2->g - l->g) * scale + l->g;
            v1->u = (v2->u - l->u) * scale + l->u;
            v1->v = (v2->v - l->v) * scale + l->v;
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
            v1->g = (v2->g - l->g) * scale + l->g;
            v1->u = (v2->u - l->u) * scale + l->u;
            v1->v = (v2->v - l->v) * scale + l->v;
            v1 = &vertices[++j];
        }
		else if (l->x > g_PhdWinxmax)
		{
            scale = (g_PhdWinxmax - l->x) / (v2->x - l->x);
            v1->x = g_PhdWinxmax;
            v1->y = (v2->y - l->y) * scale + l->y;
            v1->z = (v2->z - l->z) * scale + l->z;
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
	
	
    
	//return 1;
}

//---------------------------------

int32_t ClipVertices2(int32_t num, VBUF2 *source)
{
	
	
    float scale;
    //VBUF2 vertices[8];
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
            v1->g = (v2->g - l->g) * scale + l->g;
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
            v1->g = (v2->g - l->g) * scale + l->g;
            v1 = &vertices[++j];
        }

        if (l->x < g_SurfaceMinX)
		{
            scale = (g_SurfaceMinX - l->x) / (v2->x - l->x);
            v1->x = g_SurfaceMinX;
            v1->y = (v2->y - l->y) * scale + l->y;
            v1->g = (v2->g - l->g) * scale + l->g;
            v1 = &vertices[++j];
        }
		else if (l->x > g_PhdWinxmax)
		{
            scale = (g_PhdWinxmax - l->x) / (v2->x - l->x);
            v1->x = g_PhdWinxmax;
            v1->y = (v2->y - l->y) * scale + l->y;
            v1->g = (v2->g - l->g) * scale + l->g;
            v1 = &vertices[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
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

    for (int i = 0; i < num; i++)
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
            v1->g = (v2->g - l->g) * scale + l->g;
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
            v1->g = (v2->g - l->g) * scale + l->g;
            v1 = &source[++j];
        }

        if (l->y < g_SurfaceMinY)
		{
            scale = (g_SurfaceMinY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * scale + l->x;
            v1->y = g_SurfaceMinY;
            v1->g = (v2->g - l->g) * scale + l->g;
            v1 = &source[++j];
        }
		else if (l->y > g_SurfaceMaxY)
		{
            scale = (g_SurfaceMaxY - l->y) / (v2->y - l->y);
            v1->x = (v2->x - l->x) * scale + l->x;
            v1->y = g_SurfaceMaxY;
            v1->g = (v2->g - l->g) * scale + l->g;
            v1 = &source[++j];
        }
		else
		{
            v1->x = l->x;
            v1->y = l->y;
            v1->g = l->g;
            v1 = &source[++j];
        }
    }

    if (j < 3)
	{
        return 0;
    }

	return j;
	
	
    
	//return 1;

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

    if (r->num_lights == 0) {
        g_LsAdder = r->ambient;
        g_LsDivider = 0;
    } else {
        int32_t ambient = 0x1FFF - r->ambient;
        int32_t brightest = 0;

        PHD_VECTOR ls = { 0, 0, 0 };
        for (int i = 0; i < r->num_lights; i++) {
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

            if (shade > brightest) {
                brightest = shade;
                ls = lc;
            }
        }

        g_LsAdder = (ambient + brightest) / 2;
        if (brightest == ambient) {
            g_LsDivider = 0;
        } else {
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

/*
void InitialiseLaraLoad(int16_t item_num)
{
    g_Lara.item_number = item_num;
    g_LaraItem = &g_Items[item_num];
}
*/

/*
void ControlLaraExtra(int16_t item_num)
{
    AnimateItem(&g_Items[item_num]);
}
*/
void InitialiseEvilLara(int16_t item_num)
{
    g_Objects[O_EVIL_LARA].frame_base = g_Objects[O_LARA].frame_base;
    g_Items[item_num].data = NULL;
}

void ControlEvilLara(int16_t item_num)
{
    ITEM_INFO *item = &g_Items[item_num];

    if (item->hit_points < LARA_HITPOINTS) {
        g_LaraItem->hit_points -= (LARA_HITPOINTS - item->hit_points) * 10;
        item->hit_points = LARA_HITPOINTS;
    }

    if (!item->data) {
        int32_t x = 2 * 36 * WALL_L - g_LaraItem->pos.x;
        int32_t y = g_LaraItem->pos.y;
        int32_t z = 2 * 60 * WALL_L - g_LaraItem->pos.z;

        int16_t room_num = item->room_number;
        FLOOR_INFO *floor = GetFloor(x, y, z, &room_num);
        int32_t h = GetHeight(floor, x, y, z);
        item->floor = h;

        room_num = g_LaraItem->room_number;
        floor = GetFloor(
            g_LaraItem->pos.x, g_LaraItem->pos.y, g_LaraItem->pos.z, &room_num);
        int32_t lh = GetHeight(
            floor, g_LaraItem->pos.x, g_LaraItem->pos.y, g_LaraItem->pos.z);

        item->anim_number = g_LaraItem->anim_number;
        item->frame_number = g_LaraItem->frame_number;
        item->pos.x = x;
        item->pos.y = y;
        item->pos.z = z;
        item->pos.x_rot = g_LaraItem->pos.x_rot;
        item->pos.y_rot = g_LaraItem->pos.y_rot - PHD_180;
        item->pos.z_rot = g_LaraItem->pos.z_rot;
        ItemNewRoom(item_num, g_LaraItem->room_number);

        if (h >= lh + WALL_L && !g_LaraItem->gravity_status) {
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

    if (item->data) {
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

    for (int i = 0; i < LM_NUMBER_OF; i++) {
        old_mesh_ptrs[i] = g_Lara.mesh_ptrs[i];
        g_Lara.mesh_ptrs[i] = g_Meshes[g_Objects[O_EVIL_LARA].mesh_index + i];
    }

    DrawLara(item);

    for (int i = 0; i < LM_NUMBER_OF; i++) {
        g_Lara.mesh_ptrs[i] = old_mesh_ptrs[i];
    }
}

void AnimateItem(ITEM_INFO *item)
{
    item->touch_bits = 0;
    item->hit_status = 0;

    ANIM_STRUCT *anim = &g_Anims[item->anim_number];

    item->frame_number++;

    if (anim->number_changes > 0) {
        if (GetChange(item, anim)) {
            anim = &g_Anims[item->anim_number];
            item->current_anim_state = anim->current_anim_state;

            if (item->required_anim_state == item->current_anim_state) {
                item->required_anim_state = 0;
            }
        }
    }

    if (item->frame_number > anim->frame_end) {
        if (anim->number_commands > 0) {
            int16_t *command = &g_AnimCommands[anim->command_index];
            for (int i = 0; i < anim->number_commands; i++) {
                switch (*command++) {
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
        if (item->required_anim_state == item->current_anim_state) {
            item->required_anim_state = 0;
        }
    }

    if (anim->number_commands > 0) {
        int16_t *command = &g_AnimCommands[anim->command_index];
        for (int i = 0; i < anim->number_commands; i++)
		{
            switch (*command++) {
            case AC_MOVE_ORIGIN:
                command += 3;
                break;

            case AC_JUMP_VELOCITY:
                command += 2;
                break;

            case AC_SOUND_FX:
                if (item->frame_number == command[0])
				{
                    Sound_Effect(
                        command[1], &item->pos,
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

/*
FLOOR_INFO *GetFloor(int32_t x, int32_t y, int32_t z, int16_t *room_num)
{
    int16_t data;
    FLOOR_INFO *floor;
    ROOM_INFO *r = &g_RoomInfo[*room_num];
    do {
        int32_t x_floor = (z - r->z) >> WALL_SHIFT;
        int32_t y_floor = (x - r->x) >> WALL_SHIFT;

        if (x_floor <= 0) {
            x_floor = 0;
            if (y_floor < 1) {
                y_floor = 1;
            } else if (y_floor > r->y_size - 2) {
                y_floor = r->y_size - 2;
            }
        } else if (x_floor >= r->x_size - 1) {
            x_floor = r->x_size - 1;
            if (y_floor < 1) {
                y_floor = 1;
            } else if (y_floor > r->y_size - 2) {
                y_floor = r->y_size - 2;
            }
        } else if (y_floor < 0) {
            y_floor = 0;
        } else if (y_floor >= r->y_size) {
            y_floor = r->y_size - 1;
        }

        floor = &r->floor[x_floor + y_floor * r->x_size];
        if (!floor->index) {
            break;
        }

        data = GetDoor(floor);
        if (data != NO_ROOM) {
            *room_num = data;
            r = &g_RoomInfo[data];
        }
    } while (data != NO_ROOM);

    if (y >= ((int32_t)floor->floor << 8)) {
        do {
            if (floor->pit_room == NO_ROOM) {
                break;
            }

            *room_num = floor->pit_room;

            r = &g_RoomInfo[floor->pit_room];
            int32_t x_floor = (z - r->z) >> WALL_SHIFT;
            int32_t y_floor = (x - r->x) >> WALL_SHIFT;
            floor = &r->floor[x_floor + y_floor * r->x_size];
        } while (y >= ((int32_t)floor->floor << 8));
    } else if (y < ((int32_t)floor->ceiling << 8)) {
        do {
            if (floor->sky_room == NO_ROOM) {
                break;
            }

            *room_num = floor->sky_room;

            r = &g_RoomInfo[floor->sky_room];
            int32_t x_floor = (z - r->z) >> WALL_SHIFT;
            int32_t y_floor = (x - r->x) >> WALL_SHIFT;
            floor = &r->floor[x_floor + y_floor * r->x_size];
        } while (y < ((int32_t)floor->ceiling << 8));
    }

    return floor;
}

int16_t GetHeight(FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z)
{
    g_HeightType = HT_WALL;
    while (floor->pit_room != NO_ROOM) {
        ROOM_INFO *r = &g_RoomInfo[floor->pit_room];
        int32_t x_floor = (z - r->z) >> WALL_SHIFT;
        int32_t y_floor = (x - r->x) >> WALL_SHIFT;
        floor = &r->floor[x_floor + y_floor * r->x_size];
    }

    int16_t height = floor->floor << 8;

    g_TriggerIndex = NULL;

    if (!floor->index) {
        return height;
    }

    int16_t *data = &g_FloorData[floor->index];
    int16_t type;
    int16_t trigger;
    do {
        type = *data++;

        switch (type & DATA_TYPE) {
        case FT_TILT: {
            int16_t xoff = data[0] >> 8;
            int16_t yoff = (int8_t)data[0];

            if (!g_ChunkyFlag || (ABS(xoff) <= 2 && ABS(yoff) <= 2)) {
                if (ABS(xoff) > 2 || ABS(yoff) > 2) {
                    g_HeightType = HT_BIG_SLOPE;
                } else {
                    g_HeightType = HT_SMALL_SLOPE;
                }

                if (xoff < 0) {
                    height -= (int16_t)((xoff * (z & (WALL_L - 1))) >> 2);
                } else {
                    height +=
                        (int16_t)((xoff * ((WALL_L - 1 - z) & (WALL_L - 1))) >> 2);
                }

                if (yoff < 0) {
                    height -= (int16_t)((yoff * (x & (WALL_L - 1))) >> 2);
                } else {
                    height +=
                        (int16_t)((yoff * ((WALL_L - 1 - x) & (WALL_L - 1))) >> 2);
                }
            }

            data++;
            break;
        }

        case FT_ROOF:
        case FT_DOOR:
            data++;
            break;

        case FT_LAVA:
            g_TriggerIndex = data - 1;
            break;

        case FT_TRIGGER:
            if (!g_TriggerIndex) {
                g_TriggerIndex = data - 1;
            }

            data++;
            do {
                trigger = *data++;
                if (TRIG_BITS(trigger) != TO_OBJECT) {
                    if (TRIG_BITS(trigger) == TO_CAMERA) {
                        trigger = *data++;
                    }
                } else {
                    ITEM_INFO *item = &g_Items[trigger & VALUE_BITS];
                    OBJECT_INFO *object = &g_Objects[item->object_number];
                    if (object->floor) {
                        object->floor(item, x, y, z, &height);
                    }
                }
            } while (!(trigger & END_BIT));
            break;

        default:
            //Shell_ExitSystem("GetHeight(): Unknown type");
            break;
        }
    } while (!(type & END_BIT));

    return height;
}
*/

/*
void ItemNewRoom(int16_t item_num, int16_t room_num)
{
    ITEM_INFO *item = &g_Items[item_num];
    ROOM_INFO *r = &g_RoomInfo[item->room_number];

    int16_t linknum = r->item_number;
    if (linknum == item_num) {
        r->item_number = item->next_item;
    } else {
        for (; linknum != NO_ITEM; linknum = g_Items[linknum].next_item) {
            if (g_Items[linknum].next_item == item_num) {
                g_Items[linknum].next_item = item->next_item;
                break;
            }
        }
    }

    r = &g_RoomInfo[room_num];
    item->room_number = room_num;
    item->next_item = r->item_number;
    r->item_number = item_num;
}
*/
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

        case TT_KEY: {
            int16_t value = *data++ & VALUE_BITS;
            if (!KeyTrigger(value))
			{
                return;
            }
            break;
        }

        case TT_PICKUP: {
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
    do {
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

            if (type == TT_COMBAT) {
                break;
            }

            if (type == TT_SWITCH && timer && switch_off) {
                break;
            }

            if (g_Camera.number == g_Camera.last && type != TT_SWITCH) {
                break;
            }

            g_Camera.timer = camera_timer;
            if (g_Camera.timer != 1) {
                g_Camera.timer *= FRAMES_PER_SECOND;
            }

            if (camera_flags & IF_ONESHOT) {
                g_Camera.fixed[g_Camera.number].flags |= IF_ONESHOT;
            }

            g_Camera.speed = ((camera_flags & IF_CODE_BITS) >> 6) + 1;
            g_Camera.type = heavy ? CAM_HEAVY : CAM_FIXED;
            break;
        }

        case TO_TARGET:
            camera_item = &g_Items[value];
            break;

        case TO_SINK: {
            OBJECT_VECTOR *obvector = &g_Camera.fixed[value];

            if (g_Lara.LOT.required_box != obvector->flags) {
                g_Lara.LOT.target.x = obvector->x;
                g_Lara.LOT.target.y = obvector->y;
                g_Lara.LOT.target.z = obvector->z;
                g_Lara.LOT.required_box = obvector->flags;
            }

            g_Lara.current_active = obvector->data * 6;
            break;
        }

        case TO_FLIPMAP:
            if (g_FlipMapTable[value] & IF_ONESHOT) {
                break;
            }

            if (type == TT_SWITCH) {
                g_FlipMapTable[value] ^= flags & IF_CODE_BITS;
            } else if (flags & IF_CODE_BITS) {
                g_FlipMapTable[value] |= flags & IF_CODE_BITS;
            }

            if ((g_FlipMapTable[value] & IF_CODE_BITS) == IF_CODE_BITS) {
                if (flags & IF_ONESHOT) {
                    g_FlipMapTable[value] |= IF_ONESHOT;
                }

                if (!g_FlipStatus) {
                    flip = 1;
                }
            } else if (g_FlipStatus) {
                flip = 1;
            }
            break;

        case TO_FLIPON:
            if ((g_FlipMapTable[value] & IF_CODE_BITS) == IF_CODE_BITS
                && !g_FlipStatus) {
                flip = 1;
            }
            break;

        case TO_FLIPOFF:
            if ((g_FlipMapTable[value] & IF_CODE_BITS) == IF_CODE_BITS
                && g_FlipStatus) {
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
            //Control_TriggerMusicTrack(value, flags, type);
            break;

        case TO_SECRET:
            if ((g_SaveGame.secrets & (1 << value))) {
                break;
            }
            g_SaveGame.secrets |= 1 << value;
            //Music_Play(13);
            Sound_Effect(SFX_SECRET, NULL, SPM_ALWAYS);
            break;
        }
    } while (!(trigger & END_BIT));

    if (camera_item
        && (g_Camera.type == CAM_FIXED || g_Camera.type == CAM_HEAVY))
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
    } else {
        frame = frmptr[0];
    }

    // save matrix for hair
    saved_matrix = *g_PhdMatrixPtr;

    Output_DrawShadow(object->shadow_size, frame, item);
    phd_PushMatrix();
    phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

    int32_t clip = S_GetObjectBounds(frame);
    if (!clip) {
        phd_PopMatrix();
        return;
    }

    phd_PushMatrix();

    CalculateObjectLighting(item, frame);

    int32_t *bone = &g_AnimBones[object->bone_index];
    int32_t *packed_rotation = (int32_t *)(frame + FRAME_ROT);

    phd_TranslateRel(
        frame[FRAME_POS_X], frame[FRAME_POS_Y], frame[FRAME_POS_Z]);
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
    if (g_Lara.gun_status == LGS_READY || g_Lara.gun_status == LGS_DRAW
        || g_Lara.gun_status == LGS_UNDRAW) {
        fire_arms = g_Lara.gun_type;
    }

    switch (fire_arms) {
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

        packed_rotation =
            (int32_t
                 *)(g_Lara.right_arm.frame_base + g_Lara.right_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_RotYXZ(
            g_Lara.right_arm.y_rot, g_Lara.right_arm.x_rot,
            g_Lara.right_arm.z_rot);
        phd_RotYXZpack(packed_rotation[LM_UARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

        phd_TranslateRel(bone[33], bone[34], bone[35]);
        phd_RotYXZpack(packed_rotation[LM_LARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

        phd_TranslateRel(bone[37], bone[38], bone[39]);
        phd_RotYXZpack(packed_rotation[LM_HAND_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

        if (g_Lara.right_arm.flash_gun) {
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

        packed_rotation =
            (int32_t
                 *)(g_Lara.left_arm.frame_base + g_Lara.left_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_RotYXZ(
            g_Lara.left_arm.y_rot, g_Lara.left_arm.x_rot,
            g_Lara.left_arm.z_rot);
        phd_RotYXZpack(packed_rotation[LM_UARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

        phd_TranslateRel(bone[45], bone[46], bone[47]);
        phd_RotYXZpack(packed_rotation[LM_LARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

        phd_TranslateRel(bone[49], bone[50], bone[51]);
        phd_RotYXZpack(packed_rotation[LM_HAND_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

        if (g_Lara.left_arm.flash_gun) {
            DrawGunFlash(fire_arms, clip);
        }
        if (g_Lara.right_arm.flash_gun) {
            *g_PhdMatrixPtr = saved_matrix;
            DrawGunFlash(fire_arms, clip);
        }

        phd_PopMatrix();
        break;

    case LGT_SHOTGUN:
        phd_PushMatrix();

        packed_rotation =
            (int32_t
                 *)(g_Lara.right_arm.frame_base + g_Lara.right_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_TranslateRel(bone[29], bone[30], bone[31]);
        phd_RotYXZpack(packed_rotation[LM_UARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

        phd_TranslateRel(bone[33], bone[34], bone[35]);
        phd_RotYXZpack(packed_rotation[LM_LARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

        phd_TranslateRel(bone[37], bone[38], bone[39]);
        phd_RotYXZpack(packed_rotation[LM_HAND_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

        if (g_Lara.right_arm.flash_gun) {
            saved_matrix = *g_PhdMatrixPtr;
        }

        phd_PopMatrix();

        phd_PushMatrix();

        packed_rotation =
            (int32_t
                 *)(g_Lara.left_arm.frame_base + g_Lara.left_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_TranslateRel(bone[41], bone[42], bone[43]);
        phd_RotYXZpack(packed_rotation[LM_UARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

        phd_TranslateRel(bone[45], bone[46], bone[47]);
        phd_RotYXZpack(packed_rotation[LM_LARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

        phd_TranslateRel(bone[49], bone[50], bone[51]);
        phd_RotYXZpack(packed_rotation[LM_HAND_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

        if (g_Lara.right_arm.flash_gun) {
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

/*
void LavaBurn(ITEM_INFO *item)
{
    if (g_Lara.water_status == LWS_CHEAT)
	{
        return;
    }

    if (item->hit_points < 0)
	{
        return;
    }

    int16_t room_num = item->room_number;
    FLOOR_INFO *floor = GetFloor(item->pos.x, 32000, item->pos.z, &room_num);

    if (item->floor != GetHeight(floor, item->pos.x, 32000, item->pos.z))
	{
        return;
    }

    item->hit_points = -1;
    item->hit_status = 1;
    for (int i = 0; i < 10; i++)
	{
        int16_t fx_num = CreateEffect(item->room_number);
        
		if (fx_num != NO_ITEM)
		{
            FX_INFO *fx = &g_Effects[fx_num];
            fx->object_number = O_FLAME;
            fx->frame_number =
                (g_Objects[O_FLAME].nmeshes * Random_GetControl()) / 0x7FFF;
            fx->counter = -1 - Random_GetControl() * 24 / 0x7FFF;
        }
    }
}

*/
void RefreshCamera(int16_t type, int16_t *data)
{
    int16_t trigger;
    int16_t target_ok = 2;
    do {
        trigger = *data++;
        int16_t value = trigger & VALUE_BITS;

        switch (TRIG_BITS(trigger)) {
        case TO_CAMERA:
            data++;

            if (value == g_Camera.last) {
                g_Camera.number = value;

                if (g_Camera.timer < 0 || g_Camera.type == CAM_LOOK
                    || g_Camera.type == CAM_COMBAT) {
                    g_Camera.timer = -1;
                    target_ok = 0;
                } else {
                    g_Camera.type = CAM_FIXED;
                    target_ok = 1;
                }
            } else {
                target_ok = 0;
            }
            break;

        case TO_TARGET:
            if (g_Camera.type != CAM_LOOK && g_Camera.type != CAM_COMBAT) {
                g_Camera.item = &g_Items[value];
            }
            break;
        }
    } while (!(trigger & END_BIT));

    if (g_Camera.item != NULL) {
        if (!target_ok
            || (target_ok == 2 && g_Camera.item->looked_at
                && g_Camera.item != g_Camera.last_item)) {
            g_Camera.item = NULL;
        }
    }
}

/*
int32_t SwitchTrigger(int16_t item_num, int16_t timer)
{
    ITEM_INFO *item = &g_Items[item_num];
    if (item->status != IS_DEACTIVATED)
	{
        return 0;
    }
    if (item->current_anim_state == SWITCH_STATE_OFF && timer > 0)
	{
        item->timer = timer;
        if (timer != 1)
		{
            item->timer *= FRAMES_PER_SECOND;
        }
        item->status = IS_ACTIVE;
    } 
	else
	{
        RemoveActiveItem(item_num);
        item->status = IS_NOT_ACTIVE;
    }
    return 1;
}
*/
/*
int32_t KeyTrigger(int16_t item_num)
{
    ITEM_INFO *item = &g_Items[item_num];
    if (item->status == IS_ACTIVE && g_Lara.gun_status != LGS_HANDSBUSY)
	{
        item->status = IS_DEACTIVATED;
        return 1;
    }
    return 0;
}
*/

/*
int32_t PickupTrigger(int16_t item_num)
{
    ITEM_INFO *item = &g_Items[item_num];
    if (item->status != IS_INVISIBLE) {
        return 0;
    }
    item->status = IS_DEACTIVATED;
    return 1;
}
*/
/*
void AddActiveItem(int16_t item_num)
{
    ITEM_INFO *item = &g_Items[item_num];

    if (!g_Objects[item->object_number].control)
	{
        item->status = IS_NOT_ACTIVE;
        return;
    }

    if (item->active)
	{
        //Shell_ExitSystemFmt(
          //  "Item(%d)(Obj%d) already Active\n", item_num, item->object_number);
    }

    item->active = 1;
    item->next_active = g_NextItemActive;
    g_NextItemActive = item_num;
}
*/
/*
int32_t EnableBaddieAI(int16_t item_num, int32_t always)
{
    if (g_Items[item_num].data)
	{
        return 1;
    }

    if (m_SlotsUsed < NUM_SLOTS)
	{
        for (int32_t slot = 0; slot < NUM_SLOTS; slot++)
		{
            CREATURE_INFO *creature = &m_BaddieSlots[slot];
            if (creature->item_num == NO_ITEM)
			{
                InitialiseSlot(item_num, slot);
                return 1;
            }
        }
        //Shell_ExitSystem("UnpauseBaddie() grimmer!");
    }

    int32_t worst_dist = 0;
    if (!always) {
        ITEM_INFO *item = &g_Items[item_num];
        int32_t x = (item->pos.x - g_Camera.pos.x) >> 8;
        int32_t y = (item->pos.y - g_Camera.pos.y) >> 8;
        int32_t z = (item->pos.z - g_Camera.pos.z) >> 8;
        worst_dist = SQUARE(x) + SQUARE(y) + SQUARE(z);
    }

    int32_t worst_slot = -1;
    for (int32_t slot = 0; slot < NUM_SLOTS; slot++) {
        CREATURE_INFO *creature = &m_BaddieSlots[slot];
        ITEM_INFO *item = &g_Items[creature->item_num];
        int32_t x = (item->pos.x - g_Camera.pos.x) >> 8;
        int32_t y = (item->pos.y - g_Camera.pos.y) >> 8;
        int32_t z = (item->pos.z - g_Camera.pos.z) >> 8;
        int32_t dist = SQUARE(x) + SQUARE(y) + SQUARE(z);
        if (dist > worst_dist) {
            worst_dist = dist;
            worst_slot = slot;
        }
    }

    if (worst_slot < 0) {
        return 0;
    }

    g_Items[m_BaddieSlots[worst_slot].item_num].status = IS_INVISIBLE;
    DisableBaddieAI(m_BaddieSlots[worst_slot].item_num);
    InitialiseSlot(item_num, worst_slot);
    return 1;
}
*/


void FlipMap()
{
    //Sound_StopAmbientSounds();

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
    //if (!g_Config.enable_braid || !g_Objects[O_HAIR].loaded) {
	if (!g_Objects[O_HAIR].loaded) {
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

    switch (weapon_type) {
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

/*
int32_t GetChange(ITEM_INFO *item, ANIM_STRUCT *anim)
{
    if (item->current_anim_state == item->goal_anim_state) {
        return 0;
    }

    ANIM_CHANGE_STRUCT *change = &g_AnimChanges[anim->change_index];
    for (int i = 0; i < anim->number_changes; i++, change++) {
        if (change->goal_anim_state == item->goal_anim_state) {
            ANIM_RANGE_STRUCT *range = &g_AnimRanges[change->range_index];
            for (int j = 0; j < change->number_ranges; j++, range++) {
                if (item->frame_number >= range->start_frame
                    && item->frame_number <= range->end_frame) {
                    item->anim_number = range->link_anim_num;
                    item->frame_number = range->link_frame_num;
                    return 1;
                }
            }
        }
    }

    return 0;
}
*/
void TranslateItem(ITEM_INFO *item, int32_t x, int32_t y, int32_t z)
{
    int32_t c = phd_cos(item->pos.y_rot);
    int32_t s = phd_sin(item->pos.y_rot);

    item->pos.x += (c * x + s * z) >> W2V_SHIFT;
    item->pos.y += y;
    item->pos.z += (c * z - s * x) >> W2V_SHIFT;
}
/*
bool Sound_Effect(int32_t sfx_num, PHD_3DPOS *pos, uint32_t flags)
{
	return true;
}
*/
/*
int16_t GetDoor(FLOOR_INFO *floor)
{
    if (!floor->index) {
        return NO_ROOM;
    }

    int16_t *data = &g_FloorData[floor->index];
    int16_t type = *data++;

    if (type == FT_TILT) {
        data++;
        type = *data++;
    }

    if (type == FT_ROOF) {
        data++;
        type = *data++;
    }

    if ((type & DATA_TYPE) == FT_DOOR) {
        return *data;
    }
    return NO_ROOM;
}
*/
void DrawLaraInt(
    ITEM_INFO *item, int16_t *frame1, int16_t *frame2, int32_t frac,
    int32_t rate)
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
    if (!clip) {
        phd_PopMatrix();
        return;
    }

    phd_PushMatrix();

    CalculateObjectLighting(item, frame1);

    int32_t *bone = &g_AnimBones[object->bone_index];
    int32_t *packed_rotation1 = (int32_t *)(frame1 + FRAME_ROT);
    int32_t *packed_rotation2 = (int32_t *)(frame2 + FRAME_ROT);

    InitInterpolate(frac, rate);

    phd_TranslateRel_ID(
        frame1[FRAME_POS_X], frame1[FRAME_POS_Y], frame1[FRAME_POS_Z],
        frame2[FRAME_POS_X], frame2[FRAME_POS_Y], frame2[FRAME_POS_Z]);

    phd_RotYXZpack_I(packed_rotation1[LM_HIPS], packed_rotation2[LM_HIPS]);
    Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_HIPS], clip);

    phd_PushMatrix_I();

    phd_TranslateRel_I(bone[1], bone[2], bone[3]);
    phd_RotYXZpack_I(
        packed_rotation1[LM_THIGH_L], packed_rotation2[LM_THIGH_L]);
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
    phd_RotYXZpack_I(
        packed_rotation1[LM_THIGH_R], packed_rotation2[LM_THIGH_R]);
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
    if (g_Lara.gun_status == LGS_READY || g_Lara.gun_status == LGS_DRAW
        || g_Lara.gun_status == LGS_UNDRAW) {
        fire_arms = g_Lara.gun_type;
    }

    switch (fire_arms) {
    case LGT_UNARMED:
        phd_PushMatrix_I();

        phd_TranslateRel_I(bone[29], bone[30], bone[31]);
        phd_RotYXZpack_I(
            packed_rotation1[LM_UARM_R], packed_rotation2[LM_UARM_R]);
        Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_UARM_R], clip);

        phd_TranslateRel_I(bone[33], bone[34], bone[35]);
        phd_RotYXZpack_I(
            packed_rotation1[LM_LARM_R], packed_rotation2[LM_LARM_R]);
        Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_LARM_R], clip);

        phd_TranslateRel_I(bone[37], bone[38], bone[39]);
        phd_RotYXZpack_I(
            packed_rotation1[LM_HAND_R], packed_rotation2[LM_HAND_R]);
        Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_HAND_R], clip);

        phd_PopMatrix_I();

        phd_PushMatrix_I();

        phd_TranslateRel_I(bone[41], bone[42], bone[43]);
        phd_RotYXZpack_I(
            packed_rotation1[LM_UARM_L], packed_rotation2[LM_UARM_L]);
        Output_DrawPolygons_I(g_Lara.mesh_ptrs[11], clip);

        phd_TranslateRel_I(bone[45], bone[46], bone[47]);
        phd_RotYXZpack_I(
            packed_rotation1[LM_LARM_L], packed_rotation2[LM_LARM_L]);
        Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_LARM_L], clip);

        phd_TranslateRel_I(bone[49], bone[50], bone[51]);
        phd_RotYXZpack_I(
            packed_rotation1[LM_HAND_L], packed_rotation2[LM_HAND_L]);
        Output_DrawPolygons_I(g_Lara.mesh_ptrs[LM_HAND_L], clip);

        phd_PopMatrix_I();
        break;

    case LGT_PISTOLS:
    case LGT_MAGNUMS:
    case LGT_UZIS:
        phd_PushMatrix_I();

        phd_TranslateRel_I(bone[29], bone[30], bone[31]);
        InterpolateArmMatrix();

        packed_rotation1 =
            (int32_t
                 *)(g_Lara.right_arm.frame_base + g_Lara.right_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_RotYXZ(
            g_Lara.right_arm.y_rot, g_Lara.right_arm.x_rot,
            g_Lara.right_arm.z_rot);
        phd_RotYXZpack(packed_rotation1[LM_UARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

        phd_TranslateRel(bone[33], bone[34], bone[35]);
        phd_RotYXZpack(packed_rotation1[LM_LARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

        phd_TranslateRel(bone[37], bone[38], bone[39]);
        phd_RotYXZpack(packed_rotation1[LM_HAND_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

        if (g_Lara.right_arm.flash_gun) {
            saved_matrix = *g_PhdMatrixPtr;
        }

        phd_PopMatrix_I();

        phd_PushMatrix_I();

        phd_TranslateRel_I(bone[41], bone[42], bone[43]);
        InterpolateArmMatrix();

        packed_rotation1 =
            (int32_t
                 *)(g_Lara.left_arm.frame_base + g_Lara.left_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_RotYXZ(
            g_Lara.left_arm.y_rot, g_Lara.left_arm.x_rot,
            g_Lara.left_arm.z_rot);
        phd_RotYXZpack(packed_rotation1[LM_UARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

        phd_TranslateRel(bone[45], bone[46], bone[47]);
        phd_RotYXZpack(packed_rotation1[LM_LARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

        phd_TranslateRel(bone[49], bone[50], bone[51]);
        phd_RotYXZpack(packed_rotation1[LM_HAND_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

        if (g_Lara.left_arm.flash_gun) {
            DrawGunFlash(fire_arms, clip);
        }

        if (g_Lara.right_arm.flash_gun) {
            *g_PhdMatrixPtr = saved_matrix;
            DrawGunFlash(fire_arms, clip);
        }

        phd_PopMatrix_I();
        break;

    case LGT_SHOTGUN:
        phd_PushMatrix_I();
        InterpolateMatrix();

        packed_rotation1 =
            (int32_t
                 *)(g_Lara.right_arm.frame_base + g_Lara.right_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_TranslateRel(bone[29], bone[30], bone[31]);
        phd_RotYXZpack(packed_rotation1[LM_UARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_R], clip);

        phd_TranslateRel(bone[33], bone[34], bone[35]);
        phd_RotYXZpack(packed_rotation1[LM_LARM_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_R], clip);

        phd_TranslateRel(bone[37], bone[38], bone[39]);
        phd_RotYXZpack(packed_rotation1[LM_HAND_R]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_R], clip);

        if (g_Lara.right_arm.flash_gun) {
            saved_matrix = *g_PhdMatrixPtr;
        }

        phd_PopMatrix();

        phd_PushMatrix();

        packed_rotation1 =
            (int32_t
                 *)(g_Lara.left_arm.frame_base + g_Lara.left_arm.frame_number * (object->nmeshes * 2 + FRAME_ROT) + 10);
        phd_TranslateRel(bone[41], bone[42], bone[43]);
        phd_RotYXZpack(packed_rotation1[LM_UARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_UARM_L], clip);

        phd_TranslateRel(bone[45], bone[46], bone[47]);
        phd_RotYXZpack(packed_rotation1[LM_LARM_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_LARM_L], clip);

        phd_TranslateRel(bone[49], bone[50], bone[51]);
        phd_RotYXZpack(packed_rotation1[LM_HAND_L]);
        Output_DrawPolygons(g_Lara.mesh_ptrs[LM_HAND_L], clip);

        if (g_Lara.right_arm.flash_gun) {
            *g_PhdMatrixPtr = saved_matrix;
            DrawGunFlash(fire_arms, clip);
        }

        phd_PopMatrix_I();
        break;
    }

    phd_PopMatrix();
    phd_PopMatrix();
}

/*
int16_t CreateEffect(int16_t room_num)
{
    int16_t fx_num = g_NextFxFree;
    if (fx_num == NO_ITEM) {
        return fx_num;
    }

    FX_INFO *fx = &g_Effects[fx_num];
    g_NextFxFree = fx->next_fx;

    ROOM_INFO *r = &g_RoomInfo[room_num];
    fx->room_number = room_num;
    fx->next_fx = r->fx_number;
    r->fx_number = fx_num;

    fx->next_active = g_NextFxActive;
    g_NextFxActive = fx_num;

    return fx_num;
}
*/
/*
void RemoveActiveItem(int16_t item_num)
{
    if (!g_Items[item_num].active)
	{
        //Shell_ExitSystem("Item already deactive");
    }

    g_Items[item_num].active = 0;

    int16_t linknum = g_NextItemActive;
    if (linknum == item_num) {
        g_NextItemActive = g_Items[item_num].next_active;
        return;
    }

    for (; linknum != NO_ITEM; linknum = g_Items[linknum].next_active) {
        if (g_Items[linknum].next_active == item_num) {
            g_Items[linknum].next_active = g_Items[item_num].next_active;
            break;
        }
    }
}

*/
/*
void InitialiseSlot(int16_t item_num, int32_t slot)
{
    CREATURE_INFO *creature = &m_BaddieSlots[slot];
    ITEM_INFO *item = &g_Items[item_num];
    item->data = creature;
    creature->item_num = item_num;
    creature->mood = MOOD_BORED;
    creature->head_rotation = 0;
    creature->neck_rotation = 0;
    creature->maximum_turn = PHD_DEGREE;
    creature->flags = 0;

    creature->LOT.step = STEP_L;
    creature->LOT.drop = -STEP_L;
    creature->LOT.block_mask = BLOCKED;
    creature->LOT.fly = 0;

    switch (item->object_number) {
    case O_BAT:
    case O_ALLIGATOR:
    case O_FISH:
        creature->LOT.step = WALL_L * 20;
        creature->LOT.drop = -WALL_L * 20;
        creature->LOT.fly = STEP_L / 16;
        break;

    case O_DINOSAUR:
    case O_WARRIOR1:
    case O_CENTAUR:
        creature->LOT.block_mask = BLOCKABLE;
        break;

    case O_WOLF:
    case O_LION:
    case O_LIONESS:
    case O_PUMA:
        creature->LOT.drop = -WALL_L;
        break;

    case O_APE:
        creature->LOT.step = STEP_L * 2;
        creature->LOT.drop = -WALL_L;
        break;
    }

    ClearLOT(&creature->LOT);
    CreateZone(item);

    m_SlotsUsed++;
}
*/
/*
void DisableBaddieAI(int16_t item_num)
{
    ITEM_INFO *item = &g_Items[item_num];
    CREATURE_INFO *creature = (CREATURE_INFO *)item->data;
    item->data = NULL;
    if (creature) {
        creature->item_num = NO_ITEM;
        m_SlotsUsed--;
    }
}
*/
void RemoveRoomFlipItems(ROOM_INFO *r)
{
    for (int16_t item_num = r->item_number; item_num != NO_ITEM;
         item_num = g_Items[item_num].next_item) {
        ITEM_INFO *item = &g_Items[item_num];

        switch (item->object_number) {
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
         item_num = g_Items[item_num].next_item) {
        ITEM_INFO *item = &g_Items[item_num];

        switch (item->object_number) {
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
    if (!frac) {
        return frmptr[0];
    }

    for (int i = 0; i < 6; i++) {
        int16_t a = frmptr[0][i];
        int16_t b = frmptr[1][i];
        m_InterpolatedBounds[i] = a + (((b - a) * frac) / rate);
    }
    return m_InterpolatedBounds;
}

/*
void ClearLOT(LOT_INFO *LOT)
{
    LOT->search_number = 0;
    LOT->head = NO_BOX;
    LOT->tail = NO_BOX;
    LOT->target_box = NO_BOX;
    LOT->required_box = NO_BOX;

    for (int i = 0; i < g_NumberBoxes; i++) {
        BOX_NODE *node = &LOT->node[i];
        node->search_number = 0;
        node->exit_box = NO_BOX;
        node->next_expansion = NO_BOX;
    }
}
*/
/*
void CreateZone(ITEM_INFO *item)
{
    CREATURE_INFO *creature = (CREATURE_INFO *)item->data;

    int16_t *zone;
    int16_t *flip;
    if (creature->LOT.fly)
	{
        zone = g_FlyZone[0];
        flip = g_FlyZone[1];
    }
	else if (creature->LOT.step == STEP_L)
	{
        zone = g_GroundZone[0];
        flip = g_GroundZone[1];
    }
	else
	{
        zone = g_GroundZone2[1];
        flip = g_GroundZone2[1];
    }

    ROOM_INFO *r = &g_RoomInfo[item->room_number];
    int32_t x_floor = (item->pos.z - r->z) >> WALL_SHIFT;
    int32_t y_floor = (item->pos.x - r->x) >> WALL_SHIFT;
    item->box_number = r->floor[x_floor + y_floor * r->x_size].box;

    int16_t zone_number = zone[item->box_number];
    int16_t flip_number = flip[item->box_number];

    creature->LOT.zone_count = 0;
    BOX_NODE *node = creature->LOT.node;
    for (int i = 0; i < g_NumberBoxes; i++) {
        if (zone[i] == zone_number || flip[i] == flip_number) {
            node->box_number = i;
            node++;
            creature->LOT.zone_count++;
        }
    }
}
*/
/*
void AlterFloorHeight(ITEM_INFO *item, int32_t height)
{
    int16_t room_num = item->room_number;
    FLOOR_INFO *floor =
        GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_num);
    FLOOR_INFO *ceiling = GetFloor(
        item->pos.x, item->pos.y + height - WALL_L, item->pos.z, &room_num);

    if (floor->floor == NO_HEIGHT / 256) {
        floor->floor = ceiling->ceiling + height / 256;
    } else {
        floor->floor += height / 256;
        if (floor->floor == ceiling->ceiling) {
            floor->floor = NO_HEIGHT / 256;
        }
    }

    if (g_Boxes[floor->box].overlap_index & BLOCKABLE)
	{
        if (height < 0)
		{
            g_Boxes[floor->box].overlap_index |= BLOCKED;
        }
		else
		{
            g_Boxes[floor->box].overlap_index &= ~BLOCKED;
        }
    }
}
*/

/*
void CreatureCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
    ITEM_INFO *item = &g_Items[item_num];

    if (!TestBoundsCollide(item, lara_item, coll->radius))
	{
        return;
    }
    if (!TestCollision(item, lara_item))
	{
        return;
    }

    if (coll->enable_baddie_push)
	{
        if (item->hit_points <= 0)
		{
            ItemPushLara(item, lara_item, coll, 0, 0);
        }
		else
		{
            ItemPushLara(item, lara_item, coll, coll->enable_spaz, 0);
        }
    }
}
*/
/*
int32_t TestBoundsCollide(ITEM_INFO *item, ITEM_INFO *lara_item, int32_t radius)
{
    int16_t *bounds = GetBestFrame(item);
    int16_t *larabounds = GetBestFrame(lara_item);
    if (item->pos.y + bounds[FRAME_BOUND_MAX_Y]
            <= lara_item->pos.y + larabounds[FRAME_BOUND_MIN_Y]
        || item->pos.y + bounds[FRAME_BOUND_MIN_Y]
            >= lara_item->pos.y + larabounds[FRAME_BOUND_MAX_Y]) {
        return 0;
    }

    int32_t c = phd_cos(item->pos.y_rot);
    int32_t s = phd_sin(item->pos.y_rot);
    int32_t x = lara_item->pos.x - item->pos.x;
    int32_t z = lara_item->pos.z - item->pos.z;
    int32_t rx = (c * x - s * z) >> W2V_SHIFT;
    int32_t rz = (c * z + s * x) >> W2V_SHIFT;
    int32_t minx = bounds[FRAME_BOUND_MIN_X] - radius;
    int32_t maxx = bounds[FRAME_BOUND_MAX_X] + radius;
    int32_t minz = bounds[FRAME_BOUND_MIN_Z] - radius;
    int32_t maxz = bounds[FRAME_BOUND_MAX_Z] + radius;
    if (rx >= minx && rx <= maxx && rz >= minz && rz <= maxz) {
        return 1;
    }

    return 0;
}
*/
/*
int32_t TestCollision(ITEM_INFO *item, ITEM_INFO *lara_item)
{
    SPHERE slist_baddie[34];
    SPHERE slist_lara[34];

    uint32_t flags = 0;
    int32_t num1 = GetSpheres(item, slist_baddie, 1);
    int32_t num2 = GetSpheres(lara_item, slist_lara, 1);

    for (int i = 0; i < num1; i++) {
        SPHERE *ptr1 = &slist_baddie[i];
        if (ptr1->r <= 0) {
            continue;
        }
        for (int j = 0; j < num2; j++) {
            SPHERE *ptr2 = &slist_lara[j];
            if (ptr2->r <= 0) {
                continue;
            }
            int32_t x = ptr2->x - ptr1->x;
            int32_t y = ptr2->y - ptr1->y;
            int32_t z = ptr2->z - ptr1->z;
            int32_t r = ptr2->r + ptr1->r;
            int32_t d = SQUARE(x) + SQUARE(y) + SQUARE(z);
            int32_t r2 = SQUARE(r);
            if (d < r2) {
                flags |= 1 << i;
                break;
            }
        }
    }

    item->touch_bits = flags;
    return flags;
}
*/
/*
void ItemPushLara(ITEM_INFO *item, ITEM_INFO *lara_item, COLL_INFO *coll, int32_t spazon, int32_t bigpush)
{
    int32_t x = lara_item->pos.x - item->pos.x;
    int32_t z = lara_item->pos.z - item->pos.z;
    int32_t c = phd_cos(item->pos.y_rot);
    int32_t s = phd_sin(item->pos.y_rot);
    int32_t rx = (c * x - s * z) >> W2V_SHIFT;
    int32_t rz = (c * z + s * x) >> W2V_SHIFT;

    int16_t *bounds = GetBestFrame(item);
    int32_t minx = bounds[FRAME_BOUND_MIN_X];
    int32_t maxx = bounds[FRAME_BOUND_MAX_X];
    int32_t minz = bounds[FRAME_BOUND_MIN_Z];
    int32_t maxz = bounds[FRAME_BOUND_MAX_Z];

    if (bigpush) {
        minx -= coll->radius;
        maxx += coll->radius;
        minz -= coll->radius;
        maxz += coll->radius;
    }

    if (rx >= minx && rx <= maxx && rz >= minz && rz <= maxz) {
        int32_t l = rx - minx;
        int32_t r = maxx - rx;
        int32_t t = maxz - rz;
        int32_t b = rz - minz;

        if (l <= r && l <= t && l <= b) {
            rx -= l;
        } else if (r <= l && r <= t && r <= b) {
            rx += r;
        } else if (t <= l && t <= r && t <= b) {
            rz += t;
        } else {
            rz -= b;
        }

        int32_t ax = (c * rx + s * rz) >> W2V_SHIFT;
        int32_t az = (c * rz - s * rx) >> W2V_SHIFT;

        lara_item->pos.x = item->pos.x + ax;
        lara_item->pos.z = item->pos.z + az;

        rx = (bounds[FRAME_BOUND_MIN_X] + bounds[FRAME_BOUND_MAX_X]) / 2;
        rz = (bounds[FRAME_BOUND_MIN_Z] + bounds[FRAME_BOUND_MAX_Z]) / 2;
        x -= (c * rx + s * rz) >> W2V_SHIFT;
        z -= (c * rz - s * rx) >> W2V_SHIFT;

        if (spazon) {
            PHD_ANGLE hitang =
                lara_item->pos.y_rot - (PHD_180 + phd_atan(z, x));
            g_Lara.hit_direction = (hitang + PHD_45) / PHD_90;
            if (!g_Lara.hit_frame)
			{
                Sound_Effect(SFX_LARA_BODYSL, &lara_item->pos, SPM_NORMAL);
            }

            g_Lara.hit_frame++;
            if (g_Lara.hit_frame > 34) {
                g_Lara.hit_frame = 34;
            }
        }

        coll->bad_pos = NO_BAD_POS;
        coll->bad_neg = -STEPUP_HEIGHT;
        coll->bad_ceiling = 0;

        int16_t old_facing = coll->facing;
        coll->facing = phd_atan(
            lara_item->pos.z - coll->old.z, lara_item->pos.x - coll->old.x);
        GetCollisionInfo(
            coll, lara_item->pos.x, lara_item->pos.y, lara_item->pos.z,
            lara_item->room_number, LARA_HITE);
        coll->facing = old_facing;

        if (coll->coll_type != COLL_NONE)
		{
            lara_item->pos.x = coll->old.x;
            lara_item->pos.z = coll->old.z;
        }
		else
		{
            coll->old.x = lara_item->pos.x;
            coll->old.y = lara_item->pos.y;
            coll->old.z = lara_item->pos.z;
            
			UpdateLaraRoom(lara_item, -10);
        }
    }
}
*/
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

/*

int32_t GetSpheres(ITEM_INFO *item, SPHERE *ptr, int32_t world_space)
{
    static int16_t null_rotation[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    if (!item) {
        return 0;
    }

    int32_t x;
    int32_t y;
    int32_t z;
    if (world_space) {
        x = item->pos.x;
        y = item->pos.y;
        z = item->pos.z;
        phd_PushUnitMatrix();
        g_PhdMatrixPtr->_03 = 0;
        g_PhdMatrixPtr->_13 = 0;
        g_PhdMatrixPtr->_23 = 0;
    } else {
        x = 0;
        y = 0;
        z = 0;
        phd_PushMatrix();
        phd_TranslateAbs(item->pos.x, item->pos.y, item->pos.z);
    }

    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

    int16_t *frame = GetBestFrame(item);
    phd_TranslateRel(
        frame[FRAME_POS_X], frame[FRAME_POS_Y], frame[FRAME_POS_Z]);

    int32_t *packed_rotation = (int32_t *)(frame + FRAME_ROT);
    phd_RotYXZpack(*packed_rotation++);

    OBJECT_INFO *object = &g_Objects[item->object_number];
    int16_t **meshpp = &g_Meshes[object->mesh_index];
    int32_t *bone = &g_AnimBones[object->bone_index];

    int16_t *objptr = *meshpp++;
    phd_PushMatrix();
    phd_TranslateRel(objptr[0], objptr[1], objptr[2]);
    ptr->x = x + (g_PhdMatrixPtr->_03 >> W2V_SHIFT);
    ptr->y = y + (g_PhdMatrixPtr->_13 >> W2V_SHIFT);
    ptr->z = z + (g_PhdMatrixPtr->_23 >> W2V_SHIFT);
    ptr->r = objptr[3];
    ptr++;
    phd_PopMatrix();

    int16_t *extra_rotation = (int16_t *)(item->data ? item->data : &null_rotation);
    for (int i = 1; i < object->nmeshes; i++)
	{
        int32_t bone_extra_flags = bone[0];
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

        if (bone_extra_flags & BEB_ROT_Y) {
            phd_RotY(*extra_rotation++);
        }
        if (bone_extra_flags & BEB_ROT_X) {
            phd_RotX(*extra_rotation++);
        }
        if (bone_extra_flags & BEB_ROT_Z) {
            phd_RotZ(*extra_rotation++);
        }

        objptr = *meshpp++;
        phd_PushMatrix();
        phd_TranslateRel(objptr[0], objptr[1], objptr[2]);
        ptr->x = x + (g_PhdMatrixPtr->_03 >> W2V_SHIFT);
        ptr->y = y + (g_PhdMatrixPtr->_13 >> W2V_SHIFT);
        ptr->z = z + (g_PhdMatrixPtr->_23 >> W2V_SHIFT);
        ptr->r = objptr[3];
        phd_PopMatrix();

        ptr++;
        bone += 4;
    }

    phd_PopMatrix();
    return object->nmeshes;
}
*/
/*
void GetCollisionInfo(COLL_INFO *coll, int32_t xpos, int32_t ypos, int32_t zpos, int16_t room_num, int32_t objheight)
{
    coll->coll_type = COLL_NONE;
    coll->shift.x = 0;
    coll->shift.y = 0;
    coll->shift.z = 0;
    coll->quadrant = (uint16_t)(coll->facing + PHD_45) / PHD_90;

    int32_t x = xpos;
    int32_t y = ypos - objheight;
    int32_t z = zpos;
    int32_t ytop = y - 160;

    FLOOR_INFO *floor = GetFloor(x, ytop, z, &room_num);
    int32_t height = GetHeight(floor, x, ytop, z);
    if (height != NO_HEIGHT) {
        height -= ypos;
    }

    int32_t ceiling = GetCeiling(floor, x, ytop, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->mid_floor = height;
    coll->mid_ceiling = ceiling;
    coll->mid_type = g_HeightType;
    coll->trigger = g_TriggerIndex;

    int16_t tilt = GetTiltType(floor, x, g_LaraItem->pos.y, z);
    coll->tilt_z = tilt >> 8;
    coll->tilt_x = (int8_t)tilt;

    int32_t xleft;
    int32_t zleft;
    int32_t xright;
    int32_t zright;
    int32_t xfront;
    int32_t zfront;
    switch (coll->quadrant) {
    case DIR_NORTH:
        xfront = (phd_sin(coll->facing) * coll->radius) >> W2V_SHIFT;
        zfront = coll->radius;
        xleft = -coll->radius;
        zleft = coll->radius;
        xright = coll->radius;
        zright = coll->radius;
        break;

    case DIR_EAST:
        xfront = coll->radius;
        zfront = (phd_cos(coll->facing) * coll->radius) >> W2V_SHIFT;
        xleft = coll->radius;
        zleft = coll->radius;
        xright = coll->radius;
        zright = -coll->radius;
        break;

    case DIR_SOUTH:
        xfront = (phd_sin(coll->facing) * coll->radius) >> W2V_SHIFT;
        zfront = -coll->radius;
        xleft = coll->radius;
        zleft = -coll->radius;
        xright = -coll->radius;
        zright = -coll->radius;
        break;

    case DIR_WEST:
        xfront = -coll->radius;
        zfront = (phd_cos(coll->facing) * coll->radius) >> W2V_SHIFT;
        xleft = -coll->radius;
        zleft = -coll->radius;
        xright = -coll->radius;
        zright = coll->radius;
        break;

    default:
        xfront = 0;
        zfront = 0;
        xleft = 0;
        zleft = 0;
        xright = 0;
        zright = 0;
        break;
    }

    x = xpos + xfront;
    z = zpos + zfront;
    floor = GetFloor(x, ytop, z, &room_num);
    height = GetHeight(floor, x, ytop, z);
    if (height != NO_HEIGHT) {
        height -= ypos;
    }

    ceiling = GetCeiling(floor, x, ytop, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->front_floor = height;
    coll->front_ceiling = ceiling;
    coll->front_type = g_HeightType;
    if (coll->slopes_are_walls && coll->front_type == HT_BIG_SLOPE
        && coll->front_floor < 0) {
        coll->front_floor = -32767;
    } else if (
        coll->slopes_are_pits && coll->front_type == HT_BIG_SLOPE
        && coll->front_floor > 0) {
        coll->front_floor = 512;
    } else if (
        coll->lava_is_pit && coll->front_floor > 0 && g_TriggerIndex
        && (g_TriggerIndex[0] & DATA_TYPE) == FT_LAVA) {
        coll->front_floor = 512;
    }

    x = xpos + xleft;
    z = zpos + zleft;
    floor = GetFloor(x, ytop, z, &room_num);
    height = GetHeight(floor, x, ytop, z);
    if (height != NO_HEIGHT) {
        height -= ypos;
    }

    ceiling = GetCeiling(floor, x, ytop, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->left_floor = height;
    coll->left_ceiling = ceiling;
    coll->left_type = g_HeightType;
    if (coll->slopes_are_walls && coll->left_type == HT_BIG_SLOPE
        && coll->left_floor < 0) {
        coll->left_floor = -32767;
    } else if (
        coll->slopes_are_pits && coll->left_type == HT_BIG_SLOPE
        && coll->left_floor > 0) {
        coll->left_floor = 512;
    } else if (
        coll->lava_is_pit && coll->left_floor > 0 && g_TriggerIndex
        && (g_TriggerIndex[0] & DATA_TYPE) == FT_LAVA) {
        coll->left_floor = 512;
    }

    x = xpos + xright;
    z = zpos + zright;
    floor = GetFloor(x, ytop, z, &room_num);
    height = GetHeight(floor, x, ytop, z);
    if (height != NO_HEIGHT) {
        height -= ypos;
    }

    ceiling = GetCeiling(floor, x, ytop, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->right_floor = height;
    coll->right_ceiling = ceiling;
    coll->right_type = g_HeightType;
    if (coll->slopes_are_walls && coll->right_type == HT_BIG_SLOPE
        && coll->right_floor < 0) {
        coll->right_floor = -32767;
    } else if (
        coll->slopes_are_pits && coll->right_type == HT_BIG_SLOPE
        && coll->right_floor > 0) {
        coll->right_floor = 512;
    } else if (
        coll->lava_is_pit && coll->right_floor > 0 && g_TriggerIndex
        && (g_TriggerIndex[0] & DATA_TYPE) == FT_LAVA) {
        coll->right_floor = 512;
    }

    CollideStaticObjects(coll, xpos, ypos, zpos, room_num, objheight);

    if (coll->mid_floor == NO_HEIGHT) {
        coll->shift.x = coll->old.x - xpos;
        coll->shift.y = coll->old.y - ypos;
        coll->shift.z = coll->old.z - zpos;
        coll->coll_type = COLL_FRONT;
        return;
    }

    if (coll->mid_floor - coll->mid_ceiling <= 0) {
        coll->shift.x = coll->old.x - xpos;
        coll->shift.y = coll->old.y - ypos;
        coll->shift.z = coll->old.z - zpos;
        coll->coll_type = COLL_CLAMP;
        return;
    }

    if (coll->mid_ceiling >= 0) {
        coll->shift.y = coll->mid_ceiling;
        coll->coll_type = COLL_TOP;
    }

    if (coll->front_floor > coll->bad_pos || coll->front_floor < coll->bad_neg
        || coll->front_ceiling > coll->bad_ceiling) {
        switch (coll->quadrant) {
        case DIR_NORTH:
        case DIR_SOUTH:
            coll->shift.x = coll->old.x - xpos;
            coll->shift.z = FindGridShift(zpos + zfront, zpos);
            break;

        case DIR_EAST:
        case DIR_WEST:
            coll->shift.x = FindGridShift(xpos + xfront, xpos);
            coll->shift.z = coll->old.z - zpos;
            break;
        }

        coll->coll_type = COLL_FRONT;
        return;
    }

    if (coll->front_ceiling >= coll->bad_ceiling) {
        coll->shift.x = coll->old.x - xpos;
        coll->shift.y = coll->old.y - ypos;
        coll->shift.z = coll->old.z - zpos;
        coll->coll_type = COLL_TOPFRONT;
        return;
    }

    if (coll->left_floor > coll->bad_pos || coll->left_floor < coll->bad_neg) {
        switch (coll->quadrant) {
        case DIR_NORTH:
        case DIR_SOUTH:
            coll->shift.x = FindGridShift(xpos + xleft, xpos + xfront);
            break;

        case DIR_EAST:
        case DIR_WEST:
            coll->shift.z = FindGridShift(zpos + zleft, zpos + zfront);
            break;
        }

        coll->coll_type = COLL_LEFT;
        return;
    }

    if (coll->right_floor > coll->bad_pos
        || coll->right_floor < coll->bad_neg) {
        switch (coll->quadrant) {
        case DIR_NORTH:
        case DIR_SOUTH:
            coll->shift.x = FindGridShift(xpos + xright, xpos + xfront);
            break;

        case DIR_EAST:
        case DIR_WEST:
            coll->shift.z = FindGridShift(zpos + zright, zpos + zfront);
            break;
        }

        coll->coll_type = COLL_RIGHT;
        return;
    }
}
*/
/*
void UpdateLaraRoom(ITEM_INFO *item, int32_t height)
{
    int32_t x = item->pos.x;
    int32_t y = item->pos.y + height;
    int32_t z = item->pos.z;
    int16_t room_num = item->room_number;
    FLOOR_INFO *floor = GetFloor(x, y, z, &room_num);
    item->floor = GetHeight(floor, x, y, z);
    if (item->room_number != room_num) {
        ItemNewRoom(g_Lara.item_number, room_num);
    }
}

*/
/*
int16_t GetCeiling(FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z)
{
    int16_t *data;
    int16_t type;
    int16_t trigger;

    FLOOR_INFO *f = floor;
    while (f->sky_room != NO_ROOM) {
        ROOM_INFO *r = &g_RoomInfo[f->sky_room];
        int32_t x_floor = (z - r->z) >> WALL_SHIFT;
        int32_t y_floor = (x - r->x) >> WALL_SHIFT;
        f = &r->floor[x_floor + y_floor * r->x_size];
    }

    int16_t height = f->ceiling << 8;

    if (f->index) {
        data = &g_FloorData[f->index];
        type = *data++ & DATA_TYPE;

        if (type == FT_TILT) {
            data++;
            type = *data++ & DATA_TYPE;
        }

        if (type == FT_ROOF) {
            int32_t xoff = data[0] >> 8;
            int32_t yoff = (int8_t)data[0];

            if (!g_ChunkyFlag
                || (xoff >= -2 && xoff <= 2 && yoff >= -2 && yoff <= 2)) {
                if (xoff < 0) {
                    height += (int16_t)((xoff * (z & (WALL_L - 1))) >> 2);
                } else {
                    height -=
                        (int16_t)((xoff * ((WALL_L - 1 - z) & (WALL_L - 1))) >> 2);
                }

                if (yoff < 0) {
                    height +=
                        (int16_t)((yoff * ((WALL_L - 1 - x) & (WALL_L - 1))) >> 2);
                } else {
                    height -= (int16_t)((yoff * (x & (WALL_L - 1))) >> 2);
                }
            }
        }
    }

    while (floor->pit_room != NO_ROOM) {
        ROOM_INFO *r = &g_RoomInfo[floor->pit_room];
        int32_t x_floor = (z - r->z) >> WALL_SHIFT;
        int32_t y_floor = (x - r->x) >> WALL_SHIFT;
        floor = &r->floor[x_floor + y_floor * r->x_size];
    }

    if (!floor->index) {
        return height;
    }

    data = &g_FloorData[floor->index];
    do {
        type = *data++;

        switch (type & DATA_TYPE) {
        case FT_DOOR:
        case FT_TILT:
        case FT_ROOF:
            data++;
            break;

        case FT_LAVA:
            break;

        case FT_TRIGGER:
            data++;
            do {
                trigger = *data++;
                if (TRIG_BITS(trigger) != TO_OBJECT) {
                    if (TRIG_BITS(trigger) == TO_CAMERA) {
                        trigger = *data++;
                    }
                } else {
                    ITEM_INFO *item = &g_Items[trigger & VALUE_BITS];
                    OBJECT_INFO *object = &g_Objects[item->object_number];
                    if (object->ceiling) {
                        object->ceiling(item, x, y, z, &height);
                    }
                }
            } while (!(trigger & END_BIT));
            break;

        default:
            //Shell_ExitSystem("GetCeiling(): Unknown type");
            break;
        }
    } while (!(type & END_BIT));

    return height;
}
*/

/*
int16_t GetTiltType(FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z)
{
    ROOM_INFO *r;

    while (floor->pit_room != NO_ROOM) {
        r = &g_RoomInfo[floor->pit_room];
        floor = &r->floor
                     [((z - r->z) >> WALL_SHIFT)
                      + ((x - r->x) >> WALL_SHIFT) * r->x_size];
    }

    if (y + 512 < ((int32_t)floor->floor << 8)) {
        return 0;
    }

    if (floor->index) {
        int16_t *data = &g_FloorData[floor->index];
        if ((data[0] & DATA_TYPE) == FT_TILT) {
            return data[1];
        }
    }

    return 0;
}
*/

/*
int32_t FindGridShift(int32_t src, int32_t dst)
{
    int32_t srcw = src >> WALL_SHIFT;
    int32_t dstw = dst >> WALL_SHIFT;
    if (srcw == dstw) {
        return 0;
    }

    src &= WALL_L - 1;
    if (dstw > srcw) {
        return WALL_L - (src - 1);
    } else {
        return -(src + 1);
    }
}
*/

/*
int32_t CollideStaticObjects(
    COLL_INFO *coll, int32_t x, int32_t y, int32_t z, int16_t room_number,
    int32_t hite)
{
    PHD_VECTOR shifter;

    coll->hit_static = 0;
    int32_t inxmin = x - coll->radius;
    int32_t inxmax = x + coll->radius;
    int32_t inymin = y - hite;
    int32_t inymax = y;
    int32_t inzmin = z - coll->radius;
    int32_t inzmax = z + coll->radius;

    shifter.x = 0;
    shifter.y = 0;
    shifter.z = 0;

    GetNearByRooms(x, y, z, coll->radius + 50, hite + 50, room_number);

    for (int i = 0; i < g_RoomsToDrawCount; i++)
	{
        int16_t room_num = g_RoomsToDraw[i];
        ROOM_INFO *r = &g_RoomInfo[room_num];
        MESH_INFO *mesh = r->static_mesh;

        for (int j = 0; j < r->num_static_meshes; j++, mesh++)
		{
            STATIC_INFO *sinfo = &g_StaticObjects[mesh->static_number];
            if (sinfo->flags & 1)
			{
                continue;
            }

            int32_t ymin = mesh->y + sinfo->y_minc;
            int32_t ymax = mesh->y + sinfo->y_maxc;
            int32_t xmin;
            int32_t xmax;
            int32_t zmin;
            int32_t zmax;
            switch (mesh->y_rot) {
            case PHD_90:
                xmin = mesh->x + sinfo->z_minc;
                xmax = mesh->x + sinfo->z_maxc;
                zmin = mesh->z - sinfo->x_maxc;
                zmax = mesh->z - sinfo->x_minc;
                break;

            case -PHD_180:
                xmin = mesh->x - sinfo->x_maxc;
                xmax = mesh->x - sinfo->x_minc;
                zmin = mesh->z - sinfo->z_maxc;
                zmax = mesh->z - sinfo->z_minc;
                break;

            case -PHD_90:
                xmin = mesh->x - sinfo->z_maxc;
                xmax = mesh->x - sinfo->z_minc;
                zmin = mesh->z + sinfo->x_minc;
                zmax = mesh->z + sinfo->x_maxc;
                break;

            default:
                xmin = mesh->x + sinfo->x_minc;
                xmax = mesh->x + sinfo->x_maxc;
                zmin = mesh->z + sinfo->z_minc;
                zmax = mesh->z + sinfo->z_maxc;
                break;
            }

            if (inxmax <= xmin || inxmin >= xmax || inymax <= ymin
                || inymin >= ymax || inzmax <= zmin || inzmin >= zmax) {
                continue;
            }

            int32_t shl = inxmax - xmin;
            int32_t shr = xmax - inxmin;
            if (shl < shr) {
                shifter.x = -shl;
            } else {
                shifter.x = shr;
            }

            shl = inzmax - zmin;
            shr = zmax - inzmin;
            if (shl < shr) {
                shifter.z = -shl;
            } else {
                shifter.z = shr;
            }

            switch (coll->quadrant) {
            case DIR_NORTH:
                if (shifter.x > coll->radius || shifter.x < -coll->radius) {
                    coll->shift.z = shifter.z;
                    coll->shift.x = coll->old.x - x;
                    coll->coll_type = COLL_FRONT;
                } else if (shifter.x > 0) {
                    coll->shift.x = shifter.x;
                    coll->shift.z = 0;
                    coll->coll_type = COLL_LEFT;
                } else if (shifter.x < 0) {
                    coll->shift.x = shifter.x;
                    coll->shift.z = 0;
                    coll->coll_type = COLL_RIGHT;
                }
                break;

            case DIR_EAST:
                if (shifter.z > coll->radius || shifter.z < -coll->radius) {
                    coll->shift.x = shifter.x;
                    coll->shift.z = coll->old.z - z;
                    coll->coll_type = COLL_FRONT;
                } else if (shifter.z > 0) {
                    coll->shift.z = shifter.z;
                    coll->shift.x = 0;
                    coll->coll_type = COLL_RIGHT;
                } else if (shifter.z < 0) {
                    coll->shift.z = shifter.z;
                    coll->shift.x = 0;
                    coll->coll_type = COLL_LEFT;
                }
                break;

            case DIR_SOUTH:
                if (shifter.x > coll->radius || shifter.x < -coll->radius) {
                    coll->shift.z = shifter.z;
                    coll->shift.x = coll->old.x - x;
                    coll->coll_type = COLL_FRONT;
                } else if (shifter.x > 0) {
                    coll->shift.x = shifter.x;
                    coll->shift.z = 0;
                    coll->coll_type = COLL_RIGHT;
                } else if (shifter.x < 0) {
                    coll->shift.x = shifter.x;
                    coll->shift.z = 0;
                    coll->coll_type = COLL_LEFT;
                }
                break;

            case DIR_WEST:
                if (shifter.z > coll->radius || shifter.z < -coll->radius) {
                    coll->shift.x = shifter.x;
                    coll->shift.z = coll->old.z - z;
                    coll->coll_type = COLL_FRONT;
                } else if (shifter.z > 0) {
                    coll->shift.z = shifter.z;
                    coll->shift.x = 0;
                    coll->coll_type = COLL_LEFT;
                } else if (shifter.z < 0) {
                    coll->shift.z = shifter.z;
                    coll->shift.x = 0;
                    coll->coll_type = COLL_RIGHT;
                }
                break;
            }

            coll->hit_static = 1;
            return 1;
        }
    }

    return 0;
}
*/

/*
void GetNearByRooms(
    int32_t x, int32_t y, int32_t z, int32_t r, int32_t h, int16_t room_num)
{
    g_RoomsToDrawCount = 0;
    if (g_RoomsToDrawCount + 1 < MAX_ROOMS_TO_DRAW) {
        g_RoomsToDraw[g_RoomsToDrawCount++] = room_num;
    }
    GetNewRoom(x + r, y, z + r, room_num);
    GetNewRoom(x - r, y, z + r, room_num);
    GetNewRoom(x + r, y, z - r, room_num);
    GetNewRoom(x - r, y, z - r, room_num);
    GetNewRoom(x + r, y - h, z + r, room_num);
    GetNewRoom(x - r, y - h, z + r, room_num);
    GetNewRoom(x + r, y - h, z - r, room_num);
    GetNewRoom(x - r, y - h, z - r, room_num);
}

void GetNewRoom(int32_t x, int32_t y, int32_t z, int16_t room_num)
{
    GetFloor(x, y, z, &room_num);

    for (int i = 0; i < g_RoomsToDrawCount; i++) {
        int16_t drawn_room = g_RoomsToDraw[i];
        if (drawn_room == room_num) {
            return;
        }
    }

    if (g_RoomsToDrawCount + 1 < MAX_ROOMS_TO_DRAW) {
        g_RoomsToDraw[g_RoomsToDrawCount++] = room_num;
    }
}
*/
/*
int16_t GetWaterHeight(int32_t x, int32_t y, int32_t z, int16_t room_num)
{
    ROOM_INFO *r = &g_RoomInfo[room_num];

    int16_t data;
    FLOOR_INFO *floor;
    int32_t x_floor, y_floor;

    do {
        x_floor = (z - r->z) >> WALL_SHIFT;
        y_floor = (x - r->x) >> WALL_SHIFT;

        if (x_floor <= 0) {
            x_floor = 0;
            if (y_floor < 1) {
                y_floor = 1;
            } else if (y_floor > r->y_size - 2) {
                y_floor = r->y_size - 2;
            }
        } else if (x_floor >= r->x_size - 1) {
            x_floor = r->x_size - 1;
            if (y_floor < 1) {
                y_floor = 1;
            } else if (y_floor > r->y_size - 2) {
                y_floor = r->y_size - 2;
            }
        } else if (y_floor < 0) {
            y_floor = 0;
        } else if (y_floor >= r->y_size) {
            y_floor = r->y_size - 1;
        }

        floor = &r->floor[x_floor + y_floor * r->x_size];
        data = GetDoor(floor);
        if (data != NO_ROOM) {
            r = &g_RoomInfo[data];
        }
    } while (data != NO_ROOM);

    if (r->flags & RF_UNDERWATER) {
        while (floor->sky_room != NO_ROOM) {
            r = &g_RoomInfo[floor->sky_room];
            if (!(r->flags & RF_UNDERWATER)) {
                break;
            }
            x_floor = (z - r->z) >> WALL_SHIFT;
            y_floor = (x - r->x) >> WALL_SHIFT;
            floor = &r->floor[x_floor + y_floor * r->x_size];
        }
        return floor->ceiling << 8;
    } else {
        while (floor->pit_room != NO_ROOM) {
            r = &g_RoomInfo[floor->pit_room];
            if (r->flags & RF_UNDERWATER) {
                return floor->floor << 8;
            }
            x_floor = (z - r->z) >> WALL_SHIFT;
            y_floor = (x - r->x) >> WALL_SHIFT;
            floor = &r->floor[x_floor + y_floor * r->x_size];
        }
        return NO_HEIGHT;
    }
}
*/
int16_t *CalcRoomVertices(int16_t *obj_ptr)
{

    int32_t vertex_count = *obj_ptr++;

    for (int i = 0; i < vertex_count; i++)
	{
        int32_t xv = g_PhdMatrixPtr->_00 * obj_ptr[0]
            + g_PhdMatrixPtr->_01 * obj_ptr[1]
            + g_PhdMatrixPtr->_02 * obj_ptr[2] + g_PhdMatrixPtr->_03;

			
		int32_t yv = g_PhdMatrixPtr->_10 * obj_ptr[0]
            + g_PhdMatrixPtr->_11 * obj_ptr[1]
            + g_PhdMatrixPtr->_12 * obj_ptr[2] + g_PhdMatrixPtr->_13;
        
		int32_t zv = g_PhdMatrixPtr->_20 * obj_ptr[0]
            + g_PhdMatrixPtr->_21 * obj_ptr[1]
            + g_PhdMatrixPtr->_22 * obj_ptr[2] + g_PhdMatrixPtr->_23;

		m_VBuf[i].xv = xv;
        m_VBuf[i].yv = yv;
        m_VBuf[i].zv = zv;
        m_VBuf[i].g = obj_ptr[3];

        if (zv < Z_NEAR)
		//if (zv < ZNear)
		{
            m_VBuf[i].clip = (int16_t)0x8000;
        }
		else
		{
            int16_t clip_flags = 0;
            int32_t depth = zv >> W2V_SHIFT;

            if (depth > DRAW_DIST_FADE)
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

				//m_VBuf[i].g = 0x1FFF;
                m_VBuf[i].g += g_ShadeTable[(((uint8_t)g_WibbleOffset + (uint8_t)g_RandTable[(vertex_count - i) % WIBBLE_SIZE])	% WIBBLE_SIZE)];

                CLAMP(m_VBuf[i].g, 0, 0x1FFF);
            }

            m_VBuf[i].xs = xs;
            m_VBuf[i].ys = ys;
            m_VBuf[i].clip = clip_flags;
        }
        obj_ptr += 4;
    }

    return obj_ptr;
}

/*
int32_t UpdateLOT(LOT_INFO *LOT, int32_t expansion)
{
    if (LOT->required_box != NO_BOX && LOT->required_box != LOT->target_box) {
        LOT->target_box = LOT->required_box;

        BOX_NODE *expand = &LOT->node[LOT->target_box];
        if (expand->next_expansion == NO_BOX && LOT->tail != LOT->target_box) {
            expand->next_expansion = LOT->head;

            if (LOT->head == NO_BOX) {
                LOT->tail = LOT->target_box;
            }

            LOT->head = LOT->target_box;
        }

        expand->search_number = ++LOT->search_number;
        expand->exit_box = NO_BOX;
    }

    return SearchLOT(LOT, expansion);
}
*/
/*
int32_t SearchLOT(LOT_INFO *LOT, int32_t expansion)
{
    int16_t *zone;
    if (LOT->fly) {
        zone = g_FlyZone[g_FlipStatus];
    } else if (LOT->step == STEP_L) {
        zone = g_GroundZone[g_FlipStatus];
    } else {
        zone = g_GroundZone2[g_FlipStatus];
    }

    int16_t search_zone = zone[LOT->head];
    for (int i = 0; i < expansion; i++) {
        if (LOT->head == NO_BOX) {
            return 0;
        }

        BOX_NODE *node = &LOT->node[LOT->head];
        BOX_INFO *box = &g_Boxes[LOT->head];

        int done = 0;
        int index = box->overlap_index & OVERLAP_INDEX;
        do {
            int16_t box_number = g_Overlap[index++];
            if (box_number & END_BIT) {
                done = 1;
                box_number &= BOX_NUMBER;
            }

            if (search_zone != zone[box_number]) {
                continue;
            }

            int change = g_Boxes[box_number].height - box->height;
            if (change > LOT->step || change < LOT->drop) {
                continue;
            }

            BOX_NODE *expand = &LOT->node[box_number];
            if ((node->search_number & SEARCH_NUMBER)
                < (expand->search_number & SEARCH_NUMBER)) {
                continue;
            }

            if (node->search_number & BLOCKED_SEARCH) {
                if ((node->search_number & SEARCH_NUMBER)
                    == (expand->search_number & SEARCH_NUMBER)) {
                    continue;
                }
                expand->search_number = node->search_number;
            } else {
                if ((node->search_number & SEARCH_NUMBER)
                        == (expand->search_number & SEARCH_NUMBER)
                    && !(expand->search_number & BLOCKED_SEARCH)) {
                    continue;
                }

                if (g_Boxes[box_number].overlap_index & LOT->block_mask) {
                    expand->search_number =
                        node->search_number | BLOCKED_SEARCH;
                } else {
                    expand->search_number = node->search_number;
                    expand->exit_box = LOT->head;
                }
            }

            if (expand->next_expansion == NO_BOX && box_number != LOT->tail) {
                LOT->node[LOT->tail].next_expansion = box_number;
                LOT->tail = box_number;
            }
        } while (!done);

        LOT->head = node->next_expansion;
        node->next_expansion = NO_BOX;
    }

    return 1;
}
*/

/*
void GetJointAbsPosition(ITEM_INFO *item, PHD_VECTOR *vec, int32_t joint)
{
    OBJECT_INFO *object = &g_Objects[item->object_number];

    phd_PushUnitMatrix();
    g_PhdMatrixPtr->_03 = 0;
    g_PhdMatrixPtr->_13 = 0;
    g_PhdMatrixPtr->_23 = 0;

    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);

    int16_t *frame = GetBestFrame(item);
    phd_TranslateRel(
        frame[FRAME_POS_X], frame[FRAME_POS_Y], frame[FRAME_POS_Z]);

    int32_t *packed_rotation = (int32_t *)(frame + FRAME_ROT);
    phd_RotYXZpack(*packed_rotation++);

    int32_t *bone = &g_AnimBones[object->bone_index];

    int16_t *extra_rotation = (int16_t *)item->data;
    for (int i = 0; i < joint; i++) {
        int32_t bone_extra_flags = bone[0];
        if (bone_extra_flags & BEB_POP) {
            phd_PopMatrix();
        }
        if (bone_extra_flags & BEB_PUSH) {
            phd_PushMatrix();
        }

        phd_TranslateRel(bone[1], bone[2], bone[3]);
        phd_RotYXZpack(*packed_rotation++);

        if (bone_extra_flags & BEB_ROT_Y) {
            phd_RotY(*extra_rotation++);
        }
        if (bone_extra_flags & BEB_ROT_X) {
            phd_RotX(*extra_rotation++);
        }
        if (bone_extra_flags & BEB_ROT_Z) {
            phd_RotZ(*extra_rotation++);
        }

        bone += 4;
    }

    phd_TranslateRel(vec->x, vec->y, vec->z);
    vec->x = (g_PhdMatrixPtr->_03 >> W2V_SHIFT) + item->pos.x;
    vec->y = (g_PhdMatrixPtr->_13 >> W2V_SHIFT) + item->pos.y;
    vec->z = (g_PhdMatrixPtr->_23 >> W2V_SHIFT) + item->pos.z;
    phd_PopMatrix();
}
*/
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

/*
void ObjectCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
    ITEM_INFO *item = &g_Items[item_num];

    if (!TestBoundsCollide(item, lara_item, coll->radius)) {
        return;
    }
    if (!TestCollision(item, lara_item)) {
        return;
    }

    if (coll->enable_baddie_push) {
        ItemPushLara(item, lara_item, coll, 0, 1);
    }
}
*/

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

/*
void DoorCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
    ITEM_INFO *item = &g_Items[item_num];

    if (!TestBoundsCollide(item, lara_item, coll->radius)) {
        return;
    }
    if (!TestCollision(item, lara_item)) {
        return;
    }

    if (coll->enable_baddie_push) {
        if (item->current_anim_state != item->goal_anim_state) {
            ItemPushLara(item, lara_item, coll, coll->enable_spaz, 1);
        } else {
            ItemPushLara(item, lara_item, coll, 0, 1);
        }
    }s
}
*/

/*
int32_t TestLaraPosition(int16_t *bounds, ITEM_INFO *item, ITEM_INFO *lara_item)
{
    PHD_ANGLE xrotrel = lara_item->pos.x_rot - item->pos.x_rot;
    PHD_ANGLE yrotrel = lara_item->pos.y_rot - item->pos.y_rot;
    PHD_ANGLE zrotrel = lara_item->pos.z_rot - item->pos.z_rot;
    if (xrotrel < bounds[6] || xrotrel > bounds[7]) {
        return 0;
    }
    if (yrotrel < bounds[8] || yrotrel > bounds[9]) {
        return 0;
    }
    if (zrotrel < bounds[10] || zrotrel > bounds[11]) {
        return 0;
    }

    int32_t x = lara_item->pos.x - item->pos.x;
    int32_t y = lara_item->pos.y - item->pos.y;
    int32_t z = lara_item->pos.z - item->pos.z;
    phd_PushUnitMatrix();
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    PHD_MATRIX *mptr = g_PhdMatrixPtr;
    int32_t rx = (mptr->_00 * x + mptr->_10 * y + mptr->_20 * z) >> W2V_SHIFT;
    int32_t ry = (mptr->_01 * x + mptr->_11 * y + mptr->_21 * z) >> W2V_SHIFT;
    int32_t rz = (mptr->_02 * x + mptr->_12 * y + mptr->_22 * z) >> W2V_SHIFT;
    phd_PopMatrix();
    if (rx < bounds[0] || rx > bounds[1]) {
        return 0;
    }
    if (ry < bounds[2] || ry > bounds[3]) {
        return 0;
    }
    if (rz < bounds[4] || rz > bounds[5]) {
        return 0;
    }

    return 1;
}
*/
/*
void AnimateLaraUntil(ITEM_INFO *lara_item, int32_t goal)
{
    lara_item->goal_anim_state = goal;
    do {
        AnimateLara(lara_item);
    } while (lara_item->current_anim_state != goal);
}
*/

/*
int32_t MoveLaraPosition(PHD_VECTOR *vec, ITEM_INFO *item, ITEM_INFO *lara_item)
{
    PHD_3DPOS dest;
    dest.x_rot = item->pos.x_rot;
    dest.y_rot = item->pos.y_rot;
    dest.z_rot = item->pos.z_rot;
    phd_PushUnitMatrix();
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    PHD_MATRIX *mptr = g_PhdMatrixPtr;
    dest.x = item->pos.x
        + ((mptr->_00 * vec->x + mptr->_01 * vec->y + mptr->_02 * vec->z)
           >> W2V_SHIFT);
    dest.y = item->pos.y
        + ((mptr->_10 * vec->x + mptr->_11 * vec->y + mptr->_12 * vec->z)
           >> W2V_SHIFT);
    dest.z = item->pos.z
        + ((mptr->_20 * vec->x + mptr->_21 * vec->y + mptr->_22 * vec->z)
           >> W2V_SHIFT);
    phd_PopMatrix();
    return Move3DPosTo3DPos(&lara_item->pos, &dest, MOVE_SPEED, MOVE_ANG);
}
*/

/*
int32_t Move3DPosTo3DPos(PHD_3DPOS *srcpos, PHD_3DPOS *destpos, int32_t velocity, PHD_ANGLE angadd)
{
    PHD_ANGLE angdif;

    int32_t x = destpos->x - srcpos->x;
    int32_t y = destpos->y - srcpos->y;
    int32_t z = destpos->z - srcpos->z;
    int32_t dist = phd_sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
    if (velocity >= dist) {
        srcpos->x = destpos->x;
        srcpos->y = destpos->y;
        srcpos->z = destpos->z;
    } else {
        srcpos->x += (x * velocity) / dist;
        srcpos->y += (y * velocity) / dist;
        srcpos->z += (z * velocity) / dist;
    }

    angdif = destpos->x_rot - srcpos->x_rot;
    if (angdif > angadd) {
        srcpos->x_rot += angadd;
    } else if (angdif < -angadd) {
        srcpos->x_rot -= angadd;
    } else {
        srcpos->x_rot = destpos->x_rot;
    }

    angdif = destpos->y_rot - srcpos->y_rot;
    if (angdif > angadd) {
        srcpos->y_rot += angadd;
    } else if (angdif < -angadd) {
        srcpos->y_rot -= angadd;
    } else {
        srcpos->y_rot = destpos->y_rot;
    }

    angdif = destpos->z_rot - srcpos->z_rot;
    if (angdif > angadd) {
        srcpos->z_rot += angadd;
    } else if (angdif < -angadd) {
        srcpos->z_rot -= angadd;
    } else {
        srcpos->z_rot = destpos->z_rot;
    }

    return srcpos->x == destpos->x && srcpos->y == destpos->y
        && srcpos->z == destpos->z && srcpos->x_rot == destpos->x_rot
        && srcpos->y_rot == destpos->y_rot && srcpos->z_rot == destpos->z_rot;
}
*/
void DrawPickupItem(ITEM_INFO *item)
{
     DrawSpriteItem(item);

}

void DrawSpriteItem(ITEM_INFO *item)
{
	Output_DrawSprite(item->pos.x, item->pos.y, item->pos.z,
        g_Objects[item->object_number].mesh_index - item->frame_number,
        item->shade);
}

void Output_DrawScreenSprite(
    int32_t sx, int32_t sy, int32_t z, int32_t scale_h, int32_t scale_v,
    int32_t sprnum, int16_t shade, uint16_t flags)
{
	PHDSPRITESTRUCT *sprite = &g_PhdSpriteInfo[sprnum];
    //PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];
    int32_t x1 = sx + (scale_h * (sprite->x1 >> 3) / PHD_ONE);
    int32_t x2 = sx + (scale_h * (sprite->x2 >> 3) / PHD_ONE);
    int32_t y1 = sy + (scale_v * (sprite->y1 >> 3) / PHD_ONE);
    int32_t y2 = sy + (scale_v * (sprite->y2 >> 3) / PHD_ONE);
    //if (x2 >= 0 && y2 >= 0 && x1 < ViewPort_GetWidth() && y1 < ViewPort_GetHeight())
	if (x2 >= 0 && y2 >= 0 && x1 < Screen_GetResWidth() && y1 < Screen_GetResHeight())
	{
        S_Output_DrawSprite(x1, y1, x2, y2, 8 * z, sprnum, 0);
    }
}


void Output_DrawScreenSprite2D(int32_t sx, int32_t sy, int32_t z, int32_t scale_h,
    int32_t scale_v, int32_t sprnum, int16_t shade, uint16_t flags, int32_t page)
{
	PHDSPRITESTRUCT *sprite = &g_PhdSpriteInfo[sprnum];

    int32_t x1 = sx + (scale_h * sprite->x1 / PHD_ONE);
    int32_t x2 = sx + (scale_h * sprite->x2 / PHD_ONE);
    int32_t y1 = sy + (scale_v * sprite->y1 / PHD_ONE);
    int32_t y2 = sy + (scale_v * sprite->y2 / PHD_ONE);

	if (x2 >= 0 && y2 >= 0 && x1 < Screen_GetResWidth() && y1 < Screen_GetResHeight())
	{
        S_Output_DrawSprite(x1, y1, x2, y2, z, sprnum, 0);
    }
}

void Output_DrawSprite(int32_t x, int32_t y, int32_t z, int16_t sprnum, int16_t shade)
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
    
	if (zv < Z_NEAR || zv > (DRAW_DIST_MAX << W2V_SHIFT) )
	{
        return;
    }

    int32_t xv = g_W2VMatrix._00 * x + g_W2VMatrix._01 * y + g_W2VMatrix._02 * z;
    int32_t yv = g_W2VMatrix._10 * x + g_W2VMatrix._11 * y + g_W2VMatrix._12 * z;
    int32_t zp = zv / g_PhdPersp;

	
    //PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];

	PHDSPRITESTRUCT *sprite = &g_PhdSpriteInfo[sprnum];

    int32_t x1 = ViewPort_GetCenterX() + (xv + (sprite->x1 << W2V_SHIFT)) / zp;
    int32_t y1 = ViewPort_GetCenterY() + (yv + (sprite->y1 << W2V_SHIFT)) / zp;
    int32_t x2 = ViewPort_GetCenterX() + (xv + (sprite->x2 << W2V_SHIFT)) / zp;
    int32_t y2 = ViewPort_GetCenterY() + (yv + (sprite->y2 << W2V_SHIFT)) / zp;
    
	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;
	
	if (x2 >= g_PhdLeft && y2 >= g_PhdTop
        && x1 <= g_PhdRight && y1 <= g_PhdBottom)
	{
        int32_t depth = zv >> W2V_SHIFT;
        shade += CalcFogShade(depth);
        CLAMPG(shade, 0x1FFF);
        S_Output_DrawSprite(x1, y1, x2, y2, zv, sprnum, shade);
    }
	
}

void S_Output_DrawSprite(int16_t x1, int16_t y1, int16_t x2, int y2, int z, int sprnum, int shade)
{
	int32_t * sort = sort3dptr;
	int16_t * info = info3dptr;
		
	sort[0] = (int32_t)info;
	sort[1] = z; //depth

	sort3dptr += 2;

	info[0] = 8; //scaled sprite draw func
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

void Output_DrawUISprite(int32_t x, int32_t y, int32_t scale, int16_t sprnum, int16_t shade)
{
    //PHD_SPRITE *sprite = &g_PhdSpriteInfo[sprnum];
	PHDSPRITESTRUCT *sprite = &g_PhdSpriteInfo[sprnum];

	int32_t x1 = x + (scale * sprite->x1 >> 16);
    int32_t x2 = x + (scale * sprite->x2 >> 16);
    int32_t y1 = y + (scale * sprite->y1 >> 16);
    int32_t y2 = y + (scale * sprite->y2 >> 16);

	g_PhdLeft = 0;
	g_PhdTop = 0;
    g_PhdRight = Screen_GetResWidth() - 1;
    g_PhdBottom = Screen_GetResHeight() - 1;
	

	if (x2 >= g_PhdLeft && y2 >= g_PhdTop
        && x1 <= g_PhdRight && y1 <= g_PhdBottom)
	{
        S_Output_DrawSprite(x1, y1, x2, y2, 200, sprnum, shade);
    }
}
/*
void Output_DrawScreenLine(int32_t sx, int32_t sy, int32_t w, int32_t h, int col)
{
    VBUF2 vertices[8];
    //рисуем белую линию посредине молнии Тора
    vertices[0].x = (float)sx;
    vertices[0].y = (float)sy;
    //vertices[0].z = depth;
    vertices[0].g = col; //Compose_Colour(255, 255, 255);

    vertices[1].x = (float)(sx + w);
    vertices[1].y = (float)(sy + h);
    //vertices[1].z = depth;
    vertices[1].g = col; //Compose_Colour(255, 255, 255);

    S_Output_DrawLine(vertices, 0);
}
*/

void Output_DrawScreenBox(int32_t sx, int32_t sy, int32_t w, int32_t h)
{
	
    //RGB888 rgb_border_light = Output_GetPaletteColor(15);
    //RGB888 rgb_border_dark = Output_GetPaletteColor(31);

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

void Output_DrawScreenFBox(int32_t sx, int32_t sy, int32_t w, int32_t h)
{
    //S_Output_DrawTranslucentQuad(sx, sy, sx + w, sy + h);

    int32_t* sort = sort3dptr;
    int16_t* info = info3dptr;

    sort[0] = (int32_t)info;
    sort[1] = 1200; //depth исправить как в ТР1

    sort3dptr += 2;

    info[0] = 7; //draw routine flat transparent shaded poly

    //info[1] = Compose_Colour(30, 30, 30); //color исправить как в ТР1
    info[1] = 24;
    info[2] = 4; //num coords

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

void Output_DrawScreenFlatQuad(int32_t sx, int32_t sy, int32_t w, int32_t h, RGB888 color, int depth)
{
    //S_Output_Draw2DQuad(sx, sy, sx + w, sy + h, color, color, color, color);

    int32_t* sort = sort3dptr;
    int16_t* info = info3dptr;

    sort[0] = (int32_t)info;
    sort[1] = depth; //depth

    sort3dptr += 2;

    info[0] = 5; //draw routine flat shaded poly
    
    info[1] = Compose_Colour(color.r, color.g, color.b); //color
    info[2] = 4; //num coords
    
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



/*
void AlignLaraPosition(PHD_VECTOR* vec, ITEM_INFO* item, ITEM_INFO* lara_item)
{
    lara_item->pos.x_rot = item->pos.x_rot;
    lara_item->pos.y_rot = item->pos.y_rot;
    lara_item->pos.z_rot = item->pos.z_rot;

    phd_PushUnitMatrix();
    phd_RotYXZ(item->pos.y_rot, item->pos.x_rot, item->pos.z_rot);
    PHD_MATRIX *mptr = g_PhdMatrixPtr;
    lara_item->pos.x = item->pos.x
        + ((mptr->_00 * vec->x + mptr->_01 * vec->y + mptr->_02 * vec->z)
           >> W2V_SHIFT);
    lara_item->pos.y = item->pos.y
        + ((mptr->_10 * vec->x + mptr->_11 * vec->y + mptr->_12 * vec->z)
           >> W2V_SHIFT);
    lara_item->pos.z = item->pos.z
        + ((mptr->_20 * vec->x + mptr->_21 * vec->y + mptr->_22 * vec->z)
           >> W2V_SHIFT);
    phd_PopMatrix();
}
*/

/*
void TrapCollision(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
    ITEM_INFO *item = &g_Items[item_num];

    if (item->status == IS_ACTIVE) {
        if (TestBoundsCollide(item, lara_item, coll->radius)) {
            TestCollision(item, lara_item);
        }
    } else if (item->status != IS_INVISIBLE) {
        ObjectCollision(item_num, lara_item, coll);
    }
}
*/

void S_AnimateTextures(int32_t ticks)
{
    g_WibbleOffset = (g_WibbleOffset + ticks) % WIBBLE_SIZE;

    static int32_t tick_comp = 0;
    tick_comp += ticks;

    while (tick_comp > TICKS_PER_FRAME * 5) {
        int16_t *ptr = g_AnimTextureRanges;
        int16_t i = *ptr++;
        while (i > 0) {
            int16_t j = *ptr++;
            PHD_TEXTURE temp = g_PhdTextureInfo[*ptr];
            while (j > 0) {
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
        //if (g_PhdMatrixPtr->_23 > Output_GetNearZ()
		if (g_PhdMatrixPtr->_23 > Z_NEAR
            //&& g_PhdMatrixPtr->_23 < Output_GetFarZ()) {
			&& g_PhdMatrixPtr->_23 < DRAW_DIST_MAX << W2V_SHIFT) {
            phd_RotYXZ(fx->pos.y_rot, fx->pos.x_rot, fx->pos.z_rot);
            if (object->nmeshes) {
                Output_CalculateStaticLight(fx->shade);
                Output_DrawPolygons(g_Meshes[object->mesh_index], -1);
            } else {
                Output_CalculateLight(
                    fx->pos.x, fx->pos.y, fx->pos.z, fx->room_number);
                Output_DrawPolygons(g_Meshes[fx->frame_number], -1);
            }
        }
        phd_PopMatrix();
    }
}

void Output_DrawLightningSegment(int32_t x1, int32_t y1, int32_t z1, int32_t x2, int32_t y2, int32_t z2, int32_t width)
{

	if (z1 >= Z_NEAR && z1 <= DRAW_DISTANCE_MAX && z2 >= Z_NEAR && z2 <= DRAW_DISTANCE_MAX)
	{
        x1 = ViewPort_GetCenterX() + x1 / (z1 / g_PhdPersp);
        y1 = ViewPort_GetCenterY() + y1 / (z1 / g_PhdPersp);
        x2 = ViewPort_GetCenterX() + x2 / (z2 / g_PhdPersp);
        y2 = ViewPort_GetCenterY() + y2 / (z2 / g_PhdPersp);
        
		int32_t thickness1 = (width << W2V_SHIFT) / (z1 / g_PhdPersp);
        int32_t thickness2 = (width << W2V_SHIFT) / (z2 / g_PhdPersp);
        
		S_Output_DrawLightningSegment(x1, y1, z1, thickness1, x2, y2, z2, thickness2);
    }
}

void S_Output_DrawLightningSegment(int x1, int y1, int z1, int thickness1, int x2, int y2, int z2, int thickness2)
{
	//рисуем саму синюю молнию от Тора
    VBUF2 vertices[8];
    
	int depth = z1;
	if(z2 < z1)
		depth = z2;

	vertices[0].x = (float)x1;
    vertices[0].y = (float)y1;
	//vertices[0].z = (float)8589934592.0f / z1;
    vertices[0].g = 0.0f;

    vertices[1].x = (float)(thickness1 + x1);
    vertices[1].y = (float)y1;
    //vertices[1].z = (float)8589934592.0f / z1;
    vertices[1].g = 0.0f;

    vertices[2].x = (float) (thickness2 + x2);
    vertices[2].y = (float)y2;
    //vertices[2].z = 8589934592.0f / z2;
    vertices[2].g = 0.0f;

    vertices[3].x = (float)x2;
    vertices[3].y = (float)y2;
    //vertices[3].z = 8589934592.0f / z2;
    vertices[3].g = 0.0f;

    int num = ClipVertices2(4, vertices);
    
	if (num)
	{
        S_Output_DrawTriangle(vertices, num, depth);
    }


	//рисуем белую линию посредине молнии Тора
    vertices[0].x = (float)(thickness1 / 2 + x1);
    vertices[0].y = (float)y1;
    //vertices[0].z = depth;
    vertices[0].g = (float)ColorLighting2; //Compose_Colour(255, 255, 255);

    vertices[1].x = (float) (thickness2 / 2 + x2);
    vertices[1].y = (float)y2;
    //vertices[1].z = depth;
    vertices[1].g = (float)ColorLighting2; //Compose_Colour(255, 255, 255);

	S_Output_DrawLine(vertices, depth);
}

void Output_DrawScreenLine(int32_t sx, int32_t sy, int32_t w, int32_t h, int color)
{
    //S_Output_Draw2DLine(sx, sy, sx + w, sy + h, col, col);

	VBUF2 vertices[8];

	vertices[0].x = (float)sx;
    vertices[0].y = (float)sy;
    //vertices[0].z = depth;
    vertices[0].g = (float)color; //Compose_Colour(255, 255, 255);

    vertices[1].x = (float)sx + w;
    vertices[1].y = (float)sy + h;
    //vertices[1].z = depth;
    vertices[1].g = (float)color; //Compose_Colour(255, 255, 255);

	S_Output_DrawLine(vertices, 0);
}


void S_Output_DrawTriangle(VBUF2 * vertices, int vert_count, int depth)
{
	int32_t * sort = sort3dptr;
	int16_t * info = info3dptr;
		
	sort[0] = (int32_t)info;
	sort[1] = depth;

	sort3dptr += 2;

	info[0] = 6; //tex->drawtype;
	info[1] = ColorLighting1; //color tex->tpage;
	info[2] = vert_count;

	info += 3;

	int32_t indx = 0;

	do
	{
		info[0] = (short int) vertices[indx].x;
		info[1] = (short int) vertices[indx].y;
		info[2] = (short int) vertices[indx].g;

		info += 3;
		indx++;

	}while ( indx < vert_count );
	
	info3dptr = info;
								
	surfacenum++;

}

void S_Output_DrawLine(VBUF2 * vertices, int depth)
{
	int32_t * sort = sort3dptr;
	int16_t * info = info3dptr;
		
	sort[0] = (int32_t)info;
	sort[1] = depth - 2;

	sort3dptr += 2;

	info[0] = 4; //tex->drawtype;
	info[1] = (short int) vertices[0].x;
	info[2] = (short int) vertices[0].y;
	info[3] = (short int) vertices[1].x;
	info[4] = (short int) vertices[1].y;
	//info[5] = ColorLighting2; //color 
	info[5] = (short int) vertices[0].g; //color 
	
	info += 6;

	info3dptr = info;
								
	surfacenum++;

}


void Output_DrawSpriteRel(int32_t x, int32_t y, int32_t z, int16_t sprnum, int16_t shade)
{
    int32_t zv = g_PhdMatrixPtr->_20 * x + g_PhdMatrixPtr->_21 * y
        + g_PhdMatrixPtr->_22 * z + g_PhdMatrixPtr->_23;
    if (zv < Z_NEAR || zv > (DRAW_DIST_MAX << W2V_SHIFT)) {
        return;
    }

    int32_t xv = g_PhdMatrixPtr->_00 * x + g_PhdMatrixPtr->_01 * y
        + g_PhdMatrixPtr->_02 * z + g_PhdMatrixPtr->_03;
    int32_t yv = g_PhdMatrixPtr->_10 * x + g_PhdMatrixPtr->_11 * y
        + g_PhdMatrixPtr->_12 * z + g_PhdMatrixPtr->_13;
    int32_t zp = zv / g_PhdPersp;

    PHDSPRITESTRUCT*sprite = &g_PhdSpriteInfo[sprnum];
    
	int32_t x1 = ViewPort_GetCenterX() + (xv + (sprite->x1 << W2V_SHIFT)) / zp;
    int32_t y1 = ViewPort_GetCenterY() + (yv + (sprite->y1 << W2V_SHIFT)) / zp;
    int32_t x2 = ViewPort_GetCenterX() + (xv + (sprite->y1 << W2V_SHIFT)) / zp;
    int32_t y2 = ViewPort_GetCenterY() + (yv + (sprite->y2 << W2V_SHIFT)) / zp;
    
	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;
	
	
	if (x2 >= g_PhdLeft && y2 >= g_PhdTop
        && x1 <= g_PhdRight && y1 <= g_PhdBottom) {
        int32_t depth = zv >> W2V_SHIFT;
        shade += CalcFogShade(depth);
        CLAMPG(shade, 0x1FFF);
        S_Output_DrawSprite(x1, y1, x2, y2, zv, sprnum, shade);
    }
}
