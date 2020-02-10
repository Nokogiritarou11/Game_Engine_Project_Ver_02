#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <stdio.h>
#include <memory>
#include "Behaviour.h"


class Camera : public Behavior
{
public:

	DirectX::XMFLOAT4 focus = { 0.0f, 0.0f, 0.0f, 0.0f };
	float FOV = 30.0f;
	float near_z = 0.1f;	// ï\é¶ç≈ãﬂñ Ç‹Ç≈ÇÃãóó£
	float far_z = 10000.0f;	// ï\é¶ç≈âìñ Ç‹Ç≈ÇÃãóó£

	DirectX::XMFLOAT4X4 V = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 P = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	Camera();

	void Initialize(std::shared_ptr<GameObject> obj);
	void Update();
	DirectX::XMFLOAT2 WorldToViewportPoint(DirectX::XMFLOAT3 pos);
private:

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
};