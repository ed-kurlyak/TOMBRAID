

#include "vars.h"
#include "lara.h"
#include "screen.h"
#include "types.h"
#include <windows.h>

float BRIGHTNESS = 1.0f;

//LPDIRECT3DTEXTURE9 g_pTexture;

uint32_t TexturePageCount;


LPDIRECT3DTEXTURE9* m_pLevelTile = NULL;
LPDIRECT3DDEVICE9 g_d3d_Device;
LPDIRECT3D9 g_d3d;
IDirect3DVertexDeclaration9* g_pVertDeclTexColor;
IDirect3DVertexDeclaration9* g_pVertDeclColor;
IDirect3DVertexDeclaration9* g_pVertDeclLines;
ID3DXEffect* pEffectTexColor;
D3DXHANDLE g_hTechTexColor;

TEXTUREBUCKET_OPAQUE Bucket_Tex_Color_Opaque[MAXBUCKETS];
COLOREDBUCKET Bucket_Colored;
LINESBUCKET Bucket_Lines;
TRANSQUADBUCKET Bucket_TransQuad;

int g_LevelNumTR;

int Game_Finished = 0;

//#include "inv.h"

/*






INVENTORY_ITEM *g_InvMainList[23] = {
	&g_InvItemCompass,
	&g_InvItemPistols,
	&g_InvItemShotgun,
	&g_InvItemMagnum,
	&g_InvItemUzi,
	&g_InvItemGrenade,
	&g_InvItemBigMedi,
	&g_InvItemMedi,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

int16_t g_InvMainObjects = 8;

int16_t g_InvMainQtys[24] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

*/

GAMEFLOW g_GameFlow;

int ColorLighting1 = 0;
int ColorLighting2 = 0;

//-------------------------
void (*g_EffectRoutines[])(ITEM_INFO *item) = {
	Turn180,	DinoStomp, LaraNormal,	LaraBubbles,  FinishLevel,
	EarthQuake, Flood,	 RaisingBlock,  Stairs2Slope, DropSand,
	PowerUp,	Explosion, LaraHandsFree, FxFlipMap,	LaraDrawRightGun,
	ChainBlock, Flicker,
};

/*
void (*g_LaraControlRoutines[])(ITEM_INFO *item, COLL_INFO *coll) = {
	LaraAsWalk,      LaraAsRun,       LaraAsStop,      LaraAsForwardJump,
	LaraAsPose,      LaraAsFastBack,  LaraAsTurnR,     LaraAsTurnL,
	LaraAsDeath,     LaraAsFastFall,  LaraAsHang,      LaraAsReach,
	LaraAsSplat,     LaraAsTread,     LaraAsLand,      LaraAsCompress,
	LaraAsBack,      LaraAsSwim,      LaraAsGlide,     LaraAsNull,
	LaraAsFastTurn,  LaraAsStepRight, LaraAsStepLeft,  LaraAsRoll2,
	LaraAsSlide,     LaraAsBackJump,  LaraAsRightJump, LaraAsLeftJump,
	LaraAsUpJump,    LaraAsFallBack,  LaraAsHangLeft,  LaraAsHangRight,
	LaraAsSlideBack, LaraAsSurfTread, LaraAsSurfSwim,  LaraAsDive,
	LaraAsPushBlock, LaraAsPullBlock, LaraAsPPReady,   LaraAsPickup,
	LaraAsSwitchOn,  LaraAsSwitchOff, LaraAsUseKey,    LaraAsUsePuzzle,
	LaraAsUWDeath,   LaraAsRoll,      LaraAsSpecial,   LaraAsSurfBack,
	LaraAsSurfLeft,  LaraAsSurfRight, LaraAsUseMidas,  LaraAsDieMidas,
	LaraAsSwanDive,  LaraAsFastDive,  LaraAsGymnast,   LaraAsWaterOut,
};

void (*g_LaraCollisionRoutines[])(ITEM_INFO *item, COLL_INFO *coll) = {
	LaraColWalk,      LaraColRun,       LaraColStop,      LaraColForwardJump,
	LaraColPose,      LaraColFastBack,  LaraColTurnR,     LaraColTurnL,
	LaraColDeath,     LaraColFastFall,  LaraColHang,      LaraColReach,
	LaraColSplat,     LaraColTread,     LaraColLand,      LaraColCompress,
	LaraColBack,      LaraColSwim,      LaraColGlide,     LaraColNull,
	LaraColFastTurn,  LaraColStepRight, LaraColStepLeft,  LaraColRoll2,
	LaraColSlide,     LaraColBackJump,  LaraColRightJump, LaraColLeftJump,
	LaraColUpJump,    LaraColFallBack,  LaraColHangLeft,  LaraColHangRight,
	LaraColSlideBack, LaraColSurfTread, LaraColSurfSwim,  LaraColDive,
	LaraColPushBlock, LaraColPullBlock, LaraColPPReady,   LaraColPickup,
	LaraColSwitchOn,  LaraColSwitchOff, LaraColUseKey,    LaraColUsePuzzle,
	LaraColUWDeath,   LaraColRoll,      LaraColSpecial,   LaraColSurfBack,
	LaraColSurfLeft,  LaraColSurfRight, LaraColUseMidas,  LaraColDieMidas,
	LaraColSwanDive,  LaraColFastDive,  LaraColGymnast,   LaraColWaterOut,
};

*/

