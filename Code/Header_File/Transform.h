#pragma once
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include <unordered_map>
#include "GameObject.h"

namespace BeastEngine
{
	class Transform final : public Component
	{
	public:
		Transform();
		Transform(Vector3 position, Quaternion rotation);
		Transform(Vector3 position, Vector3 euler);

		//false�ɐݒ肳��Ă���ύX�����������ǂ���
		bool has_changed = false;

		//GetterSetter

		[[nodiscard]] Vector3 Get_Position() const;							//Transform��Position(���W)��Ԃ�
		void Set_Position(Vector3 v);							            //Transform��Position(���W)��ݒ肷��
		void Set_Position(float f1, float f2, float f3);					//Transform��Position(���W)��ݒ肷��
		[[nodiscard]] Quaternion Get_Rotation() const;						//Transform��Rotation(�N�H�[�^�j�I��)��Ԃ�
		void Set_Rotation(Quaternion q);						            //Transform��Rotation(�N�H�[�^�j�I��)��ݒ肷��
		void Set_Rotation(float f1, float f2, float f3, float f4);			//Transform��Rotation(�N�H�[�^�j�I��)��ݒ肷��
		[[nodiscard]] Vector3 Get_Scale() const;							//Transform��Scale(�T�C�Y)��Ԃ�
		void Set_Scale(Vector3 v);								            //Transform��Scale(�T�C�Y)��ݒ肷��
		void Set_Scale(float f1, float f2, float f3);						//Transform��Scale(�T�C�Y)��ݒ肷��

		[[nodiscard]] Vector3 Get_Local_Position() const;					//Transform��LocalPosition(���[�J�����W)��Ԃ�
		void Set_Local_Position(Vector3 v);					                //Transform��LocalPosition(���[�J�����W)��ݒ肷��
		void Set_Local_Position(float f1, float f2, float f3);				//Transform��LocalPosition(���[�J�����W)��ݒ肷��
		[[nodiscard]] Quaternion Get_Local_Rotation() const;				//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��Ԃ�
		void Set_Local_Rotation(Quaternion q);					            //Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��ݒ肷��
		void Set_Local_Rotation(float f1, float f2, float f3, float f4);	//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)��ݒ肷��
		[[nodiscard]] Vector3 Get_Local_Scale() const;						//Transform��LocalScale(���[�J���T�C�Y)��Ԃ�
		void Set_Local_Scale(Vector3 v);						            //Transform��LocalScale(���[�J���T�C�Y)��ݒ肷��
		void Set_Local_Scale(float f1, float f2, float f3);					//Transform��LocalScale(���[�J���T�C�Y)��ݒ肷��

		[[nodiscard]] Vector3 Get_Forward() const;							//Transform�̃��[���h��Ԃł�Z��(�O����)��Ԃ�
		[[nodiscard]] Vector3 Get_Right() const;							//Transform�̃��[���h��Ԃł�X��(�E����)��Ԃ�
		[[nodiscard]] Vector3 Get_Up() const;								//Transform�̃��[���h��Ԃł�Y��(�����)��Ԃ�

		[[nodiscard]] Vector3 Get_Euler_Angles() const;						//Transform��Rotation(�N�H�[�^�j�I��)���I�C���[�p�ɕϊ����ĕԂ�
		void Set_Euler_Angles(Vector3 v);						            //Transform��Rotation(�N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
		void Set_Euler_Angles(float f1, float f2, float f3);				//Transform��Rotation(�N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
		[[nodiscard]] Vector3 Get_Local_Euler_Angles() const;				//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)���I�C���[�p�ɕϊ����ĕԂ�
		void Set_Local_Euler_Angles(Vector3 v);				                //Transform��LocalRotation(���[�J���N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��
		void Set_Local_Euler_Angles(float f1, float f2, float f3);			//Transform��LocalRotation(���[�J���N�H�[�^�j�I��)�ɃI�C���[�p��ϊ����Đݒ肷��

		[[nodiscard]] std::weak_ptr<Transform> Get_Parent() const;			    //Transform�̐e��Ԃ�
		void Set_Parent(const std::shared_ptr<Transform>& p);					//Transform�̐e��ݒ肷��
		void Set_Parent(const std::shared_ptr<Transform>& p, int index_insert);	//Transform�̐e��ݒ肷��(���������Ԗڂ̎q�ɂȂ邩�w�肷��)

		std::weak_ptr<Transform> Get_Child(int index);						    //Transform�̎w�肵���C���f�b�N�X�̎q��Ԃ�
		int	Get_Child_Count() const;											//Transform���q�������ێ����Ă��邩��Ԃ�(�q�����Ȃ��ꍇ�A�O)

		[[nodiscard]] int Get_Sibling_Index() const;						    //�q�G�����L�[��ł��̊K�w(�Z��֌W)�̒��ŉ��Ԗڂ���Ԃ�
		void Set_Sibling_Index(int index);									    //�q�G�����L�[��ł��̊K�w(�Z��֌W)�̒��ł̏��Ԃ��C���f�b�N�X�Ŏw�肷��

		[[nodiscard]] Matrix Get_World_Matrix() const;						    //Transform�̃��[���h�ϊ��s���Ԃ�
		void Set_World_Matrix(Matrix matrix);					                //Transform�̃��[���h�ϊ��s��𒼐ڐݒ肷��

																			    //Function
		[[nodiscard]] std::weak_ptr<Transform> Find(std::string n) const;		//Transform�ȉ��̃I�u�W�F�N�g����������
		[[nodiscard]] Quaternion Look_At(Vector3 pos) const;					//Transform��Forward���w�肵�����W�֌�����

	private:
		Vector3    position = { 0, 0, 0 };
		Quaternion rotation = { 0, 0, 0 ,1 };
		Vector3    scale = { 1, 1, 1 };

		Vector3    local_position = { 0, 0, 0 };
		Quaternion local_rotation = { 0, 0, 0 ,1 };
		Vector3    local_scale = { 1, 1, 1 };

		Vector3 forward = { 0.0f, 0.0f, 1.0f };
		Vector3 right = { 1.0f, 0.0f, 0.0f };
		Vector3 up = { 0.0f, 1.0f, 0.0f };

		Matrix world_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		Matrix scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		Matrix local_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		Matrix local_scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix local_rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix local_translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		std::weak_ptr<Transform> parent;
		std::vector<std::weak_ptr<Transform>> children{};

		bool update_GUI = false;

		void On_Parent_Changed(); //�eTransform�̎p�����ύX���ꂽ�Ƃ��ɌĂ΂��
		void Change_Children(); //���g�̎p�����ύX���ꂽ�Ƃ��Ɏq�ɑ΂��ď������s��
		void Remove_Parent(); //�e�q�֌W����������

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //������
		bool Draw_ImGui() override; //ImGui�`��
		bool Can_Multiple() override { return false; } //���R���|�[�l���g�𕡐��A�^�b�`�ł��邩

		friend class Editor;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version);
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Transform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Transform)
CEREAL_CLASS_VERSION(BeastEngine::Transform, 1)

template<class Archive>
void BeastEngine::Transform::serialize(Archive& archive, std::uint32_t const version)
{
	archive(cereal::base_class<Component>(this),
		gameobject,
		position, rotation, scale,
		local_position, local_rotation, local_scale,
		forward, right, up,
		world_matrix, scale_matrix, rotation_matrix, translation_matrix,
		local_matrix, local_scale_matrix, local_rotation_matrix, local_translation_matrix,
		parent, children);
}