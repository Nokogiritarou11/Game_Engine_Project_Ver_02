#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;

	class UI_Manager final : public MonoBehaviour
	{
	public:
		void Activate_Player_Health_Bar(const std::weak_ptr<Character_Parameter>& character_parameter) const;
		void Activate_Enemy_Health_Bar(const std::weak_ptr<Character_Parameter>& character_parameter) const;

	private:
		bool Draw_ImGui() override;

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