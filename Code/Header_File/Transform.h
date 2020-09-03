#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include "GameObject.h"

class Transform : public Component
{
public:

	// convert coordinate system from 'UP:+Z FRONT:+Y RIGHT-HAND' to 'UP:+Y FRONT:+Z LEFT-HAND'
	Matrix coordinate_conversion = {
	1, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 1
	};

	Transform();
	Transform(Vector3 _position, Quaternion _rotation);
	Transform(Vector3 _position, Vector3 _euler);
	~Transform();

	void Initialize(std::shared_ptr<GameObject> obj);
	//void Update();
	bool Draw_ImGui();
	bool hasChanged = false;

	//GetterSetter
	Vector3             Get_position     () const;
	void                Set_position     (Vector3 V);
	void                Set_position     (float f1, float f2, float f3);
	Quaternion          Get_rotation     () const;
	void                Set_rotation     (Quaternion Q);
	void                Set_rotation     (float f1, float f2, float f3, float f4);
	Vector3             Get_scale        () const;
	void                Set_scale        (Vector3 V);
	void                Set_scale        (float f1, float f2, float f3);

	Vector3             Get_localPosition() const;
	void                Set_localPosition(Vector3 V);
	void                Set_localPosition(float f1, float f2, float f3);
	Quaternion          Get_localRotation() const;
	void                Set_localRotation(Quaternion Q);
	void                Set_localRotation(float f1, float f2, float f3, float f4);
	Vector3             Get_localScale   () const;
	void                Set_localScale   (Vector3 V);
	void                Set_localScale   (float f1, float f2, float f3);

	Vector3             Get_forward      () const;
	void                Set_forward      (Vector3 V);
	void                Set_forward      (float f1, float f2, float f3);
	Vector3             Get_right        () const;
	void                Set_right        (Vector3 V);
	void                Set_right        (float f1, float f2, float f3);
	Vector3             Get_up           () const;
	void                Set_up           (Vector3 V);
	void                Set_up           (float f1, float f2, float f3);

	Vector3             Get_eulerAngles  () const;
	void                Set_eulerAngles  (Vector3 V);
	void                Set_eulerAngles  (float f1, float f2, float f3);
	Vector3             Get_localEulerAngles() const;
	void                Set_localEulerAngles(Vector3 V);
	void                Set_localEulerAngles(float f1, float f2, float f3);

	std::weak_ptr<Transform> Get_parent       () const;
	void				Set_parent       (std::weak_ptr<Transform>   P);
	void				Set_parent       (std::shared_ptr<Transform> P);

	Matrix              Get_world_matrix() const;

	//Function
	Quaternion LookAt(Vector3 pos);


private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);

	Vector3    position         = { 0, 0, 0 };
	Quaternion rotation         = { 0, 0, 0 ,1 };
	Vector3    scale            = { 1, 1, 1 };

	Vector3    localPosition = { 0, 0, 0 };
	Quaternion localRotation = { 0, 0, 0 ,1 };
	Vector3    localScale    = { 1, 1, 1 };

	Vector3 forward = { 0.0f, 0.0f, 1.0f };
	Vector3 right   = { 1.0f, 0.0f, 0.0f };
	Vector3 up      = { 0.0f, 1.0f, 0.0f };

	Matrix world_matrix       = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Matrix scale_matrix       = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix rotation_matrix    = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	Matrix local_matrix            = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Matrix localScale_matrix       = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix localRotation_matrix    = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix localTranslation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	std::weak_ptr<Transform> parent;
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
		local_matrix, localScale_matrix, localRotation_matrix, localTranslation_matrix, parent);
}