#pragma once

//#include "util.h"

#define CLIP_VERTCOUNT_SCALE 4

#define INT16_MAX_TR 32767
#define INT16_MIN_TR -32768

#define MAX_BADDIE_COLLISION 12

//#define DRAW_DIST_MAX 0x5000
#define DRAW_DIST_MAX 0x7000

//#define DRAW_DIST_FADE 0x3000
#define DRAW_DIST_FADE 0x5000

#define HAIR_SEGMENTS 6

#define ABS(x) (((x) < 0) ? (-(x)) : (x))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))

#define Z_NEAR (20 << W2V_SHIFT)
//#define Z_NEAR (127 << W2V_SHIFT)

// 20 * 1024 = 0x5000
//#define DRAW_DISTANCE_MAX ((20 * 1024) << W2V_SHIFT)
#define DRAW_DISTANCE_MAX 0x5000 << W2V_SHIFT
#define DRAW_DISTANCE_MIN 20 << W2V_SHIFT
//#define DRAW_DISTANCE_MIN  (127 << W2V_SHIFT)

//#define MAX_SOUND_SAMPLES 370

#define SQUARE(A) ((A) * (A))

//#define Center_X (SCREEN_WIDTH / 2)
//#define Center_Y (SCREEN_HEIGHT / 2)

//#define WIEVPORT_CENTER_X (SCREEN_WIDTH / 2)
//#define WIEVPORT_CENTER_Y (SCREEN_HEIGHT / 2)

#define PHD_ONE 0x10000
#define PHD_DEGREE (PHD_ONE / 360) // = 182
#define PHD_360 (PHD_ONE)		   // = 65536 = 0x10000
#define PHD_180 (PHD_ONE / 2)	  // = 32768 = 0x8000
#define PHD_90 (PHD_ONE / 4)	   // = 16384 = 0x4000
#define PHD_45 (PHD_ONE / 8)	   // = 8192 = 0x2000
#define PHD_135 (PHD_45 * 3)	   // = 24576 = 0x6000

#define MIN_UI_SCALE 0.5f
#define MAX_UI_SCALE 2.0f
#define DEFAULT_UI_SCALE 1.0f

#define MIN_BRIGHTNESS 0.1f
#define MAX_BRIGHTNESS 2.0f
#define DEFAULT_BRIGHTNESS 1.0f

#define FRAMES_PER_SECOND 30
#define TICKS_PER_FRAME 2
#define TICKS_PER_SECOND (FRAMES_PER_SECOND * TICKS_PER_FRAME)

