#pragma once

#include "windows.h"
#include "const.h"

#include <d3dx9.h>
#include <math.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

//#include "global/const.h"

//#include <stdbool.h>
//#include <stdint.h>

struct VERTEX_TRANS_QUAD
{
	float x, y, z, w;
	DWORD diffuse;
};

struct VERTEX_COLOR_LINE
{
	float x, y, z, w;
	DWORD diffuse;
};

struct VERTEX_COLOR_TEX
{
	//D3DXVECTOR4 p;
	float x, y, z, w;
	//D3DXVECTOR2 t;
	float tu, tv;
	DWORD diffuse;

};

struct VERTEX_COLOR
{
	//D3DXVECTOR4 p;
	float x, y, z, w;
	//D3DXVECTOR2 t;
	//float tu, tv;
	DWORD diffuse;

};

#define VERTSPERBUCKET (6 * 1024 + 32)

struct TEXTUREBUCKET_OPAQUE
{
	DWORD tpage;
	LPDIRECT3DTEXTURE9 lp_tpage;
	int count;
	VERTEX_COLOR_TEX Vertex[VERTSPERBUCKET];
	LPDIRECT3DVERTEXBUFFER9 VertBuff;
};

struct TRANSQUADBUCKET
{
	int count;
	VERTEX_TRANS_QUAD Vertex[VERTSPERBUCKET];
	LPDIRECT3DVERTEXBUFFER9 VertBuff;
};

struct LINESBUCKET
{
	int count;
	VERTEX_COLOR_LINE Vertex[VERTSPERBUCKET];
	LPDIRECT3DVERTEXBUFFER9 VertBuff;
};

struct COLOREDBUCKET
{
	int count;
	VERTEX_COLOR Vertex[VERTSPERBUCKET];
	LPDIRECT3DVERTEXBUFFER9 VertBuff;
};

#define uint32_t unsigned int
#define int32_t int
#define uint16_t unsigned short int
#define int16_t short int
#define uint8_t unsigned char
#define int8_t char

typedef enum
{
	T1M_BL_TOP_LEFT = 0,
	T1M_BL_TOP_CENTER = 1,
	T1M_BL_TOP_RIGHT = 2,
	T1M_BL_BOTTOM_LEFT = 3,
	T1M_BL_BOTTOM_CENTER = 4,
	T1M_BL_BOTTOM_RIGHT = 5,
} T1M_BAR_LOCATION;

/*

#define GS_NUMBER_OF 95

typedef enum GAMEFLOW_LEVEL_TYPE {
	GFL_TITLE = 0,
	GFL_NORMAL = 1,
	GFL_SAVED = 2,
	GFL_DEMO = 3,
	GFL_CUTSCENE = 4,
	GFL_GYM = 5,
	GFL_CURRENT = 6, // legacy level type for reading TombATI's savegames
} GAMEFLOW_LEVEL_TYPE;



typedef struct GAMEFLOW_LEVEL {
	GAMEFLOW_LEVEL_TYPE level_type;
	int16_t music;
	const char *level_title;
	const char *level_file;
	const char *key1;
	const char *key2;
	const char *key3;
	const char *key4;
	const char *pickup1;
	const char *pickup2;
	const char *puzzle1;
	const char *puzzle2;
	const char *puzzle3;
	const char *puzzle4;
	int8_t demo;
	int16_t secrets;
} GAMEFLOW_LEVEL;

*/

#pragma pack(push, foo, 1)

typedef enum
{
	SWITCH_STATE_OFF = 0,
	SWITCH_STATE_ON = 1,
	SWITCH_STATE_LINK = 2,
} SWITCH_STATE;

typedef struct VBUF
{
	float x, y, z, w;
	float u, v;
	//BYTE g;
	float g;
} VBUF;

typedef struct VBUF2
{
	// float x, y, z, g;
	float x, y, z, g;
} VBUF2;

typedef struct
{

	int16_t number;
	int16_t volume;
	int16_t randomness;
	int16_t flags;
	/*
		int16_t number;			// base sound effect number
		uint8_t volume;	// default volume of sound
		int8_t radius;			// radius of sound (#blocks)
		int8_t randomness;		// Chance of playing sample 0-255  0=Always
		int8_t pitch;		// pitch shift -128 to +127
		int16_t flags;			// Flags for all sorts of stuff
		*/
} SOUND_SAMPLE_INFO;

typedef struct PHD_SPRITE
{
	uint16_t tpage;
	uint16_t offset;
	uint16_t width;
	uint16_t height;
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
} PHD_SPRITE;

typedef enum GAME_BUFFER
{
	GBUF_TEXTURE_PAGES,
	GBUF_MESH_POINTERS,
	GBUF_MESHES,
	GBUF_ANIMS,
	GBUF_ANIM_CHANGES,
	GBUF_ANIM_RANGES,
	GBUF_ANIM_COMMANDS,
	GBUF_ANIM_BONES,
	GBUF_ANIM_FRAMES,
	GBUF_ROOM_INFOS,
	GBUF_ROOM_MESH,
	GBUF_ROOM_DOOR,
	GBUF_ROOM_FLOOR,
	GBUF_ROOM_LIGHTS,
	GBUF_ROOM_STATIC_MESH_INFOS,
	GBUF_FLOOR_DATA,
	GBUF_ITEMS,
	GBUF_CAMERAS,
	GBUF_SOUND_FX,
	GBUF_BOXES,
	GBUF_GROUNDZONE,
	GBUF_FLYZONE,
	GBUF_ANIMATING_TEXTURE_RANGES,
	GBUF_CINEMATIC_FRAMES,
	GBUF_LOADDEMO_BUFFER,
	GBUF_EXTRA_DOOR_STUFF,
	GBUF_EFFECTS,
	GBUF_CREATURE_INFO,
	GBUF_CREATURE_LOT,
	GBUF_SAMPLE_INFOS,
	GBUF_SAMPLES,
	GBUF_TRAP_DATA,
	GBUF_CREATURE_DATA,
} GAME_BUFFER;

typedef int16_t PHD_ANGLE;
typedef uint32_t SG_COL;

typedef enum SAMPLE_FLAG
{
	SAMPLE_FLAG_NO_PAN = 1 << 12,
	SAMPLE_FLAG_PITCH_WIBBLE = 1 << 13,
	SAMPLE_FLAG_VOLUME_WIBBLE = 1 << 14,
} SAMPLE_FLAG;

typedef enum CAMERA_TYPE
{
	CAM_CHASE = 0,
	CAM_FIXED = 1,
	CAM_LOOK = 2,
	CAM_COMBAT = 3,
	CAM_CINEMATIC = 4,
	CAM_HEAVY = 5,
} CAMERA_TYPE;

