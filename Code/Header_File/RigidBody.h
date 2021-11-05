#pragma once
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btTransform.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include <memory>
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Collider;

	//�����x���Z���̗͂̌v�Z���@
	enum class Force_Mode
	{
		Force,
		Impulse
	};

	//���W�b�h�{�f�B�R���|�[�l���g (Unity�Ƃ͈Ⴂ�A�R���C�_�[�ɓ���)
	class RigidBody
	{
	public:
		~RigidBody();

		void Set_Mass(float set_mass);        //���ʂ�ݒ肷��
		[[nodiscard]] float Get_Mass() const; //���ʂ��擾����

		void Set_Kinematic(bool value); //�L�l�}�e�B�b�N�R���C�_�[���ǂ�����ݒ肷��
		void Set_Dynamic(bool value);   //�_�C�i�~�b�N�R���C�_�[���ǂ�����ݒ肷��
		void Use_Gravity(bool value);   //�d�͂ɉe������邩�ݒ肷��

		void Set_Freeze_Position(bool x_axis, bool y_axis, bool z_axis); //�����Ƃɍ��W���Œ肷��
		void Set_Freeze_Rotation(bool x_axis, bool y_axis, bool z_axis); //�����Ƃɉ�]���Œ肷��

		void Set_Angular_Velocity(Vector3 velocity) const;  //�p���x��ݒ肷��
		[[nodiscard]] Vector3 Get_Angular_Velocity() const; //�p���x���擾����
		void Set_Velocity(Vector3 velocity) const;  //�����x��ݒ肷��
		[[nodiscard]] Vector3 Get_Velocity() const; //�����x���擾����

		void Add_Force(Vector3 force, Force_Mode mode = Force_Mode::Force) const;
		void Add_Force_AtPosition(Vector3 force, Vector3 position, Force_Mode mode = Force_Mode::Force) const;

	private:
		void Initialize(const std::shared_ptr<Collider>& col); //������
		void Create();       //���W�b�h�{�f�B���쐬���}�l�[�W���[�ɓo�^����
		void Resize() const; //���T�C�Y����
		void Remove();       //�}�l�[�W���[����폜����

		void Get_BtTransform(btTransform& t) const;       //Bullet���ł̎p�����擾����
		void Set_BtTransform(const btTransform& t) const; //Bullet���ł̎p����ݒ肷��
		void Set_Debug_Draw(bool value) const;            //�f�o�b�O�`��ɓo�^����
		bool Get_Debug_Drawed() const;                    //�f�o�b�O�`��ɓo�^���邩�擾����

		std::unique_ptr<btRigidBody> rigidbody;
		std::unique_ptr<btDefaultMotionState> motion_state;
		std::weak_ptr<Collider> collider;

		float mass = 1.0f;         //����
		float send_mass = 1.0f;    //Bullet�����Ŏg�p���Ă��鎿��
		bool is_dynamic = true;    //�_�C�i�~�b�N�R���C�_�[��
		bool is_kinematic = false; //�L�l�}�e�B�b�N�R���C�_�[��
		Vector3 gravity = { 0.0f, -9.8f, 0.0f };       //�d��
		Vector3 linear_factor = { 1.0f, 1.0f, 1.0f };  //�����x
		Vector3 angular_factor = { 1.0f, 1.0f, 1.0f }; //�p�����x

		friend class Collider;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(mass, is_dynamic, is_kinematic, gravity, linear_factor, angular_factor);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::RigidBody, 1)