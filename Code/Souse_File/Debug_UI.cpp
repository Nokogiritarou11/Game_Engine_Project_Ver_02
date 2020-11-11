#include "DxSystem.h"
#include "Debug_UI.h"
#include "Debug.h"
#include "Original_Math.h"
#include "Scene_Manager.h"
#include "Engine.h"
#include "Particle_Manager.h"
#include "View_Scene.h"
#include "View_Game.h"
#include "All_Component_List.h"
#include "Resources.h"
#include "Include_ImGui.h"
#include <ImGuizmo.h>
#include "System_Function.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;
using namespace DirectX;

Debug_UI::Debug_UI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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

	if (!ImGui_ImplDX11_Init(DxSystem::Device.Get(), DxSystem::DeviceContext.Get()))
	{
		std::cout << "ImGui_ImplDX11_Init failed\n";
		ImGui::DestroyContext();
		std::exit(1);
	}

	//日本語フォントに対応
	ImFont* font = io.Fonts->AddFontFromFileTTF(Font_Name, Font_Size_Pixels, nullptr, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);

	io.FontGlobalScale = Font_Size; // フォントの大きさを一括で変更できます。
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(UI_Size); // UIの大きさを一括で変更できます。

	float size_icon = 20.0f; // 30.0fにすると大きすぎたので20.0fにしています。
	ImFontConfig config;
	config.MergeMode = true; // フォントテクスチャを合体させます。
	config.PixelSnapH = true;
	config.GlyphMinAdvanceX = size_icon; // アイコンを等幅にします。
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("Default_Resource/Font/fontawesome-webfont.ttf", size_icon, &config, icon_ranges);
	io.Fonts->Build();

	Vector3 p = { -50, 40, -50 };
	Vector3 e = { 15, 45, 0 };
	Debug_Camera_Transform = make_unique<Transform>(p, e);

	//カメラ行列初回計算
	{
		// プロジェクション行列を作成
		// 画面サイズ取得のためビューポートを取得
		{
			// 角度をラジアン(θ)に変換
			fov_y = XMConvertToRadians(30);	// 画角
			aspect = (float)Engine::view_scene->screen_x / (float)Engine::view_scene->screen_y;	// 画面比率
			near_z = 0.1f;
			far_z = 100000.0f;

			XMStoreFloat4x4(&Debug_Camera_P, XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z));

			//XMStoreFloat4x4(&Debug_Camera_P, XMMatrixOrthographicLH((float)Engine::view_scene->screen_x, (float)Engine::view_scene->screen_y, 0.1f, 1000.0f));
		}
		// ビュー行列を作成
		// カメラの設定
		{
			Vector3 pos = Debug_Camera_Transform->Get_position();
			Vector4 eye = { pos.x,pos.y,pos.z ,0 };
			XMVECTOR eye_v = XMLoadFloat4(&eye);

			XMVECTOR focus_v = eye_v + XMLoadFloat3(&Debug_Camera_Transform->Get_forward());

			XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
			camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
			camForward = XMVector3Normalize(camForward);
			XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

			XMVECTOR up_v = Debug_Camera_Transform->Get_up();
			XMStoreFloat4x4(&Debug_Camera_V, XMMatrixLookAtLH(eye_v, focus_v, up_v));
		}
	}
}

Debug_UI::~Debug_UI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Debug_UI::Update(const unique_ptr<Scene>& scene)
{
	if (Draw_Debug_UI)
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
}

void Debug_UI::Render()
{
	if (Draw_Debug_UI)
	{
		Engine::particle_manager->Update(Debug_Camera_Transform, fov_y, near_z, far_z, aspect);
		Engine::view_scene->Render(Debug_Camera_V, Debug_Camera_P, Debug_Camera_Transform);

		// レンダーターゲットビュー設定
		DxSystem::SetDefaultView();
		DxSystem::SetViewPort(DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight());
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
	}
}

//ドッキング用親ウィンドウ描画
void Debug_UI::Main_Window_Render()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
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
void Debug_UI::Print_Log(string log)
{
	Debug_Log.push_back(log + u8"\n");
	Debug_Log_Changed = true;
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

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
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

	void    Draw(const char* title, bool* p_open = NULL)
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
void Debug_UI::Debug_Log_Render()
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	static bool Open_Log = true;
	ImGui::SetNextWindowPos(ImVec2(0, 900), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_FirstUseEver);
	static Debug_Logger logger;
	if (Debug_Log_Changed)
	{
		for (unsigned int i = 0; i < Debug_Log.size(); i++)
		{
			logger.AddLog(Debug_Log[i].c_str());
		}
		Debug_Log.clear();
	}
	logger.Draw((u8"デバッグログ"), &Open_Log);
}

