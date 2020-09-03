#include "Engine.h"
#include "Render_Manager.h"
#include "Animator_Manager.h"
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
	debug_ui      = make_unique<Debug_UI>();
	view_game     = make_unique<View_Game>();
	view_scene    = make_unique<View_Scene>();
	scene_manager->CreateScene_Default("Default_Scene");
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
	debug_ui->Update(scene_manager->Get_Active_Scene());
	Animator_Manager::Update();
	Render_Manager::Render();
	debug_ui->Render();
	
	DxSystem::Flip(0);
}