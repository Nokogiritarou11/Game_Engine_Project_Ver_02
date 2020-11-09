#include "Engine.h"
#include "Scene_Manager.h"
#include "Audio_Manager.h"
#include "Render_Manager.h"
#include "Animator_Manager.h"
#include "Light_Manager.h"
#include "Particle_Manager.h"
#include "Debug_UI.h"
#include "View_Game.h"
#include "View_Scene.h"
#include "System_Function.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

unique_ptr<Scene_Manager>	 Engine::scene_manager;
unique_ptr<Input_Manager>	 Engine::input_manager;
unique_ptr<Audio_Manager>	 Engine::audio_manager;
unique_ptr<Render_Manager>	 Engine::render_manager;
unique_ptr<Animator_Manager> Engine::animator_manager;
unique_ptr<Light_Manager>	 Engine::light_manager;
unique_ptr<Particle_Manager> Engine::particle_manager;
unique_ptr<Debug_UI>		 Engine::debug_ui;
unique_ptr<View_Game>		 Engine::view_game;
unique_ptr<View_Scene>		 Engine::view_scene;

Engine::Engine()
{
	audio_manager	 = make_unique<Audio_Manager>();
	input_manager    = make_unique<Input_Manager>();
	scene_manager    = make_unique<Scene_Manager>();
	render_manager   = make_unique<Render_Manager>();
	animator_manager = make_unique<Animator_Manager>();
	light_manager    = make_unique<Light_Manager>();
	particle_manager = make_unique<Particle_Manager>();

	view_game = make_unique<View_Game>();

#if _DEBUG
	view_scene = make_unique<View_Scene>();
	debug_ui = make_unique<Debug_UI>();
	//scene_manager->CreateScene_Default("Default_Scene");
	string load_path;
	ifstream iIfstream("Default_Resource\\System\\last_save.bin");
	if (iIfstream.is_open())
	{
		getline(iIfstream, load_path);
		if (load_path != "")
		{
			Scene_Manager::LoadScene(load_path);
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
	string load_path;
	ifstream iIfstream("Default_Resource\\System\\last_save.bin");
	if (iIfstream.is_open())
	{
		getline(iIfstream, load_path);
		if (load_path != "")
		{
			Scene_Manager::LoadScene(load_path);
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
	scene_manager->Release();
}

void Engine::Update()
{
	DxSystem::Clear();

	input_manager->Update();
	audio_manager->Update();
	scene_manager->Update();

#if _DEBUG
	debug_ui->Update(scene_manager->Get_Active_Scene());
	animator_manager->Update();
	render_manager->Render();
	debug_ui->Render();
#else
	view_game->Set_Screen_Size(DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight());
	animator_manager->Update();
	render_manager->Render();
#endif

	DxSystem::Flip(0);
}