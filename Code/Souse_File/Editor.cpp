#include "DxSystem.h"
#include "Editor.h"
#include "Debug.h"
#include "Original_Math.h"
#include "Scene_Manager.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Particle_Manager.h"
#include "View_Scene.h"
#include "View_Game.h"
#include "FBX_Converter.h"
#include "All_Component_List.h"
#include "Resources.h"
#include "Cursor.h"
#include "Transform.h"
#include "Scene.h"
#include "Include_ImGui.h"
#include <ImGuizmo.h>
#include "System_Function.h"
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

	//カメラ行列初回計算
	{
		// プロジェクション行列を作成
		// 画面サイズ取得のためビューポートを取得
		{
			// 角度をラジアン(θ)に変換
			fov_y = XMConvertToRadians(30);	// 画角
			aspect = (float)Engine::view_scene->screen_x / (float)Engine::view_scene->screen_y;	// 画面比率
			near_z = 0.1f;
			far_z = 1000.0f;

			//Debug_Camera_P = XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z);
			debug_camera_projection_matrix = XMMatrixPerspectiveFovRH(fov_y, aspect, near_z, far_z);

			//XMStoreFloat4x4(&Debug_Camera_P, XMMatrixOrthographicLH((float)Engine::view_scene->screen_x, (float)Engine::view_scene->screen_y, 0.1f, 1000.0f));
		}
		// ビュー行列を作成
		// カメラの設定
		{
			Vector3 eye_v = debug_camera_transform->Get_Position();
			Vector3 focus_v = eye_v + debug_camera_transform->Get_Forward();

			XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
			camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
			camForward = XMVector3Normalize(camForward);
			XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

			XMVECTOR up_v = debug_camera_transform->Get_Up();
			debug_camera_view_matrix = XMMatrixLookAtRH(eye_v, focus_v, up_v);
		}
	}
}

Editor::~Editor()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Editor::Update(const unique_ptr<Scene>& scene)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();

	Main_Window_Render();
	//ゲームオブジェクト関連
	{
		//ヒエラルキー
		Hierarchy_Render(scene);
		//インスペクタ
		Inspector_Render();
	}

	//シーン再生UI
	ScenePlayer_Render();

	//アセットマネージャー
	FileResource_Render();

	//描画
	{
		GameView_Render();
		SceneView_Render();
	}
	//デバッグログ
	Debug_Log_Render();

	//ショートカットキーチェック
	ShortCut_Check();
}

void Editor::Render()
{
	Engine::particle_manager->Camera_Update(debug_camera_transform, fov_y, near_z, far_z, aspect);
	Engine::view_scene->Render(debug_camera_view_matrix, debug_camera_projection_matrix, debug_camera_transform);

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

	void Draw(const char* title, bool* p_open = NULL)
	{
		if (!ImGui::Begin(title, p_open))
		{
			ImGui::End();
			return;
		}

		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");

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
		ImGui::End();
	}
};

//デバッグログ描画
void Editor::Debug_Log_Render()
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	static bool Open_Log = true;
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
	logger.Draw((u8"コンソール"), &Open_Log);
}

