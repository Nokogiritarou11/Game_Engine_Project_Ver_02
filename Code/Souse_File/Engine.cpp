#include "Engine.h"
#include "Camera_Manager.h"
#include "Debug_UI.h"
using namespace std;

std::shared_ptr<Scene_Manager> Engine::scene_manager;
std::shared_ptr<Input_Manager> Engine::input_manager;
std::shared_ptr<Audio_Manager> Engine::audio_manager;

Engine::Engine()
{
	Debug_UI::Initialize();
	input_manager = make_shared<Input_Manager>();
	audio_manager = make_shared<Audio_Manager>();
	scene_manager = make_shared<Scene_Manager>();
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
	Debug_UI::Render();
	
	DxSystem::Flip(0);
}