#include "Title.h"
#include "Scene_Change_Manager.h"
#include "Sound_Manager.h"
using namespace std;
using namespace BeastEngine;

void Title::Awake()
{
	start_sprite = transform->Find("Start_Sprite").lock()->Get_Component<Sprite_Renderer>();
	scene_change_manager = GameObject::Find("Scene_Change_Manager").lock()->Get_Component<Scene_Change_Manager>();
	GameObject::Find("Sound_Manager").lock()->Get_Component<Sound_Manager>()->Play_BGM(BGM_Name::Title);
}

void Title::Update()
{
	const auto& fade = start_sprite.lock();
	timer += Time::delta_time;

	//画像を点滅させる
	if (state == 0)
	{
		if (const float rate = timer; rate < 1)
		{
			fade->color.w = Mathf::Lerp(0, 1, rate);
		}
		else
		{
			fade->color.w = 1;
			timer = 0;
			++state;
		}
	}
	else if (state == 1)
	{
		if(timer >= 0.5f)
		{
			timer = 0;
			++state;
		}
	}
	else if (state == 2)
	{
		if (const float rate = timer; rate < 1)
		{
			fade->color.w = Mathf::Lerp(1, 0, rate);
		}
		else
		{
			fade->color.w = 0;
			timer = 0;
			state = 0;
		}
	}

	//スタートボタンが押されたら暗転後シーン切り替え
	if(Input::Get_Pad_Button_Down(Button_Code::Start))
	{
		scene_change_manager.lock()->Change_Scene("Assets\\Scene\\Main_Stage.bin");
	}
}

bool Title::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Title", open)) return false;

	if (open)
	{
		//const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"設定可能なパラメータはありません");
	}
	return true;
}