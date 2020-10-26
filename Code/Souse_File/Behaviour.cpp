#include "Behaviour.h"

Behaviour::Behaviour()
{
}

Behaviour::~Behaviour()
{
}

void Behaviour::SetEnabled(bool value)
{
	enabled = value;
}

bool Behaviour::enableSelf()
{
	return enabled;
}