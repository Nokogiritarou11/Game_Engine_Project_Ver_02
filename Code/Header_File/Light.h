#pragma once
#include "Behaviour.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

class Light : public Behaviour
{
public:
	Vector4 Color = { 0.5f ,0.5f ,0.5f ,1.0f };
	float Intensity = 1;

private:
	void Initialize(std::shared_ptr<GameObject> obj) override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<Component>(this), Color, Intensity);
	}
};

CEREAL_REGISTER_TYPE(Light)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Light)
CEREAL_CLASS_VERSION(Light, 1);