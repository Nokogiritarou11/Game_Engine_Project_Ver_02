#pragma once
#include "Collider.h"

namespace BeastEngine
{
	class Box_Collider :public BeastEngine::Collider
	{
	public:
		BeastEngine::Vector3 Get_Scaling_Size() const;
		BeastEngine::Vector3 size;

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		void Set_Active(bool value) override;


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