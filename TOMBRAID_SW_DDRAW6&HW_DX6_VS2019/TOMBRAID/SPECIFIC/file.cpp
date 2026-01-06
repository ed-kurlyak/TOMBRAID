#include "file.h"
#include "collide.h"
#include "draw.h"
#include "lara.h"
#include "screen.h"
#include "vars.h"
#include "cinema.h"
#include "..\\objects\\abortion.h"
#include "..\\objects\\alligator.h"
#include "..\\objects\\ape.h"
#include "..\\objects\\baldy.h"
#include "..\\objects\\bat.h"
#include "..\\objects\\bear.h"
#include "..\\objects\\blood.h"
#include "..\\objects\\boat.h"
#include "..\\objects\\bridge.h"
#include "..\\objects\\cabin.h"
#include "..\\objects\\centaur.h"
#include "..\\objects\\cog.h"
#include "..\\objects\\cowboy.h"
#include "..\\objects\\crocodile.h"
#include "..\\objects\\dino.h"
#include "..\\objects\\door.h"
#include "..\\objects\\gunshot.h"
#include "..\\objects\\keyhole.h"
#include "..\\objects\\larson.h"
#include "..\\objects\\lion.h"
#include "..\\objects\\mummy.h"
#include "..\\objects\\mutant.h"
#include "..\\objects\\natla.h"
#include "..\\objects\\pickup.h"
#include "..\\objects\\pierre.h"
#include "..\\objects\\pod.h"
#include "..\\objects\\puzzle_hole.h"
#include "..\\objects\\raptor.h"
#include "..\\objects\\rat.h"
#include "..\\objects\\scion.h"
#include "..\\objects\\skate_kid.h"
#include "..\\objects\\statue.h"
#include "..\\objects\\switch.h"
#include "..\\objects\\trapdoor.h"
#include "..\\objects\\vole.h"
#include "..\\objects\\wolf.h"
#include "..\\traps\\dart.h"
#include "..\\traps\\dart_emitter.h"
#include "..\\traps\\falling_block.h"
#include "..\\traps\\movable_block.h"
#include "..\\traps\\pendulum.h"
#include "..\\traps\\damocles_sword.h"
#include "..\\traps\\falling_ceiling.h"
#include "..\\traps\\rolling_ball.h"
#include "..\\traps\\spikes.h"
#include "..\\traps\\teeth_trap.h"
#include "..\\traps\\lightning_emitter.h"
#include "..\\traps\\midas_touch.h"
#include "..\\traps\\rolling_block.h"
#include "..\\traps\\thors_hammer.h"
#include "..\\traps\\flame.h"
#include "..\\traps\\lava.h"
#include "..\\effects\\body_part.h"
#include "..\\effects\\missile.h"
#include "..\\effects\\ricochet.h"
#include "..\\effects\\splash.h"
#include "..\\effects\\twinkle.h"
#include "..\\effects\\waterfall.h"
#include "box.h"
#include "items.h"
#include "sound.h"

bool S_LoadLevel(char *szLevelName)
{
	int32_t LevelVersion;
	int32_t LevelNum;

	Init_Game_Malloc();

	FILE *fp = NULL;

	fopen_s(&fp, szLevelName, "rb");

	if (!fp)
	{
		MessageBox(NULL, "S_LoadLevel(): Could not open level file",
				   "Tomb Raider", MB_OK);
		return false;
	}

	//читаем номер версии файла уровня
	// 20h или 32d для Tomb Raider 1
	fread(&LevelVersion, sizeof(int32_t), 1, fp);

	if (LevelVersion != 32)
	{
		MessageBox(NULL, "Wrong Tomb Raider level version", "Tomb Raider",
				   MB_OK);
		return false;
	}

	if (!Load_Texture_Pages(fp))
	{
		return false;
	}

	//читаем номер уровня
	fread(&LevelNum, sizeof(int32_t), 1, fp);

	if (!Load_Rooms(fp))
	{
		return false;
	}

	if (!Load_Objects(fp))
	{
		return false;
	}

	if (!LoadSprites(fp))
	{
		return false;
	}

	if (!LoadCameras(fp))
	{
		return false;
	}

	if (!LoadSoundEffects(fp))
	{
		return false;
	}

	if (!LoadBoxes(fp))
	{
		return false;
	}

	if (!LoadAnimatedTextures(fp))
	{
		return false;
	}

	if (!LoadItems(fp))
	{
		return false;
	}

	if (!LoadDepthQ(fp))
	{
		return false;
	}

	if (!LoadPalette(fp))
	{
		return false;
	}

	if (!LoadCinematic(fp))
	{
		return false;
	}

	if (!LoadDemo(fp))
	{
		return false;
	}

	if (!LoadSoundSamples(fp))
	{
		return false;
	}

	fclose(fp);

	if(Hardware)
		Create_DX_Textures();

	return true;
}

//uint32_t TexturePageCount;
#define ROOM_TRANSPARENCY  0xff



//не забыть освободить
void Create_DX_Textures()
{
	HRESULT hr;
	

	//m_pLevelTile = new LPDIRECT3DTEXTURE2[TexturePageCount];
	m_pLevelTile = new DWORD[TexturePageCount];

	UINT s = 0;

	for (UINT p = 0; p < TexturePageCount; p++)
	{
		BYTE alpha = 0;
		UINT TexTile = p;
		//UINT offset, c_index;
		UINT c_index;
		BYTE* TempTexTile = new BYTE[256 * 256 * 4];

		uint8_t* input_ptr = (uint8_t*)TexturePagePtrs[TexTile];

		for (UINT j = 0; j < 256; ++j)
		{
			for (UINT k = 0; k < 256; ++k)
			{
				//c_index = (j * 256) + k;
				//offset = Level->Textile16[TexTile].Tile[c_index];

				uint8_t pal_idx = *input_ptr++;

				uint8_t alpha = pal_idx == 0 ? 0 : 0xFF;

				UINT r = GameNormalPalette[pal_idx].r;
				UINT g = GameNormalPalette[pal_idx].g;
				UINT b = GameNormalPalette[pal_idx].b;
				


				c_index = (j * 1024) + (k * 4);

				TempTexTile[c_index + 0] = r; //r
				TempTexTile[c_index + 1] = g; //g
				TempTexTile[c_index + 2] = b; //b
				TempTexTile[c_index + 3] = alpha;
			}
		}

		DDSURFACEDESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
		ddsd.dwWidth = 256;
		ddsd.dwHeight = 256;

		ZeroMemory(&ddsd.ddpfPixelFormat, sizeof(ddsd.ddpfPixelFormat));
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		ddsd.ddpfPixelFormat.dwFourCC = 0;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
		ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
		ddsd.ddpfPixelFormat.dwZBufferBitDepth = 16;
		ddsd.ddpfPixelFormat.dwAlphaBitDepth = 16;
		ddsd.ddpfPixelFormat.dwLuminanceBitCount = 16;
		ddsd.ddpfPixelFormat.dwBumpBitCount = 16;
		ddsd.ddpfPixelFormat.dwRBitMask = 0x7C00;
		ddsd.ddpfPixelFormat.dwYBitMask = 0x7C00;
		ddsd.ddpfPixelFormat.dwStencilBitDepth = 0x7C00;
		ddsd.ddpfPixelFormat.dwLuminanceBitMask = 0x7C00;
		ddsd.ddpfPixelFormat.dwBumpDuBitMask = 0x7C00;
		ddsd.ddpfPixelFormat.dwGBitMask = 0x3E0;
		ddsd.ddpfPixelFormat.dwUBitMask = 0x3E0;
		ddsd.ddpfPixelFormat.dwZBitMask = 0x3E0;
		ddsd.ddpfPixelFormat.dwBumpDvBitMask = 0x3E0;
		ddsd.ddpfPixelFormat.dwBBitMask = 0x1F;
		ddsd.ddpfPixelFormat.dwVBitMask = 0x1F;
		ddsd.ddpfPixelFormat.dwStencilBitMask = 0x1F;
		ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask = 0x1f;
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0x8000;
		ddsd.ddpfPixelFormat.dwYUVAlphaBitMask = 0x8000;
		ddsd.ddpfPixelFormat.dwLuminanceAlphaBitMask = 0x8000;
		ddsd.ddpfPixelFormat.dwRGBZBitMask = 0x8000;
		ddsd.ddpfPixelFormat.dwYUVZBitMask = 0x8000;


		IDirectDrawSurface* pSurf;
		hr = g_pDD2->CreateSurface(&ddsd, &pSurf , NULL);

		IDirectDrawSurface* pSurf3;
		pSurf->QueryInterface(IID_IDirectDrawSurface3, (void**)& pSurf3);

		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
		ddsd.dwSize = sizeof(DDSURFACEDESC);
		pSurf3->Lock(NULL, &ddsd, DDLOCK_WRITEONLY, NULL);


		unsigned char* imageData = (unsigned char*)ddsd.lpSurface;


		for (UINT y = 0; y < 256; ++y)
		{
			BYTE* dest = imageData + y * ddsd.lPitch;
			BYTE* src = TempTexTile + y * 256 * 4;

			for (UINT x = 0; x < 256; ++x)
			{
		   		UINT srcIndex = x * 4;

				BYTE b = src[srcIndex + 2]; //b
				BYTE g = src[srcIndex + 1]; //g
				BYTE r = src[srcIndex + 0]; //r
				BYTE a = src[srcIndex + 3]; //a

				/*
				DWORD dwRGBA = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3) | ((a ? 1 : 0) << 15);

				for (int nBits = 16; nBits > 0; nBits -= 8)
				{
					*dest++ = (unsigned char)dwRGBA;
					dwRGBA >>= 8;
				}
				*/

				UINT16 pix16 = (UINT16)(((a ? 1u : 0u) << 15) |
					((r >> 3) << 10) |
					((g >> 3) << 5) |
					(b >> 3));

				// запись в little-endian (low byte first)
				*dest++ = (BYTE)(pix16 & 0xFF);
				*dest++ = (BYTE)((pix16 >> 8) & 0xFF);
			}
		}
		


		pSurf3->Unlock(NULL);

		

		
		LPDIRECT3DTEXTURE2 FloorTextureFrom = NULL;
		//скопировали изображение BMP в поверхность
	//у поверхности запрашиваем интерфейс текстуры
		pSurf3->QueryInterface(IID_IDirect3DTexture2,
			//(VOID**)&m_pLevelTile[s]);
			(VOID**)&FloorTextureFrom);

		FloorTextureFrom->GetHandle(g_pD3dDevice, &m_pLevelTile[s]);

		pSurf3->Release();
		pSurf->Release();
		delete[] TempTexTile;

		s++;

		

	}

	

}

bool Load_Texture_Pages(FILE *fp)
{
	//int32_t TexturePageCount;

	fread(&TexturePageCount, 4, 1, fp);

	int8_t *Buff = (int8_t *)Game_Alloc(TexturePageCount * 256 * 256, GBUF_TEXTURE_PAGES);

	fread(Buff, 256 * 256, TexturePageCount, fp);

	for (UINT i = 0; i < TexturePageCount; i++)
	{
		TexturePagePtrs[i] = Buff;
		Buff += 256 * 256;
	}

	return true;
}

