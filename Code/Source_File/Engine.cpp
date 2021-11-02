#include <fstream>
#include "Engine.h"
#include "Input.h"
#include "Cursor.h"
#include "Asset_Manager.h"
#include "Scene_Manager.h"
#include "Audio_Manager.h"
#include "Render_Manager.h"
#include "Animator_Manager.h"
#include "Light_Manager.h"
#include "Particle_Manager.h"
#include "Shadow_Manager.h"
#include "BulletPhysics_Manager.h"
#include "Debug_Draw_Manager.h"
#include "FBX_Converter.h"
#include "Editor.h"
#include "Render_Texture.h"
#include "Gaussian_Filter.h"
#include "SkyBox.h"
#include "Scene.h"
#include "Project_Settings.h"
#include "System_Function.h"
#include "Thread_Pool.h"
using namespace std;
using namespace BeastEngine;

unique_ptr<Input>                 Engine::input_manager;
unique_ptr<Cursor>                Engine::cursor_manager;
unique_ptr<Asset_Manager>         Engine::asset_manager;
unique_ptr<Scene_Manager>         Engine::scene_manager;
unique_ptr<Audio_Manager>         Engine::audio_manager;
unique_ptr<Render_Manager>        Engine::render_manager;
unique_ptr<Animator_Manager>      Engine::animator_manager;
unique_ptr<Light_Manager>         Engine::light_manager;
unique_ptr<Particle_Manager>      Engine::particle_manager;
unique_ptr<Shadow_Manager>	      Engine::shadow_manager;
unique_ptr<BulletPhysics_Manager> Engine::bulletphysics_manager;
unique_ptr<Debug_Draw_Manager>	  Engine::debug_draw_manager;
unique_ptr<Fbx_Converter>	      Engine::fbx_converter;
unique_ptr<Editor>                Engine::editor;
unique_ptr<Thread_Pool>           Engine::thread_pool;

Engine::Engine()
{
	thread_pool = make_unique<Thread_Pool>();
	input_manager = make_unique<Input>();
	cursor_manager = make_unique<Cursor>();
	asset_manager = make_unique<Asset_Manager>();
	audio_manager = make_unique<Audio_Manager>();
	particle_manager = make_unique<Particle_Manager>();
	shadow_manager = make_unique<Shadow_Manager>();
	render_manager = make_unique<Render_Manager>();
	animator_manager = make_unique<Animator_Manager>();
	light_manager = make_unique<Light_Manager>();
	bulletphysics_manager = make_unique<BulletPhysics_Manager>();
	scene_manager = make_unique<Scene_Manager>();

#if _DEBUG
	debug_draw_manager = make_unique<Debug_Draw_Manager>();
	bulletphysics_manager->Set_Debug_Drawer();
	fbx_converter = make_unique<Fbx_Converter>();
	editor = make_unique<Editor>();
	if (ifstream i_ifstream("Default_Assets\\System\\last_save.bin"); i_ifstream.is_open())
	{
		string load_path;
		getline(i_ifstream, load_path);

		if (load_path != "")
		{
			scene_manager->Load_Scene(load_path);
		}
		else
		{
			string path = System_Function::Get_Save_File_Name("bin", "\\Assets\\Scene");
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;//7
				int ext_i = path.find_last_of(".");//10
				string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				path = pathname + filename + ".bin";
				Engine::scene_manager->Create_Scene_Default(path, filename);
			}
		}
	}
	else
	{
		string path = System_Function::Get_Save_File_Name("bin", "\\Assets\\Scene");
		if (path != "")
		{
			int path_i = path.find_last_of("\\") + 1;//7
			int ext_i = path.find_last_of(".");//10
			string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
			string filename = path.substr(path_i, ext_i - path_i); //ファイル名
			path = pathname + filename + ".bin";
			Engine::scene_manager->Create_Scene_Default(path, filename);
		}
	}

#else
	string load_path;
	ifstream iIfstream("Default_Assets\\System\\last_save.bin");
	if (iIfstream.is_open())
	{
		getline(iIfstream, load_path);
		if (load_path != "")
		{
			scene_manager->Load_Scene(load_path);
		}
		else
		{
		}
	}
	else
	{
	}
	scene_manager->run = true;
#endif
}

Engine::~Engine()
{
	DxSystem::Release();
	scene_manager->Exit();
	scene_manager.reset();
	input_manager.reset();
	cursor_manager.reset();
	audio_manager.reset();
	render_manager.reset();
	animator_manager.reset();
	light_manager.reset();
	particle_manager.reset();
	shadow_manager.reset();
	fbx_converter.reset();
	editor.reset();
	debug_draw_manager.reset();
	bulletphysics_manager->Exit();
	bulletphysics_manager.reset();
	asset_manager->Exit();
	asset_manager.reset();
}

void Engine::Update()
{
	DxSystem::Clear();

	input_manager->Update();
	cursor_manager->Update();
	audio_manager->Update();
	scene_manager->Update();
	bulletphysics_manager->Update();
	particle_manager->Update();

#if _DEBUG
	editor->Update();
	render_manager->Render();
	editor->Render();
#else
	render_manager->Render();
#endif

	DxSystem::Flip(1);
}

void Engine::Get_Handle(const UINT msg, WPARAM w_param, LPARAM l_param)
{
	switch (msg)
	{
		case WM_SETFOCUS:
			Cursor::window_focus = true;
			break;
		case WM_KILLFOCUS:
			Cursor::window_focus = false;
			break;
		default:
			break;
	}
}