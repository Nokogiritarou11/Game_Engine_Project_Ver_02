#include "DxSystem.h"
#include "Editor.h"
#include "Debug.h"
#include "Original_Math.h"
#include "Scene_Manager.h"
#include "Engine.h"
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
	io.Fonts->AddFontFromFileTTF("Default_Resource/Font/fontawesome-webfont.ttf", size_icon, &config, icon_ranges);
	io.Fonts->Build();

	Vector3 p = { -50, 40, -50 };
	Vector3 e = { 15, 45, 0 };
	Debug_Camera_Transform = make_unique<Transform>(p, e);

	//�J�����s�񏉉�v�Z
	{
		// �v���W�F�N�V�����s����쐬
		// ��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
		{
			// �p�x�����W�A��(��)�ɕϊ�
			fov_y = XMConvertToRadians(30);	// ��p
			aspect = (float)Engine::view_scene->screen_x / (float)Engine::view_scene->screen_y;	// ��ʔ䗦
			near_z = 0.1f;
			far_z = 1000.0f;

			//Debug_Camera_P = XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z);
			Debug_Camera_P = XMMatrixPerspectiveFovRH(fov_y, aspect, near_z, far_z);

			//XMStoreFloat4x4(&Debug_Camera_P, XMMatrixOrthographicLH((float)Engine::view_scene->screen_x, (float)Engine::view_scene->screen_y, 0.1f, 1000.0f));
		}
		// �r���[�s����쐬
		// �J�����̐ݒ�
		{
			Vector3 eye_v = Debug_Camera_Transform->Get_position();
			Vector3 focus_v = eye_v + Debug_Camera_Transform->Get_forward();

			XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
			camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
			camForward = XMVector3Normalize(camForward);
			XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

			XMVECTOR up_v = Debug_Camera_Transform->Get_up();
			Debug_Camera_V = XMMatrixLookAtRH(eye_v, focus_v, up_v);
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
	//�Q�[���I�u�W�F�N�g�֘A
	{
		//�q�G�����L�[
		Hierarchy_Render(scene);
		//�C���X�y�N�^
		Inspector_Render();
	}

	//�V�[���Đ�UI
	ScenePlayer_Render();

	//�A�Z�b�g�}�l�[�W���[
	FileResource_Render();

	//�`��
	{
		GameView_Render();
		SceneView_Render();
	}
	//�f�o�b�O���O
	Debug_Log_Render();

	//�V���[�g�J�b�g�L�[�`�F�b�N
	ShortCut_Check();
}

void Editor::Render()
{
	Engine::particle_manager->Camera_Update(Debug_Camera_Transform, fov_y, near_z, far_z, aspect);
	Engine::view_scene->Render(Debug_Camera_V, Debug_Camera_P, Debug_Camera_Transform);

	// �����_�[�^�[�Q�b�g�r���[�ݒ�
	DxSystem::SetDefaultView();
	DxSystem::SetViewPort(DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight());
	ImGui::Render();
	DxSystem::Clear();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

//�h�b�L���O�p�e�E�B���h�E�`��
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
	ImGui::Begin(u8"�Q�[���G���W��", NULL, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	MenuBar_Render();
	ImGui::End();
}

//���O�̒ǉ�
void Editor::Print_Log(string log)
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
void Editor::Debug_Log_Render()
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
	logger.Draw((u8"�R���\�[��"), &Open_Log);
}

//�q�G�����L�[�`��
void Editor::Hierarchy_Render(const unique_ptr<Scene>& scene)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"�q�G�����L�[", NULL, window_flags);

	static int ID = 0;

	ImGui::SetNextItemOpen(true, ImGuiCond_Always);
	if (ImGui::TreeNode(scene->name.c_str()))
	{
		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		static int selecting = -1;
		bool Item_Clicked = false;

		for (size_t i = 0; i < scene->gameObject_List.size();++i)
		{
			if (scene->gameObject_List[i]->transform->parent.expired())
			{
				GameObject_Tree_Render(ID, scene->gameObject_List[i], selecting, base_flags, Item_Clicked);
			}
		}

		if (!Item_Clicked && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
		{
			Active_Object.reset();
			Active_Object_Old.reset();
			selecting = -1;
		}

		if (ImGui::BeginPopupContextWindow(u8"�q�G�����L�[�T�u"))
		{
			if (ImGui::Selectable(u8"�V�K�I�u�W�F�N�g��ǉ�"))
			{
				GameObject::Instantiate(u8"GameObject");
			}
			if (ImGui::Selectable(u8"�v���n�u��ǂݍ���"))
			{
				string path = System_Function::Get_Open_File_Name();
				if (path != "")
				{
					Resources::Load_Prefab(path);
				}
			}
			ImGui::Separator();
			if (!Active_Object.expired())
			{
				if (ImGui::Selectable(u8"�I�u�W�F�N�g���폜"))
				{
					GameObject::Destroy(Active_Object.lock());
					Active_Object.reset();
					Active_Object_Old.reset();
					selecting = -1;
				}

				if (!Active_Object.expired())
				{
					if (ImGui::Selectable(u8"�I�u�W�F�N�g�𕡐�"))
					{
						shared_ptr<GameObject> obj = Active_Object.lock();
						weak_ptr<Transform> parent = obj->transform->Get_parent();
						if (!parent.expired())
						{
							obj->transform->Set_parent(nullptr);
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
							obj->transform->Set_parent(parent.lock());
							Resources::Load_Prefab("Default_Resource\\System\\copy.prefab")->transform->Set_parent(parent.lock());
						}
						else
						{
							Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
						}
					}

					if (ImGui::Selectable(u8"�I�u�W�F�N�g���v���n�u��"))
					{
						Resources::Create_Prefab(Active_Object.lock());
					}

					if (!Active_Object.lock()->transform->parent.expired())
					{
						if (ImGui::Selectable(u8"�I�u�W�F�N�g�̐e�q�֌W������"))
						{
							Active_Object.lock()->transform->Set_parent(nullptr);
						}
					}
				}
			}
			ImGui::EndPopup();
		}
		ImGui::TreePop();

		if (ImGui::IsWindowFocused() && !Active_Object.expired())
		{
			if (ImGui::IsKeyDown(17) && ImGui::IsKeyPressed(68)) //Ctrl + D
			{
				shared_ptr<GameObject> obj = Active_Object.lock();
				weak_ptr<Transform> parent = obj->transform->Get_parent();
				if (!parent.expired())
				{
					obj->transform->Set_parent(nullptr);
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
					obj->transform->Set_parent(parent.lock());
					Resources::Load_Prefab("Default_Resource\\System\\copy.prefab")->transform->Set_parent(parent.lock());
				}
				else
				{
					Resources::Load_Prefab("Default_Resource\\System\\copy.prefab");
				}
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

	ImGui::End();
}

//�C���X�y�N�^�`��
void Editor::Inspector_Render()
{
	ImGui::SetNextWindowPos(ImVec2(1500, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	shared_ptr<GameObject> obj = Active_Object.lock();
	shared_ptr<GameObject> obj_old = Active_Object_Old.lock();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"�C���X�y�N�^", NULL, window_flags);
	if (obj)
	{
		static bool active = false;
		if (obj_old != obj)
		{
			active = obj->activeSelf();
			Active_Object_Old = Active_Object;
		}
		//�I����
		if (ImGui::Checkbox("##active", &active))
		{
			obj->SetActive(active);
		}
		ImGui::SameLine();
		ImGui::InputText(u8"���O", &obj->name);

		static int ID = 0;
		bool removed = true;
		while (removed)
		{
			bool removed_comp = false;
			for (shared_ptr<Component> c : obj->Component_List)
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
		ImGui::Text(u8"�I�u�W�F�N�g���I������Ă��܂���");
	}

	ImGui::End();
}

//�V�[���Đ�UI�`��
void Editor::ScenePlayer_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"�V�[���Đ�", NULL, window_flags);

	bool Running = Engine::scene_manager->Run;

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
			if (!Engine::scene_manager->Pause)
			{
				Cursor::lockState = CursorLockMode::None;
				Cursor::visible = true;
				ImGui::SetWindowFocus(u8"�Q�[��");
				Active_Object.reset();
				Active_Object_Old.reset();
				Engine::scene_manager->Start_DebugScene();
			}
			else
			{
				Engine::audio_manager->Resume();
			}
			Render_Cursor = false;
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
			Render_Cursor = true;
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
			ImGui::SetWindowFocus(u8"�V�[��");
			Cursor::lockState = CursorLockMode::None;
			Cursor::visible = true;
			Active_Object.reset();
			Active_Object_Old.reset();
			Render_Cursor = true;
			Engine::scene_manager->Run = false;
			Engine::scene_manager->Pause = false;
			Engine::scene_manager->End_DebugScene();
		}
	}

	ImGui::End();
}

//�V�[���r���[�`��
void Editor::SceneView_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

	ImGui::Begin(u8"�V�[��", NULL, window_flags);
	//const float titleBarHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f + 10;
	const float window_width = ImGui::GetCurrentWindow()->InnerRect.GetWidth() - 8;
	const float window_height = ImGui::GetCurrentWindow()->InnerRect.GetHeight() - 8;
	Engine::view_scene->Set_Screen_Size((int)window_width, (int)window_height);
	ImGui::Image((void*)Engine::view_scene->ShaderResourceView_Render.Get(), ImVec2(window_width, window_height));

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
				Vector3 move = Debug_Camera_Transform->Get_forward() * mouse_wheel * 30;
				Debug_Camera_Transform->Set_position(Debug_Camera_Transform->Get_position() + move);
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

	//�J�����s��v�Z
	{
		// �v���W�F�N�V�����s����쐬
		// ��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
		{
			// �p�x�����W�A��(��)�ɕϊ�
			fov_y = XMConvertToRadians(30);	// ��p
			aspect = (float)Engine::view_scene->screen_x / (float)Engine::view_scene->screen_y;	// ��ʔ䗦
			near_z = 0.1f;
			far_z = 1000.0f;

			//Debug_Camera_P = XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z);
			Debug_Camera_P = XMMatrixPerspectiveFovRH(fov_y, aspect, near_z, far_z);

			//XMStoreFloat4x4(&Debug_Camera_P, XMMatrixOrthographicLH((float)Engine::view_scene->screen_x, (float)Engine::view_scene->screen_y, 0.1f, 1000.0f));
		}
		// �r���[�s����쐬
		// �J�����̐ݒ�
		{
			Vector3 eye_v = Debug_Camera_Transform->Get_position();
			Vector3 focus_v = eye_v + Debug_Camera_Transform->Get_forward();

			XMVECTOR camForward = XMVector3Normalize(focus_v - eye_v);    // Get forward vector based on target
			camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
			camForward = XMVector3Normalize(camForward);
			XMVECTOR camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

			XMVECTOR up_v = Debug_Camera_Transform->Get_up();
			Debug_Camera_V = XMMatrixLookAtRH(eye_v, focus_v, up_v);
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

		// �I���m�[�h�̍s��𑀍삷��
		ImGuizmo::Manipulate(
			&Debug_Camera_V._11, &Debug_Camera_P._11,	// �r���[���v���W�F�N�V�����s��
			mCurrentGizmoOperation,		// �ړ�����
			mCurrentGizmoMode,			// ���[�J����Ԃł̑���
			&matrix._11,	// ���삷�郏�[���h�s��
			nullptr);

		if (ImGuizmo::IsUsing())
		{
			trans->Set_world_matrix(matrix);
		}
	}
	ImGui::End();
}

//�Q�[���r���[�`��
void Editor::GameView_Render()
{
	ImGui::SetNextWindowPos(ImVec2(200, 0), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::Begin(u8"�Q�[��", NULL, window_flags);


	ImGuiWindow* p_win = ImGui::GetCurrentWindow();
	Game_View_Size = { p_win->InnerRect.GetWidth() - 8, p_win->InnerRect.GetHeight() - 8 };

	const ImVec2 pos = ImGui::GetCursorScreenPos();
	Game_View_Pos = { pos.x,pos.y + Game_View_Size.y };

	Engine::view_game->Set_Screen_Size(static_cast<int>(Game_View_Size.x), static_cast<int>(Game_View_Size.y));
	ImGui::Image((void*)Engine::view_game->ShaderResourceView_Render.Get(), ImVec2(Game_View_Size.x, Game_View_Size.y));

	Game_View_CenterPos = { pos.x + Game_View_Size.x / 2 ,pos.y + Game_View_Size.y / 2 };

	if (ImGui::IsWindowFocused())
	{
		if (ImGui::IsKeyPressed(27) && !Render_Cursor) //ESC
		{
			Render_Cursor = true;
		}
	}
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
	{
		Render_Cursor = false;
	}

	ImGui::End();
}

void Editor::FileResource_Render()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"�A�Z�b�g(��)", NULL, window_flags);

	if (ImGui::Button(u8"FBX�ǂݍ���"))
	{
		string path = System_Function::Get_Open_File_Name();
		if (path != "")
		{
			int path_i = path.find_last_of("\\") + 1;//7
			int ext_i = path.find_last_of(".");//10
			string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
			string extname = path.substr(ext_i, path.size() - ext_i); //�g���q
			string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
			if (extname == ".fbx" || extname == ".mesh")
			{
				FBX_Converter::Convert_From_FBX(pathname.c_str(), filename.c_str());
			}
			else
			{
				Debug::Log("�t�@�C���`�����Ή����Ă��܂���");
			}
		}
	}

	ImGui::End();
}

