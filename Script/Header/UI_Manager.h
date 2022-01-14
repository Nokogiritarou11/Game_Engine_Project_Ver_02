#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;
	class UI_Targeting;
	class UI_Smash;

	class UI_Manager final : public MonoBehaviour
	{
	public:
		void Activate_Player_Health_Bar(const std::weak_ptr<Character_Parameter>& character_parameter) const;
		void Activate_Enemy_Health_Bar(const std::weak_ptr<Character_Parameter>& character_parameter) const;
		void Set_Target_State(int value) const;
		void Set_Smash_State(int value) const;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<UI_Targeting> ui_target;
		std::weak_ptr<UI_Smash> ui_smash;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(UI_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::UI_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::UI_Manager)
CEREAL_CLASS_VERSION(BeastEngine::UI_Manager, 1)