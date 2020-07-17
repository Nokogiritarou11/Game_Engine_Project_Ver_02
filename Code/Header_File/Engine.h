#pragma once
#include "DxSystem.h"
#include "Scene_Manager.h"
#include "Audio_Manager.h"

class Engine
{
public:
	Engine();
	~Engine();

	void Update();

	static std::shared_ptr<Scene_Manager> scene_manager;
	static std::shared_ptr<Input_Manager> input_manager;
	static std::shared_ptr<Audio_Manager> audio_manager;
private:
};