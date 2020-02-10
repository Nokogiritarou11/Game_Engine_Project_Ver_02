#include "Scene.h"
#include "GameObject_Manager.h"
#include "MonoBehaviour_Manager.h"
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
#include "Sprite_Renderer.h"
#include "Animator.h"
#include "Transform.h"
#include "All_Script.h"
using namespace std;

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Instance_GameObject(shared_ptr<GameObject> gameObject)
{
	gameObject_List.emplace_back(gameObject);
}

void Scene::Destroy_GameObject(shared_ptr<GameObject> gameObject)
{
	list<shared_ptr<GameObject>>::iterator itr_end = gameObject_List.end();
	for (list<shared_ptr<GameObject>>::iterator itr = gameObject_List.begin(); itr != itr_end; itr++)
	{
		if ((*itr)->ID == gameObject->ID)
		{
			gameObject_List.erase(itr);
			return;
		}
	}
}

void Scene::Update()
{
	GameObject_Manager::Update();
	MonoBehaviour_Manager::Update();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Game_01_Scene::Initialize()
{
	Audio_Manager::BGM_title->Stop();

	shared_ptr<GameObject> player = GameObject::Instantiate("player");
	player->tag = "Player";
	player->AddComponent<Player>();
	player->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> renderer = player->AddComponent<SkinMesh_Renderer>();
	renderer->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "player_default.txt"));
	shared_ptr<Animator> animator = player->AddComponent<Animator>();
	animator->Set_Skin_Renderer(renderer);
	animator->Animation_Loop = true;

	shared_ptr<GameObject> Floor = GameObject::Instantiate("Floor");
	shared_ptr<SkinMesh_Renderer> f_renderer = Floor->AddComponent<SkinMesh_Renderer>();
	Floor->transform->eulerAngles = { -90,0,0 };
	Floor->transform->scale = { 2,2,2 };
	f_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_yuka1.txt"));



	shared_ptr<GameObject> Wall_01 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_01 = Wall_01->AddComponent<SkinMesh_Renderer>();
	Wall_01->transform->eulerAngles = { -80,0,0 };
	Wall_01->transform->position = { 65,0,50 };
	Wall_01->transform->scale = { 4,2,2 };
	w_renderer_01->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_02 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_02 = Wall_02->AddComponent<SkinMesh_Renderer>();
	Wall_02->transform->eulerAngles = { -80,0,0 };
	Wall_02->transform->position = { 0,0,50 };
	Wall_02->transform->scale = { 4,2,2 };
	w_renderer_02->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_03 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_03 = Wall_03->AddComponent<SkinMesh_Renderer>();
	Wall_03->transform->eulerAngles = { -80,0,0 };
	Wall_03->transform->position = { -65,0,50 };
	Wall_03->transform->scale = { 4,2,2 };
	w_renderer_03->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_04 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_04 = Wall_04->AddComponent<SkinMesh_Renderer>();
	Wall_04->transform->eulerAngles = { -80,90,0 };
	Wall_04->transform->position = { 90,0,50 };
	Wall_04->transform->scale = { 4,2,2 };
	w_renderer_04->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_05 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_05 = Wall_05->AddComponent<SkinMesh_Renderer>();
	Wall_05->transform->eulerAngles = { -80,90,0 };
	Wall_05->transform->position = { 90,0,0 };
	Wall_05->transform->scale = { 4,2,2 };
	w_renderer_05->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_06 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_06 = Wall_06->AddComponent<SkinMesh_Renderer>();
	Wall_06->transform->eulerAngles = { -80,90,0 };
	Wall_06->transform->position = { 90,0,-50 };
	Wall_06->transform->scale = { 4,2,2 };
	w_renderer_06->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_07 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_07 = Wall_07->AddComponent<SkinMesh_Renderer>();
	Wall_07->transform->eulerAngles = { -80,-90,0 };
	Wall_07->transform->position = { -90,0,50 };
	Wall_07->transform->scale = { 4,2,2 };
	w_renderer_07->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_08 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_08 = Wall_08->AddComponent<SkinMesh_Renderer>();
	Wall_08->transform->eulerAngles = { -80,-90,0 };
	Wall_08->transform->position = { -90,0,0 };
	Wall_08->transform->scale = { 4,2,2 };
	w_renderer_08->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_09 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_09 = Wall_09->AddComponent<SkinMesh_Renderer>();
	Wall_09->transform->eulerAngles = { -80,-90,0 };
	Wall_09->transform->position = { -90,0,-50 };
	Wall_09->transform->scale = { 4,2,2 };
	w_renderer_09->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_10 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_10 = Wall_10->AddComponent<SkinMesh_Renderer>();
	Wall_10->transform->eulerAngles = { -110,0,0 };
	Wall_10->transform->position = { 65,0,-50 };
	Wall_10->transform->scale = { 4,2,2 };
	w_renderer_10->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_11 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_11 = Wall_11->AddComponent<SkinMesh_Renderer>();
	Wall_11->transform->eulerAngles = { -110,0,0 };
	Wall_11->transform->position = { 0,0,-50 };
	Wall_11->transform->scale = { 4,2,2 };
	w_renderer_11->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_12 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_12 = Wall_12->AddComponent<SkinMesh_Renderer>();
	Wall_12->transform->eulerAngles = { -110,0,0 };
	Wall_12->transform->position = { -65,0,-50 };
	Wall_12->transform->scale = { 4,2,2 };
	w_renderer_12->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));


	shared_ptr<GameObject> G_Manager = GameObject::Instantiate("Game_Manager");
	shared_ptr<Game_Manager> g_m =  G_Manager->AddComponent<Game_Manager>();
	g_m->Stage_Number = 0;
	G_Manager->AddComponent<UI_Controller>();

	shared_ptr<GameObject> enemy = GameObject::Instantiate("enemy");
	enemy->tag = "Enemy";
	enemy->transform->position = { 35,0,0 };
	enemy->transform->scale = { 0.2f,0.2f ,0.2f };
	shared_ptr<Enemy> e = enemy->AddComponent<Enemy>();
	e->Enemy_State = 0;
	enemy->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> e_renderer = enemy->AddComponent<SkinMesh_Renderer>();
	e_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "enemy_default.txt"));
	shared_ptr<Animator> e_animator = enemy->AddComponent<Animator>();
	e_animator->Set_Skin_Renderer(e_renderer);
	e_animator->Animation_Loop = true;

	shared_ptr<GameObject> camera = GameObject::Instantiate("Main_Camera");
	shared_ptr<Camera> camera_Comp = camera->AddComponent<Camera>();
	camera->transform->position = { 2.5f,200,0.0f };
	camera->transform->eulerAngles = { 90,0,0 };




	Audio_Manager::BGM_play->SetVolume(0.25f);
	Audio_Manager::BGM_play->Play(true);
}

