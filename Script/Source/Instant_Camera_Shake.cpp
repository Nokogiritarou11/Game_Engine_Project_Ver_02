#include "Instant_Camera_Shake.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Instant_Camera_Shake::Awake()
{
	//メンバポインタの取得
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
}

void Instant_Camera_Shake::OnEnable()
{
	//アクティブ化時にカメラシェイクを呼ぶ
	camera_controller.lock()->Shake_Camera(shake_count, shake_power);
}

bool Instant_Camera_Shake::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Instant_Camera_Shake", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::LeftText_DragInt(u8"回数", "##shake_count", &shake_count, window_center);
		ImGui::LeftText_DragFloat(u8"強度", "##shake_power", &shake_power, window_center);
	}
	return true;
}