typedef enum GAME_OBJECT_ID
{
	O_LARA = 0,
	O_PISTOLS = 1,
	O_SHOTGUN = 2,
	O_MAGNUM = 3,
	O_UZI = 4,
	O_LARA_EXTRA = 5,
	O_EVIL_LARA = 6,
	O_WOLF = 7,
	O_BEAR = 8,
	O_BAT = 9,
	O_CROCODILE = 10,
	O_ALLIGATOR = 11,
	O_LION = 12,
	O_LIONESS = 13,
	O_PUMA = 14,
	O_APE = 15,
	O_RAT = 16,
	O_VOLE = 17,
	O_DINOSAUR = 18,
	O_RAPTOR = 19,
	O_WARRIOR1 = 20, // flying mutant
	O_WARRIOR2 = 21,
	O_WARRIOR3 = 22,
	O_CENTAUR = 23,
	O_MUMMY = 24,
	O_DINO_WARRIOR = 25,
	O_FISH = 26,
	O_LARSON = 27,
	O_PIERRE = 28,
	O_SKATEBOARD = 29,
	O_MERCENARY1 = 30,
	O_MERCENARY2 = 31,
	O_MERCENARY3 = 32,
	O_NATLA = 33,
	O_ABORTION = 34, // a.k.a. Adam, Torso or Evil Natla
	O_FALLING_BLOCK = 35,
	O_PENDULUM = 36,
	O_SPIKES = 37,
	O_ROLLING_BALL = 38,
	O_DARTS = 39,
	O_DART_EMITTER = 40,
	O_DRAW_BRIDGE = 41,
	O_TEETH_TRAP = 42,
	O_DAMOCLES_SWORD = 43,
	O_THORS_HANDLE = 44,
	O_THORS_HEAD = 45,
	O_LIGHTNING_EMITTER = 46,
	O_MOVING_BAR = 47,
	O_MOVABLE_BLOCK = 48,
	O_MOVABLE_BLOCK2 = 49,
	O_MOVABLE_BLOCK3 = 50,
	O_MOVABLE_BLOCK4 = 51,
	O_ROLLING_BLOCK = 52,
	O_FALLING_CEILING1 = 53,
	O_FALLING_CEILING2 = 54,
	O_SWITCH_TYPE1 = 55,
	O_SWITCH_TYPE2 = 56,
	O_DOOR_TYPE1 = 57,
	O_DOOR_TYPE2 = 58,
	O_DOOR_TYPE3 = 59,
	O_DOOR_TYPE4 = 60,
	O_DOOR_TYPE5 = 61,
	O_DOOR_TYPE6 = 62,
	O_DOOR_TYPE7 = 63,
	O_DOOR_TYPE8 = 64,
	O_TRAPDOOR = 65,
	O_TRAPDOOR2 = 66,
	O_BIGTRAPDOOR = 67,
	O_BRIDGE_FLAT = 68,
	O_BRIDGE_TILT1 = 69,
	O_BRIDGE_TILT2 = 70,
	O_PASSPORT_OPTION = 71,
	O_MAP_OPTION = 72,
	O_PHOTO_OPTION = 73,
	O_COG_1 = 74,
	O_COG_2 = 75,
	O_COG_3 = 76,
	O_PLAYER_1 = 77,
	O_PLAYER_2 = 78,
	O_PLAYER_3 = 79,
	O_PLAYER_4 = 80,
	O_PASSPORT_CLOSED = 81,
	O_MAP_CLOSED = 82,
	O_SAVEGAME_ITEM = 83,
	O_GUN_ITEM = 84,
	O_SHOTGUN_ITEM = 85,
	O_MAGNUM_ITEM = 86,
	O_UZI_ITEM = 87,
	O_GUN_AMMO_ITEM = 88,
	O_SG_AMMO_ITEM = 89,
	O_MAG_AMMO_ITEM = 90,
	O_UZI_AMMO_ITEM = 91,
	O_EXPLOSIVE_ITEM = 92,
	O_MEDI_ITEM = 93,
	O_BIGMEDI_ITEM = 94,
	O_DETAIL_OPTION = 95,
	O_SOUND_OPTION = 96,
	O_CONTROL_OPTION = 97,
	O_GAMMA_OPTION = 98,
	O_GUN_OPTION = 99,
	O_SHOTGUN_OPTION = 100,
	O_MAGNUM_OPTION = 101,
	O_UZI_OPTION = 102,
	O_GUN_AMMO_OPTION = 103,
	O_SG_AMMO_OPTION = 104,
	O_MAG_AMMO_OPTION = 105,
	O_UZI_AMMO_OPTION = 106,
	O_EXPLOSIVE_OPTION = 107,
	O_MEDI_OPTION = 108,
	O_BIGMEDI_OPTION = 109,
	O_PUZZLE_ITEM1 = 110,
	O_PUZZLE_ITEM2 = 111,
	O_PUZZLE_ITEM3 = 112,
	O_PUZZLE_ITEM4 = 113,
	O_PUZZLE_OPTION1 = 114,
	O_PUZZLE_OPTION2 = 115,
	O_PUZZLE_OPTION3 = 116,
	O_PUZZLE_OPTION4 = 117,
	O_PUZZLE_HOLE1 = 118,
	O_PUZZLE_HOLE2 = 119,
	O_PUZZLE_HOLE3 = 120,
	O_PUZZLE_HOLE4 = 121,
	O_PUZZLE_DONE1 = 122,
	O_PUZZLE_DONE2 = 123,
	O_PUZZLE_DONE3 = 124,
	O_PUZZLE_DONE4 = 125,
	O_LEADBAR_ITEM = 126,
	O_LEADBAR_OPTION = 127,
	O_MIDAS_TOUCH = 128,
	O_KEY_ITEM1 = 129,
	O_KEY_ITEM2 = 130,
	O_KEY_ITEM3 = 131,
	O_KEY_ITEM4 = 132,
	O_KEY_OPTION1 = 133,
	O_KEY_OPTION2 = 134,
	O_KEY_OPTION3 = 135,
	O_KEY_OPTION4 = 136,
	O_KEY_HOLE1 = 137,
	O_KEY_HOLE2 = 138,
	O_KEY_HOLE3 = 139,
	O_KEY_HOLE4 = 140,
	O_PICKUP_ITEM1 = 141,
	O_PICKUP_ITEM2 = 142,
	O_SCION_ITEM = 143,
	O_SCION_ITEM2 = 144,
	O_SCION_ITEM3 = 145,
	O_SCION_ITEM4 = 146,
	O_SCION_HOLDER = 147,
	O_PICKUP_OPTION1 = 148,
	O_PICKUP_OPTION2 = 149,
	O_SCION_OPTION = 150,
	O_EXPLOSION1 = 151,
	O_EXPLOSION2 = 152,
	O_SPLASH1 = 153,
	O_SPLASH2 = 154,
	O_BUBBLES1 = 155,
	O_BUBBLES2 = 156,
	O_BUBBLE_EMITTER = 157,
	O_BLOOD1 = 158,
	O_BLOOD2 = 159,
	O_DART_EFFECT = 160,
	O_STATUE = 161,
	O_PORTACABIN = 162,
	O_PODS = 163,
	O_RICOCHET1 = 164,
	O_TWINKLE = 165,
	O_GUN_FLASH = 166,
	O_DUST = 167,
	O_BODY_PART = 168,
	O_CAMERA_TARGET = 169,
	O_WATERFALL = 170,
	O_MISSILE1 = 171,
	O_MISSILE2 = 172,
	O_MISSILE3 = 173,
	O_MISSILE4 = 174,
	O_MISSILE5 = 175,
	O_LAVA = 176,
	O_LAVA_EMITTER = 177,
	O_FLAME = 178,
	O_FLAME_EMITTER = 179,
	O_LAVA_WEDGE = 180,
	O_BIG_POD = 181,
	O_BOAT = 182,
	O_EARTHQUAKE = 183,
	O_TEMP5 = 184,
	O_TEMP6 = 185,
	O_TEMP7 = 186,
	O_TEMP8 = 187,
	O_TEMP9 = 188,
	O_TEMP10 = 189,
	O_HAIR = O_TEMP10,
	O_ALPHABET = 190,
	O_NUMBER_OF = 191,
} GAME_OBJECT_ID;

typedef enum GAME_STATIC_ID
{
	STATIC_PLANT0 = 0,
	STATIC_PLANT1 = 1,
	STATIC_PLANT2 = 2,
	STATIC_PLANT3 = 3,
	STATIC_PLANT4 = 4,
	STATIC_PLANT5 = 5,
	STATIC_PLANT6 = 6,
	STATIC_PLANT7 = 7,
	STATIC_PLANT8 = 8,
	STATIC_PLANT9 = 9,
	STATIC_FURNITURE0 = 10,
	STATIC_FURNITURE1 = 11,
	STATIC_FURNITURE2 = 12,
	STATIC_FURNITURE3 = 13,
	STATIC_FURNITURE4 = 14,
	STATIC_FURNITURE5 = 15,
	STATIC_FURNITURE6 = 16,
	STATIC_FURNITURE7 = 17,
	STATIC_FURNITURE8 = 18,
	STATIC_FURNITURE9 = 19,
	STATIC_ROCK0 = 20,
	STATIC_ROCK1 = 21,
	STATIC_ROCK2 = 22,
	STATIC_ROCK3 = 23,
	STATIC_ROCK4 = 24,
	STATIC_ROCK5 = 25,
	STATIC_ROCK6 = 26,
	STATIC_ROCK7 = 27,
	STATIC_ROCK8 = 28,
	STATIC_ROCK9 = 29,
	STATIC_ARCHITECTURE0 = 30,
	STATIC_ARCHITECTURE1 = 31,
	STATIC_ARCHITECTURE2 = 32,
	STATIC_ARCHITECTURE3 = 33,
	STATIC_ARCHITECTURE4 = 34,
	STATIC_ARCHITECTURE5 = 35,
	STATIC_ARCHITECTURE6 = 36,
	STATIC_ARCHITECTURE7 = 37,
	STATIC_ARCHITECTURE8 = 38,
	STATIC_ARCHITECTURE9 = 39,
	STATIC_DEBRIS0 = 40,
	STATIC_DEBRIS1 = 41,
	STATIC_DEBRIS2 = 42,
	STATIC_DEBRIS3 = 43,
	STATIC_DEBRIS4 = 44,
	STATIC_DEBRIS5 = 45,
	STATIC_DEBRIS6 = 46,
	STATIC_DEBRIS7 = 47,
	STATIC_DEBRIS8 = 48,
	STATIC_DEBRIS9 = 49,
	STATIC_NUMBER_OF = 50,
} GAME_STATIC_ID;

