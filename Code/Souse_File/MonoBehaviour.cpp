#include "MonoBehaviour.h"
#include "MonoBehaviour_Manager.h"
#include "GameObject.h"
using namespace std;

MonoBehaviour::MonoBehaviour()
{
}

MonoBehaviour::~MonoBehaviour()
{
}

void MonoBehaviour::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	MonoBehaviour_Manager::Add(static_pointer_cast<MonoBehaviour>(shared_from_this()));
}