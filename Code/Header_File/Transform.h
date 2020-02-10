#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <memory>
#include "Component.h"

class Transform : public Component
{
public:
	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT4 rotation = { 0, 0, 0 ,0 };
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
	DirectX::XMFLOAT3 eulerAngles = { 0, 0, 0 };

	DirectX::XMFLOAT3 localPosition = { 0, 0, 0 };
	DirectX::XMFLOAT4 localRotation = { 0, 0, 0 ,0 };
	DirectX::XMFLOAT3 localScale = { 0, 0, 0 };
	DirectX::XMFLOAT3 localEulerAngles = { 0, 0, 0 };

	DirectX::XMFLOAT4 DefaultForward = { 0.0f, 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT4 DefaultRight = { 1.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 DefaultUp = { 0.0f, 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 forward = { 0.0f, 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT4 right = { 1.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 up = { 1.0f, 0.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT4X4 world = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	DirectX::XMFLOAT4X4 scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	DirectX::XMFLOAT4X4 rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	DirectX::XMFLOAT4X4 translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	float Width = 100;
	float Height = 100;

	std::weak_ptr<Transform> parent;

	// convert coordinate system from 'UP:+Z FRONT:+Y RIGHT-HAND' to 'UP:+Y FRONT:+Z LEFT-HAND'
	DirectX::XMFLOAT4X4 coordinate_conversion = {
	1, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 1
	};

	Transform();
	Transform(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT4 _rotation);
	Transform(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _euler);
	~Transform();

	void Initialize(std::shared_ptr<GameObject> obj);

	void Update();

private:

};