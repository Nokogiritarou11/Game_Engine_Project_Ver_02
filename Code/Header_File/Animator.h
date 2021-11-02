#pragma once
#include "Behaviour.h"
#include "Animator_Controller.h"

namespace BeastEngine
{
	class GameObject;
	class Transform;
	class Animator_Manager;

	//�A�j���[�^�[�R���|�[�l���g
	class Animator final : public Behaviour
	{
	public:
		void Play();  //�Đ�����
		void Stop();  //��~����
		void Pause(); //�ꎞ��~����

		void Set_Int(const std::string& key, const int& value) const;     //Key�ɑΉ�����Int�^�p�����[�^�[��value�ŏ���������
		void Set_Float(const std::string& key, const float& value) const; //Key�ɑΉ�����Float�^�p�����[�^�[��value�ŏ���������
		void Set_Bool(const std::string& key, const bool& value) const;   //Key�ɑΉ�����Bool�^�p�����[�^�[��value�ŏ���������
		void Set_Trigger(const std::string& key) const;                   //Key�ɑΉ�����Trigger�^�p�����[�^�[��true�ɂ���

		[[nodiscard]] int Get_Int(const std::string& key) const;     //Key�ɑΉ�����Int�^�p�����[�^�[��Ԃ�
		[[nodiscard]] float Get_Float(const std::string& key) const; //Key�ɑΉ�����Float�^�p�����[�^�[��Ԃ�
		[[nodiscard]] bool Get_Bool(const std::string& key) const;   //Key�ɑΉ�����Bool�^�p�����[�^�[��Ԃ�

		void Reset_Trigger(const std::string& key) const; //Key�ɑΉ�����Trigger�^�p�����[�^�[�����Z�b�g����

		std::shared_ptr<Animator_Controller> controller;

	private:
		//map�o�^�p�A�j���[�V�����f�[�^
		struct Animation_Target
		{
			std::weak_ptr<Transform> target;
			Vector3 position;
			Quaternion rotation;
			Vector3 scale;
		};

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //������
		bool Draw_ImGui() override; //ImGui�`��
		bool Can_Multiple() override { return false; }; //���R���|�[�l���g�𕡐��A�^�b�`�\��

		void Set_Default_Pose(); //�A�j���[�V�����u�����h���ɕK�v�ȏ����|�[�Y��Ԃ�ݒ肷��
		void Update(); //�X�V

		bool is_playing = true;

		std::string controller_path; //�A�j���[�V�����R���g���[���[�̃t�@�C���p�X

		std::unordered_map<std::string, Animation_Target> animation_data; //�R���g���[���[�ɓǂݍ��܂ꂽ�S�N���b�v�ɑ��݂���Animation�̈ꗗ
		std::unordered_map<std::string, Animation_Target> pose_default;   //�{�[��Transform�̏����p��
		std::unordered_map<std::string, Animation_Target> pose_playing;   //�Đ�����Ă���N���b�v�A�j���[�V�����̃|�[�Y
		std::unordered_map<std::string, Animation_Target> pose_next;      //�u�����h�p�J�ڐ�A�j���[�V�����̃|�[�Y
		std::unordered_map<std::string, Animation_Target> pose_interrupt; //�J�ڊ��荞�ݎ��̃u�����h�p���f�|�[�Y

		friend class Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Behaviour>(this), controller_path);
		}
	};
}

REGISTER_COMPONENT(Animator)
CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)
CEREAL_CLASS_VERSION(BeastEngine::Animator, 1)