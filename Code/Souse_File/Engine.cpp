#include "Engine.h"
using namespace std;

Engine::Engine()
{
	Input_Manager::Initialize();
	Audio_Manager::Initialize();
	Scene_Manager::CreateScene(make_shared<Game_01_Scene>(), u8"Game_01_Scene");
	Scene_Manager::Set_StartScene(u8"Game_01_Scene");
}

Engine::~Engine()
{
	DxSystem::Release();
}

void Engine::Update()
{
	Input_Manager::Update();
	Scene_Manager::Update();
	Camera_Manager::Update();

	DxSystem::Clear();
	
	Camera_Manager::Render();
	Debug_UI::Render();
	
	DxSystem::Flip(0);
}