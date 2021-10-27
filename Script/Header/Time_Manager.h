#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Time_Manager final : public MonoBehaviour
	{
	public:
		void Start_Time_Slow(float delay_time, float slow_time, float slow_speed);

	private:
		void Update() override;
		bool Draw_ImGui() override;

		bool is_slow = false;
		bool active_slow = false;
		float delay_timer = 0;
		float slow_timer = 0;
		float slow_scale = 0;
		float inverse_time_scale = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Time_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Time_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Time_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Time_Manager, 1)