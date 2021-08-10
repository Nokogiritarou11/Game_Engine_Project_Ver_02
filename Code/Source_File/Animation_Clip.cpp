#include "Animation_Clip.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "Engine.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

shared_ptr<Animation_Clip> Animation_Clip::Load_Clip(string fullpath)
{
	auto it = Engine::asset_manager->cache_clip.find(fullpath);
	if (it != Engine::asset_manager->cache_clip.end())
	{
		shared_ptr<Animation_Clip> clip = it->second;
		return clip;
	}
	else //ƒtƒ@ƒCƒ‹‚©‚ç“Ç‚İ‚İ
	{
		shared_ptr<Animation_Clip> clip;

		const string bin = fullpath;
		ifstream in_bin(bin, ios::binary);
		if (in_bin.is_open())
		{
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(clip);

			Engine::asset_manager->Registration_Asset(clip);
			Engine::asset_manager->cache_clip.insert(make_pair(fullpath, clip));

			return clip;
		}
	}

	return nullptr;
}