#pragma once
#include <stdio.h>
#include <string>
#include <list>
#include <memory>

namespace BeastEngine
{
	class Scene;

	class Scene_Manager
	{
	public:

		static void LoadScene(std::string Scene_Path);	  //指定した不シーンファイルをロードし、切り替える
		static std::unique_ptr<BeastEngine::Scene>& Get_Active_Scene(); //現在アクティブなシーンを返す

		bool run = false;   //再生中か
		bool pause = false; //ポーズ中か

	private:

		friend class Editor;
		friend class Engine;

		std::unique_ptr<BeastEngine::Scene> CreateScene_From_File();
		std::unique_ptr<BeastEngine::Scene> CreateScene_From_File(std::string file_path);

		void Update();
		void Create_Scene_Default(std::string file_path, std::string file_name);
		void Save_Scene(std::string Save_Path);
		void Start_Debug_Scene();
		void End_Debug_Scene();

		void Release();

		static std::unique_ptr<BeastEngine::Scene> active_scene;
		std::unique_ptr<BeastEngine::Scene> behind_scene;
		std::string behind_path = "";
		std::string last_save_path = "";

		static bool load;
		static std::string next_scene_path;
	};
}