int CurrentRoom;
int g_CameraUnderwater;

INPUT_STATE g_Input = {0};
INPUT_STATE g_InputDB = {0};
INPUT_STATE g_OldInputDB = {0};

int16_t m_InterpolatedBounds[6] = {0};

PHD_3DPOS m_Hair[HAIR_SEGMENTS + 1] = {0};

int32_t m_SlotsUsed = 0;
CREATURE_INFO *m_BaddieSlots = NULL;

float g_SurfaceMinX;
float g_SurfaceMinY;
float g_SurfaceMaxX;
float g_SurfaceMaxY;

int16_t g_PhdWinxmin;
int16_t g_PhdWinymin;
int16_t g_PhdWinxmax;
int16_t g_PhdWinymax;
int32_t g_PhdScrwidth;
int32_t g_PhdWinwidth;

// int32_t ViewPort_GetCenterX() = SCREEN_WIDTH  / 2;
// int32_t ViewPort_GetCenterY() = SCREEN_HEIGHT / 2;

int32_t surfacenum;

int16_t *info3dptr; // current pointer to info section
int32_t *sort3dptr; // current pointer to Sort section

int32_t sort3d_buffer[MAX_POLYGONS][2];   // buffer for sort info
int16_t info3d_buffer[MAX_POLYGONS * 30]; // buffer for print info

PHD_VBUF m_VBuf[1500] = {0};

char *phd_winptr = NULL;

// int g_PhdWinwidth = Screen_GetResWidth();

char depthq_table[32][256] = {0};  // depth cueing tables..
char gouraud_table[256][32] = {0}; // Gouraud shade table..

RGB888 GamePalette[256];
RGB888 GameNormalPalette[256];
RGB888 GameWaterPalette[256];

// PHD_SPRITE g_PhdSpriteInfo[MAX_SPRITES];

int8_t *TexturePagePtrs[MAX_TEXTPAGES];

char *LevelNamesGold[] = {"data\\gym.phd",  "data\\EGYPT.phd",
						  "data\\CAT.phd",  "data\\END.phd",
						  "data\\END2.phd", "data\\title.phd"};

char *LevelNamesTR1[] = {
	"data\\gym.phd",	  "data\\level1.phd",   "data\\level2.phd",
	"data\\level3a.phd",  "data\\level3b.phd",  "data\\level4.phd",
	"data\\level5.phd",   "data\\level6.phd",   "data\\level7a.phd",
	"data\\level7b.phd",  "data\\level8a.phd",  "data\\level8b.phd",
	"data\\level8c.phd",  "data\\level10a.phd", "data\\level10b.phd",
	"data\\level10c.phd", "data\\cut1.phd",		"data\\cut2.phd",
	"data\\cut3.phd",	 "data\\cut4.phd",		"data\\title.phd",
	"data\\current.phd"};

PHD_VECTOR g_LsVectorView = {0};

char *GameMemoryPointer = NULL;
int32_t g_FPSCounter = 0;

