#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <stdio.h>
#include <memory>
#include "Behaviour.h"

class Camera : public Behavior
{
public:

	Vector4 focus = { 0.0f, 0.0f, 0.0f, 0.0f };
	float FOV     = 30.0f;
	float near_z  = 0.1f;	// �\���ŋߖʂ܂ł̋���
	float far_z   = 10000.0f;	// �\���ŉ��ʂ܂ł̋���

	Matrix V = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Matrix P = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	void Initialize(std::shared_ptr<GameObject> obj);
	void Update();
	Vector2 WorldToViewportPoint(Vector3 pos);
private:

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
};