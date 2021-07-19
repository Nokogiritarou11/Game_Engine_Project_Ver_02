#pragma once
#include "Behaviour.h"
#include "Animator_Controller.h"

namespace BeastEngine
{
	class GameObject;
	class Transform;
	class Animator_Manager;

	class Animator : public BeastEngine::Behaviour
	{
	public:
		struct Animation_Target
		{
			BeastEngine::Humanoid_Rig humanoid_rig;
			std::weak_ptr<BeastEngine::Transform> target;
			BeastEngine::Vector3 position;
			BeastEngine::Quaternion rotation;
			BeastEngine::Vector3 scale;
		};

		void  Play();  //çƒê∂
		void  Stop();  //í‚é~
		void  Pause(); //àÍéûí‚é~

		void Set_Int(std::string key, int value);
		void Set_Float(std::string key, float value);
		void Set_Bool(std::string key, bool value);
		void Set_Trigger(std::string key);
		void Reset_Trigger(std::string key);

		std::shared_ptr<BeastEngine::Animator_Controller> controller;
		std::shared_ptr<BeastEngine::Avatar> avatar;

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Set_Avatar(std::shared_ptr<BeastEngine::Avatar> set_avatar);
		void Set_Default_Pose();
		void Update(); //çXêV

		bool playing = true;
		bool has_avatar = false;

		std::string controller_path;
		std::string avatar_path;

		std::unordered_map<BeastEngine::Humanoid_Rig, std::weak_ptr<BeastEngine::Transform>> humanoid_target;
		std::unordered_map<std::string, Animation_Target> animation_data;
		std::unordered_map<std::string, Animation_Target> pose_default;
		std::unordered_map<std::string, Animation_Target> pose_playing;
		std::unordered_map<std::string, Animation_Target> pose_next;
		std::unordered_map<std::string, Animation_Target> pose_interrupt;

		friend class BeastEngine::Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), controller_path, avatar_path);
		}
	};
}

REGISTER_COMPONENT(Animator)
CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)