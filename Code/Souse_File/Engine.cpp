#include "Engine.h"
#include "Camera_Manager.h"
using namespace std;

std::shared_ptr<Scene_Manager> Engine::scene_manager;
std::shared_ptr<Input_Manager> Engine::input_manager;
std::shared_ptr<Audio_Manager> Engine::audio_manager;
std::shared_ptr<Debug_UI> Engine::debug_ui;

Engine::Engine()
{
	input_manager = make_shared<Input_Manager>();
	audio_manager = make_shared<Audio_Manager>();
	scene_manager = make_shared<Scene_Manager>();
	debug_ui      = make_shared<Debug_UI>();
	scene_manager->CreateScene_Default("Default_Scene");
}

Engine::~Engine()
{
	DxSystem::Release();
}
void Engine::Update()
{
	input_manager->Update();
	scene_manager->Update();

	DxSystem::Clear();
	
	Camera_Manager::Render();
	debug_ui->Render();
	
	DxSystem::Flip(0);
}