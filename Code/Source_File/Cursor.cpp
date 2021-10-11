#include "Cursor.h"
#include "Input.h"
#include <windows.h>
#include "Screen.h"
#include "DxSystem.h"

#if _DEBUG
#include "Engine.h"
#include "Editor.h"
#endif

using namespace std;
using namespace BeastEngine;

bool Cursor::visible = true;
bool Cursor::window_focus = true;
Vector2 Cursor::lock_position = { 0,0 };
CursorLock_Mode Cursor::cursor_lock_mode = CursorLock_Mode::None;

void Cursor::Update()
{
	if (window_focus)
	{
		if (cursor_lock_mode == CursorLock_Mode::Locked)
		{
#if _DEBUG
			if (!Engine::editor->render_cursor)
			{
				Set_Cursor_Visible(false);
				lock_position = Vector2(Engine::editor->game_view_center_position.x, Engine::editor->game_view_center_position.y);
				SetCursorPos(static_cast<int>(lock_position.x), static_cast<int>(lock_position.y));
			}
			else
			{
				Set_Cursor_Visible(true);
			}
#else
			Vector2 m_pos = Input::Get_Mouse_Position();
			if (m_pos.x >= 0 && m_pos.x < Screen::Get_Width() && m_pos.y >= 0 && m_pos.y < Screen::Get_Height())
			{
				Set_Cursor_Visible(false);
			}
			else
			{
				Set_Cursor_Visible(true);
			}
			RECT rect;
			GetWindowRect(DxSystem::hwnd, &rect);
			lock_position = { static_cast<float>(rect.left) + Screen::Get_Width() / 2, static_cast<float>(rect.top) + Screen::Get_Height() / 2 };
			SetCursorPos(static_cast<int>(lock_position.x), static_cast<int>(lock_position.y));
#endif
		}
		else
		{
			if (!visible)
			{
				const Vector2 m_pos = Input::Get_Mouse_Position();
				if (m_pos.x >= 0 && m_pos.x < static_cast<float>(Screen::Get_Width()) && m_pos.y >= 0 && m_pos.y < static_cast<float>(Screen::Get_Height()))
				{
					Set_Cursor_Visible(false);
				}
				else
				{
					Set_Cursor_Visible(true);
				}
			}
			else
			{
				Set_Cursor_Visible(true);
			}
		}
	}
	else
	{
		Set_Cursor_Visible(true);
	}
}

void Cursor::Set_Cursor_Visible(bool value)
{
	if (value)
	{
		if (!is_visible)
		{
			while (ShowCursor(true) < 0) {}
			is_visible = true;
		}
	}
	else
	{
		if (is_visible)
		{
			while (ShowCursor(false) > 0) {}
			is_visible = false;
		}
	}
}