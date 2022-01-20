#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Scene_Change_Manager final : public MonoBehaviour
	{
	public:
		void Change_Scene(const std::string& scene_name);

	private:
		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::weak_ptr<Sprite_Renderer> fade_sprite;
		std::weak_ptr<GameObject> loading_object;
		std::string next_scene;
		int fade_state = 0;
		float fade_timer = 0;
		float fade_time = 0;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), fade_time);
		}
	};
}

REGISTER_COMPONENT(Scene_Change_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Scene_Change_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Scene_Change_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Scene_Change_Manager, 1)