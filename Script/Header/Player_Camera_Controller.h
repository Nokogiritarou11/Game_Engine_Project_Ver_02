#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Player_Camera_Controller : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		float angle_limit_up;
		float angle_limit_down;
		float rotate_speed;
		float follow_speed;

		std::weak_ptr<Transform> player_transform;



		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::MonoBehaviour>(this), angle_limit_up, angle_limit_down, rotate_speed, follow_speed);
		}
	};
}

REGISTER_COMPONENT(Player_Camera_Controller)
CEREAL_REGISTER_TYPE(BeastEngine::Player_Camera_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Player_Camera_Controller)
CEREAL_CLASS_VERSION(BeastEngine::Player_Camera_Controller, 1)