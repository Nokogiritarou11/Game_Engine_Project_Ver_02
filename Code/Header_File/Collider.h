#pragma once
#include <unordered_map>
#include "btBulletDynamicsCommon.h"
#include "Component.h"
#include "RigidBody.h"
#include "GhostObject.h"

namespace BeastEngine
{
	class MonoBehaviour;
	class BulletPhysics_Manager;
	class Editor;

	//衝突時データ構造体
	struct Collision
	{
		const std::shared_ptr<Collider> collider;
		const std::shared_ptr<GameObject> gameobject;
		const std::shared_ptr<Transform> transform;
		const std::vector<Vector3> contacts{};
	};

	//コライダーコンポーネント基底クラス
	class Collider : public Component
	{
	public:
		void Set_Enabled(bool value);           //表示するか
		[[nodiscard]] bool Get_Enabled() const; //現在アクティブか
		void Set_IsTrigger(bool value);         //コライダーをGhost(Trigger)に設定する
		void Set_Layer(int new_layer);
		std::shared_ptr<RigidBody> rigidbody;
		Vector3 center;                         //中心オフセット

	protected:
		virtual void Create_Shape() {}; //形状を作成する
		void Rescale_Shape() const;     //形状をリサイズする
		void Create_Collider();         //コライダーを作成する
		void Reset_Collider();          //コライダーを削除する

		void Initialize_MonoBehaviour(); //OnCollisionやOnTriggerを呼び出すMonoBehaviourを登録する

		void Draw_ImGui_Common(); //ImGui描画時共通部分描画
		bool Draw_ImGui_Header(const std::string& component_name, bool& open); //インスペクタで表示するための共通ヘッダーを描画する

		std::unique_ptr<btCollisionShape> shape;
		std::unique_ptr<GhostObject> ghost;
		bool is_trigger = false; //Ghost(Trigger)か

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		void Set_Active(bool value) override;                             //アクティブ状態を切り替える
		bool Can_Multiple() override { return false; }                    //同コンポーネントを複数アタッチ可能か

		void Update_Transform();  //Transformの姿勢をBullet内へ反映する
		void Update_Simulation(); //Bulletでの演算結果をTransformへ反映する

		void Call_Hit(Collision& collision); //衝突時に呼ばれる

		void Call_OnTrigger_Enter(Collision& collision);   //衝突開始時に自身がGhost(Trigger)だった場合呼ばれる
		void Call_OnTrigger_Stay(Collision& collision);    //接触継続時に自身がGhost(Trigger)だった場合呼ばれる
		void Call_OnTrigger_Exit(Collision& collision);    //接触終了時に自身がGhost(Trigger)だった場合呼ばれる

		void Call_OnCollision_Enter(Collision& collision); //衝突開始時に自身がGhost(Trigger)でない場合呼ばれる
		void Call_OnCollision_Stay(Collision& collision);  //接触継続時に自身がGhost(Trigger)でない場合呼ばれる
		void Call_OnCollision_Exit(Collision& collision);  //接触終了時に自身がGhost(Trigger)でない場合呼ばれる

		void Set_Debug_Draw(bool value) const; //デバッグ表示(ワイヤーフレーム)に登録する

		std::vector<std::weak_ptr<MonoBehaviour>> send_list; //OnCollisionやOnTriggerを呼び出すMonoBehaviour
		std::unordered_map<std::string, std::weak_ptr<Collider>> hit_list; //衝突しているコライダーのリスト
		std::unordered_map<std::string, std::weak_ptr<Collider>> hit_list_old; //Exit＆Stay関数呼び出し用キャッシュ

		bool is_called = false;    //Managerに登録されているか
		bool enabled = true;       //アクティブか
		bool enabled_old = false;  //変更時トリガー用キャッシュ
		bool disabled = false;     //Managerに登録された状態で非アクティブ化されたか
		bool disabled_old = false; //変更時トリガー用キャッシュ
		bool debug_drawed = false; //デバッグ表示に登録されているか

		Vector3 position_old;      //変更時トリガー用キャッシュ
		Quaternion rotation_old;   //変更時トリガー用キャッシュ

		friend class Editor;
		friend class BulletPhysics_Manager;
		friend class RigidBody;
		friend class GhostObject;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled, is_trigger, rigidbody, center);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Collider)
CEREAL_CLASS_VERSION(BeastEngine::Collider, 1)