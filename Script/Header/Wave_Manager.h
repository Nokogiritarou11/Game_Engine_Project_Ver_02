#pragma once
#include "MonoBehaviour.h"
#include "Enemy_Manager.h"

namespace BeastEngine
{
	struct Instance_Enemy_Data
	{
		Vector3 position;
		Enemy_Type type = Enemy_Type::Enemy_Normal_01;

	private:
		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(position, type);
		}
	};

	struct Wave_Data
	{
		std::vector<Instance_Enemy_Data> enemy_data{};
		float instance_delay = 0;

	private:
		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(enemy_data, instance_delay);
		}
	};

	class Wave_Manager final : public MonoBehaviour
	{
	public:
		void Add_Callback_Start(const std::function<void()>& callback);
		void Add_Callback_End(const std::function<void()>& callback);

	private:
		void Awake() override;
		void Update() override;
		void OnTrigger_Enter(Collision& collision) override;
		bool Draw_ImGui() override;

		int wave_count = -1;
		float wave_timer = 0;
		bool wave_finished = false;
		std::vector<Wave_Data> wave_data{};
		std::vector<std::function<void()>> wave_start_callback{};
		std::vector<std::function<void()>> wave_end_callback{};
		std::weak_ptr<Enemy_Manager> enemy_manager;

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), wave_data);
		}
	};
}

REGISTER_COMPONENT(Wave_Manager)
CEREAL_REGISTER_TYPE(BeastEngine::Wave_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Wave_Manager)
CEREAL_CLASS_VERSION(BeastEngine::Instance_Enemy_Data, 1)
CEREAL_CLASS_VERSION(BeastEngine::Wave_Data, 1)
CEREAL_CLASS_VERSION(BeastEngine::Wave_Manager, 1)