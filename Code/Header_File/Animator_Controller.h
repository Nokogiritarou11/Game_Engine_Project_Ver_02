#pragma once
#include <vector>
#include <unordered_map>
#include "Object.h"
#include "Animator_Condition.h"
#include "Animator_State_Machine.h"

namespace BeastEngine
{
	class Transform;
	class Animator_State_Transition;

	class Animator_Controller : public Object
	{
	public:
		std::string name;
		std::vector<std::shared_ptr<BeastEngine::Animator_State_Machine>> state_machines;
		std::shared_ptr<std::unordered_map<std::string, BeastEngine::Animation_Parameter>> parameters;
		std::string save_path;

		void Initialize();
		void Update();
		bool Add_State_Machine(std::string& name);
		bool Remove_State_Machine(std::string& name);
		void Render_ImGui();
		void Add_Parameter(std::string& p_name, BeastEngine::Parameter_Type type);

		static std::shared_ptr<Animator_Controller> Load_Animator_Controller(std::string fullpath = "");
		static std::shared_ptr<Animator_Controller> Create_New_Controller();

		std::shared_ptr<BeastEngine::Animator_State_Machine> playing_state_machine;
		std::shared_ptr<BeastEngine::Animator_State_Machine> next_state_machine;
		std::shared_ptr<BeastEngine::Animator_State_Transition> active_transition;
		float duration_timer = 0;
		int interrupt_state = 0;

	private:
		int current_state_index = 0;
		int current_transition_index = 0;

		void Save();
		void Save_As();

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this), name, state_machines, parameters, save_path);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animator_Controller)
CEREAL_CLASS_VERSION(BeastEngine::Animator_Controller, 1)