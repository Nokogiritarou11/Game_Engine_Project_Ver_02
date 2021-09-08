#pragma once

namespace BeastEngine
{
	class Character_Mover
	{
	public:
		float move_speed = 0;
		float run_speed = 0;
		float turn_speed = 0;

		virtual void Move_Normal() = 0;
		virtual void Move_Attack() = 0;
		virtual void Move_Dodge() = 0;
		virtual void Move_Damage() = 0;
		virtual void Move_Guard() = 0;

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

CEREAL_REGISTER_TYPE(BeastEngine::Character_Mover)
CEREAL_CLASS_VERSION(BeastEngine::Character_Mover, 1)