typedef enum SOUND_EFFECT_ID
{
	SFX_LARA_FEET = 0,
	SFX_LARA_CLIMB2 = 1,
	SFX_LARA_NO = 2,
	SFX_LARA_SLIPPING = 3,
	SFX_LARA_LAND = 4,
	SFX_LARA_CLIMB1 = 5,
	SFX_LARA_DRAW = 6,
	SFX_LARA_HOLSTER = 7,
	SFX_LARA_FIRE = 8,
	SFX_LARA_RELOAD = 9,
	SFX_LARA_RICOCHET = 10,
	SFX_BEAR_GROWL = 11,
	SFX_BEAR_FEET = 12,
	SFX_BEAR_ATTACK = 13,
	SFX_BEAR_SNARL = 14,
	SFX_BEAR_HURT = 16,
	SFX_BEAR_DEATH = 18,
	SFX_WOLF_JUMP = 19,
	SFX_WOLF_HURT = 20,
	SFX_WOLF_DEATH = 22,
	SFX_WOLF_HOWL = 24,
	SFX_WOLF_ATTACK = 25,
	SFX_LARA_CLIMB3 = 26,
	SFX_LARA_BODYSL = 27,
	SFX_LARA_SHIMMY2 = 28,
	SFX_LARA_JUMP = 29,
	SFX_LARA_FALL = 30,
	SFX_LARA_INJURY = 31,
	SFX_LARA_ROLL = 32,
	SFX_LARA_SPLASH = 33,
	SFX_LARA_GETOUT = 34,
	SFX_LARA_SWIM = 35,
	SFX_LARA_BREATH = 36,
	SFX_LARA_BUBBLES = 37,
	SFX_LARA_SWITCH = 38,
	SFX_LARA_KEY = 39,
	SFX_LARA_OBJECT = 40,
	SFX_LARA_GENERAL_DEATH = 41,
	SFX_LARA_KNEES_DEATH = 42,
	SFX_LARA_UZI_FIRE = 43,
	SFX_LARA_MAGNUMS = 44,
	SFX_LARA_SHOTGUN = 45,
	SFX_LARA_BLOCK_PUSH1 = 46,
	SFX_LARA_BLOCK_PUSH2 = 47,
	SFX_LARA_EMPTY = 48,
	SFX_LARA_BULLETHIT = 50,
	SFX_LARA_BLKPULL = 51,
	SFX_LARA_FLOATING = 52,
	SFX_LARA_FALLDETH = 53,
	SFX_LARA_GRABHAND = 54,
	SFX_LARA_GRABBODY = 55,
	SFX_LARA_GRABFEET = 56,
	SFX_LARA_SWITCHUP = 57,
	SFX_BAT_SQK = 58,
	SFX_BAT_FLAP = 59,
	SFX_UNDERWATER = 60,
	SFX_UNDERWATER_SWITCH = 61,
	SFX_BLOCK_SOUND = 63,
	SFX_DOOR = 64,
	SFX_PENDULUM_BLADES = 65,
	SFX_ROCK_FALL_CRUMBLE = 66,
	SFX_ROCK_FALL_FALL = 67,
	SFX_ROCK_FALL_LAND = 68,
	SFX_T_REX_DEATH = 69,
	SFX_T_REX_FOOTSTOMP = 70,
	SFX_T_REX_ROAR = 71,
	SFX_T_REX_ATTACK = 72,
	SFX_RAPTOR_ROAR = 73,
	SFX_RAPTOR_ATTACK = 74,
	SFX_RAPTOR_FEET = 75,
	SFX_MUMMY_GROWL = 76,
	SFX_LARSON_FIRE = 77,
	SFX_LARSON_RICOCHET = 78,
	SFX_WATERFALL_LOOP = 79,
	SFX_WATER_LOOP = 80,
	SFX_WATERFALL_BIG = 81,
	SFX_CHAINDOOR_UP = 82,
	SFX_CHAINDOOR_DOWN = 83,
	SFX_COGS = 84,
	SFX_LION_HURT = 85,
	SFX_LION_ATTACK = 86,
	SFX_LION_ROAR = 87,
	SFX_LION_DEATH = 88,
	SFX_GORILLA_FEET = 89,
	SFX_GORILLA_PANT = 90,
	SFX_GORILLA_DEATH = 91,
	SFX_CROC_FEET = 92,
	SFX_CROC_ATTACK = 93,
	SFX_RAT_FEET = 94,
	SFX_RAT_CHIRP = 95,
	SFX_RAT_ATTACK = 96,
	SFX_RAT_DEATH = 97,
	SFX_THUNDER = 98,
	SFX_EXPLOSION = 99,
	SFX_GORILLA_GRUNT = 100,
	SFX_GORILLA_GRUNTS = 101,
	SFX_CROC_DEATH = 102,
	SFX_DAMOCLES_SWORD = 103,
	SFX_ATLANTEAN_EXPLODE = 104,
	SFX_MENU_ROTATE = 108,
	SFX_MENU_CHOOSE = 109,
	SFX_MENU_GAMEBOY = 110,
	SFX_MENU_SPININ = 111,
	SFX_MENU_SPINOUT = 112,
	SFX_MENU_COMPASS = 113,
	SFX_MENU_GUNS = 114,
	SFX_MENU_PASSPORT = 115,
	SFX_MENU_MEDI = 116,
	SFX_RAISINGBLOCK_FX = 117,
	SFX_SAND_FX = 118,
	SFX_STAIRS2SLOPE_FX = 119,
	SFX_ATLANTEAN_WALK = 120,
	SFX_ATLANTEAN_ATTACK = 121,
	SFX_ATLANTEAN_JUMP_ATTACK = 122,
	SFX_ATLANTEAN_NEEDLE = 123,
	SFX_ATLANTEAN_BALL = 124,
	SFX_ATLANTEAN_WINGS = 125,
	SFX_ATLANTEAN_RUN = 126,
	SFX_SLAMDOOR_CLOSE = 127,
	SFX_SLAMDOOR_OPEN = 128,
	SFX_SKATEBOARD_MOVE = 129,
	SFX_SKATEBOARD_STOP = 130,
	SFX_SKATEBOARD_SHOOT = 131,
	SFX_SKATEBOARD_HIT = 132,
	SFX_SKATEBOARD_START = 133,
	SFX_SKATEBOARD_DEATH = 134,
	SFX_SKATEBOARD_HIT_GROUND = 135,
	SFX_ABORTION_HIT_GROUND = 136,
	SFX_ABORTION_ATTACK1 = 137,
	SFX_ABORTION_ATTACK2 = 138,
	SFX_ABORTION_DEATH = 139,
	SFX_ABORTION_ARM_SWING = 140,
	SFX_ABORTION_MOVE = 141,
	SFX_ABORTION_HIT = 142,
	SFX_CENTAUR_FEET = 143,
	SFX_CENTAUR_ROAR = 144,
	SFX_LARA_SPIKE_DEATH = 145,
	SFX_LARA_DEATH3 = 146,
	SFX_ROLLING_BALL = 147,
	SFX_LAVA_LOOP = 148,
	SFX_LAVA_FOUNTAIN = 149,
	SFX_FIRE = 150,
	SFX_DARTS = 151,
	SFX_METAL_DOOR_CLOSE = 152,
	SFX_METAL_DOOR_OPEN = 153,
	SFX_ALTAR_LOOP = 154,
	SFX_POWERUP_FX = 155,
	SFX_COWBOY_DEATH = 156,
	SFX_BLACK_GOON_DEATH = 157,
	SFX_LARSON_DEATH = 158,
	SFX_PIERRE_DEATH = 159,
	SFX_NATLA_DEATH = 160,
	SFX_TRAPDOOR_OPEN = 161,
	SFX_TRAPDOOR_CLOSE = 162,
	SFX_ATLANTEAN_EGG_LOOP = 163,
	SFX_ATLANTEAN_EGG_HATCH = 164,
	SFX_DRILL_ENGINE_START = 165,
	SFX_DRILL_ENGINE_LOOP = 166,
	SFX_CONVEYOR_BELT = 167,
	SFX_HUT_LOWERED = 168,
	SFX_HUT_HIT_GROUND = 169,
	SFX_EXPLOSION_FX = 170,
	SFX_ATLANTEAN_DEATH = 171,
	SFX_CHAINBLOCK_FX = 172,
	SFX_SECRET = 173,
	SFX_GYM_HINT_01 = 174,
	SFX_GYM_HINT_02 = 175,
	SFX_GYM_HINT_03 = 176,
	SFX_GYM_HINT_04 = 177,
	SFX_GYM_HINT_05 = 178,
	SFX_GYM_HINT_06 = 179,
	SFX_GYM_HINT_07 = 180,
	SFX_GYM_HINT_08 = 181,
	SFX_GYM_HINT_09 = 182,
	SFX_GYM_HINT_10 = 183,
	SFX_GYM_HINT_11 = 184,
	SFX_GYM_HINT_12 = 185,
	SFX_GYM_HINT_13 = 186,
	SFX_GYM_HINT_14 = 187,
	SFX_GYM_HINT_15 = 188,
	SFX_GYM_HINT_16 = 189,
	SFX_GYM_HINT_17 = 190,
	SFX_GYM_HINT_18 = 191,
	SFX_GYM_HINT_19 = 192,
	SFX_GYM_HINT_20 = 193,
	SFX_GYM_HINT_21 = 194,
	SFX_GYM_HINT_22 = 195,
	SFX_GYM_HINT_23 = 196,
	SFX_GYM_HINT_24 = 197,
	SFX_GYM_HINT_25 = 198,
	SFX_BALDY_SPEECH = 199,
	SFX_COWBOY_SPEECH = 200,
	SFX_LARSON_SPEECH = 201,
	SFX_NATLA_SPEECH = 202,
	SFX_PIERRE_SPEECH = 203,
	SFX_SKATEKID_SPEECH = 204,
	SFX_LARA_SETUP = 205,
} SOUND_EFFECT_ID;

typedef enum LARA_ANIMATION_FRAME
{
	AF_VAULT12 = 759,
	AF_VAULT34 = 614,
	AF_FASTFALL = 481,
	AF_STOP = 185,
	AF_FALLDOWN = 492,
	AF_STOP_LEFT = 58,
	AF_STOP_RIGHT = 74,
	AF_HITWALLLEFT = 800,
	AF_HITWALLRIGHT = 815,
	AF_RUNSTEPUP_LEFT = 837,
	AF_RUNSTEPUP_RIGHT = 830,
	AF_WALKSTEPUP_LEFT = 844,
	AF_WALKSTEPUP_RIGHT = 858,
	AF_WALKSTEPD_LEFT = 887,
	AF_WALKSTEPD_RIGHT = 874,
	AF_BACKSTEPD_LEFT = 899,
	AF_BACKSTEPD_RIGHT = 930,
	AF_LANDFAR = 358,
	AF_GRABLEDGE = 1493,
	AF_GRABLEDGE_OLD = 621,
	AF_SWIMGLIDE = 1431,
	AF_FALLBACK = 1473,
	AF_HANG = 1514,
	AF_HANG_OLD = 642,
	AF_STARTHANG = 1505,
	AF_STARTHANG_OLD = 634,
	AF_STOPHANG = 448,
	AF_SLIDE = 1133,
	AF_SLIDEBACK = 1677,
	AF_TREAD = 1736,
	AF_SURFTREAD = 1937,
	AF_SURFDIVE = 2041,
	AF_SURFCLIMB = 1849,
	AF_JUMPIN = 1895,
	AF_ROLL = 3857,
	AF_RBALL_DEATH = 3561,
	AF_SPIKE_DEATH = 3887,
	AF_GRABLEDGEIN = 3974,
	AF_PPREADY = 2091,
	AF_PICKUP = 3443,
	AF_PICKUP_UW = 2970,
	AF_PICKUPSCION = 44,
	AF_USEPUZZLE = 3372,
} LARA_ANIMATION_FRAME;

