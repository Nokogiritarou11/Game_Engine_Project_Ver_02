#pragma once
#include "DxSystem.h"
#include "Component.h"
#include "Texture.h"
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

class View_Texture;

class Renderer : public Component
{
public:

	bool enabled = true;
	std::vector<std::shared_ptr<Material>> material;

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Component>(this), enabled);
	}

	static int Set_BlendState;
	static int Set_RasterizerState;
	static int Set_DepthStencilState;

	friend class View_Texture;
	virtual void Render(Matrix V, Matrix P) {};
	virtual void Render(Matrix V, Matrix P, bool Use_Material, std::shared_ptr<Shader> shader) {};
};

CEREAL_REGISTER_TYPE(Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Renderer)