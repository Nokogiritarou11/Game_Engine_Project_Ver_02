#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>

class Break_Block : public MonoBehaviour
{
public:
	void OnEnable();
	void Update();

	bool Draw_ImGui();
	std::weak_ptr<GameObject> blocks[4];
	Vector3 add_vec[4] = { {0,0,0} };
	Vector3 rot_vec[4] = { {0,0,0} };
private:
	float Disable_Timer;
};