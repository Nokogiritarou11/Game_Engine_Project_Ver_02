#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Enemy_Manager;

	class Player_Camera_Controller final : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void LateUpdate() override;
		bool Draw_ImGui() override;

		void Update_Free_Look() const;
		void Update_Battle();
		void Update_Lock_On() const;

		bool is_target_right = false;

		float angle_limit_up = 0;
		float angle_limit_down = 0;
		float rotate_speed = 0;
		float follow_speed = 0;

		Vector3 default_position;
		Vector3 default_rotation;
		Vector3 battle_position;
		Vector3 battle_rotation;
		Vector3 lock_position;
		Vector3 lock_rotation;

		std::weak_ptr<Transform> player_transform;
		std::weak_ptr<Transform> camera_transform;
		std::weak_ptr<Transform> target_transform;

		std::weak_ptr<Camera> camera;
		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Enemy_Manager> enemy_manager;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), angle_limit_up, angle_limit_down, rotate_speed, follow_speed,
				default_position, default_rotation, battle_position, battle_rotation, lock_position, lock_rotation);
		}
	};
}

REGISTER_COMPONENT(Player_Camera_Controller)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Camera_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Camera_Controller)
CEREAL_CLASS_VERSION(BeastEngine::Player_Camera_Controller, 1)