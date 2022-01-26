#include "UI_Targeting.h"
#include "Player_Parameter.h"
#include "Enemy_Parameter.h"

using namespace std;
using namespace BeastEngine;

void UI_Targeting::Awake()
{
	camera = GameObject::Find_With_Tag("main_camera").lock()->Get_Component<Camera>();
	parameter = GameObject::Find_With_Tag("player").lock()->Get_Component<Player_Parameter>();
	sprite_target = Get_Component<Sprite_Renderer>();
}

void UI_Targeting::Set_State(const int value)
{
	if (state != value)
	{
		state = value;
		switch (state)
		{
			case 0: //非表示
				gameobject->Set_Active(false);
				break;
			case 1: //通常表示
				gameobject->Set_Active(true);
				sprite_target.lock()->color = default_color;
				break;
			case 2: //ロックオン表示
				gameobject->Set_Active(true);
				sprite_target.lock()->color = locking_color;
				break;
			default:
				break;
		}
	}
}

void UI_Targeting::LateUpdate()
{
	//ターゲットの画面上の位置に移動させる
	const auto& param = parameter.lock();
	const auto& target = param->target.lock()->lock_on_marker.lock();
	const auto& sprite = sprite_target.lock();
	const auto& cam = camera.lock();

	Vector3 vec = target->Get_Position() - cam->transform->Get_Position();
	vec.Normalize();
	if (vec.Dot(cam->transform->Get_Forward()) >= 0)
	{
		const Vector2 target_view_pos = cam->World_To_Screen_Point(target->Get_Position());
		sprite->transform->Set_Position(Vector3(target_view_pos.x - sprite->size.x * 0.5f, target_view_pos.y - sprite->size.y * 0.5f, 0));
	}
	else
	{
		//画面外でもカメラの真後ろだとスクリーン座標変換が成功してしまうので、カメラの前かどうかを判定し、そうでなければ画面外へ
		sprite->transform->Set_Position(Vector3(-500, -500, 0));
	}
}

bool UI_Targeting::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("UI_Targeting", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_ColorEdit4(u8"通常時色", "##default_color", default_color, window_center);
		ImGui::LeftText_ColorEdit4(u8"ロックオン時色", "##locking_color", locking_color, window_center);
	}
	return true;
}
