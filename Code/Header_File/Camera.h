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
	float near_z  = 0.1f;	// 表示最近面までの距離
	float far_z   = 100000.0f;	// 表示最遠面までの距離

	Matrix V = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Matrix P = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	void Initialize(std::shared_ptr<GameObject> obj);
	void Update();
	Vector2 WorldToViewportPoint(Vector3 pos);
private:

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
};