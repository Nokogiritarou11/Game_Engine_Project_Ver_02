#pragma once
#include "MonoBehaviour.h"


class Balet : public MonoBehaviour
{
public:
	std::weak_ptr<GameObject> muzzle;

	int timer;
private:
	void Awake() override;
	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		//archive(cereal::base_class<MonoBehaviour>(this), a);
	}
};

CEREAL_REGISTER_TYPE(Balet)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Balet)
CEREAL_CLASS_VERSION(Balet, 1);