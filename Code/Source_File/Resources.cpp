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
	//パスは基本固定
	string path = "Assets\\Prefab\\" + gameobject->name + ".prefab";

	//自身以下だけプレハブ化したいので一時的に親を外す
	const auto& parent = gameobject->transform->Get_Parent().lock();
	if (parent)
	{
		gameobject->transform->Set_Parent(nullptr);
	}

	//同名のファイルが既に存在しないかチェック
	if (ifstream in_bin(path, ios::binary); in_bin.is_open())
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

	//書き出し
	{
		ofstream ss(path.c_str(), ios::binary);
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(gameobject);
	}

	//解除していた親を戻す
	if (parent)
	{
		gameobject->transform->Set_Parent(parent);
	}

	Debug::Log(u8"Assets\\Prefab\\以下に" + gameobject->name + u8"をプレハブ化しました");
}

shared_ptr<GameObject> Resources::Load_Prefab(const string& file_path)
{
	if (const ifstream in_bin(file_path, ios::binary); in_bin.is_open())
	{
		shared_ptr<GameObject> obj;
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binary_input_archive(bin_s_stream);
		binary_input_archive(obj);

		//生成後の初期化処理
		Instance_Child(obj);
		obj->Initialize();
		return obj;
	}

	return nullptr;
}

void Resources::Instance_Child(shared_ptr<GameObject> gameobject)
{
	//子をマネージャーへ登録する再帰処理
	Engine::scene_manager->Get_Active_Scene()->gameobject_list.emplace_back(gameobject);
	for (int i = 0; i < gameobject->transform->Get_Child_Count(); ++i)
	{
		Instance_Child(gameobject->transform->Get_Child(i).lock()->gameobject);
	}
}