#define MAX_MATRICES 40
#define MAX_NESTED_MATRICES 32
#define MAX_REQLINES 16
//#define MAX_PLAYING_FX 24
#define MAX_PLAYING_FX 255 //������ � ��� sound.cpp
#define MAX_AMBIENT_FX 8
#define MAX_SAMPLES 256
#define MAX_SAVE_SLOTS 16
#define MAX_LEVEL_NAME_LENGTH 48
#define NUM_SLOTS 8
#define MAX_FRAMES 10
#define MAX_CD_TRACKS 64
#define MAX_TEXTURES 8192
#define MAX_TEXTPAGES 128
#define MAX_SPRITES 512
#define MAX_FLIP_MAPS 10
#define MAX_ROOMS_TO_DRAW 100
#define DEMO_COUNT_MAX 9000
#define MAX_ITEMS 10240
#define MAX_SECRETS 16
#define MAX_SAVEGAME_BUFFER (10 * 1024)
#define GRAVITY 6
#define FASTFALL_SPEED 128
#define LARA_HITPOINTS 1000
#define LARA_AIR 1800
#define LARA_TURN_UNDO (2 * PHD_DEGREE)					   // = 364
#define LARA_TURN_RATE ((PHD_DEGREE / 4) + LARA_TURN_UNDO) // = 409
#define LARA_SLOW_TURN ((PHD_DEGREE * 2) + LARA_TURN_UNDO) // = 728
#define LARA_JUMP_TURN ((PHD_DEGREE * 1) + LARA_TURN_UNDO) // = 546
#define LARA_MED_TURN ((PHD_DEGREE * 4) + LARA_TURN_UNDO)  // = 1092
#define LARA_FAST_TURN ((PHD_DEGREE * 6) + LARA_TURN_UNDO) // = 1456
#define LARA_LEAN_UNDO PHD_DEGREE
#define LARA_DEF_ADD_EDGE (5 * PHD_DEGREE) // = 910
#define LARA_LEAN_RATE 273
#define LARA_LEAN_MAX ((10 * PHD_DEGREE) + LARA_LEAN_UNDO) // = 2002
#define LARA_LEAN_MAX_UW (LARA_LEAN_MAX * 2)
#define LARA_FASTFALL_SPEED (FASTFALL_SPEED + 3) // = 131
#define LARA_RAD 100							 // global radius of g_Lara
#define LARA_HITE 762 // global height of g_Lara - less than 3/4 block
#define UW_MAXSPEED 200
#define UW_RADIUS 300
#define UW_HITE 400
#define UW_WALLDEFLECT (2 * PHD_DEGREE) // = 364
#define SURF_MAXSPEED 60
#define SURF_RADIUS 100
#define SURF_HITE 700
#define WATER_FRICTION 6
#define DAMAGE_START 140
#define DAMAGE_LENGTH 14
#define NO_ITEM (-1)
#define NO_CAMERA (-1)
#define PELLET_SCATTER (20 * PHD_DEGREE)
#define NUM_SG_SHELLS 2
#define GUN_AMMO_CLIP 16
#define MAGNUM_AMMO_CLIP 25
#define UZI_AMMO_CLIP 50
#define SHOTGUN_AMMO_CLIP 6
#define GUN_AMMO_QTY (GUN_AMMO_CLIP * 2)
#define MAGNUM_AMMO_QTY (MAGNUM_AMMO_CLIP * 2)
#define UZI_AMMO_QTY (UZI_AMMO_CLIP * 2)
#define SHOTGUN_AMMO_QTY (SHOTGUN_AMMO_CLIP * NUM_SG_SHELLS)
#define NUM_EFFECTS 100
#define DEATH_WAIT (10 * FRAMES_PER_SECOND)
#define DEATH_WAIT_MIN (2 * FRAMES_PER_SECOND)
#define MAX_HEAD_ROTATION (50 * PHD_DEGREE)		 // = 9100
#define MAX_HEAD_TILT_LOOK (22 * PHD_DEGREE)	 // = 4004
#define MIN_HEAD_TILT_LOOK (-42 * PHD_DEGREE)	// = -7644
#define MAX_HEAD_TILT_CAM (85 * PHD_DEGREE)		 // = 15470
#define MIN_HEAD_TILT_CAM (-85 * PHD_DEGREE)	 // = 15470
#define HEAD_TURN (4 * PHD_DEGREE)				 // = 728
#define HEAD_TURN_SURF (3 * PHD_DEGREE)			 // = 546
#define MAX_HEAD_ROTATION_SURF (50 * PHD_DEGREE) // = 9100
#define MAX_HEAD_TILT_SURF (40 * PHD_DEGREE)	 // = 7280
#define MIN_HEAD_TILT_SURF (-40 * PHD_DEGREE)	// = -7280
#define DIVE_COUNT 10
#define WALL_L 1024
#define WALL_SHIFT 10
#define STEP_L 256
#define NO_ROOM 0xFF
#define STEPUP_HEIGHT ((STEP_L * 3) / 2) // = 384
#define FRONT_ARC PHD_90
#define MAX_HEAD_CHANGE (PHD_DEGREE * 5) // = 910
#define MAX_TILT (PHD_DEGREE * 3)		 // = 546
#define CAM_A_HANG 0
#define CAM_E_HANG (-60 * PHD_DEGREE) // = -10920
#define W2V_SHIFT 14
#define W2V_SCALE (1 << W2V_SHIFT)
#define FOLLOW_CENTRE 1
#define NO_CHUNKY 2
#define CHASE_OBJECT 3
#define LOOK_SPEED 4
#define COMBAT_SPEED 8
#define CHASE_SPEED 12
#define MOVE_SPEED 16
#define MOVE_ANG (2 * PHD_DEGREE)		 // = 364
#define COMBAT_DISTANCE (WALL_L * 5 / 2) // = 2560
#define MAX_ELEVATION (85 * PHD_DEGREE)  // = 15470
#define DEFAULT_RADIUS 10
#define DONT_TARGET (-16384)
#define UNIT_SHADOW 256
#define NO_HEIGHT (-32512)
#define NO_BAD_POS (-NO_HEIGHT)
#define NO_BAD_NEG NO_HEIGHT
#define BAD_JUMP_CEILING ((STEP_L * 3) / 4) // = 192
#define WIBBLE_SIZE 32
#define MAX_WIBBLE 2
#define MAX_SHADE 0x300
#define MAX_EXPANSION 5
#define NO_BOX (-1)
#define BOX_NUMBER 0x7FFF
#define EXPAND_LEFT 0x1
#define EXPAND_RIGHT 0x2
#define EXPAND_TOP 0x4
#define EXPAND_BOTTOM 0x8
#define BLOCKABLE 0x8000
#define BLOCKED 0x4000
#define OVERLAP_INDEX 0x3FFF
#define SEARCH_NUMBER 0x7FFF
#define BLOCKED_SEARCH 0x8000
#define CLIP_LEFT 1
#define CLIP_RIGHT 2
#define CLIP_TOP 4
#define CLIP_BOTTOM 8
#define ALL_CLIP (CLIP_LEFT | CLIP_RIGHT | CLIP_TOP | CLIP_BOTTOM)
#define SECONDARY_CLIP 16
#define STALK_DIST (WALL_L * 3)			// = 3072
#define TARGET_DIST (WALL_L * 4)		// = 4096
#define ESCAPE_DIST (WALL_L * 5)		// = 5120
#define ATTACK_RANGE SQUARE(WALL_L * 3) // = 9437184
#define ESCAPE_CHANCE 2048
#define RECOVER_CHANCE 256
#define GAME_FOV 80

