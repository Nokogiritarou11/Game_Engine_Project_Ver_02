#pragma once
#include <stdio.h>
#include "Behaviour.h"
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

namespace BeastEngine
{
	class MonoBehaviour : public BeastEngine::Behaviour
	{
	private:

		virtual void Awake() {}; //初めてアクティブになった時に一回だけ呼ばれる(Enableに依存し、ゲームオブジェクトのActiveを無視する)
		virtual void Start() {}; //初めてアクティブになった時に一回だけ呼ばれる(ゲームオブジェクトのActiveと自身のEnableに依存する)
		virtual void Update() {}; //アクティブ時に毎フレーム呼ばれる
		virtual void LateUpdate() {}; //アクティブ時に毎フレーム、Updateのあとに呼ばれる

		virtual void OnEnable() {};//アクティブになった時に呼ばれる
		virtual void OnDisable() {};//非アクティブになった時に呼ばれる
		virtual void OnDestroy() {};//削除時に呼ばれる

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		void Set_Active(bool value) override;
		bool Can_Multiple() override { return true; };

		bool is_called_Awake = false;
		bool is_called_Start = false;
		bool is_called_Update = false;
		bool is_disable = false;
		void Add();

		friend class Scene;
		friend class GameObject;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<Behaviour>(this));
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::MonoBehaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Behaviour, BeastEngine::MonoBehaviour)