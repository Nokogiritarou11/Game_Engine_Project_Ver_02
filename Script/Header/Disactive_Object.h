#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Disactive_Object : public MonoBehaviour
	{
	public:

	private:
		void Update() override;
		bool Draw_ImGui() override;

		bool remove_parent;
		float disactive_time;
		float timer;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), remove_parent, disactive_time);
		}
	};
}

REGISTER_COMPONENT(Disactive_Object)
CEREAL_REGISTER_TYPE(BeastEngine::Disactive_Object)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Disactive_Object)
CEREAL_CLASS_VERSION(BeastEngine::Disactive_Object, 1)