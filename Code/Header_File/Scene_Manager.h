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
	static void CreateScene(std::shared_ptr<Scene> Scene_Class, std::string Scene_Name);
	static void Set_StartScene(std::string Scene_Name);
	static void LoadScene(std::string Scene_Name);

	static void Instance_GameObject(std::shared_ptr<GameObject> gameObject);
	static void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);

	static void Update();

private:

	struct Scene_Data
	{
		std::string Name;
		std::shared_ptr<Scene> Scene_ptr;
	};

	static std::weak_ptr<Scene> Active_Scene;
	static std::list<Scene_Data> Scene_List;
	static bool Load;
	static std::string Next_Scene_Name;

};
