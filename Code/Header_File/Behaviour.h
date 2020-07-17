#pragma once
#include "Component.h"

class Behavior : public Component
{
public:
	bool enabled = true;

	Behavior();
	~Behavior();

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Component>(this), enabled);
	}
};
CEREAL_REGISTER_TYPE(Behavior)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Behavior)