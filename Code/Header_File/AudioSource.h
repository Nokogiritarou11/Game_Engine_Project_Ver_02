#pragma once
#include "Audio.h"
#include "Behaviour.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace BeastEngine
{
	//オーディオの再生を担当するコンポーネントクラス
	class AudioSource final : public Behaviour
	{
	public:
		~AudioSource() override;

		void Set_Clip(const char* filepath, const char* filename);

		void Play() const;  //再生(一時停止中の場合は再開),(同一AudioSourceでの同時再生不可)
		void Pause() const; //一時停止
		void Stop() const;  //停止

		void Play_OneShot(float volume = 1.0f, float pitch = 0.0f) const; //再生(同時再生可能),(再生中制御不可)

		[[nodiscard]] bool Is_Playing() const; //再生中か

		void Set_Volume(float volume) const;                      //音量を設定する
		[[nodiscard]] float Get_Volume() const { return volume; } //音量を取得する

		void Set_Pitch(float pitch) const;                        //再生時のピッチを設定する
		[[nodiscard]] float Get_Pitch() const { return pitch; }   //再生時のピッチを取得する

		bool play_on_awake = true; //アクティブ化時に自動再生するか
		bool loop = false;         //ループ再生するか

	private:
		std::unique_ptr<DirectX::SoundEffectInstance> effect_instance{};
		std::string file_name{};
		std::string file_path{};

		float volume = 1.0f;
		float pitch = 0.0f;

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		void Set_Active(bool value) override;                             //アクティブ状態を切り替える
		bool Draw_ImGui() override;                                       //ImGui描画
		bool Can_Multiple() override { return true; };                    //同コンポーネントを複数アタッチ可能か

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Behaviour>(this), file_name, file_path, play_on_awake, loop, volume, pitch);
		}
	};
}

REGISTER_COMPONENT(AudioSource)
CEREAL_REGISTER_TYPE(BeastEngine::AudioSource)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::AudioSource)
CEREAL_CLASS_VERSION(BeastEngine::AudioSource, 1)