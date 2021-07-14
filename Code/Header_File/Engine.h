#pragma once
#include "DxSystem.h"
#include <memory>

namespace BeastEngine
{
	class Input;
	class Cursor;
	class Asset_Manager;
	class Scene_Manager;
	class Audio_Manager;
	class Render_Manager;
	class Animator_Manager;
	class Light_Manager;
	class Particle_Manager;
	class Shadow_Manager;
	class FBX_Converter;
	class Editor;
	class View_Game;
	class View_Scene;

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Update();

		static void Get_Hundle(UINT msg, WPARAM wParam, LPARAM lParam);

		static std::unique_ptr<BeastEngine::Input> input_manager;
		static std::unique_ptr<BeastEngine::Cursor> cursor_manager;
		static std::unique_ptr<BeastEngine::Asset_Manager> asset_manager;
		static std::unique_ptr<BeastEngine::Scene_Manager> scene_manager;
		static std::unique_ptr<BeastEngine::Audio_Manager> audio_manager;
		static std::unique_ptr<BeastEngine::Render_Manager> render_manager;
		static std::unique_ptr<BeastEngine::Animator_Manager> animator_manager;
		static std::unique_ptr<BeastEngine::Light_Manager> light_manager;
		static std::unique_ptr<BeastEngine::Particle_Manager> particle_manager;
		static std::unique_ptr<BeastEngine::Shadow_Manager> shadow_manager;
		static std::unique_ptr<BeastEngine::FBX_Converter> fbx_converter;
		static std::unique_ptr<BeastEngine::Editor> editor;
		static std::unique_ptr<BeastEngine::View_Game> view_game;
		static std::unique_ptr<BeastEngine::View_Scene> view_scene;
	private:
	};
}