bool Load_Rooms(FILE *fp)
{
	int32_t Size;
	int16_t NumDoors;
	int32_t FloorDataSize;

	fread(&g_RoomCount, sizeof(uint16_t), 1, fp);

	if (g_RoomCount > 1024)
	{
		MessageBox(NULL, "Load_Rooms(): Too many rooms", "Tomb Raider", MB_OK);
		return false;
	}

	g_RoomInfo = (ROOM_INFO *)Game_Alloc(sizeof(ROOM_INFO) * g_RoomCount,
										 GBUF_ROOM_INFOS);

	for (int i = 0; i < g_RoomCount; i++)
	{
		//читаем позицию комнаты на сцене
		fread(&g_RoomInfo[i].x, sizeof(uint32_t), 1, fp);
		g_RoomInfo[i].y = 0;
		fread(&g_RoomInfo[i].z, sizeof(uint32_t), 1, fp);

		fread(&g_RoomInfo[i].min_floor, sizeof(uint32_t), 1, fp);
		fread(&g_RoomInfo[i].max_ceiling, sizeof(uint32_t), 1, fp);

		//читаем мешь комнаты
		fread(&Size, sizeof(uint32_t), 1, fp);

		g_RoomInfo[i].data =
			(int16_t *)Game_Alloc(sizeof(uint16_t) * Size, GBUF_ROOM_MESH);

		fread(g_RoomInfo[i].data, sizeof(uint16_t), Size, fp);

		//читаем двери
		fread(&NumDoors, sizeof(uint16_t), 1, fp);

		if (NumDoors)
		{
			// 2 байта размер сколько дверей + сам массив дверей
			// sizeof(DOOR_INFO) * NumDoors g_RoomInfo[i].doors = (DOOR_INFOS
			// *)Game_Alloc(sizeof(uint16_t) + sizeof(DOOR_INFO *) * NumDoors,
			// GBUF_ROOM_DOOR); выделяем место под двери - число дверей +
			//указатель на массив дверей
			g_RoomInfo[i].doors = (DOOR_INFOS *)Game_Alloc(
				sizeof(uint16_t) + sizeof(DOOR_INFO *), GBUF_ROOM_DOOR);

			g_RoomInfo[i].doors->count = NumDoors;

			//выделяем место под все двери
			g_RoomInfo[i].doors->door = (DOOR_INFO *)Game_Alloc(
				sizeof(DOOR_INFO) * NumDoors, GBUF_ROOM_DOOR);

			fread(g_RoomInfo[i].doors->door, sizeof(DOOR_INFO), NumDoors, fp);
		}
		else
		{
			g_RoomInfo[i].doors = NULL;
		}

		//читаем пол комнаты
		fread(&g_RoomInfo[i].x_size, sizeof(uint16_t), 1, fp);
		fread(&g_RoomInfo[i].y_size, sizeof(uint16_t), 1, fp);

		Size = g_RoomInfo[i].y_size * g_RoomInfo[i].x_size;

		g_RoomInfo[i].floor = (FLOOR_INFO *)Game_Alloc(
			sizeof(FLOOR_INFO) * Size, GBUF_ROOM_FLOOR);

		fread(g_RoomInfo[i].floor, sizeof(FLOOR_INFO), Size, fp);

		//освещение комнаты
		fread(&g_RoomInfo[i].ambient, sizeof(uint16_t), 1, fp);
		fread(&g_RoomInfo[i].num_lights, sizeof(uint16_t), 1, fp);

		if (g_RoomInfo[i].num_lights)
		{
			g_RoomInfo[i].light = (LIGHT_INFO *)Game_Alloc(
				sizeof(LIGHT_INFO) * g_RoomInfo[i].num_lights,
				GBUF_ROOM_LIGHTS);

			fread(g_RoomInfo[i].light, sizeof(LIGHT_INFO),
				  g_RoomInfo[i].num_lights, fp);
		}
		else
		{
			g_RoomInfo[i].light = NULL;
		}

		//читаем информацию о статических мешах
		fread(&g_RoomInfo[i].num_static_meshes, sizeof(uint16_t), 1, fp);

		if (g_RoomInfo[i].num_static_meshes)
		{
			g_RoomInfo[i].static_mesh = (MESH_INFO *)Game_Alloc(
				sizeof(MESH_INFO) * g_RoomInfo[i].num_static_meshes,
				GBUF_ROOM_STATIC_MESH_INFOS);

			fread(g_RoomInfo[i].static_mesh, sizeof(MESH_INFO),
				  g_RoomInfo[i].num_static_meshes, fp);
		}
		else
		{
			g_RoomInfo[i].static_mesh = NULL;
		}

		// flipped (альтернативные) комнаты
		fread(&g_RoomInfo[i].flipped_room, sizeof(uint16_t), 1, fp);

		//флаги комнаты (например комната с водой, комната без воды)
		fread(&g_RoomInfo[i].flags, sizeof(uint16_t), 1, fp);

		//инициализация переменных
		g_RoomInfo[i].bound_active = 0;
		g_RoomInfo[i].left = Screen_GetResWidth() - 1;
		g_RoomInfo[i].top = Screen_GetResHeight() - 1;
		g_RoomInfo[i].bottom = 0;
		g_RoomInfo[i].right = 0;
		g_RoomInfo[i].item_number = -1;
		g_RoomInfo[i].fx_number = -1;
	}

	fread(&FloorDataSize, sizeof(uint32_t), 1, fp);

	g_FloorData = (int16_t *)Game_Alloc(sizeof(uint16_t) * FloorDataSize,
										GBUF_FLOOR_DATA);

	fread(g_FloorData, sizeof(uint16_t), FloorDataSize, fp);

	return true;
}

bool Load_Objects(FILE *fp)
{
	int32_t MeshCount;
	//читаем информацию про меши объектов
	fread(&MeshCount, sizeof(int32_t), 1, fp);

	g_MeshBase =
		(int16_t *)Game_Alloc(sizeof(int16_t) * MeshCount, GBUF_MESHES);

	fread(g_MeshBase, sizeof(int16_t), MeshCount, fp);

	int32_t MeshPtrCount;
	fread(&MeshPtrCount, sizeof(int32_t), 1, fp);
	//читаем информацию про индексы мешей в массиве
	uint32_t *MeshIndices = (uint32_t *)Game_Alloc(
		sizeof(uint32_t) * MeshPtrCount, GBUF_MESH_POINTERS);

	fread(MeshIndices, sizeof(uint32_t), MeshPtrCount, fp);

	g_Meshes = (int16_t **)Game_Alloc(sizeof(int16_t *) * MeshPtrCount,
									  GBUF_MESH_POINTERS);

	for (int i = 0; i < MeshPtrCount; i++)
	{
		//создаем массив мешей объектов
		g_Meshes[i] = &g_MeshBase[MeshIndices[i] / 2];
	}

	int32_t m_AnimCount;
	//читаем данные про анимацию
	fread(&m_AnimCount, sizeof(int32_t), 1, fp);

	g_Anims = (ANIM_STRUCT *)Game_Alloc(sizeof(ANIM_STRUCT) * m_AnimCount,
										GBUF_ANIMS);

	fread(g_Anims, sizeof(ANIM_STRUCT), m_AnimCount, fp);

	int32_t AnimChangeCount;
	fread(&AnimChangeCount, sizeof(int32_t), 1, fp);

	g_AnimChanges = (ANIM_CHANGE_STRUCT *)Game_Alloc(
		sizeof(ANIM_CHANGE_STRUCT) * AnimChangeCount, GBUF_ANIM_CHANGES);
	fread(g_AnimChanges, sizeof(ANIM_CHANGE_STRUCT), AnimChangeCount, fp);

	int32_t AnimRangeCount;
	fread(&AnimRangeCount, sizeof(int32_t), 1, fp);

	g_AnimRanges = (ANIM_RANGE_STRUCT *)Game_Alloc(
		sizeof(ANIM_RANGE_STRUCT) * AnimRangeCount, GBUF_ANIM_RANGES);
	fread(g_AnimRanges, sizeof(ANIM_RANGE_STRUCT), AnimRangeCount, fp);

	int32_t AnimCommandCount;
	//читаем данные про команды анимации
	fread(&AnimCommandCount, sizeof(int32_t), 1, fp);

	g_AnimCommands = (int16_t *)Game_Alloc(sizeof(int16_t) * AnimCommandCount,
										   GBUF_ANIM_COMMANDS);

	fread(g_AnimCommands, sizeof(int16_t), AnimCommandCount, fp);

	int32_t AnimBoneCount;
	//читаем данные про кости (позиции)
	fread(&AnimBoneCount, sizeof(int32_t), 1, fp);

	g_AnimBones =
		(int32_t *)Game_Alloc(sizeof(int32_t) * AnimBoneCount, GBUF_ANIM_BONES);

	fread(g_AnimBones, sizeof(int32_t), AnimBoneCount, fp);

	int32_t AnimFrameCount;
	//читаем данные про кости (повороты)
	fread(&AnimFrameCount, sizeof(int32_t), 1, fp);

	g_AnimFrames = (int16_t *)Game_Alloc(sizeof(int16_t) * AnimFrameCount,
										 GBUF_ANIM_FRAMES);

	fread(g_AnimFrames, sizeof(int16_t), AnimFrameCount, fp);

	for (int i = 0; i < m_AnimCount; i++)
	{
		g_Anims[i].frame_ptr = &g_AnimFrames[(size_t)g_Anims[i].frame_ptr / 2];
	}

	int32_t ObjectCount;
	//читаем инофрмацию про объекты в уровне
	fread(&ObjectCount, sizeof(int32_t), 1, fp);

	for (int i = 0; i < ObjectCount; i++)
	{
		int32_t tmp;
		fread(&tmp, sizeof(int32_t), 1, fp);

		fread(&g_Objects[tmp].nmeshes, sizeof(int16_t), 1, fp);
		fread(&g_Objects[tmp].mesh_index, sizeof(int16_t), 1, fp);
		fread(&g_Objects[tmp].bone_index, sizeof(int32_t), 1, fp);

		int32_t tmp2;
		fread(&tmp2, sizeof(int32_t), 1, fp);

		g_Objects[tmp].frame_base = &g_AnimFrames[tmp2 / 2];

		fread(&g_Objects[tmp].anim_index, sizeof(int16_t), 1, fp);

		g_Objects[tmp].loaded = 1;
	}

	Initialise_Objects();

	//читаем информацию про статические элементы в уровне
	int32_t StaticCount;
	fread(&StaticCount, sizeof(int32_t), 1, fp);

	for (int i = 0; i < StaticCount; i++)
	{
		int32_t tmp;
		fread(&tmp, sizeof(int32_t), 1, fp);
		STATIC_INFO *object = &g_StaticObjects[tmp];

		fread(&g_StaticObjects[tmp].mesh_number, sizeof(int16_t), 1, fp);
		fread(&g_StaticObjects[tmp].x_minp, sizeof(int16_t), 6, fp);
		fread(&g_StaticObjects[tmp].x_minc, sizeof(int16_t), 6, fp);
		fread(&g_StaticObjects[tmp].flags, sizeof(int16_t), 1, fp);
	}

	//читаем информацию про текстуры
	int32_t TextureCount;
	fread(&TextureCount, sizeof(int32_t), 1, fp);

	if (TextureCount > MAX_TEXTURES)
	{
		return false;
	}

	fread(g_PhdTextureInfo, sizeof(PHD_TEXTURE), TextureCount, fp);

	return true;
}

