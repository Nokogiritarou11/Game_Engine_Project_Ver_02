#pragma once
#include "MonoBehaviour.h"

class UI_Manager : public MonoBehaviour
{
public:

private:
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

CEREAL_REGISTER_TYPE(UI_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, UI_Manager)
CEREAL_CLASS_VERSION(UI_Manager, 1)