#include "DxSystem.h"
#include "Editor.h"
#include "Debug.h"
#include "Original_Math.h"
#include "Scene_Manager.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Particle_Manager.h"
#include "Shadow_Manager.h"
#include "Project_Settings.h"
#include "Collider.h"
#include "FBX_Converter.h"
#include "Resources.h"
#include "Cursor.h"
#include "Transform.h"
#include "Animator.h"
#include "Animator_Controller.h"
#include "Scene.h"
#include "Include_ImGui.h"
#include "System_Function.h"
#include "Render_Manager.h"
#include "Render_Texture.h"
#include "Camera.h"
#include <ImGuizmo.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

Editor::Editor()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	//テーマの変更
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	if (!ImGui_ImplWin32_Init(DxSystem::hwnd))
	{
		std::cout << "ImGui_ImplWin32_Init failed\n";
		ImGui::DestroyContext();
		std::exit(1);
	}

	if (!ImGui_ImplDX11_Init(DxSystem::device.Get(), DxSystem::device_context.Get()))
	{
		std::cout << "ImGui_ImplDX11_Init failed\n";
		ImGui::DestroyContext();
		std::exit(1);
	}

	//日本語フォントに対応
	ImFont* font = io.Fonts->AddFontFromFileTTF(font_name, font_size_pixels, nullptr, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);

	io.FontGlobalScale = font_size; // フォントの大きさを一括で変更できます。
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(ui_size); // UIの大きさを一括で変更できます。

	float size_icon = 20.0f; // 30.0fにすると大きすぎたので20.0fにしています。
	ImFontConfig config;
	config.MergeMode = true; // フォントテクスチャを合体させます。
	config.PixelSnapH = true;
	config.GlyphMinAdvanceX = size_icon; // アイコンを等幅にします。
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("Default_Resource/Font/fontawesome-webfont.ttf", size_icon, &config, icon_ranges);
	io.Fonts->Build();

	Vector3 p = { 0, 1.5f, -3 };
	Vector3 e = { 15, 0, 0 };
	debug_camera_transform = make_shared<Transform>(p, e);
	Engine::asset_manager->Registration_Asset(debug_camera_transform);
	debug_camera = make_shared<Camera>();
	Engine::asset_manager->Registration_Asset(debug_camera);
	debug_camera->transform = debug_camera_transform;
}

Editor::~Editor()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Editor::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();

	Main_Window_Render();


	//FBX読み込みメニュー
	Engine::fbx_converter->Draw_ImGui();

	//AnimatorController編集画面描画
	Controller_Render();

	//ゲームオブジェクト関連
	{
		//ヒエラルキー
		Hierarchy_Render();
		//インスペクタ
		Inspector_Render();
	}
	//描画
	{
		GameView_Render();
		SceneView_Render();
	}
	//デバッグログ
	Debug_Log_Render();

	//シーン再生UI
	ScenePlayer_Render();

	//ショートカットキーチェック
	ShortCut_Check();
}

void Editor::Render()
{
	// レンダーターゲットビュー設定
	DxSystem::Set_Default_View();
	DxSystem::Set_ViewPort(DxSystem::Get_Screen_Width(), DxSystem::Get_Screen_Height());
	ImGui::Render();
	DxSystem::Clear();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

//ドッキング用親ウィンドウ描画
void Editor::Main_Window_Render()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin(u8"ゲームエンジン", NULL, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	MenuBar_Render();

	ImGui::End();
}

//ログの追加
void Editor::Print_Log(string log)
{
	debug_log.push_back(log + u8"\n");
	debug_log_changed = true;
}

//ログ管理用構造体
struct Debug_Logger
{
	ImGuiTextBuffer     Buf;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.

	Debug_Logger()
	{
		AutoScroll = true;
		Clear();
	}

	void Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void Draw(const char* title)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(title, NULL, window_flags))
		{
			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				ImGui::Checkbox(u8"自動スクロール", &AutoScroll);
				ImGui::EndPopup();
			}

			// Main window
			if (ImGui::Button(u8"オプション"))
				ImGui::OpenPopup("Options");
			ImGui::SameLine();
			bool clear = ImGui::Button(u8"消去");
			ImGui::SameLine();
			bool copy = ImGui::Button(u8"コピー");

			//ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			ImGuiListClipper clipper;
			clipper.Begin(LineOffsets.Size);
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					ImGui::TextUnformatted(line_start, line_end);
				}
			}
			clipper.End();
			ImGui::PopStyleVar();

			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();
		}
		ImGui::End();
	}
};

