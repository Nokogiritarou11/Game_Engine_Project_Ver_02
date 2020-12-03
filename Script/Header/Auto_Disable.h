#pragma once
#include "MonoBehaviour.h"

class Auto_Disable : public MonoBehaviour
{
public:

private:
	float Disable_Time;
	float timer;

	void OnEnable() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), Disable_Time);
	}
};

CEREAL_REGISTER_TYPE(Auto_Disable)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Auto_Disable)
CEREAL_CLASS_VERSION(Auto_Disable, 1);