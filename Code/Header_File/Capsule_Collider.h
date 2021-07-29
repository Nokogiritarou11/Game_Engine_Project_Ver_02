#pragma once
#include "Collider.h"

namespace BeastEngine
{
	class Capsule_Collider :public BeastEngine::Collider
	{
	public:
		void Set_Radius(float new_radius);
		void Set_Height(float new_height);
		float Get_Radius() const { return radius; }
		float Get_Height() const { return height; }

	private:
		void Create_Shape() override;
		bool Draw_ImGui() override;

		float radius = 0.5f;
		float height = 1.0f;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Collider>(this), radius, height);
		}
	};
}

REGISTER_COMPONENT(Capsule_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Capsule_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Capsule_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Capsule_Collider, 1)