#include <windows.h>

#include "inv.h"

#include "vars.h"

int16_t g_InvKeysCurrent;
int16_t g_InvKeysObjects;

int16_t g_InvKeysQtys[24] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/*
INVENTORY_ITEM *g_InvKeysList[23] = {
    &g_InvItemLeadBar,
    &g_InvItemPuzzle1,
    &g_InvItemPuzzle2,
    &g_InvItemPuzzle3,
    &g_InvItemPuzzle4,
    &g_InvItemKey1,
    &g_InvItemKey2,
    &g_InvItemKey3,
    &g_InvItemKey4,
    &g_InvItemPickup1,
    &g_InvItemPickup2,
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
*/
INVENTORY_ITEM *g_InvKeysList[23] = {
    NULL,//&g_InvItemLeadBar,
    &g_InvItemPuzzle1,
    NULL,//&g_InvItemPuzzle2,
    NULL,//&g_InvItemPuzzle3,
    NULL,//&g_InvItemPuzzle4,
    &g_InvItemKey1,
    NULL,//&g_InvItemKey2,
    NULL,//&g_InvItemKey3,
    NULL,//&g_InvItemKey4,
    NULL,//&g_InvItemPickup1,
    NULL,//&g_InvItemPickup2,
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



int16_t g_InvMainCurrent;
int16_t g_InvMainObjects = 8;
int16_t g_InvMainQtys[24] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

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
/*
int16_t g_InvOptionCurrent;
int16_t g_InvOptionObjects = 5;
INVENTORY_ITEM *g_InvOptionList[] = {
    &g_InvItemGame,    &g_InvItemControls,  &g_InvItemSound,
    &g_InvItemDetails, &g_InvItemLarasHome,
};
*/

INVENTORY_ITEM g_InvItemCompass = {
    "Compass", //.string = 
    O_MAP_OPTION, //.object_number = 
    25,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    10,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    4352,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    -8192,//.x_rot_sel = 
    0,//.x_rot = 
    0,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    456,//.ztrans_sel = 
    0,//.ztrans = 
    0x5,//.which_meshes = 
    0x5,//.drawn_meshes = 
    0,//.inv_pos = 
    NULL,//.sprlist = 
};

INVENTORY_ITEM g_InvItemMedi = {
    "Small Medi Pack",//.string = 
    O_MEDI_OPTION,//.object_number = 
    26,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    25,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    4032,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    -7296,//.x_rot_sel = 
    0,//.x_rot = 
    -4096,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    216,//.ztrans_sel = 
    0,//.ztrans = 
    0xFFFFFFFF,//.which_meshes = 
    0xFFFFFFFF,//.drawn_meshes = 
    7,//.inv_pos = 
    NULL,//.sprlist = 
};

INVENTORY_ITEM g_InvItemBigMedi = {
    "Large Medi Pack",//.string = 
    O_BIGMEDI_OPTION,//.object_number = 
    20,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    19,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    3616,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    -8160,//.x_rot_sel = 
    0,//.x_rot = 
    -4096,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    352,//.ztrans_sel = 
    0,//.ztrans = 
    0xFFFFFFFF,//.which_meshes = 
    0xFFFFFFFF,//.drawn_meshes = 
    6,//.inv_pos = 
    NULL,//.sprlist = 
};

/*
INVENTORY_ITEM g_InvItemLeadBar = {
    .string = "Lead Bar",
    .object_number = O_LEADBAR_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 3616,
    .pt_xrot = 0,
    .x_rot_sel = -8160,
    .x_rot = 0,
    .y_rot_sel = -4096,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 352,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 100,
    .sprlist = NULL,
};


INVENTORY_ITEM g_InvItemPickup1 = {
    .string = "Pickup",
    .object_number = O_PICKUP_OPTION1,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 111,
    .sprlist = NULL,
};



INVENTORY_ITEM g_InvItemPickup2 = {
    .string = "Pickup",
    .object_number = O_PICKUP_OPTION2,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 110,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemScion = {
    .string = "Scion",
    .object_number = O_SCION_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 109,
    .sprlist = NULL,
};

*/
INVENTORY_ITEM g_InvItemPuzzle1 = {
    "Puzzle", //.string = 
    O_PUZZLE_OPTION1, //.object_number = 
    1, //.frames_total = 
    0, //.current_frame = 
    0, //.goal_frame = 
    0, //.open_frame = 
    1, //.anim_direction = 
    1, //.anim_speed = 
    0, //.anim_count = 
    7200, //.pt_xrot_sel = 
    0, //.pt_xrot = 
    -4352, //.x_rot_sel = 
    0, //.x_rot = 
    0, //.y_rot_sel = 
    0, //.y_rot = 
    0, //.ytrans_sel = 
    0, //.ytrans = 
    256, //.ztrans_sel = 
    0, //.ztrans = 
    0xFFFFFFFF, //.which_meshes = 
    0xFFFFFFFF, //.drawn_meshes = 
    108, //.inv_pos = 
    NULL, //.sprlist = 
};

/*

INVENTORY_ITEM g_InvItemPuzzle2 = {
    .string = "Puzzle",
    .object_number = O_PUZZLE_OPTION2,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 107,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemPuzzle3 = {
    .string = "Puzzle",
    .object_number = O_PUZZLE_OPTION3,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 106,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemPuzzle4 = {
    .string = "Puzzle",
    .object_number = O_PUZZLE_OPTION4,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 105,
    .sprlist = NULL,
};

*/

INVENTORY_ITEM g_InvItemKey1 = {
    "Key", //.string = 
    O_KEY_OPTION1, //.object_number = 
    1, //.frames_total = 
    0, //.current_frame = 
    0, //.goal_frame = 
    0, //.open_frame = 
    1, //.anim_direction = 
    1, //.anim_speed = 
    0, //.anim_count = 
    7200, //.pt_xrot_sel = 
    0, //.pt_xrot = 
    -4352, //.x_rot_sel = 
    0, //.x_rot = 
    0, //.y_rot_sel = 
    0, //.y_rot = 
    0, //.ytrans_sel = 
    0, //.ytrans = 
    256, //.ztrans_sel = 
    0, //.ztrans = 
    0xFFFFFFFF, //.which_meshes = 
    0xFFFFFFFF, //.drawn_meshes = 
    101, //.inv_pos = 
    NULL, //.sprlist = 
};

/*
INVENTORY_ITEM g_InvItemKey2 = {
    .string = "Key",
    .object_number = O_KEY_OPTION2,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 102,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemKey3 = {
    .string = "Key",
    .object_number = O_KEY_OPTION3,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 103,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemKey4 = {
    .string = "Key",
    .object_number = O_KEY_OPTION4,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 7200,
    .pt_xrot = 0,
    .x_rot_sel = -4352,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 256,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 104,
    .sprlist = NULL,
};

*/

INVENTORY_ITEM g_InvItemPistols = {
    "Pistols",//.string = 
    O_GUN_OPTION,//.object_number = 
    12,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    11,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    3200,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    -3808,//.x_rot_sel = 
    0,//.x_rot = 
    0,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    296,//.ztrans_sel = 
    0,//.ztrans = 
    0xFFFFFFFF,//.which_meshes = 
    0xFFFFFFFF,//.drawn_meshes = 
    1,//.inv_pos = 
    NULL,//.sprlist = 
};


INVENTORY_ITEM g_InvItemShotgun = {
    "Shotgun",//.string = 
    O_SHOTGUN_OPTION,//.object_number = 
    13,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    12,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    3200,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    0,//.x_rot_sel = 
    0,//.x_rot = 
    -8192,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    296,//.ztrans_sel = 
    0,//.ztrans = 
    0xFFFFFFFF,//.which_meshes = 
    0xFFFFFFFF,//.drawn_meshes = 
    2,//.inv_pos = 
    NULL,//.sprlist = 
};


INVENTORY_ITEM g_InvItemMagnum = {
    "Magnums",//.string = 
    O_MAGNUM_OPTION,//.object_number = 
    12,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    11,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    3200,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    -3808,//.x_rot_sel = 
    0,//.x_rot = 
    0,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    296,//.ztrans_sel = 
    0,//.ztrans = 
    0xFFFFFFFF,//.which_meshes = 
    0xFFFFFFFF,//.drawn_meshes = 
    3,//.inv_pos = 
    NULL,//.sprlist = 
};

INVENTORY_ITEM g_InvItemUzi = {
    "Uzis",//.string = 
    O_UZI_OPTION,//.object_number = 
    13,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    12,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    3200,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    -3808,//.x_rot_sel = 
    0,//.x_rot = 
    0,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    296,//.ztrans_sel = 
    0,//.ztrans = 
    0xFFFFFFFF,//.which_meshes = 
    0xFFFFFFFF,//.drawn_meshes = 
    4,//.inv_pos = 
    NULL,//.sprlist = 
};

INVENTORY_ITEM g_InvItemGrenade = {
    "Grenade",//.string = 
    O_EXPLOSIVE_OPTION,//.object_number = 
    15,//.frames_total = 
    0,//.current_frame = 
    0,//.goal_frame = 
    14,//.open_frame = 
    1,//.anim_direction = 
    1,//.anim_speed = 
    0,//.anim_count = 
    5024,//.pt_xrot_sel = 
    0,//.pt_xrot = 
    0,//.x_rot_sel = 
    0,//.x_rot = 
    0,//.y_rot_sel = 
    0,//.y_rot = 
    0,//.ytrans_sel = 
    0,//.ytrans = 
    368,//.ztrans_sel = 
    0,//.ztrans = 
    0xFFFFFFFF,//.which_meshes = 
    0xFFFFFFFF,//.drawn_meshes = 
    5,//.inv_pos = 
    NULL,//.sprlist = 
};

/*
INVENTORY_ITEM g_InvItemPistolAmmo = {
    .string = "Pistol Clips",
    .object_number = O_GUN_AMMO_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 3200,
    .pt_xrot = 0,
    .x_rot_sel = -3808,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 296,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 1,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemShotgunAmmo = {
    .string = "Shotgun Shells",
    .object_number = O_SG_AMMO_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 3200,
    .pt_xrot = 0,
    .x_rot_sel = -3808,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 296,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 2,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemMagnumAmmo = {
    .string = "Magnum Clips",
    .object_number = O_MAG_AMMO_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 3200,
    .pt_xrot = 0,
    .x_rot_sel = -3808,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 296,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 3,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemUziAmmo = {
    .string = "Uzi Clips",
    .object_number = O_UZI_AMMO_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 3200,
    .pt_xrot = 0,
    .x_rot_sel = -3808,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 296,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 4,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemGame = {
    .string = "Game",
    .object_number = O_PASSPORT_CLOSED,
    .frames_total = 30,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 14,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 4640,
    .pt_xrot = 0,
    .x_rot_sel = -4320,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 384,
    .ztrans = 0,
    .which_meshes = 0x13,
    .drawn_meshes = 0x13,
    .inv_pos = 0,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemDetails = {
    .string = "Detail Levels",
    .object_number = O_DETAIL_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 4224,
    .pt_xrot = 0,
    .x_rot_sel = -6720,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 424,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 1,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemSound = {
    .string = "Sound",
    .object_number = O_SOUND_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 4832,
    .pt_xrot = 0,
    .x_rot_sel = -2336,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 368,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 2,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemControls = {
    .string = "Controls",
    .object_number = O_CONTROL_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 5504,
    .pt_xrot = 0,
    .x_rot_sel = 1536,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 352,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 3,
    .sprlist = NULL,
};

INVENTORY_ITEM g_InvItemLarasHome = {
    .string = "g_Lara's Home",
    .object_number = O_PHOTO_OPTION,
    .frames_total = 1,
    .current_frame = 0,
    .goal_frame = 0,
    .open_frame = 0,
    .anim_direction = 1,
    .anim_speed = 1,
    .anim_count = 0,
    .pt_xrot_sel = 4640,
    .pt_xrot = 0,
    .x_rot_sel = -4320,
    .x_rot = 0,
    .y_rot_sel = 0,
    .y_rot = 0,
    .ytrans_sel = 0,
    .ytrans = 0,
    .ztrans_sel = 384,
    .ztrans = 0,
    .which_meshes = 0xFFFFFFFF,
    .drawn_meshes = 0xFFFFFFFF,
    .inv_pos = 5,
    .sprlist = NULL,
};
*/