//ヒエラルキー描画
void Debug_UI::Hierarchy_Render(const unique_ptr<Scene>& scene)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"ヒエラルキー", NULL, window_flags);

	GameObject_List_Render(scene);

	ImGui::End();
}

//インスペクタ描画
void Debug_UI::Inspector_Render()
{
	ImGui::SetNextWindowPos(ImVec2(1500, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	shared_ptr<GameObject> obj = Active_Object.lock();
	shared_ptr<GameObject> obj_old = Active_Object_Old.lock();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"インスペクタ", NULL, window_flags);
	if (obj)
	{
		static bool active = false;
		if (obj_old != obj)
		{
			active = obj->activeSelf();
			Active_Object_Old = Active_Object;
		}
		//選択時
		if (ImGui::Checkbox("##active", &active))
		{
			obj->SetActive(active);
		}
		ImGui::SameLine();
		ImGui::InputText(u8"名前", &obj->name);

		bool removed = true;
		while (removed)
		{
			bool removed_comp = false;
			for (shared_ptr<Component> c : obj->Component_List)
			{
				if (!c->Draw_ImGui())
				{
					removed_comp = true;
					break;
				}
			}
			removed = removed_comp;
		}
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
void Debug_UI::ScenePlayer_Render()
{
	ImGui::SetNextWindowPos(ImVec2(895, 0), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(105, 28), ImGuiCond_Appearing);
	ImGuiWindowFlags window_flags = 0;
	window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	ImGui::Begin(u8"シーン再生", NULL, window_flags);

	bool Running = Engine::scene_manager->Run;

	if (Running)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.85f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.6f, 1.0f));
	}
	if (ImGui::Button(ICON_FA_PLAY, ImVec2(30, 0)))
	{
		if (!Running)
		{
			if (!Engine::scene_manager->Pause)
			{
				Active_Object.reset();
				Active_Object_Old.reset();
				Engine::scene_manager->Start_DebugScene();
			}
			else
			{
				Engine::audio_manager->Resume();
			}
			Engine::scene_manager->Run = true;
			Engine::scene_manager->Pause = false;
		}
	}
	if (Running)
	{
		ImGui::PopStyleColor(3);
	}

	ImGui::SameLine();
	bool Pausing = Engine::scene_manager->Pause;
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
			Engine::scene_manager->Run = false;
			Engine::scene_manager->Pause = true;
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
			Active_Object.reset();
			Active_Object_Old.reset();
			Engine::scene_manager->Run = false;
			Engine::scene_manager->Pause = false;
			Engine::scene_manager->End_DebugScene();
		}
	}

	ImGui::End();
}

//シーンビュー描画
void Debug_UI::SceneView_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

	ImGui::Begin(u8"シーン", NULL, window_flags);
	//const float titleBarHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f + 10;
	const float window_width = ImGui::GetCurrentWindow()->InnerRect.GetWidth() - 8;
	const float window_height = ImGui::GetCurrentWindow()->InnerRect.GetHeight() - 8;
	Engine::view_scene->Set_Screen_Size((int)window_width, (int)window_height);
	ImGui::Image((void*)Engine::view_scene->ShaderResourceView_Render.Get(), ImVec2(window_width, window_height));

	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

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

	if (!Active_Object.expired())
	{
		static Matrix matrix;

		shared_ptr<Transform> trans = Active_Object.lock()->transform;
		matrix = trans->Get_world_matrix();

		ImVec2 winPos = ImGui::GetWindowPos();
		ImGuizmo::SetRect(winPos.x, winPos.y, window_width, window_height);
		ImGuizmo::SetDrawlist();

		// 選択ノードの行列を操作する
		ImGuizmo::Manipulate(
			&Debug_Camera_V._11, &Debug_Camera_P._11,	// ビュー＆プロジェクション行列
			mCurrentGizmoOperation,		// 移動操作
			mCurrentGizmoMode,			// ローカル空間での操作
			&matrix._11,	// 操作するワールド行列
			nullptr);

		if (ImGuizmo::IsUsing())
		{
			trans->Set_world_matrix(matrix);
		}
	}
	ImGui::End();
}

