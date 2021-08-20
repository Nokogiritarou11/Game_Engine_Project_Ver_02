#include "Renderer.h"
using namespace BeastEngine;

void Renderer::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Renderer::Get_Enabled()
{
	return enabled;
}