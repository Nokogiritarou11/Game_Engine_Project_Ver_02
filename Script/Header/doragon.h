#pragma once
#include "MonoBehaviour.h"

class Doragon : public MonoBehaviour
{
public:

private:


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