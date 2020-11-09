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

	virtual void Awake() {}; //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(Enable�Ɉˑ����A�Q�[���I�u�W�F�N�g��Active�𖳎�����)
	virtual void Start() {}; //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(�Q�[���I�u�W�F�N�g��Active�Ǝ��g��Enable�Ɉˑ�����)
	virtual void Update() {}; //�A�N�e�B�u���ɖ��t���[���Ă΂��
	virtual void LateUpdate() {}; //�A�N�e�B�u���ɖ��t���[���AUpdate�̂��ƂɌĂ΂��

	virtual void OnEnable() {};//�A�N�e�B�u�ɂȂ������ɌĂ΂��
	virtual void OnDisable() {};//��A�N�e�B�u�ɂȂ������ɌĂ΂��
	virtual void OnDestroy() {};//�폜���ɌĂ΂��

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