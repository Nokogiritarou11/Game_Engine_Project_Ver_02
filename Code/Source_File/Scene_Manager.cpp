#include <fstream>
#include <sstream>
#include <functional>
#include "Scene_Manager.h"
#include "Animator_Manager.h"
#include "Light_Manager.h"
#include "Transform.h"
#include "System_Function.h"
#include "Engine.h"
#include "Scene.h"
#include "Render_Manager.h"
#include "Audio_Manager.h"
#include "Particle_Manager.h"
#include "BulletPhysics_Manager.h"
#include "Project_Settings.h"
#include "Shadow_Manager.h"
using namespace std;
using namespace BeastEngine;

Scene_Manager::Scene_Manager()
{
	//設定ファイルを読み込む
	if (ifstream in_bin("Default_Assets\\System\\settings.bin", ios::binary); in_bin.is_open())
	{
		unique_ptr<Project_Settings> set;
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binary_input_archive(bin_s_stream);
		binary_input_archive(set);
		settings = move(set);
	}
	else
	{
		//なければ作成
		settings = make_unique<Project_Settings>();
		settings->tag.emplace_back("Default");
		settings->layer[0] = "Default";
		settings->layer_mask.fill(-1);
		Save_Settings();
	}
	Engine::shadow_manager->shadow_bias = settings->shadow_bias;
	Engine::shadow_manager->shadow_distance = settings->shadow_distance;
}

void Scene_Manager::Exit() const
{
	if (active_scene) active_scene->Reset();
}

unique_ptr<Scene> Scene_Manager::CreateScene_From_File()
{
	//ダイアログからシーンファイルを選択し読み込む
	if (const string path = System_Function::Get_Open_File_Name("bin", "\\Assets\\Scene"); !path.empty())
	{
		if (const ifstream in_bin(path, ios::binary); in_bin.is_open())
		{
			unique_ptr<Scene> New_Scene;
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(New_Scene);

			last_save_path = path;
			return move(New_Scene);
		}
	}

	return nullptr;
}

unique_ptr<Scene> Scene_Manager::CreateScene_From_File(const std::string file_path)
{
	//パスからシーンファイルを読み込む
	if (const ifstream in_bin(file_path, ios::binary); in_bin.is_open())
	{
		unique_ptr<Scene> New_Scene;
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binary_input_archive(bin_s_stream);
		binary_input_archive(New_Scene);

		last_save_path = file_path;
		return move(New_Scene);
	}

	return nullptr;
}

void Scene_Manager::Create_Scene_Default(const string& file_path, const string& file_name)
{
	//デフォルトのシーン(カメラとライトのみ)を作成する
	auto new_scene = make_unique<Scene>();
	new_scene->name = file_name;

	if (active_scene)
	{
		active_scene->Reset();
	}
	active_scene = move(new_scene);

	//ライト
	const auto& directional_light = active_scene->Instance_GameObject(u8"Directional_Light");
	directional_light->Add_Component<Light>();
	directional_light->transform->Set_Position(0, 50, 0);
	directional_light->transform->Set_Euler_Angles(90, 0, 0);

	//カメラ
	const auto& camera = active_scene->Instance_GameObject(u8"Main_Camera");
	camera->Add_Component<Camera>();
	camera->transform->Set_Position(-100, 80, -100);
	camera->transform->Set_Euler_Angles(30, 45, 0);

	//書き出し
	{
		ofstream ss(file_path.c_str(), ios::binary);
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(active_scene);
		last_save_path = file_path;
	}

	Load_Scene(file_path);
}

void Scene_Manager::Save_Scene(const string& save_path)
{
	const int path_i = save_path.find_last_of("\\") + 1;
	const int ext_i = save_path.find_last_of(".");
	const string filename = save_path.substr(path_i, ext_i - path_i); //ファイル名
	active_scene->name = filename;

	{
		ofstream ss(save_path.c_str(), ios::binary);
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(active_scene);
		last_save_path = save_path;
	}
}

void Scene_Manager::Save_Settings()
{
	ofstream ss("Default_Assets\\System\\settings.bin", ios::binary);
	cereal::BinaryOutputArchive o_archive(ss);
	o_archive(settings);
}

void Scene_Manager::Start_Debug_Scene()
{
	//デバッグ用に現在のシーンを逃し、そのコピーを保存してから読み込みなおす
	behind_scene = move(active_scene);
	behind_path = last_save_path;
	{
		ofstream save("Default_Assets\\System\\Debug_Scene.bin", ios::binary);
		cereal::BinaryOutputArchive out_archive(save);
		out_archive(behind_scene);
	}
	Load_Scene("Default_Assets\\System\\Debug_Scene.bin");
}

void Scene_Manager::End_Debug_Scene()
{
	//デバッグシーンの登録されたものを削除する
	Engine::animator_manager->Reset();
	Engine::render_manager->Reset();
	Engine::light_manager->Reset();
	Engine::particle_manager->Reset();
	Engine::audio_manager->Reset();

	//逃していたシーンをもとに戻す
	active_scene->Reset();
	active_scene = move(behind_scene);
	last_save_path = behind_path;
	active_scene->Initialize();
}

void Scene_Manager::Load_Scene(const string& scene_path)
{
	Engine::scene_manager->load = true;
	Engine::scene_manager->next_scene_path = scene_path;
}

void Scene_Manager::Update()
{
	if (load)
	{
		//シーンの切り替えと初期化
		Engine::animator_manager->Reset();
		Engine::render_manager->Reset();
		Engine::light_manager->Reset();
		Engine::particle_manager->Reset();
		Engine::audio_manager->Reset();
		Engine::bulletphysics_manager->Reset();

		if (active_scene)
		{
			active_scene->Reset();
		}
		Time::delta_time = 0;
		active_scene = CreateScene_From_File(next_scene_path);
		active_scene->Initialize();
		load = false;
	}

	if (run)
	{
		active_scene->Update();
		Engine::animator_manager->Update();
	}
}

unique_ptr<Scene>& Scene_Manager::Get_Active_Scene()
{
	return active_scene;
}