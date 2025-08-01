#include "overlay.h"

//#include "config.h"
//#include "game/clock.h"
//#include "game/output.h"
#include "screen.h"
#include "text.h"
#//include "game/viewport.h"
#include "backbuffer.h"
#include "const.h"
#include "draw.h"
#include "types.h"
#include "vars.h"

#include <stdio.h>

#define COLOR_STEPS 5
#define MAX_PICKUP_COLUMNS 4
#define MAX_PICKUPS 16

// static TEXTSTRING *m_AmmoText = NULL;
TEXTSTRING *m_AmmoText = NULL;
TEXTSTRING *m_FPSText = NULL;
static int16_t m_BarOffsetY[6] = {0};
static DISPLAYPU m_Pickups[MAX_PICKUPS] = {0};

static RGB888 m_ColorBarMap[][COLOR_STEPS] = {
	// gold
	{{112, 92, 44}, {164, 120, 72}, {112, 92, 44}, {88, 68, 0}, {80, 48, 20}},
	// blue
	{{100, 116, 100},
	 {92, 160, 156},
	 {100, 116, 100},
	 {76, 80, 76},
	 {48, 48, 48}},
	// grey
	{{88, 100, 88}, {116, 132, 116}, {88, 100, 88}, {76, 80, 76}, {48, 48, 48}},
	// red
	{{160, 40, 28}, {184, 44, 32}, {160, 40, 28}, {124, 32, 32}, {84, 20, 32}},
	// silver
	{{150, 150, 150},
	 {230, 230, 230},
	 {200, 200, 200},
	 {140, 140, 140},
	 {100, 100, 100}},
	// green
	{{100, 190, 20},
	 {130, 230, 30},
	 {100, 190, 20},
	 {90, 150, 15},
	 {80, 110, 10}},
	// gold2
	{{220, 170, 0}, {255, 200, 0}, {220, 170, 0}, {185, 140, 0}, {150, 100, 0}},
	// blue2
	{{0, 170, 220}, {0, 200, 255}, {0, 170, 220}, {0, 140, 185}, {0, 100, 150}},
	// pink
	{{220, 140, 170},
	 {255, 150, 200},
	 {210, 130, 160},
	 {165, 100, 120},
	 {120, 60, 70}},
};
/*
static void Overlay_GetBarLocation(
	int8_t bar_location, int32_t width, int32_t height, int32_t *x, int32_t *y);

	*/
static void Overlay_DrawBar(int32_t value, int32_t value_max, int32_t bar_type);

/*
static void Overlay_OnAmmoTextRemoval(const TEXTSTRING *textstring);
static void Overlay_OnFPSTextRemoval(const TEXTSTRING *textstring);
*/

static void Overlay_GetBarLocation(int8_t bar_location, int32_t width,
								   int32_t height, int32_t *x, int32_t *y)
{
	const int32_t screen_margin_h = 20;
	const int32_t screen_margin_v = 18;
	const int32_t bar_spacing = 8;

	*x = screen_margin_h;

	if (bar_location == T1M_BL_TOP_LEFT || bar_location == T1M_BL_BOTTOM_LEFT)
	{
		*x = screen_margin_h;
	}
	else if (bar_location == T1M_BL_TOP_RIGHT ||
			 bar_location == T1M_BL_BOTTOM_RIGHT)
	{
		*x = Screen_GetResWidthDownscaled() -
			 (int)(width * g_Config.ui.bar_scale) - screen_margin_h;
	}
	else
	{
		*x = (Screen_GetResWidthDownscaled() - width) / 2;
	}

	*y = screen_margin_v + m_BarOffsetY[bar_location];

	if (bar_location == T1M_BL_TOP_LEFT || bar_location == T1M_BL_TOP_CENTER ||
		bar_location == T1M_BL_TOP_RIGHT)
	{
		*y = screen_margin_v + m_BarOffsetY[bar_location];
	}
	else
	{
		*y = Screen_GetResHeightDownscaled() -
			 (int)(height * g_Config.ui.bar_scale) - screen_margin_v -
			 m_BarOffsetY[bar_location];
	}

	m_BarOffsetY[bar_location] += height + bar_spacing;
}

