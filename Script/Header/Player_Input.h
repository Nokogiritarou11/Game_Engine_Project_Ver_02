#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_State_Setter.h"

namespace BeastEngine
{
	class Character_Parameter;

	class Player_Input final : public MonoBehaviour, public Interface_Character_State_Setter
	{
	public:
		void Set_State() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Player_Input)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Input)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Input)
CEREAL_CLASS_VERSION(BeastEngine::Player_Input, 1)