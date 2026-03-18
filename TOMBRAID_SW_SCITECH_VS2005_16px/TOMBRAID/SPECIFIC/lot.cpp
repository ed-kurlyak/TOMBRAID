#include "lot.h"

/*
#include "game/gamebuf.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/vars.h"

#include <stddef.h>

*/

#include "const.h"
#include "init.h"
#include "vars.h"
#include <windows.h>

static int32_t m_SlotsUsed = 0;
static CREATURE_INFO *m_BaddieSlots = NULL;

void InitialiseLOTArray()
{
	m_BaddieSlots = (CREATURE_INFO *)Game_Alloc(
		NUM_SLOTS * sizeof(CREATURE_INFO), GBUF_CREATURE_INFO);

	for (int i = 0; i < NUM_SLOTS; i++)
	{
		CREATURE_INFO *creature = &m_BaddieSlots[i];
		creature->item_num = NO_ITEM;
		creature->LOT.node = (BOX_NODE *)Game_Alloc(
			sizeof(BOX_NODE) * g_NumberBoxes, GBUF_CREATURE_LOT);
	}

	m_SlotsUsed = 0;
}

void DisableBaddieAI(int16_t item_num)
{
	ITEM_INFO *item = &g_Items[item_num];
	CREATURE_INFO *creature = (CREATURE_INFO *)item->data;
	item->data = NULL;
	if (creature)
	{
		creature->item_num = NO_ITEM;
		m_SlotsUsed--;
	}
}

//функция активирует или "пробуждает" врага, если это возможно
int32_t EnableBaddieAI(int16_t item_num, int32_t always)
{
	//если у врага уже есть связанная структура CREATURE_INFO (data != nullptr),
	//значит ИИ уже активирован → вернуть успех (1)
	if (g_Items[item_num].data)
	{
		return 1;
	}

	//если общее количество занятых слотов(m_SlotsUsed)
	//меньше максимально допустимого(NUM_SLOTS), ищем свободный
	if (m_SlotsUsed < NUM_SLOTS)
	{
		for (int32_t slot = 0; slot < NUM_SLOTS; slot++)
		{
			CREATURE_INFO *creature = &m_BaddieSlots[slot];

			if (creature->item_num == NO_ITEM)
			{
				//если слот с item_num == NO_ITEM, значит он не используется
				//инициализируем этот слот для текущего врага и выходим
				InitialiseSlot(item_num, slot);
				return 1;
			}
		}
		// Shell_ExitSystem("UnpauseBaddie() grimmer!");
	}

	int32_t worst_dist = 0;

	//нет свободных слотов — ищем кандидата на вытеснение
	if (!always)
	{
		ITEM_INFO *item = &g_Items[item_num];

		int32_t x = (item->pos.x - g_Camera.pos.x) >> 8;
		int32_t y = (item->pos.y - g_Camera.pos.y) >> 8;
		int32_t z = (item->pos.z - g_Camera.pos.z) >> 8;

		//считается минимальное расстояние(worst_dist) от текущего врага
		//до камеры(g_Camera.pos) - используется как порог для сравнения
		worst_dist = SQUARE(x) + SQUARE(y) + SQUARE(z);
	}

	int32_t worst_slot = -1;

	//поиск самого дальнего активного врага
	//перебираются все занятые слоты,
	//вычисляется расстояние каждого врага до камеры
	for (int32_t slot = 0; slot < NUM_SLOTS; slot++)
	{
		CREATURE_INFO *creature = &m_BaddieSlots[slot];
		ITEM_INFO *item = &g_Items[creature->item_num];

		int32_t x = (item->pos.x - g_Camera.pos.x) >> 8;
		int32_t y = (item->pos.y - g_Camera.pos.y) >> 8;
		int32_t z = (item->pos.z - g_Camera.pos.z) >> 8;

		int32_t dist = SQUARE(x) + SQUARE(y) + SQUARE(z);

		//сохраняется индекс врага, который дальше всего от камеры
		//и имеет наибольшее dist
		if (dist > worst_dist)
		{
			worst_dist = dist;
			worst_slot = slot;
		}
	}

	//если никого вытеснить нельзя — выход
	if (worst_slot < 0)
	{
		return 0;
	}

	//сначала выключается ИИ у самого дальнего врага
	//он становится IS_INVISIBLE и слетает его слот
	g_Items[m_BaddieSlots[worst_slot].item_num].status = IS_INVISIBLE;
	DisableBaddieAI(m_BaddieSlots[worst_slot].item_num);

	//затем текущему item_num присваивается этот слот
	InitialiseSlot(item_num, worst_slot);

	return 1;
}

