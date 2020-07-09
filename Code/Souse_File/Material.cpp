#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include <clocale>
#include <unordered_map>
#include <tchar.h>
using namespace std;

unordered_map<string, shared_ptr<Material>> Material::mat_cache;

Material::Material()
{
}
/*
shared_ptr<Material> Material::Create()
{
}

shared_ptr<Material> Material::Create(std::string Material_Name, WCHAR* Shader_Name)
{
}
*/
shared_ptr<Material> Material::Create(std::string Material_Name, WCHAR* Shader_Name, std::string filename)
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
		mat->shader->Create(Shader_Name, "VSMain", "PSMain");
		mat->texture = make_shared<Texture>();
		mat->texture->Load(filename);
		mat->UV_Size = { (float)mat->texture->GetWidth(),(float)mat->texture->GetHeight() };
		mat_cache.insert(make_pair(Material_Name, mat));
		return mat;
	}
}

Material::~Material()
{
}