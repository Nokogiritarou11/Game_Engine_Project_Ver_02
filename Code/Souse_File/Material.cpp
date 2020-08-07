#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include <clocale>
#include <unordered_map>
#include <tchar.h>
#include "Include_ImGui.h"
using namespace std;

unordered_map<string, shared_ptr<Material>> Material::mat_cache;

shared_ptr<Material> Material::Create(std::string Material_Name, WCHAR* VS_Name, WCHAR* PS_Name, std::string filename)
{
	auto it = mat_cache.find(Material_Name);
	if (it != mat_cache.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<Material> mat = make_shared<Material>();
		mat->name = Material_Name;
		mat->shader = make_shared<Shader>();
		mat->shader->Create_VS(VS_Name, "VSMain");
		mat->shader->Create_PS(PS_Name, "PSMain");
		mat->texture = make_shared<Texture>();
		mat->texture->Load(filename);
		mat->UV_Size = { (float)mat->texture->GetWidth(),(float)mat->texture->GetHeight() };
		mat_cache.insert(make_pair(Material_Name, mat));
		return mat;
	}
}

void Material::Draw_ImGui()
{
	ImGui::Text(name.c_str());
	float out_color[4] = { color.x,color.y,color.z,color.w };
	ImGui::ColorEdit3("Color", out_color);
	color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };
}