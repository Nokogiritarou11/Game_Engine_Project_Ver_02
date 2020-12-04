#pragma once
#include "MonoBehaviour.h"

class Enemy_Collider;

class Dragon_HP : public MonoBehaviour
{
public:

	void Damage(int damage);
	int HP;
	std::vector<std::weak_ptr<Enemy_Collider>> colliders;

private:

	void Awake() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}
};

CEREAL_REGISTER_TYPE(Dragon_HP)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Dragon_HP)
CEREAL_CLASS_VERSION(Dragon_HP, 1);