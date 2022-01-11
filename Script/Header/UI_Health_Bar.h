#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;

	class UI_Health_Bar final : public MonoBehaviour
	{
	public:
		void Set_Parameter(const std::weak_ptr<Character_Parameter>& character_parameter);

	private:
		void Awake() override;
		void LateUpdate() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Sprite_Renderer> fill_sprite;
		float fill_max = 0;
		float health_old = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(UI_Health_Bar)
CEREAL_REGISTER_TYPE(BeastEngine::UI_Health_Bar)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::UI_Health_Bar)
CEREAL_CLASS_VERSION(BeastEngine::UI_Health_Bar, 1)