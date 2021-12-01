#include "Cut_Scene_Smash_01.h"

using namespace std;
using namespace BeastEngine;

void Cut_Scene_Smash_01::Awake()
{
	//メンバポインタの取得
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	root_transform = camera_transform.lock()->Get_Parent();
	player_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

bool Cut_Scene_Smash_01::Play_Cut_Scene()
{
	//タイマーの更新
	state_timer += Time::delta_time;

	const auto& root = root_transform.lock();
	const auto& p_trans = player_transform.lock();
	//プレイヤーを追従する
	root->Set_Local_Position(Vector3::Lerp(root->Get_Position(), p_trans->Get_Position() + Vector3(0, 1, 0), Time::delta_time * 30.0f));

	if (cut_state == 0)
	{
		//カットシーン開始直後
		const auto& data = state_data[0];
		const auto& camera = camera_transform.lock();
		const float ratio = Time::delta_time * 30.0f;

		//急にカメラの視点が切り替わるのを防ぐため、Lerpで初期位置にセットする
		root->Set_Local_Euler_Angles(Vector3::Lerp(root->Get_Euler_Angles(), p_trans->Get_Euler_Angles(), ratio));
		camera->Set_Local_Position(Vector3::Lerp(camera->Get_Local_Position(), data.start_position, ratio));
		camera->Set_Local_Euler_Angles(Vector3::Lerp(camera->Get_Local_Euler_Angles(), data.start_rotation, ratio));
		const Vector3 r = camera->Get_Euler_Angles();
		camera->Set_Euler_Angles(r.x, r.y, 0);

		//ステートの切り替え
		if (state_timer >= camera_init_time)
		{
			state_timer = 0;
			++cut_state;
		}
	}
	else if (cut_state == 1)
	{
		//1つ目のカメラ移動シークエンス
		const auto& data = state_data[0];
		const auto& camera = camera_transform.lock();
		const float ratio = state_timer / data.change_time;

		camera->Set_Local_Position(Vector3::Lerp(data.start_position, data.end_position, ratio));
		camera->Set_Local_Euler_Angles(Vector3::Lerp(data.start_rotation, data.end_rotation, ratio));
		const Vector3 r = camera->Get_Euler_Angles();
		camera->Set_Euler_Angles(r.x, r.y, 0);

		//ステートの切り替え
		if (state_timer >= data.change_time)
		{
			state_timer = 0;
			++cut_state;
		}
	}
	else if (cut_state == 2)
	{
		//2つ目のカメラ移動シークエンス
		const auto& data = state_data[1];
		const auto& camera = camera_transform.lock();
		const float ratio = state_timer / data.change_time;

		camera->Set_Local_Position(Vector3::Lerp(data.start_position, data.end_position, ratio));
		camera->Set_Local_Euler_Angles(Vector3::Lerp(data.start_rotation, data.end_rotation, ratio));
		const Vector3 r = camera->Get_Euler_Angles();
		camera->Set_Euler_Angles(r.x, r.y, 0);

		//カットシーンの終了
		if (state_timer >= data.change_time)
		{
			state_timer = 0;
			cut_state = 0;
			return false;
		}
	}

	return true;
}

bool Cut_Scene_Smash_01::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Cut_Scene_Smash_01", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"開始時カメラ移動時間", "##camera_init_time", &camera_init_time, window_center);

		if (ImGui::TreeNode(u8"カメラ移動データ"))
		{
			int size = static_cast<int>(state_data.size());

			ImGui::Text(u8"要素数");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				state_data.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);

				const string label = u8"要素 " + to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					if (ImGui::TreeNode(u8"開始姿勢"))
					{
						ImGui::LeftText_DragFloat3(u8"位置", "##start_position", state_data[i].start_position, window_center);
						ImGui::LeftText_DragFloat3(u8"向き", "##start_rotation", state_data[i].start_rotation, window_center);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode(u8"終了姿勢"))
					{
						ImGui::LeftText_DragFloat3(u8"位置", "##end_position", state_data[i].end_position, window_center);
						ImGui::LeftText_DragFloat3(u8"向き", "##end_rotation", state_data[i].end_rotation, window_center);
						ImGui::TreePop();
					}
					ImGui::LeftText_DragFloat(u8"遷移時間", "##change_time", &state_data[i].change_time, window_center);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	return true;
}