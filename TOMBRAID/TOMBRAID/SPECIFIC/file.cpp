#include "file.h"
#include "draw.h"
#include "lara.h"
#include "vars.h"
#include "collide.h"
#include "screen.h"

#include "..\\objects\\wolf.h"
#include "..\\objects\\bear.h"
#include "..\\objects\\bat.h"
#include "..\\objects\\blood.h"
#include "..\\objects\\door.h"
#include "..\\objects\\switch.h"
#include "..\\objects\\bridge.h"
#include "..\\objects\\cog.h"
#include "..\\objects\\pickup.h"
#include "..\\objects\\keyhole.h"
#include "..\\objects\\trapdoor.h"
#include "..\\objects\\puzzle_hole.h"
#include "..\\objects\\dino.h"
#include "..\\objects\\raptor.h"
#include "..\\objects\\larson.h"
#include "..\\objects\\pierre.h"
#include "..\\objects\\rat.h"
#include "..\\objects\\vole.h"
#include "..\\objects\\lion.h"
#include "..\\objects\\crocodile.h"
#include "..\\objects\\alligator.h"
#include "..\\objects\\ape.h"
#include "..\\objects\\misc.h"
#include "..\\objects\\scion.h"
#include "..\\objects\\cabin.h"
#include "..\\objects\\boat.h"
#include "..\\objects\\natla.h"
#include "..\\objects\\gunshot.h"
#include "..\\objects\\mutant.h"
#include "..\\objects\\centaur.h"
#include "..\\objects\\mummy.h"
#include "..\\objects\\skate_kid.h"
#include "..\\objects\\cowboy.h"
#include "..\\objects\\baldy.h"
#include "..\\objects\\abortion.h"
#include "..\\objects\\pod.h"
#include "..\\objects\\statue.h"

#include "..\\traps\\movable_block.h"
#include "..\\traps\\pendulum.h"
#include "..\\traps\\dart_emitter.h"
#include "..\\traps\\dart.h"
#include "..\\traps\\falling_block.h"

#include "..\\traps\\teeth_trap.h"
#include "..\\traps\\rolling_ball.h"
#include "..\\traps\\spikes.h"
#include "..\\traps\\falling_ceiling.h"
#include "..\\traps\\damocles_sword.h"

#include "..\\traps\\rolling_block.h"
#include "..\\traps\\lightning_emitter.h"
#include "..\\traps\\thors_hammer.h"
#include "..\\traps\\midas_touch.h"

#include "..\\traps\\flame.h"
#include "..\\traps\\lava.h"

#include "..\\effects\\ricochet.h"
#include "..\\effects\\twinkle.h"
#include "..\\effects\\splash.h"
#include "..\\effects\\waterfall.h"
#include "..\\effects\\missile.h"
#include "..\\effects\\body_part.h"

#include "box.h"
#include "items.h"
#include "sound.h"

