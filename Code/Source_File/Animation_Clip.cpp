#include "Animation_Clip.h"
#include <sstream>
#include <fstream>
#include "Engine.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

shared_ptr<Animation_Clip> Animation_Clip::Load_Clip(const string& full_path)
{
	//アセットマネージャー内にキャッシュがあればそれを返す
	if (const auto itr = Engine::asset_manager->cache_clip.find(full_path); itr != Engine::asset_manager->cache_clip.end())
	{
		shared_ptr<Animation_Clip> clip = itr->second;
		return clip;
	}

	//ファイルから読み込み
	if (const ifstream in_bin(full_path, ios::binary); in_bin.is_open())
	{
		shared_ptr<Animation_Clip> clip;
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(clip);

		//アセットマネージャーへ登録
		Engine::asset_manager->Registration_Asset(clip);
		Engine::asset_manager->cache_clip.insert(make_pair(full_path, clip));

		return clip;
	}

	return nullptr;
}