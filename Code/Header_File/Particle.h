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

		void Play();  //再生(一時停止中の場合は再開)
		void Pause(); //一時停止
		void Stop();  //停止

		bool Play_On_Awake = true; //アクティブ時に自動で再生を開始するか
		float Play_Speed = 1.0f;   //再生速度

	private:
		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		void SetActive(bool value) override;
		bool CanMultiple() override { return false; };

		bool IsCalled = false;
		bool Disable_flg = false;

		Effekseer::EffectRef effect = nullptr;
		Effekseer::Handle handle = -1;

		std::string file_name;
		std::string file_path;

		friend class Particle_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<Component>(this), Play_On_Awake, Play_Speed, file_name, file_path);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Particle)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Particle)