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
	bool CreateScene_FromFile();
	void CreateScene_Default(std::string new_name);

	/*
	//std::shared_ptr<GameObject> Instance_GameObject(std::string name);
	//void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);

	//std::weak_ptr<GameObject> Find(std::string Name);
	//std::weak_ptr<GameObject> FindWithTag(std::string Tag);
	*/
	
	void Update();

	void SaveScene(std::string Save_Path);
	static void LoadScene(std::string Scene_Name);
	static std::shared_ptr<Scene> Get_Active_Scene();

	std::string Last_Save_Path = "";
	bool Run = false;

private:

	void Initialize_Scene(std::weak_ptr<Scene> s);

	std::list<std::shared_ptr<Scene>> Scene_List;

	static std::shared_ptr<Scene> Active_Scene;

	static bool Load;
	static std::string Next_Scene_Name;
};
