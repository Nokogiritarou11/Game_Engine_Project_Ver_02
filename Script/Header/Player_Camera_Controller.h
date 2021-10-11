#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;

	class Player_Camera_Controller final : public MonoBehaviour
	{
	public:

	private:
		void Awake() override;
		void LateUpdate() override;
		bool Draw_ImGui() override;

		float angle_limit_up = 0;
		float angle_limit_down = 0;
		float rotate_speed = 0;
		float follow_speed = 0;

		std::weak_ptr<Transform> player_transform;
		std::weak_ptr<Character_Parameter> parameter;



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