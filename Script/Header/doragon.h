#pragma once
#include "MonoBehaviour.h"
#include "BehaviorTree.h"
#include "BehaviorData.h"
#include "Node.h"
#include "Player.h"
class Player;
class BehaviorTree;
class BehaviorData;

class Doragon : public MonoBehaviour
{
public:
	int HP;
	bool is_Howling = false;
	bool is_Howl = false;

	int count_Bless = 0;
	int count_Stomp = 0;
	float length;
	std::weak_ptr<Animator> anime;
private:
	int state;
	float timer;
	Node* activeNode;
	std::shared_ptr<BehaviorTree> aiTree;
	std::shared_ptr<BehaviorData> aiData;

	void Awake() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}
};

CEREAL_REGISTER_TYPE(Doragon)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Doragon)
CEREAL_CLASS_VERSION(Doragon, 1);
