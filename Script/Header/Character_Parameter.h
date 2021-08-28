#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter : public MonoBehaviour
	{
	public:
		float Max_HP = 0;
		float HP = 0;

		bool Eventing = false;
		bool Living = true;
		bool Moveing = false;
		bool Attacking = false;
		bool Dodging = false;
		bool Damaging = false;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), Max_HP);
		}
	};
}

REGISTER_COMPONENT(Character_Parameter)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Parameter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Parameter)
CEREAL_CLASS_VERSION(BeastEngine::Character_Parameter, 1)