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
		struct Bar_Sprite
		{
			std::weak_ptr<Sprite_Renderer> sprite;
			float max = 0;
			float old = 0;
		};

		void Awake() override;
		void LateUpdate() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Parameter> parameter;
		Bar_Sprite bar_main;
		Bar_Sprite bar_delay;

		float delay_timer = 0;
		float delay_start_time = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), delay_start_time);
		}
	};
}

REGISTER_COMPONENT(UI_Health_Bar)
CEREAL_REGISTER_TYPE(BeastEngine::UI_Health_Bar)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::UI_Health_Bar)
CEREAL_CLASS_VERSION(BeastEngine::UI_Health_Bar, 1)