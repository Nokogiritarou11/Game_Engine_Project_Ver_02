#include "Break_Block.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "SkinMesh_Renderer.h"
using namespace std;

void Break_Block::OnEnable()
{
	shared_ptr<GameObject> obj;
	for (int i = 0; i < 4; ++i)
	{
		obj = blocks[i].lock();
		obj->SetActive(true);
		obj->transform->Set_position(transform->Get_position());
		add_vec[i] = { Mathf::Random_Range(0,1.0f),1.0f,Mathf::Random_Range(0,1.0f) };
		add_vec[i].Normalize();
		rot_vec[i] = { Mathf::Random_Range(0,180.0f),Mathf::Random_Range(0,180.0f),Mathf::Random_Range(0,180.0f) };
	}
}

void Break_Block::Update()
{
	shared_ptr<GameObject> obj;
	for (int i = 0; i < 4; ++i)
	{
		obj = blocks->lock();
		obj->transform->Set_position(obj->transform->Get_position() + add_vec[i] * 3.0f * Time::deltaTime);
		obj->transform->Set_eulerAngles(obj->transform->Get_eulerAngles() + rot_vec[i] * Time::deltaTime);
	}
	Disable_Timer += Time::deltaTime;
	if (Disable_Timer > 2.0f)
	{
		for (int i = 0; i < 4; ++i)
		{
			obj = blocks[i].lock();
			obj->SetActive(false);
		}
		gameObject->SetActive(false);
	}
}

bool Break_Block::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Break_Block"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Break_Block_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Break_Block>(shared_from_this()));
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