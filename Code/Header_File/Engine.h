#pragma once
#include "DxSystem.h"
#include <memory>

class Input;
class Cursor;
class Scene_Manager;
class Audio_Manager;
class Render_Manager;
class Animator_Manager;
class Light_Manager;
class Particle_Manager;
class Editor_UI;
class View_Game;
class View_Scene;

class Engine
{
public:
	Engine();
	~Engine();

	void Update();

	static void GetHundle(UINT msg, WPARAM wParam, LPARAM lParam);

	static std::unique_ptr<Scene_Manager> scene_manager;
	static std::unique_ptr<Input> input_manager;
	static std::unique_ptr<Cursor> cursor_manager;
	static std::unique_ptr<Audio_Manager> audio_manager;
	static std::unique_ptr<Render_Manager> render_manager;
	static std::unique_ptr<Animator_Manager> animator_manager;
	static std::unique_ptr<Light_Manager> light_manager;
	static std::unique_ptr<Particle_Manager> particle_manager;
	static std::unique_ptr<Editor_UI> editor_ui;
	static std::unique_ptr<View_Game> view_game;
	static std::unique_ptr<View_Scene> view_scene;
private:
};