// int16_t sample_lut[MAX_SOUND_SAMPLES];
int32_t NumSampleInfos;
SOUND_SAMPLE_INFO *g_SampleInfos;

int16_t g_SampleLUT[MAX_SAMPLES] = {0};
// SAMPLE_INFO *g_SampleInfos = NULL;
uint16_t g_MusicTrackFlags[MAX_CD_TRACKS] = {0};

int32_t g_NoInputCount = 0;
bool g_IDelay = false;
int32_t g_IDCount = 0;
int32_t g_OptionSelected = 0;

int32_t g_PhdPersp = 0;
int32_t g_PhdLeft = 0;
int32_t g_PhdBottom = 0;
int32_t g_PhdRight = 0;
int32_t g_PhdTop = 0;
PHD_SPRITE g_PhdSpriteInfo[MAX_SPRITES] = {0};
PHD_TEXTURE g_PhdTextureInfo[MAX_TEXTURES] = {0};
PHD_MATRIX *g_PhdMatrixPtr = NULL;
PHD_MATRIX g_W2VMatrix = {0};

int32_t g_WibbleOffset = 0;
int32_t g_WibbleTable[WIBBLE_SIZE] = {0};
int32_t g_ShadeTable[WIBBLE_SIZE] = {0};
int32_t g_RandTable[WIBBLE_SIZE] = {0};

bool g_ModeLock = false;

LARA_INFO g_Lara = {0};
ITEM_INFO *g_LaraItem = NULL;
CAMERA_INFO g_Camera = {0};
SAVEGAME_INFO g_SaveGame = {0};
int32_t g_SavedGamesCount = 0;
int32_t g_SaveCounter = 0;
int32_t g_CurrentLevel = -1;
uint32_t *g_DemoData = NULL;
bool g_LevelComplete = false;
bool g_ResetFlag = false;
bool g_ChunkyFlag = false;
int32_t g_OverlayFlag = 0;
int32_t g_HeightType = 0;

int32_t g_HealthBarTimer = 0;
int16_t g_StoredLaraHealth = 0;

ROOM_INFO *g_RoomInfo = NULL;
int16_t *g_FloorData = NULL;
int16_t *g_MeshBase = NULL;
int16_t **g_Meshes = NULL;
OBJECT_INFO g_Objects[O_NUMBER_OF] = {0};
STATIC_INFO g_StaticObjects[STATIC_NUMBER_OF] = {0};
uint8_t *g_TexturePagePtrs[MAX_TEXTPAGES] = {NULL};
int16_t g_RoomCount = 0;
int32_t g_LevelItemCount = 0;
ITEM_INFO *g_Items = NULL;
int16_t g_NextItemFree = -1;
int16_t g_NextItemActive = -1;
FX_INFO *g_Effects = NULL;
int16_t g_NextFxFree = -1;
int16_t g_NextFxActive = -1;
int32_t g_NumberBoxes = 0;
BOX_INFO *g_Boxes = NULL;
uint16_t *g_Overlap = NULL;
int16_t *g_GroundZone[2] = {NULL};
int16_t *g_GroundZone2[2] = {NULL};
int16_t *g_FlyZone[2] = {NULL};
ANIM_STRUCT *g_Anims = NULL;
ANIM_CHANGE_STRUCT *g_AnimChanges = NULL;
ANIM_RANGE_STRUCT *g_AnimRanges = NULL;
int16_t *g_AnimTextureRanges = NULL;
int16_t *g_AnimCommands = NULL;
int32_t *g_AnimBones = NULL;
int16_t *g_AnimFrames = NULL;
int16_t *g_Cine = NULL;
int16_t g_NumCineFrames = 0;
int16_t g_CineFrame = -1;
PHD_3DPOS g_CinePosition = {0};
int32_t g_NumberCameras = 0;
int32_t g_NumberSoundEffects = 0;
OBJECT_VECTOR *g_SoundEffectsTable = NULL;
int16_t g_RoomsToDraw[MAX_ROOMS_TO_DRAW] = {-1};
int16_t g_RoomsToDrawCount = 0;
int g_IsWibbleEffect = false;
bool g_IsWaterEffect = false;
int g_IsShadeEffect = false;

