#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <wrl.h>
#include <memory>
#include <vector>
#include <stdio.h>
#include <locale.h>
#include "DxSystem.h"
#include "Component.h"
#include "Texture.h"
#include "Material.h"
#include "Camera.h"

class Renderer : public Component
{
public:
	Renderer();
	~Renderer();

	bool enabled = true;
	std::vector<std::shared_ptr<Material>> material;

	virtual void Render(std::shared_ptr<Camera> Render_Camera) {};

private:
	static int Set_BlendState;
	static int Set_RasterizerState;
	static int Set_DepthStencilState;
};