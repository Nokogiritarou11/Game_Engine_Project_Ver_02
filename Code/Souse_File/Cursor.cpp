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
bool Cursor::Window_Focus = true;
Vector2 Cursor::Lock_Pos = { 0,0 };
CursorLockMode Cursor::lockState = CursorLockMode::None;

void Cursor::Update()
{
	if (Window_Focus)
	{
		if (lockState == CursorLockMode::Locked)
		{
#if _DEBUG
			if (!Engine::editor->Render_Cursor)
			{
				Set_Cursor_Visible(false);
				Lock_Pos = { Engine::editor->Game_View_CenterPos.x,Engine::editor->Game_View_CenterPos.y };
				SetCursorPos(static_cast<int>(Lock_Pos.x), static_cast<int>(Lock_Pos.y));
			}
			else
			{
				Set_Cursor_Visible(true);
			}
#else
			Vector2 m_pos = Input::GetMousePosition();
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
			Lock_Pos = { static_cast<float>(rect.left) + Screen::Get_Width() / 2, static_cast<float>(rect.top) + Screen::Get_Height() / 2 };
			SetCursorPos(static_cast<int>(Lock_Pos.x), static_cast<int>(Lock_Pos.y));
#endif
		}
		else
		{
			if (!visible)
			{
				Vector2 m_pos = Input::GetMousePosition();
				if (m_pos.x >= 0 && m_pos.x < Screen::Get_Width() && m_pos.y >= 0 && m_pos.y < Screen::Get_Height())
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
		if (!Is_Visible)
		{
			while (ShowCursor(true) < 0);
			Is_Visible = true;
		}
	}
	else
	{
		if (Is_Visible)
		{
			while (ShowCursor(false) > 0);
			Is_Visible = false;
		}
	}
}