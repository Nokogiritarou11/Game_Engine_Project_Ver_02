#include "Asset_Manager.h"
#include "Object.h"

using namespace std;
using namespace BeastEngine;

void Asset_Manager::Exit()
{
	cache_shader.clear();
	cache_compute_shader.clear();
	cache_mesh.clear();
	cache_shader.clear();
	cache_texture.clear();
	cache_material.clear();
	cache_clip.clear();
	cache_asset.clear();
}

void Asset_Manager::Registration_Asset(const shared_ptr<Object> obj)
{
	cache_asset[obj->Get_Instance_ID()] = obj;
}
void Asset_Manager::Erase_Asset(const std::string& id)
{
	const auto it = cache_asset.erase(id);
	assert(it != 0);
}

weak_ptr<Object> Asset_Manager::Get_Asset_From_ID(const string& id)
{
	weak_ptr<Object> obj;
	if (const auto it = cache_asset.find(id); it != cache_asset.end())
	{
		return it->second;
	}

	return obj;
}