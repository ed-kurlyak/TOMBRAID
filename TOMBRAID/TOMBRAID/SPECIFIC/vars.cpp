

#include <windows.h>
#include "vars.h"
#include "types.h"
#include "lara.h"
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




int color_tor_lighting = 0;
int color_tor_lighting2 = 0;

//-------------------------
void (*g_EffectRoutines[])(ITEM_INFO *item) = {
    Turn180,    DinoStomp, LaraNormal,    LaraBubbles,  FinishLevel,
    EarthQuake, Flood,     RaisingBlock,  Stairs2Slope, DropSand,
    PowerUp,    Explosion, LaraHandsFree, FxFlipMap,    LaraDrawRightGun,
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

int16_t m_InterpolatedBounds[6] = { 0 };

PHD_3DPOS m_Hair[HAIR_SEGMENTS + 1] = { 0 };

int32_t m_SlotsUsed = 0;
CREATURE_INFO *m_BaddieSlots = NULL;

float g_SurfaceMinX;
float g_SurfaceMinY;
float g_SurfaceMaxX;
float g_SurfaceMaxY;

int32_t g_CenterX = SCREEN_WIDTH  / 2;
int32_t g_CenterY = SCREEN_HEIGHT / 2;

int32_t surfacenum;

int16_t		*info3dptr;                			// current pointer to info section
int32_t		*sort3dptr;                			// current pointer to Sort section

int32_t		sort3d_buffer[MAX_POLYGONS][2];		// buffer for sort info
int16_t		info3d_buffer[MAX_POLYGONS*30];		// buffer for print info


PHD_VBUF m_VBuf[1500] = { 0 };

char * phd_winptr = NULL;

int phd_winwidth = SCREEN_WIDTH;

char		depthq_table[32][256] = {0};			 	// depth cueing tables..
char		gouraud_table[256][32] = {0};          	// Gouraud shade table..

RGB888 GamePalette[256];

PHDSPRITESTRUCT phdsprinfo[MAX_SPRITES];

int8_t *texture_page_ptrs[MAX_TEXTPAGES];

char * LevelNamesGold[] = {
"data\\gym.phd",
"data\\EGYPT.phd",
"data\\CAT.phd",
"data\\END.phd",
"data\\END2.phd" };


char * LevelNamesTR1[] = {
"data\\gym.phd",
"data\\level1.phd",
"data\\level2.phd",
"data\\level3a.phd",
"data\\level3b.phd",
"data\\level4.phd",
"data\\level5.phd",
"data\\level6.phd",
"data\\level7a.phd",
"data\\level7b.phd",
"data\\level8a.phd",
"data\\level8b.phd",
"data\\level8c.phd",
"data\\level10a.phd",
"data\\level10b.phd",
"data\\level10c.phd",
"data\\cut1.phd",
"data\\cut2.phd",
"data\\cut3.phd",
"data\\cut4.phd",
"data\\title.phd",
"data\\current.phd" };

PHD_VECTOR g_LsVectorView = { 0 };

char *GameMemoryPointer = NULL;
int32_t g_FPSCounter = 0;

//int16_t sample_lut[MAX_SOUND_SAMPLES];
int32_t num_sample_infos;
SOUND_SAMPLE_INFO* g_SampleInfos;


int16_t g_SampleLUT[MAX_SAMPLES] = { 0 };
//SAMPLE_INFO *g_SampleInfos = NULL;
uint16_t g_MusicTrackFlags[MAX_CD_TRACKS] = { 0 };

int32_t g_NoInputCount = 0;
bool g_IDelay = false;
int32_t g_IDCount = 0;
int32_t g_OptionSelected = 0;

int32_t g_PhdPersp = 0;
int32_t g_PhdLeft = 0;
int32_t g_PhdBottom = 0;
int32_t g_PhdRight = 0;
int32_t g_PhdTop = 0;
PHD_SPRITE g_PhdSpriteInfo[MAX_SPRITES] = { 0 };
PHD_TEXTURE g_PhdTextureInfo[MAX_TEXTURES] = { 0 };
PHD_MATRIX *g_PhdMatrixPtr = NULL;
PHD_MATRIX g_W2VMatrix = { 0 };

int32_t g_WibbleOffset = 0;
int32_t g_WibbleTable[WIBBLE_SIZE] = { 0 };
int32_t g_ShadeTable[WIBBLE_SIZE] = { 0 };
int32_t g_RandTable[WIBBLE_SIZE] = { 0 };

bool g_ModeLock = false;

LARA_INFO g_Lara = { 0 };
ITEM_INFO *g_LaraItem = NULL;
CAMERA_INFO g_Camera = { 0 };
SAVEGAME_INFO g_SaveGame = { 0 };
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
OBJECT_INFO g_Objects[O_NUMBER_OF] = { 0 };
STATIC_INFO g_StaticObjects[STATIC_NUMBER_OF] = { 0 };
uint8_t *g_TexturePagePtrs[MAX_TEXTPAGES] = { NULL };
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
int16_t *g_GroundZone[2] = { NULL };
int16_t *g_GroundZone2[2] = { NULL };
int16_t *g_FlyZone[2] = { NULL };
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
PHD_3DPOS g_CinePosition = { 0 };
int32_t g_NumberCameras = 0;
int32_t g_NumberSoundEffects = 0;
OBJECT_VECTOR *g_SoundEffectsTable = NULL;
int16_t g_RoomsToDraw[MAX_ROOMS_TO_DRAW] = { -1 };
int16_t g_RoomsToDrawCount = 0;
int g_IsWibbleEffect = false;
bool g_IsWaterEffect = false;
int g_IsShadeEffect = false;

int16_t *g_TriggerIndex = NULL;
int32_t g_FlipTimer = 0;
int32_t g_FlipEffect = -1;
int32_t g_FlipStatus = 0;
int32_t g_FlipMapTable[MAX_FLIP_MAPS] = { 0 };

int16_t g_InvMode = INV_TITLE_MODE;
int32_t g_InvExtraData[8] = { 0 };
int16_t g_InvChosen = -1;

int32_t g_LsAdder = 0;
int32_t g_LsDivider = 0;
SHADOW_INFO g_ShadowInfo = { 0 };

RESOLUTION g_AvailableResolutions[RESOLUTIONS_SIZE] = {
    { 320, 200 },   { 512, 384 },   { 640, 480 },   { 800, 600 },
    { 1024, 768 },  { 1280, 720 },  { 1920, 1080 }, { 2560, 1440 },
    { 3840, 2160 }, { 4096, 2160 }, { 7680, 4320 }, { -1, -1 },
};
