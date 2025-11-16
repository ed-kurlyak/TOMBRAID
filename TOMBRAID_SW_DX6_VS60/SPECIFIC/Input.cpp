#include "input.h"
#include "windows.h"

//#include "config.h"
//#include "inv.h"
//#include "lara.h"
//#include "game/shell.h"
//#include "vars.h"
//#include "global/vars_platform.h"
//#include "log.h"
//#include "specific/s_shell.h"

/*
#include <dinput.h>
#include <stdbool.h>
*/

//#define KEY_DOWN(a) ((m_DIKeys[(a)] & 0x80) != 0)
#define KEY_DOWN(a) (GetAsyncKeyState(a) & 0xFF00)

bool m_KeyConflict[INPUT_KEY_NUMBER_OF] = {false};
// bool m_KeyConflict[INPUT_KEY_NUMBER_OF];

S_INPUT_KEYCODE m_Layout[2][INPUT_KEY_NUMBER_OF] = {
	// built-in controls
	{
		VK_UP,		// INPUT_KEY_UP
		VK_DOWN,	// INPUT_KEY_DOWN
		VK_LEFT,	// INPUT_KEY_LEFT
		VK_RIGHT,   // INPUT_KEY_RIGHT
		VK_DELETE,  // INPUT_KEY_STEP_L
		VK_NEXT,	// INPUT_KEY_STEP_R
		VK_SHIFT,   // INPUT_KEY_SLOW
		VK_MENU,	// INPUT_KEY_JUMP
		VK_CONTROL, // INPUT_KEY_ACTION
		VK_SPACE,   // INPUT_KEY_DRAW
		VK_INSERT,  // INPUT_KEY_LOOK
		VK_END,		// INPUT_KEY_ROLL
		VK_ESCAPE   // INPUT_KEY_OPTION
	},
	// default user controls
	{
		VK_UP,		// INPUT_KEY_UP
		VK_DOWN,	// INPUT_KEY_DOWN
		VK_LEFT,	// INPUT_KEY_LEFT
		VK_RIGHT,   // INPUT_KEY_RIGHT
		VK_HOME,	// INPUT_KEY_STEP_L
		VK_PRIOR,   // INPUT_KEY_STEP_R
		VK_SHIFT,   // INPUT_KEY_SLOW
		VK_MENU,	// INPUT_KEY_JUMP
		VK_CONTROL, // INPUT_KEY_ACTION
		VK_SPACE,   // INPUT_KEY_DRAW
		VK_INSERT,  // INPUT_KEY_LOOK
		VK_CLEAR,   // INPUT_KEY_ROLL
		VK_ESCAPE   // INPUT_KEY_OPTION
	}
	// clang-format on
};

// static LPDIRECTINPUT8 m_DInput = NULL;
// static LPDIRECTINPUTDEVICE8 m_IDID_SysKeyboard = NULL;
// static LPDIRECTINPUTDEVICE8 m_IDID_Joystick = NULL;
/*
uint8_t m_DIKeys[256] = { 0 };

int32_t m_MedipackCoolDown = 0;
*/

// static bool S_Input_DInput_Create();
// static void S_Input_DInput_Shutdown();
// static bool S_Input_DInput_KeyboardCreate();
// static void S_Input_DInput_KeyboardRelease();
// static bool S_Input_DInput_KeyboardRead();
// static bool S_Input_KbdKey(INPUT_KEY key, INPUT_LAYOUT layout);
// static bool S_Input_Key(INPUT_KEY key);

// static HRESULT S_Input_DInput_JoystickCreate();
// static void S_Input_DInput_JoystickRelease();
// static BOOL CALLBACK
// S_Input_EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE instance, LPVOID context);
// static BOOL CALLBACK
// S_Input_EnumCallback(LPCDIDEVICEINSTANCE instance, LPVOID context);

