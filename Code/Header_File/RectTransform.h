#pragma once
#include "Transform.h"

namespace BeastEngine
{
	class Rect_Transform : public BeastEngine::Transform
	{
	public:
		Rect_Transform();
		~Rect_Transform();

		float Width = 100;
		float Height = 100;

	private:
	};
}