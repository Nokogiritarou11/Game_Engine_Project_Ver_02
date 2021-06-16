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
		void Add(std::shared_ptr<BeastEngine::Light> light);
		std::vector<std::weak_ptr<BeastEngine::Light>> Light_list;
	private:

	};
}