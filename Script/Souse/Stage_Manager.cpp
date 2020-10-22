#include "Stage_Manager.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Scroll.h"
#include "SkinMesh_Renderer.h"
#include "Collider.h"
#include "Player.h"
#include "Light.h"
using namespace std;

void Stage_Manager::Start()
{
	shared_ptr<GameObject> p = GameObject::Find("Player").lock();
	player = p->GetComponent<Player>();
	camera = GameObject::Find("Main_Camera");
	game_camera_pos = { 5.0f,15.15f,-74.05f };
	start_camera_pos = camera.lock()->transform->Get_position();
	Vector3 rot = { 12.55f, -22.2f, 0.0f };
	game_camera_rot = Quaternion::Euler(rot);
	start_camera_rot = camera.lock()->transform->Get_rotation();
	Scroll_Speed_Set = Scroll_Speed;

	GameObject::Find("Directional_Light").lock()->GetComponent<Light>()->Light_View_Size = 230.0f;

	//UI
	{
		UI_Black = GameObject::Find("UI_Black").lock()->GetComponent<Sprite_Renderer>();
		UI_MainWindow = GameObject::Find("UI_MainWindow").lock()->GetComponent<Sprite_Renderer>();
		UI_Boost = GameObject::Find("UI_Boost").lock()->GetComponent<Sprite_Renderer>();
		UI_Boost_OK = GameObject::Find("UI_Boost_OK").lock()->GetComponent<Sprite_Renderer>();
		UI_Gas = GameObject::Find("UI_Gas").lock()->GetComponent<Sprite_Renderer>();
		UI_Title = GameObject::Find("UI_Title").lock()->GetComponent<Sprite_Renderer>();
		UI_Start = GameObject::Find("UI_Start").lock()->GetComponent<Sprite_Renderer>();
		UI_Over = GameObject::Find("UI_Over").lock()->GetComponent<Sprite_Renderer>();
		UI_Tutorial01 = GameObject::Find("UI_Tutorial01").lock()->GetComponent<Sprite_Renderer>();
		UI_Tutorial02 = GameObject::Find("UI_Tutorial02").lock()->GetComponent<Sprite_Renderer>();

		UI_MainWindow.lock()->gameObject->SetActive(false);
		UI_Boost.lock()->gameObject->SetActive(false);
		UI_Boost_OK.lock()->gameObject->SetActive(false);
		UI_Gas.lock()->gameObject->SetActive(false);
		UI_Title.lock()->gameObject->SetActive(true);
		UI_Start.lock()->gameObject->SetActive(true);
		UI_Over.lock()->gameObject->SetActive(false);
		UI_Tutorial01.lock()->gameObject->SetActive(false);
		UI_Tutorial02.lock()->gameObject->SetActive(false);

		UI_Boost_Timer = GameObject::Find("UI_Boost_Timer").lock()->GetComponent<Sprite_Renderer>();
		UI_Score_0001 = GameObject::Find("UI_Score_0001").lock()->GetComponent<Sprite_Renderer>();
		UI_Score_0010 = GameObject::Find("UI_Score_0010").lock()->GetComponent<Sprite_Renderer>();
		UI_Score_0100 = GameObject::Find("UI_Score_0100").lock()->GetComponent<Sprite_Renderer>();
		UI_Score_1000 = GameObject::Find("UI_Score_1000").lock()->GetComponent<Sprite_Renderer>();

		UI_Boost_Timer.lock()->gameObject->SetActive(false);
		UI_Score_0001.lock()->gameObject->SetActive(false);
		UI_Score_0010.lock()->gameObject->SetActive(false);
		UI_Score_0100.lock()->gameObject->SetActive(false);
		UI_Score_1000.lock()->gameObject->SetActive(false);
	}

	//ビル
	{
		for (int i = 0; i < 2; ++i)
		{
			shared_ptr<GameObject> building = GameObject::Instantiate(u8"Building_" + to_string(i));
			building->transform->Set_position(-70, -10, 325 + 825 * i);
			building->transform->Set_scale(0.1f, 0.1f, 0.1f);
			building->transform->Set_eulerAngles(0.0f, 178.0f, 0.0f);
			building->AddComponent<Scroll>();
			shared_ptr<SkinMesh_Renderer>skin = building->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\building\\", "building"));
			//building->SetActive(false);
			Building[i] = building;
		}
	}

	//橋
	{
		bridge_normal_count = 5;
		shared_ptr<GameObject> bridge;
		for (int i = 0; i < bridge_normal_count; ++i)
		{
			bridge = GameObject::Instantiate(u8"Bridge_Normal_" + to_string(i));
			bridge->transform->Set_position(0, 10, -bridge_offset.z * 3 + bridge_offset.z * i);
			bridge->transform->Set_scale(0.12f, 0.1f, 0.1f);
			bridge->AddComponent<Scroll>();
			shared_ptr<SkinMesh_Renderer>skin = bridge->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\bridge\\BrideBoard_Straight\\", "BrideBoard_Straight"));
			//bridge->SetActive(false);
			Bridge_Normal.push_back(bridge);
		}
		/*
		{
			bridge = GameObject::Instantiate(u8"Bridge_jump");
			bridge->transform->Set_position(0, 10, 0);
			bridge->transform->Set_scale(0.12f, 0.1f, 0.1f);
			bridge->AddComponent<Scroll>();
			shared_ptr<SkinMesh_Renderer>skin = bridge->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\bridge\\BrideBoard_Straight_deterioration\\", "BrideBoard_Straight_deterioration"));
			bridge->SetActive(false);
			Bridge_Jump = bridge;
		}
		*/
	}


	//瓦礫
	{
		for (int i = 0; i < 40; ++i)
		{
			shared_ptr<GameObject> block = GameObject::Instantiate(u8"Block_" + to_string(i));
			block->transform->Set_position(0, 0, 0);
			block->transform->Set_scale(0.12f, 0.12f, 0.12f);
			block->transform->Set_eulerAngles(0, Mathf::Random_Range(0, 360), 0);

			block->AddComponent<Scroll>();
			shared_ptr<Collider> col = block->AddComponent<Collider>();
			col->Size_X = 1.2f;
			col->Size_Z = 1.2f;
			col->obj_type = Collider::Block;

			shared_ptr<SkinMesh_Renderer>skin = block->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\rubble\\", "rubble"));
			block->SetActive(false);
			Block_obj.push_back(block);
		}
	}

	//ガソリン
	{
		for (int i = 0; i < 6; ++i)
		{
			shared_ptr<GameObject> gas = GameObject::Instantiate(u8"Gas_" + to_string(i));
			gas->transform->Set_position(0, 0, 0);
			gas->transform->Set_scale(0.1f, 0.1f, 0.1f);

			gas->AddComponent<Scroll>();
			shared_ptr<Collider> col = gas->AddComponent<Collider>();
			col->Size_X = 1.0f;
			col->Size_Z = 1.0f;
			col->obj_type = Collider::Gas;

			shared_ptr<SkinMesh_Renderer>skin = gas->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\tank\\", "tank"));
			gas->SetActive(false);
			Gas_obj.push_back(gas);
		}
	}

	//ボーナス
	{
		for (int i = 0; i < 6; ++i)
		{
			shared_ptr<GameObject> bonus = GameObject::Instantiate(u8"Bonus_" + to_string(i));
			bonus->transform->Set_position(0, 0, 0);
			bonus->transform->Set_eulerAngles(0, 180.0f, 0);
			bonus->transform->Set_scale(0.05f, 0.05f, 0.05f);

			bonus->AddComponent<Scroll>();
			shared_ptr<Collider> col = bonus->AddComponent<Collider>();
			col->Size_X = 1.0f;
			col->Size_Z = 1.0f;
			col->obj_type = Collider::Bonus;

			shared_ptr<SkinMesh_Renderer>skin = bonus->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\box\\", "box"));
			bonus->SetActive(false);
			Bonus_obj.push_back(bonus);
		}
	}

	//床
	{
		for (int i = 0; i < 15; ++i)
		{
			shared_ptr<GameObject> floor = GameObject::Instantiate(u8"Floor_" + to_string(i));
			floor->transform->Set_position(0, 0, 0);
			floor->transform->Set_scale(0.5f, 0.5f, 0.5f);

			floor->AddComponent<Scroll>();
			shared_ptr<SkinMesh_Renderer>skin = floor->AddComponent<SkinMesh_Renderer>();
			/*
			if (i > 9)
			{
				skin->Set_Mesh(Mesh::Load_Mesh("Model\\ground\\ground03\\", "ground03"));
			}
			else if (i > 4)
			{
				skin->Set_Mesh(Mesh::Load_Mesh("Model\\ground\\ground02\\", "ground02"));
			}
			else
			{
				skin->Set_Mesh(Mesh::Load_Mesh("Model\\ground\\ground01\\", "ground01"));
			}
			*/
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\ground\\ground02\\", "ground02"));
			//floor->SetActive(false);
			Floor_obj.push_back(floor);
		}
		random_device seed_gen;
		mt19937 engine(seed_gen());
		shuffle(Floor_obj.begin(), Floor_obj.end(), engine);

		int n = 0;
		for (int i = 0; i < 5; ++i)
		{
			for (int t = 0; t < 3; ++t)
			{
				Floor_obj[n].lock()->transform->Set_position(t * -73.0f, -10.0f, (i - 1) * 82.5f - 30.0f);
				++n;
			}
		}
	}


	//マップ生成用パターン生成
	{
		Block_Pattern.push_back({ 1, 0, 0 });
		Block_Pattern.push_back({ 0, 1, 0 });
		Block_Pattern.push_back({ 0, 0, 1 });
		Block_Pattern.push_back({ 1, 1, 0 });
		Block_Pattern.push_back({ 0, 1, 1 });
		Block_Pattern.push_back({ 1, 0, 1 });

		Gas_Pattern.push_back({ 2, 1, 1 });
		Gas_Pattern.push_back({ 1, 2, 1 });
		Gas_Pattern.push_back({ 1, 1, 2 });
		Gas_Pattern.push_back({ 0, 2, 1 });
		Gas_Pattern.push_back({ 1, 2, 0 });
		Gas_Pattern.push_back({ 2, 0, 1 });
		Gas_Pattern.push_back({ 1, 0, 2 });

		Bonus_Pattern.push_back({ 3, 1, 1 });
		Bonus_Pattern.push_back({ 1, 3, 1 });
		Bonus_Pattern.push_back({ 1, 1, 3 });
		Bonus_Pattern.push_back({ 0, 1, 3 });
		Bonus_Pattern.push_back({ 0, 3, 1 });
		Bonus_Pattern.push_back({ 1, 3, 0 });
		Bonus_Pattern.push_back({ 0, 3, 1 });
	}
}

