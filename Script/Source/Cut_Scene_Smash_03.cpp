#include "Cut_Scene_Smash_03.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Cut_Scene_Smash_03::Awake()
{
	//�����o�|�C���^�̎擾
	camera_transform = GameObject::Find_With_Tag("main_camera").lock()->transform;
	camera_controller = camera_transform.lock()->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	root_transform = camera_transform.lock()->Get_Parent();
	player_transform = GameObject::Find_With_Tag("player").lock()->transform;
}

Vector3 Cut_Scene_Smash_03::Play_Cut_Scene()
{
	//�^�C�}�[�̍X�V
	state_timer += Time::delta_time;

	const auto& root = root_transform.lock();
	const auto& p_trans = player_transform.lock();
	const auto& camera = camera_transform.lock();

	//�v���C���[��Ǐ]����
	root->Set_Local_Position(Vector3::Lerp(root->Get_Position(), p_trans->Get_Position() + Vector3(0, 1, 0), Time::delta_time * 30.0f));

	if (cut_state == -1)
	{
		//�J�b�g�V�[���J�n����
		const float ratio = state_timer / camera_init_time;

		//�}�ɃJ�����̎��_���؂�ւ��̂�h�����߁ALerp�ŏ����ʒu�ɃZ�b�g����
		root->Set_Local_Euler_Angles(Vector3::Lerp(root->Get_Euler_Angles(), p_trans->Get_Euler_Angles(), ratio));

		//�X�e�[�g�̐؂�ւ�
		if (state_timer >= camera_init_time)
		{
			state_timer = 0;
			++cut_state;
		}
	}
	else
	{
		//�J�����ړ��V�[�N�G���X
		const auto& data = state_data[cut_state];
		const float ratio = state_timer / data.change_time;

		camera->Set_Local_Position(Vector3::Lerp(data.start_position, data.end_position, ratio));
		camera->Set_Local_Euler_Angles(Vector3::Lerp(data.start_rotation, data.end_rotation, ratio));
		const Vector3 r = camera->Get_Euler_Angles();
		camera->Set_Euler_Angles(r.x, r.y, 0);

		//�X�e�[�g�̐؂�ւ�
		if (state_timer >= data.change_time)
		{
			if (cut_state == 0)
			{
				camera_controller.lock()->Shake_Camera(12, 0.1f);
			}
			if (cut_state == 1)
			{
				camera_controller.lock()->Shake_Camera(12, 0.05f);
			}

			state_timer = 0;
			++cut_state;

			if (cut_state >= static_cast<int>(state_data.size()))
			{
				cut_state = -1;
				is_end_play = true;
			}
		}
	}

	return camera->Get_Local_Position();
}

bool Cut_Scene_Smash_03::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Cut_Scene_Smash_03", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"�J�n���J�����ړ�����", "##camera_init_time", &camera_init_time, window_center);

		if (ImGui::TreeNode(u8"�J�����ړ��f�[�^"))
		{
			int size = static_cast<int>(state_data.size());

			ImGui::Text(u8"�v�f��");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				state_data.resize(size);
			}

			for (int i = 0; i < size; ++i)
			{
				ImGui::PushID(i);

				const string label = u8"�v�f " + to_string(i);
				if (ImGui::TreeNode(label.c_str()))
				{
					if (ImGui::TreeNode(u8"�J�n�p��"))
					{
						ImGui::LeftText_DragFloat3(u8"�ʒu", "##start_position", state_data[i].start_position, window_center);
						ImGui::LeftText_DragFloat3(u8"����", "##start_rotation", state_data[i].start_rotation, window_center);
						ImGui::TreePop();
					}
					if (ImGui::TreeNode(u8"�I���p��"))
					{
						ImGui::LeftText_DragFloat3(u8"�ʒu", "##end_position", state_data[i].end_position, window_center);
						ImGui::LeftText_DragFloat3(u8"����", "##end_rotation", state_data[i].end_rotation, window_center);
						ImGui::TreePop();
					}
					ImGui::LeftText_DragFloat(u8"�J�ڎ���", "##change_time", &state_data[i].change_time, window_center);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}
			ImGui::TreePop();
		}
	}
	return true;
}