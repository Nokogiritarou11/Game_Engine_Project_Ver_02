#pragma once
#include "Scene.h"
#include <iostream>

class Debug_UI
{
public:

	Debug_UI();
	~Debug_UI();

	void Update(std::shared_ptr<Scene> scene);
	void Render();

	bool Draw_Debug_UI = true;
	void Print_Log(std::string log);
	static std::string Get_Open_File_Name();
	static std::string Get_Save_File_Name();
private:

	char* Font_Name = "Font/mplus-1p-medium.ttf";
	float Font_Size_Pixels = 30.0f;
	float Font_Size = 0.6f;
	float UI_Size = 0.6f;
	vector<string> Debug_Log = {};
	bool Debug_Log_Changed = false;
	weak_ptr<GameObject> Active_Object;

	void Debug_Log_Render();
	void Hierarchy_Render(std::shared_ptr<Scene> scene);
	void Inspector_Render();
	void ScenePlayer_Render();

	void Scene_File_Menu_Render();
	void GameObject_List_Render(std::shared_ptr<Scene> scene);
};