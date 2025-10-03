#include "lara_effects.h"
#include "../SPECIFIC/3d_gen.h"
#include "../SPECIFIC/util.h"
#include "../SPECIFIC/vars.h"

/*
#include "game/effects/lara_effects.h"

#include "3dsystem/3d_gen.h"
#include "config.h"
#include "global/vars.h"
*/

void LaraNormal(ITEM_INFO *item)
{
	item->current_anim_state = AS_STOP;
	item->goal_anim_state = AS_STOP;
	item->anim_number = AA_STOP;
	item->frame_number = AF_STOP;
	g_Camera.type = CAM_CHASE;
	phd_AlterFOV(80 * PHD_DEGREE);
}

void LaraHandsFree(ITEM_INFO *item) { g_Lara.gun_status = LGS_ARMLESS; }

void LaraDrawRightGun(ITEM_INFO *item)
{

	int16_t *tmp_mesh;
	OBJECT_INFO *obj = &g_Objects[item->object_number];
	OBJECT_INFO *obj2 = &g_Objects[O_PISTOLS];

	SWAP(g_Meshes[obj->mesh_index + LM_THIGH_R],
		 g_Meshes[obj2->mesh_index + LM_THIGH_R], tmp_mesh);

	SWAP(g_Meshes[obj->mesh_index + LM_HAND_R],
		 g_Meshes[obj2->mesh_index + LM_HAND_R], tmp_mesh);
}
