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

		bool Run = false;   //再生中か
		bool Pause = false; //ポーズ中か

	private:

		friend class Editor;
		friend class Engine;

		std::unique_ptr<BeastEngine::Scene> CreateScene_FromFile();
		std::unique_ptr<BeastEngine::Scene> CreateScene_FromFile(std::string file_path);

		void Update();
		void CreateScene_Default(std::string file_path, std::string file_name);
		void SaveScene(std::string Save_Path);
		void Start_DebugScene();
		void End_DebugScene();

		void Release();

		static std::unique_ptr<BeastEngine::Scene> Active_Scene;
		std::unique_ptr<BeastEngine::Scene> Behind_Scene;
		std::string Behind_Path = "";
		std::string Last_Save_Path = "";

		static bool Load;
		static std::string Next_Scene_Path;
	};
}