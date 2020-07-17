#pragma once
#include "Scene.h"
#include <iostream>

class Debug_UI
{
public:

	Debug_UI() {};
	~Debug_UI();

	static void Initialize();
	static void Update(std::shared_ptr<Scene> scene);
	static void Render();

	static bool Draw_Debug_UI;
	static void Print_Log(std::string log);
	static std::string Get_Open_File_Name();
private:

	static char* Font_Name;
	static float Font_Size_Pixels;
	static float Font_Size;
	static float UI_Size;
	static std::vector<std::string> Debug_Log;
	static bool Debug_Log_Changed;
	static std::weak_ptr<GameObject> Active_Object;

	static void Debug_Log_Render();
	static void Hierarchy_Render(std::shared_ptr<Scene> scene);
	static void Inspector_Render();

	static void Scene_File_Menu_Render();
	static void GameObject_List_Render(std::shared_ptr<Scene> scene);
};