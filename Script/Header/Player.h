#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
#include "ObjectPool.h"
using namespace DirectX;

class Player : public MonoBehaviour
{
public:

	void Awake();
	void Start();
	void Update();

	bool Draw_ImGui();

private:
	float applySpeed;

	float Move_Speed;
	float Max_Speed;
	float Scroll_Speed;
	float Add_Power;
	float Down_Power;
	float Dash_Power;

	float Horizontal;
	float Vertical;

	Vector3 moveForward;

	void Check_Player_Move();
	void Move();

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<MonoBehaviour>(this), applySpeed, Move_Speed, Max_Speed, Scroll_Speed, Add_Power, Down_Power, Dash_Power);
	}
};

CEREAL_REGISTER_TYPE(Player)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Player)