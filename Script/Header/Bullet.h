#pragma once
#include "MonoBehaviour.h"

class Bullet : public MonoBehaviour
{
public:
	std::weak_ptr<GameObject> muzzle;

private:
	float timer;
	float speed;

	void Awake() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this) ,speed);
	}
};

CEREAL_REGISTER_TYPE(Bullet)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Bullet)
CEREAL_CLASS_VERSION(Bullet, 1);