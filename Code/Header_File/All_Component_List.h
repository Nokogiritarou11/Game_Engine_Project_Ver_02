#pragma once
#include <stdlib.h>
#include <memory>

namespace BeastEngine
{
	class GameObject;

	class All_Component_List
	{
	public:
		static void Add(std::shared_ptr<BeastEngine::GameObject> obj);
	};
}