//ヒエラルキー描画
void Editor::Hierarchy_Render(const unique_ptr<Scene>& scene)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"ヒエラルキー", NULL, window_flags);

	static int ID = 0;

	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if (ImGui::TreeNode(scene->name.c_str()))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selecting = -1;
		bool Item_Clicked = false;

		for (size_t i = 0; i < scene->gameobject_list.size();++i)
		{
			if (scene->gameobject_list[i]->transform->parent.expired())
			{
				GameObject_Tree_Render(ID, scene->gameobject_list[i], selecting, base_flags, Item_Clicked);
			}
		}

		if (!Item_Clicked && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
		{
			active_object.reset();
			active_object_old.reset();
			selecting = -1;
		}

		if (ImGui::BeginPopupContextWindow(u8"ヒエラルキーサブ"))
		{
			if (ImGui::Selectable(u8"新規オブジェクトを追加"))
			{
				scene->Instance_GameObject(u8"GameObject");
			}
			if (ImGui::Selectable(u8"プレハブを読み込む"))
			{
				string path = System_Function::Get_Open_File_Name("prefab","\\Resouces\\Prefab");
				if (path != "")
				{
					Resources::Load_Prefab(path);
				}
			}
			ImGui::Separator();
			if (!active_object.expired())
			{
				if (ImGui::Selectable(u8"オブジェクトを削除"))
				{
					GameObject::Destroy(active_object.lock());
					active_object.reset();
					active_object_old.reset();
					selecting = -1;
				}

				if (!active_object.expired())
				{
					if (ImGui::Selectable(u8"オブジェクトを複製"))
					{
						shared_ptr<GameObject> obj = active_object.lock();
						weak_ptr<Transform> parent = obj->transform->Get_Parent();
						if (!parent.expired())
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

						if (!parent.expired())
						{
							obj->transform->Set_Parent(parent.lock());
							Resources::Load_Prefab("Default_Resource\\System\\copy.prefab")->transform->Set_Parent(parent.lock());
						}
						else
						{
							Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
						}
					}

					if (ImGui::Selectable(u8"オブジェクトをプレハブ化"))
					{
						Resources::Create_Prefab(active_object.lock());
					}

					if (!active_object.lock()->transform->parent.expired())
					{
						if (ImGui::Selectable(u8"オブジェクトの親子関係を解除"))
						{
							active_object.lock()->transform->Set_Parent(nullptr);
						}
					}
				}
			}
			ImGui::EndPopup();
		}
		ImGui::TreePop();

		if (ImGui::IsWindowFocused() && !active_object.expired())
		{
			if (ImGui::IsKeyDown(17) && ImGui::IsKeyPressed(68)) //Ctrl + D
			{
				shared_ptr<GameObject> obj = active_object.lock();
				weak_ptr<Transform> parent = obj->transform->Get_Parent();
				if (!parent.expired())
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

				if (!parent.expired())
				{
					obj->transform->Set_Parent(parent.lock());
					Resources::Load_Prefab("Default_Resource\\System\\copy.prefab")->transform->Set_Parent(parent.lock());
				}
				else
				{
					Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
				}
			}

			if (ImGui::IsKeyPressed(46)) //Delete
			{
				GameObject::Destroy(active_object.lock());
				active_object.reset();
				active_object_old.reset();
				selecting = -1;
			}
		}
	}
	ID = 0;

	ImGui::End();
}

//インスペクタ描画
void Editor::Inspector_Render()
{
	ImGui::SetNextWindowPos(ImVec2(1500, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	shared_ptr<GameObject> obj = active_object.lock();
	shared_ptr<GameObject> obj_old = active_object_old.lock();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"インスペクタ", NULL, window_flags);
	if (obj)
	{
		static bool active = false;
		if (obj_old != obj)
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
		ImGui::InputText(u8"名前", &obj->name);

		static int ID = 0;
		bool removed = true;
		while (removed)
		{
			bool removed_comp = false;
			for (shared_ptr<Component> c : obj->component_list)
			{
				ImGui::PushID(ID);
				if (!c->Draw_ImGui())
				{
					removed_comp = true;
					ImGui::PopID();
					break;
				}
				ImGui::PopID();
				++ID;
			}
			removed = removed_comp;
		}
		ID = 0;
		ImGui::Separator();
		All_Component_List::Add(obj);
	}
	else
	{
		ImGui::Text(u8"オブジェクトが選択されていません");
	}

	ImGui::End();
}

//シーン再生UI描画
void Editor::ScenePlayer_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"シーン再生", NULL, window_flags);

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
				active_object.reset();
				active_object_old.reset();
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
			active_object.reset();
			active_object_old.reset();
			render_cursor = true;
			Engine::scene_manager->run = false;
			Engine::scene_manager->pause = false;
			Engine::scene_manager->End_Debug_Scene();
		}
	}

	ImGui::End();
}

