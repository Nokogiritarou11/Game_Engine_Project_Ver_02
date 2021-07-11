#pragma once
#include "Animation_Clip.h"
#include "Animator_State_Transition.h"

namespace BeastEngine
{
	class Transform;

	class Animator_State_Machine
	{
	public:
		std::string name;
		std::shared_ptr<BeastEngine::Animation_Clip> clip;
		std::shared_ptr<std::unordered_map<std::string, Controller_Parameter>> parameters;
		std::vector<std::shared_ptr<BeastEngine::Animator_State_Transition>> transitions;
		bool transition_trigger = false;

		float animation_speed = 1;   //çƒê∂ë¨ìx
		bool  loopAnimation = false; //ÉãÅ[ÉvÇ∑ÇÈÇ©
		float currentSeconds = 0.0f;
		bool  endAnimation = false;
		bool is_default_state = false;

		void Initialize(std::shared_ptr<std::unordered_map<std::string, Controller_Parameter>> parameter);
		void Set_Clip(std::string fullpath);
		void Set_Active(float transition_offset = 0.0f);
		void Exit();
		void Update_Transition();
		void Update_Time();
		void Add_Transition(std::shared_ptr<BeastEngine::Animator_State_Machine> next_state);
		void Remove_Transition(int index);
		std::shared_ptr<BeastEngine::Animator_State_Transition> Get_Active_Transition() { return active_transition; };

	private:
		std::string path;
		std::shared_ptr<BeastEngine::Animator_State_Transition> active_transition;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive);
	};
}