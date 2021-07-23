#pragma once
#include "Collider.h"
#include <vector>

namespace BeastEngine
{
	class Box_Collider;

	class Collider_Manager
	{
	public:
		void Add(std::shared_ptr<BeastEngine::Box_Collider>& collider);
		void Update();

	private:
		bool Judge(std::shared_ptr<BeastEngine::Box_Collider>& box_1, std::shared_ptr<BeastEngine::Box_Collider>& box_2);

		std::vector<std::weak_ptr<BeastEngine::Box_Collider>> box_list;
	};
}