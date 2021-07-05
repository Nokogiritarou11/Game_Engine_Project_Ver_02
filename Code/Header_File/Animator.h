#pragma once
#include "Behaviour.h"

namespace BeastEngine
{
	class GameObject;
	class Animator_Controller;

	class Animator : public BeastEngine::Behaviour
	{
	public:

		bool  Is_PlayAnimation() const { return currentAnimation >= 0; } //�Đ�����
		int	  Get_PlayingAnimation() const { return currentAnimation; }
		float Get_PlayingSeconds() const { return currentSeconds; }
		void  Play(int animationIndex); //�Đ�
		void  Stop();                   //��~
		void  Pause();                  //�ꎞ��~

		void Add_Clip(std::string path);

		float animation_speed = 1;   //�Đ����x
		bool  loopAnimation = false; //���[�v���邩

		std::shared_ptr<BeastEngine::Animator_Controller> animator_controller;

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Update(); //�X�V

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