//デバッグログ描画
void Editor::Debug_Log_Render()
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(0, 900), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_FirstUseEver);
	static Debug_Logger logger;
	if (debug_log_changed)
	{
		for (unsigned int i = 0; i < debug_log.size(); i++)
		{
			logger.AddLog(debug_log[i].c_str());
		}
		debug_log.clear();
	}
	logger.Draw((u8"コンソール"));
}

//ヒエラルキー描画
void Editor::Hierarchy_Render()
{
	const unique_ptr<Scene>& scene = Engine::scene_manager->active_scene;

	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin(u8"ヒエラルキー", NULL, window_flags))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if (ImGui::TreeNode(scene->name.c_str()))
		{
			static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

			select_object = false;
			for (auto& obj_list : scene->gameobject_list)
			{
				if (obj_list->transform->parent.expired())
				{
					GameObject_Tree_Render(obj_list, base_flags);
				}
			}

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && draging_object.expired())
			{
				Set_Debug_Draw(false, active_object.lock());
				active_object.reset();
				active_object_old.reset();
			}

			if (ImGui::BeginPopupContextWindow(u8"ヒエラルキーサブ"))
			{
				if (ImGui::Selectable(u8"新規オブジェクトを追加"))
				{
					auto& create_obj = scene->Instance_GameObject(u8"GameObject");
					if (auto& obj = active_object.lock())
					{
						create_obj->transform->Set_Parent(obj->transform);
					}
				}
				if (ImGui::Selectable(u8"プレハブを読み込む"))
				{
					string path = System_Function::Get_Open_File_Name("prefab", "\\Resouces\\Prefab");
					if (path != "")
					{
						Resources::Load_Prefab(path);
					}
				}
				ImGui::Separator();

				if (auto& obj = active_object.lock())
				{
					bool deleted = false;
					if (ImGui::Selectable(u8"オブジェクトを削除"))
					{
						Select_Reset();
						GameObject::Destroy(obj);
						deleted = true;
					}

					if (!deleted)
					{
						if (ImGui::Selectable(u8"オブジェクトを複製"))
						{
							auto& parent = obj->transform->Get_Parent().lock();
							if (parent)
							{
								obj->transform->Set_Parent(nullptr);
							}

							{
								ofstream ss("Default_Resource\\System\\copy.prefab", ios::binary);
								{
									cereal::BinaryOutputArchive o_archive(ss);
									o_archive(obj);
								}
							}

							if (parent)
							{
								obj->transform->Set_Parent(parent);
								Resources::Load_Prefab("Default_Resource\\System\\copy.prefab")->transform->Set_Parent(parent);
							}
							else
							{
								Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
							}
						}

						if (ImGui::Selectable(u8"オブジェクトをプレハブ化"))
						{
							Resources::Create_Prefab(obj);
						}

						if (!obj->transform->parent.expired())
						{
							if (ImGui::Selectable(u8"オブジェクトの親子関係を解除"))
							{
								obj->transform->Set_Parent(nullptr);
							}
						}
					}
				}
				ImGui::EndPopup();
			}
			ImGui::TreePop();

			if (ImGui::IsWindowFocused())
			{
				if (auto& obj = active_object.lock())
				{
					if (ImGui::IsKeyDown(17) && ImGui::IsKeyPressed(68)) //Ctrl + D
					{
						auto& parent = obj->transform->Get_Parent().lock();
						if (parent)
						{
							obj->transform->Set_Parent(nullptr);
						}

						{
							ofstream ss("Default_Resource\\System\\copy.prefab", ios::binary);
							{
								cereal::BinaryOutputArchive o_archive(ss);
								o_archive(obj);
							}
						}

						if (parent)
						{
							obj->transform->Set_Parent(parent);
							Resources::Load_Prefab("Default_Resource\\System\\copy.prefab")->transform->Set_Parent(parent);
						}
						else
						{
							Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
						}
					}

					if (ImGui::IsKeyPressed(46)) //Delete
					{
						Select_Reset();
						GameObject::Destroy(obj);
					}
				}
			}
		}
	}

	ImGui::End();
}

