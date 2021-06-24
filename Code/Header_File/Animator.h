#pragma once
#include "Behaviour.h"

namespace BeastEngine
{
	class Mesh;
	class SkinMesh_Renderer;

	class Animator : public BeastEngine::Behaviour
	{
	public:

		bool Is_PlayAnimation() const { return currentAnimation >= 0; } //�Đ�����
		int  Get_PlayingAnimation() const { return currentAnimation; }
		float  Get_PlayingSeconds() const { return currentSeconds; }
		void Play(int animationIndex);                                 //�Đ�
		void Stop();                                                   //��~
		void Pause();                                                  //�ꎞ��~
		void Update();                                                 //�X�V

		float       animation_speed = 1;                               //�Đ����x
		bool		loopAnimation = false;                             //���[�v���邩

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		void Set_Skin_Renderer(std::shared_ptr<BeastEngine::SkinMesh_Renderer> render);
		void Set_Mesh(std::shared_ptr<BeastEngine::Mesh> mesh);

		std::weak_ptr<BeastEngine::SkinMesh_Renderer> skin_renderer;
		std::shared_ptr<BeastEngine::Mesh> mesh_data;

		int			currentAnimation = -1;
		float		currentSeconds = 0.0f;
		bool		endAnimation = false;
		bool        playing = false;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Behaviour>(this));
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)