//ゲームビュー描画
void Debug_UI::GameView_Render()
{
	ImGui::SetNextWindowPos(ImVec2(200, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"ゲーム", NULL, window_flags);
	const float window_width = ImGui::GetCurrentWindow()->InnerRect.GetWidth() - 8;
	const float window_height = ImGui::GetCurrentWindow()->InnerRect.GetHeight() - 8;
	Engine::view_game->Set_Screen_Size((int)window_width, (int)window_height);
	ImGui::Image((void*)Engine::view_game->ShaderResourceView_Render.Get(), ImVec2(window_width, window_height));

	ImGui::End();
}

//メニューバー描画
void Debug_UI::MenuBar_Render()
{
	if (ImGui::BeginMenuBar())
	{
		Scene_File_Menu_Render();
		ImGui::EndMenuBar();
	}
}

//シーン保存、展開メニュー描画
void Debug_UI::Scene_File_Menu_Render()
{
	if (ImGui::BeginMenu(u8"ファイル"))
	{
		if (ImGui::MenuItem(u8"新規シーン"))
		{
			if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
			{
				string path = System_Function::Get_Save_File_Name();
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;//7
					int ext_i = path.find_last_of(".");//10
					string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
					string filename = path.substr(path_i, ext_i - path_i); //ファイル名
					path = pathname + filename + ".bin";
					Engine::scene_manager->CreateScene_Default(path, filename);
				}
			}
		}
		if (ImGui::MenuItem(u8"開く"))
		{
			if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
			{
				string path = System_Function::Get_Open_File_Name();
				if (path != "")
				{
					Engine::scene_manager->Last_Save_Path = path;
					Scene_Manager::LoadScene(path);
					Active_Object.reset();
					Active_Object_Old.reset();
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->Last_Save_Path;
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
			if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
			{
				if (Engine::scene_manager->Last_Save_Path != "")
				{
					Engine::scene_manager->SaveScene(Engine::scene_manager->Last_Save_Path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->Last_Save_Path;
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
						Engine::scene_manager->SaveScene(path);
						ofstream oOfstream("Default_Resource\\System\\last_save.bin");
						if (oOfstream.is_open())
						{
							// ファイルへ書き込む
							oOfstream << Engine::scene_manager->Last_Save_Path;
						}
						Debug::Log(u8"シーンの保存に成功");
					}
				}
			}
		}
		if (ImGui::MenuItem(u8"名前をつけて保存"))
		{
			if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
			{
				string path = System_Function::Get_Save_File_Name();
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;//7
					int ext_i = path.find_last_of(".");//10
					string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
					string filename = path.substr(path_i, ext_i - path_i); //ファイル名
					path = pathname + filename + ".bin";
					Engine::scene_manager->SaveScene(path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->Last_Save_Path;
					}
					Debug::Log(u8"シーンの保存に成功");
				}
			}
		}
		ImGui::EndMenu();
	}
}

//ヒエラルキー内のオブジェクトツリー描画
void Debug_UI::GameObject_List_Render(const unique_ptr<Scene>& scene)
{
	static int ID = 0;

	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if (ImGui::TreeNode(scene->name.c_str()))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selecting = -1;
		bool DragMenu_Active = false;

		for (size_t i = 0; i < scene->gameObject_List.size();++i)
		{
			if (scene->gameObject_List[i]->transform->parent.expired())
			{
				GameObject_Tree_Render(ID, scene->gameObject_List[i], selecting, base_flags);
			}
		}

		if (ImGui::BeginPopupContextWindow(u8"ヒエラルキーサブ"))
		{
			if (!Active_Object.expired())
			{
				if (ImGui::Selectable(u8"オブジェクトを削除"))
				{
					GameObject::Destroy(Active_Object.lock());
					Active_Object.reset();
					Active_Object_Old.reset();
					selecting = -1;
				}

				if (!Active_Object.expired())
				{
					if (ImGui::Selectable(u8"オブジェクトを複製"))
					{
						{
							ofstream ss("Default_Resource\\System\\copy.prefab", ios::binary);
							{
								cereal::BinaryOutputArchive o_archive(ss);
								o_archive(Active_Object.lock());
							}
						}
						Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
					}

					if (ImGui::Selectable(u8"オブジェクトをプレハブ化"))
					{
						Resources::Create_Prefab(Active_Object.lock());
					}

					if (!Active_Object.lock()->transform->parent.expired())
					{
						if (ImGui::Selectable(u8"オブジェクトの親子関係を解除"))
						{
							Active_Object.lock()->transform->Set_parent(nullptr);
						}
					}
					ImGui::Separator();
					if (ImGui::Selectable(u8"選択解除"))
					{
						Active_Object.reset();
						Active_Object_Old.reset();
						selecting = -1;
					}
				}
			}
			ImGui::Separator();
			if (ImGui::Selectable(u8"新規オブジェクトを追加"))
			{
				GameObject::Instantiate(u8"GameObject");
			}
			if (ImGui::Selectable(u8"プレハブを読み込む"))
			{
				string path = System_Function::Get_Open_File_Name();
				if (path != "")
				{
					Resources::Load_Prefab(path);
				}
			}
			ImGui::EndPopup();
		}
		ImGui::TreePop();

		if (ImGui::IsWindowFocused() && !Active_Object.expired())
		{
			if (ImGui::IsKeyDown(17) && ImGui::IsKeyPressed(68)) //Ctrl + D
			{
				{
					ofstream ss("Default_Resource\\System\\copy.prefab", ios::binary);
					{
						cereal::BinaryOutputArchive o_archive(ss);
						o_archive(Active_Object.lock());
					}
				}
				Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
			}

			if (ImGui::IsKeyPressed(46)) //Delete
			{
				GameObject::Destroy(Active_Object.lock());
				Active_Object.reset();
				Active_Object_Old.reset();
				selecting = -1;
			}
		}
	}
	ID = 0;
}

