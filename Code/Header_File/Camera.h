#pragma once
#include "Behaviour.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>

class Camera : public Behaviour
{
public:

	float FOV     = 30.0f;
	float near_z  = 0.1f;	// 表示最近面までの距離
	float far_z   = 10000.0f;	// 表示最遠面までの距離

	Matrix V = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Matrix P = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	Vector2 WorldToViewportPoint(Vector3 pos);

private:

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Behaviour>(this), FOV, near_z, far_z);
	}
	void Initialize(std::shared_ptr<GameObject> obj) override;
	bool Draw_ImGui() override;
	bool CanMultiple() override { return false; };

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
};
CEREAL_REGISTER_TYPE(Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Behaviour, Camera)