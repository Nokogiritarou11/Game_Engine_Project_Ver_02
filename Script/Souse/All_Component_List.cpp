#include "All_Component_List.h"
#include "Debug.h"
#include "Include_ImGui.h"
#include "GameObject.h"
#include "Camera.h"
#include "Mesh_Renderer.h"
//#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Animator.h"
#include "Particle.h"
#include "AudioSource.h"

/////////////////////////////////////////////////////////////////
//�g�������N���X(�R���|�[�l���g)���C���N���[�h
#include "Player.h"
#include"balet.h"
#include"muzzle.h"
#include"Bomb.h"
/////////////////////////////////////////////////////////////////

using namespace std;

void All_Component_List::Add(shared_ptr<GameObject> obj)
{
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	if (ImGui::Button(u8"�R���|�[�l���g��ǉ�", ImVec2(-FLT_MIN, 0.0f)))
	{
		ImGui::OpenPopup(u8"�R���|�[�l���g���X�g");
	}
	ImGui::SetNextWindowSize(ImVec2(300, 250));
	static int list_current = 0;
	if (ImGui::BeginPopup(u8"�R���|�[�l���g���X�g"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5f));

		if (ImGui::Button("Camera", ImVec2(-FLT_MIN, 0.0f)))
		{
			if (!obj->AddComponent<Camera>())
			{
				Debug::Log(u8"���̃R���|�[�l���g�͈�̃I�u�W�F�N�g�ɕ����A�^�b�`�ł��܂���");
			}
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Mesh_Renderer", ImVec2(-FLT_MIN, 0.0f)))
		{
			if (!obj->AddComponent<Mesh_Renderer>())
			{
				Debug::Log(u8"���̃R���|�[�l���g�͈�̃I�u�W�F�N�g�ɕ����A�^�b�`�ł��܂���");
			}
			ImGui::CloseCurrentPopup();
		}

		/*
		if (ImGui::Button("SkinMesh_Renderer", ImVec2(-FLT_MIN, 0.0f)))
		{
			if (!obj->AddComponent<SkinMesh_Renderer>())
			{
				Debug::Log(u8"���̃R���|�[�l���g�͈�̃I�u�W�F�N�g�ɕ����A�^�b�`�ł��܂���");
			}
			ImGui::CloseCurrentPopup();
		}
		*/

		if (ImGui::Button("Sprite_Renderer", ImVec2(-FLT_MIN, 0.0f)))
		{
			if (!obj->AddComponent<Sprite_Renderer>())
			{
				Debug::Log(u8"���̃R���|�[�l���g�͈�̃I�u�W�F�N�g�ɕ����A�^�b�`�ł��܂���");
			}
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Animator", ImVec2(-FLT_MIN, 0.0f)))
		{
			if (!obj->AddComponent<Animator>())
			{
				Debug::Log(u8"���̃R���|�[�l���g�͈�̃I�u�W�F�N�g�ɕ����A�^�b�`�ł��܂���");
			}
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Particle", ImVec2(-FLT_MIN, 0.0f)))
		{
			if (!obj->AddComponent<Particle>())
			{
				Debug::Log(u8"���̃R���|�[�l���g�͈�̃I�u�W�F�N�g�ɕ����A�^�b�`�ł��܂���");
			}
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("AudioSource", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<AudioSource>();
			ImGui::CloseCurrentPopup();
		}

		////////////////////////////////////////////////////////
		/* �ȉ����R�s�y���ă{�^�����쐬
		if (ImGui::Button("", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<>();
			ImGui::CloseCurrentPopup();
		}
		*/
		if (ImGui::Button("Player", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Player>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Balet", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Balet>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("muzzle", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Muzzle>();
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::Button("Bomb", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Bomb>();
			ImGui::CloseCurrentPopup();
		}

		////////////////////////////////////////////////////////
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}

}