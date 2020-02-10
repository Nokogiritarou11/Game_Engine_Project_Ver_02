#pragma once
#include "Transform.h"

class Rect_Transform : public Transform
{
public:
	Rect_Transform();
	~Rect_Transform();

	float Width = 100;
	float Height = 100;

private:

};