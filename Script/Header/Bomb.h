#pragma once
#include "MonoBehaviour.h"


class Bomb : public MonoBehaviour
{
public:
	weak_ptr<GameObject>muzzle;
	weak_ptr<Particle> shot;
	float power;
	int timer;
private:
	void Awake() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		//archive(cereal::base_class<MonoBehaviour>(this), a);
	}
};

CEREAL_REGISTER_TYPE(Bomb)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Bomb)
CEREAL_CLASS_VERSION(Bomb, 1);
