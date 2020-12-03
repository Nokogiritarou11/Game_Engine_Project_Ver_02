#pragma once
#include "MonoBehaviour.h"

class Object_Pool;

class Breath_Sphere : public MonoBehaviour
{
public:

private:
	std::weak_ptr<Object_Pool> obj_pool;

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

CEREAL_REGISTER_TYPE(Breath_Sphere)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Breath_Sphere)
CEREAL_CLASS_VERSION(Breath_Sphere, 1);