//インスペクタ描画
void Editor::Inspector_Render()
{
	ImGui::SetNextWindowPos(ImVec2(1500, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	if (ImGui::Begin(u8"インスペクタ", NULL, window_flags))
	{
		if (auto& obj = active_object.lock())
		{
			static bool active = false;
			if (active_object_old.lock() != obj)
			{
				active = obj->Get_Active();
				active_object_old = active_object;
			}

			//選択時
			if (ImGui::Checkbox("##active", &active))
			{
				obj->Set_Active(active);
			}
			ImGui::SameLine();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputText("##Name", &obj->name);

			const unique_ptr<Project_Settings>& settings = Engine::scene_manager->settings;
			ImGui::Text(u8"タグ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() * -0.5f);
			if (ImGui::BeginCombo("##Tag", obj->tag.data()))
			{
				const size_t tag_size = settings->tag.size();
				for (size_t i = 0; i < tag_size; ++i)
				{
					const string& select_tag = settings->tag[i];
					const bool is_selected = (obj->tag == select_tag);
					if (ImGui::Selectable(select_tag.data(), is_selected))
					{
						obj->tag = select_tag;
					}

					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::SameLine();
			ImGui::Text(u8"レイヤー");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::BeginCombo("##Layer", settings->layer[obj->layer].data()))
			{
				array<string, 32>& layer = settings->layer;
				size_t layer_size = 0;
				for (; layer_size < 32; ++layer_size)
				{
					if (layer[layer_size].empty()) break;
				}
				for (size_t i = 0; i < layer_size; ++i)
				{
					const bool is_selected = (obj->layer == i);
					if (ImGui::Selectable(settings->layer[i].data(), is_selected))
					{
						obj->layer = i;
					}

					if (is_selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Dummy({ 0,10 });
			ImGui::Separator();

			for (auto& c : obj->component_list)
			{
				ImGui::PushID(c->Get_Instance_ID().c_str());
				if (!c->Draw_ImGui())
				{
					ImGui::PopID();
					break;
				}
				ImGui::PopID();
			}

			ImGui::Separator();

			ImGui::Dummy({ 0.0f, 10.0f });
			if (ImGui::Button(u8"コンポーネントを追加", ImVec2(-FLT_MIN, 0.0f)))
			{
				ImGui::OpenPopup(u8"コンポーネントリスト");
			}
			ImGui::SetNextWindowSize(ImVec2(300, 250));
			if (ImGui::BeginPopup(u8"コンポーネントリスト"))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5f));
				auto& component_map = Component_Data::Component_Factory::getMap();
				for (auto it = component_map->begin(); it != component_map->end(); ++it)
				{
					if (ImGui::Button(it->first.c_str(), ImVec2(-FLT_MIN, 0.0f)))
					{
						if (obj->Add_Component(it->first))
						{
							Set_Debug_Draw(true, obj);
						}
						else
						{
							Debug::Log(u8"このコンポーネントは一つのオブジェクトに複数アタッチできません");
						}
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::PopStyleVar();
				ImGui::EndPopup();
			}
		}
		else
		{
			ImGui::Text(u8"オブジェクトが選択されていません");
		}
	}

	//ドラッグ終了処理
	if (ImGui::IsMouseReleased(0) && !select_object && !draging_object.expired())
	{
		draging_object.reset();
	}

	ImGui::End();
}

//シーン再生UI描画
void Editor::ScenePlayer_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	if (ImGui::Begin(u8"シーン再生", NULL, window_flags))
	{
		bool Running = Engine::scene_manager->run;

		if (Running)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.85f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.6f, 1.0f));
		}
		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f - 50);
		if (ImGui::Button(ICON_FA_PLAY, ImVec2(30, 0)))
		{
			if (!Running)
			{
				if (!Engine::scene_manager->pause)
				{
					Cursor::cursor_lock_mode = CursorLock_Mode::None;
					Cursor::visible = true;
					ImGui::SetWindowFocus(u8"ゲーム");
					Select_Reset();
					Engine::scene_manager->Start_Debug_Scene();
				}
				else
				{
					Engine::audio_manager->Resume();
				}
				render_cursor = false;
				Engine::scene_manager->run = true;
				Engine::scene_manager->pause = false;
			}
		}
		if (Running)
		{
			ImGui::PopStyleColor(3);
		}

		ImGui::SameLine();
		bool Pausing = Engine::scene_manager->pause;
		if (Pausing)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.85f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.6f, 1.0f));
		}
		if (ImGui::Button(ICON_FA_PAUSE, ImVec2(30, 0)))
		{
			if (Running)
			{
				render_cursor = true;
				Engine::scene_manager->run = false;
				Engine::scene_manager->pause = true;
				Engine::audio_manager->Suspend();
			}
		}
		if (Pausing)
		{
			ImGui::PopStyleColor(3);
		}

		ImGui::SameLine();
		if (ImGui::Button(ICON_FA_STOP, ImVec2(30, 0)))
		{
			if (Running || Pausing)
			{
				ImGui::SetWindowFocus(u8"シーン");
				Cursor::cursor_lock_mode = CursorLock_Mode::None;
				Cursor::visible = true;
				Select_Reset();
				render_cursor = true;
				Engine::scene_manager->run = false;
				Engine::scene_manager->pause = false;
				Engine::scene_manager->End_Debug_Scene();
			}
		}
	}

	ImGui::End();
}

