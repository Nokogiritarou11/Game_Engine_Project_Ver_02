#include "All_Component_List.h"
#include "Include_ImGui.h"
#include "GameObject.h"

/////////////////////////////////////////////////////////////////
//使いたいクラス(コンポーネント)をインクルード
#include "Camera.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Animator.h"
#include "Particle.h"
#include "Player.h"
#include "Scroll.h"
#include "Stage_Manager.h"
/////////////////////////////////////////////////////////////////

using namespace std;

void All_Component_List::Add(shared_ptr<GameObject> obj)
{
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	if (ImGui::Button(u8"コンポーネントを追加", ImVec2(-FLT_MIN, 0.0f)))
	{
		ImGui::OpenPopup(u8"コンポーネントリスト");
	}
	ImGui::SetNextWindowSize(ImVec2(300, 250));
	static int list_current = 0;
	if (ImGui::BeginPopup(u8"コンポーネントリスト"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5f));

		////////////////////////////////////////////////////////
		/* 以下をコピペしてボタンを作成
		if (ImGui::Button("", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<>();
			ImGui::CloseCurrentPopup();
		}
		*/
		if (ImGui::Button("Camera", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Camera>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("SkinMesh_Renderer", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<SkinMesh_Renderer>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Sprite_Renderer", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Sprite_Renderer>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Animator", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Animator>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Particle", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Particle>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Player", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Player>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Scroll", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Scroll>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Stage_Manager", ImVec2(-FLT_MIN, 0.0f)))
		{
			obj->AddComponent<Stage_Manager>();
			ImGui::CloseCurrentPopup();
		}

		////////////////////////////////////////////////////////
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}

}