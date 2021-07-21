#include "Input.h"
#include "Cursor.h"
#include "DxSystem.h"

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

void Input::Update()
{

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
		mouse_position_old = { Cursor::Lock_Pos.x - static_cast<float>(rect.left) ,static_cast<float>(rect.bottom) - Cursor::Lock_Pos.y };
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

	memcpy(key_state_old, key_state, sizeof(key_state));
	if (!GetKeyboardState(key_state))
	{
		assert(false);
	}
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