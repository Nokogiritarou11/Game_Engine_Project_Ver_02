#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Object_Pool;

	class Character_Particles final : public MonoBehaviour
	{
	private:
		struct Instance_Data
		{
			std::string key;
			Vector3 position;

		private:
			// シリアライズ関数
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(key, position);
			}
		};

		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Object_Pool> pool;
		std::vector<std::weak_ptr<GameObject>> particle_list{};
		std::vector<std::weak_ptr<GameObject>> collider_list{};
		std::vector<Instance_Data> instance_list{};

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), particle_list, collider_list, instance_list);
		}
	};
}

REGISTER_COMPONENT(Character_Particles)
CEREAL_REGISTER_TYPE(BeastEngine::Character_Particles)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Character_Particles)
CEREAL_CLASS_VERSION(BeastEngine::Character_Particles, 1)