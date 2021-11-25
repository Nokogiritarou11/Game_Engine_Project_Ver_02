#pragma once
#include "Collider.h"

namespace BeastEngine
{
	//�{�b�N�X�R���C�_�[
	class Box_Collider final :public Collider
	{
	public:
		void Set_Size(const Vector3& new_size); //�T�C�Y��ݒ肷��
		[[nodiscard]] Vector3 Get_Size() const { return size; } //�T�C�Y���擾����

	private:
		void Create_Shape() override; //�`����쐬����(Box)
		bool Draw_ImGui() override; //ImGui�`��

		Vector3 size = { 1.0f,1.0f ,1.0f };

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Collider>(this), size);
		}
	};
}

REGISTER_COMPONENT(Box_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Box_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Box_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Box_Collider, 1)