//オブジェクトツリー描画
void Debug_UI::GameObject_Tree_Render(int& ID, const shared_ptr<GameObject>& obj, int& selecting, int flag)
{
	ImGui::PushID(ID);
	const ImGuiTreeNodeFlags in_flag = flag;
	if (selecting == ID) flag |= ImGuiTreeNodeFlags_Selected;

	bool active = obj->activeSelf();
	if (active) active = obj->activeInHierarchy();

	if (obj->transform->childCount())
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
		bool open = false;
		if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
		open = ImGui::TreeNodeEx((void*)(intptr_t)ID, flag, obj->name.c_str());
		if (!active) { ImGui::PopStyleColor(); }

		if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
		{
			selecting = ID;
			Active_Object = obj;
		}

		GameObject_DragMenu_Render(obj);

		if (open)
		{
			for (size_t i = 0; i < obj->transform->children.size();++i)
			{
				++ID;
				GameObject_Tree_Render(ID, obj->transform->children[i].lock()->gameObject, selecting, in_flag);
			}
			ImGui::TreePop();
		}
		else
		{
			++ID;
		}
	}
	else
	{
		flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

		if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
		ImGui::TreeNodeEx((void*)(intptr_t)ID, flag, obj->name.c_str());
		if (!active) { ImGui::PopStyleColor(); }

		if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
		{
			selecting = ID;
			Active_Object = obj;
		}

		GameObject_DragMenu_Render(obj);

		++ID;
	}
	ImGui::PopID();
}

//ヒエラルキーでのオブジェクトドラッグ
void Debug_UI::GameObject_DragMenu_Render(const std::shared_ptr<GameObject>& obj)
{
	if (ImGui::BeginDragDropSource())
	{
		Drag_Object = obj;
		int a = 0;
		ImGui::SetDragDropPayload("DragDrop_Object", &a, sizeof(int));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDrop_Object"))
		{
			if (!Drag_Object.expired() && Drag_Object.lock() != obj)
			{
				ImGui::OpenPopup("Drag_Object_Menu");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopup("Drag_Object_Menu"))
	{
		string obj_name = obj->name;

		string s1 = u8"の子に設定";
		string s2 = u8"の上に移動";
		string s3 = u8"の下に移動";

		string s_set = obj_name + s1;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (!Drag_Object.expired())
			{
				Drag_Object.lock()->transform->Set_parent(obj->transform);
				Drag_Object.reset();
			}
		}

		s_set = obj_name + s2;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (!Drag_Object.expired())
			{
				shared_ptr<GameObject> drag_obj = Drag_Object.lock();
				drag_obj->transform->Set_parent(obj->transform->parent.lock());
				drag_obj->transform->SetSiblingIndex(obj->transform->GetSiblingIndex());
				Drag_Object.reset();
			}
		}

		s_set = obj_name + s3;
		if (ImGui::Selectable(s_set.c_str()))
		{
			if (!Drag_Object.expired())
			{
				shared_ptr<GameObject> drag_obj = Drag_Object.lock();
				shared_ptr<Transform> parent = obj->transform->parent.lock();
				drag_obj->transform->Set_parent(parent);
				drag_obj->transform->SetSiblingIndex(obj->transform->GetSiblingIndex() + 1);
				Drag_Object.reset();
			}
		}
		ImGui::EndPopup();
	}
}

