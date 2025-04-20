//#pragma once

#ifndef _INPUT_
#define _INPUT_


#include "types.h"

#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

//typedef int16_t S_INPUT_KEYCODE;
typedef int32_t S_INPUT_KEYCODE;
bool S_Input_Key(INPUT_KEY key);

S_INPUT_KEYCODE S_Input_ReadKeyCode();
const char *S_Input_GetKeyCodeName(S_INPUT_KEYCODE key);
bool S_Input_IsKeyConflicted(INPUT_KEY key);
void S_Input_SetKeyAsConflicted(INPUT_KEY key, bool is_conflicted);
S_INPUT_KEYCODE S_Input_GetAssignedKeyCode(int16_t layout_num, INPUT_KEY key);
void S_Input_AssignKeyCode(int16_t layout_num, INPUT_KEY key, S_INPUT_KEYCODE key_code);


#endif