#pragma once
#include "Collider.h"

namespace BeastEngine
{
	class Box_Collider :public BeastEngine::Collider
	{
	public:
		void Set_Size(BeastEngine::Vector3& new_size);
		BeastEngine::Vector3 Get_Size() const { return size; }

	private:
		void Create_Shape() override;
		void Resize_Shape() override;
		bool Draw_ImGui() override;

		BeastEngine::Vector3 size;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Collider>(this), size);
		}
	};
}

REGISTER_COMPONENT(Box_Collider)
CEREAL_REGISTER_TYPE(BeastEngine::Box_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Collider, BeastEngine::Box_Collider)
CEREAL_CLASS_VERSION(BeastEngine::Box_Collider, 1)