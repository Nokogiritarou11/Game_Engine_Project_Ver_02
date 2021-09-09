#include "Input.h"
#include "Cursor.h"
#include "DxSystem.h"
#include "Mathf.h"

#if _DEBUG
#include "Engine.h"
#include "Editor.h"
#endif

using namespace std;
using namespace BeastEngine;

BYTE Input::key_state[256] = { 0x00 };
BYTE Input::key_state_old[256] = { 0x00 };
Vector2 Input::mouse_position = { 0,0 };
Vector2 Input::mouse_position_old = { 0,0 };
Input::Controller_State Input::controller_state[4];

void Input::Update()
{
	//マウス位置
#if _DEBUG
#else
	RECT rect;
	GetWindowRect(DxSystem::hwnd, &rect);
#endif

	if (Cursor::cursor_lock_mode == CursorLock_Mode::Locked)
	{
#if _DEBUG
		mouse_position_old = { Cursor::lock_position.x - Engine::editor->game_view_position.x,Engine::editor->game_view_position.y - Cursor::lock_position.y };
#else
		mouse_position_old = { Cursor::lock_position.x - static_cast<float>(rect.left) ,static_cast<float>(rect.bottom) - Cursor::lock_position.y };
#endif
	}
	else
	{
		mouse_position_old = mouse_position;
	}

	POINT mouse_p;
	GetCursorPos(&mouse_p);
#if _DEBUG
	mouse_position = { static_cast<float>(mouse_p.x) - Engine::editor->game_view_position.x, Engine::editor->game_view_position.y - static_cast<float>(mouse_p.y) };
#else
	mouse_position = { static_cast<float>(mouse_p.x) - static_cast<float>(rect.left),static_cast<float>(rect.bottom) - static_cast<float>(mouse_p.y) };
#endif

	//キーボード取得
	memcpy(key_state_old, key_state, sizeof(key_state));
	if (!GetKeyboardState(key_state))
	{
		assert(false);
	}

	//パッド取得
	DWORD dwResult;
	for (DWORD i = 0; i < 4; ++i)
	{
		dwResult = XInputGetState(i, &controller_state[i].state);

		if (dwResult == ERROR_SUCCESS)
		{
			controller_state[i].is_connected = true;
			controller_state[i].Update();
		}
		else
		{
			controller_state[i].is_connected = false;
		}
	}
}

