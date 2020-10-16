#include "Stage_Manager.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Scroll.h"
#include "Block.h"
#include "SkinMesh_Renderer.h"
#include "Collider.h"
#include "Player.h"
using namespace std;

void Stage_Manager::Start()
{
	shared_ptr<GameObject> p = GameObject::Find("Player").lock();
	player = p->GetComponent<Player>();
	Scroll_Speed_Set = Scroll_Speed;

	//橋_通常
	{
		bridge_offset = { 0,0,39.8f };
		bridge_normal_count = 8;

		for (int i = 0; i < bridge_normal_count; i++)
		{
			shared_ptr<GameObject> bridge = GameObject::Instantiate(u8"Bridge_Normal_" + to_string(i));
			bridge->transform->Set_position(0, 10, -bridge_offset.z * 3 + bridge_offset.z * i);
			bridge->transform->Set_scale(0.12f, 0.1f, 0.1f);
			bridge->AddComponent<Scroll>();
			shared_ptr<SkinMesh_Renderer>skin = bridge->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\bridge\\BrideBoard_Straight\\", "BrideBoard_Straight"));
			//bridge->SetActive(false);
			Bridge_Normal.push_back(bridge);
		}
	}

	//瓦礫
	{
		block_obj_count = 10;
		for (int i = 0; i < block_obj_count; i++)
		{
			shared_ptr<GameObject> block = GameObject::Instantiate(u8"Block_" + to_string(i));
			block->transform->Set_position(0, 0, 0);
			block->transform->Set_scale(0.1f, 0.1f, 0.1f);

			block->AddComponent<Scroll>();
			block->AddComponent<Block>();
			shared_ptr<Collider> col = block->AddComponent<Collider>();
			col->Size_X = 1.0f;
			col->Size_Z = 1.0f;

			shared_ptr<SkinMesh_Renderer>skin = block->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\rubble\\", "rubble"));
			block->SetActive(false);
			Block_obj.push_back(block);
		}
	}
}

void Stage_Manager::Update()
{
	//橋_通常
	for (int i = 0; i < bridge_normal_count; i++)
	{
		shared_ptr<GameObject> bridge = Bridge_Normal[i].lock();
		if (bridge->transform->Get_position().z <= -bridge_offset.z * 3)
		{
			shared_ptr<GameObject> bridge_back;
			if (i == 0)
			{
				bridge_back = Bridge_Normal.back().lock();
			}
			else
			{
				bridge_back = Bridge_Normal[i - 1].lock();
			}
			bridge->transform->Set_position(bridge_back->transform->Get_position() + bridge_offset);
		}
	}
	//瓦礫



	//スクロール
	shared_ptr<Player> p = player.lock();
	if (p->Boosting)
	{
		Scroll_Speed = Scroll_Speed_Set * p->Boost_Magnification * p->Speed_Bonus_Magnification;
	}
	else
	{
		Scroll_Speed = Scroll_Speed_Set * p->Speed_Bonus_Magnification;
	}
}

bool Stage_Manager::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Stage_Manager"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Stage_Manager_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Stage_Manager>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}

		ImGui::DragFloat(u8"Scroll_Speed", &Scroll_Speed, 0.1f, 0.01f, FLT_MAX);
	}
	return true;
}