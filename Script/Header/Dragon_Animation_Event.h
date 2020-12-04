#pragma once
#include "MonoBehaviour.h"

class Object_Pool;
class Player;

class Dragon_Animation_Event : public MonoBehaviour
{
public:

private:
	std::weak_ptr<Object_Pool> obj_pool;
	std::weak_ptr<Animator> animator;
	std::weak_ptr<Player> player;
	std::weak_ptr<Particle> breath_round;
	std::weak_ptr<AudioSource> se_breath_round;
	std::weak_ptr<Particle> wave;

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

CEREAL_REGISTER_TYPE(Dragon_Animation_Event)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Dragon_Animation_Event)
CEREAL_CLASS_VERSION(Dragon_Animation_Event, 1);