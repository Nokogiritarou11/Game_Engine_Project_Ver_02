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
		std::shared_ptr<Animation_Clip> clip;
		std::vector<std::shared_ptr<Animator_State_Transition>> transitions;
		std::vector<Animation_Event> animation_events;
		std::vector<State_Event> state_events;
		bool transition_trigger = false;

		float animation_speed = 1;   //çƒê∂ë¨ìx
		bool use_speed_multiplier = false;
		std::string multiplier_hash;
		int start_frame = 0;
		int end_frame = -1;
		bool  is_loop_animation = false; //ÉãÅ[ÉvÇ∑ÇÈÇ©
		bool  is_default_state = false;

		float current_seconds = 0.0f;
		bool  is_end_animation = false;

		void Initialize(const std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>>& p_parameters);
		void Set_Clip(const std::string& full_path);
		void Set_Active(float transition_offset = 0.0f);
		void Exit();
		void Update_Transition();
		void Update_Time();
		void Add_Transition(std::shared_ptr<Animator_State_Machine>& next_state);
		void Add_Animation_Event();
		void Add_State_Event();
		void Remove_Transition(int index);
		[[nodiscard]] std::shared_ptr<Animator_State_Transition> Get_Active_Transition() const;

	private:
		std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>> parameters;
		std::string path;
		std::shared_ptr<Animator_State_Transition> active_transition;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(name, path, animation_speed, use_speed_multiplier, multiplier_hash, start_frame, end_frame, is_loop_animation, is_default_state, transitions, animation_events, state_events);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Animator_State_Machine, 1)