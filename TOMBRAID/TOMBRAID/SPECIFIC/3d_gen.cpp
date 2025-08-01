#include "3d_gen.h"

#include "matrix.h"
#include "phd_math.h"
#include "vars.h"
//#include "winmain.h"
#include "screen.h"
//#include "config.h"
//#include "game/output.h"
//#include "game/screen.h"
//#include "global/vars.h"

#include <math.h>

void phd_LookAt(int32_t xsrc, int32_t ysrc, int32_t zsrc, int32_t xtar,
				int32_t ytar, int32_t ztar, int16_t roll)
{
	PHD_ANGLE angles[2];
	phd_GetVectorAngles(xtar - xsrc, ytar - ysrc, ztar - zsrc, angles);

	PHD_3DPOS viewer;
	viewer.x = xsrc;
	viewer.y = ysrc;
	viewer.z = zsrc;
	viewer.x_rot = angles[1];
	viewer.y_rot = angles[0];
	viewer.z_rot = roll;
	phd_GenerateW2V(&viewer);
}

void phd_GetVectorAngles(int32_t x, int32_t y, int32_t z, int16_t *dest)
{
	dest[0] = phd_atan(z, x);

	while ((int16_t)x != x || (int16_t)y != y || (int16_t)z != z)
	{
		x >>= 2;
		y >>= 2;
		z >>= 2;
	}

	PHD_ANGLE pitch = phd_atan(phd_sqrt(SQUARE(x) + SQUARE(z)), y);

	if ((y > 0 && pitch > 0) || (y < 0 && pitch < 0))
	{
		pitch = -pitch;
	}

	dest[1] = pitch;
}

int32_t phd_VisibleZClip(PHD_VBUF *vn1, PHD_VBUF *vn2, PHD_VBUF *vn3)
{
	double v1x = vn1->xv;
	double v1y = vn1->yv;
	double v1z = vn1->zv;
	double v2x = vn2->xv;
	double v2y = vn2->yv;
	double v2z = vn2->zv;
	double v3x = vn3->xv;
	double v3y = vn3->yv;
	double v3z = vn3->zv;
	double a = v3y * v1x - v1y * v3x;
	double b = v3x * v1z - v1x * v3z;
	double c = v3z * v1y - v1z * v3y;
	return a * v2z + b * v2y + c * v2x < 0.0;
}

void phd_RotateLight(int16_t pitch, int16_t yaw)
{
	int32_t cp = phd_cos(pitch);
	int32_t sp = phd_sin(pitch);
	int32_t cy = phd_cos(yaw);
	int32_t sy = phd_sin(yaw);
	int32_t ls_x = TRIGMULT2(cp, sy);
	int32_t ls_y = -sp;
	int32_t ls_z = TRIGMULT2(cp, cy);
	g_LsVectorView.x = (g_W2VMatrix._00 * ls_x + g_W2VMatrix._01 * ls_y +
						g_W2VMatrix._02 * ls_z) >>
					   W2V_SHIFT;
	g_LsVectorView.y = (g_W2VMatrix._10 * ls_x + g_W2VMatrix._11 * ls_y +
						g_W2VMatrix._12 * ls_z) >>
					   W2V_SHIFT;
	g_LsVectorView.z = (g_W2VMatrix._20 * ls_x + g_W2VMatrix._21 * ls_y +
						g_W2VMatrix._22 * ls_z) >>
					   W2V_SHIFT;
}

void phd_AlterFOV(PHD_ANGLE fov)
{
	// In places that use GAME_FOV, it can be safely changed to user's choice.
	// But for cinematics, the FOV value chosen by devs needs to stay
	// unchanged, otherwise the game renders the low camera in the Lost Valley
	// cutscene wrong.

	/*
	if (g_Config.fov_vertical)
	{
	//double aspect_ratio = SCREEN_WIDTH / (double)SCREEN_HEIGHT;
	double fov_rad_h = fov * PI / 32760;
	double fov_rad_v = 2 * atan(aspect_ratio * tan(fov_rad_h / 2));
	//fov = round((fov_rad_v / PI) * 32760);
			fov = (PHD_ANGLE)(fov_rad_v / PI) * 32760;
}
	*/

	int16_t c = phd_cos(fov / 2);
	int16_t s = phd_sin(fov / 2);

	if (!lara_dist)
		g_PhdPersp = ((Screen_GetResWidth() / 2) * c) / s;
	else
		g_PhdPersp = ((Screen_GetResHeight() / 2) * c) / s;
}
