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

	//�e�[�}�̕ύX
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

	//���{��t�H���g�ɑΉ�
	ImFont* font = io.Fonts->AddFontFromFileTTF(Font_Name, Font_Size_Pixels, nullptr, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);

	io.FontGlobalScale = Font_Size; // �t�H���g�̑傫�����ꊇ�ŕύX�ł��܂��B
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(UI_Size); // UI�̑傫�����ꊇ�ŕύX�ł��܂��B

	float size_icon = 20.0f; // 30.0f�ɂ���Ƒ傫�������̂�20.0f�ɂ��Ă��܂��B
	ImFontConfig config;
	config.MergeMode = true; // �t�H���g�e�N�X�`�������̂����܂��B
	config.PixelSnapH = true;
	config.GlyphMinAdvanceX = size_icon; // �A�C�R���𓙕��ɂ��܂��B
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	io.Fonts->AddFontFromFileTTF("Font/fontawesome-webfont.ttf", size_icon, &config, icon_ranges);
	io.Fonts->Build();

	Vector3 p = { -100, 80, -100 };
	Vector3 e = { 30, 45, 0 };
	Debug_Camera_Transform = make_unique<Transform>(p, e);
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

		Main_Window_Render();
		//�Q�[���I�u�W�F�N�g�֘A
		{
			//�q�G�����L�[
			Hierarchy_Render(scene);
			//�C���X�y�N�^
			Inspector_Render();
		}

		//�V�[���Đ�UI
		ScenePlayer_Render();

		//�`��
		{
			GameView_Render();
			SceneView_Render();
		}
		//�f�o�b�O���O
		Debug_Log_Render();
	}
}

void Debug_UI::Render()
{
	if (Draw_Debug_UI)
	{
		Engine::view_scene->Render(Debug_Camera_V, Debug_Camera_P, Debug_Camera_Transform);

		// �����_�[�^�[�Q�b�g�r���[�ݒ�
		DxSystem::SetDefaultView();
		DxSystem::SetViewPort(DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight());
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
	}
}

//�J���t�@�C���̃p�X�擾
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
		ofn.lpstrInitialDir = szPath;       // �����t�H���_�ʒu
		ofn.lpstrFile = szFile;       // �I���t�@�C���i�[
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("���ׂẴt�@�C��(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("�t�@�C���I��");
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

//�ۑ���p�X�擾
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
		ofn.lpstrInitialDir = szPath;       // �����t�H���_�ʒu
		ofn.lpstrFile = szFile;       // �I���t�@�C���i�[
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("���ׂẴt�@�C��(*.*)\0*.*\0");
		ofn.lpstrTitle = TEXT("���O�����ĕۑ�");
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

//�h�b�L���O�p�e�E�B���h�E�`��
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
	ImGui::Begin(u8"�Q�[���G���W��", NULL, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	Scene_File_Menu_Render();
	ImGui::End();
}

//���O�̒ǉ�
void Debug_UI::Print_Log(string log)
{
	Debug_Log.push_back(log + u8"\n");
	Debug_Log_Changed = true;
}

//���O�Ǘ��p�\����
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

//�f�o�b�O���O�`��
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
	logger.Draw((u8"�f�o�b�O���O"), &Open_Log);
}

//�q�G�����L�[�`��
void Debug_UI::Hierarchy_Render(shared_ptr<Scene> scene)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"�q�G�����L�[", NULL, window_flags);

	GameObject_List_Render(scene);

	ImGui::End();
}

