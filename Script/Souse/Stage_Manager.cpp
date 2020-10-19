#include "Stage_Manager.h"
#include "Include_Mono.h"
#include "Include_ImGui.h"
#include "Scroll.h"
#include "SkinMesh_Renderer.h"
#include "Collider.h"
#include "Player.h"
using namespace std;

void Stage_Manager::Start()
{
	shared_ptr<GameObject> p = GameObject::Find("Player").lock();
	player = p->GetComponent<Player>();
	Scroll_Speed_Set = Scroll_Speed;

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

	//橋_通常
	{
		bridge_normal_count = 5;

		for (int i = 0; i < bridge_normal_count; ++i)
		{
			shared_ptr<GameObject> bridge = GameObject::Instantiate(u8"Bridge_Normal_" + to_string(i));
			bridge->transform->Set_position(0, 10, -bridge_offset.z * 3 + bridge_offset.z * i);
			bridge->transform->Set_scale(0.12f, 0.1f, 0.1f);
			bridge->AddComponent<Scroll>();
			shared_ptr<SkinMesh_Renderer>skin = bridge->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\bridge\\BrideBoard_Straight\\", "BrideBoard_Straight"));
			//bridge->SetActive(false);
			Bridge_Normal.push_back(bridge);
		}
	}

	//瓦礫
	{
		block_obj_count = 60;
		for (int i = 0; i < block_obj_count; ++i)
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
		gas_obj_count = 7;
		for (int i = 0; i < gas_obj_count; ++i)
		{
			shared_ptr<GameObject> gas = GameObject::Instantiate(u8"Gas_" + to_string(i));
			gas->transform->Set_position(0, 0, 0);
			gas->transform->Set_scale(0.01f, 0.01f, 0.01f);

			gas->AddComponent<Scroll>();
			shared_ptr<Collider> col = gas->AddComponent<Collider>();
			col->Size_X = 1.0f;
			col->Size_Z = 1.0f;
			col->obj_type = Collider::Gas;

			shared_ptr<SkinMesh_Renderer>skin = gas->AddComponent<SkinMesh_Renderer>();
			skin->Set_Mesh(Mesh::Load_Mesh("Model\\danbo_fbx\\danbo_atk\\", "danbo_atk"));
			gas->SetActive(false);
			Gas_obj.push_back(gas);
		}
	}

	//ボーナス
	{
		bonus_obj_count = 7;
		for (int i = 0; i < bonus_obj_count; ++i)
		{
			shared_ptr<GameObject> bonus = GameObject::Instantiate(u8"Bonus_" + to_string(i));
			bonus->transform->Set_position(0, 0, 0);
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
	for (int i = 0; i < 2; ++i)
	{
		shared_ptr<GameObject> building = Building[i].lock();
		if (building->transform->Get_position().z <= -500)
		{
			building->transform->Set_position(-70, -10, 1150);
		}
	}

	//橋_通常
	for (int i = 0; i < bridge_normal_count; ++i)
	{
		shared_ptr<GameObject> bridge = Bridge_Normal[i].lock();
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
			Create_Objects(instance_pos);
		}
	}

	//スクロール
	shared_ptr<Player> p = player.lock();
	if (p->Boosting)
	{
		Scroll_Speed = Scroll_Speed_Set * p->Boost_Magnification * p->Speed_Bonus_Magnification;
	}
	else
	{
		Scroll_Speed = Scroll_Speed_Set * p->Speed_Bonus_Magnification;
	}
}

void Stage_Manager::Create_Objects(Vector3 Instance_Pos)
{
	Instance_Pattern.clear();
	for (int i = 0; i < 4; ++i)
	{
		Instance_Pattern.push_back(Block_Pattern[(int)Mathf::Random_Range(0, (float)Block_Pattern.size())]);
	}
	if ((int)Mathf::Random_Range(0, 2))
	{
		Instance_Pattern.push_back(Gas_Pattern[(int)Mathf::Random_Range(0, (float)Gas_Pattern.size())]);
	}
	else
	{
		Instance_Pattern.push_back(Bonus_Pattern[(int)Mathf::Random_Range(0, (float)Bonus_Pattern.size())]);
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
			if (inst[t] == Collider::None)
			{
				continue;
			}
			else if (inst[t] == Collider::Block)
			{
				obj = Instance_Object(Block_obj);
			}
			else if (inst[t] == Collider::Gas)
			{
				obj = Instance_Object(Gas_obj);
			}
			else if (inst[t] == Collider::Bonus)
			{
				obj = Instance_Object(Bonus_obj);
			}
			obj->SetActive(true);
			Vector3 p = { -2.0f + t * 2 ,11.0f ,(Instance_Pos.z - (bridge_offset.z / 2.0f)) + (bridge_offset.z / 5.0f) * i };
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