#pragma once
#include "MonoBehaviour.h"

class Player;

class ToPlayer_Collider : public MonoBehaviour
{
public:
	float Size;

private:
	std::weak_ptr<Player> player;
	int Damage;
	bool Trigger;

	void OnEnable() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), Damage, Size);
	}
};

CEREAL_REGISTER_TYPE(ToPlayer_Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, ToPlayer_Collider)
CEREAL_CLASS_VERSION(ToPlayer_Collider, 1);