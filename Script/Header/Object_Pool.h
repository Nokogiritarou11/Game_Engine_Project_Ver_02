#pragma once
#include "MonoBehaviour.h"

class Object_Pool : public MonoBehaviour
{
public:
	std::shared_ptr<GameObject> Instance_Bullet();
	std::shared_ptr<GameObject> Instance_Bomb();
	std::shared_ptr<GameObject> Instance_Breath();
	std::shared_ptr<GameObject> Instance_Breath_Explosion();

private:
	std::vector<std::weak_ptr<GameObject>> Bullet_List;
	std::vector<std::weak_ptr<GameObject>> Bomb_List;
	std::vector<std::weak_ptr<GameObject>> Breath_List;
	std::vector<std::weak_ptr<GameObject>> Breath_Explosion_List;

	void Start() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}
};

CEREAL_REGISTER_TYPE(Object_Pool)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Object_Pool)
CEREAL_CLASS_VERSION(Object_Pool, 1);