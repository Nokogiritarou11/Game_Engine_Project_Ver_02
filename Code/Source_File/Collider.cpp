#include "Collider.h"
using namespace BeastEngine;
using namespace std;


void Collider::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Collider::Get_Enabled()
{
	return enabled;
}