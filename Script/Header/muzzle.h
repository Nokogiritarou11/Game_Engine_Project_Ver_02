#pragma once
#include "MonoBehaviour.h"

class Muzzle : public MonoBehaviour
{
public:
	std::weak_ptr<GameObject> shotpoint;
	std::weak_ptr<Particle> muzzleflash;

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

CEREAL_REGISTER_TYPE(Muzzle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Muzzle)
CEREAL_CLASS_VERSION(Muzzle, 1);
