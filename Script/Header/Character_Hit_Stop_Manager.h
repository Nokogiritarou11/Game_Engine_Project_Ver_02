#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Character_Parameter;

	class Character_Hit_Stop_Manager final : public MonoBehaviour
	{
	public:
		void Start_Hit_Stop(float stop_time, const std::vector<std::weak_ptr<Particle>>& particles);
		void Start_Hit_Stop(float stop_time);

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Animator> animator;

		bool is_stop = false;
		float stop_timer = 0;
		std::vector<std::weak_ptr<Particle>> stop_particles{};

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Character_Hit_Stop_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Hit_Stop_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Hit_Stop_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Character_Hit_Stop_Manager, 1)