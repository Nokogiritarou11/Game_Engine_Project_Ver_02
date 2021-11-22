#pragma once
#include "Character_Parameter.h"

namespace BeastEngine
{
	enum class Enemy_Type
	{
		Enemy_Normal_01,
		Enemy_Big_01,
		Boss_01
	};

	class Enemy_Parameter final : public Character_Parameter, public MonoBehaviour
	{
	public:
		Enemy_Type type;
		bool stunning = false;
		bool is_attack_preliminary = false;

	private:
		void OnEnable() override;
		bool Draw_ImGui() override;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Character_Parameter>(this), type, max_hp, max_stun, heal_stun);
		}
	};
}

REGISTER_COMPONENT(Enemy_Parameter)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Parameter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Parameter)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Parameter, 1)