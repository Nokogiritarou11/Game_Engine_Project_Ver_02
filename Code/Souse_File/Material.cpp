#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include <clocale>
#include <unordered_map>
#include <tchar.h>
using namespace std;

unordered_map<wstring, shared_ptr<Material>> Material::mat_cache;

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
shared_ptr<Material> Material::Create(std::string Material_Name, WCHAR* Shader_Name, const wchar_t* filename, bool UI_Material)
{
	setlocale(LC_ALL, "japanese");
	wchar_t m_name[MAX_PATH] = { 0 };
	size_t ret = 0;
	mbstowcs_s(&ret, m_name, MAX_PATH, Material_Name.c_str(), _TRUNCATE);
	auto it = mat_cache.find(m_name);
	if (it != mat_cache.end())
	{
		return it->second;
	}
	else
	{
		shared_ptr<Material> mat = make_shared<Material>();
		mat->name = Material_Name;
		mat->shader = make_shared<Shader>();
		mat->shader->Create(Shader_Name, "VSMain", "PSMain", UI_Material);
		mat->texture = make_shared<Texture>();
		mat->texture->Load(filename);
		mat->UV_Size = { (float)mat->texture->GetWidth(),(float)mat->texture->GetHeight() };
		mat_cache.insert(make_pair(m_name, mat));
		return mat;
	}
}

Material::~Material()
{
}
