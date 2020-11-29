#pragma once
#include "MonoBehaviour.h"

class Bomb : public MonoBehaviour
{
public:
private:
	float speed;
	float power;
	float timer;

	void OnEnable() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), speed);
	}
};

CEREAL_REGISTER_TYPE(Bomb)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Bomb)
CEREAL_CLASS_VERSION(Bomb, 1);
