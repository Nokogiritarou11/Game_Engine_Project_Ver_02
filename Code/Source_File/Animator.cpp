#include "Animator.h"
#include "Engine.h"
#include "Animator_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animation_Clip.h"
#include "Animator_State_Machine.h"
#include "Animator_Controller.h"
#include "Include_ImGui.h"
#include "Asset_Manager.h"
#include "Debug.h"
using namespace std;
using namespace BeastEngine;

void Animator::Initialize(const shared_ptr<GameObject>& obj)
{
	//�A�Z�b�g�}�l�[�W���[�ւ̓o�^��Component�̏�����
	Engine::asset_manager->Registration_Asset(shared_from_this());
	gameobject = obj;
	transform = obj->transform;

	//�A�^�b�`���ꂽ�R���g���[����ǂݍ���
	if (!controller_path.empty())
	{
		controller = Animator_Controller::Load_Animator_Controller(controller_path);
		controller->Initialize();
	}
	Set_Active(Get_Enabled());
}

void Animator::Set_Active(const bool value)
{
	if (value)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			if (Get_Enabled())
			{
				if (!is_called)
				{
					//����̂݃}�l�[�W���[�ɓo�^
					Engine::animator_manager->Add(static_pointer_cast<Animator>(shared_from_this()));
					is_called = true;
					Set_Default_Pose();
				}
				Activate();
			}
		}
	}
	else
	{
		Inactivate();
	}
}

void Animator::Activate()
{
	if (controller)
	{
		is_playing = true;

		//���Z�b�g�p�̃p�����[�^�[���R�s�[
		if (init_parameter.empty())
		{
			init_parameter = *controller->parameters.get();
		}
	}
}

void Animator::Inactivate()
{
	if (is_playing)
	{
		is_playing = false;

		//���Z�b�g����
		if (!keep_state_on_disable)
		{
			*controller->parameters.get() = init_parameter;
			controller->Reset();

			//�A�j���[�V������K��
			for (auto& data : pose_default)
			{
				auto& anim = data.second;
				if (const auto& target = anim.target.lock())
				{
					target->Set_Local_Position(anim.position);
					target->Set_Local_Rotation(anim.rotation);
					target->Set_Local_Scale(anim.scale);
				}
			}
		}
	}
}

void Animator::Set_Default_Pose()
{
	if (controller)
	{
		for (const auto& state : controller->state_machines)
		{
			if (state->clip)
			{
				//�e�A�j���[�V�����Ώۂ̌��݂̎p����ۑ�����
				for (auto& anim : state->clip->animations)
				{
					if (const auto& t_trans = transform->Find(anim.target_path).lock())
					{
						if (const auto it = pose_default.find(anim.target_path); it == pose_default.end())
						{
							const Animation_Target t = { t_trans, t_trans->Get_Local_Position(),t_trans->Get_Local_Rotation(),t_trans->Get_Local_Scale() };
							pose_default[anim.target_path] = t;
						}
					}
				}
			}
		}
		pose_playing = pose_default;
		pose_next = pose_default;
		pose_interrupt.clear();
	}
}

void Animator::Set_Int(const string& key, const int& value) const
{
	//Int�^���ǂ����m�F���Ă��珑��������
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Int)
		{
			it->second.value_int = value;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}
}
void Animator::Set_Float(const string& key, const float& value) const
{
	//Float�^���ǂ����m�F���Ă��珑��������
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Float)
		{
			it->second.value_float = value;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}
}
void Animator::Set_Bool(const string& key, const bool& value) const
{
	//Bool�^���ǂ����m�F���Ă��珑��������
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Bool)
		{
			it->second.value_bool = value;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}
}
void Animator::Set_Trigger(const string& key) const
{
	//Trigger�^���ǂ����m�F���Ă��珑��������
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Trigger)
		{
			it->second.value_bool = true;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}
}

int Animator::Get_Int(const string& key) const
{
	//Int�^���ǂ����m�F���Ă���擾����
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Int)
		{
			return it->second.value_int;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}

	return 0;
}

float Animator::Get_Float(const string& key) const
{
	//Float�^���ǂ����m�F���Ă���擾����
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Float)
		{
			return it->second.value_float;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}

	return 0;
}

bool Animator::Get_Bool(const string& key) const
{
	//Bool�^���ǂ����m�F���Ă���擾����
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Bool)
		{
			return it->second.value_bool;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}

	return false;
}

