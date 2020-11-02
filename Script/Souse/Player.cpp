#include "Animator.h"
#include "Camera.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Player.h"
using namespace std;

void Player::Awake()
{
}

void Player::Start()
{
	shared_ptr<Transform> trans = GameObject::Find("Test").lock()->transform;
	trans->Set_parent(transform);
}

void Player::Update()
{
}

bool Player::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Player"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Player_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Player>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}
	}
	return true;
}