void Input::Controller_State::Update()
{
	result_old = result;
	WORD button = state.Gamepad.wButtons;

	result.buttons[0] = (button & XINPUT_GAMEPAD_DPAD_UP);
	result.buttons[1] = (button & XINPUT_GAMEPAD_DPAD_DOWN);
	result.buttons[2] = (button & XINPUT_GAMEPAD_DPAD_RIGHT);
	result.buttons[3] = (button & XINPUT_GAMEPAD_DPAD_LEFT);
	result.buttons[4] = (button & XINPUT_GAMEPAD_A);
	result.buttons[5] = (button & XINPUT_GAMEPAD_B);
	result.buttons[6] = (button & XINPUT_GAMEPAD_X);
	result.buttons[7] = (button & XINPUT_GAMEPAD_Y);
	result.buttons[8] = (button & XINPUT_GAMEPAD_START);
	result.buttons[9] = (button & XINPUT_GAMEPAD_BACK);
	result.buttons[10] = (button & XINPUT_GAMEPAD_LEFT_THUMB);
	result.buttons[11] = (button & XINPUT_GAMEPAD_RIGHT_THUMB);
	result.buttons[12] = (button & XINPUT_GAMEPAD_LEFT_SHOULDER);
	result.buttons[13] = (button & XINPUT_GAMEPAD_RIGHT_SHOULDER);

	//左デッドゾーン
	if ((state.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(state.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		state.Gamepad.sThumbLX = 0;
		state.Gamepad.sThumbLY = 0;

	}
	//右デッドゾーン
	if ((state.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(state.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		state.Gamepad.sThumbRX = 0;
		state.Gamepad.sThumbRY = 0;
	}

	result.left_axis = { Mathf::Clamp(state.Gamepad.sThumbLX / 32767.0f, -1.0f, 1.0f), Mathf::Clamp(state.Gamepad.sThumbLY / 32767.0f, -1.0f, 1.0f) };
	result.right_axis = { Mathf::Clamp(state.Gamepad.sThumbRX / 32767.0f, -1.0f, 1.0f), Mathf::Clamp(state.Gamepad.sThumbRY / 32767.0f, -1.0f, 1.0f) };

	result.left_trigger = state.Gamepad.bLeftTrigger / 255.0f;
	result.right_trigger = state.Gamepad.bRightTrigger / 255.0f;
}

bool Input::Get_Key(Key_Code key)
{
	return (key_state[static_cast<int>(key)] & 0x80);
}

bool Input::Get_Key_Down(Key_Code key)
{
	return (key_state[static_cast<int>(key)] & 0x80) && !(key_state_old[static_cast<int>(key)] & 0x80);
}

bool Input::Get_Key_Up(Key_Code key)
{
	return !(key_state[static_cast<int>(key)] & 0x80) && (key_state_old[static_cast<int>(key)] & 0x80);
}

bool Input::Get_Mouse_Button(int button)
{
	switch (button)
	{
		case 0:
			return (key_state[static_cast<int>(Key_Code::Mouse0)] & 0x80);
			break;
		case 1:
			return (key_state[static_cast<int>(Key_Code::Mouse1)] & 0x80);
			break;
		case 2:
			return (key_state[static_cast<int>(Key_Code::Mouse2)] & 0x80);
			break;
		default:
			break;
	}
	return false;
}

bool Input::Get_Mouse_Button_Down(int button)
{
	switch (button)
	{
		case 0:
			return (key_state[static_cast<int>(Key_Code::Mouse0)] & 0x80) && !(key_state_old[static_cast<int>(Key_Code::Mouse0)] & 0x80);
			break;
		case 1:
			return (key_state[static_cast<int>(Key_Code::Mouse1)] & 0x80) && !(key_state_old[static_cast<int>(Key_Code::Mouse1)] & 0x80);
			break;
		case 2:
			return (key_state[static_cast<int>(Key_Code::Mouse2)] & 0x80) && !(key_state_old[static_cast<int>(Key_Code::Mouse2)] & 0x80);
			break;
		default:
			break;
	}
	return false;
}

bool Input::Get_Mouse_Button_Up(int button)
{
	switch (button)
	{
		case 0:
			return !(key_state[static_cast<int>(Key_Code::Mouse0)] & 0x80) && (key_state_old[static_cast<int>(Key_Code::Mouse0)] & 0x80);
			break;
		case 1:
			return !(key_state[static_cast<int>(Key_Code::Mouse1)] & 0x80) && (key_state_old[static_cast<int>(Key_Code::Mouse1)] & 0x80);
			break;
		case 2:
			return !(key_state[static_cast<int>(Key_Code::Mouse2)] & 0x80) && (key_state_old[static_cast<int>(Key_Code::Mouse2)] & 0x80);
			break;
		default:
			break;
	}
	return false;
}

Vector2 Input::Get_Mouse_Position()
{
	return mouse_position;
}

Vector2 Input::Get_Mouse_Relative_Position()
{
	return mouse_position - mouse_position_old;
}

bool Input::Get_Pad_Button(Button_Code button, int pad_id)
{
	return controller_state[pad_id].result.buttons[static_cast<int>(button)];
}

bool Input::Get_Pad_Button_Down(Button_Code button, int pad_id)
{
	return controller_state[pad_id].result.buttons[static_cast<int>(button)] && !controller_state[pad_id].result_old.buttons[static_cast<int>(button)];
}

bool Input::Get_Pad_Button_Up(Button_Code button, int pad_id)
{
	return !controller_state[pad_id].result.buttons[static_cast<int>(button)] && controller_state[pad_id].result_old.buttons[static_cast<int>(button)];
}

float Input::Get_Pad_Trigger_Left(int pad_id)
{
	return controller_state[pad_id].result.left_trigger;
}

float Input::Get_Pad_Trigger_Right(int pad_id)
{
	return controller_state[pad_id].result.right_trigger;
}

Vector2 Input::Get_Pad_Axis_Left(int pad_id)
{
	return controller_state[pad_id].result.left_axis;
}

Vector2 Input::Get_Pad_Axis_Right(int pad_id)
{
	return controller_state[pad_id].result.right_axis;
}