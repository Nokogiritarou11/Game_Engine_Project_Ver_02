#pragma once
#include "Behaviour.h"

namespace BeastEngine
{
	class GameObject;
	class Animator_Controller;

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

		std::shared_ptr<BeastEngine::Animator_Controller> animator_controller;

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Update(); //更新

		std::string controller_path;

		int			currentAnimation = -1;
		float		currentSeconds = 0.0f;
		bool		endAnimation = false;
		bool        playing = false;

		friend class BeastEngine::Animator_Controller;
		friend class BeastEngine::Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this), controller_path, loopAnimation, animation_speed);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)