void Initialise_Objects()
{
	for (int i = 0; i < O_NUMBER_OF; i++)
	{
		g_Objects[i].intelligent = 0;
		g_Objects[i].save_position = 0;
		g_Objects[i].save_hitpoints = 0;
		g_Objects[i].save_flags = 0;
		g_Objects[i].save_anim = 0;
		g_Objects[i].initialise = NULL;
		g_Objects[i].collision = NULL;
		g_Objects[i].control = NULL;
		g_Objects[i].draw_routine = DrawAnimatingItem;
		g_Objects[i].ceiling = NULL;
		g_Objects[i].floor = NULL;
		g_Objects[i].pivot_length = 0;
		g_Objects[i].radius = DEFAULT_RADIUS;
		g_Objects[i].shadow_size = 0;
		g_Objects[i].hit_points = DONT_TARGET;
	}

	BaddyObjects();
	TrapObjects();
	ObjectObjects();
}

void BaddyObjects()
{
	g_Objects[O_LARA].initialise = InitialiseLaraLoad;
	g_Objects[O_LARA].draw_routine = DrawDummyItem;
	g_Objects[O_LARA].hit_points = LARA_HITPOINTS;
	g_Objects[O_LARA].shadow_size = (UNIT_SHADOW * 10) / 16;
	g_Objects[O_LARA].save_position = 1;
	g_Objects[O_LARA].save_hitpoints = 1;
	g_Objects[O_LARA].save_anim = 1;
	g_Objects[O_LARA].save_flags = 1;

	g_Objects[O_LARA_EXTRA].control = ControlLaraExtra;

	g_Objects[O_EVIL_LARA].initialise = InitialiseEvilLara;
	g_Objects[O_EVIL_LARA].control = ControlEvilLara;
	g_Objects[O_EVIL_LARA].draw_routine = DrawEvilLara;
	g_Objects[O_EVIL_LARA].collision = CreatureCollision;
	g_Objects[O_EVIL_LARA].hit_points = LARA_HITPOINTS;
	g_Objects[O_EVIL_LARA].shadow_size = (UNIT_SHADOW * 10) / 16;
	g_Objects[O_EVIL_LARA].save_position = 1;
	g_Objects[O_EVIL_LARA].save_hitpoints = 1;
	g_Objects[O_EVIL_LARA].save_flags = 1;

	if (g_Objects[O_WOLF].loaded)
	{
		g_Objects[O_WOLF].initialise = InitialiseWolf;
		g_Objects[O_WOLF].control = WolfControl;
		g_Objects[O_WOLF].collision = CreatureCollision;
		g_Objects[O_WOLF].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_WOLF].hit_points = WOLF_HITPOINTS;
		g_Objects[O_WOLF].pivot_length = 375;
		g_Objects[O_WOLF].radius = WOLF_RADIUS;
		g_Objects[O_WOLF].smartness = WOLF_SMARTNESS;
		g_Objects[O_WOLF].intelligent = 1;
		g_Objects[O_WOLF].save_position = 1;
		g_Objects[O_WOLF].save_hitpoints = 1;
		g_Objects[O_WOLF].save_anim = 1;
		g_Objects[O_WOLF].save_flags = 1;
		g_AnimBones[g_Objects[O_WOLF].bone_index + 8] |= BEB_ROT_Y;
	}

	if (g_Objects[O_BEAR].loaded)
	{
		g_Objects[O_BEAR].initialise = InitialiseCreature;
		g_Objects[O_BEAR].control = BearControl;
		g_Objects[O_BEAR].collision = CreatureCollision;
		g_Objects[O_BEAR].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_BEAR].hit_points = BEAR_HITPOINTS;
		g_Objects[O_BEAR].pivot_length = 500;
		g_Objects[O_BEAR].radius = BEAR_RADIUS;
		g_Objects[O_BEAR].smartness = BEAR_SMARTNESS;
		g_Objects[O_BEAR].intelligent = 1;
		g_Objects[O_BEAR].save_position = 1;
		g_Objects[O_BEAR].save_hitpoints = 1;
		g_Objects[O_BEAR].save_anim = 1;
		g_Objects[O_BEAR].save_flags = 1;
		g_AnimBones[g_Objects[O_BEAR].bone_index + 52] |= BEB_ROT_Y;
	}

	if (g_Objects[O_BAT].loaded)
	{
		g_Objects[O_BAT].initialise = InitialiseBat;
		g_Objects[O_BAT].control = BatControl;
		g_Objects[O_BAT].collision = CreatureCollision;
		g_Objects[O_BAT].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_BAT].hit_points = BAT_HITPOINTS;
		g_Objects[O_BAT].radius = BAT_RADIUS;
		g_Objects[O_BAT].smartness = BAT_SMARTNESS;
		g_Objects[O_BAT].intelligent = 1;
		g_Objects[O_BAT].save_position = 1;
		g_Objects[O_BAT].save_hitpoints = 1;
		g_Objects[O_BAT].save_anim = 1;
		g_Objects[O_BAT].save_flags = 1;
	}

	if (g_Objects[O_DINOSAUR].loaded)
	{
		g_Objects[O_DINOSAUR].initialise = InitialiseCreature;
		g_Objects[O_DINOSAUR].control = DinoControl;
		g_Objects[O_DINOSAUR].draw_routine = DrawUnclippedItem;
		g_Objects[O_DINOSAUR].collision = CreatureCollision;
		g_Objects[O_DINOSAUR].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_DINOSAUR].hit_points = DINO_HITPOINTS;
		g_Objects[O_DINOSAUR].pivot_length = 2000;
		g_Objects[O_DINOSAUR].radius = DINO_RADIUS;
		g_Objects[O_DINOSAUR].smartness = DINO_SMARTNESS;
		g_Objects[O_DINOSAUR].intelligent = 1;
		g_Objects[O_DINOSAUR].save_position = 1;
		g_Objects[O_DINOSAUR].save_hitpoints = 1;
		g_Objects[O_DINOSAUR].save_anim = 1;
		g_Objects[O_DINOSAUR].save_flags = 1;
		g_AnimBones[g_Objects[O_DINOSAUR].bone_index + 40] |= BEB_ROT_Y;
		g_AnimBones[g_Objects[O_DINOSAUR].bone_index + 44] |= BEB_ROT_Y;
	}

	if (!g_Objects[O_RAPTOR].loaded)
	{
		g_Objects[O_RAPTOR].initialise = InitialiseCreature;
		g_Objects[O_RAPTOR].control = RaptorControl;
		g_Objects[O_RAPTOR].collision = CreatureCollision;
		g_Objects[O_RAPTOR].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_RAPTOR].hit_points = RAPTOR_HITPOINTS;
		g_Objects[O_RAPTOR].pivot_length = 400;
		g_Objects[O_RAPTOR].radius = RAPTOR_RADIUS;
		g_Objects[O_RAPTOR].smartness = RAPTOR_SMARTNESS;
		g_Objects[O_RAPTOR].intelligent = 1;
		g_Objects[O_RAPTOR].save_position = 1;
		g_Objects[O_RAPTOR].save_hitpoints = 1;
		g_Objects[O_RAPTOR].save_anim = 1;
		g_Objects[O_RAPTOR].save_flags = 1;
		g_AnimBones[g_Objects[O_RAPTOR].bone_index + 84] |= BEB_ROT_Y;
	}

	if (g_Objects[O_LARSON].loaded)
	{
		g_Objects[O_LARSON].initialise = InitialiseCreature;
		g_Objects[O_LARSON].control = LarsonControl;
		g_Objects[O_LARSON].collision = CreatureCollision;
		g_Objects[O_LARSON].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_LARSON].hit_points = LARSON_HITPOINTS;
		g_Objects[O_LARSON].radius = LARSON_RADIUS;
		g_Objects[O_LARSON].smartness = LARSON_SMARTNESS;
		g_Objects[O_LARSON].intelligent = 1;
		g_Objects[O_LARSON].save_position = 1;
		g_Objects[O_LARSON].save_hitpoints = 1;
		g_Objects[O_LARSON].save_anim = 1;
		g_Objects[O_LARSON].save_flags = 1;
		g_AnimBones[g_Objects[O_LARSON].bone_index + 24] |= BEB_ROT_Y;
	}

	if (g_Objects[O_PIERRE].loaded)
	{
		g_Objects[O_PIERRE].initialise = InitialiseCreature;
		g_Objects[O_PIERRE].control = PierreControl;
		g_Objects[O_PIERRE].collision = CreatureCollision;
		g_Objects[O_PIERRE].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_PIERRE].hit_points = PIERRE_HITPOINTS;
		g_Objects[O_PIERRE].radius = PIERRE_RADIUS;
		g_Objects[O_PIERRE].smartness = PIERRE_SMARTNESS;
		g_Objects[O_PIERRE].intelligent = 1;
		g_Objects[O_PIERRE].save_position = 1;
		g_Objects[O_PIERRE].save_hitpoints = 1;
		g_Objects[O_PIERRE].save_anim = 1;
		g_Objects[O_PIERRE].save_flags = 1;
		g_AnimBones[g_Objects[O_PIERRE].bone_index + 24] |= BEB_ROT_Y;
	}

	if (g_Objects[O_RAT].loaded)
	{
		g_Objects[O_RAT].initialise = InitialiseCreature;
		g_Objects[O_RAT].control = RatControl;
		g_Objects[O_RAT].collision = CreatureCollision;
		g_Objects[O_RAT].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_RAT].hit_points = RAT_HITPOINTS;
		g_Objects[O_RAT].pivot_length = 200;
		g_Objects[O_RAT].radius = RAT_RADIUS;
		g_Objects[O_RAT].smartness = RAT_SMARTNESS;
		g_Objects[O_RAT].intelligent = 1;
		g_Objects[O_RAT].save_position = 1;
		g_Objects[O_RAT].save_hitpoints = 1;
		g_Objects[O_RAT].save_anim = 1;
		g_Objects[O_RAT].save_flags = 1;
		g_AnimBones[g_Objects[O_RAT].bone_index + 4] |= BEB_ROT_Y;
	}

	if (g_Objects[O_VOLE].loaded)
	{
		g_Objects[O_VOLE].initialise = InitialiseCreature;
		g_Objects[O_VOLE].control = VoleControl;
		g_Objects[O_VOLE].collision = CreatureCollision;
		g_Objects[O_VOLE].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_VOLE].hit_points = RAT_HITPOINTS;
		g_Objects[O_VOLE].pivot_length = 200;
		g_Objects[O_VOLE].radius = RAT_RADIUS;
		g_Objects[O_VOLE].smartness = RAT_SMARTNESS;
		g_Objects[O_VOLE].intelligent = 1;
		g_Objects[O_VOLE].save_position = 1;
		g_Objects[O_VOLE].save_hitpoints = 1;
		g_Objects[O_VOLE].save_anim = 1;
		g_Objects[O_VOLE].save_flags = 1;
		g_AnimBones[g_Objects[O_VOLE].bone_index + 4] |= BEB_ROT_Y;
	}

	if (g_Objects[O_LION].loaded)
	{
		g_Objects[O_LION].initialise = InitialiseCreature;
		g_Objects[O_LION].control = LionControl;
		g_Objects[O_LION].collision = CreatureCollision;
		g_Objects[O_LION].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_LION].hit_points = LION_HITPOINTS;
		g_Objects[O_LION].pivot_length = 400;
		g_Objects[O_LION].radius = LION_RADIUS;
		g_Objects[O_LION].smartness = LION_SMARTNESS;
		g_Objects[O_LION].intelligent = 1;
		g_Objects[O_LION].save_position = 1;
		g_Objects[O_LION].save_hitpoints = 1;
		g_Objects[O_LION].save_anim = 1;
		g_Objects[O_LION].save_flags = 1;
		g_AnimBones[g_Objects[O_LION].bone_index + 76] |= BEB_ROT_Y;
	}

	if (g_Objects[O_LIONESS].loaded)
	{
		g_Objects[O_LIONESS].initialise = InitialiseCreature;
		g_Objects[O_LIONESS].control = LionControl;
		g_Objects[O_LIONESS].collision = CreatureCollision;
		g_Objects[O_LIONESS].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_LIONESS].hit_points = LIONESS_HITPOINTS;
		g_Objects[O_LIONESS].pivot_length = 400;
		g_Objects[O_LIONESS].radius = LIONESS_RADIUS;
		g_Objects[O_LIONESS].smartness = LIONESS_SMARTNESS;
		g_Objects[O_LIONESS].intelligent = 1;
		g_Objects[O_LIONESS].save_position = 1;
		g_Objects[O_LIONESS].save_hitpoints = 1;
		g_Objects[O_LIONESS].save_anim = 1;
		g_Objects[O_LIONESS].save_flags = 1;
		g_AnimBones[g_Objects[O_LIONESS].bone_index + 76] |= BEB_ROT_Y;
	}

	if (g_Objects[O_PUMA].loaded)
	{
		g_Objects[O_PUMA].initialise = InitialiseCreature;
		g_Objects[O_PUMA].control = LionControl;
		g_Objects[O_PUMA].collision = CreatureCollision;
		g_Objects[O_PUMA].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_PUMA].hit_points = PUMA_HITPOINTS;
		g_Objects[O_PUMA].pivot_length = 400;
		g_Objects[O_PUMA].radius = PUMA_RADIUS;
		g_Objects[O_PUMA].smartness = PUMA_SMARTNESS;
		g_Objects[O_PUMA].intelligent = 1;
		g_Objects[O_PUMA].save_position = 1;
		g_Objects[O_PUMA].save_hitpoints = 1;
		g_Objects[O_PUMA].save_anim = 1;
		g_Objects[O_PUMA].save_flags = 1;
		g_AnimBones[g_Objects[O_PUMA].bone_index + 76] |= BEB_ROT_Y;
	}

	if (g_Objects[O_CROCODILE].loaded)
	{
		g_Objects[O_CROCODILE].initialise = InitialiseCreature;
		g_Objects[O_CROCODILE].control = CrocControl;
		g_Objects[O_CROCODILE].collision = CreatureCollision;
		g_Objects[O_CROCODILE].shadow_size = UNIT_SHADOW / 3;
		g_Objects[O_CROCODILE].hit_points = CROCODILE_HITPOINTS;
		g_Objects[O_CROCODILE].pivot_length = 600;
		g_Objects[O_CROCODILE].radius = CROCODILE_RADIUS;
		g_Objects[O_CROCODILE].smartness = CROCODILE_SMARTNESS;
		g_Objects[O_CROCODILE].intelligent = 1;
		g_Objects[O_CROCODILE].save_position = 1;
		g_Objects[O_CROCODILE].save_hitpoints = 1;
		g_Objects[O_CROCODILE].save_anim = 1;
		g_Objects[O_CROCODILE].save_flags = 1;
		g_AnimBones[g_Objects[O_CROCODILE].bone_index + 28] |= BEB_ROT_Y;
	}

	if (g_Objects[O_ALLIGATOR].loaded)
	{
		g_Objects[O_ALLIGATOR].initialise = InitialiseCreature;
		g_Objects[O_ALLIGATOR].control = AlligatorControl;
		g_Objects[O_ALLIGATOR].collision = CreatureCollision;
		g_Objects[O_ALLIGATOR].shadow_size = UNIT_SHADOW / 3;
		g_Objects[O_ALLIGATOR].hit_points = ALLIGATOR_HITPOINTS;
		g_Objects[O_ALLIGATOR].pivot_length = 600;
		g_Objects[O_ALLIGATOR].radius = ALLIGATOR_RADIUS;
		g_Objects[O_ALLIGATOR].smartness = ALLIGATOR_SMARTNESS;
		g_Objects[O_ALLIGATOR].intelligent = 1;
		g_Objects[O_ALLIGATOR].save_position = 1;
		g_Objects[O_ALLIGATOR].save_hitpoints = 1;
		g_Objects[O_ALLIGATOR].save_anim = 1;
		g_Objects[O_ALLIGATOR].save_flags = 1;
		g_AnimBones[g_Objects[O_ALLIGATOR].bone_index + 28] |= BEB_ROT_Y;
	}

	if (g_Objects[O_APE].loaded)
	{
		g_Objects[O_APE].initialise = InitialiseCreature;
		g_Objects[O_APE].control = ApeControl;
		g_Objects[O_APE].collision = CreatureCollision;
		g_Objects[O_APE].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_APE].hit_points = APE_HITPOINTS;
		g_Objects[O_APE].pivot_length = 250;
		g_Objects[O_APE].radius = APE_RADIUS;
		g_Objects[O_APE].smartness = APE_SMARTNESS;
		g_Objects[O_APE].intelligent = 1;
		g_Objects[O_APE].save_position = 1;
		g_Objects[O_APE].save_hitpoints = 1;
		g_Objects[O_APE].save_anim = 1;
		g_Objects[O_APE].save_flags = 1;
		g_AnimBones[g_Objects[O_APE].bone_index + 52] |= BEB_ROT_Y;
	}

	if (g_Objects[O_WARRIOR1].loaded)
	{
		g_Objects[O_WARRIOR1].initialise = InitialiseCreature;
		g_Objects[O_WARRIOR1].control = FlyerControl;
		g_Objects[O_WARRIOR1].collision = CreatureCollision;
		g_Objects[O_WARRIOR1].shadow_size = UNIT_SHADOW / 3;
		g_Objects[O_WARRIOR1].hit_points = FLYER_HITPOINTS;
		g_Objects[O_WARRIOR1].pivot_length = 150;
		g_Objects[O_WARRIOR1].radius = FLYER_RADIUS;
		g_Objects[O_WARRIOR1].smartness = FLYER_SMARTNESS;
		g_Objects[O_WARRIOR1].intelligent = 1;
		g_Objects[O_WARRIOR1].save_position = 1;
		g_Objects[O_WARRIOR1].save_hitpoints = 1;
		g_Objects[O_WARRIOR1].save_anim = 1;
		g_Objects[O_WARRIOR1].save_flags = 1;
		g_AnimBones[g_Objects[O_WARRIOR1].bone_index] |= BEB_ROT_Y;
		g_AnimBones[g_Objects[O_WARRIOR1].bone_index + 8] |= BEB_ROT_Y;
	}

	if (g_Objects[O_WARRIOR2].loaded)
	{
		g_Objects[O_WARRIOR2] = g_Objects[O_WARRIOR1];
		g_Objects[O_WARRIOR2].initialise = InitialiseWarrior2;
		g_Objects[O_WARRIOR2].smartness = WARRIOR2_SMARTNESS;
	}

	if (g_Objects[O_WARRIOR3].loaded)
	{
		g_Objects[O_WARRIOR3] = g_Objects[O_WARRIOR1];
		g_Objects[O_WARRIOR3].initialise = InitialiseWarrior2;
	}

	if (g_Objects[O_CENTAUR].loaded)
	{
		g_Objects[O_CENTAUR].initialise = InitialiseCreature;
		g_Objects[O_CENTAUR].control = CentaurControl;
		g_Objects[O_CENTAUR].collision = CreatureCollision;
		g_Objects[O_CENTAUR].shadow_size = UNIT_SHADOW / 3;
		g_Objects[O_CENTAUR].hit_points = CENTAUR_HITPOINTS;
		g_Objects[O_CENTAUR].pivot_length = 400;
		g_Objects[O_CENTAUR].radius = CENTAUR_RADIUS;
		g_Objects[O_CENTAUR].smartness = CENTAUR_SMARTNESS;
		g_Objects[O_CENTAUR].intelligent = 1;
		g_Objects[O_CENTAUR].save_position = 1;
		g_Objects[O_CENTAUR].save_hitpoints = 1;
		g_Objects[O_CENTAUR].save_anim = 1;
		g_Objects[O_CENTAUR].save_flags = 1;
		g_AnimBones[g_Objects[O_CENTAUR].bone_index + 40] |= 0xCu;
	}

	if (g_Objects[O_MUMMY].loaded)
	{
		g_Objects[O_MUMMY].initialise = InitialiseMummy;
		g_Objects[O_MUMMY].control = MummyControl;
		g_Objects[O_MUMMY].collision = ObjectCollision;
		g_Objects[O_MUMMY].hit_points = MUMMY_HITPOINTS;
		g_Objects[O_MUMMY].save_flags = 1;
		g_Objects[O_MUMMY].save_hitpoints = 1;
		g_Objects[O_MUMMY].save_anim = 1;
		g_AnimBones[g_Objects[O_MUMMY].bone_index + 8] |= BEB_ROT_Y;
	}

	if (g_Objects[O_MERCENARY1].loaded)
	{
		g_Objects[O_MERCENARY1].initialise = InitialiseSkateKid;
		g_Objects[O_MERCENARY1].control = SkateKidControl;
		g_Objects[O_MERCENARY1].draw_routine = DrawSkateKid;
		g_Objects[O_MERCENARY1].collision = CreatureCollision;
		g_Objects[O_MERCENARY1].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_MERCENARY1].hit_points = SKATE_KID_HITPOINTS;
		g_Objects[O_MERCENARY1].radius = SKATE_KID_RADIUS;
		g_Objects[O_MERCENARY1].smartness = SKATE_KID_SMARTNESS;
		g_Objects[O_MERCENARY1].intelligent = 1;
		g_Objects[O_MERCENARY1].save_position = 1;
		g_Objects[O_MERCENARY1].save_hitpoints = 1;
		g_Objects[O_MERCENARY1].save_anim = 1;
		g_Objects[O_MERCENARY1].save_flags = 1;
		g_AnimBones[g_Objects[O_MERCENARY1].bone_index] |= BEB_ROT_Y;
	}

	if (g_Objects[O_MERCENARY2].loaded)
	{
		g_Objects[O_MERCENARY2].initialise = InitialiseCreature;
		g_Objects[O_MERCENARY2].control = CowboyControl;
		g_Objects[O_MERCENARY2].collision = CreatureCollision;
		g_Objects[O_MERCENARY2].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_MERCENARY2].hit_points = COWBOY_HITPOINTS;
		g_Objects[O_MERCENARY2].radius = COWBOY_RADIUS;
		g_Objects[O_MERCENARY2].smartness = COWBOY_SMARTNESS;
		g_Objects[O_MERCENARY2].intelligent = 1;
		g_Objects[O_MERCENARY2].save_position = 1;
		g_Objects[O_MERCENARY2].save_hitpoints = 1;
		g_Objects[O_MERCENARY2].save_anim = 1;
		g_Objects[O_MERCENARY2].save_flags = 1;
		g_AnimBones[g_Objects[O_MERCENARY2].bone_index] |= BEB_ROT_Y;
	}

	if (g_Objects[O_MERCENARY3].loaded)
	{
		g_Objects[O_MERCENARY3].initialise = InitialiseBaldy;
		g_Objects[O_MERCENARY3].control = BaldyControl;
		g_Objects[O_MERCENARY3].collision = CreatureCollision;
		g_Objects[O_MERCENARY3].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_MERCENARY3].hit_points = BALDY_HITPOINTS;
		g_Objects[O_MERCENARY3].radius = BALDY_RADIUS;
		g_Objects[O_MERCENARY3].smartness = BALDY_SMARTNESS;
		g_Objects[O_MERCENARY3].intelligent = 1;
		g_Objects[O_MERCENARY3].save_position = 1;
		g_Objects[O_MERCENARY3].save_hitpoints = 1;
		g_Objects[O_MERCENARY3].save_anim = 1;
		g_Objects[O_MERCENARY3].save_flags = 1;
		g_AnimBones[g_Objects[O_MERCENARY3].bone_index] |= BEB_ROT_Y;
	}

	if (g_Objects[O_ABORTION].loaded)
	{
		g_Objects[O_ABORTION].initialise = InitialiseCreature;
		g_Objects[O_ABORTION].control = AbortionControl;
		g_Objects[O_ABORTION].collision = CreatureCollision;
		g_Objects[O_ABORTION].shadow_size = UNIT_SHADOW / 3;
		g_Objects[O_ABORTION].hit_points = ABORTION_HITPOINTS;
		g_Objects[O_ABORTION].radius = ABORTION_RADIUS;
		g_Objects[O_ABORTION].smartness = ABORTION_SMARTNESS;
		g_Objects[O_ABORTION].intelligent = 1;
		g_Objects[O_ABORTION].save_position = 1;
		g_Objects[O_ABORTION].save_hitpoints = 1;
		g_Objects[O_ABORTION].save_anim = 1;
		g_Objects[O_ABORTION].save_flags = 1;
		g_AnimBones[g_Objects[O_ABORTION].bone_index + 4] |= BEB_ROT_Y;
	}

	if (g_Objects[O_NATLA].loaded)
	{
		g_Objects[O_NATLA].collision = CreatureCollision;
		g_Objects[O_NATLA].initialise = InitialiseCreature;
		g_Objects[O_NATLA].control = NatlaControl;
		g_Objects[O_NATLA].shadow_size = UNIT_SHADOW / 2;
		g_Objects[O_NATLA].hit_points = NATLA_HITPOINTS;
		g_Objects[O_NATLA].radius = NATLA_RADIUS;
		g_Objects[O_NATLA].smartness = NATLA_SMARTNESS;
		g_Objects[O_NATLA].intelligent = 1;
		g_Objects[O_NATLA].save_position = 1;
		g_Objects[O_NATLA].save_hitpoints = 1;
		g_Objects[O_NATLA].save_anim = 1;
		g_Objects[O_NATLA].save_flags = 1;
		g_AnimBones[g_Objects[O_NATLA].bone_index + 8] |= BEB_ROT_Z | BEB_ROT_X;
	}

	if (g_Objects[O_PODS].loaded)
	{
		g_Objects[O_PODS].initialise = InitialisePod;
		g_Objects[O_PODS].control = PodControl;
		g_Objects[O_PODS].collision = ObjectCollision;
		g_Objects[O_PODS].save_anim = 1;
		g_Objects[O_PODS].save_flags = 1;
	}

	if (g_Objects[O_BIG_POD].loaded)
	{
		g_Objects[O_BIG_POD].initialise = InitialisePod;
		g_Objects[O_BIG_POD].control = PodControl;
		g_Objects[O_BIG_POD].collision = ObjectCollision;
		g_Objects[O_BIG_POD].save_anim = 1;
		g_Objects[O_BIG_POD].save_flags = 1;
	}

	if (g_Objects[O_STATUE].loaded)
	{
		g_Objects[O_STATUE].initialise = InitialiseStatue;
		g_Objects[O_STATUE].control = StatueControl;
		g_Objects[O_STATUE].collision = ObjectCollision;
		g_Objects[O_STATUE].save_anim = 1;
		g_Objects[O_STATUE].save_flags = 1;
	}
}

