#include "Kunai.h"
#include "Game_Manager.h"
#include "Attack_Decision.h"

void Kunai::Start()
{
	player = GameObject::FindWithTag("Player");
	shared_ptr<GameObject> obj = GameObject::Find("Game_Manager").lock();
	game_manager = obj->GetComponent<Game_Manager>();
	shared_ptr<Game_Manager> manager = game_manager.lock();
	manager->LockOn_able_List.emplace_back(gameObject);
	AddComponent<Attack_Decision>();
}

void Kunai::Update()
{
	float y = static_cast<Vector3>(transform->eulerAngles).y;
	transform->position.x -= sin(XMConvertToRadians(y)) * speed * Time::deltaTime;
	transform->position.z -= cos(XMConvertToRadians(y)) * speed * Time::deltaTime;
	timer += Time::deltaTime;
	if(timer > Disable_time)
	{
		gameObject->SetActive(false);
		timer = 0;
	}
}