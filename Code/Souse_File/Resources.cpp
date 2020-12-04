#include "Resources.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Debug.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

void Resources::Create_Prefab(shared_ptr<GameObject> gameobject)
{
	string path = "Resouces\\Prefab\\" + gameobject->name + ".prefab";

	weak_ptr<Transform> parent = gameobject->transform->Get_parent();
	if (!parent.expired())
	{
		gameobject->transform->Set_parent(nullptr);
	}
	{
		ifstream in_bin(path, ios::binary);
		if (in_bin.is_open())
		{
			for (int i = 1; ; ++i)
			{
				string _path = "Resouces\\Prefab\\" + gameobject->name + "_" + to_string(i) + ".prefab";
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
		{
			cereal::BinaryOutputArchive o_archive(ss);
			o_archive(gameobject);
		}
	}

	if (!parent.expired())
	{
		gameobject->transform->Set_parent(parent.lock());
	}

	Debug::Log(u8"Resouces\\Prefab\\ˆÈ‰º‚É" + gameobject->name + u8"‚ðƒvƒŒƒnƒu‰»‚µ‚Ü‚µ‚½");
}

shared_ptr<GameObject> Resources::Load_Prefab(string file_path)
{
	ifstream in_bin(file_path, ios::binary);
	if (in_bin.is_open())
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(obj);

		Instance_child(obj);
		obj->Initialize();
		return obj;
	}
	else
	{
		return nullptr;
	}

	return nullptr;
}

void Resources::Instance_child(shared_ptr<GameObject> gameobject)
{
	Engine::scene_manager->Get_Active_Scene()->gameObject_List.emplace_back(gameobject);
	for (int i = 0; i < gameobject->transform->childCount(); ++i)
	{
		Instance_child(gameobject->transform->GetChild(i).lock()->gameObject);
	}
}