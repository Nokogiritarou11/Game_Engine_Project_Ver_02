#include "Component.h"
#include "Transform.h"
#include "GameObject.h"
using namespace std;

Component::Component()
{
}

Component::~Component()
{
}

bool Component::CompareTag(string _tag)
{
	if (tag == _tag)
	{
		return true;
	}
	return false;
}