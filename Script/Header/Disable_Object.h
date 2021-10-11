#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Disable_Object : public MonoBehaviour
	{
	private:
		void Update() override;
		bool Draw_ImGui() override;

		bool remove_parent;
		float disable_time;
		float timer;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), remove_parent, disable_time);
		}
	};
}

REGISTER_COMPONENT(Disable_Object)
CEREAL_REGISTER_TYPE(BeastEngine::Disable_Object)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Disable_Object)
CEREAL_CLASS_VERSION(BeastEngine::Disable_Object, 1)