void TrapObjects()
{
	g_Objects[O_FALLING_BLOCK].control = FallingBlockControl; // obj# 35
	g_Objects[O_FALLING_BLOCK].floor = FallingBlockFloor;
	g_Objects[O_FALLING_BLOCK].ceiling = FallingBlockCeiling;
	g_Objects[O_FALLING_BLOCK].save_position = 1;
	g_Objects[O_FALLING_BLOCK].save_anim = 1;
	g_Objects[O_FALLING_BLOCK].save_flags = 1;

	g_Objects[O_PENDULUM].control = PendulumControl; // obj# 36
	g_Objects[O_PENDULUM].collision = TrapCollision;
	g_Objects[O_PENDULUM].shadow_size = UNIT_SHADOW / 2;
	g_Objects[O_PENDULUM].save_flags = 1;
	g_Objects[O_PENDULUM].save_anim = 1;

	g_Objects[O_TEETH_TRAP].control = TeethTrapControl;					// obj# 42
	g_Objects[O_TEETH_TRAP].collision = TrapCollision;
	g_Objects[O_TEETH_TRAP].save_flags = 1;
	g_Objects[O_TEETH_TRAP].save_anim = 1;

	g_Objects[O_ROLLING_BALL].initialise = InitialiseRollingBall;			// obj# 38
	g_Objects[O_ROLLING_BALL].control = RollingBallControl;
	g_Objects[O_ROLLING_BALL].collision = RollingBallCollision;
	g_Objects[O_ROLLING_BALL].save_position = 1;
	g_Objects[O_ROLLING_BALL].save_anim = 1;
	g_Objects[O_ROLLING_BALL].save_flags = 1;

	g_Objects[O_SPIKES].collision = SpikeCollision;					// obj# 37

	g_Objects[O_FALLING_CEILING1].control = FallingCeilingControl;	// obj# 53
	g_Objects[O_FALLING_CEILING1].collision = TrapCollision;
	g_Objects[O_FALLING_CEILING1].save_position = 1;
	g_Objects[O_FALLING_CEILING1].save_anim = 1;
	g_Objects[O_FALLING_CEILING1].save_flags = 1;

	g_Objects[O_FALLING_CEILING2].control = FallingCeilingControl;	// obj# 54
	g_Objects[O_FALLING_CEILING2].collision = TrapCollision;
	g_Objects[O_FALLING_CEILING2].save_position = 1;
	g_Objects[O_FALLING_CEILING2].save_anim = 1;
	g_Objects[O_FALLING_CEILING2].save_flags = 1;

	g_Objects[O_DAMOCLES_SWORD].initialise = InitialiseDamoclesSword;				// obj# 43
	g_Objects[O_DAMOCLES_SWORD].control = DamoclesSwordControl;
	g_Objects[O_DAMOCLES_SWORD].collision = DamoclesSwordCollision;
	g_Objects[O_DAMOCLES_SWORD].shadow_size = UNIT_SHADOW;
	g_Objects[O_DAMOCLES_SWORD].save_position = 1;
	g_Objects[O_DAMOCLES_SWORD].save_anim = 1;
	g_Objects[O_DAMOCLES_SWORD].save_flags = 1;

	g_Objects[O_MOVABLE_BLOCK].initialise = InitialiseMovableBlock; // obj# 48
	g_Objects[O_MOVABLE_BLOCK].control = MovableBlockControl;
	g_Objects[O_MOVABLE_BLOCK].draw_routine = DrawMovableBlock;
	g_Objects[O_MOVABLE_BLOCK].collision = MovableBlockCollision;
	g_Objects[O_MOVABLE_BLOCK].save_position = 1;
	g_Objects[O_MOVABLE_BLOCK].save_anim = 1;
	g_Objects[O_MOVABLE_BLOCK].save_flags = 1;

	g_Objects[O_MOVABLE_BLOCK2].initialise = InitialiseMovableBlock; // obj# 49
	g_Objects[O_MOVABLE_BLOCK2].control = MovableBlockControl;
	g_Objects[O_MOVABLE_BLOCK2].draw_routine = DrawMovableBlock;
	g_Objects[O_MOVABLE_BLOCK2].collision = MovableBlockCollision;
	g_Objects[O_MOVABLE_BLOCK2].save_position = 1;
	g_Objects[O_MOVABLE_BLOCK2].save_anim = 1;
	g_Objects[O_MOVABLE_BLOCK2].save_flags = 1;

	g_Objects[O_MOVABLE_BLOCK3].initialise = InitialiseMovableBlock; // obj# 50
	g_Objects[O_MOVABLE_BLOCK3].control = MovableBlockControl;
	g_Objects[O_MOVABLE_BLOCK3].draw_routine = DrawMovableBlock;
	g_Objects[O_MOVABLE_BLOCK3].collision = MovableBlockCollision;
	g_Objects[O_MOVABLE_BLOCK3].save_position = 1;
	g_Objects[O_MOVABLE_BLOCK3].save_anim = 1;
	g_Objects[O_MOVABLE_BLOCK3].save_flags = 1;

	g_Objects[O_MOVABLE_BLOCK4].initialise = InitialiseMovableBlock; // obj# 41
	g_Objects[O_MOVABLE_BLOCK4].control = MovableBlockControl;
	g_Objects[O_MOVABLE_BLOCK4].draw_routine = DrawMovableBlock;
	g_Objects[O_MOVABLE_BLOCK4].collision = MovableBlockCollision;
	g_Objects[O_MOVABLE_BLOCK4].save_position = 1;
	g_Objects[O_MOVABLE_BLOCK4].save_anim = 1;
	g_Objects[O_MOVABLE_BLOCK4].save_flags = 1;

	g_Objects[O_ROLLING_BLOCK].initialise = InitialiseRollingBlock; // obj# 52
	g_Objects[O_ROLLING_BLOCK].control = RollingBlockControl;
	g_Objects[O_ROLLING_BLOCK].save_position = 1;
	g_Objects[O_ROLLING_BLOCK].save_anim = 1;
	g_Objects[O_ROLLING_BLOCK].save_flags = 1;

	g_Objects[O_LIGHTNING_EMITTER].initialise = InitialiseLightning;		// obj# 46
	g_Objects[O_LIGHTNING_EMITTER].control = LightningControl;
	g_Objects[O_LIGHTNING_EMITTER].draw_routine = DrawLightning;
	g_Objects[O_LIGHTNING_EMITTER].collision = LightningCollision;
	g_Objects[O_LIGHTNING_EMITTER].save_flags = 1;

	g_Objects[O_THORS_HANDLE].initialise = InitialiseThorsHandle;			// obj# 44
	g_Objects[O_THORS_HANDLE].control = ThorsHandleControl;
	g_Objects[O_THORS_HANDLE].draw_routine = DrawUnclippedItem;
	g_Objects[O_THORS_HANDLE].collision = ThorsHandleCollision;
	g_Objects[O_THORS_HANDLE].save_flags = 1;
	g_Objects[O_THORS_HANDLE].save_anim = 1;

	g_Objects[O_THORS_HEAD].collision = ThorsHeadCollision;			// obj# 45
	g_Objects[O_THORS_HEAD].draw_routine = DrawUnclippedItem;
	g_Objects[O_THORS_HEAD].save_flags = 1;
	g_Objects[O_THORS_HEAD].save_anim = 1;

	g_Objects[O_MIDAS_TOUCH].collision = MidasCollision;	// obj# 128
	g_Objects[O_MIDAS_TOUCH].draw_routine = DrawDummyItem;

	g_Objects[O_DART_EMITTER].control = DartEmitterControl; // obj# 40

	g_Objects[O_DARTS].collision = ObjectCollision; // obj# 39
	g_Objects[O_DARTS].control = DartsControl;
	g_Objects[O_DARTS].shadow_size = UNIT_SHADOW / 2;
	g_Objects[O_DARTS].save_flags = 1;

	g_Objects[O_DART_EFFECT].control = DartEffectControl; // obj# 160
	g_Objects[O_DART_EFFECT].draw_routine = DrawSpriteItem;

	g_Objects[O_FLAME_EMITTER].control = FlameEmitterControl;	// obj# 179
	g_Objects[O_FLAME_EMITTER].draw_routine = DrawDummyItem;

	g_Objects[O_FLAME].control = FlameControl;		// obj# 178

	g_Objects[O_LAVA_EMITTER].control = LavaEmitterControl;	// obj# 177
	g_Objects[O_LAVA_EMITTER].draw_routine = DrawDummyItem;
	g_Objects[O_LAVA_EMITTER].collision = ObjectCollision;

	g_Objects[O_LAVA].control = LavaControl;		// obj# 176

	g_Objects[O_LAVA_WEDGE].control = LavaWedgeControl;	// obj# 180
	g_Objects[O_LAVA_WEDGE].collision = CreatureCollision;
	g_Objects[O_LAVA_WEDGE].save_position = 1;
	g_Objects[O_LAVA_WEDGE].save_anim = 1;
	g_Objects[O_LAVA_WEDGE].save_flags = 1;
}

