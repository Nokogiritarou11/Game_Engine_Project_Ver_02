#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Player_Parameter;
	class Enemy_Manager;
	class Interface_Cut_Scene;

	class Player_Camera_Controller final : public MonoBehaviour
	{
	public:
		void Shake_Camera(const int& count, const float& power);
		void Play_Cut_Scene(const int& index);

	private:
		void Awake() override;
		void LateUpdate() override;
		bool Draw_ImGui() override;

		void Update_Free_Look();
		void Update_Battle();
		void Update_Lock_On() const;

		bool is_target_right = false;
		bool is_playing_cut_scene = false;

		float angle_limit_up = 0;
		float angle_limit_down = 0;
		float rotate_speed = 0;
		float follow_speed = 0;

		int shake_count = 0;
		float shake_power = 0;
		float shake_timer;

		Vector3 final_position;
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
		std::weak_ptr<Player_Parameter> parameter;
		std::weak_ptr<Enemy_Manager> enemy_manager;

		std::vector<std::weak_ptr<Interface_Cut_Scene>> cut_scene;
		std::weak_ptr<Interface_Cut_Scene> playing_cut_scene;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this),
				angle_limit_up, angle_limit_down,
				rotate_speed, follow_speed,
				default_position, default_rotation,
				battle_position, battle_rotation,
				lock_position, lock_rotation);
		}
	};
}

REGISTER_COMPONENT(Player_Camera_Controller)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Camera_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Camera_Controller)
CEREAL_CLASS_VERSION(BeastEngine::Player_Camera_Controller, 1)