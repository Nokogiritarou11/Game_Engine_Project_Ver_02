#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

void MonoBehaviour::Initialize(shared_ptr<GameObject> obj)
{
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;
	is_called_Awake = false;
	is_called_Start = false;
	is_called_Update = false;
	if (Engine::scene_manager->run)
	{
		Add();
	}
}

void  MonoBehaviour::Set_Active(bool value)
{
	if (value)
	{
		Add();
	}
	else
	{
		if (Get_Enabled())
		{
			OnDisable();
		}
	}
}

void  MonoBehaviour::Add()
{
	if (gameobject->Get_Active_In_Hierarchy())
	{
		if (!is_called_Awake)
		{
			Awake();
			is_called_Awake = true;
		}
		if (gameobject->Get_Active_In_Hierarchy())
		{
			if (Get_Enabled())
			{
				OnEnable();
				if (gameobject->Get_Active_In_Hierarchy())
				{
					if (Get_Enabled())
					{
						if (!is_called_Start)
						{
							Scene_Manager::Get_Active_Scene()->monobehaviour_Start_next_list.emplace_back(static_pointer_cast<MonoBehaviour>(shared_from_this()));
						}
						else
						{
							if (!is_called_Update)
							{
								Scene_Manager::Get_Active_Scene()->monobehaviour_Update_next_list.emplace_back(static_pointer_cast<MonoBehaviour>(shared_from_this()));
								is_called_Update = true;
							}
						}
					}
				}
			}
		}
	}
}