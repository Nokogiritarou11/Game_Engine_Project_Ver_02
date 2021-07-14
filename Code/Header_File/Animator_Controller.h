#pragma once
#include <vector>
#include <unordered_map>
#include "Object.h"
#include "Animator_Condition.h"
#include "Humanoid_Avatar.h"
#include "Animator_State_Machine.h"

namespace BeastEngine
{
	class Transform;
	class Animator_State_Transition;

	class Animator_Controller : public Object
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

		std::string name;
		std::unordered_map<std::string, Animation_Target> animation_data;
		std::vector<std::shared_ptr<BeastEngine::Animator_State_Machine>> state_machines;
		std::shared_ptr<std::unordered_map<std::string, BeastEngine::Controller_Parameter>> parameters;
		std::string save_path;

		Animator_Controller(){};
		~Animator_Controller() {};

		void Initialize(std::shared_ptr<BeastEngine::Transform>& root);
		void Update();
		bool Add_State_Machine(std::string& name);
		bool Remove_State_Machine(std::string& name);
		void Render_ImGui();
		void Add_Parameter(std::string& p_name, BeastEngine::Condition_Type type);

		static std::shared_ptr<Animator_Controller> Load_Animator_Controller(std::string fullpath = "");
		static std::shared_ptr<Animator_Controller> Create_New_Controller();
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

		int current_state_index = 0;

		void Set_Animation_Target(std::shared_ptr<Animator_State_Machine>& state);

		void Save();
		void Save_As();

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<Object>(this), name, state_machines, parameters, save_path);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animator_Controller)