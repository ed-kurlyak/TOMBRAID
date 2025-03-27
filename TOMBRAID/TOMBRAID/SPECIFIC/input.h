//#pragma once

#ifndef _INPUT_
#define _INPUT_


#include "types.h"


typedef int16_t S_INPUT_KEYCODE;

S_INPUT_KEYCODE S_Input_ReadKeyCode();
const char *S_Input_GetKeyCodeName(S_INPUT_KEYCODE key);
bool S_Input_IsKeyConflicted(INPUT_KEY key);
void S_Input_SetKeyAsConflicted(INPUT_KEY key, bool is_conflicted);
S_INPUT_KEYCODE S_Input_GetAssignedKeyCode(int16_t layout_num, INPUT_KEY key);
void S_Input_AssignKeyCode(int16_t layout_num, INPUT_KEY key, S_INPUT_KEYCODE key_code);


#endif