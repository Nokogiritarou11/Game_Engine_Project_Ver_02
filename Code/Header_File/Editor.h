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

	//ImGui描画管理クラス
	class Editor
	{
	public:
		Editor();
		~Editor();

		void Update();
		static void Render();

		void Print_Log(std::string log);
		[[nodiscard]] std::shared_ptr<GameObject> Get_Drag_Object() const;

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

		void Main_Window_Render();                //ドッキング用の全画面ウィンドウを描画する
		void Debug_Log_Render();                  //デバッグログ描画
		void Hierarchy_Render();                  //ヒエラルキー描画
		void Inspector_Render();                  //インスペクタ描画
		void ScenePlayer_Render();                //シーン再生ボタン描画
		void SceneView_Render() const;            //シーンビュー描画
		void GameView_Render();                   //ゲームビュー描画
		void Animation_Controller_Render() const; //アニメーションコントローラー描画

		void MenuBar_Render(); //メニューバー描画
		void Scene_File_Menu_Render(); //シーン関連メニュー描画
		void Scene_Setting_Menu_Render() const; //シーン設定メニュー描画
		void GameObject_Tree_Render(const std::shared_ptr<GameObject>& obj, int flag); //インスペクタ内でのゲームオブジェクト表示用再帰関数
		void GameObject_DragMenu_Render(const std::shared_ptr<GameObject>& obj); //D&Dした際のメニューウィンドウ描画

		void ShortCut_Check();            //ショートカットキーのチェック
		void Debug_Camera_Update() const; //シーンビューカメラの操作

		void Select_Reset(); //選択状態解除時の後始末
		void Activate_Select_Object(const std::shared_ptr<GameObject>& obj); //ゲームオブジェクト選択時の初期化
		static void Set_Debug_Draw(bool value, const std::shared_ptr<GameObject>& obj); //選択時のデバッグ描画登録用再帰関数
	};
}