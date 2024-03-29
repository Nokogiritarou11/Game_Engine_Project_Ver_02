#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include "Animator_Condition.h"

namespace BeastEngine
{
	class Animator_State_Machine;

	//ステート間をつなぐ遷移クラス
	class Animator_State_Transition
	{
	public:
		enum class Interruption_Source
		{
			None,
			Current_State,
			Next_State,
		};

		bool has_exit_time = true;
		bool exit_trigger = false;
		bool exit_called = false;
		float exit_time = 1.0f;
		float transition_duration = 0;
		float transition_offset = 0;
		Interruption_Source interruption_source = Interruption_Source::None;
		std::vector<std::shared_ptr<Condition>> conditions {};
		std::weak_ptr<Animator_State_Machine> next_state;
		std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>> parameters;

		void Initialize(const std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>>& p_parameters, const std::shared_ptr<Animator_State_Machine>& next); //初期化
		bool Check_Transition(); //自身の遷移条件が合致しているかを返すe
		void Activate();         //遷移有効家事に呼ぶ
		void Add_Condition(const std::string& key, Parameter_Type type, Condition_Mode mode, float threshold); //新規遷移条件を追加する
		void Remove_Condition(int index); //遷移条件を削除する

	private:
		bool can_transition = false; //遷移が有効か

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(has_exit_time, exit_time, transition_duration, transition_offset, interruption_source, conditions, next_state);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Animator_State_Transition, 1)