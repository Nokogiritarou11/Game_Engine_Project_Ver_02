#pragma once
#include "MonoBehaviour.h"


class Enemy_Collider : public MonoBehaviour
{
public:
	float Damage_Magnification;
	float Size;

private:
	void Start() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), Damage_Magnification, Size);
	}
};

CEREAL_REGISTER_TYPE(Enemy_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Enemy_Collider)
CEREAL_CLASS_VERSION(Enemy_Collider, 1);