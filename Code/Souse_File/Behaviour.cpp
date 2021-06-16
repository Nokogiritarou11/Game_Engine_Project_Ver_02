#include "Behaviour.h"
using namespace BeastEngine;

Behaviour::Behaviour()
{
	enabled_old = enabled;
}

void Behaviour::SetEnabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		SetActive(value);
	}
}

bool Behaviour::enableSelf()
{
	return enabled;
}