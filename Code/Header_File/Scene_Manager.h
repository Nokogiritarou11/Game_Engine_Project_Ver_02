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

	class Scene_Manager
	{
	public:
		Scene_Manager();

		void Load_Scene(const std::string& scene_path);	  //指定した不シーンファイルをロードし、切り替える
		std::unique_ptr<Scene>& Get_Active_Scene(); //現在アクティブなシーンを返す

		bool run = false;   //再生中か
		bool pause = false; //ポーズ中か

	private:
		friend class Editor;
		friend class Engine;
		friend class BulletPhysics_Manager;

		std::unique_ptr<Scene> CreateScene_From_File();
		std::unique_ptr<Scene> CreateScene_From_File(std::string file_path);

		void Exit() const;
		void Update();
		void Create_Scene_Default(const std::string& file_path, const std::string& file_name);
		void Save_Scene(const std::string& save_path);
		void Save_Settings();
		void Start_Debug_Scene();
		void End_Debug_Scene();

		std::unique_ptr<Project_Settings> settings;
		std::unique_ptr<Scene> active_scene;
		std::unique_ptr<Scene> behind_scene;
		std::string behind_path;
		std::string last_save_path;

		bool load = false;
		std::string next_scene_path;
	};
}