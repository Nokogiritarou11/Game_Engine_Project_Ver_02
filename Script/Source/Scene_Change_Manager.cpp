#include "Scene_Change_Manager.h"
#include "Sound_Manager.h"

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

	if (fade_state == Fade_State::Before_Fade)
	{
		//delta_time���傫���Ȃ肷����̂�h�����߂�2�t���[���x��
		fade_timer = 0;
		if (++fade_frame > 1)
		{
			fade_state = Fade_State::Fade_Out;
		}
	}
	else if (fade_state == Fade_State::Fade_Out)
	{
		//�Ó]�p�e�N�X�`�����t�F�[�h�A�E�g������
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
	else if (fade_state == Fade_State::Fade_In)
	{
		//�Ó]�p�e�N�X�`�����t�F�[�h�C��������
		if (const float rate = fade_timer / fade_time; rate < 1)
		{
			fade->color.w = Mathf::Lerp(0, 1, rate);
		}
		else
		{
			fade->color.w = 1;
			fade_timer = 0;
			loading_object.lock()->Set_Active(true);
			fade_state = Fade_State::Fade_End;
		}
	}
	else if (fade_state == Fade_State::Fade_End)
	{
		//�t�F�[�h�C�����I�������̂ŃV�[���`�F���W�����s
		Scene_Manager::Load_Scene(next_scene);
	}
}

void Scene_Change_Manager::Change_Scene(const std::string& scene_name)
{
	//�t�F�[�h�I����ȊO�͖�����
	if (fade_state == Fade_State::Fade_Out)
	{
		next_scene = scene_name;
		fade_state = Fade_State::Fade_In;
		gameobject->Set_Active(true);
		GameObject::Find("Sound_Manager").lock()->Get_Component<Sound_Manager>()->Play_BGM(BGM_Name::None);
	}
}

bool Scene_Change_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Scene_Change_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragFloat(u8"�J�ڎ���", "##fade_time", &fade_time, window_center);
	}
	return true;
}