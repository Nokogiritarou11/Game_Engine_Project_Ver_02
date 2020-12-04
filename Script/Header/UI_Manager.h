#pragma once
#include "MonoBehaviour.h"

class Player;
class Dragon_HP;

class UI_Manager : public MonoBehaviour
{
public:

private:
	std::weak_ptr<Sprite_Renderer> UI_Cursor;
	std::weak_ptr<Sprite_Renderer> UI_HP_Player;
	std::weak_ptr<Sprite_Renderer> UI_HP_Dragon;

	std::weak_ptr<Player> player;
	std::weak_ptr<Dragon_HP> dragon;

	int player_hp_max;
	int dragon_hp_max;

	float player_UI_max;
	float dragon_UI_max;

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

CEREAL_REGISTER_TYPE(UI_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, UI_Manager)
CEREAL_CLASS_VERSION(UI_Manager, 1)