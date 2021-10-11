#include "Resources.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "GameObject.h"
#include <sstream>
#include <fstream>
using namespace std;
using namespace BeastEngine;

void Resources::Create_Prefab(shared_ptr<GameObject> gameobject)
{
	string path = "Assets\\Prefab\\" + gameobject->name + ".prefab";

	const auto& parent = gameobject->transform->Get_Parent().lock();
	if (parent)
	{
		gameobject->transform->Set_Parent(nullptr);
	}
	{
		ifstream in_bin(path, ios::binary);
		if (in_bin.is_open())
		{
			for (int i = 1; ; ++i)
			{
				string _path = "Assets\\Prefab\\" + gameobject->name + "_" + to_string(i) + ".prefab";
				ifstream in_bin(_path, ios::binary);
				if (!in_bin.is_open())
				{
					path = _path;
					break;
				}
			}
		}
	}
	{
		ofstream ss(path.c_str(), ios::binary);
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(gameobject);
	}

	if (parent)
	{
		gameobject->transform->Set_Parent(parent);
	}

	//Debug::Log(u8"Assets\\Prefab\\以下に" + gameobject->name + u8"をプレハブ化しました");
}

shared_ptr<GameObject> Resources::Load_Prefab(const string& file_path)
{
	if (const ifstream in_bin(file_path, ios::binary); in_bin.is_open())
	{
		shared_ptr<GameObject> obj;
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(obj);

		Instance_child(obj);
		obj->Initialize();
		return obj;
	}

	return nullptr;
}

void Resources::Instance_child(shared_ptr<GameObject> gameobject)
{
	Engine::scene_manager->Get_Active_Scene()->gameobject_list.emplace_back(gameobject);
	for (int i = 0; i < gameobject->transform->Get_Child_Count(); ++i)
	{
		Instance_child(gameobject->transform->Get_Child(i).lock()->gameobject);
	}
}