static void Overlay_DrawBar(int32_t value, int32_t value_max, int32_t bar_type)
{
	static int32_t blink_counter = 0;
	const int32_t percent_max = 100;

	if (value < 0)
	{
		value = 0;
	}
	else if (value > value_max)
	{
		value = value_max;
	}
	int32_t percent = value * 100 / value_max;

	const RGB888 rgb_bgnd = {0, 0, 0};
	const RGB888 rgb_border_light = {128, 128, 128};
	const RGB888 rgb_border_dark = {64, 64, 64};

	int32_t width = 100;
	int32_t height = 5;
	int16_t bar_color = bar_type;

	int32_t x = 0;
	int32_t y = 0;
	if (bar_type == BT_LARA_HEALTH)
	{
		// g_Config.healthbar_location = 0
		Overlay_GetBarLocation(g_Config.healthbar_location, width, height, &x,
							   &y);
		bar_color = g_Config.healthbar_color;
		// bar_color = 3;
	}
	else if (bar_type == BT_LARA_AIR)
	{
		// g_Config.airbar_location = 2
		Overlay_GetBarLocation(g_Config.airbar_location, width, height, &x, &y);
		bar_color = g_Config.airbar_color;
		// bar_color = 1;
	}
	else if (bar_type == BT_ENEMY_HEALTH)
	{
		// Overlay_GetBarLocation(g_Config.enemy_healthbar_location, width,
		// height, &x, &y);
		Overlay_GetBarLocation(g_Config.enemy_healthbar_location, width, height,
							   &x, &y);
		// bar_color = g_Config.enemy_healthbar_color;
		// bar_color = 2;
	}

	int32_t padding = Screen_GetResWidth() <= 800 ? 1 : 2;
	int32_t border = 1;
	int32_t sx = Screen_GetRenderScale(x) - padding;
	int32_t sy = Screen_GetRenderScale(y) - padding;
	int32_t sw = (int)(Screen_GetRenderScale(width) * g_Config.ui.bar_scale) +
				 padding * 2;
	int32_t sh = (int)(Screen_GetRenderScale(height) * g_Config.ui.bar_scale) +
				 padding * 2;

	// int32_t sw = 191;
	// int32_t sh = 13;

	// border
	Output_DrawScreenFlatQuad(sx - border, sy - border, sw + border,
							  sh + border, rgb_border_dark, 200);
	Output_DrawScreenFlatQuad(sx, sy, sw + border, sh + border,
							  rgb_border_light, 200);

	// background
	Output_DrawScreenFlatQuad(sx, sy, sw, sh, rgb_bgnd, 190);

	const int32_t blink_interval = 20;
	const int32_t blink_threshold = bar_type == BT_ENEMY_HEALTH ? 0 : 20;
	int32_t blink_time = blink_counter++ % blink_interval;
	int32_t blink =
		percent <= blink_threshold && blink_time > blink_interval / 2;

	if (percent && !blink)
	{
		width = width * percent / percent_max;

		sx = Screen_GetRenderScale(x);
		sy = Screen_GetRenderScale(y);
		sw = (int)(Screen_GetRenderScale(width) * g_Config.ui.bar_scale);
		sh = (int)(Screen_GetRenderScale(height) * g_Config.ui.bar_scale);
		/*
		if (g_Config.enable_smooth_bars) {
			for (int i = 0; i < COLOR_STEPS - 1; i++) {
				RGB888 c1 = m_ColorBarMap[bar_color][i];
				RGB888 c2 = m_ColorBarMap[bar_color][i + 1];
				int32_t lsy = sy + i * sh / (COLOR_STEPS - 1);
				int32_t lsh = sy + (i + 1) * sh / (COLOR_STEPS - 1) - lsy;
				Output_DrawScreenGradientQuad(sx, lsy, sw, lsh, c1, c1, c2, c2);
			}
		} else

		{
			for (int i = 0; i < COLOR_STEPS; i++)
			{
				RGB888 color = m_ColorBarMap[bar_color][i];
				int32_t lsy = sy + i * sh / COLOR_STEPS;
				int32_t lsh = sy + (i + 1) * sh / COLOR_STEPS - lsy;
				Output_DrawScreenFlatQuad(sx, lsy, sw, lsh, color);
			}
		}
		*/

		// red
		// RGB888 color = { 160, 40, 28 };
		RGB888 color = m_ColorBarMap[bar_color][0];
		Output_DrawScreenFlatQuad(sx, sy, sw, sh, color, 180);
	}
}

/*
static void Overlay_OnAmmoTextRemoval(const TEXTSTRING *textstring)
{
	m_AmmoText = NULL;
}


static void Overlay_OnFPSTextRemoval(const TEXTSTRING *textstring)
{
	m_FPSText = NULL;
}
*/
void Overlay_Init()
{
	for (int i = 0; i < MAX_PICKUPS; i++)
	{
		m_Pickups[i].duration = 0;
	}
}

