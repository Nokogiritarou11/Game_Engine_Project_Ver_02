#include "DxSystem.h"
#include "Debug_UI.h"
#include "Debug.h"
#include "Original_Math.h"
#include "Scene_Manager.h"
#include "Engine.h"
#include "All_Component_List.h"
#include "Include_ImGui.h"
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
	io.Fonts->AddFontFromFileTTF("Font/fontawesome-webfont.ttf", size_icon, &config, icon_ranges);
	io.Fonts->Build();
}

Debug_UI::~Debug_UI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Debug_UI::Update(shared_ptr<Scene> scene)
{
	if (Draw_Debug_UI)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//ゲームオブジェクト関連
		{
			//ヒエラルキー
			Hierarchy_Render(scene);
			//インスペクタ
			Inspector_Render();
		}

		//シーン再生
		ScenePlayer_Render();
		//デバッグログ
		Debug_Log_Render();
	}
}

void Debug_UI::Render()
{
	if (Draw_Debug_UI)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
	}
}

string Debug_UI::Get_Open_File_Name()
{
	static OPENFILENAME     ofn;
	static TCHAR            szPath[MAX_PATH];
	static TCHAR            szFile[MAX_PATH];
	static string           str_pass;
	static string           current_pass;
	static size_t           current_pass_size;

	if (szPath[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, szPath);
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szPath, -1, char_pass, MAX_PATH, NULL, NULL);
		current_pass = char_pass;
		current_pass_size = current_pass.size();
	}
	if (ofn.lStructSize == 0)
	{
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = DxSystem::hwnd;
		ofn.lpstrInitialDir = szPath;       // 初期フォルダ位置
		ofn.lpstrFile = szFile;       // 選択ファイル格納
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("すべてのファイル(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("ファイル選択");
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	}
	if (GetOpenFileName(&ofn))
	{
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, char_pass, MAX_PATH, NULL, NULL);
		str_pass = char_pass;
		str_pass = str_pass.substr(current_pass_size + 1, str_pass.size());
	}
	else
	{
		str_pass = "";
	}
	return str_pass;
}

string Debug_UI::Get_Save_File_Name()
{
	static OPENFILENAME     ofn;
	static TCHAR            szPath[MAX_PATH];
	static TCHAR            szFile[MAX_PATH];
	static string           str_pass;
	static size_t           current_pass_size;

	if (szPath[0] == TEXT('\0'))
	{
		GetCurrentDirectory(MAX_PATH, szPath);
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szPath, -1, char_pass, MAX_PATH, NULL, NULL);
		string current_pass = char_pass;
		current_pass_size = current_pass.size();
	}
	if (ofn.lStructSize == 0)
	{
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = DxSystem::hwnd;
		ofn.lpstrInitialDir = szPath;       // 初期フォルダ位置
		ofn.lpstrFile = szFile;       // 選択ファイル格納
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("すべてのファイル(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("名前をつけて保存");
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	}
	if (GetSaveFileName(&ofn))
	{
		char char_pass[MAX_PATH];
		WideCharToMultiByte(CP_ACP, 0, szFile, -1, char_pass, MAX_PATH, NULL, NULL);
		str_pass = char_pass;
		str_pass = str_pass.substr(current_pass_size + 1, str_pass.size());
	}
	else
	{
		str_pass = "";
	}
	return str_pass;
}

void Debug_UI::Print_Log(string log)
{
	Debug_Log.push_back(log + u8"\n");
	Debug_Log_Changed = true;
}

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

void Debug_UI::Debug_Log_Render()
{
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
	static bool Open_Log = true;
	ImGui::SetNextWindowPos(ImVec2(0, 900), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_Once);
	static Debug_Logger logger;
	if (Debug_Log_Changed)
	{
		for (int i = 0; i < Debug_Log.size(); i++)
		{
			logger.AddLog(Debug_Log[i].c_str());
		}
		Debug_Log.clear();
	}
	logger.Draw((u8"デバッグログ"), &Open_Log);
}

void Debug_UI::Hierarchy_Render(shared_ptr<Scene> scene)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui::Begin(u8"ヒエラルキー", NULL, window_flags);

	Scene_File_Menu_Render();
	GameObject_List_Render(scene);
	
	ImGui::End();
}

