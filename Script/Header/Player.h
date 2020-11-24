#pragma once
#include "MonoBehaviour.h"
#include"balet.h"

class Player : public MonoBehaviour
{
public:
	int a;
	float gravity ;
	float jumppower;
	bool jumpflug;
	float jumpspeed;
	weak_ptr<GameObject> stage;
private:
	void Awake() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this),a);
	}
};

CEREAL_REGISTER_TYPE(Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Player)
CEREAL_CLASS_VERSION(Player, 1);