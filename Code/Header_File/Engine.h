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
	class BulletPhysics_Manager;
	class Debug_Draw_Manager;
	class Fbx_Converter;
	class Editor;
	class Thread_Pool;

	//エンジン本体
	class Engine
	{
	public:
		Engine();
		~Engine();

		static void Update(); //更新
		static void Get_Handle(UINT msg, WPARAM w_param, LPARAM l_param); //メインウィンドウのハンドルを取得する

		static std::unique_ptr<Input> input_manager;
		static std::unique_ptr<Cursor> cursor_manager;
		static std::unique_ptr<Asset_Manager> asset_manager;
		static std::unique_ptr<Scene_Manager> scene_manager;
		static std::unique_ptr<Audio_Manager> audio_manager;
		static std::unique_ptr<Render_Manager> render_manager;
		static std::unique_ptr<Animator_Manager> animator_manager;
		static std::unique_ptr<Light_Manager> light_manager;
		static std::unique_ptr<Particle_Manager> particle_manager;
		static std::unique_ptr<Shadow_Manager> shadow_manager;
		static std::unique_ptr<BulletPhysics_Manager> bulletphysics_manager;
		static std::unique_ptr<Debug_Draw_Manager> debug_draw_manager;
		static std::unique_ptr<Fbx_Converter> fbx_converter;
		static std::unique_ptr<Editor> editor;
		static std::unique_ptr<Thread_Pool> thread_pool;
	private:
	};
}