void Overlay_DrawHealthBar()
{
	static int32_t old_hit_points = 0;

	for (int i = 0; i < 6; i++)
	{
		m_BarOffsetY[i] = 0;
	}

	int hit_points = g_LaraItem->hit_points;
	if (hit_points < 0)
	{
		hit_points = 0;
	}
	else if (hit_points > LARA_HITPOINTS)
	{
		hit_points = LARA_HITPOINTS;
	}

	if (old_hit_points != hit_points)
	{
		old_hit_points = hit_points;
		g_HealthBarTimer = 40;
	}

	if (g_HealthBarTimer < 0)
	{
		g_HealthBarTimer = 0;
	}

	int32_t show = g_HealthBarTimer > 0 || hit_points <= 0 ||
				   g_Lara.gun_status == LGS_READY;
	/*
	switch (g_Config.healthbar_showing_mode) {
	case T1M_BSM_ALWAYS:
		show = 1;
		break;
	case T1M_BSM_NEVER:
		show = 0;
		break;
	case T1M_BSM_FLASHING_OR_DEFAULT:
		show |= hit_points <= (LARA_HITPOINTS * 20) / 100;
		break;
	case T1M_BSM_FLASHING_ONLY:
		show = hit_points <= (LARA_HITPOINTS * 20) / 100;
		break;
	}
	*/

	show |= hit_points <= (LARA_HITPOINTS * 20) / 100;

	if (!show)
	{
		return;
	}

	Overlay_DrawBar(hit_points, LARA_HITPOINTS, BT_LARA_HEALTH);
}

void Overlay_DrawAirBar()
{
	int32_t show = g_Lara.water_status == LWS_UNDERWATER ||
				   g_Lara.water_status == LWS_SURFACE;

	if (!show)
	{
		return;
	}

	/*
		switch (g_Config.airbar_showing_mode) {
		case T1M_BSM_ALWAYS:
			show = 1;
			break;
		case T1M_BSM_NEVER:
			show = 0;
			break;
		case T1M_BSM_FLASHING_OR_DEFAULT:
			show |= g_Lara.air <= (LARA_AIR * 20) / 100;
			break;
		case T1M_BSM_FLASHING_ONLY:
			show = g_Lara.air <= (LARA_AIR * 20) / 100;
			break;
		}
		*/
	show |= g_Lara.air <= (LARA_AIR * 20) / 100;

	/*
		if (!show)
		{
			return;
		}
		*/
	int air = g_Lara.air;
	if (air < 0)
	{
		air = 0;
	}
	else if (g_Lara.air > LARA_AIR)
	{
		air = LARA_AIR;
	}

	Overlay_DrawBar(air, LARA_AIR, BT_LARA_AIR);
}

void Overlay_DrawEnemyBar()
{
	if (!g_Config.enable_enemy_healthbar || !g_Lara.target)
	{
		return;
	}

	Overlay_DrawBar(g_Lara.target->hit_points,
					g_Objects[g_Lara.target->object_number].hit_points,
					BT_ENEMY_HEALTH);
}

void Overlay_DrawAmmoInfo()
{
	const double scale = 0.8;
	const int32_t text_height = (int)(17 * scale);
	const int32_t text_offset_x = 3;
	const int32_t screen_margin_h = 20;
	const int32_t screen_margin_v = 18;

	char ammostring[80] = "";

	// if (g_Lara.gun_status != LGS_READY || g_OverlayFlag <= 0 ||
	// (g_SaveGame.bonus_flag & GBF_NGPLUS))
	if (g_Lara.gun_status != LGS_READY || g_OverlayFlag <= 0)
	{
		if (m_AmmoText)
		{
			Text_Remove(m_AmmoText);
			m_AmmoText = NULL;
		}
		return;
	}

	switch (g_Lara.gun_type)
	{
	case LGT_PISTOLS:
		return;
	case LGT_MAGNUMS:
		sprintf(ammostring, "%5d B", g_Lara.magnums.ammo);
		break;
	case LGT_UZIS:
		sprintf(ammostring, "%5d C", g_Lara.uzis.ammo);
		break;
	case LGT_SHOTGUN:
		sprintf(ammostring, "%5d A", g_Lara.shotgun.ammo / SHOTGUN_AMMO_CLIP);
		break;
	default:
		return;
	}

	Overlay_MakeAmmoString(ammostring);

	if (m_AmmoText)
	{
		Text_ChangeText(m_AmmoText, ammostring);
	}
	else
	{
		m_AmmoText = Text_Create(-screen_margin_h - text_offset_x,
								 text_height + screen_margin_v, ammostring);
		// m_AmmoText->on_remove = Overlay_OnAmmoTextRemoval;
		Text_SetScale(m_AmmoText, (int)(PHD_ONE * scale),
					  (int)(PHD_ONE * scale));
		Text_AlignRight(m_AmmoText, 1);
	}

	m_AmmoText->pos.y =
		m_BarOffsetY[T1M_BL_TOP_RIGHT]
			? text_height + screen_margin_v + m_BarOffsetY[T1M_BL_TOP_RIGHT]
			: text_height + screen_margin_v;
}

