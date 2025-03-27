#include <windows.h>

#include "game.h"
#include "text.h"
#include "draw.h"
#include "control_util.h"
#include "vars.h"
#include "phd_math.h"
#include "winmain.h"
#include "lara.h"
#include "timer.h"
#include "3d_gen.h"
#include "backbuffer.h"
#include "3d_out.h"
#include "matrix.h"
#include "box.h"
#include "camera.h"
#include "inv.h"
#include "collide.h"
#include "sound.h"
#include "backbuffer.h"
#include "overlay.h"

#include "option.h"
#include "screen.h"

static int32_t m_MedipackCoolDown = 0;

int LevelStats(int32_t level_num)
{
    char string[100];
    char time_str[100];
    TEXTSTRING* txt;

    Text_RemoveAll();

    // heading
    sprintf(string, "%s", g_GameFlow.levels[level_num].level_title);
    txt = Text_Create(0, -50, string);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);

    // time taken
    int32_t seconds = g_SaveGame.timer / 30;
    int32_t hours = seconds / 3600;
    int32_t minutes = (seconds / 60) % 60;
    seconds %= 60;

    if (hours)
    {
        sprintf(time_str, "%d:%d%d:%d%d", hours, minutes / 10, minutes % 10, seconds / 10, seconds % 10);
    }
    else
    {
        sprintf(time_str, "%d:%d%d", minutes, seconds / 10, seconds % 10);
    }

    sprintf(string, g_GameFlow.strings[GS_STATS_TIME_TAKEN_FMT], time_str);
    
    txt = Text_Create(0, 70, string);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);

    // secrets
    int32_t secrets_taken = 0;
    int32_t secrets_total = MAX_SECRETS;
    do {
        if (g_SaveGame.secrets & 1)
        {
            secrets_taken++;
        }

        g_SaveGame.secrets >>= 1;
        secrets_total--;
    } while (secrets_total);
    
    sprintf(string, g_GameFlow.strings[GS_STATS_SECRETS_FMT], secrets_taken, g_GameFlow.levels[level_num].secrets);
    
    txt = Text_Create(0, 40, string);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);


    // pickups
    sprintf(string, g_GameFlow.strings[GS_STATS_PICKUPS_FMT], g_SaveGame.pickups);
    txt = Text_Create(0, 10, string);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);

    // kills
    sprintf(string, g_GameFlow.strings[GS_STATS_KILLS_FMT], g_SaveGame.kills);
    txt = Text_Create(0, -20, string);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);


    //wait till action key release
    //while (g_Input.select || g_Input.deselect)
    while (!g_Input.select && !g_Input.deselect)
    {
        Input_Update();
        S_InitialisePolyList();
        Clear_BackBuffer();
        //Output_CopyBufferToScreen(); draw picture
        Input_Update();
        Text_Draw();
        
        S_OutputPolyList();
        S_DumpScreen();
    }

    if (level_num == g_GameFlow.last_level_num)
    {
        return 1;
        //exit to inventory
        //return GF_EXIT_TO_TITLE;
    }

    return 0;
    

}

#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
//#define VK_5 0x35
//#define VK_6 0x36
//#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39


int Game_Loop(int demo_mode)
{
	int32_t nframes=1, game_over = false;
	g_OverlayFlag = 1;

	Initialise_Camera();

	//-----------------------------

	game_over = Control_Phase(nframes, demo_mode);

	//while (!g_bWindowClosed)
	
	while(!game_over)
    //while(1)
	{
		//draw fase
		nframes = Draw_Phase_Game();
		//control fase
		game_over = Control_Phase(nframes, demo_mode);
        
        if (game_over == 1)
            break;
	}


    if (game_over == 1)
    {
        if (LevelStats(g_CurrentLevel))
            return GF_EXIT_TO_TITLE;

        return GF_START_GAME | (g_CurrentLevel + 1 & ((1 << 6) - 1));
    }

	return game_over;


}

void Initialise_Camera()
{
    g_Camera.shift = g_LaraItem->pos.y - WALL_L;

    g_Camera.target.x = g_LaraItem->pos.x;
    g_Camera.target.y = g_Camera.shift;
    g_Camera.target.z = g_LaraItem->pos.z;
    g_Camera.target.room_number = g_LaraItem->room_number;

    g_Camera.pos.x = g_Camera.target.x;
    g_Camera.pos.y = g_Camera.target.y;
    g_Camera.pos.z = g_Camera.target.z - 100;
    g_Camera.pos.room_number = g_Camera.target.room_number;

    g_Camera.target_distance = WALL_L * 3 / 2;
	//g_Camera.target_distance = 100;
    g_Camera.item = NULL;

    g_Camera.number_frames = 1;
    g_Camera.type = CAM_CHASE;
    g_Camera.flags = 0;
    g_Camera.bounce = 0;
    g_Camera.number = NO_CAMERA;
    g_Camera.additional_angle = 0;
    g_Camera.additional_elevation = 0;

    CalculateCamera();
}

