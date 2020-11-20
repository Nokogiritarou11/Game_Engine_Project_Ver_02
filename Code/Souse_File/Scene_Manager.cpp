#include "Animator_Manager.h"
#include "SkinMesh_Renderer.h"
#include "Light_Manager.h"
#include "Transform.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "System_Function.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Render_Manager.h"
#include "Audio_Manager.h"
#include "Particle_Manager.h"
using namespace std;

//**********************************************
//
//		シーンの管理
//
//**********************************************

unique_ptr<Scene> Scene_Manager::Active_Scene;
bool Scene_Manager::Load = false;
string Scene_Manager::Next_Scene_Path;

unique_ptr<Scene> Scene_Manager::CreateScene_FromFile()
{
	string load_path = System_Function::Get_Open_File_Name();

	if (load_path != "")
	{
		int path_i = load_path.find_last_of("\\") + 1;//7
		int ext_i = load_path.find_last_of(".");//10
		string file_name = load_path.substr(path_i, ext_i - path_i); //ファイル名

		ifstream in_bin(load_path, ios::binary);
		if (in_bin.is_open())
		{
			unique_ptr<Scene> New_Scene = make_unique<Scene>();
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(New_Scene);
			New_Scene->name = file_name;

			Last_Save_Path = load_path;
			return move(New_Scene);
		}
		else
		{
			return nullptr;
		}
	}
	return nullptr;
}

unique_ptr<Scene> Scene_Manager::CreateScene_FromFile(std::string file_path)
{
	int path_i = file_path.find_last_of("\\") + 1;//7
	int ext_i = file_path.find_last_of(".");//10
	string file_name = file_path.substr(path_i, ext_i - path_i); //ファイル名

	ifstream in_bin(file_path, ios::binary);
	if (in_bin.is_open())
	{
		unique_ptr<Scene> New_Scene = make_unique<Scene>();
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(New_Scene);
		New_Scene->name = file_name;

		Last_Save_Path = file_path;
		return move(New_Scene);
	}
	else
	{
		return nullptr;
	}

	return nullptr;
}

void Scene_Manager::CreateScene_Default(string file_path, string file_name)
{
	unique_ptr<Scene> New_Scene = make_unique<Scene>();
	New_Scene->name = file_name;

	if (Active_Scene)
	{
		Active_Scene->Reset();
	}
	Active_Scene = move(New_Scene);

	shared_ptr<GameObject> directional_light = GameObject::Instantiate(u8"Directional_Light");
	directional_light->AddComponent<Light>();
	directional_light->transform->Set_position(0, 50, 0);
	directional_light->transform->Set_eulerAngles(90, 0, 0);

	shared_ptr<GameObject> camera = GameObject::Instantiate(u8"Main_Camera");
	camera->AddComponent<Camera>();
	camera->transform->Set_position(-100, 80, -100);
	camera->transform->Set_eulerAngles(30, 45, 0);

	ofstream ss(file_path.c_str(), ios::binary);
	{
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(Active_Scene);
		Last_Save_Path = file_path;
	}

	LoadScene(file_path);
}

void Scene_Manager::SaveScene(string Save_Path)
{
	int path_i = Save_Path.find_last_of("\\") + 1;
	int ext_i = Save_Path.find_last_of(".");
	string filename = Save_Path.substr(path_i, ext_i - path_i); //ファイル名
	Active_Scene->name = filename;

	ofstream ss(Save_Path.c_str(), ios::binary);
	{
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(Active_Scene);
		Last_Save_Path = Save_Path;
	}
}

void Scene_Manager::Start_DebugScene()
{
	Behind_Scene = move(Active_Scene);
	Behind_Path = Last_Save_Path;
	{
		ofstream save("Default_Resource\\System\\Debug_Scene.bin", ios::binary);
		{
			cereal::BinaryOutputArchive out_archive(save);
			out_archive(Behind_Scene);
		}
	}
	LoadScene("Default_Resource\\System\\Debug_Scene.bin");
}

void Scene_Manager::End_DebugScene()
{
	Engine::animator_manager->Reset();
	Engine::render_manager->Reset();
	Engine::light_manager->Reset();
	Engine::particle_manager->Reset();
	Engine::audio_manager->Reset();

	Active_Scene->Reset();
	Active_Scene = move(Behind_Scene);
	Last_Save_Path = Behind_Path;
	Active_Scene->Initialize();
}

void Scene_Manager::LoadScene(string Scene_Path)
{
	Load = true;
	Next_Scene_Path = Scene_Path;
}

void Scene_Manager::Update()
{
	if (Load)
	{
		Engine::animator_manager->Reset();
		Engine::render_manager->Reset();
		Engine::light_manager->Reset();
		Engine::particle_manager->Reset();
		Engine::audio_manager->Reset();

		if (Active_Scene)
		{
			Active_Scene->Reset();
		}
		Active_Scene = CreateScene_FromFile(Next_Scene_Path);
		Active_Scene->Initialize();
		Load = false;
	}

	if (Run)
	{
		Active_Scene->Update();
	}
}

unique_ptr<Scene>& Scene_Manager::Get_Active_Scene()
{
	return Active_Scene;
}

void Scene_Manager::Release()
{
	if (Active_Scene) Active_Scene->Reset();
}