bool S_LoadLevel(char * szLevelName)
{
	int32_t LevelVersion;
	int32_t LevelNum;

	Init_Game_Malloc();

	FILE *fp = NULL;

	fopen_s(&fp, szLevelName, "rb");

	if(!fp)
	{
		MessageBox(NULL, "S_LoadLevel(): Could not open level file", "Tomb Raider", MB_OK);
		return false;
	}

	//читаем номер версии файла уровня
	//20h или 32d для Tomb Raider 1
	fread(&LevelVersion, sizeof(int32_t), 1, fp);

	if (LevelVersion != 32)
	{
		MessageBox(NULL, "Wrong Tomb Raider level version", "Tomb Raider", MB_OK);
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
      

	return true;
}

bool Load_Texture_Pages(FILE *fp)
{
	int32_t TexturePageCount;

	fread(&TexturePageCount, 4, 1, fp);

	int8_t *Buff = (int8_t *) Game_Alloc(TexturePageCount * 256 * 256, GBUF_TEXTURE_PAGES);

	fread(Buff, 256 * 256, TexturePageCount, fp);

	for (int i = 0; i < TexturePageCount; i++)
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

	if(g_RoomCount > 1024)
	{
		MessageBox(NULL, "Load_Rooms(): Too many rooms", "Tomb Raider", MB_OK);
		return false;
	}

	g_RoomInfo = (ROOM_INFO *)Game_Alloc(sizeof(ROOM_INFO) * g_RoomCount, GBUF_ROOM_INFOS);
    
	for ( int i = 0; i < g_RoomCount; i++ )
	{
		//читаем позицию комнаты на сцене
		fread(&g_RoomInfo[i].x, sizeof(uint32_t), 1, fp);
        g_RoomInfo[i].y = 0;
        fread(&g_RoomInfo[i].z, sizeof(uint32_t), 1, fp);

        fread(&g_RoomInfo[i].min_floor, sizeof(uint32_t), 1, fp);
        fread(&g_RoomInfo[i].max_ceiling, sizeof(uint32_t), 1, fp);

		//читаем мешь комнаты
        fread(&Size, sizeof(uint32_t), 1, fp);
        
		g_RoomInfo[i].data = (int16_t *)Game_Alloc(sizeof(uint16_t) * Size, GBUF_ROOM_MESH);
        
		fread(g_RoomInfo[i].data, sizeof(uint16_t), Size, fp);
		
		//читаем двери
		fread(&NumDoors, sizeof(uint16_t), 1, fp);

        if (NumDoors)
		{
			//2 байта размер сколько дверей + сам массив дверей sizeof(DOOR_INFO) * NumDoors
			//g_RoomInfo[i].doors = (DOOR_INFOS *)Game_Alloc(sizeof(uint16_t) + sizeof(DOOR_INFO *) * NumDoors, GBUF_ROOM_DOOR);
			//выделяем место под двери - число дверей + указатель на массив дверей
			g_RoomInfo[i].doors = (DOOR_INFOS *)Game_Alloc(sizeof(uint16_t) + sizeof(DOOR_INFO *), GBUF_ROOM_DOOR);
			
			g_RoomInfo[i].doors->count = NumDoors;

			//выделяем место под все двери
			g_RoomInfo[i].doors->door = (DOOR_INFO *)Game_Alloc(sizeof(DOOR_INFO) * NumDoors, GBUF_ROOM_DOOR);

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
        
		g_RoomInfo[i].floor = (FLOOR_INFO *)Game_Alloc(sizeof(FLOOR_INFO) * Size, GBUF_ROOM_FLOOR);
        
		fread(g_RoomInfo[i].floor, sizeof(FLOOR_INFO), Size, fp);

		//освещение комнаты
        fread(&g_RoomInfo[i].ambient, sizeof(uint16_t), 1, fp);
        fread(&g_RoomInfo[i].num_lights, sizeof(uint16_t), 1, fp);

		if (g_RoomInfo[i].num_lights)
		{
            g_RoomInfo[i].light = (LIGHT_INFO *)Game_Alloc(sizeof(LIGHT_INFO) * g_RoomInfo[i].num_lights, GBUF_ROOM_LIGHTS);
            
			fread(g_RoomInfo[i].light, sizeof(LIGHT_INFO), g_RoomInfo[i].num_lights, fp);
        }
		else
		{
            g_RoomInfo[i].light = NULL;
        }
		
		//читаем информацию о статических мешах
        fread(&g_RoomInfo[i].num_static_meshes, sizeof(uint16_t), 1, fp);
        
		if (g_RoomInfo[i].num_static_meshes)
		{
            g_RoomInfo[i].static_mesh = (MESH_INFO *)Game_Alloc(sizeof(MESH_INFO) * g_RoomInfo[i].num_static_meshes, GBUF_ROOM_STATIC_MESH_INFOS);
            
			fread(g_RoomInfo[i].static_mesh, sizeof(MESH_INFO), g_RoomInfo[i].num_static_meshes, fp);
        }
		else
		{
            g_RoomInfo[i].static_mesh = NULL;
        }

		//flipped (альтернативные) комнаты
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
    
	g_FloorData = (int16_t *)Game_Alloc(sizeof(uint16_t) * FloorDataSize, GBUF_FLOOR_DATA);
    
	fread(g_FloorData, sizeof(uint16_t), FloorDataSize, fp);

	return true;
}

bool Load_Objects(FILE *fp)
{
	int32_t MeshCount;
	//читаем информацию про меши объектов
	fread(&MeshCount, sizeof(int32_t), 1, fp);

    g_MeshBase = (int16_t *)Game_Alloc(sizeof(int16_t) * MeshCount, GBUF_MESHES);

	fread(g_MeshBase, sizeof(int16_t), MeshCount, fp);

	int32_t MeshPtrCount;
	fread(&MeshPtrCount, sizeof(int32_t), 1, fp);
    //читаем информацию про индексы мешей в массиве
	uint32_t *MeshIndices = (uint32_t *)Game_Alloc(sizeof(uint32_t) * MeshPtrCount, GBUF_MESH_POINTERS);
    
	fread(MeshIndices, sizeof(uint32_t), MeshPtrCount, fp);

	g_Meshes = (int16_t **)Game_Alloc(sizeof(int16_t *) * MeshPtrCount, GBUF_MESH_POINTERS);
    
	for (int i = 0; i < MeshPtrCount; i++)
	{
		//создаем массив мешей объектов
        g_Meshes[i] = &g_MeshBase[MeshIndices[i] / 2];
    }

	int32_t m_AnimCount;
	//читаем данные про анимацию
	fread(&m_AnimCount, sizeof(int32_t), 1, fp);
    
    g_Anims = (ANIM_STRUCT *)Game_Alloc(sizeof(ANIM_STRUCT) * m_AnimCount, GBUF_ANIMS);
    
	fread(g_Anims, sizeof(ANIM_STRUCT), m_AnimCount, fp);

	int32_t AnimChangeCount;
	fread(&AnimChangeCount, sizeof(int32_t), 1, fp);
    
    g_AnimChanges = (ANIM_CHANGE_STRUCT *)Game_Alloc(sizeof(ANIM_CHANGE_STRUCT) * AnimChangeCount, GBUF_ANIM_CHANGES);
    fread(g_AnimChanges, sizeof(ANIM_CHANGE_STRUCT), AnimChangeCount, fp);

	int32_t AnimRangeCount;
	fread(&AnimRangeCount, sizeof(int32_t), 1, fp);
    
    g_AnimRanges = (ANIM_RANGE_STRUCT *)Game_Alloc(sizeof(ANIM_RANGE_STRUCT) * AnimRangeCount, GBUF_ANIM_RANGES);
    fread(g_AnimRanges, sizeof(ANIM_RANGE_STRUCT), AnimRangeCount, fp);

	int32_t AnimCommandCount;
	//читаем данные про команды анимации
    fread(&AnimCommandCount, sizeof(int32_t), 1, fp);
    
    g_AnimCommands = (int16_t *)Game_Alloc(sizeof(int16_t) * AnimCommandCount, GBUF_ANIM_COMMANDS);
    
	fread(g_AnimCommands, sizeof(int16_t), AnimCommandCount, fp);

	int32_t AnimBoneCount;
	//читаем данные про кости (позиции)
    fread(&AnimBoneCount, sizeof(int32_t), 1, fp);
    
    g_AnimBones = (int32_t *)Game_Alloc(sizeof(int32_t) * AnimBoneCount, GBUF_ANIM_BONES);
    
	fread(g_AnimBones, sizeof(int32_t), AnimBoneCount, fp);

	int32_t AnimFrameCount;
	//читаем данные про кости (повороты)
	fread(&AnimFrameCount, sizeof(int32_t), 1, fp);
    
    g_AnimFrames = (int16_t *)Game_Alloc(sizeof(int16_t) * AnimFrameCount, GBUF_ANIM_FRAMES);
    
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

	if(g_Objects[O_WOLF].loaded)
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

	if(g_Objects[O_BEAR].loaded)
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

	if(g_Objects[O_BAT].loaded)
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

	SetupDino(&g_Objects[O_DINOSAUR]);
    SetupRaptor(&g_Objects[O_RAPTOR]);
    SetupLarson(&g_Objects[O_LARSON]);
    SetupPierre(&g_Objects[O_PIERRE]);
    SetupRat(&g_Objects[O_RAT]);
    SetupVole(&g_Objects[O_VOLE]);
    SetupLion(&g_Objects[O_LION]);
    SetupLioness(&g_Objects[O_LIONESS]);
    SetupPuma(&g_Objects[O_PUMA]);
    SetupCrocodile(&g_Objects[O_CROCODILE]);
    SetupAlligator(&g_Objects[O_ALLIGATOR]);
    SetupApe(&g_Objects[O_APE]);
    SetupWarrior(&g_Objects[O_WARRIOR1]);
    SetupWarrior2(&g_Objects[O_WARRIOR2]);
    SetupWarrior3(&g_Objects[O_WARRIOR3]);
    SetupCentaur(&g_Objects[O_CENTAUR]);
    SetupMummy(&g_Objects[O_MUMMY]);
    SetupSkateKid(&g_Objects[O_MERCENARY1]);
    SetupCowboy(&g_Objects[O_MERCENARY2]);
    SetupBaldy(&g_Objects[O_MERCENARY3]);
    SetupAbortion(&g_Objects[O_ABORTION]);
    SetupNatla(&g_Objects[O_NATLA]);
    SetupPod(&g_Objects[O_PODS]);
    SetupBigPod(&g_Objects[O_BIG_POD]);
    SetupStatue(&g_Objects[O_STATUE]);
}

void TrapObjects()
{
	g_Objects[O_FALLING_BLOCK].control = FallingBlockControl;	//obj# 35
    g_Objects[O_FALLING_BLOCK].floor = FallingBlockFloor;
    g_Objects[O_FALLING_BLOCK].ceiling = FallingBlockCeiling;
    g_Objects[O_FALLING_BLOCK].save_position = 1;
    g_Objects[O_FALLING_BLOCK].save_anim = 1;
    g_Objects[O_FALLING_BLOCK].save_flags = 1;

	g_Objects[O_PENDULUM].control = PendulumControl; //obj# 36
    g_Objects[O_PENDULUM].collision = TrapCollision;
    g_Objects[O_PENDULUM].shadow_size = UNIT_SHADOW / 2;
    g_Objects[O_PENDULUM].save_flags = 1;
    g_Objects[O_PENDULUM].save_anim = 1;

	//-----------------------

	SetupTeethTrap(&g_Objects[O_TEETH_TRAP]); //obj# 42
    SetupRollingBall(&g_Objects[O_ROLLING_BALL]); //obj# 38
    SetupSpikes(&g_Objects[O_SPIKES]); //obj# 37
    SetupFallingCeilling(&g_Objects[O_FALLING_CEILING1]); //obj# 53
    SetupFallingCeilling(&g_Objects[O_FALLING_CEILING2]); //obj# 54
    SetupDamoclesSword(&g_Objects[O_DAMOCLES_SWORD]); //obj# 43

	//--------------------------

	g_Objects[O_MOVABLE_BLOCK].initialise = InitialiseMovableBlock;  //obj# 48
    g_Objects[O_MOVABLE_BLOCK].control = MovableBlockControl;
    g_Objects[O_MOVABLE_BLOCK].draw_routine = DrawMovableBlock;
    g_Objects[O_MOVABLE_BLOCK].collision = MovableBlockCollision;
    g_Objects[O_MOVABLE_BLOCK].save_position = 1;
    g_Objects[O_MOVABLE_BLOCK].save_anim = 1;
    g_Objects[O_MOVABLE_BLOCK].save_flags = 1;

	g_Objects[O_MOVABLE_BLOCK2].initialise = InitialiseMovableBlock;  //obj# 49
    g_Objects[O_MOVABLE_BLOCK2].control = MovableBlockControl;
    g_Objects[O_MOVABLE_BLOCK2].draw_routine = DrawMovableBlock;
    g_Objects[O_MOVABLE_BLOCK2].collision = MovableBlockCollision;
    g_Objects[O_MOVABLE_BLOCK2].save_position = 1;
    g_Objects[O_MOVABLE_BLOCK2].save_anim = 1;
    g_Objects[O_MOVABLE_BLOCK2].save_flags = 1;


	g_Objects[O_MOVABLE_BLOCK3].initialise = InitialiseMovableBlock;  //obj# 50
    g_Objects[O_MOVABLE_BLOCK3].control = MovableBlockControl;
    g_Objects[O_MOVABLE_BLOCK3].draw_routine = DrawMovableBlock;
    g_Objects[O_MOVABLE_BLOCK3].collision = MovableBlockCollision;
    g_Objects[O_MOVABLE_BLOCK3].save_position = 1;
    g_Objects[O_MOVABLE_BLOCK3].save_anim = 1;
    g_Objects[O_MOVABLE_BLOCK3].save_flags = 1;

	g_Objects[O_MOVABLE_BLOCK4].initialise = InitialiseMovableBlock;  //obj# 41
    g_Objects[O_MOVABLE_BLOCK4].control = MovableBlockControl;
    g_Objects[O_MOVABLE_BLOCK4].draw_routine = DrawMovableBlock;
    g_Objects[O_MOVABLE_BLOCK4].collision = MovableBlockCollision;
    g_Objects[O_MOVABLE_BLOCK4].save_position = 1;
    g_Objects[O_MOVABLE_BLOCK4].save_anim = 1;
    g_Objects[O_MOVABLE_BLOCK4].save_flags = 1;
	
	g_Objects[O_ROLLING_BLOCK].initialise = InitialiseRollingBlock;  //obj# 52
    g_Objects[O_ROLLING_BLOCK].control = RollingBlockControl;
    g_Objects[O_ROLLING_BLOCK].save_position = 1;
    g_Objects[O_ROLLING_BLOCK].save_anim = 1;
    g_Objects[O_ROLLING_BLOCK].save_flags = 1;

    SetupLightningEmitter(&g_Objects[O_LIGHTNING_EMITTER]); //obj# 46
    SetupThorsHandle(&g_Objects[O_THORS_HANDLE]); //obj# 44
    SetupThorsHead(&g_Objects[O_THORS_HEAD]); //obj# 45
    SetupMidasTouch(&g_Objects[O_MIDAS_TOUCH]); //obj# 128

	g_Objects[O_DART_EMITTER].control = DartEmitterControl;  //obj# 40

	g_Objects[O_DARTS].collision = ObjectCollision;	//obj# 39
    g_Objects[O_DARTS].control = DartsControl;
    g_Objects[O_DARTS].shadow_size = UNIT_SHADOW / 2;
    g_Objects[O_DARTS].save_flags = 1;

	g_Objects[O_DART_EFFECT].control = DartEffectControl;	//obj# 160
    g_Objects[O_DART_EFFECT].draw_routine = DrawSpriteItem;

    SetupFlameEmitter(&g_Objects[O_FLAME_EMITTER]); //obj# 179
    SetupFlame(&g_Objects[O_FLAME]); //obj# 178
    SetupLavaEmitter(&g_Objects[O_LAVA_EMITTER]); //obj# 177
    SetupLava(&g_Objects[O_LAVA]); //obj# 176
    SetupLavaWedge(&g_Objects[O_LAVA_WEDGE]); //obj# 180

	
}

void ObjectObjects()
{
	g_Objects[O_CAMERA_TARGET].draw_routine = DrawDummyItem; //obj# 169
    
	g_Objects[O_BRIDGE_FLAT].floor = BridgeFlatFloor;	 //obj# 68
    g_Objects[O_BRIDGE_FLAT].ceiling = BridgeFlatCeiling;
	
	g_Objects[O_BRIDGE_TILT1].floor = BridgeTilt1Floor;	 //obj# 69
    g_Objects[O_BRIDGE_TILT1].ceiling = BridgeTilt1Ceiling;
	
	g_Objects[O_BRIDGE_TILT2].floor = BridgeTilt2Floor;	 //obj# 70
    g_Objects[O_BRIDGE_TILT2].ceiling = BridgeTilt2Ceiling;
	
	if(g_Objects[O_DRAW_BRIDGE].loaded)
	{
		g_Objects[O_DRAW_BRIDGE].ceiling = DrawBridgeCeiling;	//obj# 41
		g_Objects[O_DRAW_BRIDGE].collision = DrawBridgeCollision;
		g_Objects[O_DRAW_BRIDGE].control = CogControl;
		g_Objects[O_DRAW_BRIDGE].save_anim = 1;
		g_Objects[O_DRAW_BRIDGE].save_flags = 1;
		g_Objects[O_DRAW_BRIDGE].floor = DrawBridgeFloor;
	}
    
	g_Objects[O_SWITCH_TYPE1].control = SwitchControl;	 //obj# 55
    g_Objects[O_SWITCH_TYPE1].collision = SwitchCollision;
    g_Objects[O_SWITCH_TYPE1].save_anim = 1;
    g_Objects[O_SWITCH_TYPE1].save_flags = 1;
	
	g_Objects[O_SWITCH_TYPE2].control = SwitchControl;	//obj# 56
    g_Objects[O_SWITCH_TYPE2].collision = SwitchCollision2;
    g_Objects[O_SWITCH_TYPE2].save_anim = 1;
    g_Objects[O_SWITCH_TYPE2].save_flags = 1;
	
	g_Objects[O_DOOR_TYPE1].initialise = InitialiseDoor;	//obj# 57
    g_Objects[O_DOOR_TYPE1].control = DoorControl;
    g_Objects[O_DOOR_TYPE1].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE1].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE1].save_anim = 1;
    g_Objects[O_DOOR_TYPE1].save_flags = 1;

	g_Objects[O_DOOR_TYPE2].initialise = InitialiseDoor;	//obj# 58
    g_Objects[O_DOOR_TYPE2].control = DoorControl;
    g_Objects[O_DOOR_TYPE2].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE2].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE2].save_anim = 1;
    g_Objects[O_DOOR_TYPE2].save_flags = 1;

	g_Objects[O_DOOR_TYPE3].initialise = InitialiseDoor;	//obj# 59
    g_Objects[O_DOOR_TYPE3].control = DoorControl;
    g_Objects[O_DOOR_TYPE3].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE3].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE3].save_anim = 1;
    g_Objects[O_DOOR_TYPE3].save_flags = 1;

    g_Objects[O_DOOR_TYPE4].initialise = InitialiseDoor;	//obj# 60
    g_Objects[O_DOOR_TYPE4].control = DoorControl;
    g_Objects[O_DOOR_TYPE4].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE4].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE4].save_anim = 1;
    g_Objects[O_DOOR_TYPE4].save_flags = 1;

	//решетка под водой Мидас начало уровня
	g_Objects[O_DOOR_TYPE5].initialise = InitialiseDoor;	//obj# 61
    g_Objects[O_DOOR_TYPE5].control = DoorControl;
    g_Objects[O_DOOR_TYPE5].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE5].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE5].save_anim = 1;
    g_Objects[O_DOOR_TYPE5].save_flags = 1;

	g_Objects[O_DOOR_TYPE6].initialise = InitialiseDoor;	//obj# 62
    g_Objects[O_DOOR_TYPE6].control = DoorControl;
    g_Objects[O_DOOR_TYPE6].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE6].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE6].save_anim = 1;
    g_Objects[O_DOOR_TYPE6].save_flags = 1;

	g_Objects[O_DOOR_TYPE7].initialise = InitialiseDoor;	//obj# 63
    g_Objects[O_DOOR_TYPE7].control = DoorControl;
    g_Objects[O_DOOR_TYPE7].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE7].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE7].save_anim = 1;
    g_Objects[O_DOOR_TYPE7].save_flags = 1;
	
	g_Objects[O_DOOR_TYPE8].initialise = InitialiseDoor;	//obj# 64
    g_Objects[O_DOOR_TYPE8].control = DoorControl;
    g_Objects[O_DOOR_TYPE8].draw_routine = DrawUnclippedItem;
    g_Objects[O_DOOR_TYPE8].collision = DoorCollision;
    g_Objects[O_DOOR_TYPE8].save_anim = 1;
    g_Objects[O_DOOR_TYPE8].save_flags = 1;
	
	g_Objects[O_TRAPDOOR].control = TrapDoorControl; //obj# 65
    g_Objects[O_TRAPDOOR].floor = TrapDoorFloor;
    g_Objects[O_TRAPDOOR].ceiling = TrapDoorCeiling;
    g_Objects[O_TRAPDOOR].save_anim = 1;
    g_Objects[O_TRAPDOOR].save_flags = 1;

	g_Objects[O_TRAPDOOR2].control = TrapDoorControl; //obj# 66
    g_Objects[O_TRAPDOOR2].floor = TrapDoorFloor;
    g_Objects[O_TRAPDOOR2].ceiling = TrapDoorCeiling;
    g_Objects[O_TRAPDOOR2].save_anim = 1;
    g_Objects[O_TRAPDOOR2].save_flags = 1;

    SetupCog(&g_Objects[O_COG_1]); //obj# 74
    SetupCog(&g_Objects[O_COG_2]); //obj# 75
    SetupCog(&g_Objects[O_COG_3]); //obj# 76
    SetupMovingBar(&g_Objects[O_MOVING_BAR]); //obj# 47
	
	g_Objects[O_PICKUP_ITEM1].draw_routine = DrawPickupItem;  //obj# 141
    g_Objects[O_PICKUP_ITEM1].collision = PickUpCollision;
    g_Objects[O_PICKUP_ITEM1].save_flags = 1;

	g_Objects[O_PICKUP_ITEM2].draw_routine = DrawPickupItem;  //obj# 142
    g_Objects[O_PICKUP_ITEM2].collision = PickUpCollision;
    g_Objects[O_PICKUP_ITEM2].save_flags = 1;

	g_Objects[O_KEY_ITEM1].draw_routine = DrawPickupItem;  //obj# 129
    g_Objects[O_KEY_ITEM1].collision = PickUpCollision;
    g_Objects[O_KEY_ITEM1].save_flags = 1;

	g_Objects[O_KEY_ITEM2].draw_routine = DrawPickupItem;  //obj# 130
    g_Objects[O_KEY_ITEM2].collision = PickUpCollision;
    g_Objects[O_KEY_ITEM2].save_flags = 1;

	g_Objects[O_KEY_ITEM3].draw_routine = DrawPickupItem;  //obj# 131
    g_Objects[O_KEY_ITEM3].collision = PickUpCollision;
    g_Objects[O_KEY_ITEM3].save_flags = 1;

	g_Objects[O_KEY_ITEM4].draw_routine = DrawPickupItem;  //obj# 132
    g_Objects[O_KEY_ITEM4].collision = PickUpCollision;
    g_Objects[O_KEY_ITEM4].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM1].draw_routine = DrawPickupItem;  //obj# 110
    g_Objects[O_PUZZLE_ITEM1].collision = PickUpCollision;
    g_Objects[O_PUZZLE_ITEM1].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM2].draw_routine = DrawPickupItem;  //obj# 111
    g_Objects[O_PUZZLE_ITEM2].collision = PickUpCollision;
    g_Objects[O_PUZZLE_ITEM2].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM3].draw_routine = DrawPickupItem;  //obj# 112
    g_Objects[O_PUZZLE_ITEM3].collision = PickUpCollision;
    g_Objects[O_PUZZLE_ITEM3].save_flags = 1;

	g_Objects[O_PUZZLE_ITEM4].draw_routine = DrawPickupItem;  //obj# 113
    g_Objects[O_PUZZLE_ITEM4].collision = PickUpCollision;
    g_Objects[O_PUZZLE_ITEM4].save_flags = 1;

	g_Objects[O_GUN_ITEM].draw_routine = DrawPickupItem;  //obj# 84
    g_Objects[O_GUN_ITEM].collision = PickUpCollision;
    g_Objects[O_GUN_ITEM].save_flags = 1;

	g_Objects[O_SHOTGUN_ITEM].draw_routine = DrawPickupItem;  //obj# 85
    g_Objects[O_SHOTGUN_ITEM].collision = PickUpCollision;
    g_Objects[O_SHOTGUN_ITEM].save_flags = 1;

	g_Objects[O_MAGNUM_ITEM].draw_routine = DrawPickupItem;  //obj# 86
    g_Objects[O_MAGNUM_ITEM].collision = PickUpCollision;
    g_Objects[O_MAGNUM_ITEM].save_flags = 1;

	g_Objects[O_UZI_ITEM].draw_routine = DrawPickupItem;  //obj# 87
    g_Objects[O_UZI_ITEM].collision = PickUpCollision;
    g_Objects[O_UZI_ITEM].save_flags = 1;

	g_Objects[O_GUN_AMMO_ITEM].draw_routine = DrawPickupItem;  //obj# 88
    g_Objects[O_GUN_AMMO_ITEM].collision = PickUpCollision;
    g_Objects[O_GUN_AMMO_ITEM].save_flags = 1;

	g_Objects[O_SG_AMMO_ITEM].draw_routine = DrawPickupItem;  //obj# 89
    g_Objects[O_SG_AMMO_ITEM].collision = PickUpCollision;
    g_Objects[O_SG_AMMO_ITEM].save_flags = 1;

	g_Objects[O_MAG_AMMO_ITEM].draw_routine = DrawPickupItem;  //obj# 90
    g_Objects[O_MAG_AMMO_ITEM].collision = PickUpCollision;
    g_Objects[O_MAG_AMMO_ITEM].save_flags = 1;

	g_Objects[O_UZI_AMMO_ITEM].draw_routine = DrawPickupItem;  //obj# 91
    g_Objects[O_UZI_AMMO_ITEM].collision = PickUpCollision;
    g_Objects[O_UZI_AMMO_ITEM].save_flags = 1;

	g_Objects[O_EXPLOSIVE_ITEM].draw_routine = DrawPickupItem;  //obj# 92
    g_Objects[O_EXPLOSIVE_ITEM].collision = PickUpCollision;
    g_Objects[O_EXPLOSIVE_ITEM].save_flags = 1;

	g_Objects[O_MEDI_ITEM].draw_routine = DrawPickupItem; //obj# 93
    g_Objects[O_MEDI_ITEM].collision = PickUpCollision;
    g_Objects[O_MEDI_ITEM].save_flags = 1;
	
	g_Objects[O_BIGMEDI_ITEM].draw_routine = DrawPickupItem; //obj# 94
    g_Objects[O_BIGMEDI_ITEM].collision = PickUpCollision;
    g_Objects[O_BIGMEDI_ITEM].save_flags = 1;
    
	SetupScion1(&g_Objects[O_SCION_ITEM]); //obj# 143
    SetupScion2(&g_Objects[O_SCION_ITEM2]); //obj# 144
    SetupScion3(&g_Objects[O_SCION_ITEM3]); //obj# 145
    SetupScion4(&g_Objects[O_SCION_ITEM4]); //obj# 146
    SetupScionHolder(&g_Objects[O_SCION_HOLDER]); //obj# 147

	SetupLeadBar(&g_Objects[O_LEADBAR_ITEM]); //obj# 126
	
    //закоментировать строку ниже если нужно рисовать O_SAVEGAME_ITEM
	g_Objects[O_SAVEGAME_ITEM].draw_routine = DrawDummyItem;  //obj# 83

	g_Objects[O_KEY_HOLE1].collision = KeyHoleCollision; //obj# 137
    g_Objects[O_KEY_HOLE1].save_flags = 1;

	g_Objects[O_KEY_HOLE2].collision = KeyHoleCollision; //obj# 138
    g_Objects[O_KEY_HOLE2].save_flags = 1;

	g_Objects[O_KEY_HOLE3].collision = KeyHoleCollision; //obj# 139
    g_Objects[O_KEY_HOLE3].save_flags = 1;

	g_Objects[O_KEY_HOLE4].collision = KeyHoleCollision; //obj# 140
    g_Objects[O_KEY_HOLE4].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE1].collision = PuzzleHoleCollision; //obj# 118
    g_Objects[O_PUZZLE_HOLE1].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE2].collision = PuzzleHoleCollision; //obj# 119
    g_Objects[O_PUZZLE_HOLE2].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE3].collision = PuzzleHoleCollision; //obj# 120
    g_Objects[O_PUZZLE_HOLE3].save_flags = 1;

	g_Objects[O_PUZZLE_HOLE4].collision = PuzzleHoleCollision; //obj# 121
    g_Objects[O_PUZZLE_HOLE4].save_flags = 1;

	g_Objects[O_PUZZLE_DONE1].save_flags = 1; //obj# 122

	g_Objects[O_PUZZLE_DONE2].save_flags = 1; //obj# 122

	g_Objects[O_PUZZLE_DONE3].save_flags = 1; //obj# 122

	g_Objects[O_PUZZLE_DONE4].save_flags = 1; //obj# 122

	//O_PORTACABIN это комната на тросу Natla Mines уровень
    SetupCabin(&g_Objects[O_PORTACABIN]); //obj# 162
    SetupBoat(&g_Objects[O_BOAT]); //obj# 182
    SetupEarthquake(&g_Objects[O_EARTHQUAKE]); //obj# 183

	/*
	//player 1 - 77
    g_Objects[O_PLAYER_1].initialise = InitialisePlayer1;
    g_Objects[O_PLAYER_1].control = ControlCinematicPlayer;
    g_Objects[O_PLAYER_1].hit_points = 1;
	//player 2 - 78
    g_Objects[O_PLAYER_2].initialise = InitialiseGenPlayer;
    g_Objects[O_PLAYER_2].control = ControlCinematicPlayer;
    g_Objects[O_PLAYER_2].hit_points = 1;
	//player 3 - 79
    g_Objects[O_PLAYER_3].initialise = InitialiseGenPlayer;
    g_Objects[O_PLAYER_3].control = ControlCinematicPlayer;
    g_Objects[O_PLAYER_3].hit_points = 1;
	//player 4 - 80
    g_Objects[O_PLAYER_4].initialise = InitialiseGenPlayer;
    g_Objects[O_PLAYER_4].control = ControlCinematicPlayer4;
    g_Objects[O_PLAYER_4].hit_points = 1;
	*/

	g_Objects[O_BLOOD1].control = ControlBlood1; //obj# 158

    SetupBubble(&g_Objects[O_BUBBLES1]); //obj# 155

	//взрыв когда мумия разлетается на части O_BODY_PART
    SetupExplosion(&g_Objects[O_EXPLOSION1]); //obj# 151

	g_Objects[O_RICOCHET1].control = ControlRicochet1; //obj# 164

	SetupTwinkle(&g_Objects[O_TWINKLE]); //obj# 165
    SetupSplash(&g_Objects[O_SPLASH1]); //obj# 153
    SetupWaterfall(&g_Objects[O_WATERFALL]); //obj# 170
    
	//когда взрыв то мумия разлетается на части O_EXPLOSION1
	SetupBodyPart(&g_Objects[O_BODY_PART]); //obj# 168

    SetupNatlaGun(&g_Objects[O_MISSILE1]); //obj# 171
    SetupMissile(&g_Objects[O_MISSILE2]); //obj# 172
    SetupMissile(&g_Objects[O_MISSILE3]); //obj# 173
    SetupGunShot(&g_Objects[O_GUN_FLASH]); //obj# 166
}