#define PASS_PT_XROT 0x1220
#define PASS_ZTRANS 0x180
#define PASS_YTRANS 0
#define PASS_X_ROT (-0X10E0)
#define PASS_Y_ROT 0
#define PASS_MESH (PFRONT | PSPINE | PBACK)

#define BIFF (WALL_L >> 1)

#define FRAME_BOUND_MIN_X 0
#define FRAME_BOUND_MAX_X 1
#define FRAME_BOUND_MIN_Y 2
#define FRAME_BOUND_MAX_Y 3
#define FRAME_BOUND_MIN_Z 4
#define FRAME_BOUND_MAX_Z 5
#define FRAME_POS_X 6
#define FRAME_POS_Y 7
#define FRAME_POS_Z 8
#define SLOPE_DIF 60
#define VAULT_ANGLE (30 * PHD_DEGREE)
#define HANG_ANGLE (35 * PHD_DEGREE)
#define FRAME_ROT 10
#define END_BIT 0x8000
#define DATA_TYPE 0x00FF
#define VALUE_BITS 0x03FF
#define TRIG_BITS(T) ((T & 0x3C00) >> 10)

#define MIN_SQUARE SQUARE(WALL_L / 4) // = 65536
#define GROUND_SHIFT (STEP_L)

#define DEFAULT_RADIUS 10

#define OPEN_FRAMES (64 / 2)
#define CLOSE_FRAMES (64 / 2)
#define RINGSWITCH_FRAMES (96 / 2)
#define SELECTING_FRAMES (32 / 2)
#define ROTATE_DURATION (48 / 2)
#define OPEN_ROTATION (-0x8000)  // = -32768
#define CLOSE_ROTATION (-0x8000) // = -32768
#define OPTION_RING_OBJECTS 4
#define TITLE_RING_OBJECTS 5
#define MAX_MAP_SHAPES (32 * 32) // = 1024
#define RING_RADIUS 688
#define CAMERA_STARTHEIGHT (-0x600) // = -1536
#define CAMERA_YOFFSET (-96)
#define CAMERA_HEIGHT (-0x100) // = -256
#define CAMERA_2_RING 598
#define LOW_LIGHT 0x1400  // = 5120
#define HIGH_LIGHT 0x1000 // = 4096

#define NO_ACTION 0
#define GAME_ACTION 1
#define END_ACTION 2
#define LOAD_ACTION 4
#define SAVE_ACTION 8
#define SOUND_ACTION 16
#define VIDEO_ACTION 32

#define RESOLUTIONS_SIZE 12

#if _MSC_VER > 0x500
#define _strdup _strdup   // fixes error about POSIX function
#define _USE_MATH_DEFINES // makes maths.h also define M_PI
#endif
