#include "MonoBehaviour_Manager.h"
#include "GameObject.h"
using namespace std;

list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Update_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Awake_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Start_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Disable_list;
list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Enable_list;

list<weak_ptr<MonoBehaviour>> MonoBehaviour_Manager::MonoBehaviour_Next_Update_list;
int MonoBehaviour_Manager::Update_Stage = 0;

void MonoBehaviour_Manager::Reset()
{
	MonoBehaviour_Update_list.clear();
	MonoBehaviour_Awake_list.clear();
	MonoBehaviour_Start_list.clear();
	MonoBehaviour_Disable_list.clear();
	MonoBehaviour_Enable_list.clear();
	Update_Stage = 0;
}

void MonoBehaviour_Manager::Update()
{
	if (!MonoBehaviour_Next_Update_list.empty())
	{
		Update_Stage = 0;
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Next_Update_list.begin(); itr != MonoBehaviour_Next_Update_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Next_Update_list.erase(itr);
				continue;
			}
			else
			{
				shared_ptr<MonoBehaviour> mono = itr->lock();
				MonoBehaviour_Update_list.emplace_back(mono);
				itr = MonoBehaviour_Next_Update_list.erase(itr);
				continue;
			}
		}
	}
	if (!MonoBehaviour_Awake_list.empty())
	{
		Update_Stage = 1;
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
				mono->Awake();
				itr = MonoBehaviour_Awake_list.erase(itr);
				continue;
			}
			itr++;
		}
	}
	if (!MonoBehaviour_Enable_list.empty())
	{
		Update_Stage = 2;
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
		Update_Stage = 3;
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
					itr = MonoBehaviour_Start_list.erase(itr);
					continue;
				}
			}
			itr++;
		}
	}

	for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Update_list.begin(); itr != MonoBehaviour_Update_list.end();)
	{
		Update_Stage = 4;
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
				mono->Update();
			}
		}
		itr++;
	}
	for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Update_list.begin(); itr != MonoBehaviour_Update_list.end();)
	{
		Update_Stage = 5;
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
		Update_Stage = 6;
		for (list<weak_ptr<MonoBehaviour>>::iterator itr = MonoBehaviour_Disable_list.begin(); itr != MonoBehaviour_Disable_list.end();)
		{
			if (itr->expired())
			{
				itr = MonoBehaviour_Disable_list.erase(itr);
				continue;
			}
			shared_ptr<MonoBehaviour> mono = itr->lock();
			if (mono->enabled)
			{
				mono->OnDisable();
				itr = MonoBehaviour_Disable_list.erase(itr);
				continue;
			}
			itr++;
		}
	}
}

void MonoBehaviour_Manager::Add(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //‰Šú
			MonoBehaviour_Awake_list.emplace_back(mono);
			MonoBehaviour_Enable_list.emplace_back(mono);
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Update_list.emplace_back(mono);
			break;
		case 1: //Awake
			mono->Awake();
			MonoBehaviour_Enable_list.emplace_back(mono);
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 2: //Enabled
			mono->Awake();
			mono->OnEnable();
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 3: //Start
			mono->Awake();
			mono->OnEnable();
			MonoBehaviour_Start_list.emplace_back(mono);
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 4: //Update
			mono->Awake();
			mono->OnEnable();
			mono->Start();
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 5: //LateUpdate
			mono->Awake();
			mono->OnEnable();
			mono->Start();
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		case 6: //Disabled
			mono->Awake();
			mono->OnEnable();
			mono->Start();
			MonoBehaviour_Next_Update_list.emplace_back(mono);
			break;
		default:
			break;
	}
}
void MonoBehaviour_Manager::Add_Disable(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //‰Šú
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 1: //Awake
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 2: //Enabled
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 3: //Start
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 4: //Update
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 5: //LateUpdate
			MonoBehaviour_Disable_list.emplace_back(mono);
			break;
		case 6: //Disabled
			mono->OnDisable();
			break;
		default:
			break;
	}
}
void MonoBehaviour_Manager::Add_Enable(shared_ptr<MonoBehaviour> mono)
{
	switch (Update_Stage)
	{
		case 0: //‰Šú
			MonoBehaviour_Enable_list.emplace_back(mono);
			break;
		case 1: //Awake
			MonoBehaviour_Enable_list.emplace_back(mono);
			break;
		case 2: //Enabled
			mono->OnEnable();
			break;
		case 3: //Start
			mono->OnEnable();
			break;
		case 4: //Update
			mono->OnEnable();
			break;
		case 5: //LateUpdate
			mono->OnEnable();
			break;
		case 6: //Disabled
			mono->OnEnable();
			break;
		default:
			break;
	}
}