int16_t *g_TriggerIndex = NULL;
int32_t g_FlipTimer = 0;
int32_t g_FlipEffect = -1;
int32_t g_FlipStatus = 0;
int32_t g_FlipMapTable[MAX_FLIP_MAPS] = {0};

int16_t g_InvMode = INV_TITLE_MODE;
int32_t g_InvExtraData[8] = {0};
int16_t g_InvChosen = -1;

int32_t g_LsAdder = 0;
int32_t g_LsDivider = 0;
SHADOW_INFO g_ShadowInfo = {0};

RESOLUTION g_AvailableResolutions[RESOLUTIONS_SIZE] = {
	{320, 200},   {512, 384},   {640, 480},   {800, 600},
	{1024, 768},  {1280, 720},  {1920, 1080}, {2560, 1440},
	{3840, 2160}, {4096, 2160}, {7680, 4320}, {-1, -1},
};

//------------------------------------

/*
STRING_DEF m_StringDefs[] = {
	{ GS_HEADING_INVENTORY, "INVENTORY" },
	{ GS_HEADING_GAME_OVER, "GAME OVER" },
	{ GS_HEADING_OPTION, "OPTION" },
	{ GS_HEADING_ITEMS, "ITEMS" },
	{ GS_PASSPORT_SELECT_LEVEL, "Select Level" },
	{ GS_PASSPORT_SELECT_MODE, "Select Mode" },
	{ GS_PASSPORT_MODE_NEW_GAME, "New Game" },
	{ GS_PASSPORT_MODE_NEW_GAME_PLUS, "New Game+" },
	{ GS_PASSPORT_MODE_NEW_GAME_JP, "Japanese NG" },
	{ GS_PASSPORT_MODE_NEW_GAME_JP_PLUS, "Japanese NG+" },
	{ GS_PASSPORT_NEW_GAME, "New Game" },
	{ GS_PASSPORT_LOAD_GAME, "Load Game" },
	{ GS_PASSPORT_SAVE_GAME, "Save Game" },
	{ GS_PASSPORT_EXIT_GAME, "Exit Game" },
	{ GS_PASSPORT_EXIT_TO_TITLE, "Exit to Title" },
	{ GS_DETAIL_SELECT_DETAIL, "Select Detail" },
	{ GS_DETAIL_LEVEL_HIGH, "High" },
	{ GS_DETAIL_LEVEL_MEDIUM, "Medium" },
	{ GS_DETAIL_LEVEL_LOW, "Low" },
	{ GS_DETAIL_PERSPECTIVE_FMT, "Perspective     %s" },
	{ GS_DETAIL_BILINEAR_FMT, "Bilinear        %s" },
	{ GS_DETAIL_BRIGHTNESS_FMT, "Brightness      %.1f" },
	{ GS_DETAIL_UI_TEXT_SCALE_FMT, "UI text scale   %.1f" },
	{ GS_DETAIL_UI_BAR_SCALE_FMT, "UI bar scale    %.1f" },
	{ GS_DETAIL_VIDEO_MODE_FMT, "Game Video Mode %s" },
	{ GS_SOUND_SET_VOLUMES, "Set Volumes" },
	{ GS_CONTROL_DEFAULT_KEYS, "Default Keys" },
	{ GS_CONTROL_USER_KEYS, "User Keys" },
	{ GS_KEYMAP_RUN, "Run" },
	{ GS_KEYMAP_BACK, "Back" },
	{ GS_KEYMAP_LEFT, "Left" },
	{ GS_KEYMAP_RIGHT, "Right" },
	{ GS_KEYMAP_STEP_LEFT, "Step Left" },
	{ GS_KEYMAP_STEP_RIGHT, "Step Right" },
	{ GS_KEYMAP_WALK, "Walk" },
	{ GS_KEYMAP_JUMP, "Jump" },
	{ GS_KEYMAP_ACTION, "Action" },
	{ GS_KEYMAP_DRAW_WEAPON, "Draw Weapon" },
	{ GS_KEYMAP_LOOK, "Look" },
	{ GS_KEYMAP_ROLL, "Roll" },
	{ GS_KEYMAP_INVENTORY, "Inventory" },
	{ GS_KEYMAP_PAUSE, "Pause" },
	{ GS_KEYMAP_FLY_CHEAT, "Fly cheat" },
	{ GS_KEYMAP_ITEM_CHEAT, "Item cheat" },
	{ GS_KEYMAP_LEVEL_SKIP_CHEAT, "Level skip" },
	{ GS_KEYMAP_CAMERA_UP, "Camera Up" },
	{ GS_KEYMAP_CAMERA_DOWN, "Camera Down" },
	{ GS_KEYMAP_CAMERA_LEFT, "Camera Left" },
	{ GS_KEYMAP_CAMERA_RIGHT, "Camera Right" },
	{ GS_KEYMAP_CAMERA_RESET, "Reset Camera" },
	{ GS_STATS_TIME_TAKEN_FMT, "TIME TAKEN %s" },
	{ GS_STATS_SECRETS_FMT, "SECRETS %d OF %d" },
	{ GS_STATS_PICKUPS_FMT, "PICKUPS %d" },
	{ GS_STATS_KILLS_FMT, "KILLS %d" },
	{ GS_PAUSE_PAUSED, "Paused" },
	{ GS_PAUSE_EXIT_TO_TITLE, "Exit to title?" },
	{ GS_PAUSE_CONTINUE, "Continue" },
	{ GS_PAUSE_QUIT, "Quit" },
	{ GS_PAUSE_ARE_YOU_SURE, "Are you sure?" },
	{ GS_PAUSE_YES, "Yes" },
	{ GS_PAUSE_NO, "No" },
	{ GS_MISC_ON, "On" },
	{ GS_MISC_OFF, "Off" },
	{ GS_MISC_EMPTY_SLOT_FMT, "- EMPTY SLOT %d -" },
	{ GS_MISC_DEMO_MODE, "Demo Mode" },
	{ GS_INV_ITEM_MEDI, "Small Medi Pack" },
	{ GS_INV_ITEM_BIG_MEDI, "Large Medi Pack" },
	{ GS_INV_ITEM_PUZZLE1, "Puzzle" },
	{ GS_INV_ITEM_PUZZLE2, "Puzzle" },
	{ GS_INV_ITEM_PUZZLE3, "Puzzle" },
	{ GS_INV_ITEM_PUZZLE4, "Puzzle" },
	{ GS_INV_ITEM_KEY1, "Key" },
	{ GS_INV_ITEM_KEY2, "Key" },
	{ GS_INV_ITEM_KEY3, "Key" },
	{ GS_INV_ITEM_KEY4, "Key" },
	{ GS_INV_ITEM_PICKUP1, "Pickup" },
	{ GS_INV_ITEM_PICKUP2, "Pickup" },
	{ GS_INV_ITEM_LEADBAR, "Lead Bar" },
	{ GS_INV_ITEM_SCION, "Scion" },
	{ GS_INV_ITEM_PISTOLS, "Pistols" },
	{ GS_INV_ITEM_SHOTGUN, "Shotgun" },
	{ GS_INV_ITEM_MAGNUM, "Magnums" },
	{ GS_INV_ITEM_UZI, "Uzis" },
	{ GS_INV_ITEM_GRENADE, "Grenade" },
	{ GS_INV_ITEM_PISTOL_AMMO, "Pistol Clips" },
	{ GS_INV_ITEM_SHOTGUN_AMMO, "Shotgun Shells" },
	{ GS_INV_ITEM_MAGNUM_AMMO, "Magnum Clips" },
	{ GS_INV_ITEM_UZI_AMMO, "Uzi Clips" },
	{ GS_INV_ITEM_COMPASS, "Compass" },
	{ GS_INV_ITEM_GAME, "Game" },
	{ GS_INV_ITEM_DETAILS, "Detail Levels" },
	{ GS_INV_ITEM_SOUND, "Sound" },
	{ GS_INV_ITEM_CONTROLS, "Controls" },
	{ GS_INV_ITEM_LARAS_HOME, "Lara's Home" },
	{ 0, NULL },
};

*/

// CONFIG g_Config = { 0 };
CONFIG g_Config;