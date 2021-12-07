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

	//�Փˎ��f�[�^�\����
	struct Collision
	{
		const std::shared_ptr<Collider> collider;
		const std::shared_ptr<GameObject> gameobject;
		const std::shared_ptr<Transform> transform;
		const std::vector<Vector3> contacts{};
	};

	//�R���C�_�[�R���|�[�l���g���N���X
	class Collider : public Component
	{
	public:
		void Set_Enabled(bool value);           //�\�����邩
		[[nodiscard]] bool Get_Enabled() const; //���݃A�N�e�B�u��
		void Set_IsTrigger(bool value);         //�R���C�_�[��Ghost(Trigger)�ɐݒ肷��
		void Set_Layer(int new_layer);
		std::shared_ptr<RigidBody> rigidbody;
		Vector3 center;                         //���S�I�t�Z�b�g

	protected:
		virtual void Create_Shape() {}; //�`����쐬����
		void Rescale_Shape() const;     //�`������T�C�Y����
		void Create_Collider();         //�R���C�_�[���쐬����
		void Reset_Collider();          //�R���C�_�[���폜����

		void Initialize_MonoBehaviour(); //OnCollision��OnTrigger���Ăяo��MonoBehaviour��o�^����

		void Draw_ImGui_Common(); //ImGui�`�掞���ʕ����`��
		bool Draw_ImGui_Header(const std::string& component_name, bool& open); //�C���X�y�N�^�ŕ\�����邽�߂̋��ʃw�b�_�[��`�悷��

		std::unique_ptr<btCollisionShape> shape;
		std::unique_ptr<GhostObject> ghost;
		bool is_trigger = false; //Ghost(Trigger)��

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override; //������
		void Set_Active(bool value) override;                             //�A�N�e�B�u��Ԃ�؂�ւ���
		bool Can_Multiple() override { return false; }                    //���R���|�[�l���g�𕡐��A�^�b�`�\��

		void Update_Transform();  //Transform�̎p����Bullet���֔��f����
		void Update_Simulation(); //Bullet�ł̉��Z���ʂ�Transform�֔��f����

		void Call_Hit(Collision& collision); //�Փˎ��ɌĂ΂��

		void Call_OnTrigger_Enter(Collision& collision);   //�ՓˊJ�n���Ɏ��g��Ghost(Trigger)�������ꍇ�Ă΂��
		void Call_OnTrigger_Stay(Collision& collision);    //�ڐG�p�����Ɏ��g��Ghost(Trigger)�������ꍇ�Ă΂��
		void Call_OnTrigger_Exit(Collision& collision);    //�ڐG�I�����Ɏ��g��Ghost(Trigger)�������ꍇ�Ă΂��

		void Call_OnCollision_Enter(Collision& collision); //�ՓˊJ�n���Ɏ��g��Ghost(Trigger)�łȂ��ꍇ�Ă΂��
		void Call_OnCollision_Stay(Collision& collision);  //�ڐG�p�����Ɏ��g��Ghost(Trigger)�łȂ��ꍇ�Ă΂��
		void Call_OnCollision_Exit(Collision& collision);  //�ڐG�I�����Ɏ��g��Ghost(Trigger)�łȂ��ꍇ�Ă΂��

		void Set_Debug_Draw(bool value) const; //�f�o�b�O�\��(���C���[�t���[��)�ɓo�^����

		std::vector<std::weak_ptr<MonoBehaviour>> send_list; //OnCollision��OnTrigger���Ăяo��MonoBehaviour
		std::unordered_map<std::string, std::weak_ptr<Collider>> hit_list; //�Փ˂��Ă���R���C�_�[�̃��X�g
		std::unordered_map<std::string, std::weak_ptr<Collider>> hit_list_old; //Exit��Stay�֐��Ăяo���p�L���b�V��

		bool is_called = false;    //Manager�ɓo�^����Ă��邩
		bool enabled = true;       //�A�N�e�B�u��
		bool enabled_old = false;  //�ύX���g���K�[�p�L���b�V��
		bool disabled = false;     //Manager�ɓo�^���ꂽ��ԂŔ�A�N�e�B�u�����ꂽ��
		bool disabled_old = false; //�ύX���g���K�[�p�L���b�V��
		bool debug_drawed = false; //�f�o�b�O�\���ɓo�^����Ă��邩

		Vector3 position_old;      //�ύX���g���K�[�p�L���b�V��
		Quaternion rotation_old;   //�ύX���g���K�[�p�L���b�V��

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