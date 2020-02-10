#include "MonoBehaviour_Manager.h"
#include "GameObject.h"
using namespace std;

list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Update_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Awake_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Start_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Disable_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Enable_list;

void MonoBehaviour_Manager::Reset()
{
	MonoBehaviour_Update_list.clear();
	MonoBehaviour_Awake_list.clear();
	MonoBehaviour_Start_list.clear();
	MonoBehaviour_Disable_list.clear();
	MonoBehaviour_Enable_list.clear();
}

void MonoBehaviour_Manager::Update()
{
	if (!MonoBehaviour_Awake_list.empty())
	{
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Awake_list.begin(); itr != MonoBehaviour_Awake_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Awake_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enabled)
				{
					mono->Awake();
					itr = MonoBehaviour_Awake_list.erase(itr);
					continue;
				}
			}
			itr++;
		}
	}
	if (!MonoBehaviour_Enable_list.empty())
	{
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Enable_list.begin(); itr != MonoBehaviour_Enable_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Enable_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enabled)
				{
					mono->OnEnable();
					itr = MonoBehaviour_Enable_list.erase(itr);
					continue;
				}
			}
			itr++;
		}
	}
	if (!MonoBehaviour_Start_list.empty())
	{
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Start_list.begin(); itr != MonoBehaviour_Start_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Start_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enabled)
				{
					mono->Start();
					mono->Start_Flag = true;
					itr = MonoBehaviour_Start_list.erase(itr);
					continue;
				}
			}
			itr++;
		}
	}

	for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Update_list.begin(); itr != MonoBehaviour_Update_list.end();)
	{
		if (itr->expired())
		{
			itr = MonoBehaviour_Update_list.erase(itr);
			continue;
		}
		shared_ptr<MonoBehaviour> mono = itr->lock();
		if (mono->gameObject->activeSelf())
		{
			if (mono->enabled)
			{
				if (!mono->Start_Flag)
				{
					mono->Awake();
					mono->Start();
					mono->Start_Flag = true;
				}
				mono->Update();
			}
		}
		itr++;
	}
	for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Update_list.begin(); itr != MonoBehaviour_Update_list.end();)
	{
		if (itr->expired())
		{
			itr = MonoBehaviour_Update_list.erase(itr);
			continue;
		}
		shared_ptr<MonoBehaviour> mono = itr->lock();
		if (mono->gameObject->activeSelf())
		{
			if (mono->enabled)
			{
				mono->LateUpdate();
			}
		}
		itr++;
	}

	if (!MonoBehaviour_Disable_list.empty())
	{
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Disable_list.begin(); itr != MonoBehaviour_Disable_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Disable_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->gameObject->activeSelf())
			{
				if (mono->enabled)
				{
					mono->OnDisable();
					itr = MonoBehaviour_Disable_list.erase(itr);
					continue;
				}
			}
			itr++;
		}
	}
}

void MonoBehaviour_Manager::Add(shared_ptr<MonoBehaviour> mono)
{
	MonoBehaviour_Start_list.emplace_back(mono);
	MonoBehaviour_Awake_list.emplace_back(mono);
	MonoBehaviour_Enable_list.emplace_back(mono);
	MonoBehaviour_Update_list.emplace_back(mono);
}
void MonoBehaviour_Manager::Add_Disable(shared_ptr<MonoBehaviour> mono)
{
	MonoBehaviour_Disable_list.emplace_back(mono);
}
void MonoBehaviour_Manager::Add_Enable(shared_ptr<MonoBehaviour> mono)
{
	MonoBehaviour_Enable_list.emplace_back(mono);
}