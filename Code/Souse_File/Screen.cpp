#include "Screen.h"

#if _DEBUG
#include "Engine.h"
#include "Debug_UI.h"

#else
#include "DxSystem.h"

#endif
using namespace std;

int Screen::Get_Width()
{
#if _DEBUG
	return static_cast<int>(Engine::debug_ui->Game_View_Size.x);
#else
	RECT rect;
	GetClientRect(DxSystem::hwnd, &rect);
	return rect.right;
#endif
}

int Screen::Get_Height()
{
#if _DEBUG
	return static_cast<int>(Engine::debug_ui->Game_View_Size.y);
#else
	RECT rect;
	GetClientRect(DxSystem::hwnd, &rect);
	return rect.bottom;
#endif
}