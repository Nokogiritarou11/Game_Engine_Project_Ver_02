#include "Input.h"
#include "Cursor.h"
#include "DxSystem.h"

#if _DEBUG
#include "Engine.h"
#include "Editor.h"
#endif

using namespace std;

BYTE Input::key_state[256] = { 0x00 };
BYTE Input::key_state_old[256] = { 0x00 };
Vector2 Input::mousePosition = { 0,0 };
Vector2 Input::mousePosition_old = { 0,0 };

void Input::Update()
{

#if _DEBUG
#else
	RECT rect;
	GetWindowRect(DxSystem::hwnd, &rect);
#endif

	if (Cursor::lockState == CursorLockMode::Locked)
	{
#if _DEBUG
		mousePosition_old = { Cursor::Lock_Pos.x - Engine::editor->Game_View_Pos.x,Engine::editor->Game_View_Pos.y - Cursor::Lock_Pos.y };
#else
		mousePosition_old = { Cursor::Lock_Pos.x - static_cast<float>(rect.left) ,static_cast<float>(rect.bottom) - Cursor::Lock_Pos.y };
#endif
	}
	else
	{
		mousePosition_old = mousePosition;
	}

	POINT mouse_p;
	GetCursorPos(&mouse_p);
#if _DEBUG
	mousePosition = { static_cast<float>(mouse_p.x) - Engine::editor->Game_View_Pos.x, Engine::editor->Game_View_Pos.y - static_cast<float>(mouse_p.y) };
#else
	mousePosition = { static_cast<float>(mouse_p.x) - static_cast<float>(rect.left),static_cast<float>(rect.bottom) - static_cast<float>(mouse_p.y) };
#endif

	memcpy(key_state_old, key_state, sizeof(key_state));
	if (!GetKeyboardState(key_state))
	{
		assert(false);
	}
}

bool Input::GetKey(KeyCode key)
{
	return (key_state[static_cast<int>(key)] & 0x80);
}

bool Input::GetKeyDown(KeyCode key)
{
	return (key_state[static_cast<int>(key)] & 0x80) && !(key_state_old[static_cast<int>(key)] & 0x80);
}

bool Input::GetKeyUp(KeyCode key)
{
	return !(key_state[static_cast<int>(key)] & 0x80) && (key_state_old[static_cast<int>(key)] & 0x80);
}

bool Input::GetMouseButton(int button)
{
	switch (button)
	{
		case 0:
			return (key_state[static_cast<int>(KeyCode::Mouse0)] & 0x80);
			break;
		case 1:
			return (key_state[static_cast<int>(KeyCode::Mouse1)] & 0x80);
			break;
		case 2:
			return (key_state[static_cast<int>(KeyCode::Mouse2)] & 0x80);
			break;
		default:
			break;
	}
	return false;
}

bool Input::GetMouseButtonDown(int button)
{
	switch (button)
	{
		case 0:
			return (key_state[static_cast<int>(KeyCode::Mouse0)] & 0x80) && !(key_state_old[static_cast<int>(KeyCode::Mouse0)] & 0x80);
			break;
		case 1:
			return (key_state[static_cast<int>(KeyCode::Mouse1)] & 0x80) && !(key_state_old[static_cast<int>(KeyCode::Mouse1)] & 0x80);
			break;
		case 2:
			return (key_state[static_cast<int>(KeyCode::Mouse2)] & 0x80) && !(key_state_old[static_cast<int>(KeyCode::Mouse2)] & 0x80);
			break;
		default:
			break;
	}
	return false;
}

bool Input::GetMouseButtonUp(int button)
{
	switch (button)
	{
		case 0:
			return !(key_state[static_cast<int>(KeyCode::Mouse0)] & 0x80) && (key_state_old[static_cast<int>(KeyCode::Mouse0)] & 0x80);
			break;
		case 1:
			return !(key_state[static_cast<int>(KeyCode::Mouse1)] & 0x80) && (key_state_old[static_cast<int>(KeyCode::Mouse1)] & 0x80);
			break;
		case 2:
			return !(key_state[static_cast<int>(KeyCode::Mouse2)] & 0x80) && (key_state_old[static_cast<int>(KeyCode::Mouse2)] & 0x80);
			break;
		default:
			break;
	}
	return false;
}

Vector2 Input::GetMousePosition()
{
	return mousePosition;
}

Vector2 Input::GetMouseRelativePosition()
{
	return mousePosition - mousePosition_old;
}