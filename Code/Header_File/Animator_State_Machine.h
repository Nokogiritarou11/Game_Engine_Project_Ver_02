#pragma once
#include "Animation_Clip.h"
#include "Animator_State_Transition.h"

namespace BeastEngine
{
	class Transform;

	//アニメーターコントローラー内で使用されるステートマシンクラス
	class Animator_State_Machine
	{
	public:
		std::string name;
		std::shared_ptr<Animation_Clip> clip;
		std::vector<std::shared_ptr<Animator_State_Transition>> transitions;
		std::vector<Animation_Event> animation_events;
		std::vector<State_Event> state_events;

		bool transition_trigger = false;   //遷移が有効か

		float animation_speed = 1;         //再生速度
		bool use_speed_multiplier = false; //再生速度にパラメーター乗数を使用するか
		std::string multiplier_hash;       //パラメーター乗数のハッシュキー
		int start_frame = 0;               //アニメーションクリップの再生開始フレーム
		int end_frame = -1;                //アニメーションクリップの再生終了フレーム
		bool  is_loop_animation = false;   //ループするか
		bool  is_default_state = false;    //初期化時に最初に選択されるステートか

		float current_seconds = 0.0f;      //再生時間
		bool  is_end_animation = false;    //アニメーション再生が終了しているか

		void Initialize(const std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>>& p_parameters); //初期化
		void Set_Clip(const std::string& full_path); //ファイルパスからクリップを読み込みセットする
		void Activate(float transition_offset = 0.0f); //遷移時などアクティブ化時に呼ぶ
		void Exit(); //ステートを抜ける際に呼ぶ
		void Update_Transition(); //遷移状態を更新する
		void Update_Time(); //再生時間を更新する
		void Add_Transition(std::shared_ptr<Animator_State_Machine>& next_state); //遷移を追加する
		void Add_Animation_Event(); //アニメーションイベントを追加する
		void Add_State_Event(); //ステートイベントを追加する
		void Remove_Transition(int index); //遷移を削除する
		[[nodiscard]] std::shared_ptr<Animator_State_Transition> Get_Active_Transition() const; //現在有効な遷移を取得する

	private:
		std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>> parameters;
		std::string path; //クリップのパス
		std::shared_ptr<Animator_State_Transition> active_transition;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(name, path, animation_speed, use_speed_multiplier, multiplier_hash, start_frame, end_frame, is_loop_animation, is_default_state, transitions, animation_events, state_events);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Animator_State_Machine, 1)