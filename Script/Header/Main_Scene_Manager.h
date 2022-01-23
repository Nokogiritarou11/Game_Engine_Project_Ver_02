#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Main_Scene_Manager final : public MonoBehaviour
	{
	public:
		void Game_Over(float delay_time);
		void Game_Clear(float delay_time);

	private:
		enum class Game_State { Playing, Game_Over, Game_Clear, Scene_Change };

		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Sprite_Renderer> fade_sprite;
		std::weak_ptr<Sprite_Renderer> over_sprite;
		std::weak_ptr<Sprite_Renderer> clear_sprite;
		Game_State state = Game_State::Playing;
		float fade_timer = 0;
		float fade_time = 0;
		float wait_timer = 0;
		float wait_time = 0;
		float end_time = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), fade_time, end_time);
		}
	};
}

REGISTER_COMPONENT(Main_Scene_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Main_Scene_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Main_Scene_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Main_Scene_Manager, 1)