#pragma once
#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include "Scene.h"
#include "GameObject.h"

class Scene_Manager
{
public:

	static void LoadScene(std::string Scene_Path);	  //�w�肵���s�V�[���t�@�C�������[�h���A�؂�ւ���
	static std::shared_ptr<Scene> Get_Active_Scene(); //���݃A�N�e�B�u�ȃV�[����Ԃ�

	bool Run = false;   //�Đ�����
	bool Pause = false; //�|�[�Y����

private:

	//std::list<std::shared_ptr<Scene>> Scene_List;

	friend class Debug_UI;
	friend class Engine;

	std::shared_ptr<Scene> CreateScene_FromFile();
	std::shared_ptr<Scene> CreateScene_FromFile(std::string file_path);

	void Update();
	void CreateScene_Default(std::string file_path, std::string file_name);
	void SaveScene(std::string Save_Path);
	void Start_DebugScene();
	void End_DebugScene();

	static std::shared_ptr<Scene> Active_Scene;
	std::shared_ptr<Scene> Behind_Scene;
	std::string Behind_Path = "";
	std::string Last_Save_Path = "";

	static bool Load;
	static std::string Next_Scene_Path;
};