//シーンビュー描画
void Editor::SceneView_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

	ImGui::Begin(u8"シーン", NULL, window_flags);
	//const float titleBarHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f + 10;
	const float window_width = ImGui::GetCurrentWindow()->InnerRect.GetWidth() - 8;
	const float window_height = ImGui::GetCurrentWindow()->InnerRect.GetHeight() - 8;
	Engine::view_scene->Set_Screen_Size((int)window_width, (int)window_height);
	ImGui::Image((void*)Engine::view_scene->shader_resource_view_render.Get(), ImVec2(window_width, window_height));

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

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
				Vector3 move = debug_camera_transform->Get_Forward() * mouse_wheel * 10;
				debug_camera_transform->Set_Position(debug_camera_transform->Get_Position() + move);
			}
		}
	}

	if (ImGui::IsWindowFocused())
	{
		Debug_Camera_Update();

		if (!ImGui::IsMouseDown(1))
		{
			if (ImGui::IsKeyPressed(87)) //w
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed(69)) //e
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed(82)) //r
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			if (ImGui::IsKeyPressed(88)) //x
			{
				if (mCurrentGizmoMode == ImGuizmo::LOCAL)
				{
					mCurrentGizmoMode = ImGuizmo::WORLD;
				}
				else
				{
					mCurrentGizmoMode = ImGuizmo::LOCAL;
				}
			}
		}
	}

	//カメラ行列計算
	{
		// プロジェクション行列を作成
		// 画面サイズ取得のためビューポートを取得
		{
			// 角度をラジアン(θ)に変換
			fov_y = XMConvertToRadians(30);	// 画角
			aspect = (float)Engine::view_scene->screen_x / (float)Engine::view_scene->screen_y;	// 画面比率
			near_z = 0.1f;
			far_z = 1000.0f;

			//Debug_Camera_P = XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z);
			debug_camera_projection_matrix = XMMatrixPerspectiveFovRH(fov_y, aspect, near_z, far_z);

			//XMStoreFloat4x4(&Debug_Camera_P, XMMatrixOrthographicLH((float)Engine::view_scene->screen_x, (float)Engine::view_scene->screen_y, 0.1f, 1000.0f));
		}
		// ビュー行列を作成
		// カメラの設定
		{
			Vector3 eye_v = debug_camera_transform->Get_Position();
			Vector3 focus_v = eye_v + debug_camera_transform->Get_Forward();

			XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
			camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
			camForward = XMVector3Normalize(camForward);
			XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

			XMVECTOR up_v = debug_camera_transform->Get_Up();
			debug_camera_view_matrix = XMMatrixLookAtRH(eye_v, focus_v, up_v);
		}
	}

	if (!active_object.expired())
	{
		static Matrix matrix;

		shared_ptr<Transform> trans = active_object.lock()->transform;
		matrix = trans->Get_World_Matrix();

		ImVec2 winPos = ImGui::GetWindowPos();
		ImGuizmo::SetRect(winPos.x, winPos.y, window_width, window_height);
		ImGuizmo::SetDrawlist();

		// 選択ノードの行列を操作する
		ImGuizmo::Manipulate(
			&debug_camera_view_matrix._11, &debug_camera_projection_matrix._11,	// ビュー＆プロジェクション行列
			mCurrentGizmoOperation,		// 移動操作
			mCurrentGizmoMode,			// ローカル空間での操作
			&matrix._11,	// 操作するワールド行列
			nullptr);

		if (ImGuizmo::IsUsing())
		{
			trans->Set_World_Matrix(matrix);
		}
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

	ImGui::Begin(u8"ゲーム", NULL, window_flags);


	ImGuiWindow* p_win = ImGui::GetCurrentWindow();
	game_view_size = { p_win->InnerRect.GetWidth() - 8, p_win->InnerRect.GetHeight() - 8 };

	const ImVec2 pos = ImGui::GetCursorScreenPos();
	game_view_position = { pos.x,pos.y + game_view_size.y };

	Engine::view_game->Set_Screen_Size(static_cast<int>(game_view_size.x), static_cast<int>(game_view_size.y));
	ImGui::Image((void*)Engine::view_game->shader_resource_view_render.Get(), ImVec2(game_view_size.x, game_view_size.y));

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

	ImGui::End();
}

