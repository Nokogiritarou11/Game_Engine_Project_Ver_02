#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Player_Parameter;

	class UI_Smash final : public MonoBehaviour
	{
	public:
		void Set_State(int value);

	private:
		void Awake() override;
		void LateUpdate() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Sprite_Renderer> sprite_main;
		int state = 0;
		float inactive_time = 0;
		float timer = 0;
		float end_scale = 0;
		bool is_playing = false;
		Vector4 start_color = { 1,1,1,1 };
		Vector4 end_color = { 1,1,1,1 };

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), inactive_time, end_scale, start_color, end_color);
		}
	};
}

REGISTER_COMPONENT(UI_Smash)
CEREAL_REGISTER_TYPE(BeastEngine::UI_Smash)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::UI_Smash)
CEREAL_CLASS_VERSION(BeastEngine::UI_Smash, 1)