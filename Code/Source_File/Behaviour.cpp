#include "Behaviour.h"
using namespace BeastEngine;

Behaviour::Behaviour()
{
	enabled_old = enabled;
}

void Behaviour::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Behaviour::Get_Enabled()
{
	return enabled;
}