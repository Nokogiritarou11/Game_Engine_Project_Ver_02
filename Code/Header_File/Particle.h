#pragma once
#include "Component.h"
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "DxSystem.h"
#include "Original_Math.h"
#include <memory>
#include <vector>
#include <locale.h>


namespace BeastEngine
{
	class Particle : public BeastEngine::Component
	{
	public:
		~Particle();

		void Set_Particle(const char* filepath, const char* filename);
		bool Get_Is_Playing() { return is_playing; }

		void Play();  //�Đ�(�ꎞ��~���̏ꍇ�͍ĊJ)
		void Pause(); //�ꎞ��~
		void Stop();  //��~

		bool play_on_awake = true; //�A�N�e�B�u���Ɏ����ōĐ����J�n���邩
		float play_speed = 1.0f;   //�Đ����x

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override;
		bool Draw_ImGui() override;
		void Set_Active(bool value) override;
		bool Can_Multiple() override { return false; };

		bool is_called = false;
		bool is_playing = false;

		Effekseer::EffectRef effect = nullptr;
		Effekseer::Handle handle = -1;

		std::string file_name;
		std::string file_path;

		friend class Particle_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), play_on_awake, play_speed, file_name, file_path);
		}
	};
}

REGISTER_COMPONENT(Particle)
CEREAL_REGISTER_TYPE(BeastEngine::Particle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Particle)
CEREAL_CLASS_VERSION(BeastEngine::Particle, 1)