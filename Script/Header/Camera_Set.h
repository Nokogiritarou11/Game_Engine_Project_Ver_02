#pragma once
#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Transform.h"
using namespace DirectX;

class Camera_Set : public MonoBehaviour
{
public:
	void Awake();
	void Start();
	void LateUpdate();
private:
	int Camera_State = 0;
	Vector3 move_pos = { 0,0,0 };
	std::weak_ptr<Transform> player_transform;

	void Surveillance(Vector3 target);
	void Fixed_relative_position(Vector3 target);
	void TPS(std::shared_ptr<Transform> parent);
	void FPS(std::shared_ptr<Transform> parent);
	void Chase(Vector3 target);
	void Free();
};