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

	static std::shared_ptr<GameObject> Instance_GameObject(std::string name);
	static void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);

	static std::weak_ptr<GameObject> Find(std::string Name);
	static std::weak_ptr<GameObject> FindWithTag(std::string Tag);

	static void Update();

private:

	struct Scene_Data
	{
		std::string Name;
		std::shared_ptr<Scene> Scene_ptr;
	};

	static std::list<Scene_Data> Scene_List;

	static std::weak_ptr<Scene> Active_Scene;
	static std::string Active_Scene_Name;

	static bool Load;
	static std::string Next_Scene_Name;
};