//���j���[�o�[�`��
void Editor::MenuBar_Render()
{
	if (ImGui::BeginMenuBar())
	{
		Scene_File_Menu_Render();
		ImGui::EndMenuBar();
	}
}

//�V�[���ۑ��A�W�J���j���[�`��
void Editor::Scene_File_Menu_Render()
{
	if (ImGui::BeginMenu(u8"�t�@�C��"))
	{
		if (ImGui::MenuItem(u8"�V�K�V�[��"))
		{
			if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
			{
				string path = System_Function::Get_Save_File_Name();
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;//7
					int ext_i = path.find_last_of(".");//10
					string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
					string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
					path = pathname + filename + ".bin";
					Engine::scene_manager->CreateScene_Default(path, filename);
				}
			}
		}
		if (ImGui::MenuItem(u8"�J��"))
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
						// �t�@�C���֏�������
						oOfstream << Engine::scene_manager->Last_Save_Path;
					}
				}
				else
				{
					Debug::Log(u8"�t�@�C�����J���܂���ł���");
				}
			}
		}
		if (ImGui::MenuItem(u8"�㏑���ۑ�"))
		{
			if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
			{
				if (Engine::scene_manager->Last_Save_Path != "")
				{
					Engine::scene_manager->SaveScene(Engine::scene_manager->Last_Save_Path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// �t�@�C���֏�������
						oOfstream << Engine::scene_manager->Last_Save_Path;
					}
					Debug::Log(u8"�V�[���̕ۑ��ɐ���");
				}
				else
				{
					string path = System_Function::Get_Save_File_Name();
					if (path != "")
					{
						int path_i = path.find_last_of("\\") + 1;//7
						int ext_i = path.find_last_of(".");//10
						string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
						string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
						path = pathname + filename + ".bin";
						Engine::scene_manager->SaveScene(path);
						ofstream oOfstream("Default_Resource\\System\\last_save.bin");
						if (oOfstream.is_open())
						{
							// �t�@�C���֏�������
							oOfstream << Engine::scene_manager->Last_Save_Path;
						}
						Debug::Log(u8"�V�[���̕ۑ��ɐ���");
					}
				}
			}
		}
		if (ImGui::MenuItem(u8"���O�����ĕۑ�"))
		{
			if (!Engine::scene_manager->Run && !Engine::scene_manager->Pause)
			{
				string path = System_Function::Get_Save_File_Name();
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;//7
					int ext_i = path.find_last_of(".");//10
					string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
					string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
					path = pathname + filename + ".bin";
					Engine::scene_manager->SaveScene(path);
					ofstream oOfstream("Default_Resource\\System\\last_save.bin");
					if (oOfstream.is_open())
					{
						// �t�@�C���֏�������
						oOfstream << Engine::scene_manager->Last_Save_Path;
					}
					Debug::Log(u8"�V�[���̕ۑ��ɐ���");
				}
			}
		}
		ImGui::EndMenu();
	}
}

