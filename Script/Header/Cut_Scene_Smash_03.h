#pragma once
#include "MonoBehaviour.h"
#include "Interface_Cut_Scene.h"

namespace BeastEngine
{
	class Player_Camera_Controller;

	class Cut_Scene_Smash_03 final : public MonoBehaviour, public Interface_Cut_Scene
	{
	public:
		Vector3 Play_Cut_Scene() override;

	private:
		void Awake() override;
		bool Draw_ImGui() override;

		float camera_init_time = 0;
		std::weak_ptr<Transform> root_transform;
		std::weak_ptr<Transform> camera_transform;
		std::weak_ptr<Transform> player_transform;
		std::weak_ptr<Player_Camera_Controller> camera_controller;

		// �V���A���C�Y�֐�
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), cereal::base_class<Interface_Cut_Scene>(this), camera_init_time);
		}
	};
}

REGISTER_COMPONENT(Cut_Scene_Smash_03)
CEREAL_REGISTER_TYPE(BeastEngine::Cut_Scene_Smash_03)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Cut_Scene_Smash_03)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Interface_Cut_Scene, BeastEngine::Cut_Scene_Smash_03)
CEREAL_CLASS_VERSION(BeastEngine::Cut_Scene_Smash_03, 1)