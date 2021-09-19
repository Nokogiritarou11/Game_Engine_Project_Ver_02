#include "Disactive_Particle.h"

using namespace std;
using namespace BeastEngine;

void Disactive_Particle::Awake()
{
	particle = Get_Component<Particle>();
}

void Disactive_Particle::Update()
{
	if (auto& p = particle.lock())
	{
		if (!p->Get_Is_Playing())
		{
			if (remove_parent)
			{
				transform->Set_Parent(nullptr);
			}
			gameobject->Set_Active(false);
		}
	}
}

bool Disactive_Particle::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Disactive_Particle", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_Checkbox("remove_parent", "##remove_parent", &remove_parent, window_center);
	}
	return true;
}