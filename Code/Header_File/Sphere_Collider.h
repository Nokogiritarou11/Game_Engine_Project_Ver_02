#pragma once
#include "Collider.h"

namespace BeastEngine
{
	//�X�t�B�A�R���C�_�[�R���|�[�l���g
	class Sphere_Collider :public Collider
	{
	public:
		void Set_Radius(float new_radius); //���a��ݒ肷��
		[[nodiscard]] float Get_Radius() const { return radius; } //���a���擾����

	private:
		void Create_Shape() override; //�`����쐬����(Sphere)
		bool Draw_ImGui() override; //ImGui�`��

		float radius = 0.5f; //���a

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Collider>(this), radius);
		}
	};
}

REGISTER_COMPONENT(Sphere_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Sphere_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Sphere_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Sphere_Collider, 1)