void Stage_Manager::Update()
{
	//スクロール
	shared_ptr<Player> p = player.lock();
	p->Stage_State = Stage_State;
	if (Stage_State != 2 && Stage_State != 3)
	{
		if (p->Boosting)
		{
			Scroll_Speed = Scroll_Speed_Set * p->Boost_Magnification * p->Speed_Bonus_Magnification;
		}
		else
		{
			Scroll_Speed = Scroll_Speed_Set * p->Speed_Bonus_Magnification;
		}
	}
	else
	{
		Scroll_Speed = 0;
	}

	shared_ptr<GameObject> building;
	for (int i = 0; i < 2; ++i)
	{
		building = Building[i].lock();
		if (building->transform->Get_position().z <= -500)
		{
			building->transform->Set_position(-70, -10, 1150);
		}
	}
	//床
	shared_ptr<GameObject> floor;
	for (int i = 0; i < 15; ++i)
	{
		floor = Floor_obj[i].lock();
		Vector3 v = floor->transform->Get_position();
		if (v.z <= -85.0f)
		{
			floor->transform->Set_position(v.x, -10, 300.0f);
		}
	}

	//橋_通常
	shared_ptr<GameObject> bridge;
	for (int i = 0; i < bridge_normal_count; ++i)
	{
		bridge = Bridge_Normal[i].lock();
		if (bridge->transform->Get_position().z <= -bridge_offset.z * 3)
		{
			shared_ptr<GameObject> bridge_back;
			if (i == 0)
			{
				bridge_back = Bridge_Normal.back().lock();
			}
			else
			{
				bridge_back = Bridge_Normal[i - 1].lock();
			}
			Vector3 instance_pos = bridge_back->transform->Get_position() + bridge_offset;
			bridge->transform->Set_position(instance_pos);
			if (Stage_State != 0)
			{
				Create_Objects(instance_pos);
			}
		}
	}

	if (Stage_State == 0)
	{
		if (UI_Black_Old)
		{
			shared_ptr<Sprite_Renderer> s = UI_Black.lock();
			s->Color.w -= Time::deltaTime;
			if (s->Color.w <= 0)
			{
				s->Color.w = 0;
				s->gameObject->SetActive(false);
				UI_Black_Old = false;

				Audio_Manager::BGM_title->Play(true);
			}
		}

		UI_flash_Timer += Time::deltaTime * 2;
		if ((int)(UI_flash_Timer) % 2 == 1)
		{
			if (UI_Flush_Old)
			{
				UI_Start.lock()->enabled = false;
				UI_Flush_Old = false;
			}
		}
		else
		{
			if (!UI_Flush_Old)
			{
				UI_Start.lock()->enabled = true;
				UI_Flush_Old = true;
			}
		}
		if ((Input_Manager::key_tracker.IsKeyPressed(Keyboard::Space) || Input_Manager::pad_tracker.a == GamePad::ButtonStateTracker::PRESSED))
		{
			UI_Title.lock()->gameObject->SetActive(false);
			UI_Start.lock()->gameObject->SetActive(false);

			Audio_Manager::BGM_title->Stop();
			Audio_Manager::Start->Play();
			++Stage_State;
		}
	}
	else if (Stage_State == 1)
	{
		camera_timer += Time::deltaTime / 3;
		shared_ptr<GameObject> cam = camera.lock();
		cam->transform->Set_position(Vector3::Lerp(start_camera_pos, game_camera_pos, Mathf::Clamp(camera_timer, 0.0f, 1.0f)));
		cam->transform->Set_rotation(Quaternion::Slerp(start_camera_rot, game_camera_rot, Mathf::Clamp(camera_timer, 0.0f, 1.0f)));
		Scroll_Speed = Scroll_Speed_Set * p->Speed_Bonus_Magnification * 2.5f;
		if (camera_timer >= 1.5f)
		{
			UI_Tutorial01.lock()->gameObject->SetActive(true);
			Audio_Manager::BGM_play->Play(true);
			++Stage_State;
		}

	}
	else if (Stage_State == 2)
	{
		if ((Input_Manager::key_tracker.IsKeyPressed(Keyboard::Space) || Input_Manager::pad_tracker.a == GamePad::ButtonStateTracker::PRESSED))
		{
			UI_Tutorial01.lock()->gameObject->SetActive(false);
			UI_Tutorial02.lock()->gameObject->SetActive(true);
			++Stage_State;
		}
	}
	else if (Stage_State == 3)
	{
		if ((Input_Manager::key_tracker.IsKeyPressed(Keyboard::Space) || Input_Manager::pad_tracker.a == GamePad::ButtonStateTracker::PRESSED))
		{
			UI_Tutorial02.lock()->gameObject->SetActive(false);
			UI_MainWindow.lock()->gameObject->SetActive(true);
			UI_Boost.lock()->gameObject->SetActive(true);
			UI_Gas.lock()->gameObject->SetActive(true);
			UI_Boost_Timer.lock()->gameObject->SetActive(true);
			UI_Score_0001.lock()->gameObject->SetActive(true);
			UI_Score_0010.lock()->gameObject->SetActive(true);
			UI_Score_0100.lock()->gameObject->SetActive(true);
			UI_Score_1000.lock()->gameObject->SetActive(true);
			++Stage_State;
		}
	}
	else if (Stage_State == 4)
	{
		shared_ptr<Sprite_Renderer> gas = UI_Gas.lock();
		int per = 8 - (p->Gas / (p->Gas_Max / 8));
		if (p->Gas > 0)
		{
			gas->UV_Origin = { (float)per * 500 + 20,gas->UV_Origin.y };
		}
		else
		{
			if (gas->gameObject->activeSelf())
			{
				gas->gameObject->SetActive(false);
				UI_Over.lock()->gameObject->SetActive(true);
				++Stage_State;
			}
		}

		if (p->Can_Boost)
		{
			if (!Boost_Old)
			{
				Boost_Old = true;
				UI_Boost.lock()->gameObject->SetActive(false);
				UI_Boost_OK.lock()->gameObject->SetActive(true);
				UI_Boost_Timer.lock()->gameObject->SetActive(false);
			}
		}
		else
		{
			if (Boost_Old && !p->Boosting)
			{
				Boost_Old = false;
				UI_Boost.lock()->gameObject->SetActive(true);
				UI_Boost_OK.lock()->gameObject->SetActive(false);
				UI_Boost_Timer.lock()->gameObject->SetActive(true);
			}
			Number_Sprite(UI_Boost_Timer, 10 - (int)p->Boost_Charge_Timer);
		}

		if (p->Get_Bonus)
		{
			Score += 50;
			p->Get_Bonus = false;
		}

		Score += (Scroll_Speed * Time::deltaTime * 0.25f);

		if (Score < 10000)
		{
			int s = (int)Score;
			int a[4];
			a[0] = (s % 10); s /= 10;
			a[1] = (s % 10); s /= 10;
			a[2] = (s % 10); s /= 10;
			a[3] = (s % 10); s /= 10;
			Number_Sprite(UI_Score_0001, a[0]);
			Number_Sprite(UI_Score_0010, a[1]);
			Number_Sprite(UI_Score_0100, a[2]);
			Number_Sprite(UI_Score_1000, a[3]);
		}
	}
	else if (Stage_State == 5)
	{
		Scroll_Speed = 0;
#if _DEBUG
#else
		if ((Input_Manager::key_tracker.IsKeyPressed(Keyboard::Space) || Input_Manager::pad_tracker.a == GamePad::ButtonStateTracker::PRESSED))
		{
			UI_Black_Old = true;
			shared_ptr<Sprite_Renderer> s = UI_Black.lock();
			s->gameObject->SetActive(true);
		}

		if (UI_Black_Old)
		{
			shared_ptr<Sprite_Renderer> s = UI_Black.lock();
			s->Color.w += Time::deltaTime;
			if (s->Color.w >= 1.0f)
			{
				s->Color.w = 1.0f;

				Audio_Manager::BGM_play->Stop();
				Scene_Manager::LoadScene("Scene\\Main_Scene.bin");
			}
		}

#endif
	}
}

