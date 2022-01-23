#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	enum class BGM_Name
	{
		None,
		Title,
		Normal_Battle,
		Boss_Battle,
	};

	class Sound_Manager final : public MonoBehaviour
	{
	public:
		void Play_BGM(BGM_Name name);

	private:
		enum class Fade_State { Stop, Playing, Fade_In, Fade_Out };

		void Awake() override;
		void Update() override;
		bool Draw_ImGui() override;

		std::vector<std::weak_ptr<AudioSource>> bgm_list{};
		std::weak_ptr<AudioSource> playing_source;
		BGM_Name next_bgm = BGM_Name::None;
		Fade_State fade_state = Fade_State::Stop;
		float fade_timer = 0;
		float fade_time = 0;
		float bgm_volume = 0.5f;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), fade_time, bgm_volume);
		}
	};
}

REGISTER_COMPONENT(Sound_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Sound_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Sound_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Sound_Manager, 1)