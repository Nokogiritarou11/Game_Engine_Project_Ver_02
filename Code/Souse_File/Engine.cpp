#include "Engine.h"
#include "Render_Manager.h"
#include "Animator_Manager.h"
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
			scene_manager->CreateScene_FromFile(load_pass);
		}else
		{
			scene_manager->CreateScene_Default("Default_Scene");
		}
	}
	else
	{
		scene_manager->CreateScene_Default("Default_Scene");
	}

#else
	scene_manager->CreateScene_Default("Default_Scene");
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