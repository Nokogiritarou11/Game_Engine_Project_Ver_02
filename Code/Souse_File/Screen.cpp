#include "Screen.h"
using namespace BeastEngine;

#if _DEBUG
#include "Engine.h"
#include "Editor.h"

#else
#include "DxSystem.h"

#endif
using namespace std;

int Screen::Get_Width()
{
#if _DEBUG
	return static_cast<int>(Engine::editor->game_view_size.x);
#else
	RECT rect;
	GetClientRect(DxSystem::hwnd, &rect);
	return rect.right;
#endif
}

int Screen::Get_Height()
{
#if _DEBUG
	return static_cast<int>(Engine::editor->game_view_size.y);
#else
	RECT rect;
	GetClientRect(DxSystem::hwnd, &rect);
	return rect.bottom;
#endif
}