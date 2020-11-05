#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include "GameObject.h"

class Transform : public Component
{
public:

	Transform();
	Transform(Vector3 _position, Quaternion _rotation);
	Transform(Vector3 _position, Vector3 _euler);
	~Transform();

	//false�ɐݒ肳��Ă���ύX�����������ǂ���
	bool hasChanged = false;

	//GetterSetter

	Vector3					 Get_position        () const;											//Transform��Position(���W)��Ԃ�
	void					 Set_position        (Vector3 V);										//Transform��Position(���W)��ݒ肷��
	void					 Set_position        (float f1, float f2, float f3);					//Transform��Position(���W)��ݒ肷��
	Quaternion				 Get_rotation        () const;											//Transform��Rotation(�N�H�[�^�j�I��)��Ԃ�
	void					 Set_rotation        (Quaternion Q);									//Transform��Rotation(�N�H�[�^�j�I��)��ݒ肷��
	void					 Set_rotation        (float f1, float f2, float f3, float f4);			//Transform��Rotation(�N�H�[�^�j�I��)��ݒ肷��
	Vector3					 Get_scale           () const;											//Transform��Scale(�T�C�Y)��Ԃ�
	void					 Set_scale           (Vector3 V);										//Transform��Scale(�T�C�Y)��ݒ肷��
	void					 Set_scale           (float f1, float f2, float f3);					//Transform��Scale(�T�C�Y)��ݒ肷��

	Vector3					 Get_localPosition   () const;											//Transform��LocalPosition(���[�J�����W)��Ԃ�
	void					 Set_localPosition   (Vector3 V);										//Transform��LocalPosition(���[�J�����W)��ݒ肷��
	void					 Set_localPosition   (float f1, float f2, float f3);					//Transform��LocalPosition(���[�J�����W)��ݒ肷��
	Quaternion				 Get_localRotation   () const;											//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��Ԃ�
	void					 Set_localRotation   (Quaternion Q);									//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��ݒ肷��
	void					 Set_localRotation   (float f1, float f2, float f3, float f4);			//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��ݒ肷��
	Vector3					 Get_localScale      () const;											//Transform��LocalScale(���[�J���T�C�Y)��Ԃ�
	void					 Set_localScale      (Vector3 V);										//Transform��LocalScale(���[�J���T�C�Y)��ݒ肷��
	void					 Set_localScale      (float f1, float f2, float f3);					//Transform��LocalScale(���[�J���T�C�Y)��ݒ肷��

	Vector3					 Get_forward         () const;											//Transform�̃��[���h��Ԃł�Z��(�O����)��Ԃ�
	Vector3					 Get_right           () const;											//Transform�̃��[���h��Ԃł�X��(�E����)��Ԃ�
	Vector3					 Get_up              () const;											//Transform�̃��[���h��Ԃł�Y��(�����)��Ԃ�

	Vector3					 Get_eulerAngles     () const;											//Transform��Rotation(�N�H�[�^�j�I��)���I�C���[�p�ɕϊ����ĕԂ�
	void					 Set_eulerAngles     (Vector3 V);										//Transform��Rotation(�N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
	void					 Set_eulerAngles     (float f1, float f2, float f3);					//Transform��Rotation(�N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
	Vector3					 Get_localEulerAngles() const;											//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)���I�C���[�p�ɕϊ����ĕԂ�
	void					 Set_localEulerAngles(Vector3 V);										//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
	void					 Set_localEulerAngles(float f1, float f2, float f3);					//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��

	std::weak_ptr<Transform> Get_parent          () const;											//Transform�̐e��Ԃ�
	void					 Set_parent          (std::shared_ptr<Transform> P);					//Transform�̐e��ݒ肷��
	void					 Set_parent          (std::shared_ptr<Transform> P, int index_insert);	//Transform�̐e��ݒ肷��(���������Ԗڂ̎q�ɂȂ邩�w�肷��)

	std::weak_ptr<Transform> GetChild            (int index);										//Transform�̎w�肵���C���f�b�N�X�̎q��Ԃ�
	int						 childCount          ();												//Transform���q�������ێ����Ă��邩��Ԃ�(�q�����Ȃ��ꍇ�A�O)

	int						 GetSiblingIndex     () const;											//�q�G�����L�[��ł��̊K�w(�Z��֌W)�̒��ŉ��Ԗڂ���Ԃ�
	void					 SetSiblingIndex     (int index);										//�q�G�����L�[��ł��̊K�w(�Z��֌W)�̒��ł̏��Ԃ��C���f�b�N�X�Ŏw�肷��

	Matrix					 Get_world_matrix    () const;											//Transform�̃��[���h�ϊ��s���Ԃ�

	//Function

	Quaternion				 LookAt              (Vector3 pos);										//Transform��Forward���w�肵�����W�֌�����

private:

	Vector3    position            = { 0, 0, 0 };
	Quaternion rotation            = { 0, 0, 0 ,1 };
	Vector3    scale               = { 1, 1, 1 };

	Vector3    localPosition       = { 0, 0, 0 };
	Quaternion localRotation       = { 0, 0, 0 ,1 };
	Vector3    localScale          = { 1, 1, 1 };

	Vector3 forward                = { 0.0f, 0.0f, 1.0f };
	Vector3 right                  = { 1.0f, 0.0f, 0.0f };
	Vector3 up                     = { 0.0f, 1.0f, 0.0f };

	Matrix world_matrix            = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Matrix scale_matrix            = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix rotation_matrix         = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix translation_matrix      = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	Matrix local_matrix            = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Matrix localScale_matrix       = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix localRotation_matrix    = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix localTranslation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	std::weak_ptr<Transform> parent;
	std::vector<std::weak_ptr<Transform>> children;

	void OnParentChanged();
	void Change_Children();
	void Remove_Parent();

	void Initialize(std::shared_ptr<GameObject> obj) override;
	bool Draw_ImGui() override;

	friend class Debug_UI;
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);
};
CEREAL_REGISTER_TYPE(Transform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component,Transform)

template<class Archive>
void Transform::serialize(Archive& archive)
{
	archive(cereal::base_class<Component>(this),
		position, rotation, scale,
		localPosition, localRotation, localScale,
		forward, right, up,
		world_matrix, scale_matrix, rotation_matrix, translation_matrix,
		local_matrix, localScale_matrix, localRotation_matrix, localTranslation_matrix,
		parent, children);
}