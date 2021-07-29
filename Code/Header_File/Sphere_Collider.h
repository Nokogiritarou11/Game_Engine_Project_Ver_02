#pragma once
#include "Collider.h"

namespace BeastEngine
{
	class Sphere_Collider :public BeastEngine::Collider
	{
	public:
		void Set_Radius(float new_radius);
		float Get_Radius() const { return radius; }

	private:
		void Create_Shape() override;
		bool Draw_ImGui() override;

		float radius = 0.5f;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Collider>(this), radius);
		}
	};
}

REGISTER_COMPONENT(Sphere_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Sphere_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Sphere_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Sphere_Collider, 1)