void Game_02_Scene::Initialize()
{
	shared_ptr<GameObject> player = GameObject::Instantiate("player");
	player->tag = "Player";
	player->AddComponent<Player>();
	player->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> renderer = player->AddComponent<SkinMesh_Renderer>();
	renderer->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "player_default.txt"));
	shared_ptr<Animator> animator = player->AddComponent<Animator>();
	animator->Set_Skin_Renderer(renderer);
	animator->Animation_Loop = true;

	shared_ptr<GameObject> Floor = GameObject::Instantiate("Floor");
	shared_ptr<SkinMesh_Renderer> f_renderer = Floor->AddComponent<SkinMesh_Renderer>();
	Floor->transform->eulerAngles = { -90,0,0 };
	Floor->transform->scale = { 2,2,2 };
	f_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_yuka1.txt"));

	shared_ptr<GameObject> Wall_01 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_01 = Wall_01->AddComponent<SkinMesh_Renderer>();
	Wall_01->transform->eulerAngles = { -80,0,0 };
	Wall_01->transform->position = { 65,0,50 };
	Wall_01->transform->scale = { 4,2,2 };
	w_renderer_01->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_02 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_02 = Wall_02->AddComponent<SkinMesh_Renderer>();
	Wall_02->transform->eulerAngles = { -80,0,0 };
	Wall_02->transform->position = { 0,0,50 };
	Wall_02->transform->scale = { 4,2,2 };
	w_renderer_02->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_03 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_03 = Wall_03->AddComponent<SkinMesh_Renderer>();
	Wall_03->transform->eulerAngles = { -80,0,0 };
	Wall_03->transform->position = { -65,0,50 };
	Wall_03->transform->scale = { 4,2,2 };
	w_renderer_03->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_04 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_04 = Wall_04->AddComponent<SkinMesh_Renderer>();
	Wall_04->transform->eulerAngles = { -80,90,0 };
	Wall_04->transform->position = { 90,0,50 };
	Wall_04->transform->scale = { 4,2,2 };
	w_renderer_04->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_05 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_05 = Wall_05->AddComponent<SkinMesh_Renderer>();
	Wall_05->transform->eulerAngles = { -80,90,0 };
	Wall_05->transform->position = { 90,0,0 };
	Wall_05->transform->scale = { 4,2,2 };
	w_renderer_05->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_06 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_06 = Wall_06->AddComponent<SkinMesh_Renderer>();
	Wall_06->transform->eulerAngles = { -80,90,0 };
	Wall_06->transform->position = { 90,0,-50 };
	Wall_06->transform->scale = { 4,2,2 };
	w_renderer_06->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_07 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_07 = Wall_07->AddComponent<SkinMesh_Renderer>();
	Wall_07->transform->eulerAngles = { -80,-90,0 };
	Wall_07->transform->position = { -90,0,50 };
	Wall_07->transform->scale = { 4,2,2 };
	w_renderer_07->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_08 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_08 = Wall_08->AddComponent<SkinMesh_Renderer>();
	Wall_08->transform->eulerAngles = { -80,-90,0 };
	Wall_08->transform->position = { -90,0,0 };
	Wall_08->transform->scale = { 4,2,2 };
	w_renderer_08->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_09 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_09 = Wall_09->AddComponent<SkinMesh_Renderer>();
	Wall_09->transform->eulerAngles = { -80,-90,0 };
	Wall_09->transform->position = { -90,0,-50 };
	Wall_09->transform->scale = { 4,2,2 };
	w_renderer_09->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_10 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_10 = Wall_10->AddComponent<SkinMesh_Renderer>();
	Wall_10->transform->eulerAngles = { -110,0,0 };
	Wall_10->transform->position = { 65,0,-50 };
	Wall_10->transform->scale = { 4,2,2 };
	w_renderer_10->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_11 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_11 = Wall_11->AddComponent<SkinMesh_Renderer>();
	Wall_11->transform->eulerAngles = { -110,0,0 };
	Wall_11->transform->position = { 0,0,-50 };
	Wall_11->transform->scale = { 4,2,2 };
	w_renderer_11->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_12 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_12 = Wall_12->AddComponent<SkinMesh_Renderer>();
	Wall_12->transform->eulerAngles = { -110,0,0 };
	Wall_12->transform->position = { -65,0,-50 };
	Wall_12->transform->scale = { 4,2,2 };
	w_renderer_12->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> G_Manager = GameObject::Instantiate("Game_Manager");
	G_Manager->AddComponent<Game_Manager>();
	G_Manager->AddComponent<UI_Controller>();

	shared_ptr<GameObject> enemy = GameObject::Instantiate("enemy");
	enemy->tag = "Enemy";
	enemy->transform->position = { 40,0,0 };
	enemy->transform->scale = { 0.2f,0.2f ,0.2f };
	shared_ptr<Enemy> e = enemy->AddComponent<Enemy>();
	e->Enemy_State = 0;
	enemy->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> e_renderer = enemy->AddComponent<SkinMesh_Renderer>();
	e_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "enemy_default.txt"));
	shared_ptr<Animator> e_animator = enemy->AddComponent<Animator>();
	e_animator->Set_Skin_Renderer(e_renderer);
	e_animator->Animation_Loop = true;

	shared_ptr<GameObject> enemy_02 = GameObject::Instantiate("enemy_02");
	enemy_02->tag = "Enemy";
	enemy_02->transform->position = { -40,0,0 };
	enemy_02->transform->scale = { 0.2f,0.2f ,0.2f };
	shared_ptr<Enemy> e_02 = enemy_02->AddComponent<Enemy>();
	e_02->Enemy_State = 0;
	enemy_02->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> e_renderer_02 = enemy_02->AddComponent<SkinMesh_Renderer>();
	e_renderer_02->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "enemy_default.txt"));
	shared_ptr<Animator> e_animator_02 = enemy_02->AddComponent<Animator>();
	e_animator_02->Set_Skin_Renderer(e_renderer_02);
	e_animator_02->Animation_Loop = true;

	shared_ptr<GameObject> camera = GameObject::Instantiate("Main_Camera");
	shared_ptr<Camera> camera_Comp = camera->AddComponent<Camera>();
	camera->transform->position = { 2.5f,200,0.0f };
	camera->transform->eulerAngles = { 90,0,0 };
}
void Game_03_Scene::Initialize()
{
	shared_ptr<GameObject> player = GameObject::Instantiate("player");
	player->tag = "Player";
	player->AddComponent<Player>();
	player->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> renderer = player->AddComponent<SkinMesh_Renderer>();
	renderer->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "player_default.txt"));
	shared_ptr<Animator> animator = player->AddComponent<Animator>();
	animator->Set_Skin_Renderer(renderer);
	animator->Animation_Loop = true;

	shared_ptr<GameObject> Floor = GameObject::Instantiate("Floor");
	shared_ptr<SkinMesh_Renderer> f_renderer = Floor->AddComponent<SkinMesh_Renderer>();
	Floor->transform->eulerAngles = { -90,0,0 };
	Floor->transform->scale = { 2,2,2 };
	f_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_yuka1.txt"));

	shared_ptr<GameObject> Wall_01 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_01 = Wall_01->AddComponent<SkinMesh_Renderer>();
	Wall_01->transform->eulerAngles = { -80,0,0 };
	Wall_01->transform->position = { 65,0,50 };
	Wall_01->transform->scale = { 4,2,2 };
	w_renderer_01->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_02 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_02 = Wall_02->AddComponent<SkinMesh_Renderer>();
	Wall_02->transform->eulerAngles = { -80,0,0 };
	Wall_02->transform->position = { 0,0,50 };
	Wall_02->transform->scale = { 4,2,2 };
	w_renderer_02->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_03 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_03 = Wall_03->AddComponent<SkinMesh_Renderer>();
	Wall_03->transform->eulerAngles = { -80,0,0 };
	Wall_03->transform->position = { -65,0,50 };
	Wall_03->transform->scale = { 4,2,2 };
	w_renderer_03->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_04 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_04 = Wall_04->AddComponent<SkinMesh_Renderer>();
	Wall_04->transform->eulerAngles = { -80,90,0 };
	Wall_04->transform->position = { 90,0,50 };
	Wall_04->transform->scale = { 4,2,2 };
	w_renderer_04->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_05 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_05 = Wall_05->AddComponent<SkinMesh_Renderer>();
	Wall_05->transform->eulerAngles = { -80,90,0 };
	Wall_05->transform->position = { 90,0,0 };
	Wall_05->transform->scale = { 4,2,2 };
	w_renderer_05->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_06 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_06 = Wall_06->AddComponent<SkinMesh_Renderer>();
	Wall_06->transform->eulerAngles = { -80,90,0 };
	Wall_06->transform->position = { 90,0,-50 };
	Wall_06->transform->scale = { 4,2,2 };
	w_renderer_06->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_07 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_07 = Wall_07->AddComponent<SkinMesh_Renderer>();
	Wall_07->transform->eulerAngles = { -80,-90,0 };
	Wall_07->transform->position = { -90,0,50 };
	Wall_07->transform->scale = { 4,2,2 };
	w_renderer_07->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_08 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_08 = Wall_08->AddComponent<SkinMesh_Renderer>();
	Wall_08->transform->eulerAngles = { -80,-90,0 };
	Wall_08->transform->position = { -90,0,0 };
	Wall_08->transform->scale = { 4,2,2 };
	w_renderer_08->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_09 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_09 = Wall_09->AddComponent<SkinMesh_Renderer>();
	Wall_09->transform->eulerAngles = { -80,-90,0 };
	Wall_09->transform->position = { -90,0,-50 };
	Wall_09->transform->scale = { 4,2,2 };
	w_renderer_09->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_10 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_10 = Wall_10->AddComponent<SkinMesh_Renderer>();
	Wall_10->transform->eulerAngles = { -110,0,0 };
	Wall_10->transform->position = { 65,0,-50 };
	Wall_10->transform->scale = { 4,2,2 };
	w_renderer_10->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_11 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_11 = Wall_11->AddComponent<SkinMesh_Renderer>();
	Wall_11->transform->eulerAngles = { -110,0,0 };
	Wall_11->transform->position = { 0,0,-50 };
	Wall_11->transform->scale = { 4,2,2 };
	w_renderer_11->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> Wall_12 = GameObject::Instantiate("Wall");
	shared_ptr<SkinMesh_Renderer> w_renderer_12 = Wall_12->AddComponent<SkinMesh_Renderer>();
	Wall_12->transform->eulerAngles = { -110,0,0 };
	Wall_12->transform->position = { -65,0,-50 };
	Wall_12->transform->scale = { 4,2,2 };
	w_renderer_12->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_tobira.txt"));

	shared_ptr<GameObject> G_Manager = GameObject::Instantiate("Game_Manager");
	G_Manager->AddComponent<Game_Manager>();
	G_Manager->AddComponent<UI_Controller>();

	shared_ptr<GameObject> enemy = GameObject::Instantiate("enemy");
	enemy->tag = "Enemy";
	enemy->transform->position = { -40,0,0 };
	enemy->transform->scale = { 0.2f,0.2f ,0.2f };
	shared_ptr<Enemy> e = enemy->AddComponent<Enemy>();
	e->Enemy_State = 0;
	enemy->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> e_renderer = enemy->AddComponent<SkinMesh_Renderer>();
	e_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "enemy_default.txt"));
	shared_ptr<Animator> e_animator = enemy->AddComponent<Animator>();
	e_animator->Set_Skin_Renderer(e_renderer);
	e_animator->Animation_Loop = true;

	shared_ptr<GameObject> enemy_02 = GameObject::Instantiate("enemy_02");
	enemy_02->tag = "Enemy";
	enemy_02->transform->position = { 40,0,0 };
	enemy_02->transform->scale = { 0.2f,0.2f ,0.2f };
	shared_ptr<Enemy> e_02 = enemy_02->AddComponent<Enemy>();
	e_02->Enemy_State = 0;
	enemy_02->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> e_renderer_02 = enemy_02->AddComponent<SkinMesh_Renderer>();
	e_renderer_02->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "enemy_default.txt"));
	shared_ptr<Animator> e_animator_02 = enemy_02->AddComponent<Animator>();
	e_animator_02->Set_Skin_Renderer(e_renderer_02);
	e_animator_02->Animation_Loop = true;


	shared_ptr<GameObject> enemy_03 = GameObject::Instantiate("enemy_03");
	enemy_03->tag = "Enemy";
	enemy_03->transform->position = { 0,0,35 };
	enemy_03->transform->scale = { 0.2f,0.2f ,0.2f };
	shared_ptr<Enemy> e_03 = enemy_03->AddComponent<Enemy>();
	e_03->Enemy_State = 0;
	enemy_03->AddComponent<Pauser>();
	shared_ptr<SkinMesh_Renderer> e_renderer_03 = enemy_03->AddComponent<SkinMesh_Renderer>();
	e_renderer_03->Set_Mesh(Mesh::Load_Mesh("Model\\main\\", "enemy_default.txt"));
	shared_ptr<Animator> e_animator_03 = enemy_03->AddComponent<Animator>();
	e_animator_03->Set_Skin_Renderer(e_renderer_03);
	e_animator_03->Animation_Loop = true;

	shared_ptr<GameObject> camera = GameObject::Instantiate("Main_Camera");
	shared_ptr<Camera> camera_Comp = camera->AddComponent<Camera>();
	camera->transform->position = { 2.5f,200,0.0f };
	camera->transform->eulerAngles = { 90,0,0 };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void Title_Scene::Initialize()
{
	Audio_Manager::BGM_play->Stop();

	shared_ptr<GameObject> camera = GameObject::Instantiate("Main_Camera");
	shared_ptr<Camera> camera_Comp = camera->AddComponent<Camera>();
	camera->transform->position = { 0,200,0.0f };
	camera->transform->eulerAngles = { 90,0,0 };

	shared_ptr<GameObject> player = GameObject::Instantiate("player");
	player->AddComponent<Title>();

	shared_ptr<GameObject> Floor = GameObject::Instantiate("Floor");
	shared_ptr<SkinMesh_Renderer> f_renderer = Floor->AddComponent<SkinMesh_Renderer>();
	Floor->transform->eulerAngles = { -90,0,0 };
	Floor->transform->scale = { 2,2,2 };
	f_renderer->Set_Mesh(Mesh::Load_Mesh("Model\\haikei\\", "haikei_yuka1.txt"));

	Audio_Manager::BGM_title->SetVolume(0.25f);
	Audio_Manager::BGM_title->Play(true);
}