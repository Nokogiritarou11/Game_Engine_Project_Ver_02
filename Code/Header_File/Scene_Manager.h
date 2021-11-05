#pragma once
#include <string>
#include <memory>

namespace BeastEngine
{
	struct Project_Settings;
	class Scene;
	class Editor;
	class Engine;
	class BulletPhysics_Manager;

	//シーン管理クラス
	class Scene_Manager
	{
	public:
		Scene_Manager();

		void Load_Scene(const std::string& scene_path);	//ファイルパスからシーンファイルをロードし、切り替える
		std::unique_ptr<Scene>& Get_Active_Scene();     //現在アクティブなシーンを取得する

		bool run = false;   //再生中か
		bool pause = false; //ポーズ中か

	private:
		std::unique_ptr<Scene> CreateScene_From_File();                      //シーンをファイルダイアログから読み込む
		std::unique_ptr<Scene> CreateScene_From_File(std::string file_path); //ファイルパスからシーンを詠み込む

		void Exit() const; //シーンを開放する
		void Update();     //更新
		void Create_Scene_Default(const std::string& file_path, const std::string& file_name); //シーンを新規作成する
		void Save_Scene(const std::string& save_path); //シーンを保存する
		void Save_Settings();     //シーン設定を保存する
		void Start_Debug_Scene(); //デバッグ用のシーンを再生開始する
		void End_Debug_Scene();   //デバッグ用のシーンの再生を終了する

		std::unique_ptr<Project_Settings> settings; //シーン設定
		std::unique_ptr<Scene> active_scene;        //現在アクティブなシーン
		std::unique_ptr<Scene> behind_scene;        //デバッグ時に待避させているシーン
		std::string behind_path;                    //デバッグシーン再生時に読み込むためのパス
		std::string last_save_path;                 //上書きセーブ用ファイルパス

		bool load = false; //次のシーンをロードするか
		std::string next_scene_path; //次に読み込むシーンのファイルパス

		friend class Editor;
		friend class Engine;
		friend class BulletPhysics_Manager;
	};
}