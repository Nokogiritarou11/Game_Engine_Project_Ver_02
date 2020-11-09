#include "Component.h"
#include "Transform.h"
#include "GameObject.h"
using namespace std;

bool Component::CompareTag(string _tag)
{
	if (gameObject->tag == _tag)
	{
		return true;
	}
	return false;
}