void ObjectObjects()
{
	g_Objects[O_CAMERA_TARGET].draw_routine = DrawDummyItem; // obj# 169

	g_Objects[O_BRIDGE_FLAT].floor = BridgeFlatFloor; // obj# 68
	g_Objects[O_BRIDGE_FLAT].ceiling = BridgeFlatCeiling;

	g_Objects[O_BRIDGE_TILT1].floor = BridgeTilt1Floor; // obj# 69
	g_Objects[O_BRIDGE_TILT1].ceiling = BridgeTilt1Ceiling;

	g_Objects[O_BRIDGE_TILT2].floor = BridgeTilt2Floor; // obj# 70
	g_Objects[O_BRIDGE_TILT2].ceiling = BridgeTilt2Ceiling;

	if (g_Objects[O_DRAW_BRIDGE].loaded)
	{
		g_Objects[O_DRAW_BRIDGE].ceiling = DrawBridgeCeiling; // obj# 41
		g_Objects[O_DRAW_BRIDGE].collision = DrawBridgeCollision;
		g_Objects[O_DRAW_BRIDGE].control = CogControl;
		g_Objects[O_DRAW_BRIDGE].save_anim = 1;
		g_Objects[O_DRAW_BRIDGE].save_flags = 1;
		g_Objects[O_DRAW_BRIDGE].floor = DrawBridgeFloor;
	}

	g_Objects[O_SWITCH_TYPE1].control = SwitchControl; // obj# 55
	g_Objects[O_SWITCH_TYPE1].collision = SwitchCollision;
	g_Objects[O_SWITCH_TYPE1].save_anim = 1;
	g_Objects[O_SWITCH_TYPE1].save_flags = 1;

	g_Objects[O_SWITCH_TYPE2].control = SwitchControl; // obj# 56
	g_Objects[O_SWITCH_TYPE2].collision = SwitchCollision2;
	g_Objects[O_SWITCH_TYPE2].save_anim = 1;
	g_Objects[O_SWITCH_TYPE2].save_flags = 1;

	g_Objects[O_DOOR_TYPE1].initialise = InitialiseDoor; // obj# 57
	g_Objects[O_DOOR_TYPE1].control = DoorControl;
	g_Objects[O_DOOR_TYPE1].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE1].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE1].save_anim = 1;
	g_Objects[O_DOOR_TYPE1].save_flags = 1;

	g_Objects[O_DOOR_TYPE2].initialise = InitialiseDoor; // obj# 58
	g_Objects[O_DOOR_TYPE2].control = DoorControl;
	g_Objects[O_DOOR_TYPE2].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE2].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE2].save_anim = 1;
	g_Objects[O_DOOR_TYPE2].save_flags = 1;

	g_Objects[O_DOOR_TYPE3].initialise = InitialiseDoor; // obj# 59
	g_Objects[O_DOOR_TYPE3].control = DoorControl;
	g_Objects[O_DOOR_TYPE3].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE3].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE3].save_anim = 1;
	g_Objects[O_DOOR_TYPE3].save_flags = 1;

	g_Objects[O_DOOR_TYPE4].initialise = InitialiseDoor; // obj# 60
	g_Objects[O_DOOR_TYPE4].control = DoorControl;
	g_Objects[O_DOOR_TYPE4].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE4].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE4].save_anim = 1;
	g_Objects[O_DOOR_TYPE4].save_flags = 1;

	//решетка под водой Мидас начало уровня
	g_Objects[O_DOOR_TYPE5].initialise = InitialiseDoor; // obj# 61
	g_Objects[O_DOOR_TYPE5].control = DoorControl;
	g_Objects[O_DOOR_TYPE5].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE5].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE5].save_anim = 1;
	g_Objects[O_DOOR_TYPE5].save_flags = 1;

	g_Objects[O_DOOR_TYPE6].initialise = InitialiseDoor; // obj# 62
	g_Objects[O_DOOR_TYPE6].control = DoorControl;
	g_Objects[O_DOOR_TYPE6].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE6].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE6].save_anim = 1;
	g_Objects[O_DOOR_TYPE6].save_flags = 1;

	g_Objects[O_DOOR_TYPE7].initialise = InitialiseDoor; // obj# 63
	g_Objects[O_DOOR_TYPE7].control = DoorControl;
	g_Objects[O_DOOR_TYPE7].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE7].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE7].save_anim = 1;
	g_Objects[O_DOOR_TYPE7].save_flags = 1;

	g_Objects[O_DOOR_TYPE8].initialise = InitialiseDoor; // obj# 64
	g_Objects[O_DOOR_TYPE8].control = DoorControl;
	g_Objects[O_DOOR_TYPE8].draw_routine = DrawUnclippedItem;
	g_Objects[O_DOOR_TYPE8].collision = DoorCollision;
	g_Objects[O_DOOR_TYPE8].save_anim = 1;
	g_Objects[O_DOOR_TYPE8].save_flags = 1;

	g_Objects[O_TRAPDOOR].control = TrapDoorControl; // obj# 65
	g_Objects[O_TRAPDOOR].floor = TrapDoorFloor;
	g_Objects[O_TRAPDOOR].ceiling = TrapDoorCeiling;
	g_Objects[O_TRAPDOOR].save_anim = 1;
	g_Objects[O_TRAPDOOR].save_flags = 1;

	g_Objects[O_TRAPDOOR2].control = TrapDoorControl; // obj# 66
	g_Objects[O_TRAPDOOR2].floor = TrapDoorFloor;
	g_Objects[O_TRAPDOOR2].ceiling = TrapDoorCeiling;
	g_Objects[O_TRAPDOOR2].save_anim = 1;
	g_Objects[O_TRAPDOOR2].save_flags = 1;

	g_Objects[O_COG_1].control = CogControl;	// obj# 74
	g_Objects[O_COG_1].save_flags = 1;

	g_Objects[O_COG_2].control = CogControl;	// obj# 75
	g_Objects[O_COG_2].save_flags = 1;

	g_Objects[O_COG_3].control = CogControl;	// obj# 76
	g_Objects[O_COG_3].save_flags = 1;

	g_Objects[O_MOVING_BAR].control = CogControl;	// obj# 47
	g_Objects[O_MOVING_BAR].collision = ObjectCollision;
	g_Objects[O_MOVING_BAR].save_flags = 1;
	g_Objects[O_MOVING_BAR].save_anim = 1;
	g_Objects[O_MOVING_BAR].save_position = 1;

	g_Objects[O_PICKUP_ITEM1].draw_routine = DrawPickupItem; // obj# 141
	g_Objects[O_PICKUP_ITEM1].collision = PickUpCollision;
	g_Objects[O_PICKUP_ITEM1].save_flags = 1;

	g_Objects[O_PICKUP_ITEM2].draw_routine = DrawPickupItem; // obj# 142
	g_Objects[O_PICKUP_ITEM2].collision = PickUpCollision;
	g_Objects[O_PICKUP_ITEM2].save_flags = 1;

	g_Objects[O_KEY_ITEM1].draw_routine = DrawPickupItem; // obj# 129
	g_Objects[O_KEY_ITEM1].collision = PickUpCollision;
	g_Objects[O_KEY_ITEM1].save_flags = 1;

	g_Objects[O_KEY_ITEM2].draw_routine = DrawPickupItem; // obj# 130
	g_Objects[O_KEY_ITEM2].collision = PickUpCollision;
	g_Objects[O_KEY_ITEM2].save_flags = 1;

	g_Objects[O_KEY_ITEM3].draw_routine = DrawPickupItem; // obj# 131
	g_Objects[O_KEY_ITEM3].collision = PickUpCollision;
	g_Objects[O_KEY_ITEM3].save_flags = 1;

	g_Objects[O_KEY_ITEM4].draw_routine = DrawPickupItem; // obj# 132
	g_Objects[O_KEY_ITEM4].collision = PickUpCollision;
	g_Objects[O_KEY_ITEM4].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM1].draw_routine = DrawPickupItem; // obj# 110
	g_Objects[O_PUZZLE_ITEM1].collision = PickUpCollision;
	g_Objects[O_PUZZLE_ITEM1].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM2].draw_routine = DrawPickupItem; // obj# 111
	g_Objects[O_PUZZLE_ITEM2].collision = PickUpCollision;
	g_Objects[O_PUZZLE_ITEM2].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM3].draw_routine = DrawPickupItem; // obj# 112
	g_Objects[O_PUZZLE_ITEM3].collision = PickUpCollision;
	g_Objects[O_PUZZLE_ITEM3].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM4].draw_routine = DrawPickupItem; // obj# 113
	g_Objects[O_PUZZLE_ITEM4].collision = PickUpCollision;
	g_Objects[O_PUZZLE_ITEM4].save_flags = 1;

	g_Objects[O_GUN_ITEM].draw_routine = DrawPickupItem; // obj# 84
	g_Objects[O_GUN_ITEM].collision = PickUpCollision;
	g_Objects[O_GUN_ITEM].save_flags = 1;

	g_Objects[O_SHOTGUN_ITEM].draw_routine = DrawPickupItem; // obj# 85
	g_Objects[O_SHOTGUN_ITEM].collision = PickUpCollision;
	g_Objects[O_SHOTGUN_ITEM].save_flags = 1;

	g_Objects[O_MAGNUM_ITEM].draw_routine = DrawPickupItem; // obj# 86
	g_Objects[O_MAGNUM_ITEM].collision = PickUpCollision;
	g_Objects[O_MAGNUM_ITEM].save_flags = 1;

	g_Objects[O_UZI_ITEM].draw_routine = DrawPickupItem; // obj# 87
	g_Objects[O_UZI_ITEM].collision = PickUpCollision;
	g_Objects[O_UZI_ITEM].save_flags = 1;

	g_Objects[O_GUN_AMMO_ITEM].draw_routine = DrawPickupItem; // obj# 88
	g_Objects[O_GUN_AMMO_ITEM].collision = PickUpCollision;
	g_Objects[O_GUN_AMMO_ITEM].save_flags = 1;

	g_Objects[O_SG_AMMO_ITEM].draw_routine = DrawPickupItem; // obj# 89
	g_Objects[O_SG_AMMO_ITEM].collision = PickUpCollision;
	g_Objects[O_SG_AMMO_ITEM].save_flags = 1;

	g_Objects[O_MAG_AMMO_ITEM].draw_routine = DrawPickupItem; // obj# 90
	g_Objects[O_MAG_AMMO_ITEM].collision = PickUpCollision;
	g_Objects[O_MAG_AMMO_ITEM].save_flags = 1;

	g_Objects[O_UZI_AMMO_ITEM].draw_routine = DrawPickupItem; // obj# 91
	g_Objects[O_UZI_AMMO_ITEM].collision = PickUpCollision;
	g_Objects[O_UZI_AMMO_ITEM].save_flags = 1;

	g_Objects[O_EXPLOSIVE_ITEM].draw_routine = DrawPickupItem; // obj# 92
	g_Objects[O_EXPLOSIVE_ITEM].collision = PickUpCollision;
	g_Objects[O_EXPLOSIVE_ITEM].save_flags = 1;

	g_Objects[O_MEDI_ITEM].draw_routine = DrawPickupItem; // obj# 93
	g_Objects[O_MEDI_ITEM].collision = PickUpCollision;
	g_Objects[O_MEDI_ITEM].save_flags = 1;

	g_Objects[O_BIGMEDI_ITEM].draw_routine = DrawPickupItem; // obj# 94
	g_Objects[O_BIGMEDI_ITEM].collision = PickUpCollision;
	g_Objects[O_BIGMEDI_ITEM].save_flags = 1;

	g_Objects[O_SCION_ITEM].draw_routine = DrawPickupItem;	// obj# 143
	g_Objects[O_SCION_ITEM].collision = PickUpScionCollision;

	g_Objects[O_SCION_ITEM2].draw_routine = DrawPickupItem;	// obj# 144
	g_Objects[O_SCION_ITEM2].collision = PickUpCollision;
	g_Objects[O_SCION_ITEM2].save_flags = 1;

	g_Objects[O_SCION_ITEM3].control = Scion3Control;	// obj# 145
	g_Objects[O_SCION_ITEM3].hit_points = 5;
	g_Objects[O_SCION_ITEM3].save_flags = 1;

	g_Objects[O_SCION_ITEM4].control = ScionControl;	// obj# 146
	g_Objects[O_SCION_ITEM4].collision = PickUpScion4Collision;
	g_Objects[O_SCION_ITEM4].save_flags = 1;

	g_Objects[O_SCION_HOLDER].control = ScionControl;	// obj# 147
	g_Objects[O_SCION_HOLDER].collision = ObjectCollision;
	g_Objects[O_SCION_HOLDER].save_anim = 1;
	g_Objects[O_SCION_HOLDER].save_flags = 1;

	g_Objects[O_LEADBAR_ITEM].draw_routine = DrawPickupItem;	// obj# 126
	g_Objects[O_LEADBAR_ITEM].collision = PickUpCollision;
	g_Objects[O_LEADBAR_ITEM].save_flags = 1;

	//закоментировать строку ниже если нужно рисовать O_SAVEGAME_ITEM
	g_Objects[O_SAVEGAME_ITEM].draw_routine = DrawDummyItem; // obj# 83

	g_Objects[O_KEY_HOLE1].collision = KeyHoleCollision; // obj# 137
	g_Objects[O_KEY_HOLE1].save_flags = 1;

	g_Objects[O_KEY_HOLE2].collision = KeyHoleCollision; // obj# 138
	g_Objects[O_KEY_HOLE2].save_flags = 1;

	g_Objects[O_KEY_HOLE3].collision = KeyHoleCollision; // obj# 139
	g_Objects[O_KEY_HOLE3].save_flags = 1;

	g_Objects[O_KEY_HOLE4].collision = KeyHoleCollision; // obj# 140
	g_Objects[O_KEY_HOLE4].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE1].collision = PuzzleHoleCollision; // obj# 118
	g_Objects[O_PUZZLE_HOLE1].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE2].collision = PuzzleHoleCollision; // obj# 119
	g_Objects[O_PUZZLE_HOLE2].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE3].collision = PuzzleHoleCollision; // obj# 120
	g_Objects[O_PUZZLE_HOLE3].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE4].collision = PuzzleHoleCollision; // obj# 121
	g_Objects[O_PUZZLE_HOLE4].save_flags = 1;

	g_Objects[O_PUZZLE_DONE1].save_flags = 1; // obj# 122

	g_Objects[O_PUZZLE_DONE2].save_flags = 1; // obj# 122

	g_Objects[O_PUZZLE_DONE3].save_flags = 1; // obj# 122

	g_Objects[O_PUZZLE_DONE4].save_flags = 1; // obj# 122

	// O_PORTACABIN это комната на тросу Natla Mines уровень
	g_Objects[O_PORTACABIN].control = CabinControl;		// obj# 162
	g_Objects[O_PORTACABIN].draw_routine = DrawUnclippedItem;
	g_Objects[O_PORTACABIN].collision = ObjectCollision;
	g_Objects[O_PORTACABIN].save_anim = 1;
	g_Objects[O_PORTACABIN].save_flags = 1;

	g_Objects[O_BOAT].control = BoatControl;	// obj# 182
	g_Objects[O_BOAT].save_flags = 1;
	g_Objects[O_BOAT].save_anim = 1;
	g_Objects[O_BOAT].save_position = 1;

	g_Objects[O_EARTHQUAKE].control = EarthQuakeControl;	// obj# 183
	g_Objects[O_EARTHQUAKE].draw_routine = DrawDummyItem;
	g_Objects[O_EARTHQUAKE].save_flags = 1;

	//player 1 - obj# 77
	g_Objects[O_PLAYER_1].initialise = InitialisePlayer1;
	g_Objects[O_PLAYER_1].control = ControlCinematicPlayer;
	g_Objects[O_PLAYER_1].hit_points = 1;
	//player 2 - obj# 78
	g_Objects[O_PLAYER_2].initialise = InitialiseGenPlayer;
	g_Objects[O_PLAYER_2].control = ControlCinematicPlayer;
	g_Objects[O_PLAYER_2].hit_points = 1;
	//player 3 - obj# 79
	g_Objects[O_PLAYER_3].initialise = InitialiseGenPlayer;
	g_Objects[O_PLAYER_3].control = ControlCinematicPlayer;
	g_Objects[O_PLAYER_3].hit_points = 1;
	//player 4 - obj# 80
	g_Objects[O_PLAYER_4].initialise = InitialiseGenPlayer;
	g_Objects[O_PLAYER_4].control = ControlCinematicPlayer4;
	g_Objects[O_PLAYER_4].hit_points = 1;

	g_Objects[O_BLOOD1].control = ControlBlood1; // obj# 158

	g_Objects[O_BUBBLES1].control = ControlBubble1;	// obj# 155

	//взрыв когда мумия разлетается на части O_BODY_PART
	g_Objects[O_EXPLOSION1].control = ControlExplosion1;	// obj# 151

	g_Objects[O_RICOCHET1].control = ControlRicochet1; // obj# 164

	g_Objects[O_TWINKLE].control = ControlTwinkle;	// obj# 165

	g_Objects[O_SPLASH1].control = ControlSplash1;	// obj# 153

	g_Objects[O_WATERFALL].control = ControlWaterFall;	// obj# 170
	g_Objects[O_WATERFALL].draw_routine = DrawDummyItem;

	//когда взрыв то мумия разлетается на части O_EXPLOSION1
	g_Objects[O_BODY_PART].control = ControlBodyPart;	// obj# 168
	g_Objects[O_BODY_PART].nmeshes = 0;
	g_Objects[O_BODY_PART].loaded = 1;

	g_Objects[O_MISSILE1].control = ControlNatlaGun;	// obj# 171

	g_Objects[O_MISSILE2].control = ControlMissile;	// obj# 172

	g_Objects[O_MISSILE3].control = ControlMissile;	// obj# 173

	g_Objects[O_GUN_FLASH].control = ControlGunShot;	// obj# 166
}

