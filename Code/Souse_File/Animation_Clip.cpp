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

unordered_map<string, shared_ptr<Animation_Clip>> Animation_Clip::cache_clip;

shared_ptr<Animation_Clip> Animation_Clip::Load_Clip(string fullpath)
{
	auto it = cache_clip.find(fullpath);
	if (it != cache_clip.end())
	{
		shared_ptr<Animation_Clip> clip = it->second;
		return clip;
	}
	else //ƒtƒ@ƒCƒ‹‚©‚ç“Ç‚İ‚İ
	{
		shared_ptr<Animation_Clip> clip = make_shared<Animation_Clip>();

		const string bin = fullpath;
		ifstream in_bin(bin, ios::binary);
		if (in_bin.is_open())
		{
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(clip);

			Engine::asset_manager->Registration_Asset(clip);
			cache_clip.insert(make_pair(fullpath, clip));

			return clip;
		}
	}

	return nullptr;
}