bool LoadSprites(FILE *fp)
{
	int32_t SpriteInfoCount;

	fread(&SpriteInfoCount, sizeof(int32_t), 1, fp);

	fread(&g_PhdSpriteInfo, sizeof(PHDSPRITESTRUCT), SpriteInfoCount, fp);

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
            fread(&g_StaticObjects[static_num].mesh_number, sizeof(int16_t), 1, fp);
        }
    }

    return true;
}

bool LoadCameras(FILE *fp)
{
    fread(&g_NumberCameras, sizeof(int32_t), 1, fp);

	g_Camera.fixed = (OBJECT_VECTOR *)Game_Alloc(sizeof(OBJECT_VECTOR) * g_NumberCameras, GBUF_CAMERAS);
    
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

    g_SoundEffectsTable = (OBJECT_VECTOR *)Game_Alloc(sizeof(OBJECT_VECTOR) * g_NumberSoundEffects, GBUF_SOUND_FX);
    
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

	g_Boxes = (BOX_INFO *) Game_Alloc(sizeof(BOX_INFO) * g_NumberBoxes, GBUF_BOXES);

	if (!fread(g_Boxes, sizeof(BOX_INFO), g_NumberBoxes, fp))
	{
        MessageBox(NULL, "LoadBoxes(): Unable to load boxes", "Tomb Raider", MB_OK);
        return false;
    }

	int32_t OverlapCount;

	fread(&OverlapCount, sizeof(int32_t), 1, fp);

	g_Overlap = (uint16_t *) Game_Alloc(sizeof(uint16_t) * OverlapCount, (GAME_BUFFER)22);

	if (!fread(g_Overlap, sizeof(uint16_t), OverlapCount, fp))
	{
		MessageBox(NULL, "LoadBoxes(): Unable to load box overlaps", "Tomb Raider", MB_OK);
        return false;
    }

    for (int i = 0; i < 2; i++)
	{
        g_GroundZone[i] = (int16_t *) Game_Alloc(sizeof(int16_t) * g_NumberBoxes, GBUF_GROUNDZONE);
        if (!g_GroundZone[i] || !fread(g_GroundZone[i], sizeof(int16_t), g_NumberBoxes, fp))
			{
				MessageBox(NULL, "LoadBoxes(): Unable to load 'ground_zone'", "Tomb Raider", MB_OK);
		        return false;
			}

        g_GroundZone2[i] = (int16_t *) Game_Alloc(sizeof(int16_t) * g_NumberBoxes, GBUF_GROUNDZONE);
        if (!g_GroundZone2[i] || !fread(g_GroundZone2[i], sizeof(int16_t), g_NumberBoxes, fp))
			{
				MessageBox(NULL, "LoadBoxes(): Unable to load 'ground2_zone'", "Tomb Raider", MB_OK);
	            return false;
		    }

        g_FlyZone[i] = (int16_t *) Game_Alloc(sizeof(int16_t) * g_NumberBoxes, GBUF_FLYZONE);
        if (!g_FlyZone[i] || !fread(g_FlyZone[i], sizeof(int16_t), g_NumberBoxes, fp))
			{
				MessageBox(NULL, "LoadBoxes(): Unable to load 'fly_zone'", "Tomb Raider", MB_OK);
		        return false;
			}
    }

	return true;
}