typedef enum LARA_SHOTGUN_ANIMATION_FRAME
{
	AF_SG_AIM = 0,
	AF_SG_DRAW = 13,
	AF_SG_RECOIL = 47,
	AF_SG_UNDRAW = 80,
	AF_SG_UNAIM = 114,
	AF_SG_END = 127,
} LARA_SHOTGUN_ANIMATION_FRAME;

typedef enum LARA_GUN_ANIMATION_FRAME
{
	AF_G_AIM = 0,
	AF_G_AIM_L = 4,
	AF_G_DRAW1 = 5,
	AF_G_DRAW1_L = 12,
	AF_G_DRAW2 = 13,
	AF_G_DRAW2_L = 23,
	AF_G_RECOIL = 24,
} LARA_GUN_ANIMATION_FRAME;

typedef enum LARA_ANIMATION
{
	AA_VAULT12 = 50,
	AA_VAULT34 = 42,
	AA_FASTFALL = 32,
	AA_STOP = 11,
	AA_FALLDOWN = 34,
	AA_STOP_LEFT = 2,
	AA_STOP_RIGHT = 3,
	AA_HITWALLLEFT = 53,
	AA_HITWALLRIGHT = 54,
	AA_RUNSTEPUP_LEFT = 56,
	AA_RUNSTEPUP_RIGHT = 55,
	AA_WALKSTEPUP_LEFT = 57,
	AA_WALKSTEPUP_RIGHT = 58,
	AA_WALKSTEPD_LEFT = 60,
	AA_WALKSTEPD_RIGHT = 59,
	AA_BACKSTEPD_LEFT = 61,
	AA_BACKSTEPD_RIGHT = 62,
	AA_LANDFAR = 24,
	AA_GRABLEDGE = 96,
	AA_GRABLEDGE_OLD = 32,
	AA_SWIMGLIDE = 87,
	AA_FALLBACK = 93,
	AA_HANG = 96,
	AA_HANG_OLD = 33,
	AA_STARTHANG = 96,
	AA_STARTHANG_OLD = 33,
	AA_STOPHANG = 28,
	AA_SLIDE = 70,
	AA_SLIDEBACK = 104,
	AA_TREAD = 108,
	AA_SURFTREAD = 114,
	AA_SURFDIVE = 119,
	AA_SURFCLIMB = 111,
	AA_JUMPIN = 112,
	AA_ROLL = 146,
	AA_RBALL_DEATH = 139,
	AA_SPIKE_DEATH = 149,
	AA_GRABLEDGEIN = 150,
	AA_SPAZ_FORWARD = 125,
	AA_SPAZ_BACK = 126,
	AA_SPAZ_RIGHT = 127,
	AA_SPAZ_LEFT = 128,
} LARA_ANIMATION;

typedef enum LARA_WATER_STATUS
{
	LWS_ABOVEWATER = 0,
	LWS_UNDERWATER = 1,
	LWS_SURFACE = 2,
	LWS_CHEAT = 3,
} LARA_WATER_STATUS;

typedef enum LARA_STATE
{
	AS_WALK = 0,
	AS_RUN = 1,
	AS_STOP = 2,
	AS_FORWARDJUMP = 3,
	AS_POSE = 4,
	AS_FASTBACK = 5,
	AS_TURN_R = 6,
	AS_TURN_L = 7,
	AS_DEATH = 8,
	AS_FASTFALL = 9,
	AS_HANG = 10,
	AS_REACH = 11,
	AS_SPLAT = 12,
	AS_TREAD = 13,
	AS_LAND = 14,
	AS_COMPRESS = 15,
	AS_BACK = 16,
	AS_SWIM = 17,
	AS_GLIDE = 18,
	AS_NULL = 19,
	AS_FASTTURN = 20,
	AS_STEPRIGHT = 21,
	AS_STEPLEFT = 22,
	AS_HIT = 23,
	AS_SLIDE = 24,
	AS_BACKJUMP = 25,
	AS_RIGHTJUMP = 26,
	AS_LEFTJUMP = 27,
	AS_UPJUMP = 28,
	AS_FALLBACK = 29,
	AS_HANGLEFT = 30,
	AS_HANGRIGHT = 31,
	AS_SLIDEBACK = 32,
	AS_SURFTREAD = 33,
	AS_SURFSWIM = 34,
	AS_DIVE = 35,
	AS_PUSHBLOCK = 36,
	AS_PULLBLOCK = 37,
	AS_PPREADY = 38,
	AS_PICKUP = 39,
	AS_SWITCHON = 40,
	AS_SWITCHOFF = 41,
	AS_USEKEY = 42,
	AS_USEPUZZLE = 43,
	AS_UWDEATH = 44,
	AS_ROLL = 45,
	AS_SPECIAL = 46,
	AS_SURFBACK = 47,
	AS_SURFLEFT = 48,
	AS_SURFRIGHT = 49,
	AS_USEMIDAS = 50,
	AS_DIEMIDAS = 51,
	AS_SWANDIVE = 52,
	AS_FASTDIVE = 53,
	AS_GYMNAST = 54,
	AS_WATEROUT = 55,
} LARA_STATE;

typedef enum LARA_GUN_STATE
{
	LGS_ARMLESS = 0,
	LGS_HANDSBUSY = 1,
	LGS_DRAW = 2,
	LGS_UNDRAW = 3,
	LGS_READY = 4,
} LARA_GUN_STATE;

typedef enum LARA_GUN_TYPE
{
	LGT_UNARMED = 0,
	LGT_PISTOLS = 1,
	LGT_MAGNUMS = 2,
	LGT_UZIS = 3,
	LGT_SHOTGUN = 4,
	NUM_WEAPONS = 5
} LARA_GUN_TYPE;

typedef enum LARA_MESH
{
	LM_HIPS = 0,
	LM_THIGH_L = 1,
	LM_CALF_L = 2,
	LM_FOOT_L = 3,
	LM_THIGH_R = 4,
	LM_CALF_R = 5,
	LM_FOOT_R = 6,
	LM_TORSO = 7,
	LM_UARM_R = 8,
	LM_LARM_R = 9,
	LM_HAND_R = 10,
	LM_UARM_L = 11,
	LM_LARM_L = 12,
	LM_HAND_L = 13,
	LM_HEAD = 14,
	LM_NUMBER_OF = 15,
} LARA_MESH;

typedef enum MOOD_TYPE
{
	MOOD_BORED = 0,
	MOOD_ATTACK = 1,
	MOOD_ESCAPE = 2,
	MOOD_STALK = 3,
} MOOD_TYPE;

typedef enum TARGET_TYPE
{
	TARGET_NONE = 0,
	TARGET_PRIMARY = 1,
	TARGET_SECONDARY = 2,
} TARGET_TYPE;

typedef enum D_FLAGS
{
	D_TRANS1 = 1,
	D_TRANS2 = 2,
	D_TRANS3 = 3,
	D_TRANS4 = 4,
	D_NEXT = 1 << 3,
} D_FLAGS;

typedef enum COLL_TYPE
{
	COLL_NONE = 0,
	COLL_FRONT = 1,
	COLL_LEFT = 2,
	COLL_RIGHT = 4,
	COLL_TOP = 8,
	COLL_TOPFRONT = 16,
	COLL_CLAMP = 32,
} COLL_TYPE;

typedef enum HEIGHT_TYPE
{
	HT_WALL = 0,
	HT_SMALL_SLOPE = 1,
	HT_BIG_SLOPE = 2,
} HEIGHT_TYPE;

typedef enum DIRECTION
{
	DIR_NORTH = 0,
	DIR_EAST = 1,
	DIR_SOUTH = 2,
	DIR_WEST = 3,
} DIRECTION;

typedef enum ANIM_COMMAND
{
	AC_NULL = 0,
	AC_MOVE_ORIGIN = 1,
	AC_JUMP_VELOCITY = 2,
	AC_ATTACK_READY = 3,
	AC_DEACTIVATE = 4,
	AC_SOUND_FX = 5,
	AC_EFFECT = 6,
} ANIM_COMMAND;

typedef enum BONE_EXTRA_BITS
{
	BEB_POP = 1 << 0,
	BEB_PUSH = 1 << 1,
	BEB_ROT_X = 1 << 2,
	BEB_ROT_Y = 1 << 3,
	BEB_ROT_Z = 1 << 4,
} BONE_EXTRA_BITS;

typedef enum ITEM_STATUS
{
	IS_NOT_ACTIVE = 0,
	IS_ACTIVE = 1,
	IS_DEACTIVATED = 2,
	IS_INVISIBLE = 3,
} ITEM_STATUS;

typedef enum ROOM_FLAG
{
	RF_UNDERWATER = 1,
} ROOM_FLAG;

