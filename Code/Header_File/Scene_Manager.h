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

	std::shared_ptr<Scene> CreateScene_FromFile();
	std::shared_ptr<Scene> CreateScene_FromFile(std::string file_path);

	void CreateScene_Default(std::string file_path, std::string file_name);

	void Update();

	void SaveScene(std::string Save_Path);
	void Start_DebugScene();
	void End_DebugScene();
	static void LoadScene(std::string Scene_Path);
	static std::shared_ptr<Scene> Get_Active_Scene();

	std::string Last_Save_Path = "";
	bool Run = false;
	bool Pause = false;

private:

	void Initialize_Scene(std::weak_ptr<Scene> s);

	//std::list<std::shared_ptr<Scene>> Scene_List;

	static std::shared_ptr<Scene> Active_Scene;
	std::shared_ptr<Scene> Behind_Scene;

	static bool Load;
	static std::string Next_Scene_Path;
};
