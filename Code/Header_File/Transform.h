#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include "Component.h"
#include "Property.h"

class Transform : public Component
{
public:
	Vector3 position = { 0, 0, 0 };
	Vector3 scale = { 1, 1, 1 };

	Property<Vector3> eulerAngles
	{ _eulerAngles,
	[this](Vector3 v) {Change_rot(v);},
	[this]() { if (static_cast<Vector3>(_rotation.eulerAngles) != _eulerAngles) { _eulerAngles = static_cast<Vector3>(_rotation.eulerAngles); } return _eulerAngles; } };

	Property<Quaternion> rotation
	{ _rotation,
	[this](Quaternion q) {Change_rot(q);},
	nullptr };

	Vector3 localPosition = { 0, 0, 0 };
	Vector3 localScale = { 0, 0, 0 };

	Property<Vector3> localEulerAngles
	{ _localEulerAngles,
	[this](Vector3 v) {Change_rot(v);},
	nullptr };

	Property<Quaternion> localRotation
	{ _localRotation,
	[this](Quaternion q) {Change_rot(q);},
	nullptr };

	Vector3 DefaultForward = { 0.0f, 0.0f, 1.0f };
	Vector3 DefaultRight   = { 1.0f, 0.0f, 0.0f };
	Vector3 DefaultUp      = { 0.0f, 1.0f, 0.0f };
	Vector3 forward        = { 0.0f, 0.0f, 1.0f };
	Vector3 right          = { 1.0f, 0.0f, 0.0f };
	Vector3 up             = { 1.0f, 0.0f, 0.0f };

	Matrix world              = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix scale_matrix       = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix rotation_matrix    = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	float Width  = 100;
	float Height = 100;

	std::weak_ptr<Transform> parent;

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

	void Update();

private:
	Quaternion _rotation         = { 0, 0, 0 ,0 };
	Quaternion _localRotation    = { 0, 0, 0 ,0 };
	Vector3    _eulerAngles      = { 0, 0, 0 };
	Vector3    _localEulerAngles = { 0, 0, 0 };

	bool isUpdate_rot = false;

	void Change_rot(Quaternion q);
	void Change_rot(Vector3 v);
};