void Debug_UI::Inspector_Render()
{
	ImGui::SetNextWindowPos(ImVec2(1500, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
	shared_ptr<GameObject> obj = Active_Object.lock();
	ImGui::Begin(u8"インスペクタ");
	if (obj)
	{
		//選択時
		ImGui::Checkbox("", &obj->Active);
		ImGui::SameLine();
		ImGui::InputText(u8"名前", &obj->name);
		list<shared_ptr<Component>>::iterator itr_end = obj->Component_List.end();
		for (list<shared_ptr<Component>>::iterator itr = obj->Component_List.begin(); itr != itr_end; itr++)
		{
			(*itr)->Draw_ImGui();
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

void Debug_UI::ScenePlayer_Render()
{
	ImGui::SetNextWindowPos(ImVec2(935, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(45, 30), ImGuiCond_Once);
	ImGuiWindowFlags window_flags = 0;
	window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	ImGui::Begin(u8"シーン再生",NULL,window_flags);

	if (Engine::scene_manager->Run)
	{
		if (ImGui::Button(ICON_FA_STOP))
		{
			Engine::scene_manager->Run = false;
		}
	}
	else
	{
		if (ImGui::Button(ICON_FA_PLAY))
		{
			Engine::scene_manager->Run = true;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PAUSE))
	{
	}

	ImGui::End();
}

void Debug_UI::Scene_File_Menu_Render()
{
	static bool open_new_scene_menu = false;
	open_new_scene_menu = false;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"ファイル"))
		{
			if (ImGui::MenuItem(u8"新規シーン"))
			{
				open_new_scene_menu = true;
			}
			if (ImGui::MenuItem(u8"開く"))
			{
				if (Engine::scene_manager->CreateScene_FromFile())
				{
				}
				else
				{
					Debug::Log(u8"ファイルが開けませんでした");
				}
			}
			if (ImGui::MenuItem(u8"上書き保存"))
			{
				if (Engine::scene_manager->Last_Save_Path != "")
				{
					Engine::scene_manager->SaveScene(Engine::scene_manager->Last_Save_Path);
				}
				else
				{
					string path = Debug_UI::Get_Save_File_Name();
					if (path != "")
					{
						int path_i = path.find_last_of("\\") + 1;//7
						int ext_i = path.find_last_of(".");//10
						string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
						string filename = path.substr(path_i, ext_i - path_i); //ファイル名
						path = pathname + filename + ".bin";
						Engine::scene_manager->SaveScene(path);
					}
				}
			}
			if (ImGui::MenuItem(u8"名前をつけて保存"))
			{
				string path = Debug_UI::Get_Save_File_Name();
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;//7
					int ext_i = path.find_last_of(".");//10
					string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
					string filename = path.substr(path_i, ext_i - path_i); //ファイル名
					path = pathname + filename + ".bin";
					Engine::scene_manager->SaveScene(path);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (open_new_scene_menu)
	{
		ImGui::OpenPopup(u8"新規シーン");
	}
	if (ImGui::BeginPopupModal(u8"新規シーン", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char input[MAX_PATH] = "";
		ImGui::InputText(u8"新規シーン名", input, MAX_PATH);
		if (ImGui::Button(u8"作成", ImVec2(120, 0)))
		{
			string s = input;
			Engine::scene_manager->CreateScene_Default(s);
			strcpy_s(input, "");
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button(u8"キャンセル", ImVec2(120, 0)))
		{
			strcpy_s(input, "");
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

}

void Debug_UI::GameObject_List_Render(std::shared_ptr<Scene> scene)
{
	int node_clicked = -1;
	static int ID = 0;

	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if (ImGui::TreeNode(scene->name.c_str()))
	{
		ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selection_mask = (0);
		list<shared_ptr<GameObject>>::iterator itr_end = scene->gameObject_List.end();
		for (list<shared_ptr<GameObject>>::iterator itr = scene->gameObject_List.begin(); itr != itr_end; itr++)
		{
			ImGui::PushID(ID);
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (selection_mask & (1 << ID)) != 0;
			if (is_selected) node_flags |= ImGuiTreeNodeFlags_Selected;

			const bool active = (*itr)->activeSelf();
			if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)ID, node_flags, (*itr)->name.c_str());
			if (!active) { ImGui::PopStyleColor(); }

			if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
			{
				node_clicked = ID;
				Active_Object = *itr;
			}
			if (node_open)
			{
				ImGui::TreePop();
			}
			ID++;
			ImGui::PopID();
		}

		if (ImGui::BeginPopupContextWindow(u8"ヒエラルキーサブ"))
		{
			if (ImGui::Selectable(u8"新規オブジェクトを追加"))
			{
				GameObject::Instantiate(u8"GameObject");
			}
			if (ImGui::Selectable(u8"オブジェクトを削除"))
			{
				GameObject::Destroy(Active_Object.lock());
				Active_Object.reset();
			}
			ImGui::EndPopup();
		}

		if (node_clicked != -1)
		{
			// Update selection state
			// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
				selection_mask = (1 << node_clicked);           // Click to single-select
		}
		ImGui::TreePop();
	}
	ID = 0;
}