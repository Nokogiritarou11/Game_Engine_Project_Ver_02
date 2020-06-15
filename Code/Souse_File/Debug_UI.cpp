#include "DxSystem.h"
#include "Debug_UI.h"
#include "Original_Math.h"
using namespace std;
using namespace DirectX;

bool Debug_UI::Draw_Debug_UI = true;
char* Debug_UI::Font_Name = "Font/mplus-1p-medium.ttf";
float Debug_UI::Font_Size_Pixels = 30.0f;
float Debug_UI::Font_Size = 0.6f;
float Debug_UI::UI_Size = 0.6f;
vector<string> Debug_UI::Debug_Log = {};
bool Debug_UI::Debug_Log_Changed = false;
weak_ptr<GameObject> Debug_UI::Active_Object;

Debug_UI::~Debug_UI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Debug_UI::Initialize(HWND hWnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//テーマの変更
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();

	if (!ImGui_ImplWin32_Init(hWnd))
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

void Debug_UI::Update(shared_ptr<Scene> scene, std::string name)
{
	if (Draw_Debug_UI)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//ゲームオブジェクト関連
		{
			//ヒエラルキー
			Hierarchy_Render(scene, name);
			//インスペクタ
			Inspector_Render();
		}

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
	}
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

void Debug_UI::Hierarchy_Render(shared_ptr<Scene> scene, std::string name)
{
	int node_clicked = -1;
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_Once);
	ImGui::Begin(u8"ヒエラルキー");

	static int ID = 0;
	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if (ImGui::TreeNode(name.c_str()))
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

			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)ID, node_flags, (*itr)->name.c_str());
			if (ImGui::IsItemClicked())
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
		ImGui::SetNextItemOpen(true, ImGuiCond_Always);
		if (ImGui::TreeNode(obj->name.c_str()))
		{
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::TreeNode("Transform"))
			{
				static float pos[3] = { 0,0,0 };
				Vector3 trans_pos = obj->transform->Get_position();
				pos[0] = trans_pos.x;
				pos[1] = trans_pos.y;
				pos[2] = trans_pos.z;
				ImGui::DragFloat3(u8"Position", pos, 0.05f, -FLT_MAX, FLT_MAX);
				obj->transform->Set_position(pos[0], pos[1], pos[2]);

				static float rot[3] = { 0,0,0 };
				Vector3 trans_rot = obj->transform->Get_eulerAngles();
				rot[0] = trans_rot.x;
				rot[1] = trans_rot.y;
				rot[2] = trans_rot.z;
				ImGui::DragFloat3(u8"Rotation", rot, 0.05f, -FLT_MAX, FLT_MAX);
				obj->transform->Set_eulerAngles(rot[0], rot[1], rot[2]);

				static float scl[3] = { 1,1,1 };
				Vector3 trans_scl = obj->transform->Get_scale();
				scl[0] = trans_scl.x;
				scl[1] = trans_scl.y;
				scl[2] = trans_scl.z;
				ImGui::DragFloat3(u8"Scale", scl, 0.01f, -FLT_MAX, FLT_MAX);
				obj->transform->Set_scale(scl[0], scl[1], scl[2]);

				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::Text(u8"オブジェクトが選択されていません");
	}

	ImGui::End();
}