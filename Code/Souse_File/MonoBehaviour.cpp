#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Engine.h"
#include "Scene_Manager.h"
using namespace std;

void MonoBehaviour::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	IsCalled_Awake = false;
	IsCalled_Start = false;
	IsCalled_Update = false;
	Disable_flg = false;
	if (Engine::scene_manager->Run)
	{
		Add();
	}
}

void  MonoBehaviour::SetActive(bool value)
{
	if (value)
	{
		Add();
	}
	else
	{
		if (enableSelf())
		{
			OnDisable();
		}
	}
}

void  MonoBehaviour::Add()
{
	if (gameObject->activeInHierarchy())
	{
		if (!IsCalled_Awake)
		{
			Awake();
			IsCalled_Awake = true;
		}
		if (gameObject->activeInHierarchy())
		{
			if (enableSelf())
			{
				OnEnable();
				if (gameObject->activeInHierarchy())
				{
					if (enableSelf())
					{
						if (!IsCalled_Start)
						{
							Scene_Manager::Get_Active_Scene()->MonoBehaviour_Start_Next_list.emplace_back(static_pointer_cast<MonoBehaviour>(shared_from_this()));
						}
						else
						{
							if (!IsCalled_Update)
							{
								Scene_Manager::Get_Active_Scene()->MonoBehaviour_Update_list.emplace_back(static_pointer_cast<MonoBehaviour>(shared_from_this()));
								IsCalled_Update = true;
							}
							Disable_flg = false;
						}
					}
				}
			}
		}
	}
}