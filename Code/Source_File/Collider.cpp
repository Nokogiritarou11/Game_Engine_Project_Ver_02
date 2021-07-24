#include "Collider.h"
#include "GameObject.h"
#include "MonoBehaviour.h"

using namespace BeastEngine;
using namespace std;


void Collider::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Collider::Get_Enabled()
{
	return enabled;
}

void Collider::Initialize_MonoBehaviour()
{
	for (auto& com : gameobject->component_list)
	{
		shared_ptr<MonoBehaviour> buff = dynamic_pointer_cast<MonoBehaviour>(com);
		if (buff != nullptr)
		{
			send_list.emplace_back(buff);
		}
	}
}

void Collider::Call_Hit(shared_ptr<Collider>& col)
{
	string& id = col->Get_Instance_ID();
	auto it = hit_list.find(id);
	if (it == hit_list.end())
	{
		hit_list[id] = col;
		if (col->is_trigger)
		{
			Call_OnTrigger_Enter(col);
		}
		else
		{
			Call_OnCollision_Enter(col);
		}
	}
	else
	{
		if (col->is_trigger)
		{
			Call_OnTrigger_Stay(col);
		}
		else
		{
			Call_OnCollision_Stay(col);
		}
	}
}

void Collider::Call_Update()
{

}

void Collider::Call_OnTrigger_Enter(shared_ptr<Collider>& col)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnTrigger_Enter(col);
		}
	}
}
void Collider::Call_OnCollision_Enter(shared_ptr<Collider>& col)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnCollision_Enter(col);
		}
	}
}
void Collider::Call_OnTrigger_Stay(shared_ptr<Collider>& col)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnTrigger_Stay(col);
		}
	}
}
void Collider::Call_OnCollision_Stay(shared_ptr<Collider>& col)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnCollision_Stay(col);
		}
	}
}
void Collider::Call_OnTrigger_Exit(shared_ptr<Collider>& col)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnTrigger_Exit(col);
		}
	}
}
void Collider::Call_OnCollision_Exit(shared_ptr<Collider>& col)
{
	shared_ptr<MonoBehaviour> mono;
	for (auto& m : send_list)
	{
		if (!m.expired())
		{
			mono = m.lock();
			mono->OnCollision_Exit(col);
		}
	}
}