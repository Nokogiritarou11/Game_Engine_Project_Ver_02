#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	struct Cut_Scene_State_Data
	{
		Vector3 start_position{};
		Vector3 start_rotation{};
		Vector3 end_position{};
		Vector3 end_rotation{};
		float change_time = 0;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				start_position, start_rotation,
				end_position, end_rotation,
				change_time);
		}
	};

	class Interface_Cut_Scene
	{
	public:
		virtual ~Interface_Cut_Scene() = default;
		virtual Vector3 Play_Cut_Scene() { return Vector3::Zero; };

		bool is_end_play = false;
		int cut_state = -1;
		float state_timer = 0;
		std::vector<Cut_Scene_State_Data> state_data{};

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(state_data);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Cut_Scene_State_Data, 1)
CEREAL_REGISTER_TYPE(BeastEngine::Interface_Cut_Scene)
CEREAL_CLASS_VERSION(BeastEngine::Interface_Cut_Scene, 1)