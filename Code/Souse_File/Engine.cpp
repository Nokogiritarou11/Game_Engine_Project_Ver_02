#include "Engine.h"
#include "Render_Manager.h"
#include "Animator_Manager.h"
#include "System_Function.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

unique_ptr<Scene_Manager> Engine::scene_manager;
unique_ptr<Input_Manager> Engine::input_manager;
unique_ptr<Audio_Manager> Engine::audio_manager;
unique_ptr<Debug_UI>	  Engine::debug_ui;
unique_ptr<View_Game>	  Engine::view_game;
unique_ptr<View_Scene>	  Engine::view_scene;

Engine::Engine()
{
	input_manager = make_unique<Input_Manager>();
	audio_manager = make_unique<Audio_Manager>();
	scene_manager = make_unique<Scene_Manager>();

	view_game = make_unique<View_Game>();

#if _DEBUG
	debug_ui   = make_unique<Debug_UI>();
	view_scene = make_unique<View_Scene>();
	//scene_manager->CreateScene_Default("Default_Scene");
	string load_pass;
	ifstream iIfstream("Default_Resource\\System\\last_save.bin");
	if (iIfstream.is_open())
	{
		getline(iIfstream, load_pass);
		if(load_pass != "")
		{
			Scene_Manager::LoadScene(load_pass);
		}
		else
		{
			string path = System_Function::Get_Save_File_Name();
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;//7
				int ext_i = path.find_last_of(".");//10
				string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				path = pathname + filename + ".bin";
				Engine::scene_manager->CreateScene_Default(path, filename);
			}
		}
	}
	else
	{
		string path = System_Function::Get_Save_File_Name();
		if (path != "")
		{
			int path_i = path.find_last_of("\\") + 1;//7
			int ext_i = path.find_last_of(".");//10
			string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
			string filename = path.substr(path_i, ext_i - path_i); //ファイル名
			path = pathname + filename + ".bin";
			Engine::scene_manager->CreateScene_Default(path, filename);
		}
	}

#else
	string load_pass;
	ifstream iIfstream("Default_Resource\\System\\last_save.bin");
	if (iIfstream.is_open())
	{
		getline(iIfstream, load_pass);
		if (load_pass != "")
		{
			Scene_Manager::LoadScene(load_pass);
		}
		else
		{
		}
	}
	else
	{
	}
	scene_manager->Run = true;
#endif
}

Engine::~Engine()
{
	DxSystem::Release();
}
void Engine::Update()
{
	DxSystem::Clear();

	input_manager->Update();
	scene_manager->Update();

#if _DEBUG
	debug_ui->Update(scene_manager->Get_Active_Scene());
	Animator_Manager::Update();
	Render_Manager::Render();
	debug_ui->Render();
#else
	Engine::view_game->Set_Screen_Size(DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight());
	Animator_Manager::Update();
	Render_Manager::Render();
#endif

	DxSystem::Flip(0);
}