//ショートカットキーチェック
void Debug_UI::ShortCut_Check()
{
	if (ImGui::IsKeyDown(17)) //Ctrl
	{
		if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
		{
			if (ImGui::IsKeyPressed(83)) //S
			{
				if (Engine::scene_manager->Last_Save_Path != "")
				{
					Engine::scene_manager->SaveScene(Engine::scene_manager->Last_Save_Path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// ファイルへ書き込む
						oOfstream << Engine::scene_manager->Last_Save_Path;
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
						Engine::scene_manager->SaveScene(path);
						ofstream oOfstream("Default_Resource\\System\\last_save.bin");
						if (oOfstream.is_open())
						{
							// ファイルへ書き込む
							oOfstream << Engine::scene_manager->Last_Save_Path;
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
				if (Engine::scene_manager->Run)
				{
					Engine::scene_manager->Run = false;
					Engine::scene_manager->Pause = true;
					Engine::audio_manager->Suspend();
				}
			}
			else
			{
				if (Engine::scene_manager->Run)
				{
					Active_Object.reset();
					Active_Object_Old.reset();
					Engine::scene_manager->Run = false;
					Engine::scene_manager->Pause = false;
					Engine::scene_manager->End_DebugScene();
				}
				else
				{
					if (!Engine::scene_manager->Pause)
					{
						Active_Object.reset();
						Active_Object_Old.reset();
						Engine::scene_manager->Start_DebugScene();
					}
					else
					{
						Engine::audio_manager->Resume();
					}
					Engine::scene_manager->Run = true;
					Engine::scene_manager->Pause = false;
				}
			}
		}
	}
}

//シーンビューのカメラ操作
void Debug_UI::Debug_Camera_Update()
{
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
				const float dis_x = (mouse_pos.x - mouse_old_pos.x) * 0.1f;
				const float dis_y = (mouse_pos.y - mouse_old_pos.y) * 0.1f;

				Vector3 rot = Debug_Camera_Transform->Get_eulerAngles();
				rot.y += dis_x;
				rot.x += dis_y;
				rot.z = 0;
				Debug_Camera_Transform->Set_eulerAngles(rot);

				mouse_old_pos = mouse_pos;
			}

			Vector3 move = { 0,0,0 };
			const float speed = 50;
			if (ImGui::IsKeyDown(87))
			{
				move += Debug_Camera_Transform->Get_forward() * Time::deltaTime * speed;
			}
			if (ImGui::IsKeyDown(83))
			{
				move -= Debug_Camera_Transform->Get_forward() * Time::deltaTime * speed;
			}
			if (ImGui::IsKeyDown(65))
			{
				move -= Debug_Camera_Transform->Get_right() * Time::deltaTime * speed;
			}
			if (ImGui::IsKeyDown(68))
			{
				move += Debug_Camera_Transform->Get_right() * Time::deltaTime * speed;
			}
			Debug_Camera_Transform->Set_position(Debug_Camera_Transform->Get_position() + move);
		}
		else
		{
			mouse_old_pos = { -1,-1 };
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
			far_z = 100000.0f;

			XMStoreFloat4x4(&Debug_Camera_P, XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z));

			//XMStoreFloat4x4(&Debug_Camera_P, XMMatrixOrthographicLH((float)Engine::view_scene->screen_x, (float)Engine::view_scene->screen_y, 0.1f, 1000.0f));
		}
		// ビュー行列を作成
		// カメラの設定
		{
			Vector3 pos = Debug_Camera_Transform->Get_position();
			Vector4 eye = { pos.x,pos.y,pos.z ,0 };
			XMVECTOR eye_v = XMLoadFloat4(&eye);

			XMVECTOR focus_v = eye_v + XMLoadFloat3(&Debug_Camera_Transform->Get_forward());

			XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
			camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
			camForward = XMVector3Normalize(camForward);
			XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

			XMVECTOR up_v = Debug_Camera_Transform->Get_up();
			XMStoreFloat4x4(&Debug_Camera_V, XMMatrixLookAtLH(eye_v, focus_v, up_v));
		}
	}
}