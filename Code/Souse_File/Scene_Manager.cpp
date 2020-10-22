#include "Scene_Manager.h"
#include "Animator_Manager.h"
#include "SkinMesh_Renderer.h"
#include "Render_Manager.h"
#include "Light_Manager.h"
#include "Transform.h"
#include "Debug_UI.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "System_Function.h"
using namespace std;

//**********************************************
//
//		シーンの管理
//
//**********************************************

shared_ptr<Scene> Scene_Manager::Active_Scene;
bool Scene_Manager::Load = false;
string Scene_Manager::Next_Scene_Path;

shared_ptr<Scene> Scene_Manager::CreateScene_FromFile()
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
			shared_ptr<Scene> New_Scene = make_shared<Scene>();
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(New_Scene);
			New_Scene->name = file_name;

			Last_Save_Path = load_path;
			return New_Scene;
		}
		else
		{
			return nullptr;
		}
	}
	return nullptr;
}

shared_ptr<Scene> Scene_Manager::CreateScene_FromFile(std::string file_path)
{
	int path_i = file_path.find_last_of("\\") + 1;//7
	int ext_i = file_path.find_last_of(".");//10
	string file_name = file_path.substr(path_i, ext_i - path_i); //ファイル名

	ifstream in_bin(file_path, ios::binary);
	if (in_bin.is_open())
	{
		shared_ptr<Scene> New_Scene = make_shared<Scene>();
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(New_Scene);
		New_Scene->name = file_name;

		Last_Save_Path = file_path;
		return New_Scene;
	}
	else
	{
		return nullptr;
	}

	return nullptr;
}

void Scene_Manager::CreateScene_Default(string file_path, string file_name)
{
	shared_ptr<Scene> New_Scene = make_shared<Scene>();
	New_Scene = make_shared<Scene>();
	New_Scene->name = file_name;

	Active_Scene = New_Scene;

	shared_ptr<GameObject> directional_light = GameObject::Instantiate(u8"Directional_Light");
	directional_light->AddComponent<Light>();
	directional_light->transform->Set_position(0, 50, 0);
	directional_light->transform->Set_eulerAngles(90, 0, 0);

	shared_ptr<GameObject> camera = GameObject::Instantiate(u8"Main_Camera");
	camera->AddComponent<Camera>();
	camera->transform->Set_position(-100, 80, -100);
	camera->transform->Set_eulerAngles(30, 45, 0);

	shared_ptr<GameObject> Floor = GameObject::Instantiate(u8"Glid_Tile");
	shared_ptr<SkinMesh_Renderer> f_renderer = Floor->AddComponent<SkinMesh_Renderer>();
	Floor->transform->Set_eulerAngles(0, 0, 0);
	Floor->transform->Set_scale(1, 1, 1);
	f_renderer->Set_Mesh(Mesh::Load_Mesh("Default_Resource\\Model\\Glid_Tile\\", "Glid_Tile"));

	ofstream ss(file_path.c_str(), ios::binary);
	{
		cereal::BinaryOutputArchive o_archive(ss);
		o_archive(New_Scene);
		Last_Save_Path = file_path;
	}

	LoadScene(file_path);
}

void Scene_Manager::Initialize_Scene(weak_ptr<Scene> s)
{
	shared_ptr<Scene> scene = s.lock();
	//scene->Reset();
	list<shared_ptr<GameObject>>::iterator itr_end = scene->gameObject_List.end();
	for (list<shared_ptr<GameObject>>::iterator itr = scene->gameObject_List.begin(); itr != itr_end; itr++)
	{
		list<shared_ptr<Component>>::iterator itr_comp_end = (*itr)->Component_List.end();
		for (list<shared_ptr<Component>>::iterator itr_comp = (*itr)->Component_List.begin(); itr_comp != itr_comp_end; itr_comp++)
		{
			(*itr_comp)->Initialize((*itr));
		}
	}
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
	Behind_Scene = Active_Scene;
	Behind_Path = Last_Save_Path;
	{
		ofstream save("Default_Resource\\System\\Debug_Scene.bin", ios::binary);
		{
			cereal::BinaryOutputArchive out_archive(save);
			out_archive(Active_Scene);
		}
	}
	LoadScene("Default_Resource\\System\\Debug_Scene.bin");
}

void Scene_Manager::End_DebugScene()
{
	Animator_Manager::Reset();
	Render_Manager::Reset();
	Light_Manager::Reset();
	Active_Scene = Behind_Scene;
	Last_Save_Path = Behind_Path;
	Initialize_Scene(Active_Scene);
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
		Animator_Manager::Reset();
		Render_Manager::Reset();
		Light_Manager::Reset();
		Active_Scene = CreateScene_FromFile(Next_Scene_Path);
		Initialize_Scene(Active_Scene);
		Load = false;
	}

	if (Run)
	{
		Active_Scene->Update();
	}
}

shared_ptr<Scene>  Scene_Manager::Get_Active_Scene()
{
	return Active_Scene;
}