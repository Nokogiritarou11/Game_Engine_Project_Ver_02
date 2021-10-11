#pragma once
#include <vector>
#include <memory>

namespace BeastEngine
{
	class Light;

	class Light_Manager
	{
	public:
		void Reset();
		void Add(const std::shared_ptr<Light>& light);
		std::vector<std::weak_ptr<Light>> light_list;
	private:

	};
}