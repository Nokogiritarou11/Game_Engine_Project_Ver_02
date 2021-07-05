#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Animation_Clip.h"
#include "Object.h"

namespace BeastEngine
{
	class Animator_State_Transition;

	class Animator_State_Machine
	{
	public:
		struct Animation_Target //�A�j���[�V�����Ώۃf�[�^
		{
			std::weak_ptr<BeastEngine::Transform> target_transform;
			BeastEngine::Animation_Clip::Animation& animation;
		};

		struct Clip_Data //Clip�ƃA�j���[�V�����Ώۂ�R�t����\����
		{
			std::shared_ptr<BeastEngine::Animation_Clip> clip;
			std::vector<Animation_Target> targets;
		};

		Clip_Data clip_data;
		std::vector<Animator_State_Transition> transitions;

		std::string path;

	private:

	};
}