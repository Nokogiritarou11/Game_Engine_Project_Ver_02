#include "Character_Hit_Stop_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Hit_Stop_Manager::Awake()
{
	parameter = Get_Component<Character_Parameter>();
	animator = Get_Component<Animator>();
}

void Character_Hit_Stop_Manager::Update()
{
	if (is_stop)
	{
		stop_timer += Time::delta_time;
		if (stop_timer > 0.1f)
		{
			for (auto& particle : stop_particles)
			{
				particle.lock()->Play();
			}

			animator.lock()->Set_Float("Hit_Stop_Speed", 1);
			parameter.lock()->pausing = false;

			stop_particles.clear();
			stop_timer = 0;
			is_stop = false;
		}
	}
}

void Character_Hit_Stop_Manager::Start_Hit_Stop(vector<weak_ptr<Particle>>& particles)
{
	stop_particles = particles;
	for (auto& particle : stop_particles)
	{
		particle.lock()->Pause();
	}

	animator.lock()->Set_Float("Hit_Stop_Speed", 0);
	parameter.lock()->pausing = true;

	is_stop = true;
}

bool Character_Hit_Stop_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Hit_Stop_Manager", open)) return false;

	if (open)
	{
		float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"İ’è‚Å‚«‚éƒpƒ‰ƒ[ƒ^‚Í‚ ‚è‚Ü‚¹‚ñ");
	}
	return true;
}