bool LoadAnimatedTextures(FILE *fp)
{
	int32_t AnimTextureRangeCount;

	fread(&AnimTextureRangeCount, sizeof(int32_t), 1, fp);
    
    g_AnimTextureRanges = (int16_t *) Game_Alloc(sizeof(int16_t) * AnimTextureRangeCount, GBUF_ANIMATING_TEXTURE_RANGES);
    
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
            MessageBox(NULL, "LoadItems(): Too Many g_Items being Loaded!", "Tomb Raider", MB_OK);
            return false;
        }

		g_Items = (ITEM_INFO *) Game_Alloc(sizeof(ITEM_INFO) * MAX_ITEMS, GBUF_ITEMS);

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

            if (g_Items[i].object_number < 0 || g_Items[i].object_number >= O_NUMBER_OF)
			{
				MessageBox(NULL, "LoadItems(): Bad Object number on Item", "Tomb Raider", MB_OK);
				//"Level_LoadItems(): Bad Object number (%d) on Item %d", item->object_number, i);
            }

            InitialiseItem(i);
        }
	}

	return true;
}

bool LoadDepthQ(FILE *fp)
{
	//пропуск загрузки 32 * 256 = depthQ size
    //fseek(fp, sizeof(uint8_t) * 32 * 256, SEEK_CUR);

    fread(depthq_table, 32*256, 1, fp);

	// Force colour 0 to black 
	for (int i=0; i<32; i++)
    {
		depthq_table[i][0] = 0;
    }

	for (int i=0; i<32; i++)
	{
		for (int j=0; j<256; j++)
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
    
	for ( int i = 0; i < 256; i++ )
	{
		GamePalette[i].r = palette[i].r * 4;
		GamePalette[i].g = palette[i].g * 4;
		GamePalette[i].b = palette[i].b * 4;
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

    g_Cine = (int16_t *) Game_Alloc(sizeof(int16_t) * 8 * g_NumCineFrames, GBUF_CINEMATIC_FRAMES);
    
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

    g_DemoData = (uint32_t *) Game_Alloc(sizeof(uint32_t) * DEMO_COUNT_MAX, GBUF_LOADDEMO_BUFFER);
    
	fread(g_DemoData, 1, size, fp);
    
	return true;
}

bool LoadSoundSamples(FILE *fp)
{
	fread(&g_SampleLUT[0],sizeof(int16_t)*256,1,fp);
	//fread(&g_SampleLUT[0],sizeof(int16_t)*MAX_SOUND_SAMPLES,1,fp);

	fread(&NumSampleInfos,sizeof(int32_t),1, fp);

	if (NumSampleInfos == 0)
	{
		MessageBox(NULL, "No sample infos", "Tomb Radier", MB_OK);
		return false;
	}

	g_SampleInfos = (SOUND_SAMPLE_INFO *) Game_Alloc(sizeof(SOUND_SAMPLE_INFO) * NumSampleInfos, GBUF_SAMPLE_INFOS);
    
	fread(g_SampleInfos, sizeof(SOUND_SAMPLE_INFO), NumSampleInfos, fp);

    int32_t SampleDataSize;
    fread(&SampleDataSize, sizeof(int32_t), 1, fp);
    
    if (!SampleDataSize)
	{
		MessageBox(NULL, "No Sample Data", "Tomb Raider", MB_OK);
        return false;
    }

	char *sample_data = (char *) Game_Alloc(SampleDataSize, GBUF_SAMPLES);
    fread(sample_data, sizeof(char), SampleDataSize, fp);

	int32_t NumSamples;
    
	fread(&NumSamples, sizeof(int32_t), 1, fp);
    
    if (!NumSamples)
	{
		MessageBox(NULL, "No Samples", "Tomb Raider", MB_OK);
        return false;
    }

	int32_t *SampleOffsets = (int32_t *) malloc(sizeof(int32_t) * NumSamples);
    size_t *SampleSizes = (size_t *) malloc(sizeof(size_t) * NumSamples);
    
	fread(SampleOffsets, sizeof(int32_t), NumSamples, fp);

	const char **SamplePointers = (const char **)malloc(sizeof(char *) * NumSamples);
    
	for (int i = 0; i < NumSamples; i++)
	{
        SamplePointers[i] = sample_data + SampleOffsets[i];
    }

	for (int i = 0; i < NumSamples; i++)
	{
        int current_offset = SampleOffsets[i];
        int next_offset = i + 1 >= NumSamples ? (int)File_Size(fp) : SampleOffsets[i + 1];
        SampleSizes[i] = next_offset - current_offset;
    }

	DirectSound_Init();

	DWORD dwHeader[11], dwWaveLength;
	WAVEFORMATEX* pWF = reinterpret_cast<WAVEFORMATEX*>(&dwHeader[5]);

	for ( int i = 0; i < NumSamples; i++ )
	{
		memcpy(dwHeader, SamplePointers[i], sizeof(DWORD) * 11);

		if (dwHeader[0]!=MAKEFOURCC('R','I','F','F') || 
			dwHeader[2]!=MAKEFOURCC('W','A','V','E') ||
			dwHeader[9]!=MAKEFOURCC('d','a','t','a'))

		{
			MessageBox(NULL, "Not WAVE format!", "Tomb Raider", MB_OK);
		}

		//pWF->cbSize = sizeof(WAVEFORMATEX);
		pWF->cbSize = 0;

		dwWaveLength = dwHeader[10];

		/*
        //SAVE SOUND TO FILE
		//id 72 = sound 108 rotate ring sound
		//id 73 = sound 109 open item sound
		
		if(i == 73)
		{
			FILE *fp;

			fp = fopen("sound.wav", "wb");

			fwrite(SamplePointers[i], SampleSizes[i], 1, fp);

			fclose(fp);
		}
		*/
		
		if (FAILED(DS_MakeSample(i, pWF,
			//пропускаем заголовок WAV файла sizeof(DWORD) * 11
			(unsigned char*)SamplePointers[i] + sizeof(DWORD) * 11,
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

