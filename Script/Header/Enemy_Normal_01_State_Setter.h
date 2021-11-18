#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_State_Setter.h"

namespace BeastEngine
{
	class Character_Parameter;
	class Enemy_Manager;
	class Object_Pool;

	class Enemy_Normal_01_State_Setter final : public MonoBehaviour, public Interface_Character_State_Setter
	{
	public:
		void Set_State() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Transform> target_transform;
		std::weak_ptr<Character_Parameter> parameter;
		std::weak_ptr<Enemy_Manager> enemy_manager;
		std::weak_ptr<Object_Pool> pool;

		float attack_distance = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), attack_distance);
		}
	};
}

REGISTER_COMPONENT(Enemy_Normal_01_State_Setter)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Normal_01_State_Setter)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Normal_01_State_Setter)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Normal_01_State_Setter, 1)