#pragma once
#include <vector>
#include <unordered_map>
#include "Object.h"
#include "Animator_Condition.h"
#include "Animator_State_Machine.h"

namespace BeastEngine
{
	class Transform;
	class Animator_State_Transition;

	class Animator_Controller final : public Object
	{
	public:
		std::vector<std::shared_ptr<Animator_State_Machine>> state_machines;
		std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>> parameters;
		std::string save_path; //�㏑���Z�[�u�p�p�X

		void Initialize();               //������
		void Reset();                    //�N�����̃f�[�^�Ƀ��Z�b�g����
		void Update(const float& speed); //�X�V
		void Render_ImGui();             //ImGui�`��

		bool Add_State_Machine(const std::string& name);                    //�V�K�X�e�[�g�}�V����ǉ�����
		bool Remove_State_Machine(const std::string& name);                 //�X�e�[�g�}�V�����폜����
		void Add_Parameter(std::string& p_name, Parameter_Type type) const; //�V�K�A�j���[�V�����p�����[�^�[��ǉ�����

		static std::shared_ptr<Animator_Controller> Load_Animator_Controller(const std::string& full_path = ""); //�t�@�C���p�X����A�j���[�^�[�R���g���[���[��ǂݍ���
		static std::shared_ptr<Animator_Controller> Create_New_Controller(); //�V�K�A�j���[�V�����R���g���[���[�𐶐�����

		std::shared_ptr<Animator_State_Machine> playing_state_machine; //�Đ����̃X�e�[�g�}�V��
		std::shared_ptr<Animator_State_Machine> next_state_machine; //�J�ڐ�̃X�e�[�g�}�V��
		std::shared_ptr<Animator_State_Transition> active_transition; //���ݗL���ȑJ��
		float duration_timer = 0; //�J�ڔ���p�^�C�}�[
		int interrupt_state = 0;  //�J�ڊ��荞�݂̏�ԃX�e�[�g

	private:
		int current_state_index = 0;	  //GUI��őI�����ꂽ�X�e�[�g�}�V���̃C���f�b�N�X
		int current_transition_index = 0; //GUI��őI�����ꂽ�J�ڂ̃C���f�b�N�X

		void Save();    //�t�@�C���_�C�A���O����Z�[�u����
		void Save_As(); //save_path�ɏ㏑���Z�[�u����

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this), state_machines, parameters, save_path);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animator_Controller)
CEREAL_CLASS_VERSION(BeastEngine::Animator_Controller, 1)