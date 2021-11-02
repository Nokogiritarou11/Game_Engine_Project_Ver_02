#pragma once
#include "Component.h"

namespace BeastEngine
{
	//Enable�����R���|�[�l���g�̊��N���X
	class Behaviour : public Component
	{
	public:
		Behaviour();

		void Set_Enabled(bool value);           //�A�N�e�B�u��Ԃ�؂�ւ���
		[[nodiscard]] bool Get_Enabled() const; //�A�N�e�B�u��Ԃ��擾����

	protected:
		bool Draw_ImGui_Header(const std::string& component_name, bool& open); //Inspector���ŕ\�����鋤�ʂ̃^�u��`�悷�� �eDraw_ImGui�֐����ōŏ��ɌĂԕK�v������

	private:
		bool enabled = true;      //�A�N�e�B�u���
		bool enabled_old = false; //�ύX���g���K�[�p�L���b�V��

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Behaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Behaviour)
CEREAL_CLASS_VERSION(BeastEngine::Behaviour, 1)