bool LoadSprites(FILE *fp)
{
	int32_t SpriteInfoCount;

	fread(&SpriteInfoCount, sizeof(int32_t), 1, fp);

	fread(&g_PhdSpriteInfo, sizeof(PHD_SPRITE), SpriteInfoCount, fp);

	int32_t SpriteCount;

	fread(&SpriteCount, sizeof(int32_t), 1, fp);

	for (int i = 0; i < SpriteCount; i++)
	{
		int32_t object_num;

		fread(&object_num, sizeof(int32_t), 1, fp);

		if (object_num < O_NUMBER_OF)
		{
			fread(&g_Objects[object_num], sizeof(int16_t), 1, fp);
			fread(&g_Objects[object_num].mesh_index, sizeof(int16_t), 1, fp);

			g_Objects[object_num].loaded = 1;
		}
		else
		{
			int32_t static_num = object_num - O_NUMBER_OF;

			fseek(fp, 2, SEEK_CUR);
			fread(&g_StaticObjects[static_num].mesh_number, sizeof(int16_t), 1,
				  fp);
		}
	}

	return true;
}

bool LoadCameras(FILE *fp)
{
	fread(&g_NumberCameras, sizeof(int32_t), 1, fp);

	g_Camera.fixed = (OBJECT_VECTOR *)Game_Alloc(
		sizeof(OBJECT_VECTOR) * g_NumberCameras, GBUF_CAMERAS);

	if (!g_Camera.fixed)
	{
		return false;
	}

	fread(g_Camera.fixed, sizeof(OBJECT_VECTOR), g_NumberCameras, fp);

	return true;
}

