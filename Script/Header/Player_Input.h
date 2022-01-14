#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_State_Setter.h"

namespace BeastEngine
{
	class Player_Parameter;
	class Player_Camera_Controller;
	class Enemy_Manager;
	class UI_Manager;

	class Player_Input final : public MonoBehaviour, public Interface_Character_State_Setter
	{
	public:
		void Set_State() override;

		Vector3 input_direction{};
		[[nodiscard]] float Get_Input_To_Target_Angle(Vector3 target_position) const;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Transform> camera_transform;
		std::weak_ptr<Player_Camera_Controller> camera_controller;
		std::weak_ptr<Player_Parameter> parameter;
		std::weak_ptr<Enemy_Manager> enemy_manager;
		std::weak_ptr<UI_Manager> ui_manager;

		bool is_stick_reset = false;
		bool can_guard_stick = false;
		bool can_guard_button = false;

		float guard_degree = 0;
		float guard_stick_time = 0;
		float guard_stick_timer = 0;
		float guard_button_time = 0;
		float guard_button_timer = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), guard_degree, guard_stick_time, guard_button_time);
		}
	};
}

REGISTER_COMPONENT(Player_Input)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Input)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Input)
CEREAL_CLASS_VERSION(BeastEngine::Player_Input, 1)