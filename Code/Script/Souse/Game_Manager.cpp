#include "Game_Manager.h"
#include "Scene_Manager.h"

int Game_Manager::Stage_Number;

void Game_Manager::Start()
{
	player = GameObject::FindWithTag("Player");
	shared_ptr<GameObject> cam = GameObject::Find("Main_Camera").lock();
	camera = cam->GetComponent<Camera>();
	LockOn_able_List.emplace_back(player);
	Change_Object.resize(2);
}

void Game_Manager::Update()
{
	if (Input_Manager::pad.buttons.leftShoulder && Change_State < 2)
	{
		Targeting_Object();
		if (Input_Manager::pad_tracker.rightShoulder == GamePad::ButtonStateTracker::PRESSED)
		{
			Select_Object();
		}
	}
	if (Change_State == 2)
	{
		Locking = false;
	}
	if (Input_Manager::pad_tracker.leftShoulder == GamePad::ButtonStateTracker::RELEASED)
	{
		Locking = false;
		Change_State = 0;
	}

	if (Game_Over)
	{
		timer += Time::deltaTime;
		if (gameover_timer < timer)
		{
			Scene_Manager::LoadScene("Title_Scene");
		}
	}

	if (Enemy_Count <= 0)
	{
		Game_Clear = true;
		gameclear_timer -= Time::deltaTime;
		if (gameclear_timer < 0)
		{
			if (Stage_Number == 2)
			{
				Scene_Manager::LoadScene("Title_Scene");
			}
			if (Stage_Number == 1)
			{
				Scene_Manager::LoadScene("Game_03_Scene");
				Stage_Number++;
			}
			if (Stage_Number == 0)
			{
				Scene_Manager::LoadScene("Game_02_Scene");
				Stage_Number++;
			}
		}
	}

}

void Game_Manager::Targeting_Object()
{
	if (!Locking)
	{
		Locking = true;
		shared_ptr<GameObject> p = player.lock();
		target = p->transform;
	}

	int LockOn_Direction = 0;
	float Horizontal = Input_Manager::pad.thumbSticks.rightX;
	float Vertical = Input_Manager::pad.thumbSticks.rightY;
	if (abs(Horizontal) > 0.5f || abs(Vertical) > 0.5f)
	{
		if (abs(Horizontal) > abs(Vertical))
		{
			if (Horizontal > 0)
			{
				LockOn_Direction = 3;
			}
			else
			{
				LockOn_Direction = 4;
			}
		}
		else
		{
			if (Vertical < 0)
			{
				LockOn_Direction = 1;
			}
			else
			{
				LockOn_Direction = 2;
			}
		}
	}

	shared_ptr<Transform> trg = target.lock();
	shared_ptr<Camera> cam = camera.lock();
	target_pos = cam->WorldToViewportPoint(trg->position);
	if (LockOn_Direction != 0 && LockOn_Direction != LockOn_Direction_Old)
	{
		if (0 < LockOn_able_List.size())
		{
			float min_target_distance = FLT_MAX;
			Vector2 targetScreenPoint;

			for (list<weak_ptr<GameObject>>::iterator itr = LockOn_able_List.begin(); itr != LockOn_able_List.end(); itr++)
			{
				if (itr->expired())
				{
					itr = LockOn_able_List.erase(itr);
					continue;
				}
				shared_ptr<GameObject> obj = itr->lock();
				if (!obj->activeSelf())
				{
					continue;
				}
				targetScreenPoint = cam->WorldToViewportPoint(obj->transform->position);
				if (targetScreenPoint.x < DxSystem::GetScreenWidth() && targetScreenPoint.x > 0 && targetScreenPoint.y < DxSystem::GetScreenHeight() && targetScreenPoint.y > 0)
				{
					float target_distance = Vector2::Distance(target_pos, Vector2(targetScreenPoint.x, targetScreenPoint.y));

					if (target_distance < min_target_distance)
					{
						switch (LockOn_Direction)
						{
						case 1:
							if (targetScreenPoint.y > target_pos.y)
							{
								min_target_distance = target_distance;
								target = obj->transform;
							}
							break;
						case 2:
							if (targetScreenPoint.y < target_pos.y)
							{
								min_target_distance = target_distance;
								target = obj->transform;
							}
							break;
						case 3:
							if (targetScreenPoint.x > target_pos.x)
							{
								min_target_distance = target_distance;
								target = obj->transform;
							}
							break;
						case 4:
							if (targetScreenPoint.x < target_pos.x)
							{
								min_target_distance = target_distance;
								target = obj->transform;
							}
							break;
						}
					}
				}
			}
		}
	}
	LockOn_Direction_Old = LockOn_Direction;
}

void Game_Manager::Select_Object()
{
	Change_Object[Change_State] = target;
	shared_ptr<Transform> trg = target.lock();
	shared_ptr<Camera> cam = camera.lock();
	Select_pos = cam->WorldToViewportPoint(trg->position);
	Change_State++;
	if (Change_State == 2)
	{
		shared_ptr<Transform> t0 = Change_Object[0].lock();
		shared_ptr<Transform> t1 = Change_Object[1].lock();
		Vector3 pos_01 = t0->position;
		Vector3 pos_02 = t1->position;

		t0->position = pos_02;
		t1->position = pos_01;
	}
}