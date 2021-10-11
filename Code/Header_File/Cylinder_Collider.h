#pragma once
#include "Collider.h"

namespace BeastEngine
{
	class Cylinder_Collider :public Collider
	{
	public:
		void Set_Radius(float new_radius);
		void Set_Height(float new_height);
		[[nodiscard]] float Get_Radius() const { return radius; }
		[[nodiscard]] float Get_Height() const { return height; }

	private:
		void Create_Shape() override;
		bool Draw_ImGui() override;

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