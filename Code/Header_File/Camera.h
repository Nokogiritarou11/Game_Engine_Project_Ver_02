#pragma once
#include "Behaviour.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>

class Camera : public Behavior
{
public:

	Vector4 focus = { 0.0f, 0.0f, 0.0f, 0.0f };
	float FOV     = 30.0f;
	float near_z  = 0.1f;	// ï\é¶ç≈ãﬂñ Ç‹Ç≈ÇÃãóó£
	float far_z   = 100000.0f;	// ï\é¶ç≈âìñ Ç‹Ç≈ÇÃãóó£

	Matrix V = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	Matrix P = { 0.0f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	void Initialize();
	void Initialize(std::shared_ptr<GameObject> obj);
	void Update(u_int width, u_int height);
	bool Draw_ImGui();
	Vector2 WorldToViewportPoint(Vector3 pos);


private:

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Behavior>(this), focus, FOV, near_z, far_z);
	}

	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
};
CEREAL_REGISTER_TYPE(Camera)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Behavior, Camera)