typedef enum FLOOR_TYPE
{
	FT_FLOOR = 0,
	FT_DOOR = 1,
	FT_TILT = 2,
	FT_ROOF = 3,
	FT_TRIGGER = 4,
	FT_LAVA = 5,
} FLOOR_TYPE;

typedef enum TRIGGER_TYPE
{
	TT_TRIGGER = 0,
	TT_PAD = 1,
	TT_SWITCH = 2,
	TT_KEY = 3,
	TT_PICKUP = 4,
	TT_HEAVY = 5,
	TT_ANTIPAD = 6,
	TT_COMBAT = 7,
	TT_DUMMY = 8,
} TRIGGER_TYPE;

typedef enum TRIGGER_OBJECT
{
	TO_OBJECT = 0,
	TO_CAMERA = 1,
	TO_SINK = 2,
	TO_FLIPMAP = 3,
	TO_FLIPON = 4,
	TO_FLIPOFF = 5,
	TO_TARGET = 6,
	TO_FINISH = 7,
	TO_CD = 8,
	TO_FLIPEFFECT = 9,
	TO_SECRET = 10,
} TRIGGER_OBJECT;

typedef enum ITEM_FLAG
{
	IF_ONESHOT = 0x0100,
	IF_CODE_BITS = 0x3E00,
	IF_REVERSE = 0x4000,
	IF_NOT_VISIBLE = 0x0100,
	IF_KILLED_ITEM = 0x8000,
} ITEM_FLAG;

typedef enum INV_MODE
{
	INV_GAME_MODE = 0,
	INV_TITLE_MODE = 1,
	INV_KEYS_MODE = 2,
	INV_SAVE_MODE = 3,
	INV_LOAD_MODE = 4,
	INV_DEATH_MODE = 5,
	INV_SAVE_CRYSTAL_MODE = 6,
	INV_PAUSE_MODE = 7,
} INV_MODE;

typedef enum INV_TEXT
{
	IT_NAME = 0,
	IT_QTY = 1,
	IT_NUMBER_OF = 2,
} INV_TEXT;

typedef enum INV_COLOUR
{
	IC_BLACK = 0,
	IC_GREY = 1,
	IC_WHITE = 2,
	IC_RED = 3,
	IC_ORANGE = 4,
	IC_YELLOW = 5,
	IC_GREEN1 = 6,
	IC_GREEN2 = 7,
	IC_GREEN3 = 8,
	IC_GREEN4 = 9,
	IC_GREEN5 = 10,
	IC_GREEN6 = 11,
	IC_DARKGREEN = 12,
	IC_GREEN = 13,
	IC_CYAN = 14,
	IC_BLUE = 15,
	IC_MAGENTA = 16,
	IC_NUMBER_OF = 17,
} INV_COLOUR;

typedef enum RING_STATUS
{
	RNG_OPENING = 0,
	RNG_OPEN = 1,
	RNG_CLOSING = 2,
	RNG_MAIN2OPTION = 3,
	RNG_MAIN2KEYS = 4,
	RNG_KEYS2MAIN = 5,
	RNG_OPTION2MAIN = 6,
	RNG_SELECTING = 7,
	RNG_SELECTED = 8,
	RNG_DESELECTING = 9,
	RNG_DESELECT = 10,
	RNG_CLOSING_ITEM = 11,
	RNG_EXITING_INVENTORY = 12,
	RNG_DONE = 13,
} RING_STATUS;

typedef enum RING_TYPE
{
	RT_MAIN = 0,
	RT_OPTION = 1,
	RT_KEYS = 2,
} RING_TYPE;

typedef enum SHAPE
{
	SHAPE_SPRITE = 1,
	SHAPE_LINE = 2,
	SHAPE_BOX = 3,
	SHAPE_FBOX = 4
} SHAPE;

typedef enum DOOR_ANIM
{
	DOOR_CLOSED = 0,
	DOOR_OPEN = 1,
} DOOR_ANIM;

typedef enum TRAP_ANIM
{
	TRAP_SET = 0,
	TRAP_ACTIVATE = 1,
	TRAP_WORKING = 2,
	TRAP_FINISHED = 3,
} TRAP_ANIM;

typedef enum ROLLING_BLOCK_STATE
{
	RBS_START = 0,
	RBS_END = 1,
	RBS_MOVING = 2,
} ROLLING_BLOCK_STATE;

typedef enum REQUEST_INFO_FLAGS
{
	RIF_BLOCKED = 1 << 0,
	RIF_BLOCKABLE = 1 << 1,
} REQUEST_INFO_FLAGS;

typedef enum GAMEFLOW_LEVEL_TYPE
{
	GFL_TITLE = 0,
	GFL_NORMAL = 1,
	GFL_SAVED = 2,
	GFL_DEMO = 3,
	GFL_CUTSCENE = 4,
	GFL_GYM = 5,
	GFL_CURRENT = 6, // legacy level type for reading TombATI's savegames
} GAMEFLOW_LEVEL_TYPE;

typedef enum GAMEFLOW_OPTION
{
	GF_NOP_BREAK = -2,
	GF_NOP = -1,
	GF_START_GAME = 0,
	GF_START_CINE = 1 << 6,
	GF_START_FMV = 2 << 6,
	GF_START_DEMO = 3 << 6,
	GF_EXIT_TO_TITLE = 4 << 6,
	GF_LEVEL_COMPLETE = 5 << 6,
	GF_EXIT_GAME = 6 << 6,
	GF_START_SAVED_GAME = 7 << 6,
} GAMEFLOW_OPTION;

typedef enum GAMEFLOW_SEQUENCE_TYPE
{
	GFS_END = -1,
	GFS_START_GAME,
	GFS_LOOP_GAME,
	GFS_STOP_GAME,
	GFS_START_CINE,
	GFS_LOOP_CINE,
	GFS_STOP_CINE,
	GFS_PLAY_FMV,
	GFS_LEVEL_STATS,
	GFS_DISPLAY_PICTURE,
	GFS_EXIT_TO_TITLE,
	GFS_EXIT_TO_LEVEL,
	GFS_EXIT_TO_CINE,
	GFS_SET_CAM_X,
	GFS_SET_CAM_Y,
	GFS_SET_CAM_Z,
	GFS_SET_CAM_ANGLE,
	GFS_FLIP_MAP,
	GFS_REMOVE_GUNS,
	GFS_REMOVE_SCIONS,
	GFS_PLAY_SYNCED_AUDIO,
	GFS_MESH_SWAP,
	GFS_FIX_PYRAMID_SECRET_TRIGGER,
} GAMEFLOW_SEQUENCE_TYPE;

typedef enum GAME_STRING_ID
{
	GS_HEADING_INVENTORY,
	GS_HEADING_GAME_OVER,
	GS_HEADING_OPTION,
	GS_HEADING_ITEMS,

	GS_PASSPORT_SELECT_LEVEL,
	GS_PASSPORT_SELECT_MODE,
	GS_PASSPORT_MODE_NEW_GAME,
	GS_PASSPORT_MODE_NEW_GAME_PLUS,
	GS_PASSPORT_MODE_NEW_GAME_JP,
	GS_PASSPORT_MODE_NEW_GAME_JP_PLUS,
	GS_PASSPORT_NEW_GAME,
	GS_PASSPORT_LOAD_GAME,
	GS_PASSPORT_SAVE_GAME,
	GS_PASSPORT_EXIT_GAME,
	GS_PASSPORT_EXIT_TO_TITLE,

	GS_DETAIL_SELECT_DETAIL,
	GS_DETAIL_LEVEL_HIGH,
	GS_DETAIL_LEVEL_MEDIUM,
	GS_DETAIL_LEVEL_LOW,
	GS_DETAIL_PERSPECTIVE_FMT,
	GS_DETAIL_BILINEAR_FMT,
	GS_DETAIL_BRIGHTNESS_FMT,
	GS_DETAIL_UI_TEXT_SCALE_FMT,
	GS_DETAIL_UI_BAR_SCALE_FMT,
	GS_DETAIL_VIDEO_MODE_FMT,

	GS_SOUND_SET_VOLUMES,

	GS_CONTROL_DEFAULT_KEYS,
	GS_CONTROL_USER_KEYS,

	GS_KEYMAP_RUN,
	GS_KEYMAP_BACK,
	GS_KEYMAP_LEFT,
	GS_KEYMAP_RIGHT,
	GS_KEYMAP_STEP_LEFT,
	GS_KEYMAP_STEP_RIGHT,
	GS_KEYMAP_WALK,
	GS_KEYMAP_JUMP,
	GS_KEYMAP_ACTION,
	GS_KEYMAP_DRAW_WEAPON,
	GS_KEYMAP_LOOK,
	GS_KEYMAP_ROLL,
	GS_KEYMAP_INVENTORY,
	GS_KEYMAP_FLY_CHEAT,
	GS_KEYMAP_ITEM_CHEAT,
	GS_KEYMAP_LEVEL_SKIP_CHEAT,
	GS_KEYMAP_PAUSE,
	GS_KEYMAP_CAMERA_UP,
	GS_KEYMAP_CAMERA_DOWN,
	GS_KEYMAP_CAMERA_LEFT,
	GS_KEYMAP_CAMERA_RIGHT,
	GS_KEYMAP_CAMERA_RESET,

	GS_STATS_TIME_TAKEN_FMT,
	GS_STATS_SECRETS_FMT,
	GS_STATS_PICKUPS_FMT,
	GS_STATS_KILLS_FMT,

	GS_PAUSE_PAUSED,
	GS_PAUSE_EXIT_TO_TITLE,
	GS_PAUSE_CONTINUE,
	GS_PAUSE_QUIT,
	GS_PAUSE_ARE_YOU_SURE,
	GS_PAUSE_YES,
	GS_PAUSE_NO,

	GS_MISC_ON,
	GS_MISC_OFF,
	GS_MISC_EMPTY_SLOT_FMT,
	GS_MISC_DEMO_MODE,

	GS_INV_ITEM_MEDI,
	GS_INV_ITEM_BIG_MEDI,

	GS_INV_ITEM_PUZZLE1,
	GS_INV_ITEM_PUZZLE2,
	GS_INV_ITEM_PUZZLE3,
	GS_INV_ITEM_PUZZLE4,

	GS_INV_ITEM_KEY1,
	GS_INV_ITEM_KEY2,
	GS_INV_ITEM_KEY3,
	GS_INV_ITEM_KEY4,

	GS_INV_ITEM_PICKUP1,
	GS_INV_ITEM_PICKUP2,
	GS_INV_ITEM_LEADBAR,
	GS_INV_ITEM_SCION,

	GS_INV_ITEM_PISTOLS,
	GS_INV_ITEM_SHOTGUN,
	GS_INV_ITEM_MAGNUM,
	GS_INV_ITEM_UZI,
	GS_INV_ITEM_GRENADE,

	GS_INV_ITEM_PISTOL_AMMO,
	GS_INV_ITEM_SHOTGUN_AMMO,
	GS_INV_ITEM_MAGNUM_AMMO,
	GS_INV_ITEM_UZI_AMMO,

	GS_INV_ITEM_COMPASS,
	GS_INV_ITEM_GAME,
	GS_INV_ITEM_DETAILS,
	GS_INV_ITEM_SOUND,
	GS_INV_ITEM_CONTROLS,
	GS_INV_ITEM_GAMMA,
	GS_INV_ITEM_LARAS_HOME,

	GS_NUMBER_OF,
} GAME_STRING_ID;

