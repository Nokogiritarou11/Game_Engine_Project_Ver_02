#pragma once
#include "Component.h"
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "DxSystem.h"
#include "Original_Math.h"
#include <memory>


namespace BeastEngine
{
	//Effekseerのエフェクトを読み込み表示するパーティクルコンポーネント
	class Particle final : public Component
	{
	public:
		~Particle() override;

		void Set_Particle(const char* filepath, const char* filename);
		[[nodiscard]] bool Get_Is_Playing() const { return is_playing; }

		void Play();  //再生(一時停止中の場合は再開)
		void Pause() const; //一時停止
		void Stop();  //停止

		bool play_on_awake = true; //アクティブ時に自動で再生を開始するか
		float play_speed = 1.0f;   //再生速度

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		bool Draw_ImGui() override; //ImGui描画
		void Set_Active(bool value) override; //アクティブ状態を設定する
		bool Can_Multiple() override { return false; }; //同コンポーネントを複数アタッチできるか

		bool is_called = false; //既にマネージャーに登録されているか
		bool is_playing = false; //再生中か

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