/*
void S_Input_Init()
{
	if (!S_Input_DInput_Create()) {
		Shell_ExitSystem("Fatal DirectInput error!");
	}

	if (!S_Input_DInput_KeyboardCreate()) {
		Shell_ExitSystem("Fatal DirectInput error!");
	}

	if (g_Config.enable_xbox_one_controller) {
		S_Input_DInput_JoystickCreate();
	} else {
		m_IDID_Joystick = NULL;
	}
}

void InputShutdown()
{
	S_Input_DInput_KeyboardRelease();
	if (g_Config.enable_xbox_one_controller) {
		S_Input_DInput_JoystickRelease();
	}
	S_Input_DInput_Shutdown();
}

static bool S_Input_DInput_Create()
{
	HRESULT result = DirectInput8Create(
		g_TombModule, DIRECTINPUT_VERSION, &IID_IDirectInput8,
		(LPVOID *)&m_DInput, NULL);

	if (result) {
		LOG_ERROR("Error while calling DirectInput8Create: 0x%lx", result);
		return false;
	}

	return true;
}

static void S_Input_DInput_Shutdown()
{
	if (m_DInput) {
		IDirectInput_Release(m_DInput);
		m_DInput = NULL;
	}
}

bool S_Input_DInput_KeyboardCreate()
{
	HRESULT result = IDirectInput8_CreateDevice(
		m_DInput, &GUID_SysKeyboard, &m_IDID_SysKeyboard, NULL);
	if (result) {
		LOG_ERROR(
			"Error while calling IDirectInput8_CreateDevice: 0x%lx", result);
		return false;
	}

	result = IDirectInputDevice_SetCooperativeLevel(
		m_IDID_SysKeyboard, g_TombHWND, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (result) {
		LOG_ERROR(
			"Error while calling IDirectInputDevice_SetCooperativeLevel: 0x%lx",
			result);
		return false;
	}

	result =
		IDirectInputDevice_SetDataFormat(m_IDID_SysKeyboard, &c_dfDIKeyboard);
	if (result) {
		LOG_ERROR(
			"Error while calling IDirectInputDevice_SetDataFormat: 0x%lx",
			result);
		return false;
	}

	return true;
}

void S_Input_DInput_KeyboardRelease()
{
	if (m_IDID_SysKeyboard) {
		IDirectInputDevice_Unacquire(m_IDID_SysKeyboard);
		IDirectInputDevice_Release(m_IDID_SysKeyboard);
		m_IDID_SysKeyboard = NULL;
	}
}

static bool S_Input_DInput_KeyboardRead()
{
	if (!m_IDID_SysKeyboard) {
		return false;
	}

	while (IDirectInputDevice_GetDeviceState(
		m_IDID_SysKeyboard, sizeof(m_DIKeys), m_DIKeys)) {
		if (IDirectInputDevice_Acquire(m_IDID_SysKeyboard)) {
			memset(m_DIKeys, 0, sizeof(m_DIKeys));
			break;
		}
	}
	S_Shell_SpinMessageLoop();

	return true;
}
*/
static bool S_Input_KbdKey(INPUT_KEY key, INPUT_LAYOUT layout)
{
	S_INPUT_KEYCODE key_code = m_Layout[layout][key];

	if (KEY_DOWN(key_code))
	{
		return true;
	}
	/*
	if (key_code == DIK_LCONTROL) {
		return KEY_DOWN(DIK_RCONTROL);
	}
	if (key_code == DIK_RCONTROL) {
		return KEY_DOWN(DIK_LCONTROL);
	}
	if (key_code == DIK_LSHIFT) {
		return KEY_DOWN(DIK_RSHIFT);
	}
	if (key_code == DIK_RSHIFT) {
		return KEY_DOWN(DIK_LSHIFT);
	}
	if (key_code == DIK_LMENU) {
		return KEY_DOWN(DIK_RMENU);
	}
	if (key_code == DIK_RMENU) {
		return KEY_DOWN(DIK_LMENU);
	}
	*/
	return false;
}

bool S_Input_Key(INPUT_KEY key)
{
	return S_Input_KbdKey(key, INPUT_LAYOUT_USER) ||
		   (!S_Input_IsKeyConflicted(key) &&
			S_Input_KbdKey(key, INPUT_LAYOUT_DEFAULT));
}

