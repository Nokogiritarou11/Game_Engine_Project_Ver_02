#include "Disable_Particle.h"

using namespace std;
using namespace BeastEngine;

void Disable_Particle::Awake()
{
	//メンバポインタの取得
	particle = Get_Component<Particle>();
}

void Disable_Particle::Update()
{
	//パーティクルの存在チェック
	if (const auto& p = particle.lock())
	{
		//パーティクルが再生終了していた場合
		if (!p->Get_Is_Playing())
		{
			//親子を解除する場合
			if (remove_parent)
			{
				transform->Set_Parent(nullptr);
			}
			//ゲームオブジェクトを非アクティブ化
			gameobject->Set_Active(false);
		}
	}
}

bool Disable_Particle::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Disable_Particle", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_Checkbox("remove_parent", "##remove_parent", &remove_parent, window_center);
	}
	return true;
}