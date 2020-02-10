#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include "Shader.h"
#include "Texture.h"

class Material
{
public:
	std::string name;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	DirectX::XMFLOAT2 UV_Origin = { 0,0 };
	DirectX::XMFLOAT2 UV_Size = { 0,0 };

	Material();
	~Material();

	//static std::shared_ptr<Material> Create();
	//static std::shared_ptr<Material> Create(std::string Material_Name, WCHAR* Shader_Name);
	static std::shared_ptr<Material> Create(std::string Material_Name, WCHAR* Shader_Name, const wchar_t* filename, bool UI_Material);

private:

	static std::unordered_map<std::wstring, std::shared_ptr<Material>> mat_cache;
};