//シーンビュー描画
void Editor::SceneView_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

	if (ImGui::Begin(u8"シーン", NULL, window_flags))
	{
		const float window_width = ImGui::GetCurrentWindow()->InnerRect.GetWidth() - 8;
		const float window_height = ImGui::GetCurrentWindow()->InnerRect.GetHeight() - 8;

		Engine::render_manager->render_scene = true;
		Engine::render_manager->scene_texture->Set_Screen_Size((int)window_width, (int)window_height);
		ImGui::Image((void*)Engine::render_manager->scene_texture->Get_Texture().Get(), ImVec2(window_width, window_height));

		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::MODE::LOCAL);

		Debug_Camera_Update();

		if (ImGui::IsWindowFocused())
		{
			if (!ImGui::IsMouseDown(1))
			{
				if (ImGui::IsKeyPressed(87)) //w
					mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
				if (ImGui::IsKeyPressed(69)) //e
					mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
				if (ImGui::IsKeyPressed(82)) //r
					mCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
				if (ImGui::IsKeyPressed(88)) //x
				{
					if (mCurrentGizmoMode == ImGuizmo::MODE::LOCAL)
					{
						mCurrentGizmoMode = ImGuizmo::MODE::WORLD;
					}
					else
					{
						mCurrentGizmoMode = ImGuizmo::MODE::LOCAL;
					}
				}
			}
		}

		if (auto& obj = active_object.lock())
		{
			static Matrix matrix;

			matrix = obj->transform->Get_World_Matrix();

			ImVec2 winPos = ImGui::GetWindowPos();
			ImGuizmo::SetRect(winPos.x + 4, winPos.y + 16, window_width, window_height);
			ImGuizmo::SetDrawlist();

			// 選択ノードの行列を操作する
			ImGuizmo::Manipulate(
				&debug_camera->view_matrix._11, &debug_camera->projection_matrix._11,	// ビュー＆プロジェクション行列
				mCurrentGizmoOperation,		// 移動操作
				mCurrentGizmoMode,			// ローカル空間での操作
				&matrix._11,	// 操作するワールド行列
				nullptr);

			if (ImGuizmo::IsUsing())
			{
				obj->transform->Set_World_Matrix(matrix);
			}
		}
	}
	else
	{
		Engine::render_manager->render_scene = false;
	}

	ImGui::End();
}

//ゲームビュー描画
void Editor::GameView_Render()
{
	ImGui::SetNextWindowPos(ImVec2(200, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin(u8"ゲーム", NULL, window_flags))
	{
		const ImGuiWindow* p_win = ImGui::GetCurrentWindow();
		const Vector2 p_win_size = { p_win->InnerRect.GetWidth() - 8, p_win->InnerRect.GetHeight() - 8 };

		Engine::render_manager->render_game = true;
		Engine::render_manager->game_texture->Set_Screen_Size(1920, 1080);
		constexpr float aspect_lock = 1920.0f / 1080.0f;
		if (p_win_size != game_view_size)
		{
			game_view_size = p_win_size;
			game_view_render_size = game_view_size;
			game_view_aspect = game_view_size.x / game_view_size.y;

			if (aspect_lock < game_view_aspect) //横のほうが既定より長いとき
			{
				game_view_render_size.x = game_view_render_size.y * aspect_lock;
			}
			else //縦が長いとき
			{
				game_view_render_size.y = game_view_render_size.x / aspect_lock;
			}
		}

		const ImVec2 pos = ImGui::GetCursorScreenPos();
		game_view_position = { pos.x,pos.y + game_view_size.y };

		if (aspect_lock < game_view_aspect) //横のほうが既定より長いとき
		{
			ImGui::Dummy({});
			ImGui::SameLine((game_view_size.x - game_view_render_size.x) * 0.5f);
		}
		else
		{
			ImGui::Dummy({ 0, (game_view_size.y - game_view_render_size.y) * 0.5f });
		}

		ImGui::Image((void*)Engine::render_manager->game_texture->Get_Texture().Get(), ImVec2(game_view_render_size.x, game_view_render_size.y));

		game_view_center_position = { pos.x + game_view_size.x / 2 ,pos.y + game_view_size.y / 2 };

		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsKeyPressed(27) && !render_cursor) //ESC
			{
				render_cursor = true;
			}
		}
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
		{
			render_cursor = false;
		}

	}
	else
	{
		Engine::render_manager->render_game = false;
	}

	ImGui::End();
}

void Editor::Controller_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	if (ImGui::Begin(u8"アニメーター", NULL, window_flags))
	{
		if (auto& c = controller.lock())
		{
			c->Render_ImGui();
		}
		else
		{
			ImGui::Text(u8"アニメーションコントローラーがありません");
		}
	}

	ImGui::End();
}

