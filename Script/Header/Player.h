#pragma once
#include "MonoBehaviour.h"

class Object_Pool;

class Player : public MonoBehaviour
{
public:

private:
	std::weak_ptr<Object_Pool> obj_pool;
	std::weak_ptr<Transform> muzzle_trans;
	float move_speed;
	float aim_speed;
	float gravity;
	float jump_power;
	float jump_speed;
	bool jump_flg;

	void Move();

	void Awake() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), move_speed, aim_speed, gravity, jump_power);
	}
};

CEREAL_REGISTER_TYPE(Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Player)
CEREAL_CLASS_VERSION(Player, 1);