//------------------------------------

typedef struct GAMEFLOW_LEVEL
{
	GAMEFLOW_LEVEL_TYPE level_type;
	int16_t music;
	char level_title[256];
	char level_file[256];
	char key1[256];
	char key2[256];
	char key3[256];
	char key4[256];
	char pickup1[256];
	char pickup2[256];
	char puzzle1[256];
	char puzzle2[256];
	char puzzle3[256];
	char puzzle4[256];
	int8_t demo;
	int16_t secrets;
} GAMEFLOW_LEVEL;

typedef struct GAMEFLOW
{
	const char *main_menu_background_path;
	int32_t gym_level_num;
	int32_t first_level_num;
	int32_t last_level_num;
	int32_t title_level_num;
	int32_t level_count;
	char save_game_fmt[256];
	int8_t has_demo;
	int32_t demo_delay;
	int8_t enable_game_modes;
	int8_t enable_save_crystals;

	GAMEFLOW_LEVEL levels[22];
	char strings[GS_NUMBER_OF][256];
} GAMEFLOW;

//------------------------------------

typedef enum BAR_TYPE
{
	BT_LARA_HEALTH = 0,
	BT_LARA_AIR = 1,
	BT_ENEMY_HEALTH = 2,
} BAR_TYPE;

typedef enum SOUND_PLAY_MODE
{
	SPM_NORMAL = 0,
	SPM_UNDERWATER = 1,
	SPM_ALWAYS = 2,
} SOUND_PLAY_MODE;

typedef enum GAME_BONUS_FLAG
{
	GBF_NGPLUS = 1 << 0,
	GBF_JAPANESE = 1 << 1,
} GAME_BONUS_FLAG;

#pragma pack(push, 1)

typedef struct RESOLUTION
{
	int width;
	int height;
} RESOLUTION;

typedef struct RGBF
{
	float r;
	float g;
	float b;
} RGBF;

typedef struct RGB888
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB888;

typedef struct POS_2D
{
	uint16_t x;
	uint16_t y;
} POS_2D;

typedef struct POS_3D
{
	uint16_t x;
	uint16_t y;
	uint16_t z;
} POS_3D;

typedef struct PHD_VECTOR
{
	int32_t x;
	int32_t y;
	int32_t z;
} PHD_VECTOR;

typedef struct PHD_MATRIX
{
	int32_t _00;
	int32_t _01;
	int32_t _02;
	int32_t _03;
	int32_t _10;
	int32_t _11;
	int32_t _12;
	int32_t _13;
	int32_t _20;
	int32_t _21;
	int32_t _22;
	int32_t _23;
} PHD_MATRIX;

typedef struct PHD_3DPOS
{
	int32_t x;
	int32_t y;
	int32_t z;
	int16_t x_rot;
	int16_t y_rot;
	int16_t z_rot;
} PHD_3DPOS;

typedef struct POINT_INFO
{
	float xv;
	float yv;
	float zv;
	float xs;
	float ys;
	float u;
	float v;
	float g;
} POINT_INFO;

typedef struct PHD_VBUF
{
	int32_t xv;
	int32_t yv;
	int32_t zv;
	int32_t xs;
	int32_t ys;
	int32_t dist;
	int16_t clip;
	int16_t g;
	uint16_t u;
	uint16_t v;
} PHD_VBUF;

typedef struct PHD_UV
{
	uint16_t u1;
	uint16_t v1;
} PHD_UV;

typedef struct PHD_TEXTURE
{
	uint16_t drawtype;
	uint16_t tpage;
	PHD_UV uv[4];
} PHD_TEXTURE;

/*
typedef struct PHD_SPRITE {
	uint16_t tpage;
	uint16_t offset;
	uint16_t width;
	uint16_t height;
	int16_t x1;
	int16_t y1;
	int16_t x2;
	int16_t y2;
} PHD_SPRITE;
*/

typedef struct DOOR_INFO
{
	int16_t room_num;
	int16_t x;
	int16_t y;
	int16_t z;
	POS_3D vertex[4];
} DOOR_INFO;

typedef struct DOOR_INFOS
{
	uint16_t count;
	// DOOR_INFO door[];
	DOOR_INFO *door;
} DOOR_INFOS;

typedef struct FLOOR_INFO
{
	uint16_t index;
	int16_t box;
	uint8_t pit_room;
	int8_t floor;
	uint8_t sky_room;
	int8_t ceiling;
} FLOOR_INFO;

typedef struct DOORPOS_DATA
{
	FLOOR_INFO *floor;
	FLOOR_INFO old_floor;
	int16_t block;
} DOORPOS_DATA;

typedef struct DOOR_DATA
{
	DOORPOS_DATA d1;
	DOORPOS_DATA d1flip;
	DOORPOS_DATA d2;
	DOORPOS_DATA d2flip;
} DOOR_DATA;

typedef struct LIGHT_INFO
{
	int32_t x;
	int32_t y;
	int32_t z;
	int16_t intensity;
	int32_t falloff;
} LIGHT_INFO;

typedef struct MESH_INFO
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
	PHD_ANGLE y_rot;
	uint16_t shade;
	uint16_t static_number;
} MESH_INFO;

typedef struct ROOM_INFO
{
	int16_t *data;
	DOOR_INFOS *doors;
	FLOOR_INFO *floor;
	LIGHT_INFO *light;
	MESH_INFO *static_mesh;
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t min_floor;
	int32_t max_ceiling;
	int16_t x_size;
	int16_t y_size;
	int16_t ambient;
	int16_t num_lights;
	uint16_t num_static_meshes;
	int16_t left;
	int16_t right;
	int16_t top;
	int16_t bottom;
	int16_t bound_active;
	int16_t item_number;
	int16_t fx_number;
	int16_t flipped_room;
	uint16_t flags;
} ROOM_INFO;

typedef struct ITEM_INFO
{
	int32_t floor;
	uint32_t touch_bits;
	uint32_t mesh_bits;
	int16_t object_number;
	int16_t current_anim_state;
	int16_t goal_anim_state;
	int16_t required_anim_state;
	int16_t anim_number;
	int16_t frame_number;
	int16_t room_number;
	int16_t next_item;
	int16_t next_active;
	int16_t speed;
	int16_t fall_speed;
	int16_t hit_points;
	int16_t box_number;
	int16_t timer;
	int16_t flags;
	int16_t shade;
	void *data;
	PHD_3DPOS pos;
	uint16_t active : 1;
	uint16_t status : 2;
	uint16_t gravity_status : 1;
	uint16_t hit_status : 1;
	uint16_t collidable : 1;
	uint16_t looked_at : 1;
} ITEM_INFO;

typedef struct LARA_ARM
{
	int16_t *frame_base;
	int16_t frame_number;
	int16_t lock;
	PHD_ANGLE y_rot;
	PHD_ANGLE x_rot;
	PHD_ANGLE z_rot;
	uint16_t flash_gun;
} LARA_ARM;

typedef struct AMMO_INFO
{
	int32_t ammo;
	int32_t hit;
	int32_t miss;
} AMMO_INFO;

typedef struct BOX_NODE
{
	int16_t exit_box;
	uint16_t search_number;
	int16_t next_expansion;
	int16_t box_number;
} BOX_NODE;

typedef struct LOT_INFO
{
	BOX_NODE *node;
	int16_t head;
	int16_t tail;
	uint16_t search_number;
	uint16_t block_mask;
	int16_t step;
	int16_t drop;
	int16_t fly;
	int16_t zone_count;
	int16_t target_box;
	int16_t required_box;
	PHD_VECTOR target;
} LOT_INFO;

