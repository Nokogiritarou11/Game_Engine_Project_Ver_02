#pragma once
#include "Animation_Clip.h"
#include "Animator_State_Transition.h"

namespace BeastEngine
{
	class Animator_State_Transition;
	class Transform;

	class Animator_State_Machine
	{
	public:
		struct Animation_Target //アニメーション対象データ
		{
			BeastEngine::Humanoid_Rig humanoid_type = BeastEngine::Humanoid_Rig::None;
			std::weak_ptr<BeastEngine::Transform> target_transform;
			BeastEngine::Animation_Clip::Animation& animation;
		};

		struct Clip_Data //Clipとアニメーション対象を紐付ける構造体
		{
			std::shared_ptr<BeastEngine::Animation_Clip> clip;
			std::vector<Animation_Target> targets;
		};

		Clip_Data clip_data;
		std::vector<Animator_State_Transition> transitions;

		float animation_speed = 1;   //再生速度
		bool  loopAnimation = false; //ループするか
		float currentSeconds = 0.0f;
		bool  endAnimation = false;

		void Initialize(std::shared_ptr<BeastEngine::Transform> root);
		void Update(const std::unordered_map<std::string, BeastEngine::Controller_Parameter>& parameter_map);
		void Add_Transition(std::shared_ptr<BeastEngine::Animator_State_Machine> next_state);
		void Remove_Transition(int index);

	private:
		std::string path;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(path, animation_speed, loopAnimation, transitions);
		}
	};
}