//メニューバー描画
void Editor::MenuBar_Render()
{
	if (ImGui::BeginMenuBar())
	{
		Scene_File_Menu_Render();
		Scene_Setting_Menu_Render();
		ImGui::EndMenuBar();
	}
}

//シーン保存、展開メニュー描画
void Editor::Scene_File_Menu_Render()
{
	if (ImGui::BeginMenu(u8"ファイル"))
	{
		if (ImGui::MenuItem(u8"新規シーン"))
		{
			if (!Engine::scene_manager->run && !Engine::scene_manager->pause)
			{
				string path = System_Function::Get_Save_File_Name("bin", "\\Resouces\\Scene");
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
		if (ImGui::MenuItem(u8"開く"))
		{
			if (!Engine::scene_manager->run && !Engine::scene_manager->pause)
			{
				string path = System_Function::Get_Open_File_Name("bin", "\\Resouces\\Scene");
				if (path != "")
				{
					Engine::scene_manager->last_save_path = path;
					Engine::scene_manager->Load_Scene(path);
					Select_Reset();
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->last_save_path;
					}
				}
				else
				{
					Debug::Log(u8"ファイルが開けませんでした");
				}
			}
		}
		if (ImGui::MenuItem(u8"上書き保存"))
		{
			if (!Engine::scene_manager->run && !Engine::scene_manager->pause)
			{
				if (Engine::scene_manager->last_save_path != "")
				{
					Engine::scene_manager->Save_Scene(Engine::scene_manager->last_save_path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->last_save_path;
					}
					Debug::Log(u8"シーンの保存に成功");
				}
				else
				{
					string path = System_Function::Get_Save_File_Name("bin", "\\Resouces\\Scene");
					if (path != "")
					{
						int path_i = path.find_last_of("\\") + 1;//7
						int ext_i = path.find_last_of(".");//10
						string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
						string filename = path.substr(path_i, ext_i - path_i); //ファイル名
						path = pathname + filename + ".bin";
						Engine::scene_manager->Save_Scene(path);
						ofstream oOfstream("Default_Resource\\System\\last_save.bin");
						if (oOfstream.is_open())
						{
							// ファイルへ書き込む
							oOfstream << Engine::scene_manager->last_save_path;
						}
						Debug::Log(u8"シーンの保存に成功");
					}
				}
			}
		}
		if (ImGui::MenuItem(u8"名前をつけて保存"))
		{
			if (!Engine::scene_manager->run && !Engine::scene_manager->pause)
			{
				string path = System_Function::Get_Save_File_Name();
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;//7
					int ext_i = path.find_last_of(".");//10
					string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
					string filename = path.substr(path_i, ext_i - path_i); //ファイル名
					path = pathname + filename + ".bin";
					Engine::scene_manager->Save_Scene(path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->last_save_path;
					}
					Debug::Log(u8"シーンの保存に成功");
				}
			}
		}
		ImGui::EndMenu();
	}
}

