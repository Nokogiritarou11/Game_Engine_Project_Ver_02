#include "UI_Controller.h"
#include "Game_Manager.h"
#include "Sprite_Renderer.h"
#include "Player.h"
#include "GameObject_Manager.h"
#include "GameObject.h"

void UI_Controller::Start()
{
	shared_ptr<GameObject> obj = GameObject_Manager::FindWithTag("Player").lock();
	player = obj->GetComponent<Player>();

	shared_ptr<GameObject> image_move = GameObject::Instantiate("Cursor_Move");
	shared_ptr<Sprite_Renderer> m_renderer = image_move->AddComponent<Sprite_Renderer>();
	m_renderer->Set_Texture("Cursor_Move", L"Code/Shader/2D.fx", L"Image\\ka-soru_B.png");
	Cursor_Move = image_move;

	shared_ptr<GameObject> image_set = GameObject::Instantiate("Cursor_Set");
	shared_ptr<Sprite_Renderer> s_renderer = image_set->AddComponent<Sprite_Renderer>();
	s_renderer->Set_Texture("Cursor_Set", L"Code/Shader/2D.fx", L"Image\\ka-soru_R.png");
	Cursor_Set = image_set;

	shared_ptr<GameObject> image_player = GameObject::Instantiate("Player_HP");
	shared_ptr<Sprite_Renderer> p_renderer = image_player->AddComponent<Sprite_Renderer>();
	p_renderer->Set_Texture("HP_3", L"Code/Shader/2D.fx", L"Image\\UI_HP_3.png");
	image_player->transform->Width = 1024 / 2;
	image_player->transform->Height = 256 / 2;
	image_player->transform->position = { 25,25,0 };

	shared_ptr<GameObject> image_player_bar = GameObject::Instantiate("Player_HP_Bar");
	shared_ptr<Sprite_Renderer> b_renderer = image_player_bar->AddComponent<Sprite_Renderer>();
	b_renderer->Set_Texture("HP_4", L"Code/Shader/2D.fx", L"Image\\UI_HP_4.png");
	b_renderer->material->color = { 0,1,0,1 };
	player_bar = b_renderer;
	image_player_bar->transform->Width = 420;
	image_player_bar->transform->Height = 256 / 2;
	image_player_bar->transform->position = { 95,25,0 };

	shared_ptr<GameObject> image_clear = GameObject::Instantiate("Clear");
	shared_ptr<Sprite_Renderer> c_renderer = image_clear->AddComponent<Sprite_Renderer>();
	c_renderer->Set_Texture("Clear", L"Code/Shader/2D.fx", L"Image\\text1.png");
	image_clear->transform->Width = 1500;
	image_clear->transform->Height = 400;
	image_clear->transform->position = { (float)DxSystem::GetScreenWidth() / 2 - image_clear->transform->Width / 2,(float)DxSystem::GetScreenHeight() / 2 - 200,0 };
	image_clear->SetActive(false);
	Clear = image_clear;

	shared_ptr<GameObject> image_faild = GameObject::Instantiate("Faild");
	shared_ptr<Sprite_Renderer> f_renderer = image_faild->AddComponent<Sprite_Renderer>();
	f_renderer->Set_Texture("Faild", L"Code/Shader/2D.fx", L"Image\\text2.png");
	image_faild->transform->Width = 1500;
	image_faild->transform->Height = 400;
	image_faild->transform->position = { (float)DxSystem::GetScreenWidth() / 2 - image_faild->transform->Width / 2,(float)DxSystem::GetScreenHeight() / 2 - 200,0 };
	image_faild->SetActive(false);
	Failed = image_faild;

	shared_ptr<GameObject> m = GameObject::Find("Game_Manager").lock();
	game_manager = m->GetComponent<Game_Manager>();
}

void UI_Controller::Update()
{
	shared_ptr<Game_Manager> g_manager = game_manager.lock();
	shared_ptr<GameObject> cursor_set = Cursor_Set.lock();
	shared_ptr<GameObject> cursor_move = Cursor_Move.lock();
	shared_ptr<Player> p = player.lock();
	shared_ptr<Sprite_Renderer> bar = player_bar.lock();

	bar->transform->Width = 420 * p->HP / 100;
	if (p->HP / 100.0f > 0.5f)
	{
		bar->material->color = { 0,1,0,1 };
	}
	else if (p->HP / 100.0f > 0.25f)
	{
		bar->material->color = { 1,1,0,1 };
	}
	else
	{
		bar->material->color = { 1,0,0,1 };
	}

	if (g_manager->Change_State == 1)
	{
		if (!cursor_set->activeSelf())
		{
			cursor_set->SetActive(true);
			cursor_set->transform->position = { g_manager->Select_pos.x - cursor_set->transform->Width / 2 ,g_manager->Select_pos.y - cursor_set->transform->Height / 2,0 };
		}
	}
	else
	{
		if (cursor_set->activeSelf())
		{
			cursor_set->SetActive(false);
		}
	}

	if (g_manager->Locking)
	{
		if (!cursor_move->activeSelf())
		{
			cursor_move->SetActive(true);
		}
		cursor_move->transform->position = { g_manager->target_pos.x - cursor_move->transform->Width / 2 ,g_manager->target_pos.y - cursor_move->transform->Height / 2 ,0 };
	}
	else
	{
		if (cursor_move->activeSelf())
		{
			cursor_move->SetActive(false);
		}
	}

	if (g_manager->Game_Clear)
	{
		shared_ptr<GameObject> clear = Clear.lock();
		clear->SetActive(true);
	}
	if (g_manager->Game_Over)
	{
		shared_ptr<GameObject> faild = Failed.lock();
		faild->SetActive(true);
	}
}