#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
using namespace DirectX;

class Collider : public MonoBehaviour
{
public:
	void Start();
	void Update();

	bool Draw_ImGui();

	float Size_X;
	float Size_Z;

	int obj_type = 0;

	enum Obj_Type
	{
		None,
		Block,
		Gas,
		Bonus
	};
private:

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<MonoBehaviour>(this));
	}
};

CEREAL_REGISTER_TYPE(Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Collider)
CEREAL_CLASS_VERSION(Collider, 1)