typedef struct FX_INFO
{
	PHD_3DPOS pos;
	int16_t room_number;
	int16_t object_number;
	int16_t next_fx;
	int16_t next_active;
	int16_t speed;
	int16_t fall_speed;
	int16_t frame_number;
	int16_t counter;
	int16_t shade;
} FX_INFO;

typedef struct LARA_INFO
{
	int16_t item_number;
	int16_t gun_status;
	int16_t gun_type;
	int16_t request_gun_type;
	int16_t calc_fall_speed;
	int16_t water_status;
	int16_t pose_count;
	int16_t hit_frame;
	int16_t hit_direction;
	int16_t air;
	int16_t dive_count;
	int16_t death_count;
	int16_t current_active;
	int16_t spaz_effect_count;
	FX_INFO *spaz_effect;
	int32_t mesh_effects;
	int16_t *mesh_ptrs[LM_NUMBER_OF];
	ITEM_INFO *target;
	PHD_ANGLE target_angles[2];
	int16_t turn_rate;
	int16_t move_angle;
	int16_t head_y_rot;
	int16_t head_x_rot;
	int16_t head_z_rot;
	int16_t torso_y_rot;
	int16_t torso_x_rot;
	int16_t torso_z_rot;
	LARA_ARM left_arm;
	LARA_ARM right_arm;
	AMMO_INFO pistols;
	AMMO_INFO magnums;
	AMMO_INFO uzis;
	AMMO_INFO shotgun;
	LOT_INFO LOT;
} LARA_INFO;

typedef struct START_INFO
{
	uint16_t pistol_ammo;  //+0
	uint16_t magnum_ammo;  //+02
	uint16_t uzi_ammo;	 //+04
	uint16_t shotgun_ammo; //+06
	uint8_t num_medis;	 //+08
	uint8_t num_big_medis; //+09
	uint8_t num_scions;	//+0A
	int8_t gun_status;	 //+0B
	int8_t gun_type;	   //+0C
	//��������� ���� = +0D (1 ���� �������������� ��� packed-����)
	uint16_t available : 1;
	uint16_t got_pistols : 1;
	uint16_t got_magnums : 1;
	uint16_t got_uzis : 1;
	uint16_t got_shotgun : 1;
	uint16_t costume : 1;
} START_INFO;

typedef struct SAVEGAME_INFO
{
	START_INFO start[22];
	uint32_t timer;
	uint32_t kills;
	uint16_t secrets;
	uint16_t current_level;
	uint8_t pickups;
	uint8_t bonus_flag;
	uint8_t num_pickup1;
	uint8_t num_pickup2;
	uint8_t num_puzzle1;
	uint8_t num_puzzle2;
	uint8_t num_puzzle3;
	uint8_t num_puzzle4;
	uint8_t num_key1;
	uint8_t num_key2;
	uint8_t num_key3;
	uint8_t num_key4;
	uint8_t num_leadbar;
	uint8_t challenge_failed;
	char buffer[MAX_SAVEGAME_BUFFER];
} SAVEGAME_INFO;

typedef struct CREATURE_INFO
{
	int16_t head_rotation;
	int16_t neck_rotation;
	int16_t maximum_turn;
	uint16_t flags;
	int16_t item_num;
	MOOD_TYPE mood;
	LOT_INFO LOT;
	PHD_VECTOR target;
} CREATURE_INFO;

typedef struct TEXTSTRING
{
	union {
		uint32_t all;
		struct
		{
			uint32_t active : 1;
			uint32_t flash : 1;
			uint32_t rotate_h : 1;
			uint32_t centre_h : 1;
			uint32_t centre_v : 1;
			uint32_t right : 1;
			uint32_t bottom : 1;
			uint32_t background : 1;
			uint32_t outline : 1;
		};
	} flags;
	struct
	{
		int16_t x;
		int16_t y;
	} pos;
	int16_t letter_spacing;
	int16_t word_spacing;
	struct
	{
		int16_t rate;
		int16_t count;
	} flash;
	struct
	{
		int16_t x;
		int16_t y;
	} bgnd_size;
	struct
	{
		int16_t x;
		int16_t y;
	} bgnd_off;
	struct
	{
		int32_t h;
		int32_t v;
	} scale;
	char *string;

	// void (*on_remove)(const struct TEXTSTRING *);
} TEXTSTRING;

typedef struct DISPLAYPU
{
	int16_t duration;
	int16_t sprnum;
} DISPLAYPU;

typedef struct COLL_INFO
{
	int32_t mid_floor;
	int32_t mid_ceiling;
	int32_t mid_type;
	int32_t front_floor;
	int32_t front_ceiling;
	int32_t front_type;
	int32_t left_floor;
	int32_t left_ceiling;
	int32_t left_type;
	int32_t right_floor;
	int32_t right_ceiling;
	int32_t right_type;
	int32_t radius;
	int32_t bad_pos;
	int32_t bad_neg;
	int32_t bad_ceiling;
	PHD_VECTOR shift;
	PHD_VECTOR old;
	int16_t facing;
	int16_t quadrant;
	int16_t coll_type;
	int16_t *trigger;
	int8_t tilt_x;
	int8_t tilt_z;
	int8_t hit_by_baddie;
	int8_t hit_static;
	uint16_t slopes_are_walls : 1;
	uint16_t slopes_are_pits : 1;
	uint16_t lava_is_pit : 1;
	uint16_t enable_baddie_push : 1;
	uint16_t enable_spaz : 1;
} COLL_INFO;

typedef struct OBJECT_INFO
{
	int16_t nmeshes;
	int16_t mesh_index;
	int32_t bone_index;
	int16_t *frame_base;
	void (*initialise)(int16_t item_num);
	void (*control)(int16_t item_num);
	void (*floor)(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
				  int16_t *height);
	void (*ceiling)(ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
					int16_t *height);
	void (*draw_routine)(ITEM_INFO *item);
	void (*collision)(int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll);
	int16_t anim_index;
	int16_t hit_points;
	int16_t pivot_length;
	int16_t radius;
	int16_t smartness;
	int16_t shadow_size;
	uint16_t loaded : 1;
	uint16_t intelligent : 1;
	uint16_t save_position : 1;
	uint16_t save_hitpoints : 1;
	uint16_t save_flags : 1;
	uint16_t save_anim : 1;
} OBJECT_INFO;

typedef struct SHADOW_INFO
{
	int16_t x;
	int16_t y;
	int16_t z;
	int16_t radius;
	int16_t poly_count;
	int16_t vertex_count;
	POS_3D vertex[32];
} SHADOW_INFO;

typedef struct STATIC_INFO
{
	int16_t mesh_number;
	int16_t flags;
	int16_t x_minp;
	int16_t x_maxp;
	int16_t y_minp;
	int16_t y_maxp;
	int16_t z_minp;
	int16_t z_maxp;
	int16_t x_minc;
	int16_t x_maxc;
	int16_t y_minc;
	int16_t y_maxc;
	int16_t z_minc;
	int16_t z_maxc;
} STATIC_INFO;

typedef struct GAME_VECTOR
{
	int32_t x;
	int32_t y;
	int32_t z;
	int16_t room_number;
	int16_t box_number;
} GAME_VECTOR;

typedef struct OBJECT_VECTOR
{
	int32_t x;
	int32_t y;
	int32_t z;
	int16_t data;
	int16_t flags;
} OBJECT_VECTOR;

typedef struct CAMERA_INFO
{
	GAME_VECTOR pos;
	GAME_VECTOR target;
	int32_t type;
	int32_t shift;
	int32_t flags;
	int32_t fixed_camera;
	int32_t number_frames;
	int32_t bounce;
	int32_t underwater;
	int32_t target_distance;
	int32_t target_square;
	int16_t target_angle;
	int16_t actual_angle;
	int16_t target_elevation;
	int16_t box;
	int16_t number;
	int16_t last;
	int16_t timer;
	int16_t speed;
	ITEM_INFO *item;
	ITEM_INFO *last_item;
	OBJECT_VECTOR *fixed;
	// used for the manual camera control
	int16_t additional_angle;
	int16_t additional_elevation;
} CAMERA_INFO;

typedef struct ANIM_STRUCT
{
	int16_t *frame_ptr;
	int16_t interpolation;
	int16_t current_anim_state;
	int32_t velocity;
	int32_t acceleration;
	int16_t frame_base;
	int16_t frame_end;
	int16_t jump_anim_num;
	int16_t jump_frame_num;
	int16_t number_changes;
	int16_t change_index;
	int16_t number_commands;
	int16_t command_index;
} ANIM_STRUCT;

typedef struct ANIM_CHANGE_STRUCT
{
	int16_t goal_anim_state;
	int16_t number_ranges;
	int16_t range_index;
} ANIM_CHANGE_STRUCT;

typedef struct ANIM_RANGE_STRUCT
{
	int16_t start_frame;
	int16_t end_frame;
	int16_t link_anim_num;
	int16_t link_frame_num;
} ANIM_RANGE_STRUCT;

typedef struct DOOR_VBUF
{
	int32_t xv;
	int32_t yv;
	int32_t zv;
} DOOR_VBUF;

typedef struct WEAPON_INFO
{
	PHD_ANGLE lock_angles[4];
	PHD_ANGLE left_angles[4];
	PHD_ANGLE right_angles[4];
	PHD_ANGLE aim_speed;
	PHD_ANGLE shot_accuracy;
	int32_t gun_height;
	int32_t damage;
	int32_t target_dist;
	int16_t recoil_frame;
	int16_t flash_time;
	int16_t sample_num;
} WEAPON_INFO;

