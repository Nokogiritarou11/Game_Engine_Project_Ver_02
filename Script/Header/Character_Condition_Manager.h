#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Interface_Character_Mover;
	class Interface_Character_State_Setter;

	class Character_Condition_Manager final : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Interface_Character_Mover> character_move;
		std::weak_ptr<Interface_Character_State_Setter> character_state_setter;
		std::weak_ptr<Animator> animator;

		int character_state_old = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Character_Condition_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Condition_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Condition_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Character_Condition_Manager, 1)