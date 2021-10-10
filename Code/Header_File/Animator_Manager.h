#pragma once
#include <memory>
#include <list>

namespace BeastEngine
{
	class Animator;

	class Animator_Manager
	{
	public:
		void Reset();

		void Update();
		void Add(std::shared_ptr<Animator> animator);

	private:
		std::list<std::weak_ptr<Animator>> animator_list;
	};
}