#pragma once
#include "DxSystem.h"
#include "Scene_Manager.h"
#include "Audio_Manager.h"
#include "Debug_UI.h"
#include "View_Game.h"
#include "View_Scene.h"

class Engine
{
public:
	Engine();
	~Engine();

	void Update();

	static std::unique_ptr<Scene_Manager> scene_manager;
	static std::unique_ptr<Input_Manager> input_manager;
	static std::unique_ptr<Audio_Manager> audio_manager;
	static std::unique_ptr<Debug_UI> debug_ui;
	static std::unique_ptr<View_Game> view_game;
	static std::unique_ptr<View_Scene> view_scene;
private:
};