#pragma once
#include <memory>
#include <vector>

namespace BeastEngine
{
	class Animator;

	//アニメーター管理クラス
	class Animator_Manager
	{
	public:
		void Reset(); //リストのリセット

		void Update(); //更新
		void Add(std::shared_ptr<Animator> animator); //リストへ追加

	private:
		std::vector<std::weak_ptr<Animator>> animator_list;
	};
}