bool LoadSoundEffects(FILE *fp)
{
	fread(&g_NumberSoundEffects, sizeof(int32_t), 1, fp);

	if (!g_NumberSoundEffects)
	{
		return true;
	}

	g_SoundEffectsTable = (OBJECT_VECTOR *)Game_Alloc(
		sizeof(OBJECT_VECTOR) * g_NumberSoundEffects, GBUF_SOUND_FX);

	// g_SoundEffectsTable = (OBJECT_VECTOR*)malloc(sizeof(OBJECT_VECTOR) *
	// g_NumberSoundEffects);

	if (!g_SoundEffectsTable)
	{
		return false;
	}

	fread(g_SoundEffectsTable, sizeof(OBJECT_VECTOR), g_NumberSoundEffects, fp);

	return true;
}

bool LoadBoxes(FILE *fp)
{
	fread(&g_NumberBoxes, sizeof(int32_t), 1, fp);

	g_Boxes =
		(BOX_INFO *)Game_Alloc(sizeof(BOX_INFO) * g_NumberBoxes, GBUF_BOXES);

	if (!fread(g_Boxes, sizeof(BOX_INFO), g_NumberBoxes, fp))
	{
		MessageBox(NULL, "LoadBoxes(): Unable to load boxes", "Tomb Raider",
				   MB_OK);
		return false;
	}

	int32_t OverlapCount;

	fread(&OverlapCount, sizeof(int32_t), 1, fp);

	g_Overlap = (uint16_t *)Game_Alloc(sizeof(uint16_t) * OverlapCount,
									   (GAME_BUFFER)22);

	if (!fread(g_Overlap, sizeof(uint16_t), OverlapCount, fp))
	{
		MessageBox(NULL, "LoadBoxes(): Unable to load box overlaps",
				   "Tomb Raider", MB_OK);
		return false;
	}

	for (int i = 0; i < 2; i++)
	{
		g_GroundZone[i] = (int16_t *)Game_Alloc(sizeof(int16_t) * g_NumberBoxes,
												GBUF_GROUNDZONE);
		if (!g_GroundZone[i] ||
			!fread(g_GroundZone[i], sizeof(int16_t), g_NumberBoxes, fp))
		{
			MessageBox(NULL, "LoadBoxes(): Unable to load 'ground_zone'",
					   "Tomb Raider", MB_OK);
			return false;
		}

		g_GroundZone2[i] = (int16_t *)Game_Alloc(
			sizeof(int16_t) * g_NumberBoxes, GBUF_GROUNDZONE);
		if (!g_GroundZone2[i] ||
			!fread(g_GroundZone2[i], sizeof(int16_t), g_NumberBoxes, fp))
		{
			MessageBox(NULL, "LoadBoxes(): Unable to load 'ground2_zone'",
					   "Tomb Raider", MB_OK);
			return false;
		}

		g_FlyZone[i] = (int16_t *)Game_Alloc(sizeof(int16_t) * g_NumberBoxes,
											 GBUF_FLYZONE);
		if (!g_FlyZone[i] ||
			!fread(g_FlyZone[i], sizeof(int16_t), g_NumberBoxes, fp))
		{
			MessageBox(NULL, "LoadBoxes(): Unable to load 'fly_zone'",
					   "Tomb Raider", MB_OK);
			return false;
		}
	}

	return true;
}