S_INPUT_KEYCODE S_Input_ReadKeyCode()
{
	for (S_INPUT_KEYCODE key = 0; key < 256; key++)
	{
		if (KEY_DOWN(key))
		{
			return key;
		}
	}

	return -1;
}
/*

static HRESULT S_Input_DInput_JoystickCreate()
{
	HRESULT result;

	// Look for the first simple joystick we can find.
	if (FAILED(
			result = IDirectInput8_EnumDevices(
				m_DInput, DI8DEVCLASS_GAMECTRL, S_Input_EnumCallback, NULL,
				DIEDFL_ATTACHEDONLY))) {
		LOG_ERROR(
			"Error while calling IDirectInput8_EnumDevices: 0x%lx", result);
		return result;
	}

	// Make sure we got a joystick
	if (!m_IDID_Joystick) {
		LOG_ERROR("Joystick not found.\n");
		return E_FAIL;
	}
	LOG_INFO("Joystick found.\n");

	DIDEVCAPS capabilities;
	// request simple joystick format 2
	if (FAILED(
			result = IDirectInputDevice_SetDataFormat(
				m_IDID_Joystick, &c_dfDIJoystick2))) {
		LOG_ERROR(
			"Error while calling IDirectInputDevice_SetDataFormat: 0x%lx",
			result);
		S_Input_DInput_JoystickRelease();
		return result;
	}

	// don't request exclusive access
	if (FAILED(
			result = IDirectInputDevice_SetCooperativeLevel(
				m_IDID_Joystick, NULL,
				DISCL_NONEXCLUSIVE | DISCL_BACKGROUND))) {
		LOG_ERROR(
			"Error while calling IDirectInputDevice_SetCooperativeLevel: 0x%lx",
			result);
		S_Input_DInput_JoystickRelease();
		return result;
	}

	// get axis count, we should know what it is but best to ask
	capabilities.dwSize = sizeof(DIDEVCAPS);
	if (FAILED(
			result = IDirectInputDevice_GetCapabilities(
				m_IDID_Joystick, &capabilities))) {
		LOG_ERROR(
			"Error while calling IDirectInputDevice_GetCapabilities: 0x%lx",
			result);
		S_Input_DInput_JoystickRelease();
		return result;
	}

	// set the range we expect each axis to report back in
	if (FAILED(
			result = IDirectInputDevice_EnumObjects(
				m_IDID_Joystick, S_Input_EnumAxesCallback, NULL, DIDFT_AXIS))) {
		LOG_ERROR(
			"Error while calling IDirectInputDevice_EnumObjects: 0x%lx",
			result);
		S_Input_DInput_JoystickRelease();
		return result;
	}
	return result;
}

static void S_Input_DInput_JoystickRelease()
{
	if (m_IDID_Joystick) {
		IDirectInputDevice_Unacquire(m_IDID_Joystick);
		IDirectInputDevice_Release(m_IDID_Joystick);
		m_IDID_Joystick = NULL;
	}
}

static BOOL CALLBACK
S_Input_EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE instance, LPVOID context)
{
	HRESULT result;
	DIPROPRANGE propRange;

	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = -1024;
	propRange.lMax = 1024;

	// Set the range for the axis
	if (FAILED(
			result = IDirectInputDevice8_SetProperty(
				m_IDID_Joystick, DIPROP_RANGE, &propRange.diph))) {
		LOG_ERROR(
			"Error while calling IDirectInputDevice8_SetProperty: 0x%lx",
			result);
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

static BOOL CALLBACK
S_Input_EnumCallback(LPCDIDEVICEINSTANCE instance, LPVOID context)
{
	HRESULT result;

	// Obtain an interface to the enumerated joystick.
	result = IDirectInput8_CreateDevice(
		m_DInput, &instance->guidInstance, &m_IDID_Joystick, NULL);

	if (FAILED(result)) {
		LOG_ERROR(
			"Error while calling IDirectInput8_CreateDevice: 0x%lx", result);
		return DIENUM_CONTINUE;
	}
	// we got one, it will do.
	return DIENUM_STOP;
}

static HRESULT DInputJoystickPoll(DIJOYSTATE2 *joystate)
{
	HRESULT result;

	if (!m_IDID_Joystick) {
		return S_OK;
	}

	// Poll the device to read the current state
	result = IDirectInputDevice8_Poll(m_IDID_Joystick);
	if (FAILED(result)) {
		// focus was lost, try to reaquire the device
		result = IDirectInputDevice8_Acquire(m_IDID_Joystick);
		while (result == DIERR_INPUTLOST) {
			result = IDirectInputDevice8_Acquire(m_IDID_Joystick);
		}

		// A fatal error? Return failure.
		if ((result == DIERR_INVALIDPARAM)
			|| (result == DIERR_NOTINITIALIZED)) {
			LOG_ERROR(
				"Error while calling IDirectInputDevice8_Acquire: 0x%lx",
				result);
			return E_FAIL;
		}

		// If another application has control of this device, return
		// successfully.
		if (result == DIERR_OTHERAPPHASPRIO) {
			return S_OK;
		}
	}

	// Get the input's device state
	if (FAILED(
			result = IDirectInputDevice8_GetDeviceState(
				m_IDID_Joystick, sizeof(DIJOYSTATE2), joystate))) {
		return result; // The device should have been acquired during the Poll()
	}

	return S_OK;
}

INPUT_STATE S_Input_GetCurrentState()
{
	S_Input_DInput_KeyboardRead();

	INPUT_STATE linput = { 0 };

	linput.forward = S_Input_Key(INPUT_KEY_UP);
	linput.back = S_Input_Key(INPUT_KEY_DOWN);
	linput.left = S_Input_Key(INPUT_KEY_LEFT);
	linput.right = S_Input_Key(INPUT_KEY_RIGHT);
	linput.step_left = S_Input_Key(INPUT_KEY_STEP_L);
	linput.step_right = S_Input_Key(INPUT_KEY_STEP_R);
	linput.slow = S_Input_Key(INPUT_KEY_SLOW);
	linput.jump = S_Input_Key(INPUT_KEY_JUMP);
	linput.action = S_Input_Key(INPUT_KEY_ACTION);
	linput.draw = S_Input_Key(INPUT_KEY_DRAW);
	linput.look = S_Input_Key(INPUT_KEY_LOOK);
	linput.roll =
		S_Input_Key(INPUT_KEY_ROLL) || (linput.forward && linput.back);
	linput.option =
		S_Input_Key(INPUT_KEY_OPTION) && g_Camera.type != CAM_CINEMATIC;
	linput.pause = S_Input_Key(INPUT_KEY_PAUSE);
	linput.camera_up = S_Input_Key(INPUT_KEY_CAMERA_UP);
	linput.camera_down = S_Input_Key(INPUT_KEY_CAMERA_DOWN);
	linput.camera_left = S_Input_Key(INPUT_KEY_CAMERA_LEFT);
	linput.camera_right = S_Input_Key(INPUT_KEY_CAMERA_RIGHT);
	linput.camera_reset = S_Input_Key(INPUT_KEY_CAMERA_RESET);

	if (g_Config.enable_cheats) {
		linput.item_cheat = S_Input_Key(INPUT_KEY_ITEM_CHEAT);
		linput.fly_cheat = S_Input_Key(INPUT_KEY_FLY_CHEAT);
		linput.level_skip_cheat = S_Input_Key(INPUT_KEY_LEVEL_SKIP_CHEAT);
		linput.health_cheat = KEY_DOWN(DIK_F11);
	}

	if (g_Config.enable_tr3_sidesteps) {
		if (linput.slow && !linput.forward && !linput.back && !linput.step_left
			&& !linput.step_right) {
			if (linput.left) {
				linput.left = 0;
				linput.step_left = 1;
			} else if (linput.right) {
				linput.right = 0;
				linput.step_right = 1;
			}
		}
	}

	if (g_Config.enable_numeric_keys) {
		if (KEY_DOWN(DIK_1) && Inv_RequestItem(O_GUN_ITEM)) {
			g_Lara.request_gun_type = LGT_PISTOLS;
		} else if (KEY_DOWN(DIK_2) && Inv_RequestItem(O_SHOTGUN_ITEM)) {
			g_Lara.request_gun_type = LGT_SHOTGUN;
		} else if (KEY_DOWN(DIK_3) && Inv_RequestItem(O_MAGNUM_ITEM)) {
			g_Lara.request_gun_type = LGT_MAGNUMS;
		} else if (KEY_DOWN(DIK_4) && Inv_RequestItem(O_UZI_ITEM)) {
			g_Lara.request_gun_type = LGT_UZIS;
		}

		if (m_MedipackCoolDown) {
			m_MedipackCoolDown--;
		} else {
			if (KEY_DOWN(DIK_8) && Inv_RequestItem(O_MEDI_OPTION)) {
				UseItem(O_MEDI_OPTION);
				m_MedipackCoolDown = FRAMES_PER_SECOND / 2;
			} else if (KEY_DOWN(DIK_9) && Inv_RequestItem(O_BIGMEDI_OPTION)) {
				UseItem(O_BIGMEDI_OPTION);
				m_MedipackCoolDown = FRAMES_PER_SECOND / 2;
			}
		}
	}

	linput.select = linput.action || KEY_DOWN(DIK_RETURN);
	linput.deselect = S_Input_Key(INPUT_KEY_OPTION);

	if (linput.left && linput.right) {
		linput.left = 0;
		linput.right = 0;
	}

	if (!g_ModeLock && g_Camera.type != CAM_CINEMATIC) {
		linput.save = KEY_DOWN(DIK_F5);
		linput.load = KEY_DOWN(DIK_F6);
	}

	if (KEY_DOWN(DIK_F3)) {
		g_Config.render_flags.bilinear ^= 1;
		while (KEY_DOWN(DIK_F3)) {
			S_Input_DInput_KeyboardRead();
		}
	}

	if (KEY_DOWN(DIK_F4)) {
		g_Config.render_flags.perspective ^= 1;
		while (KEY_DOWN(DIK_F4)) {
			S_Input_DInput_KeyboardRead();
		}
	}

	if (KEY_DOWN(DIK_F2)) {
		g_Config.render_flags.fps_counter ^= 1;
		while (KEY_DOWN(DIK_F2)) {
			S_Input_DInput_KeyboardRead();
		}
	}

	if (m_IDID_Joystick) {
		DIJOYSTATE2 state;
		DInputJoystickPoll(&state);

		// check Y
		if (state.lY > 512) {
			linput.back = 1;
		} else if (state.lY < -512) {
			linput.forward = 1;
		}
		// check X
		if (state.lX > 512) {
			linput.right = 1;
		} else if (state.lX < -512) {
			linput.left = 1;
		}
		// check Z
		if (state.lZ > 512) {
			linput.step_left = 0;
		} else if (state.lZ < -512) {
			linput.step_right = 1;
		}

		// check 2nd stick X
		if (state.lRx > 512) {
			linput.camera_right = 1;
		} else if (state.lRx < -512) {
			linput.camera_left = 1;
		}
		// check 2nd stick Y
		if (state.lRy > 512) {
			linput.camera_down = 1;
		} else if (state.lRy < -512) {
			linput.camera_up = 1;
		}

		// check buttons
		if (state.rgbButtons[0]) { // A
			linput.jump = 1;
			linput.select = 1;
		}
		if (state.rgbButtons[1]) { // B
			linput.roll = 1;
			linput.deselect = 1;
		}
		if (state.rgbButtons[2]) { // X
			linput.action = 1;
			linput.select = 1;
		}
		if (state.rgbButtons[3]) { // Y
			linput.look = 1;
			linput.deselect = 1;
		}
		if (state.rgbButtons[4]) { // LB
			linput.slow = 1;
		}
		if (state.rgbButtons[5]) { // RB
			linput.draw = 1;
		}
		if (state.rgbButtons[6]) { // back
			linput.option = 1;
		}
		if (state.rgbButtons[7]) { // start
			linput.pause = 1;
		}
		if (state.rgbButtons[9]) { // 2nd axis click
			linput.camera_reset = 1;
		}
		// check dpad
		if (state.rgdwPOV[0] == 0) { // up
			linput.draw = 1;
		}
	}

	return linput;
}
*/

