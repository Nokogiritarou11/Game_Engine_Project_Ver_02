#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Wall final : public MonoBehaviour
	{
	public:
		void Wall_Up();
		void Wall_Down();

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		Vector3 start_pos;
		Vector3 end_pos;
		int state = 0;
		float fade_time = 0;
		float fade_timer = 0;
		std::weak_ptr<Collider> collider;
		std::weak_ptr<Transform> wall_obj;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), fade_time, start_pos, end_pos);
		}
	};
}

REGISTER_COMPONENT(Wall)
CEREAL_REGISTER_TYPE(BeastEngine::Wall)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Wall)
CEREAL_CLASS_VERSION(BeastEngine::Wall, 1)