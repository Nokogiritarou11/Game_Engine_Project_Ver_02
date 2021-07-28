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
#include "Box_Collider.h"

namespace BeastEngine
{
	class MonoBehaviour : public BeastEngine::Behaviour
	{
	private:

		virtual void Awake() {};      //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(Enable�Ɉˑ����A�Q�[���I�u�W�F�N�g��Active�𖳎�����)
		virtual void Start() {};      //���߂ăA�N�e�B�u�ɂȂ������Ɉ�񂾂��Ă΂��(�Q�[���I�u�W�F�N�g��Active�Ǝ��g��Enable�Ɉˑ�����)
		virtual void Update() {};     //�A�N�e�B�u���ɖ��t���[���Ă΂��
		virtual void LateUpdate() {}; //�A�N�e�B�u���ɖ��t���[���AUpdate�̂��ƂɌĂ΂��

		virtual void OnEnable() {};  //�A�N�e�B�u�ɂȂ������ɌĂ΂��
		virtual void OnDisable() {}; //��A�N�e�B�u�ɂȂ������ɌĂ΂��
		virtual void OnDestroy() {}; //�폜���ɌĂ΂��

		virtual void OnTrigger_Enter(BeastEngine::Collision& collision) {};	  //���̃I�u�W�F�N�g�ɃA�^�b�`���ꂽ�R���C�_�[�����R���C�_�[�ɐڐG�����Ƃ�
		virtual void OnCollision_Enter(BeastEngine::Collision& collision) {}; //��g���K�[��

		virtual void OnTrigger_Stay(BeastEngine::Collision& collision) {};	  //�ڐG���Ƀt���[���ȏ㑱�����Ƃ�(�ڐG���ێ��������薈�t���[��)
		virtual void OnCollision_Stay(BeastEngine::Collision& collision) {};  //��g���K�[��

		virtual void OnTrigger_Exit(BeastEngine::Collision& collision) {};	  //�ڐG���Ă����R���C�_�[�Ɨ��ꂽ�Ƃ�
		virtual void OnCollision_Exit(BeastEngine::Collision& collision) {};  //��g���K�[��

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		void Set_Active(bool value) override;
		bool Can_Multiple() override { return true; };

		bool is_called_Awake = false;
		bool is_called_Start = false;
		bool is_called_Update = false;
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