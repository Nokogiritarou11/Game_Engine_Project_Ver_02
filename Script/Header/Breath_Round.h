#pragma once
#include "MonoBehaviour.h"

class ToPlayer_Collider;

class Breath_Round : public MonoBehaviour
{
public:

private:
	std::weak_ptr<ToPlayer_Collider> col;

	void OnEnable() override;
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

CEREAL_REGISTER_TYPE(Breath_Round)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Breath_Round)
CEREAL_CLASS_VERSION(Breath_Round, 1);