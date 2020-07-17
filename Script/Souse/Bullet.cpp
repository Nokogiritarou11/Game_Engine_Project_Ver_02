#include "Bullet.h"
using namespace std;
void Bullet::Start()
{
	shared_ptr<GameObject> m = GameObject::FindWithTag("Enemy_Manager").lock();
	e_manager = m->GetComponent<Enemy_Manager>();
	transform->Set_scale(0.01f, 0.01f, 0.01f);
	speed = 100;
}

void Bullet::Update()
{
	transform->Set_position(transform->Get_position() + transform->Get_forward() * speed * Time::deltaTime);
	timer += Time::deltaTime;

	shared_ptr<Enemy_Manager> manager = e_manager.lock();
	for (int i = 0; i < 3; i++)
	{
		shared_ptr<GameObject> e = manager->enemys[i].lock();
		float dis = Vector3::Distance(transform->Get_position(), e->transform->Get_position());
		if (dis < 10)
		{
			gameObject->SetActive(false);
			timer = 0;
		}
	}

	if (timer > 2)
	{
		gameObject->SetActive(false);
		timer = 0;
	}
}