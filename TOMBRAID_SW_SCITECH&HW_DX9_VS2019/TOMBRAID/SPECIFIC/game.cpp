#include <windows.h>
#include "util.h"
#include "input.h"
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
#include "savegame.h"
#include "game.h"
#include "gameflow.h"
#include "drawprimitive.h"

static int32_t m_MedipackCoolDown = 0;

int Print_Level_Stats(int32_t level_num)
{
	if (GameType == VER_TR1)
	{
		if (level_num > 19)
			return GF_EXIT_TO_TITLE;

		switch (level_num)
		{
		case 0:
			return GF_EXIT_TO_TITLE;

			break;

		case 1:
			Print_Final_Stats(1);
			return 2;

			break;

		case 2:

			Print_Final_Stats(2);
			return 3;

			break;

		case 3:

			Print_Final_Stats(3);
			return 4;

			break;

		case 4:

			//return 0x50;
			return 16 | GF_START_CINE;

			break;

		case 5:

			Print_Final_Stats(5);
			return 6;

			break;

		case 6:

			Print_Final_Stats(6);
			return 7;

			break;

		case 7:

			Print_Final_Stats(7);
			return 8;

			break;

		case 8:

			Print_Final_Stats(8);
			return 9;

			break;

		case 9:

			//return 0x51;
			return 17 | GF_START_CINE;

			break;

		case 10:

			Print_Final_Stats(10);
			return 11;

			break;

		case 11:

			Print_Final_Stats(11);
			return 12;

			break;

		case 12:

			Print_Final_Stats(12);
			return 13;

			break;

		case 13:

			Print_Final_Stats(13);
			//return 0x52;
			return 18 | GF_START_CINE;

			break;

		case 14:

			Play_FMV_Init_Malloc(7, 1);
			//return 0x53;
			return 19 | GF_START_CINE;

			break;

		case 15:

			Print_Final_Stats(15);
			//Final_Titiles_After_Last_Level();
			return GF_EXIT_TO_TITLE;

			break;

		case 16:

			Print_Final_Stats(4);
			return 5;

			break;

		case 17:

			Print_Final_Stats(9);
			return 10;

			break;

		case 18:

			return 0xE;

			break;

		case 19:

			Print_Final_Stats(14);
			return 15;

			break;

		default:

			return GF_EXIT_TO_TITLE;

			break;
		}

	}
	else
	{
		return Print_Final_Stats(level_num);
	}

	return 0;

}


int Print_Final_Stats(int32_t level_num)
{
	char string[100];
	char time_str[100];
	TEXTSTRING *txt;

	Text_Init();

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
		sprintf(time_str, "%d:%d%d:%d%d", hours, minutes / 10, minutes % 10,
				seconds / 10, seconds % 10);
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
	do
	{
		if (g_SaveGame.secrets & 1)
		{
			secrets_taken++;
		}

		g_SaveGame.secrets >>= 1;
		secrets_total--;
	} while (secrets_total);

	sprintf(string, g_GameFlow.strings[GS_STATS_SECRETS_FMT], secrets_taken,
			g_GameFlow.levels[level_num].secrets);

	txt = Text_Create(0, 40, string);
	Text_CentreH(txt, 1);
	Text_CentreV(txt, 1);

	// pickups
	sprintf(string, g_GameFlow.strings[GS_STATS_PICKUPS_FMT],
			g_SaveGame.pickups);
	txt = Text_Create(0, 10, string);
	Text_CentreH(txt, 1);
	Text_CentreV(txt, 1);

	// kills
	sprintf(string, g_GameFlow.strings[GS_STATS_KILLS_FMT], g_SaveGame.kills);
	txt = Text_Create(0, -20, string);
	Text_CentreH(txt, 1);
	Text_CentreV(txt, 1);

	while (g_Input.select || g_Input.deselect)
	{
		Input_Update();
	}
	
	while (!g_Input.select && !g_Input.deselect)
	{
		Input_Update();
		S_InitialisePolyList();

		// CopyBufferToScreen(); draw picture
		Text_Draw();

		S_OutputPolyList();
		S_DumpScreen();
	}

	//добавил я что бы не вылетало LoadTitle
	//после финальной статистики
	while (g_Input.select || g_Input.deselect)
	{
		Input_Update();
	}

	if (level_num == g_GameFlow.last_level_num)
	{
		Game_Finished = 1;
		// exit to inventory

		for (int i = 0; i < 15; i++)
			ModifyStartInfo(i);

		return GF_EXIT_TO_TITLE;
	}
	else
	{
		CreateStartInfo(level_num + 1);
		ModifyStartInfo(level_num + 1);

		return (GF_START_GAME | (g_CurrentLevel + 1 & ((1 << 6) - 1)));
	}

	return 0;
}