void Animator::Reset_Trigger(const string& key) const
{
	if (const auto it = controller->parameters->find(key); it != controller->parameters->end())
	{
		if (it->second.type == Parameter_Type::Trigger)
		{
			it->second.value_bool = false;
		}
		else
		{
			Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[�ƌ^����v���܂���");
		}
	}
	else
	{
		Debug::Log(u8"�w�肵���A�j���[�V�����p�����[�^�[��������܂���");
	}
}

void Animator::Update()
{
	if (!is_playing) return;
	if (!controller) return;

	controller->Update(speed);

	//�J�ڊ��荞�݂̏�Ԃɂ���ċ�����ύX
	switch (controller->interrupt_state)
	{
		case 1:
			pose_interrupt.clear();
			controller->interrupt_state = 0;
			break;
		case 2:
			pose_interrupt = animation_data;
			controller->interrupt_state = 0;
			break;
		default:
			break;
	}

	//�p���̏�����
	pose_playing = pose_default;
	pose_next = pose_default;
	animation_data = pose_default;

	//�J�ڒ����ǂ����ŕ���
	if (controller->next_state_machine)
	{
		//�J�ڌ��̎p�����Z�o
		const float& current_sec = controller->playing_state_machine->current_seconds;
		for (auto& animation : controller->playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int key_count = static_cast<int>(keyframes.size());
			bool is_end_frame = true;

			for (int keyIndex = 0; keyIndex < key_count - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.target_path];
					// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					is_end_frame = false;
					break;
				}
			}

			//�Đ����I������ꍇ�A�ŏI�t���[���̎p�����ێ�
			if (is_end_frame)
			{
				const Animation_Clip::Keyframe& last_keyframe = keyframes[key_count - 1];
				Animation_Target& target = pose_playing[animation.target_path];
				target.position = last_keyframe.position;
				target.rotation = last_keyframe.rotation;
				target.scale = last_keyframe.scale;
			}
		}

		//�J�ڐ�̎p�����Z�o
		const float& next_sec = controller->next_state_machine->current_seconds;
		for (auto& animation : controller->next_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int key_count = static_cast<int>(keyframes.size());
			bool is_end_frame = true;

			for (int keyIndex = 0; keyIndex < key_count - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (next_sec >= keyframe0.time && next_sec < keyframe1.time)
				{
					const float rate = (next_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_next[animation.target_path];
					// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					is_end_frame = false;
					break;
				}
			}

			//�Đ����I������ꍇ�A�ŏI�t���[���̎p�����ێ�
			if (is_end_frame)
			{
				const Animation_Clip::Keyframe& last_keyframe = keyframes[key_count - 1];
				Animation_Target& target = pose_next[animation.target_path];
				target.position = last_keyframe.position;
				target.rotation = last_keyframe.rotation;
				target.scale = last_keyframe.scale;
			}
		}

		//�u�����h�x�����̎Z�o
		float rate;
		if (controller->active_transition->transition_duration)
		{
			rate = controller->duration_timer / controller->active_transition->transition_duration;
		}
		else
		{
			rate = 1;
		}

		//�u�����h��̎p�����Z�o
		for (auto& data : animation_data)
		{
			const Animation_Target& playing = pose_playing[data.first];
			const Animation_Target& next = pose_next[data.first];
			data.second.position = Vector3::Lerp(playing.position, next.position, rate);
			data.second.rotation = Quaternion::Slerp(playing.rotation, next.rotation, rate);
			data.second.scale = Vector3::Lerp(playing.scale, next.scale, rate);
		}

		//���荞�݂̏ꍇ�̃u�����h
		if (!pose_interrupt.empty())
		{
			for (auto& data : animation_data)
			{
				const Animation_Target& playing = pose_interrupt[data.first];
				const Animation_Target& next = data.second;
				data.second.position = Vector3::Lerp(playing.position, next.position, rate);
				data.second.rotation = Quaternion::Slerp(playing.rotation, next.rotation, rate);
				data.second.scale = Vector3::Lerp(playing.scale, next.scale, rate);
			}
		}
	}
	else
	{
		//�A�j���[�V�������Z�o����
		const float& current_sec = controller->playing_state_machine->current_seconds;
		for (auto& animation : controller->playing_state_machine->clip->animations)
		{
			const vector<Animation_Clip::Keyframe>& keyframes = animation.keys;

			const int key_count = static_cast<int>(keyframes.size());
			bool is_end_frame = true;

			for (int keyIndex = 0; keyIndex < key_count - 1; ++keyIndex)
			{
				// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
				const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
				const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

				if (current_sec >= keyframe0.time && current_sec < keyframe1.time)
				{
					const float rate = (current_sec - keyframe0.time) / (keyframe1.time - keyframe0.time);

					Animation_Target& target = pose_playing[animation.target_path];
					// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
					target.position = Vector3::Lerp(keyframe0.position, keyframe1.position, rate);
					target.rotation = Quaternion::Slerp(keyframe0.rotation, keyframe1.rotation, rate);
					target.scale = Vector3::Lerp(keyframe0.scale, keyframe1.scale, rate);
					is_end_frame = false;
					break;
				}
			}

			//�Đ����I������ꍇ�A�ŏI�t���[���̎p�����ێ�
			if (is_end_frame)
			{
				const auto& last_keyframe = keyframes[key_count - 1];
				Animation_Target& target = pose_playing[animation.target_path];
				target.position = last_keyframe.position;
				target.rotation = last_keyframe.rotation;
				target.scale = last_keyframe.scale;
			}
		}
		animation_data = pose_playing;
	}

	//�A�j���[�V������K��
	for (auto& data : animation_data)
	{
		auto& anim = data.second;
		if (const auto& target = anim.target.lock())
		{
			target->Set_Local_Position(anim.position);
			target->Set_Local_Rotation(anim.rotation);
			target->Set_Local_Scale(anim.scale);
		}
	}
}

bool Animator::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Animator", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		if (controller)
		{
			ImGui::Text(u8"�A�^�b�`�� : ");
			ImGui::SameLine();
			const string controller_name = controller->name + ".controller";
			ImGui::Text(controller_name.c_str());
		}
		else
		{
			ImGui::Text(u8"�A�j���[�^�[�R���g���[���[���o�^����Ă��܂���");
		}

		ImGui::LeftText_Checkbox(u8"��A�N�e�B�u���Ƀ��Z�b�g���Ȃ�", "##keep_state_on_disable", &keep_state_on_disable, window_center);

		ImGui::Dummy({ 0.0f, 10.0f });
		if (ImGui::Button(u8"�V�K�쐬"))
		{
			controller = Animator_Controller::Create_New_Controller();
			if (controller)
			{
				controller_path = controller->save_path;
				controller->Initialize();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"�ǂݍ���"))
		{
			controller = Animator_Controller::Load_Animator_Controller();
			if (controller)
			{
				controller_path = controller->save_path;
				controller->Initialize();
				Set_Default_Pose();
			}
		}
	}
	return true;
}