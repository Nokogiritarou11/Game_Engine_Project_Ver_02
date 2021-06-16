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

		static void LoadScene(std::string Scene_Path);	  //�w�肵���s�V�[���t�@�C�������[�h���A�؂�ւ���
		static std::unique_ptr<BeastEngine::Scene>& Get_Active_Scene(); //���݃A�N�e�B�u�ȃV�[����Ԃ�

		bool Run = false;   //�Đ�����
		bool Pause = false; //�|�[�Y����

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