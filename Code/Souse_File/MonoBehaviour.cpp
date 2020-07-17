#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Scene_Manager.h"
using namespace std;

MonoBehaviour::MonoBehaviour()
{
}

MonoBehaviour::~MonoBehaviour()
{
}

void MonoBehaviour::Initialize()
{
	Scene_Manager::Get_Active_Scene()->Add(static_pointer_cast<MonoBehaviour>(shared_from_this()));
}
void MonoBehaviour::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform  = obj->transform;
	Scene_Manager::Get_Active_Scene()->Add(static_pointer_cast<MonoBehaviour>(shared_from_this()));
}