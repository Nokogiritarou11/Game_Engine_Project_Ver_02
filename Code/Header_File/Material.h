#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include "DxSystem.h"
#include "Shader.h"
#include "Texture.h"

class Material
{
public:
	std::string name;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;

	DirectX::XMFLOAT4 color     = { 1,1,1,1 };
	DirectX::XMFLOAT2 UV_Origin = { 0,0 };
	DirectX::XMFLOAT2 UV_Size   = { 0,0 };

	int BlendState        = DxSystem::BS_NONE;
	int RasterizerState   = DxSystem::RS_CULL_NONE;
	int DepthStencilState = DxSystem::DS_TRUE;

	Material();
	~Material();

	//static std::shared_ptr<Material> Create();
	//static std::shared_ptr<Material> Create(std::string Material_Name, WCHAR* Shader_Name);
	static std::shared_ptr<Material> Create(std::string Material_Name, WCHAR* Shader_Name, std::string filename);

private:

	static std::unordered_map<std::string, std::shared_ptr<Material>> mat_cache;
};