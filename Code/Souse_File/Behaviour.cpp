#include "Behaviour.h"

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