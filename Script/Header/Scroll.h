#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
using namespace DirectX;

class Stage_Manager;

class Scroll : public MonoBehaviour
{
public:
	void Start();
	void Update();

	bool Draw_ImGui();

private:
	std::weak_ptr<Stage_Manager> stage_manager;
	float scroll_speed;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}
};

CEREAL_REGISTER_TYPE(Scroll)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Scroll)
CEREAL_CLASS_VERSION(Scroll, 1)