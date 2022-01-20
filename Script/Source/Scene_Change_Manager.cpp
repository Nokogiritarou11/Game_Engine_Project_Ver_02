#include "Scene_Change_Manager.h"
using namespace std;
using namespace BeastEngine;

void Scene_Change_Manager::Awake()
{
	fade_sprite = Get_Component<Sprite_Renderer>();
	loading_object = transform->Get_Child(0).lock()->gameobject;
}

void Scene_Change_Manager::Update()
{
	const auto& fade = fade_sprite.lock();
	fade_timer += Time::delta_time;

	if(fade_state <= 1)
	{
		fade_timer = 0;
		++fade_state;
	}
	else if (fade_state == 2)
	{
		if (const float rate = fade_timer / fade_time; rate < 1)
		{
			fade->color.w = Mathf::Lerp(1, 0, rate);
		}
		else
		{
			fade->color.w = 0;
			fade_timer = 0;
			gameobject->Set_Active(false);
		}
	}
	else if (fade_state == 3)
	{
		if (const float rate = fade_timer / fade_time; rate < 1)
		{
			fade->color.w = Mathf::Lerp(0, 1, rate);
		}
		else
		{
			fade->color.w = 1;
			fade_timer = 0;
			loading_object.lock()->Set_Active(true);
			++fade_state;
		}
	}
	else if (fade_state == 4)
	{
		Scene_Manager::Load_Scene(next_scene);
	}
}

void Scene_Change_Manager::Change_Scene(const std::string& scene_name)
{
	if (fade_state < 3)
	{
		next_scene = scene_name;
		fade_state = 3;
		gameobject->Set_Active(true);
	}
}

bool Scene_Change_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Scene_Change_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"‘JˆÚŽžŠÔ", "##fade_time", &fade_time, window_center);
	}
	return true;
}