void Editor::FileResource_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"アセット(仮)", NULL, window_flags);

	if (ImGui::Button(u8"FBX読み込み"))
	{
		string path = System_Function::Get_Open_File_Name("fbx","\\Resouces\\Model");
		if (path != "")
		{
			int path_i = path.find_last_of("\\") + 1;//7
			int ext_i = path.find_last_of(".");//10
			string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
			string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
			string filename = path.substr(path_i, ext_i - path_i); //ファイル名
			if (extname == ".fbx" || extname == ".mesh")
			{
				FBX_Converter::Convert_From_FBX(pathname.c_str(), filename.c_str());
			}
			else
			{
				Debug::Log("ファイル形式が対応していません");
			}
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
		if (ImGui::MenuItem(u8"開く"))
		{
			if (!Engine::scene_manager->run && !Engine::scene_manager->pause)
			{
				string path = System_Function::Get_Open_File_Name("bin","\\Resouces\\Scene");
				if (path != "")
				{
					Engine::scene_manager->last_save_path = path;
					Scene_Manager::LoadScene(path);
					active_object.reset();
					active_object_old.reset();
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

//オブジェクトツリー描画
void Editor::GameObject_Tree_Render(int& ID, const shared_ptr<GameObject>& obj, int& selecting, int flag, bool& Item_Clicked)
{
	++ID;
	const ImGuiTreeNodeFlags in_flag = flag;
	if (selecting == ID) flag |= ImGuiTreeNodeFlags_Selected;

	bool active = obj->Get_Active();
	if (active) active = obj->Get_Active_In_Hierarchy();

	if (obj->transform->Get_Child_Count())
	{
		bool open = false;
		if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
		open = ImGui::TreeNodeEx((void*)(intptr_t)obj.get(), flag, obj->name.c_str());
		if (!active) { ImGui::PopStyleColor(); }

		if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
		{
			selecting = ID;
			active_object = obj;
		}
		if (ImGui::IsItemClicked())
		{
			Item_Clicked = true;
		}

		GameObject_DragMenu_Render(obj);

		if (open)
		{
			for (size_t i = 0; i < obj->transform->children.size();++i)
			{
				GameObject_Tree_Render(ID, obj->transform->children[i].lock()->gameobject, selecting, in_flag, Item_Clicked);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

		if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
		ImGui::TreeNodeEx((void*)(intptr_t)obj.get(), flag, obj->name.c_str());
		if (!active) { ImGui::PopStyleColor(); }

		if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
		{
			selecting = ID;
			active_object = obj;
		}

		GameObject_DragMenu_Render(obj);
	}
}

//ヒエラルキーでのオブジェクトドラッグ
void Editor::GameObject_DragMenu_Render(const std::shared_ptr<GameObject>& obj)
{
	if (ImGui::BeginDragDropSource())
	{
		draging_object = obj;
		int a = 0;
		ImGui::SetDragDropPayload("DragDrop_Object", &a, sizeof(int));
		ImGui::EndDragDropSource();
	}
	string Menu_Label = "Drag_Object_Menu_";
	Menu_Label += obj->name;
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDrop_Object"))
		{
			if (!draging_object.expired() && draging_object.lock() != obj)
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
			if (!draging_object.expired())
			{
				draging_object.lock()->transform->Set_Parent(obj->transform);
				draging_object.reset();
			}
		}

		s_set = obj_name + s2;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (!draging_object.expired())
			{
				shared_ptr<GameObject> drag_obj = draging_object.lock();
				drag_obj->transform->Set_Parent(obj->transform->parent.lock());
				drag_obj->transform->Set_Sibling_Index(obj->transform->Get_Sibling_Index());
				draging_object.reset();
			}
		}

		s_set = obj_name + s3;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (!draging_object.expired())
			{
				shared_ptr<GameObject> drag_obj = draging_object.lock();
				shared_ptr<Transform> parent = obj->transform->parent.lock();
				drag_obj->transform->Set_Parent(parent);
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
					active_object.reset();
					active_object_old.reset();
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
						active_object.reset();
						active_object_old.reset();
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
	ImGuiIO& io = ImGui::GetIO();
	//入力
	{
		static ImVec2 mouse_old_pos = { 0,0 };
		static ImVec2 mouse_pos = { 0,0 };
		Vector3 move = { 0,0,0 };
		if (ImGui::IsMouseDown(1))
		{
			mouse_pos = ImGui::GetMousePos();
			if (mouse_old_pos.x == -1 && mouse_old_pos.y == -1)
			{
				mouse_old_pos = mouse_pos;
			}
			if (mouse_pos.x != mouse_old_pos.x || mouse_pos.y != mouse_old_pos.y)
			{
				const float dis_x = (mouse_pos.x - mouse_old_pos.x) * 0.1f;
				const float dis_y = (mouse_pos.y - mouse_old_pos.y) * 0.1f;

				Vector3 rot = debug_camera_transform->Get_Euler_Angles();
				rot.y -= dis_x;
				rot.x += dis_y;
				rot.z = 0;
				debug_camera_transform->Set_Euler_Angles(rot);

				mouse_old_pos = mouse_pos;
			}

			static float speed = 10;
			float mouse_wheel = io.MouseWheel;
			if (mouse_wheel != 0.0f)
			{
				speed += mouse_wheel * 3;
				if (speed < 5)
				{
					speed = 5;
				}
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
			debug_camera_transform->Set_Position(debug_camera_transform->Get_Position() + move);
		}
		else
		{
			mouse_old_pos = { -1,-1 };
		}
	}
}