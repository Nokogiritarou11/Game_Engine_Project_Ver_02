#include "MonoBehaviour.h"
#include "GameObject.h"
#include "Engine.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

void MonoBehaviour::Initialize(const shared_ptr<GameObject>& obj)
{
	//マネージャーへの登録とComponentの初期化
	Engine::asset_manager->Registration_Asset(shared_from_this());
	gameobject = obj;
	transform = obj->transform;

	is_called_awake = false;
	is_called_start = false;
	is_called_update = false;
	Add();
}

void  MonoBehaviour::Set_Active(const bool value)
{
	if (Engine::scene_manager->run)
	{
		if (value)
		{
			Add();
		}
		else
		{
			if ((!gameobject->Get_Active() && Get_Enabled()) || (gameobject->Get_Active() && !Get_Enabled()))
			{
				OnDisable();
			}
		}
	}
}

void  MonoBehaviour::Add()
{
	if (Engine::scene_manager->run)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			//Awakeがまだ呼ばれていないなら呼ぶ
			if (!is_called_awake)
			{
				Awake();
				is_called_awake = true;
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
							//初回のみマネージャーに登録
							if (!is_called_start)
							{
								Engine::scene_manager->Get_Active_Scene()->monobehaviour_start_next_list.emplace_back(static_pointer_cast<MonoBehaviour>(shared_from_this()));
							}
							else
							{
								if (!is_called_update)
								{
									Engine::scene_manager->Get_Active_Scene()->monobehaviour_update_next_list.emplace_back(static_pointer_cast<MonoBehaviour>(shared_from_this()));
									is_called_update = true;
								}
							}
						}
					}
				}
			}
		}
	}
}