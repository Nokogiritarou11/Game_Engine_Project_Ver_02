#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
using namespace DirectX;

class Collider;

class Player : public MonoBehaviour
{
public:

	void Awake();
	void Start();
	void Update();

	bool Draw_ImGui();

	bool Boosting = false;
	bool Can_Boost = false;
	bool Damage = false;
	bool Get_Bonus = false;
	int Stage_State = 0;

	float Gas_Max;
	float Gas;
	float Boost_Charge_Timer;
	float Boost_Magnification;
	float Speed_Bonus_Magnification;

	std::vector<std::weak_ptr<Collider>> colliders;

private:
	float Move_Speed;
	float Boost_Time_Max;
	float Boost_Timer;
	float Boost_Charge_Time_Max;
	float Gas_Decrease;
	float Gas_Increase;
	float Gas_Damage;
	float Speed_Bonus_Magnification_Set;
	float Speed_Bonus_Time_Max;
	float Speed_Bonus_Timer;
	int	Speed_Bonus_Count;

	bool Invincible;
	bool skin_enabled = true;
	float Invincible_Timer;

	float Horizontal;
	//float Vertical;

	Vector3 moveForward;

	std::vector<std::weak_ptr<GameObject>> blocks;
	int blocks_count;

	void Check_Player_Move();
	void Check_Parameter();
	void Move();

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), Move_Speed, Boost_Magnification, Boost_Time_Max, Boost_Charge_Time_Max, Gas_Max, Gas_Decrease, Gas_Increase, Gas_Damage, Speed_Bonus_Magnification, Speed_Bonus_Time_Max);
	}
};

CEREAL_REGISTER_TYPE(Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Player)
CEREAL_CLASS_VERSION(Player, 1);