//�C���X�y�N�^�`��
void Debug_UI::Inspector_Render()
{
	ImGui::SetNextWindowPos(ImVec2(1500, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	shared_ptr<GameObject> obj = Active_Object.lock();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"�C���X�y�N�^", NULL, window_flags);
	if (obj)
	{
		//�I����
		ImGui::Checkbox("", &obj->Active);
		ImGui::SameLine();
		ImGui::InputText(u8"���O", &obj->name);

		bool removed = true;
		while (removed)
		{
			bool removed_comp = false;
			list<shared_ptr<Component>>::iterator itr_end = obj->Component_List.end();
			for (list<shared_ptr<Component>>::iterator itr = obj->Component_List.begin(); itr != itr_end; itr++)
			{
				if (!(*itr)->Draw_ImGui())
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
		ImGui::Text(u8"�I�u�W�F�N�g���I������Ă��܂���");
	}

	ImGui::End();
}

//�V�[���Đ�UI�`��
void Debug_UI::ScenePlayer_Render()
{
	ImGui::SetNextWindowPos(ImVec2(935, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(45, 30), ImGuiCond_FirstUseEver);
	ImGuiWindowFlags window_flags = 0;
	window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	ImGui::Begin(u8"�V�[���Đ�", NULL, window_flags);

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

//�V�[���r���[�`��
void Debug_UI::SceneView_Render()
{
	ImGui::SetNextWindowPos(ImVec2(200, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"�V�[��", NULL, window_flags);
	const float titleBarHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f + 10;
	Engine::view_scene->Set_Screen_Size((int)ImGui::GetWindowWidth(), (int)ImGui::GetWindowHeight() - (int)titleBarHeight);
	ImGui::Image((void*)Engine::view_scene->ShaderResourceView_Render.Get(), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - titleBarHeight));

	if (ImGui::IsWindowFocused())
	{
		Debug_Camera_Update();
	}

	ImGui::End();
}

//�Q�[���r���[�`��
void Debug_UI::GameView_Render()
{
	ImGui::SetNextWindowPos(ImVec2(200, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"�Q�[��", NULL, window_flags);
	const float titleBarHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f + 10;
	Engine::view_game->Set_Screen_Size((int)ImGui::GetWindowWidth(), (int)ImGui::GetWindowHeight() - (int)titleBarHeight);
	ImGui::Image((void*)Engine::view_game->ShaderResourceView_Render.Get(), ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - titleBarHeight));

	ImGui::End();
}

//�V�[���ۑ��A�W�JUI�`��
void Debug_UI::Scene_File_Menu_Render()
{
	static bool open_new_scene_menu = false;
	open_new_scene_menu = false;
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"�t�@�C��"))
		{
			if (ImGui::MenuItem(u8"�V�K�V�[��"))
			{
				open_new_scene_menu = true;
			}
			if (ImGui::MenuItem(u8"�J��"))
			{
				if (Engine::scene_manager->CreateScene_FromFile())
				{
				}
				else
				{
					Debug::Log(u8"�t�@�C�����J���܂���ł���");
				}
			}
			if (ImGui::MenuItem(u8"�㏑���ۑ�"))
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
						string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
						string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
						path = pathname + filename + ".bin";
						Engine::scene_manager->SaveScene(path);
					}
				}
			}
			if (ImGui::MenuItem(u8"���O�����ĕۑ�"))
			{
				string path = Debug_UI::Get_Save_File_Name();
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;//7
					int ext_i = path.find_last_of(".");//10
					string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
					string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
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
		ImGui::OpenPopup(u8"�V�K�V�[��");
	}
	if (ImGui::BeginPopupModal(u8"�V�K�V�[��", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char input[MAX_PATH] = "";
		ImGui::InputText(u8"�V�K�V�[����", input, MAX_PATH);
		if (ImGui::Button(u8"�쐬", ImVec2(120, 0)))
		{
			string s = input;
			Engine::scene_manager->CreateScene_Default(s);
			strcpy_s(input, "");
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button(u8"�L�����Z��", ImVec2(120, 0)))
		{
			strcpy_s(input, "");
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

}

//�C���X�y�N�^���̃I�u�W�F�N�g�c���[�`��
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

		if (ImGui::BeginPopupContextWindow(u8"�q�G�����L�[�T�u"))
		{
			if (ImGui::Selectable(u8"�V�K�I�u�W�F�N�g��ǉ�"))
			{
				GameObject::Instantiate(u8"GameObject");
			}
			if (!Active_Object.expired())
			{
				if (ImGui::Selectable(u8"�I�u�W�F�N�g���폜"))
				{
					GameObject::Destroy(Active_Object.lock());
					Active_Object.reset();
				}
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

//�V�[���r���[�̃J��������
void Debug_UI::Debug_Camera_Update()
{
	//����
	{
		static ImVec2 mouse_old_pos = { 0,0 };
		static ImVec2 mouse_pos = { 0,0 };
		if (Input_Manager::ms.rightButton)
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
		}
		else
		{
			mouse_old_pos = { -1,-1 };
		}
		Vector3 a = Debug_Camera_Transform->Get_eulerAngles();

		Vector3 move = { 0,0,0 };
		const float speed = 50;
		if (Input_Manager::kb.W)
		{
			move += Debug_Camera_Transform->Get_forward() * Time::deltaTime * speed;
		}
		if (Input_Manager::kb.S)
		{
			move -= Debug_Camera_Transform->Get_forward() * Time::deltaTime * speed;
		}
		if (Input_Manager::kb.A)
		{
			move -= Debug_Camera_Transform->Get_right() * Time::deltaTime * speed;
		}
		if (Input_Manager::kb.D)
		{
			move += Debug_Camera_Transform->Get_right() * Time::deltaTime * speed;
		}
		Debug_Camera_Transform->Set_position(Debug_Camera_Transform->Get_position() + move);
	}
	//�J�����s��v�Z
	{
		// �v���W�F�N�V�����s����쐬
		// ��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
		{
			// �p�x�����W�A��(��)�ɕϊ�
			float fov_y = XMConvertToRadians(30);	// ��p
			float aspect = (float)Engine::view_scene->screen_x / (float)Engine::view_scene->screen_y;	// ��ʔ䗦

			XMStoreFloat4x4(&Debug_Camera_P, XMMatrixPerspectiveFovLH(fov_y, aspect, 0.1f, 100000.0f));
		}
		// �r���[�s����쐬
		// �J�����̐ݒ�
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