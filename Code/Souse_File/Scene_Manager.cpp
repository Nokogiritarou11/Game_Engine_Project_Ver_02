#include "Scene_Manager.h"
#include "Animator_Manager.h"
#include "Camera_Manager.h"
#include "GameObject_Manager.h"
#include "MonoBehaviour_Manager.h"
#include "Render_Manager.h"
using namespace std;

//**********************************************
//
//		ÉVÅ[ÉìÇÃä«óù
//
//**********************************************


std::weak_ptr<Scene> Scene_Manager::Active_Scene;
std::list<Scene_Manager::Scene_Data> Scene_Manager::Scene_List;
bool Scene_Manager::Load;
std::string Scene_Manager::Next_Scene_Name;

void Scene_Manager::CreateScene(shared_ptr<Scene> Scene_Class, string Scene_Name)
{
	list<Scene_Data>::iterator itr_end = Scene_List.end();
	for (list<Scene_Data>::iterator itr = Scene_List.begin(); itr != itr_end; itr++)
	{
		if (itr->Name == Scene_Name)
		{
			return;
		}
	}
	shared_ptr<Scene> new_scene = move(Scene_Class);
	Scene_Data new_data = { Scene_Name,new_scene };
	Scene_List.emplace_back(new_data);
}

void Scene_Manager::Set_StartScene(string Scene_Name)
{
	list<Scene_Data>::iterator itr_end = Scene_List.end();
	for (list<Scene_Data>::iterator itr = Scene_List.begin(); itr != itr_end; itr++)
	{
		if (itr->Name == Scene_Name)
		{
			Active_Scene = itr->Scene_ptr;
			itr->Scene_ptr->Initialize();
			return;
		}
	}
	
}

void Scene_Manager::LoadScene(string Scene_Name)
{
	Load = true;
	Next_Scene_Name = Scene_Name;
}

void Scene_Manager::Instance_GameObject(shared_ptr<GameObject> gameObject)
{
	shared_ptr<Scene> scene = Active_Scene.lock();
	scene->Instance_GameObject(gameObject);
}

void Scene_Manager::Destroy_GameObject(shared_ptr<GameObject> gameObject)
{
	shared_ptr<Scene> scene = Active_Scene.lock();
	scene->Destroy_GameObject(gameObject);
}

void Scene_Manager::Update()
{
	if (Load)
	{
		list<Scene_Data>::iterator itr_end = Scene_List.end();
		for (list<Scene_Data>::iterator itr = Scene_List.begin(); itr != itr_end; itr++)
		{
			if (itr->Name == Next_Scene_Name)
			{
				MonoBehaviour_Manager::Reset();
				Animator_Manager::Reset();
				GameObject_Manager::Reset();
				Render_Manager::Reset();
				Camera_Manager::Reset();
				Active_Scene = itr->Scene_ptr;
				itr->Scene_ptr->Initialize();
				Load = false;
				break;
			}
		}
	}

	shared_ptr<Scene> Play_Scene = Active_Scene.lock();
	Play_Scene->Update();
}