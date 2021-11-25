#pragma once
#include "Character_Parameter.h"

namespace BeastEngine
{
	class Enemy_Parameter;

	class Player_Parameter final : public Character_Parameter, public MonoBehaviour
	{
	public:
		bool camera_locking = false;
		std::weak_ptr<Enemy_Parameter> target;

	private:
		void OnEnable() override;
		void Update() override;
		bool Draw_ImGui() override;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Character_Parameter>(this));
		}
	};
}

REGISTER_COMPONENT(Player_Parameter)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Parameter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Parameter)
CEREAL_CLASS_VERSION(BeastEngine::Player_Parameter, 1)