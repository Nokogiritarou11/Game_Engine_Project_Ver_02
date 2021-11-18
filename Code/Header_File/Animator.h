#pragma once
#include "Behaviour.h"
#include "Animator_Controller.h"

namespace BeastEngine
{
	class GameObject;
	class Transform;
	class Animator_Manager;

	//アニメーターコンポーネント
	class Animator final : public Behaviour
	{
	public:
		void Set_Int(const std::string& key, const int& value) const;     //Keyに対応したInt型パラメーターをvalueで書き換える
		void Set_Float(const std::string& key, const float& value) const; //Keyに対応したFloat型パラメーターをvalueで書き換える
		void Set_Bool(const std::string& key, const bool& value) const;   //Keyに対応したBool型パラメーターをvalueで書き換える
		void Set_Trigger(const std::string& key) const;                   //Keyに対応したTrigger型パラメーターをtrueにする

		[[nodiscard]] int Get_Int(const std::string& key) const;     //Keyに対応したInt型パラメーターを返す
		[[nodiscard]] float Get_Float(const std::string& key) const; //Keyに対応したFloat型パラメーターを返す
		[[nodiscard]] bool Get_Bool(const std::string& key) const;   //Keyに対応したBool型パラメーターを返す

		void Reset_Trigger(const std::string& key) const; //Keyに対応したTrigger型パラメーターをリセットする

		float speed = 1.0f;
		bool keep_state_on_disable = true; //非アクティブ化されたときにコントローラーをリセットするか
		std::shared_ptr<Animator_Controller> controller;

	private:
		//map登録用アニメーションデータ
		struct Animation_Target
		{
			std::weak_ptr<Transform> target;
			Vector3 position;
			Quaternion rotation;
			Vector3 scale;
		};

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		void Set_Active(bool value) override;
		bool Draw_ImGui() override; //ImGui描画
		bool Can_Multiple() override { return false; }; //同コンポーネントを複数アタッチ可能か

		void Set_Default_Pose(); //アニメーションブレンド時に必要な初期ポーズ状態を設定する
		void Update(); //更新
		void Activate();
		void Inactivate();

		bool is_playing = false;
		bool is_called = false;

		std::string controller_path; //アニメーションコントローラーのファイルパス

		std::unordered_map<std::string, Animation_Target> animation_data; //コントローラーに読み込まれた全クリップに存在するAnimationの一覧
		std::unordered_map<std::string, Animation_Target> pose_default;   //ボーンTransformの初期姿勢
		std::unordered_map<std::string, Animation_Target> pose_playing;   //再生されているクリップアニメーションのポーズ
		std::unordered_map<std::string, Animation_Target> pose_next;      //ブレンド用遷移先アニメーションのポーズ
		std::unordered_map<std::string, Animation_Target> pose_interrupt; //遷移割り込み時のブレンド用中断ポーズ

		std::unordered_map<std::string, Animation_Parameter> init_parameter; //リセット用のコピー

		friend class Animator_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Behaviour>(this), controller_path, keep_state_on_disable);
		}
	};
}

REGISTER_COMPONENT(Animator)
CEREAL_REGISTER_TYPE(BeastEngine::Animator)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::Animator)
CEREAL_CLASS_VERSION(BeastEngine::Animator, 1)