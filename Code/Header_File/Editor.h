#pragma once
#include "Scene.h"
#include <iostream>

class Editor
{
public:
	Editor();
	~Editor();

	void Update(const std::unique_ptr<Scene>& scene);
	void Render();

	void Print_Log(std::string log);

	Vector2 Game_View_Size;
	Vector2 Game_View_Pos;
	Vector2 Game_View_CenterPos;
	bool Render_Cursor = true;

private:
	char* Font_Name = "Default_Resource/Font/mplus-1p-medium.ttf";
	float Font_Size_Pixels = 30.0f;
	float Font_Size = 0.6f;
	float UI_Size = 0.6f;
	std::vector<std::string> Debug_Log = {};
	bool Debug_Log_Changed = false;
	std::weak_ptr<GameObject> Active_Object;
	std::weak_ptr<GameObject> Active_Object_Old;

	std::weak_ptr<GameObject> Drag_Object;

	Matrix Debug_Camera_V;
	Matrix Debug_Camera_P;
	std::shared_ptr<Transform> Debug_Camera_Transform;
	float fov_y;
	float aspect;
	float near_z;
	float far_z;

	void Main_Window_Render();
	void Debug_Log_Render();
	void Hierarchy_Render(const std::unique_ptr<Scene>& scene);
	void Inspector_Render();
	void ScenePlayer_Render();
	void SceneView_Render();
	void GameView_Render();
	void FileResource_Render();

	void MenuBar_Render();
	void Scene_File_Menu_Render();
	void GameObject_Tree_Render(int& ID, const std::shared_ptr<GameObject>& obj, int& selecting, int flag, bool& Item_Clicked);
	void GameObject_DragMenu_Render(const std::shared_ptr<GameObject>& obj);

	void ShortCut_Check();
	void Debug_Camera_Update();
};