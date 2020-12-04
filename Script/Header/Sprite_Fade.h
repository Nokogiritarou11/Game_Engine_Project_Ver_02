#pragma once
#include "MonoBehaviour.h"

class Sprite_Fade : public MonoBehaviour
{
public:

private:
	std::weak_ptr<Sprite_Renderer> UI_Fade;
	float Fade_Time;
	float timer;
	bool FadeOut;

	void Start() override;
	void Update() override;
	bool Draw_ImGui() override;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<MonoBehaviour>(this), Fade_Time, FadeOut);
	}
};

CEREAL_REGISTER_TYPE(Sprite_Fade)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Sprite_Fade)
CEREAL_CLASS_VERSION(Sprite_Fade, 1);