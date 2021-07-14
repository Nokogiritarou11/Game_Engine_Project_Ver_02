#include "Engine.h"
#include "Input.h"
#include "Cursor.h"
#include "Asset_Manager.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "Audio_Manager.h"
#include "Render_Manager.h"
#include "Animator_Manager.h"
#include "Light_Manager.h"
#include "Particle_Manager.h"
#include "Shadow_Manager.h"
#include "FBX_Converter.h"
#include "Editor.h"
#include "SkyBox.h"
#include "View_Game.h"
#include "View_Scene.h"
#include "System_Function.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;
using namespace BeastEngine;

unique_ptr<Input>			 Engine::input_manager;
unique_ptr<Cursor>			 Engine::cursor_manager;
unique_ptr<Asset_Manager>	 Engine::asset_manager;
unique_ptr<Scene_Manager>	 Engine::scene_manager;
unique_ptr<Audio_Manager>	 Engine::audio_manager;
unique_ptr<Render_Manager>	 Engine::render_manager;
unique_ptr<Animator_Manager> Engine::animator_manager;
unique_ptr<Light_Manager>	 Engine::light_manager;
unique_ptr<Particle_Manager> Engine::particle_manager;
unique_ptr<Shadow_Manager>	 Engine::shadow_manager;
unique_ptr<FBX_Converter>	 Engine::fbx_converter;
unique_ptr<Editor>			 Engine::editor;
unique_ptr<View_Game>		 Engine::view_game;
unique_ptr<View_Scene>		 Engine::view_scene;

Engine::Engine()
{
	input_manager = make_unique<Input>();
	cursor_manager = make_unique<Cursor>();
	asset_manager = make_unique<Asset_Manager>();
	scene_manager = make_unique<Scene_Manager>();
	audio_manager = make_unique<Audio_Manager>();
	render_manager = make_unique<Render_Manager>();
	animator_manager = make_unique<Animator_Manager>();
	light_manager = make_unique<Light_Manager>();
	particle_manager = make_unique<Particle_Manager>();
	shadow_manager = make_unique<Shadow_Manager>();
	view_game = make_unique<View_Game>();

#if _DEBUG
	fbx_converter = make_unique<FBX_Converter>();
	view_scene = make_unique<View_Scene>();
	editor = make_unique<Editor>();
	string load_path;
	ifstream iIfstream("Default_Resource\\System\\last_save.bin");
	if (iIfstream.is_open())
	{
		getline(iIfstream, load_path);
		if (load_path != "")
		{
			Scene_Manager::LoadScene(load_path);
		}
		else
		{
			string path = System_Function::Get_Save_File_Name();
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
		string path = System_Function::Get_Save_File_Name();
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
	ifstream iIfstream("Default_Resource\\System\\last_save.bin");
	if (iIfstream.is_open())
	{
		getline(iIfstream, load_path);
		if (load_path != "")
		{
			Scene_Manager::LoadScene(load_path);
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
	view_game.reset();
	view_scene.reset();
	asset_manager->Clear_Manager();
	asset_manager.reset();
}

void Engine::Update()
{
	DxSystem::Clear();

	input_manager->Update();
	cursor_manager->Update();
	audio_manager->Update();
	scene_manager->Update();

#if _DEBUG
	editor->Update(scene_manager->Get_Active_Scene());
	animator_manager->Update();
	particle_manager->Update();
	render_manager->Render();
	editor->Render();
#else
	view_game->Set_Screen_Size(DxSystem::Get_Screen_Width(), DxSystem::Get_Screen_Height());
	animator_manager->Update();
	particle_manager->Update();
	render_manager->Render();
#endif

	DxSystem::Flip(1);
}

void Engine::Get_Hundle(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SETFOCUS:
			Cursor::window_focus = true;
			break;
		case WM_KILLFOCUS:
			Cursor::window_focus = false;
			break;
	}
}