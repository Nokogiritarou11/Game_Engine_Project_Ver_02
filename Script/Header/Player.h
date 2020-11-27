#pragma once
#include "MonoBehaviour.h"

class Player : public MonoBehaviour
{
public:

private:
	void Awake() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), gravity, jump_power);
	}
};

CEREAL_REGISTER_TYPE(Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Player)
CEREAL_CLASS_VERSION(Player, 1);