void Overlay_DrawPickups()
{
	static int32_t old_game_timer = 0;
	int16_t time = g_SaveGame.timer - old_game_timer;
	old_game_timer = g_SaveGame.timer;

	if (time > 0 && time < 60)
	{
		int32_t sprite_height =
			MIN(Screen_GetResWidth(), Screen_GetResHeight() * 320 / 200) / 10;
		int32_t sprite_width = sprite_height * 4 / 3;
		int32_t y = Screen_GetResHeight() - sprite_height;
		int32_t x = Screen_GetResWidth() - sprite_height;
		for (int i = 0; i < MAX_PICKUPS; i++)
		{
			DISPLAYPU *pu = &m_Pickups[i];
			pu->duration -= time;
			if (pu->duration <= 0)
			{
				pu->duration = 0;
			}
			else
			{
				Output_DrawUISprite(x, y, Screen_GetRenderScaleGLRage(12288),
									pu->sprnum, 4096);

				if (i % MAX_PICKUP_COLUMNS == MAX_PICKUP_COLUMNS - 1)
				{
					x = Screen_GetResWidth() - sprite_height;
					y -= sprite_height;
				}
				else
				{
					x -= sprite_width;
				}
			}
		}
	}
}

int32_t Screen_GetRenderScaleGLRage(int32_t unit)
{
	// GLRage-style UI scaler
	double result = Screen_GetResWidth();
	result *= unit;
	result /= 800.0;

	// only scale up, not down
	if (result < unit)
	{
		result = unit;
	}

	int32_t round = (int)result;

	return round;
}

void Overlay_DrawFPSInfo()
{
	static int32_t elapsed = 0;

	if (g_Config.render_flags.fps_counter)
	{
		if (GetTickCount() - elapsed >= 1000)
		{
			if (m_FPSText)
			{
				char fps_buf[20];
				sprintf(fps_buf, "%d FPS", g_FPSCounter);
				Text_ChangeText(m_FPSText, fps_buf);
			}
			else
			{
				char fps_buf[20];
				sprintf(fps_buf, "0 FPS");
				// m_FPSText = Text_Create(10, 30, fps_buf);
				m_FPSText = Text_Create(10, 45, fps_buf);
				// m_FPSText->on_remove = Overlay_OnFPSTextRemoval;
			}
			g_FPSCounter = 0;
			elapsed = GetTickCount();
		}
	}
	else if (m_FPSText)
	{
		Text_Remove(m_FPSText);
		m_FPSText = NULL;
		g_FPSCounter = 0;
	}
}

void Overlay_DrawGameInfo()
{
	if (g_OverlayFlag > 0)
	{

		Overlay_DrawHealthBar();
		Overlay_DrawAirBar();
		Overlay_DrawEnemyBar();
		Overlay_DrawPickups();
	}

	Overlay_DrawAmmoInfo();
	Overlay_DrawFPSInfo();

	Text_Draw();
}

void Overlay_AddPickup(int16_t object_num)
{
	for (int i = 0; i < MAX_PICKUPS; i++)
	{
		if (m_Pickups[i].duration <= 0)
		{
			m_Pickups[i].duration = 75;
			m_Pickups[i].sprnum = g_Objects[object_num].mesh_index;
			return;
		}
	}
}

void Overlay_MakeAmmoString(char *string)
{
	char *c;

	for (c = string; *c != 0; c++)
	{
		if (*c == 32)
		{
			continue;
		}
		else if (*c - 'A' >= 0)
		{
			*c += 12 - 'A';
		}
		else
		{
			*c += 1 - '0';
		}
	}
}
