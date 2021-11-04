#pragma once
#include "Collider.h"

namespace BeastEngine
{
	//�V�����_�[�R���C�_�[
	class Cylinder_Collider final :public Collider
	{
	public:
		void Set_Radius(float new_radius); //���a��ݒ肷��
		void Set_Height(float new_height); //�����Ɛݒ肷��
		[[nodiscard]] float Get_Radius() const { return radius; } //���a���擾����
		[[nodiscard]] float Get_Height() const { return height; } //�������擾����

	private:
		void Create_Shape() override; //�`����쐬����(Cylinder)
		bool Draw_ImGui() override;    //ImGui�`��

		float radius = 0.5f;
		float height = 1.0f;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Collider>(this), radius, height);
		}
	};
}

REGISTER_COMPONENT(Cylinder_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Cylinder_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Cylinder_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Cylinder_Collider, 1)