/*
void CalculateCamera()
{
    if (g_RoomInfo[g_Camera.pos.room_number].flags & RF_UNDERWATER)
	{
        if (!g_Camera.underwater)
		{
            Sound_Effect(SFX_UNDERWATER, NULL, SPM_ALWAYS);
            g_Camera.underwater = 1;
        }
    }
	else if (g_Camera.underwater)
	{
        //Sound_StopEffect(SFX_UNDERWATER, NULL);
        g_Camera.underwater = 0;
    }

    if (g_Camera.type == CAM_CINEMATIC)
	{
        //InGameCinematicCamera();
        return;
    }

    if (g_Camera.flags != NO_CHUNKY)
	{
        g_ChunkyFlag = true;
    }

    int32_t fixed_camera = g_Camera.item && (g_Camera.type == CAM_FIXED || g_Camera.type == CAM_HEAVY);
    ITEM_INFO *item = fixed_camera ? g_Camera.item : g_LaraItem;

    int16_t *bounds = GetBoundsAccurate(item);

    int32_t y = item->pos.y;

    if (!fixed_camera)
	{
        y += bounds[FRAME_BOUND_MAX_Y] + ((bounds[FRAME_BOUND_MIN_Y] - bounds[FRAME_BOUND_MAX_Y]) * 3 >> 2);
    }
	else
	{
        y += (bounds[FRAME_BOUND_MIN_Y] + bounds[FRAME_BOUND_MAX_Y]) / 2;
    }

    if (g_Camera.item && !fixed_camera)
	{
        bounds = GetBoundsAccurate(g_Camera.item);

        int16_t shift = phd_sqrt(SQUARE(g_Camera.item->pos.z - item->pos.z) + SQUARE(g_Camera.item->pos.x - item->pos.x));

		int16_t angle = phd_atan(g_Camera.item->pos.z - item->pos.z, g_Camera.item->pos.x - item->pos.x) - item->pos.y_rot;
        
		int16_t tilt = phd_atan(shift, y - (g_Camera.item->pos.y + (bounds[FRAME_BOUND_MIN_Y] + bounds[FRAME_BOUND_MAX_Y]) / 2));

		angle >>= 1;
        
		tilt >>= 1;

        if (angle > -MAX_HEAD_ROTATION && angle < MAX_HEAD_ROTATION && tilt > MIN_HEAD_TILT_CAM && tilt < MAX_HEAD_TILT_CAM)
		{
            int16_t change = angle - g_Lara.head_y_rot;

            if (change > HEAD_TURN)
			{
                g_Lara.head_y_rot += HEAD_TURN;
            }
			else if (change < -HEAD_TURN)
			{
                g_Lara.head_y_rot -= HEAD_TURN;
            }
			else
			{
                g_Lara.head_y_rot += change;
            }

            change = tilt - g_Lara.head_x_rot;
            
			if (change > HEAD_TURN)
			{
                g_Lara.head_x_rot += HEAD_TURN;
            }
			else if (change < -HEAD_TURN)
			{
                g_Lara.head_x_rot -= HEAD_TURN;
            }
			else
			{
                g_Lara.head_x_rot += change;
            }

            g_Lara.torso_y_rot = g_Lara.head_y_rot;
            g_Lara.torso_x_rot = g_Lara.head_x_rot;

            g_Camera.type = CAM_LOOK;
            g_Camera.item->looked_at = 1;
        }
    }

    if (g_Camera.type == CAM_LOOK || g_Camera.type == CAM_COMBAT)
	{
        y -= STEP_L;
    
		g_Camera.target.room_number = item->room_number;

        if (g_Camera.fixed_camera)
		{
            g_Camera.target.y = y;
            g_Camera.speed = 1;
        }
		else
		{
            g_Camera.target.y += (y - g_Camera.target.y) >> 2;
            g_Camera.speed = g_Camera.type == CAM_LOOK ? LOOK_SPEED : COMBAT_SPEED;
        }

        g_Camera.fixed_camera = 0;

        if (g_Camera.type == CAM_LOOK)
		{
            LookCamera(item);
        }
		else
		{
            CombatCamera(item);
			int a = 10;
        }
    }
	else
	{
        g_Camera.target.x = item->pos.x;
        g_Camera.target.z = item->pos.z;

        if (g_Camera.flags == FOLLOW_CENTRE)
		{
            int16_t shift = (bounds[FRAME_BOUND_MIN_Z] + bounds[FRAME_BOUND_MAX_Z]) / 2;
            
			g_Camera.target.z += phd_cos(item->pos.y_rot) * shift >> W2V_SHIFT;
            g_Camera.target.x += phd_sin(item->pos.y_rot) * shift >> W2V_SHIFT;
        }

        g_Camera.target.room_number = item->room_number;

        if (g_Camera.fixed_camera != fixed_camera)
		{
            g_Camera.target.y = y;
            g_Camera.fixed_camera = 1;
            g_Camera.speed = 1;
        }
		else
		{
            g_Camera.target.y += (y - g_Camera.target.y) / 4;
            g_Camera.fixed_camera = 0;
        }

        FLOOR_INFO *floor = GetFloor(g_Camera.target.x, g_Camera.target.y, g_Camera.target.z, &g_Camera.target.room_number);

		if (g_Camera.target.y > GetHeight(floor, g_Camera.target.x, g_Camera.target.y, g_Camera.target.z))
		{
            g_ChunkyFlag = false;
        }

        if (g_Camera.type == CAM_CHASE || g_Camera.flags == CHASE_OBJECT)
		{
            ChaseCamera(item);
        }
		else
		{
            FixedCamera();
        }
    }

    g_Camera.last = g_Camera.number;
    g_Camera.fixed_camera = fixed_camera;

    // should we clear the manual camera
    switch (g_Camera.type)
	{
    case CAM_LOOK:
    case CAM_CINEMATIC:
    case CAM_COMBAT:
    case CAM_FIXED:
        g_Camera.additional_angle = 0;
        g_Camera.additional_elevation = 0;
        break;
    }

    if (g_Camera.type != CAM_HEAVY || g_Camera.timer == -1)
	{
        g_Camera.type = CAM_CHASE;
        g_Camera.number = NO_CAMERA;
        g_Camera.last_item = g_Camera.item;
        g_Camera.item = NULL;
        g_Camera.target_angle = g_Camera.additional_angle;
        g_Camera.target_elevation = g_Camera.additional_elevation;
        g_Camera.target_distance = WALL_L * 3 / 2;
		//g_Camera.target_distance = 100;
        g_Camera.flags = 0;
    }

    g_ChunkyFlag = false;
}
*/
int Control_Phase(int32_t nframes, int32_t demo_mode)
{
	int32_t return_val = 0;

    if (g_LevelComplete)
    {
        return 1;
    }

	Input_Update();

    if (g_Lara.death_count > DEATH_WAIT || (g_Lara.death_count > DEATH_WAIT_MIN && g_Input.any && !g_Input.fly_cheat) || g_OverlayFlag == 2)
    {
        if (demo_mode)
        {
            return GF_EXIT_TO_TITLE;
        }
        if (g_OverlayFlag == 2)
        {
            g_OverlayFlag = 1;
            return_val = Display_Inventory(INV_DEATH_MODE);
            if (return_val != GF_NOP)
            {
                return return_val;
            }
        }
        else
        {
            g_OverlayFlag = 2;
        }
    }

	if ((g_Input.option || g_Input.save || g_Input.load || g_OverlayFlag <= 0) && !g_Lara.death_count)
	{
            if (g_OverlayFlag > 0)
			{
                if (g_Input.load)
				{
                    g_OverlayFlag = -1;
                } else if (g_Input.save)
				{
                    g_OverlayFlag = -2;
                }
				else
				{
                    g_OverlayFlag = 0;
                }
            } else {
                if (g_OverlayFlag == -1) {
                    return_val = Display_Inventory(INV_LOAD_MODE);
                } else if (g_OverlayFlag == -2) {
                    return_val = Display_Inventory(INV_SAVE_MODE);
                } else {
                    return_val = Display_Inventory(INV_GAME_MODE);
                }

                g_OverlayFlag = 1;
                if (return_val != GF_NOP) {
                    return return_val;
                }
            }
        }

	int16_t item_num = g_NextItemActive;

    while (item_num != NO_ITEM)
	{
		ITEM_INFO *item = &g_Items[item_num];
        OBJECT_INFO *obj = &g_Objects[item->object_number];
        if (obj->control)
		{
	        obj->control(item_num);
        }
            item_num = item->next_active;
	}

	item_num = g_NextFxActive;
        while (item_num != NO_ITEM) {
            FX_INFO *fx = &g_Effects[item_num];
            OBJECT_INFO *obj = &g_Objects[fx->object_number];
            if (obj->control) {
                obj->control(item_num);
            }
            item_num = fx->next_active;
        }


	LaraControl(0);
	CalculateCamera();
	Sound_UpdateEffects();
	g_SaveGame.timer++;
    g_HealthBarTimer--;
	SpinMessageLoop();

	return g_bWindowClosed;
}

