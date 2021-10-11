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
		static void Render();

		void Print_Log(std::string log);
		std::shared_ptr<GameObject> Get_Drag_Object();

		Vector2 game_view_size;
		Vector2 game_view_position;
		Vector2 game_view_center_position;
		bool render_cursor = true;

		//デバッグカメラ関連
		std::shared_ptr<Transform> debug_camera_transform;
		std::shared_ptr<Camera> debug_camera;

	private:
		//ImGui設定
		char* font_name = "Default_Assets/Font/mplus-1p-medium.ttf";
		float font_size_pixels = 30.0f;
		float font_size = 0.6f;
		float ui_size = 0.6f;

		std::vector<std::string> debug_log = {};
		bool debug_log_changed = false;
		std::weak_ptr<GameObject> active_object;
		std::weak_ptr<GameObject> active_object_old;
		std::weak_ptr<GameObject> dragging_object;
		bool select_object{};

		std::weak_ptr<Animator_Controller> controller;

		Vector2 game_view_render_size;
		float game_view_aspect{};

		void Main_Window_Render();
		void Debug_Log_Render();
		void Hierarchy_Render();
		void Inspector_Render();
		void ScenePlayer_Render();
		void SceneView_Render() const;
		void GameView_Render();
		void Controller_Render() const;

		void MenuBar_Render();
		void Scene_File_Menu_Render();
		void Scene_Setting_Menu_Render() const;
		void GameObject_Tree_Render(const std::shared_ptr<GameObject>& obj, int flag);
		void GameObject_DragMenu_Render(const std::shared_ptr<GameObject>& obj);

		void ShortCut_Check();
		void Debug_Camera_Update() const;

		void Select_Reset();
		void Activate_Select_Object(const std::shared_ptr<GameObject>& obj);
		static void Set_Debug_Draw(bool value, const std::shared_ptr<GameObject>& obj);
	};
}