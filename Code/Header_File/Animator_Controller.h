#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Object.h"
#include "Animator_Condition.h"
#include "Humanoid_Avatar.h"

namespace BeastEngine
{
	class Transform;
	class Animator_State_Machine;
	class Animator_State_Transition;

	class Animator_Controller
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

		std::unordered_map<std::string, Animation_Target> animation_data;
		std::vector<std::shared_ptr<BeastEngine::Animator_State_Machine>> state_machines;
		std::shared_ptr<std::unordered_map<std::string, BeastEngine::Controller_Parameter>> parameters;

		void Initialize(std::shared_ptr<BeastEngine::Transform> root);
		void Update();
		bool Add_State_Machine(std::string name);

	private:
		std::unordered_map<std::string, Animation_Target> pose_default;
		std::unordered_map<std::string, Animation_Target> pose_playing;
		std::unordered_map<std::string, Animation_Target> pose_next;
		std::unordered_map<std::string, Animation_Target> pose_interrupt;
		std::weak_ptr<Transform> root_transform;
		std::shared_ptr<BeastEngine::Animator_State_Machine> playing_state_machine;
		std::shared_ptr<BeastEngine::Animator_State_Machine> next_state_machine;
		std::shared_ptr<BeastEngine::Animator_State_Transition> active_transition;
		float duration_timer;

		void Set_Animation_Target(std::shared_ptr<Animator_State_Machine> state);

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive);
	};
}