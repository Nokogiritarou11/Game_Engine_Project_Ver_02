#pragma once
#include "Behaviour.h"
#include "Animation_Clip.h"

namespace BeastEngine
{
	class GameObject;
	class Transform;

	class Animator : public BeastEngine::Behaviour
	{
	public:

		bool  Is_PlayAnimation() const { return currentAnimation >= 0; } //再生中か
		int	  Get_PlayingAnimation() const { return currentAnimation; }
		float Get_PlayingSeconds() const { return currentSeconds; }
		void  Play(int animationIndex); //再生
		void  Stop();                   //停止
		void  Pause();                  //一時停止

		void Add_Clip(std::string path);

		float animation_speed = 1;   //再生速度
		bool  loopAnimation = false; //ループするか

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Update(); //更新

		struct Animation_Target
		{
			std::weak_ptr<BeastEngine::Transform> target_transform;
			BeastEngine::Animation_Clip::Animation& animation;
		};

		struct Clip_Data
		{
			std::shared_ptr<BeastEngine::Animation_Clip> clip;
			std::vector<Animation_Target> targets;
		};

		std::vector<Clip_Data> clips;
		std::vector<std::string>  pathes;

		int			currentAnimation = -1;
		float		currentSeconds = 0.0f;
		bool		endAnimation = false;
		bool        playing = false;

		friend class Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), pathes);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)