#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Scene_Change_Manager;

	class Title final : public MonoBehaviour
	{
	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Sprite_Renderer> start_sprite;
		std::weak_ptr<Scene_Change_Manager> scene_change_manager;
		std::weak_ptr<AudioSource> push_se;
		float timer = 0;
		int state = 0;
		bool is_pushed = false;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this));
		}
	};
}

REGISTER_COMPONENT(Title)
CEREAL_REGISTER_TYPE(BeastEngine::Title)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Title)
CEREAL_CLASS_VERSION(BeastEngine::Title, 1)