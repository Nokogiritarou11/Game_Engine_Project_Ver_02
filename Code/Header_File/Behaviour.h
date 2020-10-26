#pragma once
#include "Component.h"

class Behaviour : public Component
{
public:

	void SetEnabled(bool value);
	bool enableSelf();

	Behaviour();
	~Behaviour();

private:
	bool enabled = true;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Component>(this), enabled);
	}
};
CEREAL_REGISTER_TYPE(Behaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Behaviour)