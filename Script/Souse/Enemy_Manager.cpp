#include "Enemy_Manager.h"
#include "SkinMesh_Renderer.h"
using namespace std;

void Enemy_Manager::Awake()
{
}

void Enemy_Manager::Start()
{
	Instance_Enemy(Vector3(-30, 0, 30), Vector3(180, 0, 0));
	Instance_Enemy(Vector3(0, 0, 30), Vector3(180, 0, 0));
	Instance_Enemy(Vector3(30, 0, 30), Vector3(180, 0, 0));
}

void Enemy_Manager::Update()
{
}

void Enemy_Manager::Instance_Enemy(Vector3 pos, Vector3 rot)
{
	shared_ptr<GameObject> enemy = GameObject::Instantiate("enemy", pos, rot);
	enemy->tag = "Enemy";
	enemy->AddComponent<Enemy>();
	shared_ptr<SkinMesh_Renderer> renderer = enemy->AddComponent<SkinMesh_Renderer>();
	renderer->Set_Mesh(Mesh::Load_Mesh("Model\\", "tank_enemy"));
	enemys.push_back(enemy);
}