#include "Component.h"
#include "Transform.h"
#include "GameObject.h"
using namespace std;
using namespace BeastEngine;

bool Component::Compare_Tag(string _tag)
{
	if (gameobject->tag == _tag)
	{
		return true;
	}
	return false;
}