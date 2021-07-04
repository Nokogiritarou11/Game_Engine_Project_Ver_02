#pragma once
#include <unordered_map>
#include <memory>
#include <string>

namespace BeastEngine
{
	class Object;

	class Asset_Manager
	{
	public:
		void Registration_Asset(std::shared_ptr<BeastEngine::Object> obj);
		std::shared_ptr<BeastEngine::Object> Get_Asset_From_ID(std::string id);

		~Asset_Manager() { cache_asset.clear(); };
	private:

		std::unordered_map<std::string, std::shared_ptr<BeastEngine::Object>> cache_asset;
	};
}