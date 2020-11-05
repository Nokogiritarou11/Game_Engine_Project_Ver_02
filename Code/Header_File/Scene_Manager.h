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

	static void LoadScene(std::string Scene_Path);	  //指定した不シーンファイルをロードし、切り替える
	static std::shared_ptr<Scene> Get_Active_Scene(); //現在アクティブなシーンを返す

	bool Run = false;   //再生中か
	bool Pause = false; //ポーズ中か

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
