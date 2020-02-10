#include "Hit_Controller.h"
#include "Animator.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"

void Hit_Controller::Start()
{
	animator = GetComponent<Animator>();
	IsEnemy = gameObject->CompareTag("Enemy");
}

void Hit_Controller::Hit(shared_ptr<GameObject> obj)
{
	if (obj->CompareTag("Player"))
	{
		shared_ptr<Player> p = obj->GetComponent<Player>();
		p->Damage();
	}
	else
	{
		shared_ptr<Enemy> e = obj->GetComponent<Enemy>();
		e->Damage();
	}
}