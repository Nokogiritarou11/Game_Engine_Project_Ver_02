#pragma once
#include <vector>
#include <unordered_map>
#include "Object.h"
#include "Animator_Condition.h"
#include "Animator_State_Machine.h"

namespace BeastEngine
{
	class Transform;
	class Animator_State_Transition;

	class Animator_Controller final : public Object
	{
	public:
		std::vector<std::shared_ptr<Animator_State_Machine>> state_machines;
		std::shared_ptr<std::unordered_map<std::string, Animation_Parameter>> parameters;
		std::string save_path; //上書きセーブ用パス

		void Initialize();               //初期化
		void Reset();                    //起動時のデータにリセットする
		void Update(const float& speed); //更新
		void Render_ImGui();             //ImGui描画

		bool Add_State_Machine(const std::string& name);                    //新規ステートマシンを追加する
		bool Remove_State_Machine(const std::string& name);                 //ステートマシンを削除する
		void Add_Parameter(std::string& p_name, Parameter_Type type) const; //新規アニメーションパラメーターを追加する

		static std::shared_ptr<Animator_Controller> Load_Animator_Controller(const std::string& full_path = ""); //ファイルパスからアニメーターコントローラーを読み込む
		static std::shared_ptr<Animator_Controller> Create_New_Controller(); //新規アニメーションコントローラーを生成する

		std::shared_ptr<Animator_State_Machine> playing_state_machine; //再生中のステートマシン
		std::shared_ptr<Animator_State_Machine> next_state_machine; //遷移先のステートマシン
		std::shared_ptr<Animator_State_Transition> active_transition; //現在有効な遷移
		float duration_timer = 0; //遷移判定用タイマー
		int interrupt_state = 0;  //遷移割り込みの状態ステート

	private:
		int current_state_index = 0;	  //GUI上で選択されたステートマシンのインデックス
		int current_transition_index = 0; //GUI上で選択された遷移のインデックス

		void Save();    //ファイルダイアログからセーブする
		void Save_As(); //save_pathに上書きセーブする

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this), state_machines, parameters, save_path);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animator_Controller)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animator_Controller)
CEREAL_CLASS_VERSION(BeastEngine::Animator_Controller, 1)