typedef struct SPHERE
{
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t r;
} SPHERE;

typedef struct BITE_INFO
{
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t mesh_num;
} BITE_INFO;

typedef struct AI_INFO
{
	int16_t zone_number;
	int16_t enemy_zone;
	int32_t distance;
	int32_t ahead;
	int32_t bite;
	int16_t angle;
	int16_t enemy_facing;
} AI_INFO;

typedef struct BOX_INFO
{
	int32_t left;
	int32_t right;
	int32_t top;
	int32_t bottom;
	int16_t height;
	int16_t overlap_index;
} BOX_INFO;

typedef struct REQUEST_INFO
{
	uint16_t items;			  // ���������� ��������� (������)
	uint16_t requested;		  // ������ ���������� ��������
	uint16_t vis_lines;		  // ���������� ������������ ����� (������ ����)
	uint16_t line_offset;	 // ��������� ������ (�� ������� ����� ������� ���)
	uint16_t line_old_offset; // ���������� �������� offset � ����� ���� ���
							  // ���������
	uint16_t pix_width;		  // ������ ���� � ��������
	uint16_t line_height;	 // ������ ������ � ��������
	int16_t x;				  // ������� ���� (z �� ������������)
	int16_t y;
	int16_t z;				  // unused
	uint16_t flags;			  // ����� (����� ����, ����� �����������)
	const char *heading_text; // ��������� ���� (������� �����)
	char *item_texts; // ��������� �� ������ ������� (��� ��������� ����� ����)
	// char *item_texts[16][48]; //��� ���� ������ m_LoadSaveGameStrings
	int16_t item_text_len; // ����� ����� ������ ������ � ������� `item_texts`
	TEXTSTRING *heading;   // ����������� ������� (������, ���, ������� � �.�.)
	TEXTSTRING *background;
	TEXTSTRING *moreup;
	TEXTSTRING *moredown;
	TEXTSTRING *texts[MAX_REQLINES];   // ��������� �� ������������ ������
	uint16_t item_flags[MAX_REQLINES]; // ����� ��� ����� (��������, ����������
									   // ������)
} REQUEST_INFO;

typedef struct IMOTION_INFO
{
	int16_t count;
	int16_t status;
	int16_t status_target;
	int16_t radius_target;
	int16_t radius_rate;
	int16_t camera_ytarget;
	int16_t camera_yrate;
	int16_t camera_pitch_target;
	int16_t camera_pitch_rate;
	int16_t rotate_target;
	int16_t rotate_rate;
	PHD_ANGLE item_ptxrot_target;
	PHD_ANGLE item_ptxrot_rate;
	PHD_ANGLE item_xrot_target;
	PHD_ANGLE item_xrot_rate;
	int32_t item_ytrans_target;
	int32_t item_ytrans_rate;
	int32_t item_ztrans_target;
	int32_t item_ztrans_rate;
	int32_t misc;
} IMOTION_INFO;

typedef struct INVENTORY_SPRITE
{
	int16_t shape;
	int16_t x;
	int16_t y;
	int16_t z;
	int32_t param1;
	int32_t param2;
	SG_COL *grdptr;
	int16_t sprnum;
} INVENTORY_SPRITE;

typedef struct INVENTORY_ITEM
{
	char *string;
	int16_t object_number;
	int16_t frames_total;
	int16_t current_frame;
	int16_t goal_frame;
	int16_t open_frame;
	int16_t anim_direction;
	int16_t anim_speed;
	int16_t anim_count;
	PHD_ANGLE pt_xrot_sel;
	PHD_ANGLE pt_xrot;
	PHD_ANGLE x_rot_sel;
	PHD_ANGLE x_rot;
	PHD_ANGLE y_rot_sel;
	PHD_ANGLE y_rot;
	int32_t ytrans_sel;
	int32_t ytrans;
	int32_t ztrans_sel;
	int32_t ztrans;
	uint32_t which_meshes;
	uint32_t drawn_meshes;
	int16_t inv_pos;
	INVENTORY_SPRITE **sprlist;
} INVENTORY_ITEM;

typedef struct RING_INFO
{
	INVENTORY_ITEM **list;
	int16_t type;
	int16_t radius;
	int16_t camera_pitch;
	int16_t rotating;
	int16_t rot_count;
	int16_t current_object;
	int16_t target_object;
	int16_t number_of_objects;
	int16_t angle_adder;
	int16_t rot_adder;
	int16_t rot_adder_l;
	int16_t rot_adder_r;
	PHD_3DPOS ringpos;
	PHD_3DPOS camera;
	PHD_VECTOR light;
	IMOTION_INFO *imo;
} RING_INFO;

typedef struct SAMPLE_INFO
{
	int16_t number;
	int16_t volume;
	int16_t randomness;
	int16_t flags;
} SAMPLE_INFO;

#pragma pack(pop)

typedef struct PICTURE
{
	int32_t width;
	int32_t height;
	RGB888 *data;
} PICTURE;

typedef union INPUT_STATE {
	uint32_t any;
	struct
	{
		uint32_t forward : 1;
		uint32_t back : 1;
		uint32_t left : 1;
		uint32_t right : 1;
		uint32_t jump : 1;
		uint32_t draw : 1;
		uint32_t action : 1;
		uint32_t slow : 1;
		uint32_t option : 1;
		uint32_t look : 1;
		uint32_t step_left : 1;
		uint32_t step_right : 1;
		uint32_t roll : 1;
		uint32_t pause : 1;
		uint32_t select : 1;
		uint32_t deselect : 1;
		uint32_t save : 1;
		uint32_t load : 1;
		uint32_t fly_cheat : 1;
		uint32_t item_cheat : 1;
		uint32_t level_skip_cheat : 1;
		uint32_t health_cheat : 1;
		uint32_t camera_up : 1;
		uint32_t camera_down : 1;
		uint32_t camera_left : 1;
		uint32_t camera_right : 1;
		uint32_t camera_reset : 1;
	};
} INPUT_STATE;

typedef enum INPUT_KEY
{
	INPUT_KEY_UP = 0,
	INPUT_KEY_DOWN = 1,
	INPUT_KEY_LEFT = 2,
	INPUT_KEY_RIGHT = 3,
	INPUT_KEY_STEP_L = 4,
	INPUT_KEY_STEP_R = 5,
	INPUT_KEY_SLOW = 6,
	INPUT_KEY_JUMP = 7,
	INPUT_KEY_ACTION = 8,
	INPUT_KEY_DRAW = 9,
	INPUT_KEY_LOOK = 10,
	INPUT_KEY_ROLL = 11,
	INPUT_KEY_OPTION = 12,
	INPUT_KEY_NUMBER_OF = 13,
} INPUT_KEY;

typedef enum INPUT_LAYOUT
{
	INPUT_LAYOUT_DEFAULT,
	INPUT_LAYOUT_USER,
	INPUT_LAYOUT_NUMBER_OF,
} INPUT_LAYOUT;

// typedef struct APP_CONFIG {
struct CONFIG
{
	/*
	bool disable_healing_between_levels;
	bool disable_medpacks;
	bool disable_magnums;
	bool disable_uzis;
	bool disable_shotgun;
	bool enable_enemy_healthbar;
	bool enable_enhanced_look;
	bool enable_numeric_keys;
	bool enable_shotgun_flash;
	bool enable_cheats;
	bool enable_tr3_sidesteps;
	bool enable_braid;
	bool enable_compass_stats;
	bool enable_timer_in_inventory;
	bool enable_smooth_bars;
	int8_t healthbar_showing_mode;
	int8_t healthbar_location;
	int8_t healthbar_color;
	int8_t airbar_showing_mode;
	int8_t airbar_location;
	int8_t airbar_color;
	int8_t enemy_healthbar_location;
	int8_t enemy_healthbar_color;
	bool fix_tihocan_secret_sound;
	bool fix_pyramid_secret_trigger;
	bool fix_secrets_killing_music;
	bool fix_descending_glitch;
	bool fix_wall_jump_glitch;
	bool fix_qwop_glitch;
	int32_t fov_value;
	bool fov_vertical;
	bool disable_demo;
	bool disable_fmv;
	bool disable_cine;
	bool disable_music_in_menu;
	int32_t resolution_width;
	int32_t resolution_height;
	bool enable_xbox_one_controller;
	float brightness;
	bool enable_round_shadow;
	bool enable_3d_pickups;

	struct {
		int32_t layout;
	} input;

	struct {
		uint32_t perspective : 1;
		uint32_t bilinear : 1;
		uint32_t fps_counter : 1;
	} render_flags;

	struct {
		double text_scale;
		double bar_scale;
	} ui;

	int32_t sound_volume;
	int32_t music_volume;

	SCREENSHOT_FORMAT screenshot_format;
	*/

	bool enable_enemy_healthbar;
	int8_t enemy_healthbar_location;
	int8_t enemy_healthbar_color;
	int8_t healthbar_showing_mode;
	int8_t healthbar_location;
	int8_t healthbar_color;
	int8_t airbar_showing_mode;
	int8_t airbar_location;
	int8_t airbar_color;

	bool enable_compass_stats;

	struct render_flags
	{
		uint32_t perspective : 1;
		uint32_t bilinear : 1;
		uint32_t fps_counter : 1;
	} render_flags;

	struct input
	{
		int32_t layout;
	} input;

	float brightness;

	struct ui
	{
		double text_scale;
		double bar_scale;
	} ui;

	int32_t sound_volume;
	int32_t music_volume;
};

#pragma pack(pop, foo)
