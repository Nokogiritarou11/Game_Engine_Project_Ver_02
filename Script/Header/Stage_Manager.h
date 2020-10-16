#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
using namespace DirectX;

class Player;

class Stage_Manager : public MonoBehaviour
{
public:
	void Start();
	void Update();

	bool Draw_ImGui();

	float Scroll_Speed;
private:

	std::vector<std::weak_ptr<GameObject>> Bridge_Normal;
	int bridge_normal_count;
	Vector3 bridge_offset;

	std::vector<std::weak_ptr<GameObject>> Block_obj;
	int block_obj_count;

	std::weak_ptr<Player> player;

	float Scroll_Speed_Set;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<MonoBehaviour>(this), Scroll_Speed);
	}
};

CEREAL_REGISTER_TYPE(Stage_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Stage_Manager)
CEREAL_CLASS_VERSION(Stage_Manager, 1)