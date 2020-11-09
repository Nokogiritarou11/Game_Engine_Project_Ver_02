#pragma once
#include <stdio.h>
#include "Behaviour.h"
#include "Debug.h"
#include "Input_Manager.h"
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
#include "Include_ImGui.h"
#include "Time.h"

class MonoBehaviour : public Behaviour
{
private:

	virtual void Awake() {}; //初めてアクティブになった時に一回だけ呼ばれる(Enableに依存し、ゲームオブジェクトのActiveを無視する)
	virtual void Start() {}; //初めてアクティブになった時に一回だけ呼ばれる(ゲームオブジェクトのActiveと自身のEnableに依存する)
	virtual void Update() {}; //アクティブ時に毎フレーム呼ばれる
	virtual void LateUpdate() {}; //アクティブ時に毎フレーム、Updateのあとに呼ばれる

	virtual void OnEnable() {};//アクティブになった時に呼ばれる
	virtual void OnDisable() {};//非アクティブになった時に呼ばれる
	virtual void OnDestroy() {};//削除時に呼ばれる

	void Initialize(std::shared_ptr<GameObject> obj) override;
	void SetActive(bool value) override;

	bool IsCalled_Awake = false;
	bool IsCalled_Start = false;
	bool IsCalled_Update = false;
	bool Disable_flg = false;
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
CEREAL_REGISTER_TYPE(MonoBehaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Behaviour, MonoBehaviour)