int Game_Loop(int demo_mode)
{
	//------------------------
	//мой код все оружие - начало

	if(CheatsMode)
	{
		g_Lara.pistols.ammo = 1000;
		g_Lara.shotgun.ammo = 65535;
		g_Lara.magnums.ammo = 65535;
		g_Lara.uzis.ammo = 65535;

		Inv_AddItem(O_GUN_ITEM);
		Inv_AddItem(O_UZI_ITEM);
		Inv_AddItem(O_SHOTGUN_ITEM);
		Inv_AddItem(O_MAGNUM_ITEM);

		for ( int i = 0; i < 255; i++ )
		{
			Inv_AddItem(O_BIGMEDI_ITEM);
			Inv_AddItem(O_MEDI_ITEM);
		}
	}
	//мой код все оружие - конец
	//------------------------

	int32_t nframes = 1, game_over = false;
	g_OverlayFlag = 1;

	Initialise_Camera();

	game_over = Control_Phase(nframes, demo_mode);

	while (!game_over)
	{
		// draw fase
		nframes = Draw_Phase_Game();
		// control fase
		game_over = Control_Phase(nframes, demo_mode);

		//добавил я с учетом Win программирования (не MS-DOS)
		if (g_bWindowClosed)
			return GF_EXIT_GAME;
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

int Control_Phase(int32_t nframes, int32_t demo_mode)
{
	static int32_t frame_count = 0;
	const int32_t m_AnimationRate = 0x8000;

	int InventoryMode = 0;

	int32_t return_val = 0;

	if (nframes > MAX_FRAMES)
	{
		nframes = MAX_FRAMES;
	}

	frame_count += m_AnimationRate * nframes;

	while (frame_count >= 0)
	{

		Input_Update();

		if (g_Lara.death_count > DEATH_WAIT ||
			(g_Lara.death_count > DEATH_WAIT_MIN && g_Input.any &&
			 !g_Input.fly_cheat) ||
			g_OverlayFlag == 2)
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

		if ((g_Input.option || g_Input.save || g_Input.load ||
			 g_OverlayFlag <= 0) &&
			!g_Lara.death_count)
		{

			if (g_OverlayFlag > 0)
			{
				if (g_Input.load)
				{
					g_OverlayFlag = -1;
				}
				else if (g_Input.save)
				{
					g_OverlayFlag = -2;
				}
				else
				{
					g_OverlayFlag = 0;
				}
			}
			else
			{
				if (g_OverlayFlag == -1)
				{
					InventoryMode = INV_LOAD_MODE;
				}
				else if (g_OverlayFlag == -2)
				{
					InventoryMode = INV_SAVE_MODE;
				}
				else
				{
					InventoryMode = INV_GAME_MODE;
				}

				return_val = Display_Inventory(InventoryMode);

				g_OverlayFlag = 1;

				if (return_val != 0)
				{
					//в игре страница 1 это save game
					if (g_InvExtraData[0] == 1)
					{
						if (g_CurrentLevel == g_GameFlow.gym_level_num)
						{
							return GF_START_GAME | g_GameFlow.first_level_num;
							// return 1;
						}
						else
						{
							CreateSaveGameInfo();
							S_SaveGame(&g_SaveGame, g_InvExtraData[1]);
							Settings_Write(); // save keyboard config
						}
					}
					else
					{
						return 1; // load game
					}
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

		while (item_num != NO_ITEM)
		{
			FX_INFO *fx = &g_Effects[item_num];
			OBJECT_INFO *obj = &g_Objects[fx->object_number];

			if (obj->control)
			{
				obj->control(item_num);
			}

			item_num = fx->next_active;
		}

		LaraControl(0);
		CalculateCamera();
		Sound_UpdateEffects();

		g_SaveGame.timer++;
		g_HealthBarTimer--;

		if (g_LevelComplete)
		{
			return 1;
		}

		frame_count -= 0x10000;
	}

	return 0;
}

int Get_Key_State(int key)
{

	if (GetAsyncKeyState(key) & 0xFF00)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

#define VK_L 0x4C

void Input_Update()
{

	if(Get_Key_State(VK_L))
		g_LevelComplete = true;

	g_Input.forward = S_Input_Key(INPUT_KEY_UP);
	g_Input.back = S_Input_Key(INPUT_KEY_DOWN);
	g_Input.left = S_Input_Key(INPUT_KEY_LEFT);
	g_Input.right = S_Input_Key(INPUT_KEY_RIGHT);
	g_Input.step_left = S_Input_Key(INPUT_KEY_STEP_L);  // num pad 7
	g_Input.step_right = S_Input_Key(INPUT_KEY_STEP_R); // num pad 9
	g_Input.slow = S_Input_Key(INPUT_KEY_SLOW);
	g_Input.jump = S_Input_Key(INPUT_KEY_JUMP);
	g_Input.action = S_Input_Key(INPUT_KEY_ACTION);
	g_Input.draw = S_Input_Key(INPUT_KEY_DRAW);
	g_Input.look = S_Input_Key(INPUT_KEY_LOOK);
	g_Input.roll = S_Input_Key(INPUT_KEY_ROLL); // num pad 5

	g_Input.select = Get_Key_State(VK_CONTROL) || Get_Key_State(VK_RETURN);

	g_Input.deselect = S_Input_Key(INPUT_KEY_OPTION);
	g_Input.option =
		S_Input_Key(INPUT_KEY_OPTION) && g_Camera.type != CAM_CINEMATIC;

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

	// g_InputDB = Input_GetDebounced(g_Input);
}

INPUT_STATE Input_GetDebounced(INPUT_STATE input)
{
	INPUT_STATE result;
	result.any = input.any & ~g_OldInputDB.any;
	g_OldInputDB = input;
	return result;
}

int32_t DrawPhaseCinematic()
{
	S_InitialisePolyList();
	//Output_ClearScreen();
	g_CameraUnderwater = false;
	for (int i = 0; i < g_RoomsToDrawCount; i++)
	{
		int16_t room_num = g_RoomsToDraw[i];
		ROOM_INFO* r = &g_RoomInfo[room_num];
		r->top = 0;
		r->left = 0;
		r->right = (short)g_SurfaceMaxX;
		r->bottom = (short)g_SurfaceMaxY;
		PrintRooms(room_num);
	}

	S_OutputPolyList();

	g_Camera.number_frames = S_DumpScreen();
	S_AnimateTextures(g_Camera.number_frames);
	return g_Camera.number_frames;
}


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

/*
void S_InitialisePolyList_SW()
{
	sort3dptr = (int32_t*)sort3d_buffer;
	info3dptr = (int16_t*)info3d_buffer;

	surfacenum = 0;
}
*/

//void S_InitialisePolyList_HW()
void S_InitialisePolyList()
{
	sort3dptr = (int32_t*)sort3d_buffer;
	info3dptr = (int16_t*)info3d_buffer;

	surfacenum = 0;

	if (!Hardware)
		Clear_BackBuffer();
	else
		InitBuckets();
}

void DrawRooms(int16_t current_room)
{
	ROOM_INFO *r;

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

	g_RoomsToDrawCount = 0;

	g_RoomsToDraw[g_RoomsToDrawCount++] = current_room;

	g_CameraUnderwater = r->flags & RF_UNDERWATER;

	GetRoomBounds(current_room);

	/*
	if (!Hardware)
		Clear_BackBuffer();
		*/

	if (g_CameraUnderwater)
	{
		if (!Hardware)
			Create_Water_Palette();
	}

	if (!g_CameraUnderwater)
	{
		if (!Hardware)
			Create_Normal_Palette();
	}

	for (int i = 0; i < g_RoomsToDrawCount; i++)
	{
		PrintRooms(g_RoomsToDraw[i]);
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

	
}

void S_OutputPolyList_SW()
{
	phd_SortPolyList(surfacenum, sort3d_buffer);
	phd_PrintPolyList(dibdc->surface);
}

void S_OutputPolyList_HW()
{
	//-------create vert buff tex poly

	for (int n = 0; n < MAXBUCKETS; n++)
	{
		if (Bucket_Tex_Color_Opaque[n].count != 0)
		{
			g_d3d_Device->CreateVertexBuffer(Bucket_Tex_Color_Opaque[n].count * sizeof(VERTEX_COLOR_TEX), D3DUSAGE_WRITEONLY,
				D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &Bucket_Tex_Color_Opaque[n].VertBuff, 0);

			VERTEX_COLOR_TEX* v = 0;
			Bucket_Tex_Color_Opaque[n].VertBuff->Lock(0, 0, (void**)& v, 0);

			memcpy(&v[0], &Bucket_Tex_Color_Opaque[n].Vertex[0], sizeof(VERTEX_COLOR_TEX) * Bucket_Tex_Color_Opaque[n].count);

			Bucket_Tex_Color_Opaque[n].VertBuff->Unlock();

		}
	}

	///------------------- colored poly

	if (Bucket_Colored.count > 0)
	{
		g_d3d_Device->CreateVertexBuffer(Bucket_Colored.count * sizeof(VERTEX_COLOR), D3DUSAGE_WRITEONLY,
			D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &Bucket_Colored.VertBuff, 0);

		VERTEX_COLOR* v = 0;
		Bucket_Colored.VertBuff->Lock(0, 0, (void**)& v, 0);

		memcpy(&v[0], &Bucket_Colored.Vertex[0], sizeof(VERTEX_COLOR) * Bucket_Colored.count);

		Bucket_Colored.VertBuff->Unlock();
	}

	//-------------------------------------------------
	// lines buffer

	if (Bucket_Lines.count > 0)
	{
		g_d3d_Device->CreateVertexBuffer(Bucket_Lines.count * sizeof(VERTEX_COLOR_LINE), D3DUSAGE_WRITEONLY,
			D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &Bucket_Lines.VertBuff, 0);

		VERTEX_COLOR* v = 0;
		Bucket_Lines.VertBuff->Lock(0, 0, (void**)& v, 0);

		memcpy(&v[0], &Bucket_Lines.Vertex[0], sizeof(VERTEX_COLOR_LINE) * Bucket_Lines.count);

		Bucket_Lines.VertBuff->Unlock();
	}

	//-------------------------------------------------
	// trans quad buffer for under text titles

	if (Bucket_TransQuad.count > 0)
	{
		g_d3d_Device->CreateVertexBuffer(Bucket_TransQuad.count * sizeof(VERTEX_TRANS_QUAD), D3DUSAGE_WRITEONLY,
			D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &Bucket_TransQuad.VertBuff, 0);

		VERTEX_COLOR* v = 0;
		Bucket_TransQuad.VertBuff->Lock(0, 0, (void**)& v, 0);

		memcpy(&v[0], &Bucket_TransQuad.Vertex[0], sizeof(VERTEX_TRANS_QUAD) * Bucket_TransQuad.count);

		Bucket_TransQuad.VertBuff->Unlock();
	}

	HRESULT hr;

	//hr = g_d3d_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 60, 100, 150), 1.0f, 0);
	hr = g_d3d_Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (FAILED(hr)) return;

	hr = g_d3d_Device->BeginScene();
	if (FAILED(hr)) return;

	pEffectTexColor->SetTechnique(g_hTechTexColor);

	D3DXHANDLE hMyConstant1 = pEffectTexColor->GetParameterByName(NULL, "g_ScreenWidth");
	if (hMyConstant1)
	{
		float ScreenWidth = (float)Screen_GetResWidth();
		pEffectTexColor->SetFloat(hMyConstant1, ScreenWidth);
	}

	D3DXHANDLE hMyConstant2 = pEffectTexColor->GetParameterByName(NULL, "g_ScreenHeight");
	if (hMyConstant2)
	{
		float ScreenHeight = (float)Screen_GetResHeight();
		pEffectTexColor->SetFloat(hMyConstant2, ScreenHeight);
	}
	
	D3DXHANDLE hMyConstant3 = pEffectTexColor->GetParameterByName(NULL, "ZNear");
	if (hMyConstant3)
	{
		float ZNear = Z_NEAR;
		pEffectTexColor->SetFloat(hMyConstant3, ZNear);
	}

	D3DXHANDLE hMyConstant4 = pEffectTexColor->GetParameterByName(NULL, "ZFar");
	if (hMyConstant4)
	{
		float ZFar = 0x9000 << W2V_SHIFT;
		pEffectTexColor->SetFloat(hMyConstant4, ZFar);
	}

	UINT uPass = 0;
	hr = pEffectTexColor->Begin(&uPass, 0);
	if (FAILED(hr)) return;

	hr = pEffectTexColor->BeginPass(0);
	if (FAILED(hr)) return;

	hr = g_d3d_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	if (FAILED(hr)) return;

	hr = g_d3d_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	if (FAILED(hr)) return;

	hr = g_d3d_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	if (FAILED(hr)) return;
	
	/*
	g_d3d_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_d3d_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_d3d_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	g_d3d_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	g_d3d_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	g_d3d_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	*/

	for (int n = 0; n < MAXBUCKETS; n++)
	{
		if (Bucket_Tex_Color_Opaque[n].count != 0)
		{
			hr = g_d3d_Device->SetTexture(0, m_pLevelTile[Bucket_Tex_Color_Opaque[n].tpage]);
			if (FAILED(hr)) return;
			hr = g_d3d_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
			if (FAILED(hr)) return;
			hr = g_d3d_Device->SetStreamSource(0, Bucket_Tex_Color_Opaque[n].VertBuff, 0, sizeof(VERTEX_COLOR_TEX));
			if (FAILED(hr)) return;
			hr = g_d3d_Device->SetVertexDeclaration(g_pVertDeclTexColor);
			if (FAILED(hr)) return;

			//нарисуем примитивы
			hr = g_d3d_Device->DrawPrimitive(
				D3DPT_TRIANGLELIST,             //тип примитива
				//D3DPT_TRIANGLEFAN,
				0,                              //смещение первой вершины в буфере
				Bucket_Tex_Color_Opaque[n].count / 3);                //кол-во треугольников (count вершин / 3)

		}
	}

	//g_d3d_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	hr = pEffectTexColor->EndPass();
	if (FAILED(hr)) return;

	//=== Pass 1 ===
	hr = pEffectTexColor->BeginPass(1);
	if (FAILED(hr)) return;

	if(Bucket_Colored.count > 0)
	{
		hr = g_d3d_Device->SetTexture(0, NULL);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetStreamSource(0, Bucket_Colored.VertBuff, 0, sizeof(VERTEX_COLOR));
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetVertexDeclaration(g_pVertDeclColor);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, Bucket_Colored.count / 3);
	}
		
	hr = pEffectTexColor->EndPass();
	if (FAILED(hr)) return;

	//draw trans quad under text titles

	hr = pEffectTexColor->BeginPass(4);
	if (FAILED(hr)) return;

	hr = g_d3d_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	if (FAILED(hr)) return;

	hr = g_d3d_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	if (FAILED(hr)) return;

	hr = g_d3d_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	if (FAILED(hr)) return;


	g_d3d_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_d3d_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_d3d_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	if (Bucket_TransQuad.count > 0)
	{
		hr = g_d3d_Device->SetTexture(0, NULL);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetStreamSource(0, Bucket_TransQuad.VertBuff, 0, sizeof(VERTEX_TRANS_QUAD));
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetVertexDeclaration(g_pVertDeclColor);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, Bucket_TransQuad.count / 3);
	}


	g_d3d_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	hr = pEffectTexColor->EndPass();
	if (FAILED(hr)) return;

	//draw lines
	hr = pEffectTexColor->BeginPass(3);
	if (FAILED(hr)) return;

	if (Bucket_Lines.count > 0)
	{
		hr = g_d3d_Device->SetTexture(0, NULL);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetStreamSource(0, Bucket_Lines.VertBuff, 0, sizeof(VERTEX_COLOR_LINE));
		if (FAILED(hr)) return;

		hr = g_d3d_Device->SetVertexDeclaration(g_pVertDeclColor);
		if (FAILED(hr)) return;

		hr = g_d3d_Device->DrawPrimitive(D3DPT_LINELIST, 0, Bucket_Lines.count / 2);
	}

	hr = pEffectTexColor->EndPass();
	if (FAILED(hr)) return;

	hr = pEffectTexColor->BeginPass(2);
	if (FAILED(hr)) return;

	phd_SortPolyList(surfacenum, sort3d_buffer);

	int* sptr = (int*)sort3d_buffer;

	short int* iptr;

	for (int i = 0; i < surfacenum; i++)
	{
		iptr = (short int*)(*sptr);
		short int routine = *(iptr++);

		int tex_page = *(iptr++); //tex page

		int num_verts = *(iptr++); //num verts

		VERTEX_COLOR_TEX Vertex[8];

		float* fptr;

		BYTE diffuse;
		DWORD color;

		int draw_verts_count = 0;

		if(num_verts == 4)
		{
			fptr = (float*)iptr;

			float x1 = fptr[0];
			float y1 = fptr[1];
			float g1 = fptr[2];
			float z1 = fptr[3];
			float w1 = fptr[4];
			float tu1 = fptr[5];
			float tv1 = fptr[6];

			float x2 = fptr[7];
			float y2 = fptr[8];
			float g2 = fptr[9];
			float z2 = fptr[10];
			float w2 = fptr[11];
			float tu2 = fptr[12];
			float tv2 = fptr[13];

			float x3 = fptr[14];
			float y3 = fptr[15];
			float g3 = fptr[16];
			float z3 = fptr[17];
			float w3 = fptr[18];
			float tu3 = fptr[19];
			float tv3 = fptr[20];

			float x4 = fptr[21];
			float y4 = fptr[22];
			float g4 = fptr[23];
			float z4 = fptr[24];
			float w4 = fptr[25];
			float tu4 = fptr[26];
			float tv4 = fptr[27];

			Vertex[0].x = x1;
			Vertex[0].y = y1;
			Vertex[0].w = w1;
			Vertex[0].tu = tu1;
			Vertex[0].tv = tv1;
			Vertex[0].z = z1;
			diffuse = (BYTE)CLAMP255(g1);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[0].diffuse = color;

			Vertex[1].x = x2;
			Vertex[1].y = y2;
			Vertex[1].w = w2;
			Vertex[1].tu = tu2;
			Vertex[1].tv = tv2;
			Vertex[1].z = z2;
			diffuse = (BYTE)CLAMP255(g2);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[1].diffuse = color;

			Vertex[2].x = x3;
			Vertex[2].y = y3;
			Vertex[2].w = w3;
			Vertex[2].tu = tu3;
			Vertex[2].tv = tv3;
			Vertex[2].z = z3;
			diffuse = (BYTE)CLAMP255(g3);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[2].diffuse = color;

			//second tri

			Vertex[3].x = x1;
			Vertex[3].y = y1;
			Vertex[3].w = w1;
			Vertex[3].tu = tu1;
			Vertex[3].tv = tv1;
			Vertex[3].z = z1;
			diffuse = (BYTE)CLAMP255(g1);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[3].diffuse = color;

			Vertex[4].x = x3;
			Vertex[4].y = y3;
			Vertex[4].w = w3;
			Vertex[4].tu = tu3;
			Vertex[4].tv = tv3;
			Vertex[4].z = z3;
			diffuse = (BYTE)CLAMP255(g3);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[4].diffuse = color;

			Vertex[5].x = x4;
			Vertex[5].y = y4;
			Vertex[5].w = w4;
			Vertex[5].tu = tu4;
			Vertex[5].tv = tv4;
			Vertex[5].z = z4;
			diffuse = (BYTE)CLAMP255(g4);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[5].diffuse = color;

			draw_verts_count = 6;
			
		}
		else if (num_verts == 3)
		{
			fptr = (float*)iptr;

			float x1 = fptr[0];
			float y1 = fptr[1];
			float g1 = fptr[2];
			float z1 = fptr[3];
			float w1 = fptr[4];
			float tu1 = fptr[5];
			float tv1 = fptr[6];

			float x2 = fptr[7];
			float y2 = fptr[8];
			float g2 = fptr[9];
			float z2 = fptr[10];
			float w2 = fptr[11];
			float tu2 = fptr[12];
			float tv2 = fptr[13];

			float x3 = fptr[14];
			float y3 = fptr[15];
			float g3 = fptr[16];
			float z3 = fptr[17];
			float w3 = fptr[18];
			float tu3 = fptr[19];
			float tv3 = fptr[20];

			Vertex[0].x = x1;
			Vertex[0].y = y1;
			Vertex[0].w = w1;
			Vertex[0].tu = tu1;
			Vertex[0].tv = tv1;
			Vertex[0].z = z1;
			diffuse = (BYTE)CLAMP255(g1);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[0].diffuse = color;

			Vertex[1].x = x2;
			Vertex[1].y = y2;
			Vertex[1].w = w2;
			Vertex[1].tu = tu2;
			Vertex[1].tv = tv2;
			Vertex[1].z = z2;
			diffuse = (BYTE)CLAMP255(g2);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[1].diffuse = color;

			Vertex[2].x = x3;
			Vertex[2].y = y3;
			Vertex[2].w = w3;
			Vertex[2].tu = tu3;
			Vertex[2].tv = tv3;
			Vertex[2].z = z3;
			diffuse = (BYTE)CLAMP255(g3);
			color = (0xFF << 24) | (diffuse << 16) | (diffuse << 8) | diffuse;
			Vertex[2].diffuse = color;

			draw_verts_count = 3;

		}

		if(draw_verts_count > 0)
		{

			LPDIRECT3DVERTEXBUFFER9 VertBuff;

			g_d3d_Device->CreateVertexBuffer(draw_verts_count * sizeof(VERTEX_COLOR_TEX), D3DUSAGE_WRITEONLY,
				D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &VertBuff, 0);

			VERTEX_COLOR_TEX* v = 0;
			VertBuff->Lock(0, 0, (void**)& v, 0);

			memcpy(&v[0], &Vertex[0], sizeof(VERTEX_COLOR_TEX)* draw_verts_count);

			VertBuff->Unlock();
	
			hr = g_d3d_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			if (FAILED(hr)) return;

			hr = g_d3d_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
			if (FAILED(hr)) return;

			hr = g_d3d_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			if (FAILED(hr)) return;

			g_d3d_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			g_d3d_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			g_d3d_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			hr = g_d3d_Device->SetTexture(0, m_pLevelTile[tex_page]);
		
			if (FAILED(hr)) return;
			hr = g_d3d_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
			if (FAILED(hr)) return;
			hr = g_d3d_Device->SetStreamSource(0, VertBuff, 0, sizeof(VERTEX_COLOR_TEX));
			if (FAILED(hr)) return;
			hr = g_d3d_Device->SetVertexDeclaration(g_pVertDeclTexColor);
			if (FAILED(hr)) return;

			//нарисуем примитивы
			hr = g_d3d_Device->DrawPrimitive(
				D3DPT_TRIANGLELIST,             //тип примитива
				0,                              //смещение первой вершины в буфере
				draw_verts_count / 3);                //кол-во треугольников (count вершин / 3)

	
			g_d3d_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);


			if (VertBuff)
			{
				VertBuff->Release();
				VertBuff = nullptr;
			}

		}

		sptr += 2;
	}

	hr = pEffectTexColor->EndPass();
	if (FAILED(hr)) return;

	hr = pEffectTexColor->End();
	if (FAILED(hr)) return;

	hr = g_d3d_Device->EndScene();
	if (FAILED(hr)) return;


	//----------- free vert buff для следующего кадра

	for (int n = 0; n < MAXBUCKETS; n++)
	{
		if (Bucket_Tex_Color_Opaque[n].count != 0)
		{
			if (Bucket_Tex_Color_Opaque[n].VertBuff)
			{
				Bucket_Tex_Color_Opaque[n].VertBuff->Release();
				Bucket_Tex_Color_Opaque[n].VertBuff = nullptr;
			}

		}

	}

	if (Bucket_Colored.count > 0)
	{
		Bucket_Colored.VertBuff->Release();
		Bucket_Colored.VertBuff = nullptr;
	}


	if (Bucket_Lines.count > 0)
	{
		Bucket_Lines.VertBuff->Release();
		Bucket_Lines.VertBuff = nullptr;
	}

	if (Bucket_TransQuad.count > 0)
	{
		Bucket_TransQuad.VertBuff->Release();
		Bucket_TransQuad.VertBuff = nullptr;
	}

}

int32_t S_DumpScreen()
{
	int nframes;

	nframes = Sync();

	while (nframes < 2)
	{
		while (!Sync())
			;
		nframes++;
	}

	if (Hardware)
	{
		HRESULT hr = g_d3d_Device->Present(NULL, NULL, NULL, NULL);
		if (FAILED(hr)) return 0;
	}
	else
	{
		// ScreenPartialDump()
		Present_BackBuffer();
	}

	SpinMessageLoop();
	g_FPSCounter++;

	return nframes;
}

void GetRoomBounds(int16_t room_num)
{
	ROOM_INFO *r = &g_RoomInfo[room_num];
	phd_PushMatrix();
	phd_TranslateAbs(r->x, r->y, r->z);
	if (r->doors)
	{
		for (int i = 0; i < r->doors->count; i++)
		{
			DOOR_INFO *door = &r->doors->door[i];
			if (SetRoomBounds(&door->x, door->room_num, r))
			{
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

	if ((objptr[0] * (parent->x + objptr[3] - g_W2VMatrix._03)) +
			(objptr[1] * (parent->y + objptr[4] - g_W2VMatrix._13)) +
			(objptr[2] * (parent->z + objptr[5] - g_W2VMatrix._23)) >=
		0)
	{
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
		int32_t xv = mptr->_00 * objptr[0] + mptr->_01 * objptr[1] +
					 mptr->_02 * objptr[2] + mptr->_03;
		int32_t yv = mptr->_10 * objptr[0] + mptr->_11 * objptr[1] +
					 mptr->_12 * objptr[2] + mptr->_13;
		int32_t zv = mptr->_20 * objptr[0] + mptr->_21 * objptr[1] +
					 mptr->_22 * objptr[2] + mptr->_23;
		door_vbuf[i].xv = xv;
		door_vbuf[i].yv = yv;
		door_vbuf[i].zv = zv;
		objptr += 3;

		if (zv > 0)
		{
			if (zv > (DRAW_DIST_MAX << W2V_SHIFT))
			{
				z_toofar++;
			}

			zv /= g_PhdPersp;
			int32_t xs, ys;
			if (zv)
			{
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
		for (int i = 0; i < 4; i++)
		{
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

	if (r->flags & RF_UNDERWATER)
	{
		SetupBelowWater(g_CameraUnderwater);
	}
	else
	{
		SetupAboveWater(g_CameraUnderwater);
	}

	r->bound_active = 0;

	phd_PushMatrix();
	phd_TranslateAbs(r->x, r->y, r->z);

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
		MESH_INFO *mesh = (MESH_INFO *)&r->static_mesh[i];

		if (g_StaticObjects[mesh->static_number].flags & 2)
		{
			phd_PushMatrix();
			phd_TranslateAbs(mesh->x, mesh->y, mesh->z);
			phd_RotY(mesh->y_rot);

			int clip = S_GetObjectBounds(&g_StaticObjects[mesh->static_number].x_minp);

			if (clip)
			{
				Output_CalculateStaticLight(mesh->shade);
				Output_DrawPolygons(g_Meshes[g_StaticObjects[mesh->static_number].mesh_number], clip);
			}

			phd_PopMatrix();
		}
	}

	for (int i = r->fx_number; i != NO_ITEM; i = g_Effects[i].next_fx)
	{
		DrawEffect(i);
	}

	phd_PopMatrix();
}

void DrawRoom(int16_t *obj_ptr)
{

	obj_ptr = CalcRoomVertices(obj_ptr);
	obj_ptr = S_DrawObjectGT4(obj_ptr + 1, *obj_ptr);
	obj_ptr = S_DrawObjectGT3(obj_ptr + 1, *obj_ptr);
	obj_ptr = DrawRoomSprites(obj_ptr + 1, *obj_ptr);
	
}

int32_t CalcFogShade(int32_t depth)
{
	int32_t fog_begin = DRAW_DIST_FADE;
	int32_t fog_end = DRAW_DIST_MAX;

	if (depth < fog_begin)
	{
		return 0;
	}
	if (depth >= fog_end)
	{
		return 0x1FFF;
	}

	return (depth - fog_begin) * 0x1FFF / (fog_end - fog_begin);
}

int32_t S_SaveGame(SAVEGAME_INFO *save, int32_t slot)
{
	char filename[80];
	sprintf(filename, g_GameFlow.save_game_fmt, slot);

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

	sprintf(filename, "%s",
			g_GameFlow.levels[g_SaveGame.current_level].level_title);
	fwrite(filename, sizeof(char), 75, fp);
	fwrite(&g_SaveCounter, sizeof(int32_t), 1, fp);

	if (!save->start)
	{
		// ExitSystem("null save->start");
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

	sprintf(&req->item_texts[req->item_text_len * slot], "%s %d", filename,
			g_SaveCounter);
	g_SavedGamesCount++;
	g_SaveCounter++;
	return 1;
}

int32_t S_LoadGame(SAVEGAME_INFO *save, int32_t slot)
{
	char filename[80];
	sprintf(filename, g_GameFlow.save_game_fmt, slot);
	// LOG_DEBUG("%s", filename);
	FILE *fp = fopen(filename, "rb");
	if (!fp)
	{
		return 0;
	}
	fread(filename, sizeof(char), 75, fp);
	int32_t counter;

	fread(&counter, sizeof(int32_t), 1, fp);

	if (!save->start)
	{
		// ExitSystem("null save->start");
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
	fread(&save->buffer[0], sizeof(char), MAX_SAVEGAME_BUFFER, fp);
	fclose(fp);

	return 1;
}

void GetSavedGamesList(REQUEST_INFO *req)
{
	int32_t height = Screen_GetResHeight();

	if (height <= 200)
	{
		req->y = -32;
		req->vis_lines = 5;
	}
	else if (height <= 384)
	{
		req->y = -62;
		req->vis_lines = 8;
	}
	else if (height <= 480)
	{
		req->y = -90;
		req->vis_lines = 10;
	}
	else
	{
		req->y = -100;
		req->vis_lines = 12;
	}

	if (req->requested >= req->vis_lines)
	{
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

			sprintf(&req->item_texts[req->items * req->item_text_len], "%s %d",
					filename, counter);

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

			sprintf(&req->item_texts[req->items * req->item_text_len],
					g_GameFlow.strings[GS_MISC_EMPTY_SLOT_FMT], i + 1);
		}

		req->items++;
	}

	g_SaveCounter++;

	return 1;
}
