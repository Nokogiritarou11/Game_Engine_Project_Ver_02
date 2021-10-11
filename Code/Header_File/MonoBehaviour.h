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
	class MonoBehaviour : public Behaviour
	{
	private:
		virtual void Awake() {}      //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(Enable�Ɉˑ����A�Q�[���I�u�W�F�N�g��Active�𖳎�����)
		virtual void Start() {}      //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(�Q�[���I�u�W�F�N�g��Active�Ǝ��g��Enable�Ɉˑ�����)
		virtual void Update() {}     //�A�N�e�B�u���ɖ��t���[���Ă΂��
		virtual void LateUpdate() {} //�A�N�e�B�u���ɖ��t���[���AUpdate�̂��ƂɌĂ΂��

		virtual void OnEnable() {}  //�A�N�e�B�u�ɂȂ������ɌĂ΂��
		virtual void OnDisable() {} //��A�N�e�B�u�ɂȂ������ɌĂ΂��
		virtual void OnDestroy() {} //�폜���ɌĂ΂��

		virtual void OnTrigger_Enter(Collision& collision) {}	  //���̃I�u�W�F�N�g�ɃA�^�b�`���ꂽ�R���C�_�[�����R���C�_�[�ɐڐG�����Ƃ�
		virtual void OnCollision_Enter(Collision& collision) {} //��g���K�[��

		virtual void OnTrigger_Stay(Collision& collision) {}	  //�ڐG���Ƀt���[���ȏ㑱�����Ƃ�(�ڐG���ێ��������薈�t���[��)
		virtual void OnCollision_Stay(Collision& collision) {}  //��g���K�[��

		virtual void OnTrigger_Exit(Collision& collision) {}	  //�ڐG���Ă����R���C�_�[�Ɨ��ꂽ�Ƃ�
		virtual void OnCollision_Exit(Collision& collision) {}  //��g���K�[��

		void Initialize(const std::shared_ptr<GameObject>& obj) override;
		void Set_Active(bool value) override;
		bool Can_Multiple() override { return true; };

		bool is_called_awake = false;
		bool is_called_start = false;
		bool is_called_update = false;
		void Add();

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