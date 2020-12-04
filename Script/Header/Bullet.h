#pragma once
#include "MonoBehaviour.h"

class Dragon_HP;
class Object_Pool;

class Bullet : public MonoBehaviour
{
public:
private:
	std::weak_ptr<Dragon_HP> dragon_hp;
	std::weak_ptr<Object_Pool> obj_pool;

	float speed;

	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), speed);
	}
};

CEREAL_REGISTER_TYPE(Bullet)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Bullet)
CEREAL_CLASS_VERSION(Bullet, 1);