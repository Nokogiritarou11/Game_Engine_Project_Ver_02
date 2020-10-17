#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
#include <array>
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
	const Vector3 bridge_offset = { 0,0,39.8f };

	std::vector<std::weak_ptr<GameObject>> Block_obj;
	int block_obj_count;

	std::vector<std::weak_ptr<GameObject>> Gas_obj;
	int gas_obj_count;

	std::vector<std::weak_ptr<GameObject>> Bonus_obj;
	int bonus_obj_count;

	std::weak_ptr<Player> player;

	float Scroll_Speed_Set;

	std::vector<std::weak_ptr<GameObject>> Instance_Objects;
	std::vector<std::array<float, 4>> Instance_Pattern;
	std::vector<std::array<float, 4>> Block_Pattern;
	std::vector<std::array<float, 4>> Gas_Pattern;
	std::vector<std::array<float, 4>> Bonus_Pattern;
	void Create_Objects(Vector3 Instance_Pos);
	std::shared_ptr<GameObject> Instance_Object(std::vector<std::weak_ptr<GameObject>>& objects);

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