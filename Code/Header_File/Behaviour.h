#pragma once
#include <stdio.h>
#include "Component.h"

class Behavior : public Component
{
public:
	bool enabled = true;

	Behavior();
	~Behavior();

private:
};