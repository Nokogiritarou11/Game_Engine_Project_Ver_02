#pragma once
#include <iostream>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"
#include "IconsFontAwesome4.h"
#include "Scene.h"

class Debug_UI
{
public:

	Debug_UI() {};
	~Debug_UI();

	static void Initialize(HWND hWnd);
	static void Update(shared_ptr<Scene> scene, std::string name);
	static void Render();

	static bool Draw_Debug_UI;
	static void Print_Log(std::string log);
private:

	static char* Font_Name;
	static float Font_Size_Pixels;
	static float Font_Size;
	static float UI_Size;
	static std::vector<std::string> Debug_Log;
	static bool Debug_Log_Changed;
	static weak_ptr<GameObject> Active_Object;

	static void Debug_Log_Render();
	static void Hierarchy_Render(shared_ptr<Scene> scene, std::string name);
	static void Inspector_Render();
};