#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Interface_Character_Mover
	{
	public:
		virtual ~Interface_Character_Mover() = default;

		virtual void Move_Normal() = 0;
		virtual void Move_Attack() = 0;
		virtual void Move_Damage() = 0;
		virtual void Move_Guard() = 0;

		virtual void Move_Update() = 0;

		float move_speed = 0;
		float run_speed = 0;
		float turn_speed = 0;
		float y_axis_velocity = 0;

	private:
		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
				archive(run_speed, turn_speed);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Interface_Character_Mover)
CEREAL_CLASS_VERSION(BeastEngine::Interface_Character_Mover, 1)