bool LoadAnimatedTextures(FILE *fp)
{
	int32_t AnimTextureRangeCount;

	fread(&AnimTextureRangeCount, sizeof(int32_t), 1, fp);

	g_AnimTextureRanges = (int16_t *)Game_Alloc(
		sizeof(int16_t) * AnimTextureRangeCount, GBUF_ANIMATING_TEXTURE_RANGES);

	fread(g_AnimTextureRanges, sizeof(int16_t), AnimTextureRangeCount, fp);

	return true;
}

bool LoadItems(FILE *fp)
{
	int32_t ItemCount = 0;

	fread(&ItemCount, sizeof(int32_t), 1, fp);

	if (ItemCount)
	{
		if (ItemCount > MAX_ITEMS)
		{
			MessageBox(NULL, "LoadItems(): Too Many g_Items being Loaded!",
					   "Tomb Raider", MB_OK);
			return false;
		}

		g_Items =
			(ITEM_INFO *)Game_Alloc(sizeof(ITEM_INFO) * MAX_ITEMS, GBUF_ITEMS);

		g_LevelItemCount = ItemCount;

		InitialiseItemArray(MAX_ITEMS);

		for (int i = 0; i < ItemCount; i++)
		{
			fread(&g_Items[i].object_number, sizeof(int16_t), 1, fp);
			fread(&g_Items[i].room_number, sizeof(int16_t), 1, fp);
			fread(&g_Items[i].pos.x, sizeof(int32_t), 1, fp);
			fread(&g_Items[i].pos.y, sizeof(int32_t), 1, fp);
			fread(&g_Items[i].pos.z, sizeof(int32_t), 1, fp);
			fread(&g_Items[i].pos.y_rot, sizeof(int16_t), 1, fp);
			fread(&g_Items[i].shade, sizeof(int16_t), 1, fp);
			fread(&g_Items[i].flags, sizeof(uint16_t), 1, fp);

			if (g_Items[i].object_number < 0 ||
				g_Items[i].object_number >= O_NUMBER_OF)
			{
				MessageBox(NULL, "LoadItems(): Bad Object number on Item",
						   "Tomb Raider", MB_OK);
				//"Level_LoadItems(): Bad Object number (%d) on Item %d",
				// item->object_number, i);
			}

			InitialiseItem(i);
		}
	}

	return true;
}

bool LoadDepthQ(FILE *fp)
{
	//пропуск загрузки 32 * 256 = depthQ size
	// fseek(fp, sizeof(uint8_t) * 32 * 256, SEEK_CUR);

	fread(depthq_table, 32 * 256, 1, fp);

	// Force colour 0 to black
	for (int i = 0; i < 32; i++)
	{
		depthq_table[i][0] = 0;
	}

	for (int i = 0; i < 32; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			gouraud_table[j][i] = depthq_table[i][j];
		}
	}

	return true;
}

bool LoadPalette(FILE *fp)
{
	RGB888 palette[256];

	fread(palette, sizeof(RGB888), 256, fp);

	palette[0].r = 0;
	palette[0].g = 0;
	palette[0].b = 0;

	for (int i = 0; i < 256; i++)
	{
		GamePalette[i].r = palette[i].r;
		GamePalette[i].g = palette[i].g;
		GamePalette[i].b = palette[i].b;

		GameNormalPalette[i].r = palette[i].r * 4;
		GameNormalPalette[i].g = palette[i].g * 4;
		GameNormalPalette[i].b = palette[i].b * 4;
	}

	for (int i = 0; i < 256; i++)
	{
		GameWaterPalette[i].r = (palette[i].r * 4) * 2 / 3;
		GameWaterPalette[i].g = (palette[i].g * 4) * 2 / 3;
		GameWaterPalette[i].b = palette[i].b * 4;
	}

	return true;
}

bool LoadCinematic(FILE *fp)
{
	fread(&g_NumCineFrames, sizeof(int16_t), 1, fp);

	if (!g_NumCineFrames)
	{
		return true;
	}

	g_Cine = (int16_t *)Game_Alloc(sizeof(int16_t) * 8 * g_NumCineFrames,
								   GBUF_CINEMATIC_FRAMES);

	fread(g_Cine, sizeof(int16_t) * 8, g_NumCineFrames, fp);

	return true;
}

bool LoadDemo(FILE *fp)
{
	uint16_t size = 0;

	fread(&size, sizeof(int16_t), 1, fp);

	if (!size)
	{
		return true;
	}

	g_DemoData = (uint32_t *)Game_Alloc(sizeof(uint32_t) * DEMO_COUNT_MAX,
										GBUF_LOADDEMO_BUFFER);

	fread(g_DemoData, 1, size, fp);

	return true;
}

bool LoadSoundSamples(FILE *fp)
{
	fread(&g_SampleLUT[0], sizeof(int16_t) * 256, 1, fp);
	// fread(&g_SampleLUT[0],sizeof(int16_t)*MAX_SOUND_SAMPLES,1,fp);

	fread(&NumSampleInfos, sizeof(int32_t), 1, fp);

	if (NumSampleInfos == 0)
	{
		MessageBox(NULL, "No sample infos", "Tomb Radier", MB_OK);
		return false;
	}

	g_SampleInfos = (SOUND_SAMPLE_INFO *)Game_Alloc(
		sizeof(SOUND_SAMPLE_INFO) * NumSampleInfos, GBUF_SAMPLE_INFOS);

	fread(g_SampleInfos, sizeof(SOUND_SAMPLE_INFO), NumSampleInfos, fp);

	int32_t SampleDataSize;
	fread(&SampleDataSize, sizeof(int32_t), 1, fp);

	if (!SampleDataSize)
	{
		MessageBox(NULL, "No Sample Data", "Tomb Raider", MB_OK);
		return false;
	}

	char *sample_data = (char *)Game_Alloc(SampleDataSize, GBUF_SAMPLES);
	fread(sample_data, sizeof(char), SampleDataSize, fp);

	int32_t NumSamples;

	fread(&NumSamples, sizeof(int32_t), 1, fp);

	if (!NumSamples)
	{
		MessageBox(NULL, "No Samples", "Tomb Raider", MB_OK);
		return false;
	}

	int32_t *SampleOffsets = (int32_t *)malloc(sizeof(int32_t) * NumSamples);
	size_t *SampleSizes = (size_t *)malloc(sizeof(size_t) * NumSamples);

	fread(SampleOffsets, sizeof(int32_t), NumSamples, fp);

	const char **SamplePointers =
		(const char **)malloc(sizeof(char *) * NumSamples);

	for (int i = 0; i < NumSamples; i++)
	{
		SamplePointers[i] = sample_data + SampleOffsets[i];
	}

	for (int i = 0; i < NumSamples; i++)
	{
		int current_offset = SampleOffsets[i];
		int next_offset =
			i + 1 >= NumSamples ? (int)File_Size(fp) : SampleOffsets[i + 1];
		SampleSizes[i] = next_offset - current_offset;
	}

	DirectSound_Init();

	DWORD dwHeader[11], dwWaveLength;
	WAVEFORMATEX *pWF = reinterpret_cast<WAVEFORMATEX *>(&dwHeader[5]);

	for (int i = 0; i < NumSamples; i++)
	{
		memcpy(dwHeader, SamplePointers[i], sizeof(DWORD) * 11);

		if (dwHeader[0] != MAKEFOURCC('R', 'I', 'F', 'F') ||
			dwHeader[2] != MAKEFOURCC('W', 'A', 'V', 'E') ||
			dwHeader[9] != MAKEFOURCC('d', 'a', 't', 'a'))

		{
			MessageBox(NULL, "Not WAVE format!", "Tomb Raider", MB_OK);
		}

		pWF->cbSize = sizeof(WAVEFORMATEX);
		// pWF->cbSize = 0;

		dwWaveLength = dwHeader[10];

		// SAVE SOUND TO FILE
		// id 72 = sound 108 rotate ring sound
		// id 73 = sound 109 open item sound
		/*
				if(i == 57)
				{
						FILE *fp;

						fp = fopen("sound.wav", "wb");

						fwrite(SamplePointers[i], SampleSizes[i], 1, fp);

						fclose(fp);
				}
		



				*/

		if (FAILED(DS_MakeSample(
				i, pWF,
				//пропускаем заголовок WAV файла sizeof(DWORD) * 11
				(unsigned char *)SamplePointers[i] + sizeof(DWORD) * 11,
				dwWaveLength)))
		{
			MessageBox(NULL, "DS_MakeSample Error!", "Tomb Raider", MB_OK);
		}
	}

	free(SampleOffsets);
	free(SamplePointers);
	free(SampleSizes);

	return true;
}

size_t File_Size(FILE *fp)
{
	size_t old = ftell(fp);
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, (long int)old, SEEK_SET);
	return size;
}