//シーン設定メニュー
void Editor::Scene_Setting_Menu_Render()
{
	if (ImGui::BeginMenu(u8"シーン設定"))
	{
		const unique_ptr<Project_Settings>& settings = Engine::scene_manager->settings;
		if (ImGui::BeginMenu(u8"レンダリング"))
		{
			if (ImGui::BeginMenu(u8"シャドウ"))
			{
				if (ImGui::DragFloat(u8"バイアス", &settings->shadow_bias, 0.000001f, 0, FLT_MAX, "%.9f"))
				{
					Engine::shadow_manager->shadow_bias = settings->shadow_bias;
					Engine::scene_manager->Save_Settings();
				}
				if (ImGui::DragFloat(u8"距離", &settings->shadow_distance, 0.1f, 1.0f))
				{
					if (settings->shadow_distance <= 0)
					{
						settings->shadow_distance = 1;
					}
					Engine::shadow_manager->shadow_distance = settings->shadow_distance;
					Engine::scene_manager->Save_Settings();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::SetNextWindowSize({ 250,300 });
		if (ImGui::BeginMenu(u8"タグ"))
		{
			if (ImGui::CollapsingHeader(u8"タグ一覧"))
			{
				ImGui::Indent();
				bool erase_tag = false;
				int erase_it = 0;
				vector<string>& tag = settings->tag;
				for (size_t i = 0; i < tag.size(); ++i)
				{
					ImGui::Spacing();
					ImGui::PushID(i);
					const string name = to_string(i) + ": " + tag[i];
					ImGui::Text(name.c_str());
					if (i != 0)
					{
						ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
						if (ImGui::Button(u8" × "))
						{
							erase_tag = true;
							erase_it = i;
						}
						ImGui::PopStyleColor(1);
					}
					ImGui::PopID();
				}
				if (erase_tag)
				{
					tag.erase(tag.begin() + erase_it);
					Engine::scene_manager->Save_Settings();
				}
				ImGui::Unindent();
				ImGui::Dummy({});
				ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 30.0f);
				if (ImGui::Button(u8"追加"))
				{
					ImGui::OpenPopup(u8"タグ新規入力");
				}
				if (ImGui::BeginPopup(u8"タグ新規入力"))
				{
					static string tag_name;
					ImGui::InputText(u8"新規タグ名", &tag_name);
					if (ImGui::Button(u8"追加") && !tag_name.empty())
					{
						tag.emplace_back(tag_name);
						tag_name.clear();
						Engine::scene_manager->Save_Settings();
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
			ImGui::EndMenu();
		}

		ImGui::SetNextWindowSize({ 250,300 });
		if (ImGui::BeginMenu(u8"レイヤー"))
		{
			array<string, 32>& layer = settings->layer;
			array<int, 32>& layer_mask = settings->layer_mask;
			size_t layer_size = 0;
			for (; layer_size < 32; ++layer_size)
			{
				if (layer[layer_size].empty()) break;
			}

			if (ImGui::CollapsingHeader(u8"レイヤー一覧"))
			{
				ImGui::Indent();
				bool erase_layer = false;
				int erase_it = 0;
				for (size_t i = 0; i < layer_size; ++i)
				{
					ImGui::Spacing();
					ImGui::PushID(i);
					const string name = to_string(i) + ": " + layer[i];
					ImGui::Text(name.c_str());
					if (i != 0)
					{
						ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.0f, 0.0f, 1.0f });
						if (ImGui::Button(u8" × "))
						{
							erase_layer = true;
							erase_it = i;
						}
						ImGui::PopStyleColor(1);
					}
					ImGui::PopID();
				}
				if (erase_layer)
				{
					layer[erase_it].clear();
					layer_mask[erase_it] = -1;
					Engine::scene_manager->Save_Settings();
				}
				ImGui::Unindent();
				if (layer[31].empty())
				{
					ImGui::Dummy({});
					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 30.0f);
					if (ImGui::Button(u8"追加"))
					{
						ImGui::OpenPopup(u8"レイヤー新規入力");
					}
					if (ImGui::BeginPopup(u8"レイヤー新規入力"))
					{
						static string layer_name;
						ImGui::InputText(u8"新規レイヤー名", &layer_name);
						if (ImGui::Button(u8"追加") && !layer_name.empty())
						{
							layer[layer_size] = layer_name;
							layer_name.clear();
							Engine::scene_manager->Save_Settings();
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
				}
			}
			ImGui::EndMenu();
		}

		ImGui::SetNextWindowSize({ 250,400 });
		if (ImGui::BeginMenu(u8"物理"))
		{
			array<string, 32>& layer = settings->layer;
			array<int, 32>& layer_mask = settings->layer_mask;

			if (ImGui::CollapsingHeader(u8"衝突設定"))
			{
				const float window_width = ImGui::GetWindowContentRegionWidth() - 20;

				size_t layer_size = 0;
				for (; layer_size < 32; ++layer_size)
				{
					if (layer[layer_size].empty()) break;
				}

				vector<array<bool, 32>> layer_list;
				layer_list.resize(layer_size);
				for (size_t i = 0; i < layer_list.size(); ++i)
				{
					for (size_t j = 0; j < 32; ++j)
					{
						layer_list[i][j] = layer_mask[i] & (1 << j);
					}
				}

				for (size_t i = 0; i < layer_size; ++i)
				{
					if (ImGui::TreeNode(layer[i].c_str()))
					{
						for (size_t j = i; j < layer_size; ++j)
						{
							const string id = layer[i] + to_string(j);
							ImGui::PushID(id.c_str());
							const string label = " VS : " + layer[j];
							ImGui::Text(label.c_str());
							ImGui::SameLine(window_width);
							if (ImGui::Checkbox("##Check", &layer_list[i][j]))
							{
								if (layer_list[i][j])
								{
									layer_mask[i] |= (1 << j);
									layer_mask[j] |= (1 << i);
								}
								else
								{
									layer_mask[i] &= ~(1 << j);
									layer_mask[j] &= ~(1 << i);
								}
							}
							ImGui::PopID();
						}
						ImGui::TreePop();
					}
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

//オブジェクトツリー描画
void Editor::GameObject_Tree_Render(const shared_ptr<GameObject>& obj, int flag)
{
	ImGui::PushID(obj->Get_Instance_ID().c_str());
	const ImGuiTreeNodeFlags in_flag = flag;
	if (auto& active_obj = active_object.lock())
	{
		if (active_obj == obj) flag |= ImGuiTreeNodeFlags_Selected;
	}

	bool active = obj->Get_Active();
	if (active) active = obj->Get_Active_In_Hierarchy();

	if (obj->transform->Get_Child_Count())
	{
		if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
		bool open = ImGui::TreeNodeEx(obj->name.c_str(), flag);
		if (!active) { ImGui::PopStyleColor(); }

		if (auto& drag = draging_object.lock())
		{
			if (drag == obj && ImGui::IsMouseReleased(0) && ImGui::IsItemHovered())
			{
				Activate_Select_Object(obj);
				select_object = true;
			}
		}

		if (ImGui::IsItemClicked(1))
		{
			Activate_Select_Object(obj);
			select_object = true;
		}

		if (ImGui::IsItemClicked())
		{
			draging_object = obj;
		}

		GameObject_DragMenu_Render(obj);

		ImGui::PopID();
		if (open)
		{
			for (size_t i = 0; i < obj->transform->children.size(); ++i)
			{
				GameObject_Tree_Render(obj->transform->children[i].lock()->gameobject, in_flag);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

		if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
		ImGui::TreeNodeEx(obj->name.c_str(), flag);
		if (!active) { ImGui::PopStyleColor(); }

		if (auto& drag = draging_object.lock())
		{
			if (drag == obj && ImGui::IsMouseReleased(0) && ImGui::IsItemHovered())
			{
				Activate_Select_Object(obj);
				select_object = true;
			}
		}

		if (ImGui::IsItemClicked(1))
		{
			Activate_Select_Object(obj);
			select_object = true;
		}

		if (ImGui::IsItemClicked())
		{
			draging_object = obj;
		}

		GameObject_DragMenu_Render(obj);
		ImGui::PopID();
	}
}

shared_ptr<GameObject> Editor::Get_Drag_Object()
{
	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDrop_Object"))
	{
		if (auto& obj = draging_object.lock())
		{
			return obj;
		}
	}
	return nullptr;
}

//ヒエラルキーでのオブジェクトドラッグ
void Editor::GameObject_DragMenu_Render(const shared_ptr<GameObject>& obj)
{
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("DragDrop_Object", nullptr, 0);
		ImGui::EndDragDropSource();
	}
	string Menu_Label = "Drag_Object_Menu_";
	Menu_Label += obj->name;
	if (ImGui::BeginDragDropTarget())
	{
		select_object = true;
		if (const auto& drag = Get_Drag_Object())
		{
			if (drag != obj)
			{
				ImGui::OpenPopup(Menu_Label.c_str());
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopup(Menu_Label.c_str()))
	{
		string obj_name = obj->name;

		string s1 = u8"の子に設定";
		string s2 = u8"の上に移動";
		string s3 = u8"の下に移動";

		string s_set = obj_name + s1;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (auto& drag_obj = draging_object.lock())
			{
				drag_obj->transform->Set_Parent(obj->transform);
				draging_object.reset();
			}
		}

		s_set = obj_name + s2;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (auto& drag_obj = draging_object.lock())
			{
				drag_obj->transform->Set_Parent(obj->transform->parent.lock());
				drag_obj->transform->Set_Sibling_Index(obj->transform->Get_Sibling_Index());
				draging_object.reset();
			}
		}

		s_set = obj_name + s3;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (auto& drag_obj = draging_object.lock())
			{
				drag_obj->transform->Set_Parent(obj->transform->parent.lock());
				drag_obj->transform->Set_Sibling_Index(obj->transform->Get_Sibling_Index() + 1);
				draging_object.reset();
			}
		}
		ImGui::EndPopup();
	}
}

//ショートカットキーチェック
void Editor::ShortCut_Check()
{
	if (ImGui::IsKeyDown(17)) //Ctrl
	{
		if (!Engine::scene_manager->run && !Engine::scene_manager->pause)
		{
			if (ImGui::IsKeyPressed(83)) //S
			{
				if (Engine::scene_manager->last_save_path != "")
				{
					Engine::scene_manager->Save_Scene(Engine::scene_manager->last_save_path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->last_save_path;
					}
					Debug::Log(u8"シーンの保存に成功");
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
						Engine::scene_manager->Save_Scene(path);
						ofstream oOfstream("Default_Resource\\System\\last_save.bin");
						if (oOfstream.is_open())
						{
							// ファイルへ書き込む
							oOfstream << Engine::scene_manager->last_save_path;
						}
						Debug::Log(u8"シーンの保存に成功");
					}
				}
			}
		}

		if (ImGui::IsKeyPressed(80)) //P
		{
			if (ImGui::IsKeyDown(16)) //Shift
			{
				if (Engine::scene_manager->run)
				{
					render_cursor = true;
					Engine::scene_manager->run = false;
					Engine::scene_manager->pause = true;
					Engine::audio_manager->Suspend();
				}
			}
			else
			{
				if (Engine::scene_manager->run)
				{
					Cursor::cursor_lock_mode = CursorLock_Mode::None;
					Cursor::visible = true;
					render_cursor = true;
					Select_Reset();
					Engine::scene_manager->run = false;
					Engine::scene_manager->pause = false;
					Engine::scene_manager->End_Debug_Scene();
				}
				else
				{
					if (!Engine::scene_manager->pause)
					{
						Cursor::cursor_lock_mode = CursorLock_Mode::None;
						Cursor::visible = true;
						ImGui::SetWindowFocus(u8"ゲーム");
						Select_Reset();
						Engine::scene_manager->Start_Debug_Scene();
					}
					else
					{
						Engine::audio_manager->Resume();
					}
					render_cursor = false;
					Engine::scene_manager->run = true;
					Engine::scene_manager->pause = false;
				}
			}
		}
	}
}

//シーンビューのカメラ操作
void Editor::Debug_Camera_Update()
{
	if (ImGui::IsWindowHovered())
	{
		if (ImGui::IsMouseDown(1))
		{
			ImGui::SetWindowFocus();
		}
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			float mouse_wheel = io.MouseWheel;
			if (mouse_wheel != 0.0f)
			{
				Vector3 move = debug_camera_transform->Get_Forward() * mouse_wheel * 5;
				debug_camera_transform->Set_Position(debug_camera_transform->Get_Position() + move);
			}
		}
	}

	if (ImGui::IsWindowFocused())
	{
		ImGuiIO& io = ImGui::GetIO();
		//入力
		{
			static ImVec2 mouse_old_pos = { 0,0 };
			static ImVec2 mouse_pos = { 0,0 };
			if (ImGui::IsMouseDown(1))
			{
				mouse_pos = ImGui::GetMousePos();
				if (mouse_old_pos.x == -1 && mouse_old_pos.y == -1)
				{
					mouse_old_pos = mouse_pos;
				}
				if (mouse_pos.x != mouse_old_pos.x || mouse_pos.y != mouse_old_pos.y)
				{
					const float dis_x = (mouse_pos.x - mouse_old_pos.x) * 0.15f;
					const float dis_y = (mouse_pos.y - mouse_old_pos.y) * 0.15f;

					Vector3 rot = debug_camera_transform->Get_Euler_Angles();
					rot.y -= dis_x;
					rot.x += dis_y;
					rot.z = 0;
					debug_camera_transform->Set_Local_Euler_Angles(rot);

					mouse_old_pos = mouse_pos;
				}

				Vector3 move = { 0,0,0 };
				static float speed = 5;
				float mouse_wheel = io.MouseWheel;
				if (mouse_wheel != 0.0f)
				{
					speed += mouse_wheel * 2;
					speed = Mathf::Clamp(speed, 1, 10);
				}
				if (ImGui::IsKeyDown(87))
				{
					move += debug_camera_transform->Get_Forward() * Time::delta_time * speed;
				}
				if (ImGui::IsKeyDown(83))
				{
					move -= debug_camera_transform->Get_Forward() * Time::delta_time * speed;
				}
				if (ImGui::IsKeyDown(65))
				{
					move += debug_camera_transform->Get_Right() * Time::delta_time * speed;
				}
				if (ImGui::IsKeyDown(68))
				{
					move -= debug_camera_transform->Get_Right() * Time::delta_time * speed;
				}
				debug_camera_transform->Set_Local_Position(debug_camera_transform->Get_Position() + move);
			}
			else
			{
				mouse_old_pos = { -1,-1 };
			}
		}
	}
}

void Editor::Select_Reset()
{
	Set_Debug_Draw(false, active_object.lock());
	active_object.reset();
	active_object_old.reset();
	controller.reset();
}

void Editor::Set_Debug_Draw(bool value, const shared_ptr<GameObject>& obj)
{
	if (obj)
	{
		if (auto& col = obj->Get_Component<Collider>())
		{
			col->Set_Debug_Draw(value);
		}
		for (auto& child : obj->transform->children)
		{
			Set_Debug_Draw(value, child.lock()->gameobject);
		}
	}
}

void Editor::Activate_Select_Object(const shared_ptr<GameObject>& obj)
{
	if (auto& now = active_object.lock())
	{
		Set_Debug_Draw(false, now);
	}
	active_object = obj;
	if (auto& animator = obj->Get_Component<Animator>())
	{
		if (animator->controller)
		{
			controller = animator->controller;
		}
	}

	Set_Debug_Draw(true, obj);
}