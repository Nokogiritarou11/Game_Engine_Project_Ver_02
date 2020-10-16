#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
using namespace DirectX;

class Player;

class Block : public MonoBehaviour
{
public:
	void Start();
	void Update();

	bool Draw_ImGui();

	float Decision_Size;

private:
	std::weak_ptr<Transform> player_trans;
	std::weak_ptr<Player> player;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}
};

CEREAL_REGISTER_TYPE(Block)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Block)
CEREAL_CLASS_VERSION(Block, 1)