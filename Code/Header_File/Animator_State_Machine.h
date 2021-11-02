#pragma once
#include "Animation_Clip.h"
#include "Animator_State_Transition.h"

namespace BeastEngine
{
	class Transform;

	//�A�j���[�^�[�R���g���[���[���Ŏg�p�����X�e�[�g�}�V���N���X
	class Animator_State_Machine
	{
	public:
		std::string name;
		std::shared_ptr<Animation_Clip> clip;
		std::vector<std::shared_ptr<Animator_State_Transition>> transitions;
		std::vector<Animation_Event> animation_events;
		std::vector<State_Event> state_events;

		bool transition_trigger = false;   //�J�ڂ��L����

		float animation_speed = 1;         //�Đ����x
		bool use_speed_multiplier = false; //�Đ����x�Ƀp�����[�^�[�搔���g�p���邩
		std::string multiplier_hash;       //�p�����[�^�[�搔�̃n�b�V���L�[
		int start_frame = 0;               //�A�j���[�V�����N���b�v�̍Đ��J�n�t���[��
		int end_frame = -1;                //�A�j���[�V�����N���b�v�̍Đ��I���t���[��
		bool  is_loop_animation = false;   //���[�v���邩
		bool  is_default_state = false;    //���������ɍŏ��ɑI�������X�e�[�g��

		float current_seconds = 0.0f;      //�Đ�����
		bool  is_end_animation = false;    //�A�j���[�V�����Đ����I�����Ă��邩

		void Initialize(const std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>>& p_parameters); //������
		void Set_Clip(const std::string& full_path); //�t�@�C���p�X����N���b�v��ǂݍ��݃Z�b�g����
		void Activate(float transition_offset = 0.0f); //�J�ڎ��ȂǃA�N�e�B�u�����ɌĂ�
		void Exit(); //�X�e�[�g�𔲂���ۂɌĂ�
		void Update_Transition(); //�J�ڏ�Ԃ��X�V����
		void Update_Time(); //�Đ����Ԃ��X�V����
		void Add_Transition(std::shared_ptr<Animator_State_Machine>& next_state); //�J�ڂ�ǉ�����
		void Add_Animation_Event(); //�A�j���[�V�����C�x���g��ǉ�����
		void Add_State_Event(); //�X�e�[�g�C�x���g��ǉ�����
		void Remove_Transition(int index); //�J�ڂ��폜����
		[[nodiscard]] std::shared_ptr<Animator_State_Transition> Get_Active_Transition() const; //���ݗL���ȑJ�ڂ��擾����

	private:
		std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>> parameters;
		std::string path; //�N���b�v�̃p�X
		std::shared_ptr<Animator_State_Transition> active_transition;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(name, path, animation_speed, use_speed_multiplier, multiplier_hash, start_frame, end_frame, is_loop_animation, is_default_state, transitions, animation_events, state_events);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Animator_State_Machine, 1)