int Get_Key_State(int key)
{

	if(GetAsyncKeyState(key)& 0xFF00) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void Input_Update()
{

		g_Input.forward = Get_Key_State(VK_UP);
		g_Input.back = Get_Key_State(VK_DOWN);
		g_Input.left = Get_Key_State(VK_LEFT);
		g_Input.right = Get_Key_State(VK_RIGHT);
		g_Input.action = Get_Key_State(VK_CONTROL);
		g_Input.jump = Get_Key_State(VK_MENU);
		g_Input.slow = Get_Key_State(VK_SHIFT);
		g_Input.look = Get_Key_State(VK_INSERT);
		g_Input.roll = Get_Key_State(VK_CLEAR); //num pad 5
	    g_Input.step_left = Get_Key_State(VK_HOME); //num pad 7
	    g_Input.step_right = Get_Key_State(VK_PRIOR); //num pad 9
		g_Input.draw = Get_Key_State(VK_SPACE);
        g_Input.select = Get_Key_State(VK_CONTROL) || Get_Key_State(VK_RETURN);
        g_Input.deselect = Get_Key_State(VK_ESCAPE);

        if (m_MedipackCoolDown)
        {
            m_MedipackCoolDown--;
        }
        else
        {
            if (Get_Key_State(VK_8) && Inv_RequestItem(O_MEDI_OPTION))
            {
                UseItem(O_MEDI_OPTION);
                m_MedipackCoolDown = FRAMES_PER_SECOND / 2;
            }
            else if (Get_Key_State(VK_9) && Inv_RequestItem(O_BIGMEDI_OPTION))
            {
                UseItem(O_BIGMEDI_OPTION);
                m_MedipackCoolDown = FRAMES_PER_SECOND / 2;
            }
        }

		//g_Input.option = S_Input_Key(INPUT_KEY_OPTION) && g_Camera.type != CAM_CINEMATIC;
		g_Input.option = Get_Key_State(VK_ESCAPE) && g_Camera.type != CAM_CINEMATIC;

		if (Get_Key_State(VK_1) && Inv_RequestItem(O_GUN_ITEM))
		{
            g_Lara.request_gun_type = LGT_PISTOLS;
        }
		else if (Get_Key_State(VK_2) && Inv_RequestItem(O_SHOTGUN_ITEM))
		{
            g_Lara.request_gun_type = LGT_SHOTGUN;
        }
		else if (Get_Key_State(VK_3) && Inv_RequestItem(O_MAGNUM_ITEM))
		{
            g_Lara.request_gun_type = LGT_MAGNUMS;
        }
		else if (Get_Key_State(VK_4) && Inv_RequestItem(O_UZI_ITEM))
		{
			g_Lara.request_gun_type = LGT_UZIS;
        }

		if (!g_ModeLock && g_Camera.type != CAM_CINEMATIC)
		{
	        g_Input.save = Get_Key_State(VK_F5);
		    g_Input.load = Get_Key_State(VK_F6);
		}

		g_InputDB = Input_GetDebounced(g_Input);

		//SpinMessageLoop();
}

INPUT_STATE Input_GetDebounced(INPUT_STATE input)
{
    INPUT_STATE result;
    result.any = input.any & ~g_OldInputDB.any;
    g_OldInputDB = input;
    return result;
}


/*
void LaraControl(int16_t item_num)
{

	COLL_INFO coll = { 0 };

    ITEM_INFO *item = g_LaraItem;
    ROOM_INFO *r = &g_RoomInfo[item->room_number];
    int32_t room_submerged = r->flags & RF_UNDERWATER;

    if (g_Lara.water_status == LWS_ABOVEWATER && room_submerged)
	{
        g_Lara.water_status = LWS_UNDERWATER;
        g_Lara.air = LARA_AIR;
        item->pos.y += 100;
        item->gravity_status = 0;
        UpdateLaraRoom(item, 0);
        //Sound_StopEffect(SFX_LARA_FALL, NULL);
        if (item->current_anim_state == AS_SWANDIVE)
		{
            item->goal_anim_state = AS_DIVE;
            item->pos.x_rot = -45 * PHD_DEGREE;
            AnimateLara(item);
            item->fall_speed *= 2;
        }
		else if (item->current_anim_state == AS_FASTDIVE)
		{
            item->goal_anim_state = AS_DIVE;
            item->pos.x_rot = -85 * PHD_DEGREE;
            AnimateLara(item);
            item->fall_speed *= 2;
        }
		else
		{
            item->current_anim_state = AS_DIVE;
            item->goal_anim_state = AS_SWIM;
            item->anim_number = AA_JUMPIN;
            item->frame_number = AF_JUMPIN;
            item->pos.x_rot = -45 * PHD_DEGREE;
            item->fall_speed = (item->fall_speed * 3) / 2;
        }
        g_Lara.head_x_rot = 0;
        g_Lara.head_y_rot = 0;
        g_Lara.torso_x_rot = 0;
        g_Lara.torso_y_rot = 0;
        //Splash(item);
    }
	else if (g_Lara.water_status == LWS_UNDERWATER && !room_submerged)
	{
		
        int16_t wh = GetWaterHeight(item->pos.x, item->pos.y, item->pos.z, item->room_number);
        if (wh != NO_HEIGHT && ABS(wh - item->pos.y) < STEP_L)
		{
            g_Lara.water_status = LWS_SURFACE;
            g_Lara.dive_count = DIVE_COUNT + 1;
            item->current_anim_state = AS_SURFTREAD;
            item->goal_anim_state = AS_SURFTREAD;
            item->anim_number = AA_SURFTREAD;
            item->frame_number = AF_SURFTREAD;
            item->fall_speed = 0;
            item->pos.y = wh + 1;
            item->pos.x_rot = 0;
            item->pos.z_rot = 0;
            g_Lara.head_x_rot = 0;
            g_Lara.head_y_rot = 0;
            g_Lara.torso_x_rot = 0;
            g_Lara.torso_y_rot = 0;
            UpdateLaraRoom(item, -LARA_HITE / 2);
            Sound_Effect(SFX_LARA_BREATH, &item->pos, SPM_ALWAYS);
        }
		else
		{
            g_Lara.water_status = LWS_ABOVEWATER;
            g_Lara.gun_status = LGS_ARMLESS;
            item->current_anim_state = AS_FORWARDJUMP;
            item->goal_anim_state = AS_FORWARDJUMP;
            item->anim_number = AA_FALLDOWN;
            item->frame_number = AF_FALLDOWN;
            item->speed = item->fall_speed / 4;
            item->fall_speed = 0;
            item->gravity_status = 1;
            item->pos.x_rot = 0;
            item->pos.z_rot = 0;
            g_Lara.head_x_rot = 0;
            g_Lara.head_y_rot = 0;
            g_Lara.torso_x_rot = 0;
            g_Lara.torso_y_rot = 0;
        }
		
    }
	else if (g_Lara.water_status == LWS_SURFACE && !room_submerged)
	{
        g_Lara.water_status = LWS_ABOVEWATER;
        g_Lara.gun_status = LGS_ARMLESS;
        item->current_anim_state = AS_FORWARDJUMP;
        item->goal_anim_state = AS_FORWARDJUMP;
        item->anim_number = AA_FALLDOWN;
        item->frame_number = AF_FALLDOWN;
        item->speed = item->fall_speed / 4;
        item->fall_speed = 0;
        item->gravity_status = 1;
        item->pos.x_rot = 0;
        item->pos.z_rot = 0;
        g_Lara.head_x_rot = 0;
        g_Lara.head_y_rot = 0;
        g_Lara.torso_x_rot = 0;
        g_Lara.torso_y_rot = 0;
    }


    switch (g_Lara.water_status)
	{
    case LWS_ABOVEWATER:
        g_Lara.air = LARA_AIR;
        LaraAboveWater(item, &coll);
        break;

    case LWS_UNDERWATER:
        if (item->hit_points >= 0)
		{
            g_Lara.air--;
            if (g_Lara.air < 0)
			{
                g_Lara.air = -1;
                item->hit_points -= 5;
            }
        }
        LaraUnderWater(item, &coll);
        break;

    case LWS_SURFACE:
        if (item->hit_points >= 0)
		{
            g_Lara.air += 10;
            if (g_Lara.air > LARA_AIR)
			{
                g_Lara.air = LARA_AIR;
            }
        }
        LaraSurface(item, &coll);
        break;
	}

}

*/
int Draw_Phase_Game()
{
	int result = 0;

	S_InitialisePolyList();

	DrawRooms(g_Camera.pos.room_number);
	
	Overlay_DrawGameInfo();

	S_OutputPolyList();

	g_Camera.number_frames = S_DumpScreen();

	S_AnimateTextures(g_Camera.number_frames);

	return g_Camera.number_frames;

}

void S_InitialisePolyList()
{
	sort3dptr = (int32_t*) sort3d_buffer;	
	info3dptr = (int16_t*) info3d_buffer;    

	surfacenum = 0;

}

/*
void ChaseCamera(ITEM_INFO *item)
{
    GAME_VECTOR ideal;

    g_Camera.target_elevation += item->pos.x_rot;

    if (g_Camera.target_elevation > MAX_ELEVATION)
	{
        g_Camera.target_elevation = MAX_ELEVATION;
    }
	else if (g_Camera.target_elevation < -MAX_ELEVATION)
	{
        g_Camera.target_elevation = -MAX_ELEVATION;
    }

    int32_t distance = g_Camera.target_distance * phd_cos(g_Camera.target_elevation) >> W2V_SHIFT;
    ideal.y = g_Camera.target.y + (g_Camera.target_distance * phd_sin(g_Camera.target_elevation) >> W2V_SHIFT);

    g_Camera.target_square = SQUARE(distance);

    PHD_ANGLE angle = item->pos.y_rot + g_Camera.target_angle;
    ideal.x = g_Camera.target.x - (distance * phd_sin(angle) >> W2V_SHIFT);
    ideal.z = g_Camera.target.z - (distance * phd_cos(angle) >> W2V_SHIFT);
    ideal.room_number = g_Camera.pos.room_number;

    SmartShift(&ideal, ShiftCamera);

    if (g_Camera.fixed_camera)
	{
        MoveCamera(&ideal, g_Camera.speed);
    }
	else
	{
        MoveCamera(&ideal, CHASE_SPEED);
    }
}
*/

/*

void MoveCamera(GAME_VECTOR *ideal, int32_t speed)
{
    g_Camera.pos.x += (ideal->x - g_Camera.pos.x) / speed;
    g_Camera.pos.z += (ideal->z - g_Camera.pos.z) / speed;
    g_Camera.pos.y += (ideal->y - g_Camera.pos.y) / speed;
    g_Camera.pos.room_number = ideal->room_number;

    g_ChunkyFlag = false;

    FLOOR_INFO *floor = GetFloor(
        g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z,
        &g_Camera.pos.room_number);
    int32_t height =
        GetHeight(floor, g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z)
        - GROUND_SHIFT;

    if (g_Camera.pos.y >= height && ideal->y >= height) {
        LOS(&g_Camera.target, &g_Camera.pos);
        floor = GetFloor(
            g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z,
            &g_Camera.pos.room_number);
        height =
            GetHeight(floor, g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z)
            - GROUND_SHIFT;
    }

    int32_t ceiling =
        GetCeiling(floor, g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z)
        + GROUND_SHIFT;
    if (height < ceiling) {
        ceiling = (height + ceiling) >> 1;
        height = ceiling;
    }

    if (g_Camera.bounce) {
        if (g_Camera.bounce > 0) {
            g_Camera.pos.y += g_Camera.bounce;
            g_Camera.target.y += g_Camera.bounce;
            g_Camera.bounce = 0;
        } else {
            int32_t shake;
            shake = (Random_GetControl() - 0x4000) * g_Camera.bounce / 0x7FFF;
            g_Camera.pos.x += shake;
            g_Camera.target.y += shake;
            shake = (Random_GetControl() - 0x4000) * g_Camera.bounce / 0x7FFF;
            g_Camera.pos.y += shake;
            g_Camera.target.y += shake;
            shake = (Random_GetControl() - 0x4000) * g_Camera.bounce / 0x7FFF;
            g_Camera.pos.z += shake;
            g_Camera.target.z += shake;
            g_Camera.bounce += 5;
        }
    }

    if (g_Camera.pos.y > height) {
        g_Camera.shift = height - g_Camera.pos.y;
    } else if (g_Camera.pos.y < ceiling) {
        g_Camera.shift = ceiling - g_Camera.pos.y;
    } else {
        g_Camera.shift = 0;
    }

    GetFloor(
        g_Camera.pos.x, g_Camera.pos.y + g_Camera.shift, g_Camera.pos.z,
        &g_Camera.pos.room_number);

    phd_LookAt(
        g_Camera.pos.x, g_Camera.pos.y + g_Camera.shift, g_Camera.pos.z,
        g_Camera.target.x, g_Camera.target.y, g_Camera.target.z, 0);

    g_Camera.actual_angle = phd_atan(
        g_Camera.target.z - g_Camera.pos.z, g_Camera.target.x - g_Camera.pos.x);
}

*/

void DrawRooms(int16_t current_room)
{
	ROOM_INFO	*r;

	CurrentRoom = current_room;

	r = &g_RoomInfo[current_room];

	g_PhdLeft = 0;
	g_PhdTop = 0;
	g_PhdRight = Screen_GetResWidth() - 1;
	g_PhdBottom = Screen_GetResHeight() - 1;
	
	r->left = g_PhdLeft;
	r->top = g_PhdTop;
	r->right = g_PhdRight;
	r->bottom = g_PhdBottom;
	
	r->bound_active = 1;

	g_CameraUnderwater = r->flags & RF_UNDERWATER;

	g_RoomsToDrawCount = 0;

	g_RoomsToDraw[g_RoomsToDrawCount++] = current_room;

	GetRoomBounds(current_room);

	Clear_BackBuffer();

	static int prev_cam = 0;

	if(g_CameraUnderwater && (prev_cam != g_CameraUnderwater) )
	{
		prev_cam = g_CameraUnderwater;
		Create_Water_Palette();
	}
	if(!g_CameraUnderwater && (prev_cam != g_CameraUnderwater) )
	{
		prev_cam = g_CameraUnderwater;
		Create_Normal_Palette();
	}
	
	if (g_Objects[O_LARA].loaded)
	{
        if (g_RoomInfo[g_LaraItem->room_number].flags & RF_UNDERWATER)
		{
            SetupBelowWater(g_CameraUnderwater);
        }
		else
		{
            SetupAboveWater(g_CameraUnderwater);
        }
        DrawLara(g_LaraItem);
    }
	
	for (int i = 0; i < g_RoomsToDrawCount; i++)
	{
        PrintRooms(g_RoomsToDraw[i]);
    }

	
}

void S_OutputPolyList()
{
	phd_SortPolyList(surfacenum,sort3d_buffer);
	phd_PrintPolyList(dibdc->surface);

}

int32_t S_DumpScreen()
{

	int	nframes;

	nframes = Sync();

	while (nframes < 2)	
	{
		while (!Sync());
			nframes++;
	}

	//ScreenPartialDump()
	Present_BackBuffer();

	return nframes;
}

/*
void SmartShift(
    GAME_VECTOR *ideal,
    void (*shift)(
        int32_t *x, int32_t *y, int32_t target_x, int32_t target_y,
        int32_t left, int32_t top, int32_t right, int32_t bottom))
{
    LOS(&g_Camera.target, ideal);

    ROOM_INFO *r = &g_RoomInfo[g_Camera.target.room_number];
    int32_t x_floor = (g_Camera.target.z - r->z) >> WALL_SHIFT;
    int32_t y_floor = (g_Camera.target.x - r->x) >> WALL_SHIFT;

    int16_t item_box = r->floor[x_floor + y_floor * r->x_size].box;
    BOX_INFO *box = &g_Boxes[item_box];

    r = &g_RoomInfo[ideal->room_number];
    x_floor = (ideal->z - r->z) >> WALL_SHIFT;
    y_floor = (ideal->x - r->x) >> WALL_SHIFT;

    int16_t camera_box = r->floor[x_floor + y_floor * r->x_size].box;
    if (camera_box != NO_BOX
        && (ideal->z < box->left || ideal->z > box->right || ideal->x < box->top
            || ideal->x > box->bottom)) {
        box = &g_Boxes[camera_box];
    }

    int32_t left = box->left;
    int32_t right = box->right;
    int32_t top = box->top;
    int32_t bottom = box->bottom;

    int32_t test = (ideal->z - WALL_L) | (WALL_L - 1);
    int32_t bad_left =
        BadPosition(ideal->x, ideal->y, test, ideal->room_number);
    if (!bad_left) {
        camera_box = r->floor[x_floor - 1 + y_floor * r->x_size].box;
        if (camera_box != NO_ITEM && g_Boxes[camera_box].left < left) {
            left = g_Boxes[camera_box].left;
        }
    }

    test = (ideal->z + WALL_L) & (~(WALL_L - 1));
    int32_t bad_right =
        BadPosition(ideal->x, ideal->y, test, ideal->room_number);
    if (!bad_right) {
        camera_box = r->floor[x_floor + 1 + y_floor * r->x_size].box;
        if (camera_box != NO_ITEM && g_Boxes[camera_box].right > right) {
            right = g_Boxes[camera_box].right;
        }
    }

    test = (ideal->x - WALL_L) | (WALL_L - 1);
    int32_t bad_top = BadPosition(test, ideal->y, ideal->z, ideal->room_number);
    if (!bad_top) {
        camera_box = r->floor[x_floor + (y_floor - 1) * r->x_size].box;
        if (camera_box != NO_ITEM && g_Boxes[camera_box].top < top) {
            top = g_Boxes[camera_box].top;
        }
    }

    test = (ideal->x + WALL_L) & (~(WALL_L - 1));
    int32_t bad_bottom =
        BadPosition(test, ideal->y, ideal->z, ideal->room_number);
    if (!bad_bottom) {
        camera_box = r->floor[x_floor + (y_floor + 1) * r->x_size].box;
        if (camera_box != NO_ITEM && g_Boxes[camera_box].bottom > bottom) {
            bottom = g_Boxes[camera_box].bottom;
        }
    }

    left += STEP_L;
    right -= STEP_L;
    top += STEP_L;
    bottom -= STEP_L;

    int32_t noclip = 1;
    if (ideal->z < left && bad_left) {
        noclip = 0;
        if (ideal->x < g_Camera.target.x) {
            shift(
                &ideal->z, &ideal->x, g_Camera.target.z, g_Camera.target.x,
                left, top, right, bottom);
        } else {
            shift(
                &ideal->z, &ideal->x, g_Camera.target.z, g_Camera.target.x,
                left, bottom, right, top);
        }
    } else if (ideal->z > right && bad_right) {
        noclip = 0;
        if (ideal->x < g_Camera.target.x) {
            shift(
                &ideal->z, &ideal->x, g_Camera.target.z, g_Camera.target.x,
                right, top, left, bottom);
        } else {
            shift(
                &ideal->z, &ideal->x, g_Camera.target.z, g_Camera.target.x,
                right, bottom, left, top);
        }
    }

    if (noclip) {
        if (ideal->x < top && bad_top) {
            noclip = 0;
            if (ideal->z < g_Camera.target.z) {
                shift(
                    &ideal->x, &ideal->z, g_Camera.target.x, g_Camera.target.z,
                    top, left, bottom, right);
            } else {
                shift(
                    &ideal->x, &ideal->z, g_Camera.target.x, g_Camera.target.z,
                    top, right, bottom, left);
            }
        } else if (ideal->x > bottom && bad_bottom) {
            noclip = 0;
            if (ideal->z < g_Camera.target.z) {
                shift(
                    &ideal->x, &ideal->z, g_Camera.target.x, g_Camera.target.z,
                    bottom, left, top, right);
            } else {
                shift(
                    &ideal->x, &ideal->z, g_Camera.target.x, g_Camera.target.z,
                    bottom, right, top, left);
            }
        }
    }

    if (!noclip) {
        GetFloor(ideal->x, ideal->y, ideal->z, &ideal->room_number);
    }
}

void ShiftCamera(
    int32_t *x, int32_t *y, int32_t target_x, int32_t target_y, int32_t left,
    int32_t top, int32_t right, int32_t bottom)
{
    int32_t shift;

    int32_t TL_square = SQUARE(target_x - left) + SQUARE(target_y - top);
    int32_t BL_square = SQUARE(target_x - left) + SQUARE(target_y - bottom);
    int32_t TR_square = SQUARE(target_x - right) + SQUARE(target_y - top);

    if (g_Camera.target_square < TL_square) {
        *x = left;
        shift = g_Camera.target_square - SQUARE(target_x - left);
        if (shift < 0) {
            return;
        }

        shift = phd_sqrt(shift);
        *y = target_y + ((top < bottom) ? -shift : shift);
    } else if (TL_square > MIN_SQUARE) {
        *x = left;
        *y = top;
    } else if (g_Camera.target_square < BL_square) {
        *x = left;
        shift = g_Camera.target_square - SQUARE(target_x - left);
        if (shift < 0) {
            return;
        }

        shift = phd_sqrt(shift);
        *y = target_y + ((top < bottom) ? shift : -shift);
    } else if (BL_square > MIN_SQUARE) {
        *x = left;
        *y = bottom;
    } else if (g_Camera.target_square < TR_square) {
        shift = g_Camera.target_square - SQUARE(target_y - top);
        if (shift < 0) {
            return;
        }

        shift = phd_sqrt(shift);
        *x = target_x + ((left < right) ? shift : -shift);
        *y = top;
    } else {
        *x = right;
        *y = top;
    }
}
*/
/*
int32_t zLOS(GAME_VECTOR *start, GAME_VECTOR *target)
{
    FLOOR_INFO *floor;

    int32_t dz = target->z - start->z;
    if (dz == 0) {
        return 1;
    }

    int32_t dx = ((target->x - start->x) << WALL_SHIFT) / dz;
    int32_t dy = ((target->y - start->y) << WALL_SHIFT) / dz;

    int16_t room_num = start->room_number;
    int16_t last_room;

    if (dz < 0) {
        int32_t z = start->z & ~(WALL_L - 1);
        int32_t x = start->x + ((dx * (z - start->z)) >> WALL_SHIFT);
        int32_t y = start->y + ((dy * (z - start->z)) >> WALL_SHIFT);

        while (z > target->z) {
            floor = GetFloor(x, y, z, &room_num);
            if (y > GetHeight(floor, x, y, z)
                || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_num;
                return -1;
            }

            last_room = room_num;

            floor = GetFloor(x, y, z - 1, &room_num);
            if (y > GetHeight(floor, x, y, z - 1)
                || y < GetCeiling(floor, x, y, z - 1)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }

            z -= WALL_L;
            x -= dx;
            y -= dy;
        }
    } else {
        int32_t z = start->z | (WALL_L - 1);
        int32_t x = start->x + ((dx * (z - start->z)) >> WALL_SHIFT);
        int32_t y = start->y + ((dy * (z - start->z)) >> WALL_SHIFT);

        while (z < target->z) {
            floor = GetFloor(x, y, z, &room_num);
            if (y > GetHeight(floor, x, y, z)
                || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_num;
                return -1;
            }

            last_room = room_num;

            floor = GetFloor(x, y, z + 1, &room_num);
            if (y > GetHeight(floor, x, y, z + 1)
                || y < GetCeiling(floor, x, y, z + 1)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }

            z += WALL_L;
            x += dx;
            y += dy;
        }
    }

    target->room_number = room_num;
    return 1;
}

int32_t xLOS(GAME_VECTOR *start, GAME_VECTOR *target)
{
    FLOOR_INFO *floor;

    int32_t dx = target->x - start->x;
    if (dx == 0) {
        return 1;
    }

    int32_t dy = ((target->y - start->y) << WALL_SHIFT) / dx;
    int32_t dz = ((target->z - start->z) << WALL_SHIFT) / dx;

    int16_t room_num = start->room_number;
    int16_t last_room;

    if (dx < 0) {
        int32_t x = start->x & ~(WALL_L - 1);
        int32_t y = start->y + ((dy * (x - start->x)) >> WALL_SHIFT);
        int32_t z = start->z + ((dz * (x - start->x)) >> WALL_SHIFT);

        while (x > target->x) {
            floor = GetFloor(x, y, z, &room_num);
            if (y > GetHeight(floor, x, y, z)
                || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_num;
                return -1;
            }

            last_room = room_num;

            floor = GetFloor(x - 1, y, z, &room_num);
            if (y > GetHeight(floor, x - 1, y, z)
                || y < GetCeiling(floor, x - 1, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }

            x -= WALL_L;
            y -= dy;
            z -= dz;
        }
    } else {
        int32_t x = start->x | (WALL_L - 1);
        int32_t y = start->y + ((dy * (x - start->x)) >> WALL_SHIFT);
        int32_t z = start->z + ((dz * (x - start->x)) >> WALL_SHIFT);

        while (x < target->x) {
            floor = GetFloor(x, y, z, &room_num);
            if (y > GetHeight(floor, x, y, z)
                || y < GetCeiling(floor, x, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = room_num;
                return -1;
            }

            last_room = room_num;

            floor = GetFloor(x + 1, y, z, &room_num);
            if (y > GetHeight(floor, x + 1, y, z)
                || y < GetCeiling(floor, x + 1, y, z)) {
                target->x = x;
                target->y = y;
                target->z = z;
                target->room_number = last_room;
                return 0;
            }

            x += WALL_L;
            y += dy;
            z += dz;
        }
    }

    target->room_number = room_num;
    return 1;
}


int32_t LOS(GAME_VECTOR *start, GAME_VECTOR *target)
{
    int32_t los1;
    int32_t los2;

    if (ABS(target->z - start->z) > ABS(target->x - start->x)) {
        los1 = xLOS(start, target);
        los2 = zLOS(start, target);
    } else {
        los1 = zLOS(start, target);
        los2 = xLOS(start, target);
    }

    if (!los2) {
        return 0;
    }

    FLOOR_INFO *floor =
        GetFloor(target->x, target->y, target->z, &target->room_number);

    if (ClipTarget(start, target, floor) && los1 == 1 && los2 == 1) {
        return 1;
    }

    return 0;
}
*/

void GetRoomBounds(int16_t room_num)
{
    ROOM_INFO *r = &g_RoomInfo[room_num];
    phd_PushMatrix();
    phd_TranslateAbs(r->x, r->y, r->z);
    if (r->doors) {
        for (int i = 0; i < r->doors->count; i++) {
            DOOR_INFO *door = &r->doors->door[i];
            if (SetRoomBounds(&door->x, door->room_num, r)) {
                GetRoomBounds(door->room_num);
            }
        }
    }
    phd_PopMatrix();
}

int32_t SetRoomBounds(int16_t *objptr, int16_t room_num, ROOM_INFO *parent)
{
    // XXX: the way the game passes the objptr is dangerous and relies on
    // layout of DOOR_INFO

    if ((objptr[0] * (parent->x + objptr[3] - g_W2VMatrix._03))
            + (objptr[1] * (parent->y + objptr[4] - g_W2VMatrix._13))
            + (objptr[2] * (parent->z + objptr[5] - g_W2VMatrix._23))
        >= 0) {
        return 0;
    }

    DOOR_VBUF door_vbuf[4];
    int32_t left = parent->right;
    int32_t right = parent->left;
    int32_t top = parent->bottom;
    int32_t bottom = parent->top;

    objptr += 3;
    int32_t z_toofar = 0;
    int32_t z_behind = 0;

    const PHD_MATRIX *mptr = g_PhdMatrixPtr;
    for (int i = 0; i < 4; i++)
	{
        int32_t xv = mptr->_00 * objptr[0] + mptr->_01 * objptr[1]
            + mptr->_02 * objptr[2] + mptr->_03;
        int32_t yv = mptr->_10 * objptr[0] + mptr->_11 * objptr[1]
            + mptr->_12 * objptr[2] + mptr->_13;
        int32_t zv = mptr->_20 * objptr[0] + mptr->_21 * objptr[1]
            + mptr->_22 * objptr[2] + mptr->_23;
        door_vbuf[i].xv = xv;
        door_vbuf[i].yv = yv;
        door_vbuf[i].zv = zv;
        objptr += 3;

        if (zv > 0)
		{
            //if (zv > (g_DrawDistMax << W2V_SHIFT ))
			//if (zv > (DRAW_DIST_MAX << W2V_SHIFT ))
			if (zv > (DRAW_DIST_FADE  << W2V_SHIFT))
			{
                z_toofar++;
            }

            zv /= g_PhdPersp;
            int32_t xs, ys;
            if (zv) {
                xs = ViewPort_GetCenterX() + xv / zv;
                ys = ViewPort_GetCenterY() + yv / zv;
            }
			else
			{
                xs = xv >= 0 ? g_PhdRight : g_PhdLeft;
                ys = yv >= 0 ? g_PhdBottom : g_PhdTop;
            }

            if (xs < left)
			{
                left = xs;
            }
            if (xs > right)
			{
                right = xs;
            }
            if (ys < top)
			{
                top = ys;
            }
            if (ys > bottom)
			{
                bottom = ys;
            }
        }
		else
		{
            z_behind++;
        }
    }

    if (z_behind == 4 || z_toofar == 4)
	{
        return 0;
    }

    if (z_behind > 0)
	{
        DOOR_VBUF *dest = &door_vbuf[0];
        DOOR_VBUF *last = &door_vbuf[3];
        for (int i = 0; i < 4; i++) {
            if ((dest->zv < 0) ^ (last->zv < 0))
			{
                if (dest->xv < 0 && last->xv < 0)
				{
                    left = 0;
                }
				else if (dest->xv > 0 && last->xv > 0)
				{
                    right = g_PhdRight;
                }
				else
				{
                    left = 0;
                    right = g_PhdRight;
                }

                if (dest->yv < 0 && last->yv < 0)
				{
                    top = 0;
                }
				else if (dest->yv > 0 && last->yv > 0)
				{
                    bottom = g_PhdBottom;
                }
				else
				{
                    top = 0;
                    bottom = g_PhdBottom;
                }
            }

            last = dest;
            dest++;
        }
    }

    if (left < parent->left)
	{
        left = parent->left;
    }
    if (right > parent->right)
	{
        right = parent->right;
    }
    if (top < parent->top)
	{
        top = parent->top;
    }
    if (bottom > parent->bottom)
	{
        bottom = parent->bottom;
    }

    if (left >= right || top >= bottom)
	{
        return 0;
    }

    ROOM_INFO *r = &g_RoomInfo[room_num];
    if (left < r->left)
	{
        r->left = left;
    }
    if (top < r->top)
	{
        r->top = top;
    }
    if (right > r->right)
	{
        r->right = right;
    }
    if (bottom > r->bottom)
	{
        r->bottom = bottom;
    }

    if (!r->bound_active)
	{
        if (g_RoomsToDrawCount + 1 < MAX_ROOMS_TO_DRAW)
		{
            g_RoomsToDraw[g_RoomsToDrawCount++] = room_num;
        }
        r->bound_active = 1;
    }
    return 1;
}

void SetupAboveWater(int underwater)
{
	
    g_IsWaterEffect = false;
    g_IsWibbleEffect = underwater;
    g_IsShadeEffect = underwater;

	
}

void SetupBelowWater(int underwater)
{
    g_IsWaterEffect = true;
    g_IsWibbleEffect = !underwater;
    g_IsShadeEffect = true;

	

}


void PrintRooms(int16_t room_number)
{
    ROOM_INFO *r = &g_RoomInfo[room_number];
    
	if (r->flags & RF_UNDERWATER) {
        SetupBelowWater(g_CameraUnderwater);
    }
	else
	{
        SetupAboveWater(g_CameraUnderwater);
    }

    r->bound_active = 0;

    phd_PushMatrix();
    phd_TranslateAbs(r->x, r->y, r->z);

    g_PhdLeft = r->left;
    g_PhdRight = r->right;
    g_PhdTop = r->top;
    g_PhdBottom = r->bottom;

    DrawRoom(r->data);

	for (int i = r->item_number; i != NO_ITEM; i = g_Items[i].next_item)
	{
        ITEM_INFO *item = &g_Items[i];

        if (item->status != IS_INVISIBLE)
		{
			g_Objects[item->object_number].draw_routine(item);
        }
	}

    for (int i = 0; i < r->num_static_meshes; i++)
	{
        MESH_INFO *mesh = (MESH_INFO*)&r->static_mesh[i];

		if (g_StaticObjects[mesh->static_number].flags & 2)
		{
            phd_PushMatrix();
            phd_TranslateAbs(mesh->x, mesh->y, mesh->z);
            phd_RotY(mesh->y_rot);
            int clip =
                S_GetObjectBounds(&g_StaticObjects[mesh->static_number].x_minp);
            if (clip)
			{
                Output_CalculateStaticLight(mesh->shade);
                Output_DrawPolygons(
                    g_Meshes[g_StaticObjects[mesh->static_number].mesh_number],
                    clip);
            }
            phd_PopMatrix();
        }
    }

	
    for (int i = r->fx_number; i != NO_ITEM; i = g_Effects[i].next_fx)
	{
        DrawEffect(i);
    }

    phd_PopMatrix();

    r->left = Screen_GetResWidth() - 1;
    r->bottom = 0;
    r->right = 0;
    r->top = Screen_GetResHeight() - 1;

}
/*
int32_t BadPosition(int32_t x, int32_t y, int32_t z, int16_t room_num)
{
    FLOOR_INFO *floor = GetFloor(x, y, z, &room_num);
    if (y >= GetHeight(floor, x, y, z) || y <= GetCeiling(floor, x, y, z)) {
        return 1;
    }
    return 0;
}
*/

/*
int32_t ClipTarget(GAME_VECTOR *start, GAME_VECTOR *target, FLOOR_INFO *floor)
{
    int32_t dx = target->x - start->x;
    int32_t dy = target->y - start->y;
    int32_t dz = target->z - start->z;

    int32_t height = GetHeight(floor, target->x, target->y, target->z);
    if (target->y > height && start->y < height) {
        target->y = height;
        target->x = start->x + dx * (height - start->y) / dy;
        target->z = start->z + dz * (height - start->y) / dy;
        return 0;
    }

    int32_t ceiling = GetCeiling(floor, target->x, target->y, target->z);
    if (target->y < ceiling && start->y > ceiling) {
        target->y = ceiling;
        target->x = start->x + dx * (ceiling - start->y) / dy;
        target->z = start->z + dz * (ceiling - start->y) / dy;
        return 0;
    }

    return 1;
}
*/
void DrawRoom(int16_t *obj_ptr)
{
	/*
	g_Float_Phd_Left = (float) g_PhdLeft;
	g_Float_Phd_Right = (float) g_PhdRight;
	g_Float_Phd_Top = (float) g_PhdTop;
	g_Float_Phd_Bottom = (float) g_PhdBottom;
	*/

	//*obj_ptr its count of data
	//obj_ptr + 1 its data itself
	
	
    obj_ptr = CalcRoomVertices(obj_ptr);
	obj_ptr = DrawObjectGT4(obj_ptr + 1, *obj_ptr);
	obj_ptr = DrawObjectGT3(obj_ptr + 1, *obj_ptr);
	/*
    obj_ptr = DrawRoomSprites(obj_ptr + 1, *obj_ptr);
	*/

	//draw objects

	//draw static objects
}

/*

void LaraBaddieCollision(ITEM_INFO *lara_item, COLL_INFO *coll)
{
    lara_item->hit_status = 0;
    g_Lara.hit_direction = -1;
    if (lara_item->hit_points <= 0) {
        return;
    }

    int16_t numroom = 0;
    int16_t roomies[MAX_BADDIE_COLLISION];

    roomies[numroom++] = lara_item->room_number;

    DOOR_INFOS *door = g_RoomInfo[lara_item->room_number].doors;
    if (door) {
        for (int i = 0; i < door->count; i++) {
            if (numroom >= MAX_BADDIE_COLLISION) {
                break;
            }
            roomies[numroom++] = door->door[i].room_num;
        }
    }

    for (int i = 0; i < numroom; i++) {
        int16_t item_num = g_RoomInfo[roomies[i]].item_number;
        while (item_num != NO_ITEM) {
            ITEM_INFO *item = &g_Items[item_num];
            if (item->collidable && item->status != IS_INVISIBLE) {
                OBJECT_INFO *object = &g_Objects[item->object_number];
                if (object->collision) {
                    int32_t x = lara_item->pos.x - item->pos.x;
                    int32_t y = lara_item->pos.y - item->pos.y;
                    int32_t z = lara_item->pos.z - item->pos.z;
                    if (x > -TARGET_DIST && x < TARGET_DIST && y > -TARGET_DIST
                        && y < TARGET_DIST && z > -TARGET_DIST
                        && z < TARGET_DIST) {
                        object->collision(item_num, lara_item, coll);
                    }
                }
            }
            item_num = item->next_item;
        }
    }


    if (g_Lara.spaz_effect_count)
	{
        //EffectSpaz(lara_item, coll);
    }

    if (g_Lara.hit_direction == -1)
	{
        g_Lara.hit_frame = 0;
    }

    //g_InvChosen = -1;
}
*/
/*
void ShiftItem(ITEM_INFO *item, COLL_INFO *coll)
{
    item->pos.x += coll->shift.x;
    item->pos.y += coll->shift.y;
    item->pos.z += coll->shift.z;
    coll->shift.x = 0;
    coll->shift.y = 0;
    coll->shift.z = 0;
}
*/

int32_t CalcFogShade(int32_t depth)
{
    /*
    int32_t fog_begin = DRAW_DIST_FADE;
    int32_t fog_end = DRAW_DIST_MAX;
	*/

	int32_t fog_begin = DRAW_DIST_MAX; //0x5000 my - 22528 git value
	int32_t fog_end = DRAW_DIST_FADE; //0x8000 my - 30720 git value


    if (depth < fog_begin) {
        return 0;
    }
    if (depth >= fog_end) {
        return 0x1FFF;
    }

    return (depth - fog_begin) * 0x1FFF / (fog_end - fog_begin);
}

/*
int32_t CalculateTarget(PHD_VECTOR *target, ITEM_INFO *item, LOT_INFO *LOT)
{
    int32_t left = 0;
    int32_t right = 0;
    int32_t top = 0;
    int32_t bottom = 0;

    UpdateLOT(LOT, MAX_EXPANSION);

    target->x = item->pos.x;
    target->y = item->pos.y;
    target->z = item->pos.z;

    int32_t box_number = item->box_number;
    if (box_number == NO_BOX) {
        return TARGET_NONE;
    }

    BOX_INFO *box;
    int32_t prime_free = ALL_CLIP;
    do {
        box = &g_Boxes[box_number];

        if (LOT->fly) {
            if (target->y > box->height - WALL_L) {
                target->y = box->height - WALL_L;
            }
        } else {
            if (target->y > box->height) {
                target->y = box->height;
            }
        }

        if (item->pos.z >= box->left && item->pos.z <= box->right
            && item->pos.x >= box->top && item->pos.x <= box->bottom) {
            left = box->left;
            right = box->right;
            top = box->top;
            bottom = box->bottom;
        } else {
            if (item->pos.z < box->left) {
                if ((prime_free & CLIP_LEFT) && item->pos.x >= box->top
                    && item->pos.x <= box->bottom) {
                    if (target->z < box->left + BIFF) {
                        target->z = box->left + BIFF;
                    }

                    if (prime_free & SECONDARY_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    if (box->top > top) {
                        top = box->top;
                    }
                    if (box->bottom < bottom) {
                        bottom = box->bottom;
                    }

                    prime_free = CLIP_LEFT;
                } else if (prime_free != CLIP_LEFT) {
                    target->z = right - BIFF;
                    if (prime_free != ALL_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    prime_free |= SECONDARY_CLIP;
                }
            } else if (item->pos.z > box->right) {
                if ((prime_free & CLIP_RIGHT) && item->pos.x >= box->top
                    && item->pos.x <= box->bottom) {
                    if (target->z > box->right - BIFF) {
                        target->z = box->right - BIFF;
                    }

                    if (prime_free & SECONDARY_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    if (box->top > top) {
                        top = box->top;
                    }
                    if (box->bottom < bottom) {
                        bottom = box->bottom;
                    }

                    prime_free = CLIP_RIGHT;
                } else if (prime_free != CLIP_RIGHT) {
                    target->z = left + BIFF;
                    if (prime_free != ALL_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    prime_free |= SECONDARY_CLIP;
                }
            }

            if (item->pos.x < box->top) {
                if ((prime_free & CLIP_TOP) && item->pos.z >= box->left
                    && item->pos.z <= box->right) {
                    if (target->x < box->top + BIFF) {
                        target->x = box->top + BIFF;
                    }

                    if (prime_free & SECONDARY_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    if (box->left > left) {
                        left = box->left;
                    }
                    if (box->right < right) {
                        right = box->right;
                    }

                    prime_free = CLIP_TOP;
                } else if (prime_free != CLIP_TOP) {
                    target->x = bottom - BIFF;
                    if (prime_free != ALL_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    prime_free |= SECONDARY_CLIP;
                }
            } else if (item->pos.x > box->bottom) {
                if ((prime_free & CLIP_BOTTOM) && item->pos.z >= box->left
                    && item->pos.z <= box->right) {
                    if (target->x > box->bottom - BIFF) {
                        target->x = box->bottom - BIFF;
                    }

                    if (prime_free & SECONDARY_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    if (box->left > left) {
                        left = box->left;
                    }
                    if (box->right < right) {
                        right = box->right;
                    }

                    prime_free = CLIP_BOTTOM;
                } else if (prime_free != CLIP_BOTTOM) {
                    target->x = top + BIFF;
                    if (prime_free != ALL_CLIP) {
                        return TARGET_SECONDARY;
                    }

                    prime_free |= SECONDARY_CLIP;
                }
            }
        }

        if (box_number == LOT->target_box) {
            if (prime_free & (CLIP_LEFT | CLIP_RIGHT)) {
                target->z = LOT->target.z;
            } else if (!(prime_free & SECONDARY_CLIP)) {
                if (target->z < box->left + BIFF) {
                    target->z = box->left + BIFF;
                } else if (target->z > box->right - BIFF) {
                    target->z = box->right - BIFF;
                }
            }

            if (prime_free & (CLIP_TOP | CLIP_BOTTOM)) {
                target->x = LOT->target.x;
            } else if (!(prime_free & SECONDARY_CLIP)) {
                if (target->x < box->top + BIFF) {
                    target->x = box->top + BIFF;
                } else if (target->x > box->bottom - BIFF) {
                    target->x = box->bottom - BIFF;
                }
            }

            target->y = LOT->target.y;
            return TARGET_PRIMARY;
        }

        box_number = LOT->node[box_number].exit_box;
        if (box_number != NO_BOX
            && (g_Boxes[box_number].overlap_index & LOT->block_mask)) {
            break;
        }
    } while (box_number != NO_BOX);

    if (prime_free & (CLIP_LEFT | CLIP_RIGHT)) {
        target->z = box->left + WALL_L / 2
            + (Random_GetControl() * (box->right - box->left - WALL_L) >> 15);
    } else if (!(prime_free & SECONDARY_CLIP)) {
        if (target->z < box->left + BIFF) {
            target->z = box->left + BIFF;
        } else if (target->z > box->right - BIFF) {
            target->z = box->right - BIFF;
        }
    }

    if (prime_free & (CLIP_TOP | CLIP_BOTTOM)) {
        target->x = box->top + WALL_L / 2
            + (Random_GetControl() * (box->bottom - box->top - WALL_L) >> 15);
    } else if (!(prime_free & SECONDARY_CLIP)) {
        if (target->x < box->top + BIFF) {
            target->x = box->top + BIFF;
        } else if (target->x > box->bottom - BIFF) {
            target->x = box->bottom - BIFF;
        }
    }

    if (!LOT->fly) {
        target->y = box->height;
    } else {
        target->y = box->height - STEP_L * 3 / 2;
    }

    return TARGET_NONE;
}
*/


int32_t S_SaveGame(SAVEGAME_INFO *save, int32_t slot)
{
    char filename[80];
    sprintf(filename, g_GameFlow.save_game_fmt, slot);
    //LOG_DEBUG("%s", filename);

    FILE *fp = fopen(filename, "wb");
    
    if (!fp)
    {
        return 0;
    }

    for (int i = 0; i < g_GameFlow.level_count; i++)
    {
        if (g_GameFlow.levels[i].level_type == GFL_CURRENT)
        {
            save->start[i] = save->start[save->current_level];
        }
    }

    sprintf(filename, "%s", g_GameFlow.levels[g_SaveGame.current_level].level_title);
    fwrite(filename, sizeof(char), 75, fp);
    fwrite(&g_SaveCounter, sizeof(int32_t), 1, fp);

    if (!save->start)
	{
        //Shell_ExitSystem("null save->start");
        return 0;
    }
    fwrite(&save->start[0], sizeof(START_INFO), g_GameFlow.level_count, fp);
    fwrite(&save->timer, sizeof(uint32_t), 1, fp);
    fwrite(&save->kills, sizeof(uint32_t), 1, fp);
    fwrite(&save->secrets, sizeof(uint16_t), 1, fp);
    fwrite(&save->current_level, sizeof(uint16_t), 1, fp);
    fwrite(&save->pickups, sizeof(uint8_t), 1, fp);
    fwrite(&save->bonus_flag, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_pickup1, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_pickup2, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_puzzle1, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_puzzle2, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_puzzle3, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_puzzle4, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_key1, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_key2, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_key3, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_key4, sizeof(uint8_t), 1, fp);
    fwrite(&save->num_leadbar, sizeof(uint8_t), 1, fp);
    fwrite(&save->challenge_failed, sizeof(uint8_t), 1, fp);
    fwrite(&save->buffer[0], sizeof(char), MAX_SAVEGAME_BUFFER, fp);
    fclose(fp);

    REQUEST_INFO *req = &g_LoadSaveGameRequester;
    req->item_flags[slot] &= ~RIF_BLOCKED;

    sprintf(&req->item_texts[req->item_text_len * slot], "%s %d", filename, g_SaveCounter);
    g_SavedGamesCount++;
    g_SaveCounter++;
    return 1;
}

int32_t S_LoadGame(SAVEGAME_INFO *save, int32_t slot)
{
    char filename[80];
    sprintf(filename, g_GameFlow.save_game_fmt, slot);
    //LOG_DEBUG("%s", filename);
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return 0;
    }
    fread(filename, sizeof(char), 75, fp);
    int32_t counter;

    fread(&counter, sizeof(int32_t), 1, fp);

    if (!save->start)
	{
        //Shell_ExitSystem("null save->start");
        return 0;
    }
    fread(&save->start[0], sizeof(START_INFO), g_GameFlow.level_count, fp);
    fread(&save->timer, sizeof(uint32_t), 1, fp);
    fread(&save->kills, sizeof(uint32_t), 1, fp);
    fread(&save->secrets, sizeof(uint16_t), 1, fp);
    fread(&save->current_level, sizeof(uint16_t), 1, fp);
    fread(&save->pickups, sizeof(uint8_t), 1, fp);
    fread(&save->bonus_flag, sizeof(uint8_t), 1, fp);
    fread(&save->num_pickup1, sizeof(uint8_t), 1, fp);
    fread(&save->num_pickup2, sizeof(uint8_t), 1, fp);
    fread(&save->num_puzzle1, sizeof(uint8_t), 1, fp);
    fread(&save->num_puzzle2, sizeof(uint8_t), 1, fp);
    fread(&save->num_puzzle3, sizeof(uint8_t), 1, fp);
    fread(&save->num_puzzle4, sizeof(uint8_t), 1, fp);
    fread(&save->num_key1, sizeof(uint8_t), 1, fp);
    fread(&save->num_key2, sizeof(uint8_t), 1, fp);
    fread(&save->num_key3, sizeof(uint8_t), 1, fp);
    fread(&save->num_key4, sizeof(uint8_t), 1, fp);
    fread(&save->num_leadbar, sizeof(uint8_t), 1, fp);
    fread(&save->challenge_failed, sizeof(uint8_t), 1, fp);
    int val = fread(&save->buffer[0], sizeof(char), MAX_SAVEGAME_BUFFER, fp);
    fclose(fp);

    

    /*
    for (int i = 0; i < g_GameFlow.level_count; i++)
    {
        if (g_GameFlow.levels[i].level_type == GFL_CURRENT)
        {
            save->start[save->current_level] = save->start[i];
        }
    }
    */

    //save->start[save->current_level] = save->start[21];

    return 1;
}


