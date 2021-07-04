#include "Asset_Manager.h"
#include "Object.h"

using namespace std;
using namespace BeastEngine;

void Asset_Manager::Registration_Asset(shared_ptr<Object> obj)
{
	auto it = cache_asset.find(obj->Get_Instance_ID());
	if (it == cache_asset.end())
	{
		cache_asset.insert(make_pair(obj->Get_Instance_ID(), obj));
	}
}

shared_ptr<Object> Asset_Manager::Get_Asset_From_ID(string id)
{
	auto it = cache_asset.find(id);
	if (it != cache_asset.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
	return nullptr;
}