//функция инициализирует один из доступных слотов ИИ(CREATURE_INFO)
//для врага с индексом item_num, она подготавливает данные,
//чтобы враг мог начать "думать" и действовать по ИИ - логике.
void InitialiseSlot(int16_t item_num, int32_t slot)
{
	CREATURE_INFO *creature = &m_BaddieSlots[slot];
	ITEM_INFO *item = &g_Items[item_num];

	//привязать врага(ITEM_INFO) к слоту ИИ(CREATURE_INFO)
	item->data = creature;

	creature->item_num = item_num;
	creature->mood = MOOD_BORED;
	creature->head_rotation = 0;
	creature->neck_rotation = 0;
	//максимальный угол поворота тела врага за кадр(обычно 1 градус)
	creature->maximum_turn = PHD_DEGREE;
	creature->flags = 0;

	//максимальный шаг, который враг может сделать по высоте
	creature->LOT.step = STEP_L;
	//максимальное падение
	creature->LOT.drop = -STEP_L;
	//маска препятствий
	creature->LOT.block_mask = BLOCKED;
	//параметр движения по воздуху(0 = не летает по умолчанию)
	creature->LOT.fly = 0;

	switch (item->object_number)
	{
	case O_BAT:
	case O_ALLIGATOR:
	case O_FISH:
		creature->LOT.step = WALL_L * 20;
		creature->LOT.drop = -WALL_L * 20;
		creature->LOT.fly = STEP_L / 16;
		break;

	case O_DINOSAUR:
	case O_WARRIOR1:
	case O_CENTAUR:
		creature->LOT.block_mask = BLOCKABLE;
		break;

	case O_WOLF:
	case O_LION:
	case O_LIONESS:
	case O_PUMA:
		creature->LOT.drop = -WALL_L;
		break;

	case O_APE:
		creature->LOT.step = STEP_L * 2;
		creature->LOT.drop = -WALL_L;
		break;
	}

	//сбрасывает состояние логического объекта(путь, цели, флаги и т.д.)
	ClearLOT(&creature->LOT);
	CreateZone(item);

	//обновляется глобальный счётчик занятых слотов ИИ
	m_SlotsUsed++;
}

//функция смотрит в каком box'е враг
//смотрит какая зона этого box'а
//собирает все box'ы которые принадлежат этой зоне
//таким образом по этим боксам (в этой зоне)
//может перемещаться враг
void CreateZone(ITEM_INFO *item)
{
	CREATURE_INFO *creature = (CREATURE_INFO *)item->data;

	int16_t *zone;
	int16_t *flip;

	if (creature->LOT.fly)
	{
		zone = g_FlyZone[0];
		flip = g_FlyZone[1];
	}
	else if (creature->LOT.step == STEP_L)
	{
		zone = g_GroundZone[0];
		flip = g_GroundZone[1];
	}
	else
	{
		zone = g_GroundZone2[1];
		flip = g_GroundZone2[1];
	}

	ROOM_INFO *r = &g_RoomInfo[item->room_number];

	//переводит координаты врага в координаты клетки(tile'а) уровня
	int32_t x_floor = (item->pos.z - r->z) >> WALL_SHIFT;
	int32_t y_floor = (item->pos.x - r->x) >> WALL_SHIFT;

	//из неё извлекает номер box, в которой враг сейчас стоит
	item->box_number = r->floor[x_floor + y_floor * r->x_size].box;

	//сохраняем идентификаторы зоны по основной и альтернативной
	//карте(на случай, если уровень «перевёрнут»
	int16_t zone_number = zone[item->box_number];
	int16_t flip_number = flip[item->box_number];

	creature->LOT.zone_count = 0;
	BOX_NODE *node = creature->LOT.node;

	//перебираются все существующие box'ы в уровне (g_NumberBoxes)
	//сохраняем все box'ы которые принадлежат одной зоне
	//и в этой зоне расположен box данного врага
	for (int i = 0; i < g_NumberBoxes; i++)
	{
		if (zone[i] == zone_number || flip[i] == flip_number)
		{
			node->box_number = i;
			node++;
			creature->LOT.zone_count++;
		}
	}
}

int32_t InitialiseLOT(LOT_INFO *LOT)
{
	LOT->node = (BOX_NODE *)Game_Alloc(sizeof(BOX_NODE) * g_NumberBoxes,
									   GBUF_CREATURE_LOT);
	ClearLOT(LOT);
	return 1;
}

//очистить логику маршрута(LOT = Logic Object Template),
//чтобы враг начал с чистого состояния — без предыдущих маршрутов,
//целей или данных поиска
void ClearLOT(LOT_INFO *LOT)
{
	LOT->search_number = 0;
	LOT->head = NO_BOX;
	LOT->tail = NO_BOX;
	LOT->target_box = NO_BOX;
	LOT->required_box = NO_BOX;

	for (int i = 0; i < g_NumberBoxes; i++)
	{
		BOX_NODE *node = &LOT->node[i];
		node->search_number = 0;
		node->exit_box = NO_BOX;
		node->next_expansion = NO_BOX;
	}
}
