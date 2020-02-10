#include "Engine.h"
using namespace std;

Engine::Engine()
{
	Input_Manager::Initialize();
	Audio_Manager::Initialize();
	Scene_Manager::CreateScene(make_shared<Game_01_Scene>(), "Game_01_Scene");
	Scene_Manager::CreateScene(make_shared<Game_02_Scene>(), "Game_02_Scene");
	Scene_Manager::CreateScene(make_shared<Game_03_Scene>(), "Game_03_Scene");
	Scene_Manager::CreateScene(make_shared<Title_Scene>(), "Title_Scene");
	Scene_Manager::Set_StartScene("Title_Scene");
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
	DxSystem::Flip(0);
}