void GetSavedGamesList(REQUEST_INFO *req)
{
    int32_t height = Screen_GetResHeight();

    if (height <= 200) {
        req->y = -32;
        req->vis_lines = 5;
    } else if (height <= 384) {
        req->y = -62;
        req->vis_lines = 8;
    } else if (height <= 480) {
        req->y = -90;
        req->vis_lines = 10;
    } else {
        req->y = -100;
        req->vis_lines = 12;
    }

    if (req->requested >= req->vis_lines) {
        req->line_offset = req->requested - req->vis_lines + 1;
    }
}

int32_t S_FrontEndCheck()
{
    REQUEST_INFO *req = &g_LoadSaveGameRequester;

    req->items = 0;
    g_SaveCounter = 0;
    g_SavedGamesCount = 0;
    for (int i = 0; i < MAX_SAVE_SLOTS; i++)
    {
        char filename[80];
        sprintf(filename, g_GameFlow.save_game_fmt, i);

        FILE *fp = fopen(filename, "rb");
        if (fp)
        {
            fread(filename, sizeof(char), 75, fp);
            int32_t counter = 0;
            fread(&counter, sizeof(int32_t), 1, fp);
            fclose(fp);

            req->item_flags[req->items] &= ~RIF_BLOCKED;

            sprintf(&req->item_texts[req->items * req->item_text_len], "%s %d", filename, counter);

            if (counter > g_SaveCounter)
            {
                g_SaveCounter = counter;
                req->requested = i;
            }

            g_SavedGamesCount++;



        }
        else
        {
            req->item_flags[req->items] |= RIF_BLOCKED;

            sprintf(&req->item_texts[req->items * req->item_text_len], g_GameFlow.strings[GS_MISC_EMPTY_SLOT_FMT], i + 1);
        }

        req->items++;
    }

    g_SaveCounter++;

    return 1;
}
