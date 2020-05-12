#include "Title.h"
#include "Sprite_Renderer.h"
#include "Scene_Manager.h"

void Title::Start()
{
	shared_ptr<GameObject> image_title = GameObject::Instantiate("Title");
	shared_ptr<Sprite_Renderer> t_renderer = image_title->AddComponent<Sprite_Renderer>();
	t_renderer->Set_Texture("title", L"Code/Shader/2D.fx", L"Image\\title.png");
	image_title->transform->Width = 1400;
	image_title->transform->Height = 400;
	image_title->transform->position = { (float)DxSystem::GetScreenWidth() / 2 - image_title->transform->Width / 2,(float)DxSystem::GetScreenHeight() / 2 - 300,0 };
	title = image_title;

	shared_ptr<GameObject> image_move = GameObject::Instantiate("text");
	shared_ptr<Sprite_Renderer> m_renderer = image_move->AddComponent<Sprite_Renderer>();
	m_renderer->Set_Texture("text", L"Code/Shader/2D.fx", L"Image\\ninnmukaisi.png");
	image_move->transform->Width = 1100;
	image_move->transform->Height = 150;
	image_move->transform->position = { (float)DxSystem::GetScreenWidth() / 2 - image_move->transform->Width / 2,(float)DxSystem::GetScreenHeight() / 2 + 300,0 };
	text = image_move;

	shared_ptr<GameObject> image_ex = GameObject::Instantiate("explain");
	shared_ptr<Sprite_Renderer> e_renderer = image_ex->AddComponent<Sprite_Renderer>();
	e_renderer->Set_Texture("explain", L"Code/Shader/2D.fx", L"Image\\torisetu.png");
	image_ex->transform->Width = 1980;
	image_ex->transform->Height = 1080;
	image_ex->transform->position = { (float)DxSystem::GetScreenWidth() / 2 - image_ex->transform->Width / 2,(float)DxSystem::GetScreenHeight() / 2 - image_ex->transform->Height / 2,0 };
	explain = image_ex;
	image_ex->SetActive(false);
}

void Title::Update()
{
	if (state == 0)
	{
		shared_ptr<GameObject> g = text.lock();
		timer += Time::deltaTime;
		if (timer > 1)
		{
			if (g->activeSelf())
			{
				g->SetActive(false);
			}
			else
			{
				g->SetActive(true);
			}
			timer = 0;
		}
	}

	if (Input_Manager::pad_tracker.a == GamePad::ButtonStateTracker::PRESSED)
	{
		state++;
		if (state == 1)
		{
			shared_ptr<GameObject> e = explain.lock();
			e->SetActive(true);
			shared_ptr<GameObject> t = text.lock();
			t->SetActive(false);
			shared_ptr<GameObject> ti = title.lock();
			ti->SetActive(false);
		}
		if (state == 2)
		{
			Scene_Manager::LoadScene("Game_01_Scene");
		}
	}
}