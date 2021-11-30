#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Damage_Collision;
	class Object_Pool;
	class Character_Hit_Stop_Manager;

	enum class Damage_Type
	{
		Front,
		Back,
		Right,
		Left,
		Push,
		Knock_Down,
		Up,
		Down
	};

	class Interface_Character_Damageable
	{
	public:
		virtual ~Interface_Character_Damageable() = default;
		virtual bool Take_Damage(const std::shared_ptr<Damage_Collision>& damage_collision) = 0;

		std::weak_ptr<Animator> animator;
		std::weak_ptr<Character_Hit_Stop_Manager> hit_stop_manager;
		std::weak_ptr<Object_Pool> pool;
		std::weak_ptr<Transform> hit_pos_transform;

	private:
		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(hit_pos_transform);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Interface_Character_Damageable)
CEREAL_CLASS_VERSION(BeastEngine::Interface_Character_Damageable, 1)