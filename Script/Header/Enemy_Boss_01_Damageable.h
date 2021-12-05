#pragma once
#include "MonoBehaviour.h"
#include "Interface_Character_Damageable.h"

namespace BeastEngine
{
	class Enemy_Parameter;
	class Enemy_Manager;

	class Enemy_Boss_01_Damageable final : public MonoBehaviour, public Interface_Character_Damageable
	{
	public:
		bool Take_Damage(const std::shared_ptr<Damage_Collision>& damage_collision) override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Enemy_Parameter> parameter;
		std::weak_ptr<Enemy_Manager> enemy_manager;
		std::string critical_particle_key;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Interface_Character_Damageable>(this), critical_particle_key);
		}
	};
}

REGISTER_COMPONENT(Enemy_Boss_01_Damageable)
CEREAL_REGISTER_TYPE(BeastEngine::Enemy_Boss_01_Damageable)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Enemy_Boss_01_Damageable)
CEREAL_CLASS_VERSION(BeastEngine::Enemy_Boss_01_Damageable, 1)