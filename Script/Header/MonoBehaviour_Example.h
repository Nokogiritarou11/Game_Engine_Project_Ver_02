#pragma once
#include "MonoBehaviour.h"

//�R�s�y�pMonoBehaviour�e���v���[�g
/*
namespace BeastEngine
{
	//������R���|�[�l���g�g�p���̑O���錾



	class Hoge : public MonoBehaviour
	{
	public:
		void Awake();      //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(Enable�Ɉˑ����A�Q�[���I�u�W�F�N�g��Active�𖳎�����)
		void Start();      //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(�Q�[���I�u�W�F�N�g��Active�Ǝ��g��Enable�Ɉˑ�����)
		void Update();     //�A�N�e�B�u���ɖ��t���[���Ă΂��
		void LateUpdate(); //�A�N�e�B�u���ɖ��t���[���AUpdate�̂��ƂɌĂ΂��

		void OnEnable();  //�A�N�e�B�u�ɂȂ������ɌĂ΂��
		void OnDisable(); //��A�N�e�B�u�ɂȂ������ɌĂ΂��
		void OnDestroy(); //�폜���ɌĂ΂��

		void OnTrigger_Enter(BeastEngine::Collision& collision);   //���̃I�u�W�F�N�g�ɃA�^�b�`���ꂽ�R���C�_�[�����R���C�_�[�ɐڐG�����Ƃ�
		void OnCollision_Enter(BeastEngine::Collision& collision); //��g���K�[��

		void OnTrigger_Stay(BeastEngine::Collision& collision);    //�ڐG���Ƀt���[���ȏ㑱�����Ƃ�(�ڐG���ێ��������薈�t���[��)
		void OnCollision_Stay(BeastEngine::Collision& collision);  //��g���K�[��

		void OnTrigger_Exit(BeastEngine::Collision& collision);    //�ڐG���Ă����R���C�_�[�Ɨ��ꂽ�Ƃ�
		void OnCollision_Exit(BeastEngine::Collision& collision);  //��g���K�[��

		bool Draw_ImGui() override;

	private:


		// �V���A���C�Y�֐�
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			//�V���A���C�Y�������ϐ��𖖔��ɒǉ�
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this));
		}
	};
}

//�ȉ��}�N���̃N���X����������������
REGISTER_COMPONENT(Hoge)
CEREAL_REGISTER_TYPE(BeastEngine::Hoge)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Hoge)
CEREAL_CLASS_VERSION(BeastEngine::Hoge, 1)

*/