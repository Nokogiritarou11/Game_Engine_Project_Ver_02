#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Enemy_Manager;
	class Character_Condition_Manager;

	class Tutorial_Stage final : public MonoBehaviour
	{
	private:
		enum class Tutorial_State { Move, Parry, Scene_Change };
		enum class View_State { Wait, Stop, Close, Playing };

		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::vector<std::weak_ptr<GameObject>> stop_sprite{};
		std::vector<std::weak_ptr<GameObject>> playing_sprite{};
		std::weak_ptr<Enemy_Manager> enemy_manager;
		std::weak_ptr<Character_Condition_Manager> condition_manager;
		Tutorial_State tutorial_state = Tutorial_State::Move;
		View_State view_state = View_State::Wait;
		float timer = 0;
		float wait_time = 0;
		float end_time = 0;
		int frame_count = 0;
		bool is_changing = false;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), stop_sprite, playing_sprite, wait_time, end_time);
		}
	};
}

REGISTER_COMPONENT(Tutorial_Stage)
CEREAL_REGISTER_TYPE(BeastEngine::Tutorial_Stage)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Tutorial_Stage)
CEREAL_CLASS_VERSION(BeastEngine::Tutorial_Stage, 1)