#pragma once
#include <string>
#include "Original_Math.h"

namespace BeastEngine
{
	class Scene;
	class GameObject;
	class Transform;
	class Camera;
	class Animator_Controller;

	class Editor
	{
	public:
		Editor();
		~Editor();

		void Update();
		void Render();

		void Print_Log(std::string log);

		BeastEngine::Vector2 game_view_size;
		BeastEngine::Vector2 game_view_position;
		BeastEngine::Vector2 game_view_center_position;
		bool render_cursor = true;

		//デバッグカメラ関連
		std::shared_ptr<BeastEngine::Transform> debug_camera_transform;
		std::shared_ptr<BeastEngine::Camera> debug_camera;

	private:
		//Imgui設定
		char* font_name = "Default_Resource/Font/mplus-1p-medium.ttf";
		float font_size_pixels = 30.0f;
		float font_size = 0.6f;
		float ui_size = 0.6f;

		std::vector<std::string> debug_log = {};
		bool debug_log_changed = false;
		std::weak_ptr<BeastEngine::GameObject> active_object;
		std::weak_ptr<BeastEngine::GameObject> active_object_old;
		std::weak_ptr<BeastEngine::GameObject> draging_object;

		std::weak_ptr<BeastEngine::Animator_Controller> controller;

		Vector2 game_view_render_size;
		float game_view_aspect;

		void Main_Window_Render();
		void Debug_Log_Render();
		void Hierarchy_Render();
		void Inspector_Render();
		void ScenePlayer_Render();
		void SceneView_Render();
		void GameView_Render();
		void Controller_Render();

		void MenuBar_Render();
		void Scene_File_Menu_Render();
		void Scene_Setting_Menu_Render();
		void GameObject_Tree_Render(int& ID, const std::shared_ptr<BeastEngine::GameObject>& obj, int& selecting, int flag, bool& Item_Clicked);
		void GameObject_DragMenu_Render(const std::shared_ptr<BeastEngine::GameObject>& obj);

		void ShortCut_Check();
		void Debug_Camera_Update();

		void Select_Reset();
		void Set_Debug_Draw(bool value, const std::shared_ptr<BeastEngine::GameObject>& obj);
		void Activate_Select_Object(const std::shared_ptr<BeastEngine::GameObject>& obj);
	};
}