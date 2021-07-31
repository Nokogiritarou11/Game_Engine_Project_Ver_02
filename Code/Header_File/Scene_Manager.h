#pragma once
#include <stdio.h>
#include <string>
#include <list>
#include <memory>

namespace BeastEngine
{
	struct Project_Settings;
	class Scene;
	class Editor;
	class Engine;
	class BulletPhysics_Manager;

	class Scene_Manager
	{
	public:
		Scene_Manager();

		void Load_Scene(std::string Scene_Path);	  //指定した不シーンファイルをロードし、切り替える
		std::unique_ptr<BeastEngine::Scene>& Get_Active_Scene(); //現在アクティブなシーンを返す

		bool run = false;   //再生中か
		bool pause = false; //ポーズ中か

	private:
		friend class BeastEngine::Editor;
		friend class BeastEngine::Engine;
		friend class BeastEngine::BulletPhysics_Manager;

		std::unique_ptr<BeastEngine::Scene> CreateScene_From_File();
		std::unique_ptr<BeastEngine::Scene> CreateScene_From_File(std::string file_path);

		void Exit();
		void Update();
		void Create_Scene_Default(std::string file_path, std::string file_name);
		void Save_Scene(std::string Save_Path);
		void Save_Settings();
		void Start_Debug_Scene();
		void End_Debug_Scene();

		std::unique_ptr<BeastEngine::Project_Settings> settings;
		std::unique_ptr<BeastEngine::Scene> active_scene;
		std::unique_ptr<BeastEngine::Scene> behind_scene;
		std::string behind_path = "";
		std::string last_save_path = "";

		bool load;
		std::string next_scene_path;
	};
}