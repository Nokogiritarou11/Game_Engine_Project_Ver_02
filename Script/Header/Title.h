#pragma once
#include "MonoBehaviour.h"

class Title : public MonoBehaviour
{
public:

private:
	std::weak_ptr<GameObject> UI_Title;
	std::weak_ptr<GameObject> UI_Start;
	std::weak_ptr<GameObject> UI_Tutorial;
	std::weak_ptr<GameObject> UI_Fade;

	int state;

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

CEREAL_REGISTER_TYPE(Title)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Title)
CEREAL_CLASS_VERSION(Title, 1);