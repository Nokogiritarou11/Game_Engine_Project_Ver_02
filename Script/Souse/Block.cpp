#include "Block.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Stage_Manager.h"
#include "Player.h"
using namespace std;

void Block::Start()
{
	shared_ptr<GameObject> p = GameObject::Find("Player").lock();
	player_trans = p->GetComponent<Transform>();
	player = p->GetComponent<Player>();
}

void Block::Update()
{
	shared_ptr<Transform> p_trans = player_trans.lock();

	if (Vector3::Distance(p_trans->Get_position(), transform->Get_position()) < Decision_Size)
	{
		shared_ptr<Player> p = player.lock();
		if (p->Boosting)
		{
			gameObject->SetActive(false);
		}
	}
}

bool Block::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Block"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Block_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Block>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}
		ImGui::DragFloat(u8"Decision_Size", &Decision_Size, 0.1f, 0.01f, FLT_MAX);
	}
	return true;
}