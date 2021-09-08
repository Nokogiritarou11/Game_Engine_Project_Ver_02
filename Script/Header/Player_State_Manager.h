#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Character_Mover;

	class Player_State_Manager : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Character_Mover> player_move;
		std::weak_ptr<Animator> animator;

		int player_state;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Player_State_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Player_State_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_State_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Player_State_Manager, 1)