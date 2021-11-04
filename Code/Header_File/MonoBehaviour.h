#pragma once
#include <stdio.h>
#include "Behaviour.h"
#include "Transform.h"
#include "Debug.h"
#include "Input.h"
#include "Cursor.h"
#include "Screen.h"
#include "Audio_Manager.h"
#include "Scene_Manager.h"
#include "Animator.h"
#include "AudioSource.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Mathf.h"
#include "Particle.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Resources.h"
#include "Include_ImGui.h"
#include "Time_Engine.h"
#include "Box_Collider.h"
#include "Sphere_Collider.h"
#include "Capsule_Collider.h"
#include "Cylinder_Collider.h"
#include "Physics.h"

namespace BeastEngine
{
	//ユーザーがコンポーネントを作成する際の基底クラス
	class MonoBehaviour : public Behaviour
	{
	private:
		virtual void Awake() {}      //初めてアクティブになった時に一回だけ呼ばれる(Enableに依存し、ゲームオブジェクトのActiveを無視する)
		virtual void Start() {}      //初めてアクティブになった時に一回だけ呼ばれる(ゲームオブジェクトのActiveと自身のEnableに依存する)
		virtual void Update() {}     //アクティブ時に毎フレーム呼ばれる
		virtual void LateUpdate() {} //アクティブ時に毎フレーム、Updateのあとに呼ばれる

		virtual void OnEnable() {}   //アクティブになった時に呼ばれる
		virtual void OnDisable() {}  //非アクティブになった時に呼ばれる
		virtual void OnDestroy() {}  //削除時に呼ばれる

		virtual void OnTrigger_Enter(Collision& collision) {}	//このオブジェクトにアタッチされたコライダーが他コライダーに接触したとき呼ばれる
		virtual void OnCollision_Enter(Collision& collision) {} //このオブジェクトにアタッチされたコライダー(非トリガー)が他コライダーに接触したとき呼ばれる

		virtual void OnTrigger_Stay(Collision& collision) {}	//接触が2フレーム以上続いたとき(接触が維持される限り毎フレーム)呼ばれる
		virtual void OnCollision_Stay(Collision& collision) {}  //接触が2フレーム以上続いたとき(接触が維持される限り毎フレーム)呼ばれる(非トリガー時)

		virtual void OnTrigger_Exit(Collision& collision) {}	//接触していたコライダーと離れたとき呼ばれる
		virtual void OnCollision_Exit(Collision& collision) {}  //接触していたコライダー(非トリガー)と離れたとき呼ばれる

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		void Set_Active(bool value) override;                             //アクティブ状態を設定する
		bool Can_Multiple() override { return true; };                    //同コンポーネントを複数アタッチできるか

		bool is_called_awake = false;  //既にAwakeが呼ばれているか
		bool is_called_start = false;  //既にStartが呼ばれているか
		bool is_called_update = false; //既にUpdateが呼ばれているか
		void Add(); //シーンマネージャーに登録する

		friend class Scene;
		friend class Collider;
		friend class GameObject;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Behaviour>(this));
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::MonoBehaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::MonoBehaviour)
CEREAL_CLASS_VERSION(BeastEngine::MonoBehaviour, 1)