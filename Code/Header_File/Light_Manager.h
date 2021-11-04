#pragma once
#include <vector>
#include <memory>

namespace BeastEngine
{
	class Light;

	//ライト管理クラス
	class Light_Manager
	{
	public:
		void Reset(); //リストのリセット
		void Add(const std::shared_ptr<Light>& light); //リストに追加する
		std::vector<std::weak_ptr<Light>> light_list{};
	private:

	};
}