void Stage_Manager::Create_Objects(Vector3 Instance_Pos)
{
	Instance_Pattern.clear();
	for (int i = 0; i < 2; ++i)
	{
		Instance_Pattern.push_back(Block_Pattern[(int)Mathf::Random_Range(0, (float)Block_Pattern.size())]);
	}
	if (Mathf::Probability(50.0f))
	{
		if (Mathf::Probability(80.0f))
		{
			Instance_Pattern.push_back(Gas_Pattern[(int)Mathf::Random_Range(0, (float)Gas_Pattern.size())]);
		}
		else
		{
			Instance_Pattern.push_back(Bonus_Pattern[(int)Mathf::Random_Range(0, (float)Bonus_Pattern.size())]);
		}
	}
	else
	{
		Instance_Pattern.push_back(Block_Pattern[(int)Mathf::Random_Range(0, (float)Block_Pattern.size())]);
	}

	random_device seed_gen;
	mt19937 engine(seed_gen());
	shuffle(Instance_Pattern.begin(), Instance_Pattern.end(), engine);

	for (u_int i = 0; i < Instance_Pattern.size(); ++i)
	{
		float inst[3] = { Instance_Pattern[i][0] ,Instance_Pattern[i][1] ,Instance_Pattern[i][2] };
		for (int t = 0; t < 3; ++t)
		{
			shared_ptr<GameObject> obj;
			Vector3 p;
			if (inst[t] == Collider::None)
			{
				continue;
			}
			else if (inst[t] == Collider::Block)
			{
				obj = Instance_Object(Block_obj);
				p = { -2.0f + t * 2 ,11.0f ,(Instance_Pos.z - (bridge_offset.z / 2.0f)) + (bridge_offset.z / 3.0f) * i };
			}
			else if (inst[t] == Collider::Gas)
			{
				obj = Instance_Object(Gas_obj);
				p = { -2.0f + t * 2 ,11.5f ,(Instance_Pos.z - (bridge_offset.z / 2.0f)) + (bridge_offset.z / 3.0f) * i };
			}
			else if (inst[t] == Collider::Bonus)
			{
				obj = Instance_Object(Bonus_obj);
				p = { -2.0f + t * 2 ,12.0f ,(Instance_Pos.z - (bridge_offset.z / 2.0f)) + (bridge_offset.z / 3.0f) * i };
			}
			obj->SetActive(true);
			obj->transform->Set_position(p);
		}
	}
}

shared_ptr<GameObject> Stage_Manager::Instance_Object(std::vector<std::weak_ptr<GameObject>>& objects)
{
	for (size_t i = 0; i < objects.size(); ++i)
	{
		shared_ptr<GameObject> obj = objects[i].lock();
		if (!obj->activeSelf())
		{
			return obj;
		}
	}
	return nullptr;
}

void Stage_Manager::Number_Sprite(std::weak_ptr<Sprite_Renderer> renderer, int number)
{
	renderer.lock()->UV_Origin = { 134.0f * (number + 1),256.0f };
}

bool Stage_Manager::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Stage_Manager"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("Stage_Manager_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<Stage_Manager>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
			if (!removed)
			{
				return false;
			}
		}

		ImGui::DragFloat(u8"Scroll_Speed", &Scroll_Speed, 0.1f, 0.01f, FLT_MAX);
	}
	return true;
}