//�I�u�W�F�N�g�c���[�`��
void Editor::GameObject_Tree_Render(int& ID, const shared_ptr<GameObject>& obj, int& selecting, int flag, bool& Item_Clicked)
{
	++ID;
	const ImGuiTreeNodeFlags in_flag = flag;
	if (selecting == ID) flag |= ImGuiTreeNodeFlags_Selected;

	bool active = obj->activeSelf();
	if (active) active = obj->activeInHierarchy();

	if (obj->transform->childCount())
	{
		bool open = false;
		if (!active) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(0.f, 0.f, 0.4f)); }
		open = ImGui::TreeNodeEx((void*)(intptr_t)obj.get(), flag, obj->name.c_str());
		if (!active) { ImGui::PopStyleColor(); }

		if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1))
		{
			selecting = ID;
			Active_Object = obj;
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
				GameObject_Tree_Render(ID, obj->transform->children[i].lock()->gameObject, selecting, in_flag, Item_Clicked);
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
			Active_Object = obj;
		}

		GameObject_DragMenu_Render(obj);
	}
}

//�q�G�����L�[�ł̃I�u�W�F�N�g�h���b�O
void Editor::GameObject_DragMenu_Render(const std::shared_ptr<GameObject>& obj)
{
	if (ImGui::BeginDragDropSource())
	{
		Drag_Object = obj;
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
			if (!Drag_Object.expired() && Drag_Object.lock() != obj)
			{
				ImGui::OpenPopup(Menu_Label.c_str());
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginPopup(Menu_Label.c_str()))
	{
		string obj_name = obj->name;

		string s1 = u8"�̎q�ɐݒ�";
		string s2 = u8"�̏�Ɉړ�";
		string s3 = u8"�̉��Ɉړ�";

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

//�V���[�g�J�b�g�L�[�`�F�b�N
void Editor::ShortCut_Check()
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
						// �t�@�C���֏�������
						oOfstream << Engine::scene_manager->Last_Save_Path;
					}
					Debug::Log(u8"�V�[���̕ۑ��ɐ���");
				}
				else
				{
					string path = System_Function::Get_Save_File_Name();
					if (path != "")
					{
						int path_i = path.find_last_of("\\") + 1;//7
						int ext_i = path.find_last_of(".");//10
						string pathname = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
						string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
						path = pathname + filename + ".bin";
						Engine::scene_manager->SaveScene(path);
						ofstream oOfstream("Default_Resource\\System\\last_save.bin");
						if (oOfstream.is_open())
						{
							// �t�@�C���֏�������
							oOfstream << Engine::scene_manager->Last_Save_Path;
						}
						Debug::Log(u8"�V�[���̕ۑ��ɐ���");
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
					Render_Cursor = true;
					Engine::scene_manager->Run = false;
					Engine::scene_manager->Pause = true;
					Engine::audio_manager->Suspend();
				}
			}
			else
			{
				if (Engine::scene_manager->Run)
				{
					Cursor::lockState = CursorLockMode::None;
					Cursor::visible = true;
					Render_Cursor = true;
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
						Cursor::lockState = CursorLockMode::None;
						Cursor::visible = true;
						ImGui::SetWindowFocus(u8"�Q�[��");
						Active_Object.reset();
						Active_Object_Old.reset();
						Engine::scene_manager->Start_DebugScene();
					}
					else
					{
						Engine::audio_manager->Resume();
					}
					Render_Cursor = false;
					Engine::scene_manager->Run = true;
					Engine::scene_manager->Pause = false;
				}
			}
		}
	}
}

//�V�[���r���[�̃J��������
void Editor::Debug_Camera_Update()
{
	ImGuiIO& io = ImGui::GetIO();
	//����
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

				Vector3 rot = Debug_Camera_Transform->Get_eulerAngles();
				rot.y -= dis_x;
				rot.x += dis_y;
				rot.z = 0;
				Debug_Camera_Transform->Set_eulerAngles(rot);

				mouse_old_pos = mouse_pos;
			}

			static float speed = 50;
			float mouse_wheel = io.MouseWheel;
			if (mouse_wheel != 0.0f)
			{
				speed += mouse_wheel * 5;
				if (speed < 5)
				{
					speed = 5;
				}
			}
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
				move += Debug_Camera_Transform->Get_right() * Time::deltaTime * speed;
			}
			if (ImGui::IsKeyDown(68))
			{
				move -= Debug_Camera_Transform->Get_right() * Time::deltaTime * speed;
			}
			Debug_Camera_Transform->Set_position(Debug_Camera_Transform->Get_position() + move);
		}
		else
		{
			mouse_old_pos = { -1,-1 };
		}
	}
}