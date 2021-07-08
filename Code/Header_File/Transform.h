#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include <unordered_map>
#include "GameObject.h"

namespace BeastEngine
{
	class Transform : public Component
	{
	public:

		Transform();
		Transform(BeastEngine::Vector3 _position, BeastEngine::Quaternion _rotation);
		Transform(BeastEngine::Vector3 _position, BeastEngine::Vector3 _euler);
		~Transform();

		//false�ɐݒ肳��Ă���ύX�����������ǂ���
		bool has_changed = false;

		//GetterSetter

		BeastEngine::Vector3	 Get_Position() const;											//Transform��Position(���W)��Ԃ�
		void					 Set_Position(BeastEngine::Vector3 V);							//Transform��Position(���W)��ݒ肷��
		void					 Set_Position(float f1, float f2, float f3);					//Transform��Position(���W)��ݒ肷��
		BeastEngine::Quaternion	 Get_Rotation() const;											//Transform��Rotation(�N�H�[�^�j�I��)��Ԃ�
		void					 Set_Rotation(BeastEngine::Quaternion Q);						//Transform��Rotation(�N�H�[�^�j�I��)��ݒ肷��
		void					 Set_Rotation(float f1, float f2, float f3, float f4);			//Transform��Rotation(�N�H�[�^�j�I��)��ݒ肷��
		BeastEngine::Vector3	 Get_Scale() const;												//Transform��Scale(�T�C�Y)��Ԃ�
		void					 Set_Scale(BeastEngine::Vector3 V);								//Transform��Scale(�T�C�Y)��ݒ肷��
		void					 Set_Scale(float f1, float f2, float f3);						//Transform��Scale(�T�C�Y)��ݒ肷��

		BeastEngine::Vector3	 Get_Local_Position() const;									//Transform��LocalPosition(���[�J�����W)��Ԃ�
		void					 Set_Local_Position(BeastEngine::Vector3 V);					//Transform��LocalPosition(���[�J�����W)��ݒ肷��
		void					 Set_Local_Position(float f1, float f2, float f3);				//Transform��LocalPosition(���[�J�����W)��ݒ肷��
		BeastEngine::Quaternion	 Get_Local_Rotation() const;									//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��Ԃ�
		void					 Set_Local_Rotation(BeastEngine::Quaternion Q);					//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��ݒ肷��
		void					 Set_Local_Rotation(float f1, float f2, float f3, float f4);	//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��ݒ肷��
		BeastEngine::Vector3	 Get_Local_Scale() const;										//Transform��LocalScale(���[�J���T�C�Y)��Ԃ�
		void					 Set_Local_Scale(BeastEngine::Vector3 V);						//Transform��LocalScale(���[�J���T�C�Y)��ݒ肷��
		void					 Set_Local_Scale(float f1, float f2, float f3);					//Transform��LocalScale(���[�J���T�C�Y)��ݒ肷��

		BeastEngine::Vector3	 Get_Forward() const;											//Transform�̃��[���h��Ԃł�Z��(�O����)��Ԃ�
		BeastEngine::Vector3	 Get_Right() const;												//Transform�̃��[���h��Ԃł�X��(�E����)��Ԃ�
		BeastEngine::Vector3	 Get_Up() const;												//Transform�̃��[���h��Ԃł�Y��(�����)��Ԃ�

		BeastEngine::Vector3	 Get_Euler_Angles() const;										//Transform��Rotation(�N�H�[�^�j�I��)���I�C���[�p�ɕϊ����ĕԂ�
		void					 Set_Euler_Angles(BeastEngine::Vector3 V);						//Transform��Rotation(�N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
		void					 Set_Euler_Angles(float f1, float f2, float f3);				//Transform��Rotation(�N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
		BeastEngine::Vector3	 Get_Local_Euler_Angles() const;								//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)���I�C���[�p�ɕϊ����ĕԂ�
		void					 Set_Local_Euler_Angles(BeastEngine::Vector3 V);				//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
		void					 Set_Local_Euler_Angles(float f1, float f2, float f3);			//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��

		std::weak_ptr<Transform> Get_Parent() const;											//Transform�̐e��Ԃ�
		void					 Set_Parent(std::shared_ptr<Transform> P);						//Transform�̐e��ݒ肷��
		void					 Set_Parent(std::shared_ptr<Transform> P, int index_insert);	//Transform�̐e��ݒ肷��(���������Ԗڂ̎q�ɂȂ邩�w�肷��)

		std::weak_ptr<Transform> Get_Child(int index);											//Transform�̎w�肵���C���f�b�N�X�̎q��Ԃ�
		int						 Get_Child_Count();												//Transform���q�������ێ����Ă��邩��Ԃ�(�q�����Ȃ��ꍇ�A�O)
		std::weak_ptr<Transform> Find(std::string n);
		int						 Get_Sibling_Index() const;										//�q�G�����L�[��ł��̊K�w(�Z��֌W)�̒��ŉ��Ԗڂ���Ԃ�
		void					 Set_Sibling_Index(int index);									//�q�G�����L�[��ł��̊K�w(�Z��֌W)�̒��ł̏��Ԃ��C���f�b�N�X�Ŏw�肷��

		BeastEngine::Matrix		 Get_World_Matrix() const;										//Transform�̃��[���h�ϊ��s���Ԃ�
		void					 Set_World_Matrix(BeastEngine::Matrix matrix);					//Transform�̃��[���h�ϊ��s��𒼐ڐݒ肷��

		//Function

		BeastEngine::Quaternion	 Look_At(BeastEngine::Vector3 pos);								//Transform��Forward���w�肵�����W�֌�����

	private:

		BeastEngine::Vector3    position = { 0, 0, 0 };
		BeastEngine::Quaternion rotation = { 0, 0, 0 ,1 };
		BeastEngine::Vector3    scale = { 1, 1, 1 };

		BeastEngine::Vector3    local_position = { 0, 0, 0 };
		BeastEngine::Quaternion local_rotation = { 0, 0, 0 ,1 };
		BeastEngine::Vector3    local_scale = { 1, 1, 1 };

		BeastEngine::Vector3 forward = { 0.0f, 0.0f, 1.0f };
		BeastEngine::Vector3 right = { 1.0f, 0.0f, 0.0f };
		BeastEngine::Vector3 up = { 0.0f, 1.0f, 0.0f };

		BeastEngine::Matrix world_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		BeastEngine::Matrix scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		BeastEngine::Matrix local_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		BeastEngine::Matrix local_scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix local_rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix local_translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		std::weak_ptr<Transform> parent;
		std::vector<std::weak_ptr<Transform>> children;

		bool update_GUI = false;

		void On_Parent_Changed();
		void Change_Children();
		void Remove_Parent();

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		friend class Editor;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive);
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Transform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Transform)

template<class Archive>
void BeastEngine::Transform::serialize(Archive& archive)
{
	archive(cereal::base_class<BeastEngine::Component>(this),
		gameobject,
		position, rotation, scale,
		local_position, local_rotation, local_scale,
		forward, right, up,
		world_matrix, scale_matrix, rotation_matrix, translation_matrix,
		local_matrix, local_scale_matrix, local_rotation_matrix, local_translation_matrix,
		parent, children);
}