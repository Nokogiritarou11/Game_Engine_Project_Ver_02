#pragma once
#include "Transform.h"

namespace BeastEngine
{
	class Rect_Transform : public BeastEngine::Transform
	{
	public:
		Rect_Transform();
		~Rect_Transform();

		float width = 100;
		float height = 100;

	private:
	};
}