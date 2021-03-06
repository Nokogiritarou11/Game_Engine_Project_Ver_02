#pragma once
#include "DxSystem.h"
#include "Behaviour.h"
#include "Texture.h"
#include "SkinMesh_Renderer.h"
#include "Material.h"
#include "Camera.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Original_Math.h"
#include <string>
#include <wrl.h>
#include <memory>
#include <vector>
#include <stdio.h>
#include <locale.h>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"
using Microsoft::WRL::ComPtr;

class Light : public Behaviour
{
public:

	Vector4 Direction = { 0,1,0,0 };
	Matrix V = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Matrix P = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Vector4 Color = { 0.5f ,0.5f ,0.5f ,1.0f };
	float Distance = 1000;
	float Intensity = 1;
	float Bias = 0.00001f;
	u_int Shadow_Map_Texture_Size = 2048;
	float Light_View_Size = 300;


private:
	void Initialize(std::shared_ptr<GameObject> obj) override;
	bool Draw_ImGui() override;
	bool CanMultiple() override { return false; };

	void Set(std::shared_ptr<Transform> trans);

	ComPtr<ID3D11DepthStencilView>		DepthStencilView = nullptr;
	ComPtr<ID3D11Texture2D>				DepthStencilTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView>	ShaderResourceView = nullptr;
	ComPtr<ID3D11SamplerState>		    sampler = nullptr;

	friend class View_Game;
	friend class View_Scene;
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t const version)
	{
		archive(cereal::base_class<Component>(this), Color, Distance, Intensity, Bias, Shadow_Map_Texture_Size, Light_View_Size);
	}
};

CEREAL_REGISTER_TYPE(Light)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Light)
CEREAL_CLASS_VERSION(Light, 1);