const char *S_Input_GetKeyCodeName(S_INPUT_KEYCODE key)
{

	// clang-format off
    switch (key) {
        case VK_ESCAPE:       return "ESC";
        case VK_1:            return "1";
        case VK_2:            return "2";
        case VK_3:            return "3";
        case VK_4:            return "4";
        case VK_5:            return "5";
        case VK_6:            return "6";
        case VK_7:            return "7";
        case VK_8:            return "8";
        case VK_9:            return "9";
        case VK_0:            return "0";
        //case VK_OEM_MINUS:        return "-";
        //case VK_OEM_PLUS:       return "+";
        case VK_BACK:         return "BKSP";
        case VK_TAB:          return "TAB";
        case 'Q':            return "Q";
        case 'W':            return "W";
        case 'E':            return "E";
        case 'R':            return "R";
        case 'T':            return "T";
        case 'Y':            return "Y";
        case 'U':            return "U";
        case 'I':            return "I";
        case 'O':            return "O";
        case 'P':            return "P";
        case '<':            return "<";
        case '>':            return ">";
        case VK_RETURN:       return "RET";
        case VK_CONTROL:     return "CTRL";
        case 'A':            return "A";
        case 'S':            return "S";
        case 'D':            return "D";
        case 'F':            return "F";
        case 'G':            return "G";
        case 'H':            return "H";
        case 'J':            return "J";
        case 'K':            return "K";
        case 'L':            return "L";
        case ';':            return ";";
        //case VK_OEM_7:   return "\'";
        //case VK_OEM_3:        return "`";
        case VK_SHIFT:       return "SHIFT";
        //case VK_OEM_5:    return "\\";
        case 'Z':            return "Z";
        case 'X':            return "X";
        case 'C':            return "C";
        case 'V':            return "V";
        case 'B':            return "B";
        case 'N':            return "N";
        case 'M':            return "M";
        //case VK_OEM_COMMA:        return ",";
        //case VK_OEM_PERIOD:       return ".";
        
        //case VK_OEM_2:        return "/";
        //case VK_OEM_2:          return (GetKeyState(VK_SHIFT) & 0x8000) ? "?" : "/";
        
        //case VK_SHIFT:       return "SHIFT";
        case VK_MULTIPLY:     return "PADx";
        case VK_MENU:        return "ALT";
        case VK_SPACE:        return "SPACE";
        case VK_CAPITAL:      return "CAPS";
        case VK_F1:           return "F1";
        case VK_F2:           return "F2";
        case VK_F3:           return "F3";
        case VK_F4:           return "F4";
        case VK_F5:           return "F5";
        case VK_F6:           return "F6";
        case VK_F7:           return "F7";
        case VK_F8:           return "F8";
        case VK_F9:           return "F9";
        case VK_F10:          return "F10";
        case VK_NUMLOCK:      return "NMLK";
        case VK_SCROLL:       return "SCLK";
        case VK_NUMPAD7:      return "PAD7";
        case VK_NUMPAD8:      return "PAD8";
        case VK_NUMPAD9:      return "PAD9";
        case VK_SUBTRACT:     return "PAD-";
        case VK_NUMPAD4:      return "PAD4";
        case VK_NUMPAD5:      return "PAD5";
        case VK_NUMPAD6:      return "PAD6";
        case VK_ADD:          return "PAD+";
        case VK_NUMPAD1:      return "PAD1";
        case VK_NUMPAD2:      return "PAD2";
        case VK_NUMPAD3:      return "PAD3";
        case VK_NUMPAD0:      return "PAD0";
        case VK_DECIMAL:      return "PAD.";
        case VK_F11:          return "F11";
        case VK_F12:          return "F12";
        case VK_F13:          return "F13";
        case VK_F14:          return "F14";
        case VK_F15:          return "F15";
        //case VK_NUMPADEQUALS: return "PAD=";
        //case VK_OEM_3:           return (GetKeyState(VK_SHIFT) & 0x8000) ? "@" : "\"";
        //case VK_OEM_1:        return ":";
        
        //case DIK_UNDERLINE:    return "_";
        //case VK_OEM_MINUS:      return (GetKeyState(VK_SHIFT) & 0x8000) ? "_" : "-";
        
        //case DIK_NUMPADENTER:  return "ENTER";
        //case DIK_RCONTROL:     return "CTRL";
        case VK_DIVIDE:       return "PAD/";
        //case DIK_RMENU:        return "ALT";
        case VK_HOME:         return "HOME";
        case VK_UP:           return "UP";
        case VK_PRIOR:        return "PGUP";
        case VK_LEFT:         return "LEFT";
        case VK_RIGHT:        return "RIGHT";
        case VK_END:          return "END";
        case VK_DOWN:         return "DOWN";
        case VK_NEXT:         return "PGDN";
        case VK_INSERT:       return "INS";
        case VK_DELETE:       return "DEL";
        case VK_CLEAR:          return "NUM5";
    }

	// clang-format on
	return "????";
}

bool S_Input_IsKeyConflicted(INPUT_KEY key) { return m_KeyConflict[key]; }

void S_Input_SetKeyAsConflicted(INPUT_KEY key, bool is_conflicted)
{
	m_KeyConflict[key] = is_conflicted;
}

S_INPUT_KEYCODE S_Input_GetAssignedKeyCode(int16_t layout_num, INPUT_KEY key)
{
	return m_Layout[layout_num][key];
}

void S_Input_AssignKeyCode(int16_t layout_num, INPUT_KEY key,
						   S_INPUT_KEYCODE key_code)
{
	m_Layout[layout_num][key] = key_code;
}
