#pragma once
#include "Behaviour.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

namespace BeastEngine
{
	class Light : public Behaviour
	{
	public:
		Vector3 color = { 0.5f ,0.5f ,0.5f };
		float intensity = 1;

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override;
		bool Draw_ImGui() override;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), color, intensity);
		}
	};
}
REGISTER_COMPONENT(Light)
CEREAL_REGISTER_TYPE(BeastEngine::Light)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Light)
CEREAL_CLASS_VERSION(BeastEngine::Light, 1);